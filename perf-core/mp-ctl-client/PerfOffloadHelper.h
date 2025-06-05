/******************************************************************************
  @file    PerfOffloadHelper.h
  @brief   Implementation of PerfOffloadHelper

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef __PERFOFFLOADHELPER_H__
#define __PERFOFFLOADHELPER_H__

#include <mutex>
#include <unordered_map>

using namespace std;

#define MAX_BUF_SIZE 16
#define FAILED -1
#define SUCCESS 0
#define MAX_OFFLOAD_REQ_SIZE 500

enum HELPER_STATE { NEW_HANDLE_STATE = 0,
                    RELEASE_REQ_STATE,
                    REMOVE_HANDLE_STATE,
};

typedef struct helper_data_t {
    int32_t mPerfHandle;
    int32_t mState;
} helper_data_t;

class PerfOffloadHelper {

    private:
        uint32_t mHandle;
        helper_data_t mHandleMapping[MAX_OFFLOAD_REQ_SIZE];
        mutex mHandleMappingLck;

        //ctor, copy ctor, assignment overloading
        PerfOffloadHelper();
        PerfOffloadHelper(PerfOffloadHelper const& oh);
        PerfOffloadHelper& operator=(PerfOffloadHelper const& oh);

    public:
        ~PerfOffloadHelper();
        int32_t getPerfHandle(int32_t handle);
        int32_t getNewOffloadHandle();
        int32_t mapPerfHandle(int32_t handle, int32_t requestHandle);
        void releaseHandle(int32_t handle);
        static PerfOffloadHelper &getPerfOffloadHelper();
};

#endif /*__PERFOFFLOADHELPER_H__*/
