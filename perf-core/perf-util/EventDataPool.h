/******************************************************************************
  @file    EventDataPool.h
  @brief   Implementation of event data pool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __EVENT_DATA_POOL__
#define __EVENT_DATA_POOL__

#include <pthread.h>
#include <list>
#include <inttypes.h>

using namespace std;

#define MAX_POOL_SIZE 128

typedef enum {
    PRIMARY = 0,
    SECONDARY = 1,
} DataPoolType_t;

typedef struct EventData {
    uint8_t mEvType;
    void *mEvData;
    DataPoolType_t mPoolType;
    struct EventData *mNext;
    struct EventData *mPrev;

    EventData() {
        mEvType = 0;
        mEvData = NULL;
        mPrev = NULL;
        mNext = NULL;
        mPoolType = PRIMARY;
    }
}EventData;

typedef void *(*AllocCB)();
typedef void (*DeallocCB)(void *);

class EventDataPool {
protected:
    EventData *mAllocPool;
    EventData *mFreePool;
    EventData *mSecondaryFreePool;
    int16_t mSize; //pool size
    int16_t mSecondarySize; //secondary pool size
    bool mSecondaryPoolEnabled; //Flag for secondary pool enabled
    pthread_mutex_t mMutex;

    AllocCB mAllocCB;
    DeallocCB mDeallocCB;

    int16_t Resize(int16_t n, DataPoolType_t poolType = PRIMARY);
public:
    EventDataPool();
    ~EventDataPool();
    int16_t EnableSecondaryPool();
    void SetCBs(AllocCB acb, DeallocCB dcb);
    EventData* Get(DataPoolType_t poolType = PRIMARY);
    void Return(EventData *data);
    void Dump();
};

#endif

