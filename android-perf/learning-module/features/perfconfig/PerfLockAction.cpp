/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PerfLockAction.h"

#include "Action.h"
#include "TLog.h"
#include "client.h"

PerfLockAction &PerfLockAction::getInstance() {
    static PerfLockAction instance;
    return instance;
}

PerfLockAction::PerfLockAction() : mActionCount(0) {
    TLOGD("%s: Constructor", LOG_TAG_PERFLOCK_ACTION);
}

PerfLockAction::~PerfLockAction() {
    TLOGD("%s: Destructor", LOG_TAG_PERFLOCK_ACTION);

    releaseAllPerfLocks();
}

void *PerfLockAction::getActionOutputDb() {
    return nullptr;
}

void PerfLockAction::applyAction(const AsyncTriggerPayload &payload) {
    std::lock_guard<std::mutex> lock(mMutex);

    if (payload.appName.empty()) {
        TLOGW("%s: Empty app name in payload", LOG_TAG_PERFLOCK_ACTION);
        return;
    }

    mActionCount++;

    TLOGD("%s: Applying action for package: %s, hintID: 0x%x, duration: %d",
          LOG_TAG_PERFLOCK_ACTION, payload.appName.c_str(), payload.hintID, payload.duration);

    auto it = mActivePerfLocks.find(payload.appName);
    if (it != mActivePerfLocks.end()) {
        TLOGV("%s: Package %s already has active perflock (handle: %d)", LOG_TAG_PERFLOCK_ACTION,
              payload.appName.c_str(), it->second);
        return;
    }

    logActionApplied(payload.appName, -1);
}

void PerfLockAction::releasePerfLock(const std::string &packageName) {
    std::lock_guard<std::mutex> lock(mMutex);

    auto it = mActivePerfLocks.find(packageName);
    if (it != mActivePerfLocks.end()) {
        int handle = it->second;
        int result = perf_lock_rel(handle);

        TLOGD("%s: Released perflock for %s, handle: %d, result: %d", LOG_TAG_PERFLOCK_ACTION,
              packageName.c_str(), handle, result);

        mActivePerfLocks.erase(it);
    }
}

void PerfLockAction::releaseAllPerfLocks() {
    std::lock_guard<std::mutex> lock(mMutex);

    for (auto &pair : mActivePerfLocks) {
        int result = perf_lock_rel(pair.second);
        TLOGD("%s: Released perflock for %s, handle: %d, result: %d", LOG_TAG_PERFLOCK_ACTION,
              pair.first.c_str(), pair.second, result);
    }

    mActivePerfLocks.clear();
}

size_t PerfLockAction::getActivePerfLockCount() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mActivePerfLocks.size();
}

void PerfLockAction::logActionApplied(const std::string &packageName, int handle) {
    TLOGI("%s: Action applied - package: %s, handle: %d, total actions: %u",
          LOG_TAG_PERFLOCK_ACTION, packageName.c_str(), handle, mActionCount);
}
