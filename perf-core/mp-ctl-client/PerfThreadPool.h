/******************************************************************************
  @file    PerfThreadPool.h
  @brief   Implementation of PerfThreadPool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef __PERFTHREADPOOL_H__
#define __PERFTHREADPOOL_H__

#include <functional>
#include <vector>
#include <pthread.h>
#include <mutex>

#include <atomic>
#include <condition_variable>

using namespace std;

#define NUM_THREADS 4
#define MAX_NUM_THREADS 16
#define MAX_BUF_SIZE 16
#define FAILED -1
#define SUCCESS 0
#define ONE_THREAD 1

typedef enum {
    INVALID_STATE = -1,
    SLEEPING_STATE,
    RUNNING_STATE
}ThreadState_t;


typedef enum {
    REGULAR = 0,
    DEDICATED,
}ThreadType_t;

class ThreadPoolData {
    private:
        int32_t mThreadId;
        pthread_t mThread;
        atomic<ThreadState_t> mState;
        atomic<ThreadType_t> mType;
        atomic<bool> mHasSlept;
        function<void()> *mFunc;
        atomic<bool> mGotTask;
        mutex mTaskAvailable;
        condition_variable mCondVar;
        mutex mStateMutex;
        ThreadPoolData();
        ThreadPoolData(int32_t thread_id);
        ~ThreadPoolData();

        void wait();
        void signal();

        ThreadState_t getState() {
            return mState;
        }

        int8_t getIfAvailable(ThreadType_t type);
        void setState(ThreadState_t state);

        int8_t getIfAvailable() {
            return getIfAvailable(REGULAR);
        }

    public:
        friend class PerfThreadPool;
};

class PerfThreadPool {

    private:
        mutex mSizeMutex;
        atomic<int32_t>  mPoolSize; //number of threads in pool
        atomic<bool> mPoolCreated;
        int8_t mLastThreadIndex;
        vector<ThreadPoolData*> mThreadPool; //vector of threads.
        vector<int8_t> mDedicatedThreads; //vector of dedicated threads index.

        //ctor, copy ctor, assignment overloading
        PerfThreadPool();
        PerfThreadPool(PerfThreadPool const& oh);
        PerfThreadPool& operator=(PerfThreadPool const& oh);
        void create(int32_t size); //create thread initially.
        static void *executeTask(void *);
        int32_t addThreads(int32_t size, ThreadType_t type); //create threads in pool
        int32_t resize(int32_t size, ThreadType_t type);
        static int8_t setNonRT();

    public:
        ~PerfThreadPool();
        int32_t resize(int32_t size); //add more thread to pool
        int32_t getDedicated();
        int8_t placeTask(std::function<void()> &&lambda);
        int8_t placeTaskAt(std::function<void()> &&lambda, int32_t i);
        static PerfThreadPool &getPerfThreadPool(int size = NUM_THREADS);
};

#endif /*__PERFTHREADPOOL_H__*/
