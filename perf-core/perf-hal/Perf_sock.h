/******************************************************************************
  @file    Perf_sock.h
  @brief   LE performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_PERF_V2_3_PERF_H
#define VENDOR_QTI_HARDWARE_PERF_V2_3_PERF_H


#include "PerfGlueLayer.h"
#include "PerfConfig.h"
#include <sys/socket.h>
#include <linux/un.h>
#include <config.h>

#define FAILED -1
#define SUCCESS 0

class Perf {
    private:
        PerfGlueLayer mImpl;
        PerfConfigDataStore &mPerfDataStore = PerfConfigDataStore::getPerfDataStore();
        TargetConfig &mTargetConfig = TargetConfig::getTargetConfig();
        bool mPerfEnabled;

        int32_t mSocketHandler;
        int32_t mListnerHandler;
        bool mSignalFlag;
        pthread_t mPerfThread;
        struct sockaddr_un mAddr;

        void Init();
        bool checkPerfStatus(const char*);
        //socket methods
        void startListener();
        static void *socketMain(void *);
        int32_t recvMsg(mpctl_msg_t &msg);
        int32_t sendMsg(void *msg, int32_t type = PERF_LOCK_TYPE);
        int32_t connectToSocket();
        void closeSocket(int32_t socketHandler);
        void setSocketHandler(int32_t socketHandler);
        int32_t getSocketHandler();
        void callAPI(mpctl_msg_t &msg);

        //API's
        int32_t perfLockAcquire(mpctl_msg_t &pMsg);
        void perfLockRelease(mpctl_msg_t &pMsg);
        int32_t perfHint(mpctl_msg_t &pMsg);
        void perfLockCmd(mpctl_msg_t &pMsg);
        void perfGetProp(mpctl_msg_t &pMsg);
        int32_t perfSetProp(mpctl_msg_t &pMsg);
        void perfSyncRequest(mpctl_msg_t &pMsg);
        int32_t perfLockAcqAndRelease(mpctl_msg_t &pMsg);
        void perfEvent(mpctl_msg_t &pMsg);
        int32_t perfGetFeedback(mpctl_msg_t &pMsg);
        int32_t perfHintAcqRel(mpctl_msg_t &pMsg);
        int32_t perfHintRenew(mpctl_msg_t &pMsg);
    public:
        Perf();
        static Perf &getPerfInstance();
        ~Perf();

    public:
        void joinThread();
        int32_t startService();
        void setSignal(bool value) {
            mSignalFlag = value;
        }
};

#endif  // VENDOR_QTI_HARDWARE_PERF_V2_3_PERF_H
