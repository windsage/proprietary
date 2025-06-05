/******************************************************************************
  @file    Perf_sock.cpp
  @brief   LE performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#define LOG_TAG_HAL "PERF-HAL"
#include <string>
#include <cutils/properties.h>


#include "Perf_sock.h"
#include "PerfController.h"
#include "TargetConfig.h"
#include "PerfThreadPool.h"
#include "MpctlUtils.h"
#include "PerfLog.h"

#define PERF_HAL_DISABLE_PROP "vendor.disable.perf.hal"
#define INIT_NOT_COMPLETED (-3)

int32_t Perf::perfLockAcquire(mpctl_msg_t &pMsg) {
    int32_t retVal = -1;
    client_msg_int_t reply;
    memset(&reply, 0, sizeof(client_msg_int_t));
    reply.handle = retVal;
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply);
        return retVal;
    }
    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    reply.handle = retVal;
    sendMsg(&reply);

    return retVal;
}

void Perf::perfLockRelease(mpctl_msg_t &pMsg) {
    if (!checkPerfStatus(__func__)) {
        return;
    }
    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
}

int32_t Perf::perfHint(mpctl_msg_t &pMsg) {
    int32_t retVal = -1;
    client_msg_int_t reply;
    memset(&reply, 0, sizeof(client_msg_int_t));
    reply.handle = retVal;
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply);
        return retVal;
    }
    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    QLOGL(LOG_TAG_HAL, QLOG_L2, " Perf glueLayer submitRequest returned %" PRId32, retVal);
    reply.handle = retVal;
    sendMsg(&reply);
    return retVal;
}

void Perf::perfLockCmd(mpctl_msg_t &pMsg) {
    if (!checkPerfStatus(__func__)) {
        return;
    }
    pMsg.req_type = pMsg.data;
    pMsg.data = 0;

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
}

void Perf::perfGetProp(mpctl_msg_t &pMsg) {
    client_msg_str_t reply;
    char *retVal = NULL;
    char trace_buf[TRACE_BUF_SZ];
    memset(&reply, 0, sizeof(client_msg_str_t));

    if (!checkPerfStatus(__func__)) {
        retVal = NULL;
    } else {
        retVal = mPerfDataStore.GetProperty(pMsg.usrdata_str, reply.value, sizeof(reply.value));

        if (retVal != NULL) {
            if (perf_debug_output) {
                snprintf(trace_buf, TRACE_BUF_SZ, "perfGetProp: Return Val from %s is %s", __func__, retVal);
                QLOGE(LOG_TAG_HAL, "%s", trace_buf);
            }
        }
    }
    if (retVal == NULL) {
        strlcpy(reply.value, pMsg.propDefVal, PROP_VAL_LENGTH);
        reply.value[PROP_VAL_LENGTH - 1] = '\0';
    }
    sendMsg(&reply, PERF_GET_PROP_TYPE);
}

void Perf::perfSyncRequest(mpctl_msg_t &pMsg) {
    client_msg_sync_req_t reply;
    memset(&reply, 0, sizeof(client_msg_sync_req_t));
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply, PERF_SYNC_REQ_TYPE);
        return;
    }
    std::string requestInfo = mImpl.PerfGlueLayerSyncRequest(pMsg.hint_id);
    strlcpy(reply.value, requestInfo.c_str(), SYNC_REQ_VAL_LEN);
    sendMsg(&reply, PERF_SYNC_REQ_TYPE);
}

Perf::Perf() {
    mListnerHandler = -1;
    mSocketHandler = -1;
    mSignalFlag = 0;
    memset(&mAddr, 0, sizeof(struct sockaddr_un));
    char prop_read[PROPERTY_VALUE_MAX] = {0};
    mPerfEnabled = true;
    if (property_get(PERF_HAL_DISABLE_PROP, prop_read,"0")) {
        mPerfEnabled = !std::stoi(prop_read,NULL,0);
    }

    if (mPerfEnabled) {
        Init();
    }
    QLOGL(LOG_TAG_HAL, QLOG_L1, "Perf Hal Enabled %" PRId8, mPerfEnabled);
}
Perf &Perf::getPerfInstance() {
    static Perf mPerf;
    return mPerf;
}

Perf::~Perf() {
    mImpl.PerfGlueLayerExit();
}

bool Perf::checkPerfStatus(const char* func) {
    if (!mPerfEnabled) {
        QLOGL(LOG_TAG_HAL, QLOG_WARNING, "Perf Hal is Disabled cannot execute %s", func);
    }
    return mPerfEnabled;
}

void Perf::Init() {
    char trace_prop[PROPERTY_VALUE_MAX];

    if (PerfLogInit() < 0) {
        QLOGE(LOG_TAG, "PerfLogInit failed");
    }

    //init PerfConfigStore
    mTargetConfig.InitializeTargetConfig();
    QLOGL(LOG_TAG_HAL, QLOG_L1, "TargetConfig Init Complete");
    //init PerfConfigStore
    mPerfDataStore.ConfigStoreInit();

    //register mp-ctl
    mImpl.LoadPerfLib("libqti-perfd.so");

    /* Enable systraces by adding vendor.debug.trace.perf=1 into /build.prop */
    if (property_get(PROP_NAME, trace_prop, NULL) > 0) {
        if (trace_prop[0] == '1') {
            perf_debug_output = PERF_SYSTRACE = atoi(trace_prop);
        }
    }

    //init all modules
    mImpl.PerfGlueLayerInit();
}

int32_t Perf::perfLockAcqAndRelease(mpctl_msg_t &pMsg) {
    int32_t retVal = -1;
    client_msg_int_t reply;
    memset(&reply, 0, sizeof(client_msg_int_t));
    reply.handle = retVal;
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply);
        return retVal;
    }


    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    mImpl.PerfGlueLayerSubmitRequest(&pMsg);

    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    pMsg.pl_handle = 0;

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    reply.handle = retVal;
    sendMsg(&reply);
    return retVal;
}

void Perf::perfEvent(mpctl_msg_t &pMsg) {
    if (!checkPerfStatus(__func__)) {
        return;
    }
    pMsg.req_type = MPCTL_CMD_PERFEVENT;
    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
}

int32_t Perf::perfGetFeedback(mpctl_msg_t &pMsg) {
    int32_t retVal = -1;
    client_msg_int_t reply;
    memset(&reply, 0, sizeof(client_msg_int_t));
    reply.handle = retVal;
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply);
        return retVal;
    }
    pMsg.req_type = MPCTL_CMD_PERFGETFEEDBACK;
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    reply.handle = retVal;
    sendMsg(&reply);
    return retVal;
}

int32_t Perf::perfHintAcqRel(mpctl_msg_t &pMsg) {
    int32_t retVal = -1;
    client_msg_int_t reply;
    memset(&reply, 0, sizeof(client_msg_int_t));
    reply.handle = retVal;
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply);
        return retVal;
    }
    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    mImpl.PerfGlueLayerSubmitRequest(&pMsg);

    pMsg.pl_handle = 0;
    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    reply.handle = retVal;
    sendMsg(&reply);
    return retVal;
}


int32_t Perf::perfHintRenew(mpctl_msg_t &pMsg) {
    int32_t retVal = -1;
    client_msg_int_t reply;
    memset(&reply, 0, sizeof(client_msg_int_t));
    reply.handle = retVal;
    if (!checkPerfStatus(__func__)) {
        sendMsg(&reply);
        return retVal;
    }
    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    if (pMsg.pl_handle <= 0) {
        pMsg.renewFlag = false;
    }
    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    reply.handle = retVal;
    sendMsg(&reply);
    return retVal;
}
