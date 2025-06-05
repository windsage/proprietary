/******************************************************************************
  @file    Request.h
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __REQUEST__H_
#define __REQUEST__H_
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "MpctlUtils.h"
#include <config.h>

class ResourceInfo;

enum {
  LOCK_UNKNOWN_VERSION = -1,
  LOCK_OLD_VERSION = 0,
  LOCK_NEW_VERSION,
};

enum client_t {
    REGULAR_CLIENT,
};

enum request_prio_t {
    REGULAR_PRIORITY,
    HIGH_PRIORITY,
    ALWAYS_ALLOW
};

class Request {
    private:
        int mVersion;
        timer_t mTimer;
        std::mutex mTimerLock;
        std::mutex mTimerCreatedLock;
        std::condition_variable mCondVar;
        std::atomic<bool> mTimerCreated;
        pid_t mPid;
        pid_t mTid;
        enum client_t mClient;
        uint32_t mDuration;
        uint32_t mNumArgs;
        ResourceInfo *mResources;
        request_prio_t mPriority;
        int8_t mCluster;

    public:
        //request processing which includes
        //1.parsing
        //2.validating
        //3.physical translation
        //4.backward compatibility
        bool Process(uint32_t nargs, int32_t list[]);

    public:
        //backward compatibility
        uint32_t TranslateOpcodeToNew(uint32_t opcde);
        uint32_t TranslateValueToNew(uint16_t major, uint16_t minor, uint32_t opcode);
        bool TranslateUpDownMigrate(int8_t upmigrate_idx[], int8_t downmigrate_idx[]);
        bool TranslateGrpUpDownMigrate(int8_t upmigrate_idx, int8_t downmigrate_idx);

    public:
        Request(uint32_t duration, pid_t client_pid, pid_t client_tid, enum client_t client);
        ~Request();
        Request(Request const& req);
        Request& operator=(Request const& req);
        bool operator==(Request const& req);
        bool operator!=(Request const& req);

        uint32_t GetNumLocks();
        ResourceInfo* GetResource(uint32_t idx);

        timer_t GetTimer() {
            return mTimer;
        }

        int GetDuration() {
            return mDuration;
        }

        uint32_t GetPid() {
            return mPid;
        }

        uint32_t GetTid() {
            return mTid;
        }

        enum client_t GetClient() {
            return mClient;
        }

        enum request_prio_t GetPriority() {
            return mPriority;
        }

        uint32_t OverrideClientValue(uint16_t major, uint16_t minor, uint32_t value);
        bool IsResAlwaysResetOnRelease(uint16_t major, uint16_t minor);

        static void RequestTimerCallback(sigval_t pvData);
        int32_t CreateTimer(int32_t req_handle);
        int32_t SetTimer();
        int32_t DeleteTimer();
};

#endif
