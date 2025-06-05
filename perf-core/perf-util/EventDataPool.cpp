/******************************************************************************
  @file    EventDataPool.cpp
  @brief   Implementation of event data pool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-DATA-POOL"
#include "EventQueue.h"
#include "stdlib.h"

#include "PerfLog.h"

using namespace std;

//event data memory pool
EventDataPool::EventDataPool() {
    mAllocPool = NULL;
    mFreePool = NULL;
    mAllocCB = NULL;
    mDeallocCB = NULL;
    mSecondaryFreePool = NULL;
    mSecondaryPoolEnabled = false;
    mSize = 0;
    mSecondarySize = 0;
    pthread_mutex_init(&mMutex, NULL);
    pthread_mutex_lock(&mMutex);
    Resize(MAX_POOL_SIZE);
    pthread_mutex_unlock(&mMutex);
}

//if not registered callbacks memory management of msg data lies with user
void EventDataPool::SetCBs(AllocCB acb, DeallocCB dcb) {
    pthread_mutex_lock(&mMutex);
    mAllocCB = acb;
    mDeallocCB = dcb;
    pthread_mutex_unlock(&mMutex);
}

int16_t EventDataPool::EnableSecondaryPool() {
    int16_t poolSize = -1;
    pthread_mutex_lock(&mMutex);
    if (mSecondaryPoolEnabled == true && mSecondarySize > 0) {
        pthread_mutex_unlock(&mMutex);
       return mSecondarySize;
    }
    poolSize =  Resize(MAX_POOL_SIZE, SECONDARY);
    mSecondaryPoolEnabled = true;
    pthread_mutex_unlock(&mMutex);
    return poolSize;
}

EventDataPool::~EventDataPool() {
    //freeup free pool, alloc pool
    EventData *pCur = NULL, *tmp = NULL;

    for (pCur = mFreePool; pCur;) {
        tmp = pCur;
        pCur = pCur->mNext;
        if (mDeallocCB) {
            mDeallocCB(tmp->mEvData);
        }
        delete tmp;
    }

    for (pCur = mSecondaryFreePool; pCur;) {
        tmp = pCur;
        pCur = pCur->mNext;
        if (mDeallocCB) {
            mDeallocCB(tmp->mEvData);
        }
        delete tmp;
    }

    for (pCur = mAllocPool; pCur;) {
        tmp = pCur;
        pCur = pCur->mNext;
        if (mDeallocCB) {
            mDeallocCB(tmp->mEvData);
        }
        delete tmp;
    }

    mAllocPool = NULL;
    mFreePool = NULL;
    pthread_mutex_destroy(&mMutex);
}

int16_t EventDataPool::Resize(int16_t n, DataPoolType_t poolType) {
    EventData *pCur = NULL, *poolPtr = NULL;
    int16_t i =0;
    int16_t retSize = 0;

    //if free items still available in the pool then do not resize
    if (poolType == SECONDARY) {
        poolPtr = pCur = mSecondaryFreePool;
        retSize = mSecondarySize;
    } else {
        poolPtr = pCur = mFreePool;
        retSize = mSize;
    }

    if (NULL != pCur) {
        return retSize;
    }

    //resize
    for (i = 0; i < n; i++) {
        pCur = new(std::nothrow) EventData;
        if (NULL != pCur) {
            pCur->mNext = poolPtr;
            pCur->mPoolType = poolType;

            if(NULL != poolPtr) {
                poolPtr->mPrev = pCur;
            }
            poolPtr = pCur;
        } else {
            break;
        }
    }

    retSize += i;
    QLOGL(LOG_TAG, QLOG_L1, "New size of Event Data Pool:%" PRId16 "\n", retSize);
    pCur = poolPtr;
    if (poolType == SECONDARY) {
        mSecondaryFreePool = poolPtr;
        mSecondarySize = retSize;
    } else {
        mFreePool = poolPtr;
        mSize = retSize;
    }

    if (NULL == pCur) {
        return -1;
    }

    return retSize;
}

EventData* EventDataPool::Get(DataPoolType_t poolType) {
    bool bAlloc = false;
    EventData *pCur = NULL, *poolPtr = NULL;

    pthread_mutex_lock(&mMutex);
    //all allocated, no free items
    if (poolType == SECONDARY) {
        poolPtr = mSecondaryFreePool;
        if (poolPtr == NULL) {
            QLOGE(LOG_TAG, "Secondary EventDataPool is Full");
            poolType = PRIMARY;
        }
    }
    if (poolType == PRIMARY) {
        poolPtr = mFreePool;
    }
    if (NULL == poolPtr) {
        QLOGE(LOG_TAG, "EventDataPool is Full");
        pthread_mutex_unlock(&mMutex);
        return NULL;
    }

    //remove from free pool if only type of data matches
    pCur = poolPtr;
    poolPtr = pCur->mNext;
    if(NULL != poolPtr) {
        poolPtr->mPrev = NULL;
    }
    if (NULL == pCur->mEvData) {
        //no data in event data, so allocate
        bAlloc = true;
    }

    if (bAlloc && mAllocCB) {
        pCur->mEvData = mAllocCB();
    }

    //add it to alloc pool
    pCur->mNext = mAllocPool;
    if(NULL != mAllocPool) {
        mAllocPool->mPrev = pCur;
    }
    mAllocPool = pCur;

    if (poolType == SECONDARY) {
        mSecondaryFreePool = poolPtr;
    } else {
        mFreePool = poolPtr;
    }

    pthread_mutex_unlock(&mMutex);
    return pCur;
}

void EventDataPool::Return(EventData *data) {
    EventData *tmp;
    EventData *pCur = data;

    if (NULL == pCur) {
        return;
    }

    QLOGL(LOG_TAG, QLOG_L3, "call to Return of eventpool data %p", data);

    pthread_mutex_lock(&mMutex);
    //remove from alloc pool
    tmp = pCur->mPrev;
    if(NULL != tmp) {
        tmp->mNext =  pCur->mNext;
    } else {
        mAllocPool = pCur->mNext;
    }

    if (NULL !=  pCur->mNext) {
        pCur->mNext->mPrev = tmp;
    }

    //add it to free pool
    if (pCur->mPoolType == SECONDARY) {
        pCur->mNext = mSecondaryFreePool;
        pCur->mPrev = NULL;
        if(NULL != mSecondaryFreePool) {
            mSecondaryFreePool->mPrev = pCur;
        }
        mSecondaryFreePool = pCur;
    } else {
        pCur->mNext = mFreePool;
        pCur->mPrev = NULL;
        if(NULL != mFreePool) {
            mFreePool->mPrev = pCur;
        }
        mFreePool = pCur;
    }
    pthread_mutex_unlock(&mMutex);
    return;
}

void EventDataPool::Dump() {

    EventData *pCur = NULL;
    pthread_mutex_lock(&mMutex);
    for (pCur = mFreePool; pCur;) {
        QLOGL(LOG_TAG, QLOG_L3, "Freepool: pcur:%p pcurdata:%p", pCur, pCur->mEvData);
        pCur = pCur->mNext;
    }

    for (pCur = mSecondaryFreePool; pCur;) {
        QLOGL(LOG_TAG, QLOG_L3, "SecondaryFreePool: pcur:%p pcurdata:%p", pCur, pCur->mEvData);
        pCur = pCur->mNext;
    }

    for (pCur = mAllocPool; pCur;) {
        QLOGL(LOG_TAG, QLOG_L3, "Allocpool: pcur:%p pcurdata:%p", pCur, pCur->mEvData);
        pCur = pCur->mNext;
    }
    pthread_mutex_unlock(&mMutex);
}
