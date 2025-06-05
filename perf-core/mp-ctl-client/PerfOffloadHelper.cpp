/******************************************************************************
  @file    PerfOffloadHelper.cpp
  @brief   Implementation of PerfOffloadHelper

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-OH"
#include "PerfOffloadHelper.h"
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <exception>
#include <cstring>
#include "PerfLog.h"

#define PROP_NAME "vendor.debug.trace.perf"

PerfOffloadHelper::PerfOffloadHelper() {
    mHandle = 0;
    memset(mHandleMapping,0,sizeof(mHandleMapping));
}

int32_t PerfOffloadHelper::getNewOffloadHandle() {
    int32_t handleCpy = FAILED;
    try {
        {
            std::scoped_lock lck(mHandleMappingLck);
            handleCpy = (mHandle + 1) % MAX_OFFLOAD_REQ_SIZE;
            if (handleCpy == 0) {
                handleCpy++;
            }
            if (handleCpy < MAX_OFFLOAD_REQ_SIZE && handleCpy > 0) {
                mHandleMapping[handleCpy] = {-1, NEW_HANDLE_STATE};
                mHandle = handleCpy;
            }
        }
        QLOGI(LOG_TAG, "Requested new Offload Handle: %" PRId32, handleCpy);
    } catch (std::exception &e) {
        handleCpy = FAILED;
        QLOGE(LOG_TAG, "Exception caught: %s in %s -Handle is invalid", e.what(), __func__);
    }
    return handleCpy;
}

int32_t PerfOffloadHelper::getPerfHandle(int32_t handle) {
    int32_t rc = FAILED;
    bool flag = true;
    if (MAX_OFFLOAD_REQ_SIZE <= handle || handle <= 0) {
        QLOGE(LOG_TAG, "Invalid handle req: %" PRId32, handle);
        return FAILED;
    }
    try {
        std::scoped_lock lck(mHandleMappingLck);
            rc = mHandleMapping[handle].mPerfHandle;
            if (rc == -1 && mHandleMapping[handle].mState == NEW_HANDLE_STATE) {
                mHandleMapping[handle].mState = RELEASE_REQ_STATE;
                flag = false;
            }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s -Handle is invalid", e.what(), __func__);
    }
    QLOGI(LOG_TAG, "Perf Handle:[%" PRId32 "], OffloadHandle:[%" PRId32 "], flag:[%" PRId8 "]", rc, handle, flag);
    return rc;
}

int32_t PerfOffloadHelper::mapPerfHandle(int32_t handle, int32_t requestHandle) {
    uint8_t returnState = REMOVE_HANDLE_STATE;
    if (MAX_OFFLOAD_REQ_SIZE <= handle || handle <= 0) {
        QLOGE(LOG_TAG, "Invalid handle map req: %" PRId32 " to %" PRId32, handle, requestHandle);
        return returnState;
    }
    try {
            std::scoped_lock lck(mHandleMappingLck);
            if (mHandleMapping[handle].mState == RELEASE_REQ_STATE) {
                returnState = RELEASE_REQ_STATE;
            }
            if (returnState != RELEASE_REQ_STATE) {
                mHandleMapping[handle] = {requestHandle, NEW_HANDLE_STATE};
            }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s ", e.what(), __func__);
    }
    QLOGI(LOG_TAG, "Perf Handle:[%" PRId32 "], OffloadHandle:[%" PRId32 "], State:[%" PRIu8 "]", requestHandle, handle, returnState);
    return returnState;
}

void PerfOffloadHelper::releaseHandle(int32_t handle) {
    if (MAX_OFFLOAD_REQ_SIZE <= handle || handle <= 0) {
        QLOGE(LOG_TAG, "Invalid handle release req: %" PRId32 " in %s ", handle, __func__);
        return;
    }

    try {
        std::scoped_lock lck(mHandleMappingLck);
        QLOGI(LOG_TAG, "Perf Handle: %" PRId32 " mapped to OffloadHandle: %" PRId32 " is released", mHandleMapping[handle].mPerfHandle, handle);
        mHandleMapping[handle] = {-1, NEW_HANDLE_STATE};
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s ", e.what(), __func__);
    }
}

PerfOffloadHelper &PerfOffloadHelper::getPerfOffloadHelper() {
    static PerfOffloadHelper mPerfOffloadHelper;
    return mPerfOffloadHelper;
}

PerfOffloadHelper::~PerfOffloadHelper() {
}
