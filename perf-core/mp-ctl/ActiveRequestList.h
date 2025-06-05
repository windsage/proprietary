/******************************************************************************
  @file    ActiveRequestList.h
  @brief   Implementation of active request lists

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __ACTIVEREQUESTLIST__H_
#define __ACTIVEREQUESTLIST__H_

#include <string>
#include <unordered_map>
#include <atomic>
#include <Request.h>

class Request;

class RequestListNode {
public:
    int32_t mRetHandle;
    Request *mHandle;
    uint32_t mHintId;
    int32_t mHintType;
    timeval mtv;
    RequestListNode() {
        mHandle = NULL;
        mRetHandle = -1;
    }
    RequestListNode(int32_t handle,Request *req, uint32_t hint_id, int32_t hint_type, timeval &tv) {
        mHandle = req;
        mRetHandle = handle;
        mHintId = hint_id;
        mHintType = hint_type;
        mtv = tv;
    }
    ~RequestListNode() {
        if(mHandle != NULL) {
            delete mHandle;
            mHandle = NULL;
        }
    }
};

class ActiveRequestList {
    private:
        std::unordered_map<int32_t,RequestListNode*> mActiveList;
        std::atomic<uint8_t> mActiveReqs;
        pthread_mutex_t mMutex;

    public:
        static uint8_t ACTIVE_REQS_MAX;

        ActiveRequestList() {
            mActiveReqs = 0;
            pthread_mutex_init(&mMutex, NULL);
        };
        ~ActiveRequestList() {
            pthread_mutex_destroy(&mMutex);
        };

        int8_t Add(Request *req, int32_t req_handle, uint32_t hint_id = 0, int32_t hint_type = 0);
        int getTimestamp(struct timeval &tv, std::string &timestamp);
        void Remove(int32_t handle);
        void Reset();
        Request * DoesExists(int32_t handle);
        Request * IsActive( int32_t handle, Request *req);
        void ResetRequestTimer(int32_t handle, Request *req);
        uint8_t GetNumActiveRequests();
        uint8_t GetActiveReqsInfo(int32_t *handles, uint32_t *pids);
        void Dump();
        std::string DumpActive();
        std::string getPkgNameFromPid(uint32_t pid);
        bool CanSubmit();
};

#endif
