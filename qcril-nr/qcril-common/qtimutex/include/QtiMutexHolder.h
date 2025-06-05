/* ============================================================
 *
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * ============================================================
 */

#pragma once

#ifdef __cplusplus
#include <QtiMutex.h>
#include <iostream>
#include <array>
#include <vector>
#include <atomic>
#include <unordered_map>
#include <unistd.h>
#include <sys/syscall.h>

#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))

namespace qtimutex {
    struct QtiSharedMutex;
    struct QtiRecursiveMutex;
    using StackType = std::array<uintptr_t, 10>;
    struct HolderInfo {
        pthread_t thread;
        StackType stack;
    };

    void dump_stack(std::ostream& os, StackType& stack);

    class QtiSharedMutexHolder {
    public:
        void dump(std::ostream& os);
        QtiSharedMutexHolder(QtiSharedMutex* owner) :
                mLockHold(false), mOwner(owner) {}
        // holder does not own any pointer or thread structure
        ~QtiSharedMutexHolder() = default;
        void retrieveLockBacktrace();
        void retrieveSharedLockBacktrace();
        void releaseLockBacktrace();
        void releaseSharedLockBacktrace();
    private:
        pid_t mTid; // thread ID for exclusive lock holder
        StackType mStack{}; // for stack trace of exclusive lock holder
        std::atomic<bool> mLockHold;
        std::mutex mSharedHolderMutex;
        std::unordered_map<pid_t, StackType> mSharedLockHolder;
        [[maybe_unused]] QtiSharedMutex* mOwner;
    };

    class QtiRecursiveMutexHolder {
    public:
        void dump(std::ostream& os);
        QtiRecursiveMutexHolder(QtiRecursiveMutex* owner) : mOwner(owner) {}
        // holder does not own any pointer or thread structure
        ~QtiRecursiveMutexHolder() = default;
        void retrieveLockBacktrace();
        void releaseLockBacktrace();
    private:
        pid_t mTid;
        [[maybe_unused]] QtiRecursiveMutex* mOwner;
        std::vector<StackType> mStack;
    };
}

#endif
