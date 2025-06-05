/******************************************************************************
  @file    EventQueue.cpp
  @brief   Implementation of event queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define LOG_TAG "ANDR-PERF-EVENT-QUEUE"
#include "EventQueue.h"
#include "stdlib.h"
#include "SecureOperations.h"
#include "PerfLog.h"

using namespace std;

EventQueue::EventQueue() {
    QLOGV(LOG_TAG, "EventManager::EventManager");
    update_available = false;
    pending_event_updates = 0;
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

EventQueue::~EventQueue() {
    QLOGV(LOG_TAG, "~EventManager");
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

void EventQueue::Wakeup(EventData* data)
{
    lock();
    update_available = true;
    addTriggerUpdateToPriorityQ(data);
    pthread_cond_signal(&mCond);
    unlock();
}

EventData* EventQueue::Wait()
{
    EventData *event_data = NULL;

    lock();

    while(!update_available) {
        QLOGL(LOG_TAG, QLOG_L3, "EventManagerThread Waiting for update");
        QLOGL(LOG_TAG, QLOG_L3, "event_queue size = %" PRIu32, event_queue.getSize());
        pthread_cond_wait(&mCond, &mMutex);
    }

    QLOGL(LOG_TAG, QLOG_L3 , "EventManagerThread processing trigger update.");
    QLOGL(LOG_TAG, QLOG_L3, "event_queue size = %" PRIu32, event_queue.getSize());

    event_data = readTriggerUpdatesFromPriorityQ();

    if(!event_queue.isEmpty()) {
        update_available = true;
    } else {
        update_available = false;
    }

    unlock();

    return event_data;
}

void EventQueue::addTriggerUpdateToPriorityQ(EventData* data) {
    try {
        event_queue.insert(data,0);
        pending_event_updates = SecInt::Add(pending_event_updates, 1ul);
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

EventData *EventQueue::readTriggerUpdatesFromPriorityQ() {
    EventData *dataToProcess = NULL;

    try {
        if(!event_queue.isEmpty()) {
            dataToProcess = (EventData *)event_queue.getData();
        }
        pending_event_updates = SecInt::Subtract(pending_event_updates, 1ul);
        QLOGL(LOG_TAG, QLOG_L3, "pending_event_updates = %" PRIu32, pending_event_updates);
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }

    return dataToProcess;
}

