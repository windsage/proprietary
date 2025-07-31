/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PERFLOCKACTION_H
#define PERFLOCKACTION_H

#include <mutex>
#include <unordered_map>

#include "Action.h"

#define LOG_TAG_PERFLOCK_ACTION "PERF-CONFIG-ACTION"

class PerfLockAction : public Action {
public:
    /**
     * Get singleton instance of PerfLockAction
     * @return Reference to singleton instance
     */
    static PerfLockAction &getInstance();

    /**
     * Override Action method - Get action output database (not used)
     * @return Pointer to output database (returns nullptr for this
     * implementation)
     */
    void *getActionOutputDb() override;

    /**
     * Override Action method - Apply action based on trigger payload
     * @param payload Async trigger payload containing action parameters
     */
    void applyAction(const AsyncTriggerPayload &payload) override;

    /**
     * Release PerfLock for specific package
     * @param packageName Package name whose PerfLock should be released
     */
    void releasePerfLock(const std::string &packageName);

    /**
     * Release all active PerfLocks managed by this action
     */
    void releaseAllPerfLocks();

    /**
     * Get count of currently active PerfLocks
     * @return Number of active PerfLock handles
     */
    size_t getActivePerfLockCount() const;

private:
    /**
     * Private constructor for singleton pattern
     */
    PerfLockAction();

    /**
     * Private destructor - releases all active PerfLocks
     */
    ~PerfLockAction();

    // Prevent copy/assignment for singleton
    PerfLockAction(const PerfLockAction &) = delete;
    PerfLockAction &operator=(const PerfLockAction &) = delete;

    // Internal state management
    mutable std::mutex mMutex;    // Thread safety mutex
    std::unordered_map<std::string, int>
        mActivePerfLocks;     // Package name → PerfLock handle mapping
    uint32_t mActionCount;    // Total number of actions applied

    /**
     * Log action application details for debugging
     * @param packageName Package name for which action was applied
     * @param handle PerfLock handle that was acquired
     */
    void logActionApplied(const std::string &packageName, int handle);
};

#endif /* PERFLOCKACTION_H */
