/* ============================================================
 *
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */
#include <QtiMutexHolder.h>
#include <dlfcn.h>
#include <iomanip>
#include <sstream>

using namespace qtimutex;

void get_backtrace(uintptr_t* bt, size_t sz);

namespace qtimutex {

void dump_stack(std::ostream& os, StackType& stack) {
    int count{0};
    std::function<void(std::string)> deadlock_logger =
      QtiMutexDefaults::getInstance().getDeadlockLogger();

    for (auto item: stack) {
        if (item != 0) {
            Dl_info info;
#if defined(__aarch64__)
#define AARCH64_LINUX_VA_BITS 39
            // strip the TAG and PAC bits
            auto addr = item & (((uintptr_t)1 << AARCH64_LINUX_VA_BITS) - 1);
#else
            auto addr = item;
#endif
            std::stringstream ss;
            ss << "#" << std::setw(2) << std::setfill('0') << count++;
            ss << std::hex << ": 0x" << addr << " | ";
            if (dladdr(reinterpret_cast<void*>(addr), &info)) {
                if (info.dli_sname) {
                  ss << info.dli_sname << " | ";
                } else {
                  ss << "no symbol matching exact address" << " | ";
                }
                if (info.dli_fname) {
                  ss << info.dli_fname << " | ";
                } else {
                  ss << "pathname of .so not found" << " | ";
                }
                if (info.dli_fbase) {
                  ss << info.dli_fbase << " (base address of .so) | ";
                  ss << std::hex << std::setw(16) << std::setfill('0')
                    << (addr - reinterpret_cast<uintptr_t>(info.dli_fbase)) << "(offset address) | ";
                } else {
                  ss << "base address of .so not found" << " | ";
                }
                if (info.dli_saddr) {
                  ss << info.dli_saddr << "(exact address of symbol)";
                }
                os << ss.rdbuf() << std::endl;
                if (deadlock_logger) {
                  deadlock_logger(ss.str());
                }
                ss.str("");
                ss.clear();
            }
        }
    }
}

}

void QtiSharedMutexHolder::dump(std::ostream& os) {
    std::string ss;
    std::function<void(std::string)> deadlock_logger =
      QtiMutexDefaults::getInstance().getDeadlockLogger();

    if (mLockHold) {
        ss = "[" + std::to_string(mTid) + "] Thread is holding the exclusive lock :";
        os << "\n" << ss << std::endl;
        if (deadlock_logger) {
          deadlock_logger(ss);
        }
        dump_stack(os, mStack);
        ss.clear();
    }
    if (mSharedLockHolder.size() != 0) {
        int count = 0;
        for (auto& item: mSharedLockHolder) {
            ss = "[" + std::to_string(item.first) + "] "
              + std::to_string(count++) + ": Thread is holding the shared_lock: ";
            os << "\n" << ss << std::endl;
            if (deadlock_logger) {
              deadlock_logger(ss);
            }
            dump_stack(os, item.second);
            ss.clear();
        }
    }
}

void QtiRecursiveMutexHolder::dump(std::ostream& os) {
    std::function<void(std::string)> deadlock_logger =
      QtiMutexDefaults::getInstance().getDeadlockLogger();
    if (mStack.size() != 0) {
        std::string ss;
        ss = "[" + std::to_string(mTid) + "] Thread is holding the recursive lock: ";
        os << "\n" << ss << std::endl;
        if (deadlock_logger) {
          deadlock_logger(ss);
        }
        ss.clear();
        size_t idx = 0;
        for (auto& item: mStack) {
            ss = "#" + std::to_string(idx++) + " acquire:";
            if (deadlock_logger) {
              deadlock_logger(ss);
            }
            os << ss << std::endl;
            dump_stack(os, item);
            ss.clear();
        }
    }
}

void QtiSharedMutexHolder::retrieveLockBacktrace() {
    if (mLockHold.load()) {
        std::function<void(std::string)> debug_logger =
        QtiMutexDefaults::getInstance().getDebugLogger();
        if (debug_logger) {
          debug_logger("QtiSharedMutexHolder::retrieveLockBacktrace "
          ":trying to relock while holding the exclusive access lock");
        }
        abort();
    }
    mLockHold = true;
    mTid = gettid();
    get_backtrace(mStack.data(), mStack.size());
}

void QtiSharedMutexHolder::retrieveSharedLockBacktrace() {
    StackType stack{};
    auto threadId = gettid();
    get_backtrace(stack.data(), stack.size());
    std::unique_lock<std::mutex> lock(mSharedHolderMutex);
    if (mSharedLockHolder.find(threadId) != mSharedLockHolder.end()) {
        std::function<void(std::string)> debug_logger =
        QtiMutexDefaults::getInstance().getDebugLogger();
        if (debug_logger) {
          debug_logger("QtiSharedMutexHolder::retrieveSharedLockBacktrace "
            ":trying to relock while holding the share lock");
        }
        lock.unlock();
        abort();
    } else {
        mSharedLockHolder.insert({threadId, stack});
        lock.unlock();
    }
}

void QtiSharedMutexHolder::releaseLockBacktrace() {
    if (!mLockHold) {
      std::function<void(std::string)> debug_logger =
      QtiMutexDefaults::getInstance().getDebugLogger();
      if (debug_logger) {
        debug_logger("QtiSharedMutexHolder::releaseLockBacktrace "
          ":trying to unlock without holding the lock");
      }
      abort();
    }
    mLockHold = false;
}

void QtiSharedMutexHolder::releaseSharedLockBacktrace() {
    auto thread = gettid();

    std::unique_lock<std::mutex> lock(mSharedHolderMutex);
    if (mSharedLockHolder.find(thread) != mSharedLockHolder.end()) {
        mSharedLockHolder.erase(thread);
    } else {
        std::function<void(std::string)> debug_logger =
        QtiMutexDefaults::getInstance().getDebugLogger();
        if (debug_logger) {
          debug_logger("QtiSharedMutexHolder::releaseSharedLockBacktrace "
            ":trying to unlock_shared without holding the lock");
        }
        lock.unlock();
        abort();
    }
}

void QtiRecursiveMutexHolder::retrieveLockBacktrace() {
    mTid = gettid();
    StackType bt{};
    get_backtrace(bt.data(), bt.size());
    mStack.push_back(bt);
}

void QtiRecursiveMutexHolder::releaseLockBacktrace() {
    if (!mStack.empty()) {
        mStack.pop_back();
    } else {
        abort();
    }
}
