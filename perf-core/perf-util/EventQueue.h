/******************************************************************************
  @file    EventQueue.h
  @brief   Implementation of event queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __EVENT_QUEUE__
#define __EVENT_QUEUE__

#include "EventDataPool.h"
#include "PriorityQueue.h"
#include <inttypes.h>

using namespace std;

class EventQueue {
protected:
    pthread_cond_t mCond;
    pthread_mutex_t mMutex;
    bool update_available;
    uint32_t pending_event_updates;

    PriorityQueue<void*> event_queue;

    //EventData pool
    EventDataPool mEventDataPool;

    int32_t lock() { return pthread_mutex_lock(&mMutex); }
    int32_t unlock() { return pthread_mutex_unlock(&mMutex); }

    void addTriggerUpdateToPriorityQ(EventData* data);
    EventData *readTriggerUpdatesFromPriorityQ();

public:
    EventQueue();
    ~EventQueue();

    //thread sync/commmunication related
    EventData* Wait();
    void Wakeup(EventData* data);

    //data pool related
    EventDataPool &GetDataPool() { return mEventDataPool;}
};
#endif

