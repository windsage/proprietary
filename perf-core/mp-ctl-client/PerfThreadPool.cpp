/******************************************************************************
  @file    PerfThreadPool.cpp
  @brief   Implementation of PerfThreadPool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#define LOG_TAG "ANDR-PERF-TP"
#include "PerfThreadPool.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/resource.h>
#include "PerfLog.h"
#include <linux/sched.h>
#include "Common.h"

#define THREAD_POOL_NAME "%d-%.*s-%d"
#define PROCESS_NAME_CHARS 6

ThreadPoolData::ThreadPoolData(int32_t thread_id) {
    mThreadId = thread_id;
    mFunc = NULL;
    mGotTask = false;
    mState = RUNNING_STATE;
    mHasSlept = false;
    mType = REGULAR;
}

ThreadPoolData::~ThreadPoolData() {
    if (mFunc != NULL) {
        delete mFunc;
        mFunc = NULL;
    }
}

void ThreadPoolData::wait() {
    try {
        unique_lock<mutex> lck(mTaskAvailable);
        setState(SLEEPING_STATE);
        mCondVar.wait(lck,[&]{mHasSlept = true;return mGotTask.load();});
        mGotTask = false;
        mHasSlept = false;
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

void ThreadPoolData::signal() {
    try {
        unique_lock<mutex> lck(mTaskAvailable);
        mGotTask = true;
        mCondVar.notify_one();
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

int8_t ThreadPoolData::getIfAvailable(ThreadType_t type) {
    int8_t available = FAILED;
    try {
        std::unique_lock<std::mutex> lock(mStateMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            if (getState() == SLEEPING_STATE && mHasSlept && mType == type) {
                mState = RUNNING_STATE;
                available = SUCCESS;
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return available;
}

void ThreadPoolData::setState(ThreadState_t state) {
    try {
        std::unique_lock<std::mutex> lock(mStateMutex);
        if (mState != INVALID_STATE) {
            mState = state;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

PerfThreadPool::PerfThreadPool() {
    QLOGV(LOG_TAG, "Constructor %s", __func__);
    mPoolSize = 0;
    mPoolCreated = false;
    mLastThreadIndex = -1;

}

int8_t PerfThreadPool::setNonRT() {

    struct sched_param sparam_old;
    struct sched_param sparam;
    int32_t policy_old, priority_old;
    int32_t policy, priority;
    int32_t rc;

    policy = SCHED_NORMAL;
    priority = sched_get_priority_min(policy);
    sparam.sched_priority = priority;

    rc = pthread_getschedparam(pthread_self(), &policy_old, &sparam_old);
    if (rc) {
        QLOGE(LOG_TAG, "Failed to get sched params %" PRId32, rc);
        return FAILED;
    }
    if (policy_old != policy || sparam_old.sched_priority != priority) {
        rc = pthread_setschedparam(pthread_self(), policy, &sparam);
        if (rc != SUCCESS) {
            QLOGE(LOG_TAG, "Failed to set sched params %" PRId32, rc);
            return FAILED;
        }
    }
    return SUCCESS;
}

void *PerfThreadPool::executeTask(void *ptr) {
    int32_t task_executed = 0;
    int8_t ret_code = 0;
    ThreadPoolData *thread_data = (ThreadPoolData *)ptr;
    if(thread_data == NULL) {
        QLOGE(LOG_TAG,  "Thread exiting NULL data");
        return NULL;
    }
    ret_code = setNonRT();
    if (ret_code != SUCCESS) {
        QLOGE(LOG_TAG, "Failed to set priority of thread Thread-%" PRId32, thread_data->mThreadId);
        return NULL;
    }

    do {
        QLOGI(LOG_TAG, "Thread-%" PRId32 " executed [%" PRId32 "] tasks", thread_data->mThreadId, task_executed);
        thread_data->wait();
        QLOGI(LOG_TAG, "Thread-%" PRId32 " got task", thread_data->mThreadId);
        if (thread_data->getState() == RUNNING_STATE) {
            try {
                if (thread_data->mFunc != NULL) {
                    (*(thread_data->mFunc))();
                    delete thread_data->mFunc;
                    thread_data->mFunc = NULL;
                    task_executed++;
                }
            } catch (std::exception &e) {
                QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
            } catch (...) {
                QLOGE(LOG_TAG, "Exception caught in %s", __func__);
            }
            QLOGI(LOG_TAG, "setting Thread-%" PRId32 " to sleep state", thread_data->mThreadId);
        }
    } while(thread_data->getState() != INVALID_STATE);
    QLOGI(LOG_TAG, "Thread-%" PRId32 " exiting, Total Task executed[%" PRId32 "]", thread_data->mThreadId, task_executed);
    return NULL;
}

int32_t PerfThreadPool::addThreads(int32_t size,ThreadType_t type) {
    int32_t rc = -1;
    char buf[MAX_BUF_SIZE] = {0};
    uint32_t pid = getpid();
    getthread_name(buf, MAX_BUF_SIZE);
    for (int32_t i = 0; i < size; i++) {
        char tname[MAX_BUF_SIZE] = {0};
        ThreadPoolData *tmp = new(std::nothrow) ThreadPoolData(mPoolSize);
        if (tmp != NULL) {
            rc = pthread_create(&(tmp->mThread),NULL,executeTask, tmp);
            if (rc == 0) {

                snprintf(tname, MAX_BUF_SIZE, THREAD_POOL_NAME , pid, PROCESS_NAME_CHARS, buf, mPoolSize.load());
                rc = pthread_setname_np(tmp->mThread, tname);
                if (rc != 0) {
                    QLOGE(LOG_TAG, "Failed to name Thread-%" PRId8 " for %s process",mPoolSize.load(), buf);
                }
                tmp->mType = type;
                try {
                    mThreadPool.push_back(tmp);
                    mPoolSize++;
                } catch (std::exception &e) {
                    QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
                    QLOGE(LOG_TAG, "Error in Thread Creation");
                    if (tmp != NULL) {
                        tmp->setState(INVALID_STATE);
                        tmp->signal();
                        pthread_join(tmp->mThread, NULL);
                        delete tmp;
                    }
                } catch (...) {
                    QLOGE(LOG_TAG, "Exception caught in %s", __func__);
                    QLOGE(LOG_TAG, "Error in Thread Creation");
                    if (tmp != NULL) {
                        tmp->setState(INVALID_STATE);
                        tmp->signal();
                        pthread_join(tmp->mThread, NULL);
                        delete tmp;
                    }
                }
            } else {
                QLOGE(LOG_TAG, "Error in Thread Creation");
                if (tmp != NULL) {
                    delete tmp;
                }
            }
        } else {
            QLOGE(LOG_TAG, "Error in Thread Creation");
        }
    }
    return mPoolSize;
}

/*
using this method for creating threads at runtime only for clients using perf_hint_offload API.
clients can resize the pool. only 2 threads will be created on first API call.
*/
void PerfThreadPool::create(int32_t size) {
    try {
        std::scoped_lock lck(mSizeMutex);

        if (mPoolCreated || mPoolSize > 0) {
            return;
        } else if(size <= 0) {
            size = NUM_THREADS;
            QLOGE(LOG_TAG, "Invalid Size creating %" PRId32 " Threads in Pool", size);
        }
        size = size < MAX_NUM_THREADS ? size : MAX_NUM_THREADS;
        addThreads(size, REGULAR);
        if (mPoolSize > 0) {
            mPoolCreated = true;
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}

/*
Get a dedicated thread in pool. Client has to maintain the index of thread
returned by getDedicated() to place task on thread using placeTaskAt().
*/
int32_t PerfThreadPool::getDedicated() {
    int32_t size = mPoolSize;
    int32_t dthreadindex = resize(ONE_THREAD, DEDICATED);
    if (dthreadindex != size) {
        dthreadindex--;
        mDedicatedThreads.push_back(dthreadindex);
        return dthreadindex;
    }
    QLOGE(LOG_TAG, "FAILED to create a Dedicated Thread in Pool");
    return FAILED;
}

/*
resize/extend number of threads in ThreadPool.
*/
int32_t PerfThreadPool::resize(int32_t size) {
    return resize(size,REGULAR);
}

int32_t PerfThreadPool::resize(int32_t size,ThreadType_t type) {
    int32_t pool_new_size = 0;
    try {
        std::scoped_lock lck(mSizeMutex);
        pool_new_size = mPoolSize;
        if (!mPoolCreated || !(mPoolSize > 0)) {
            create(size);
            return mPoolSize;
        } else if (size <= 0 || (size + mPoolSize) > INT32_MAX || (size + mPoolSize) > MAX_NUM_THREADS || (size + mPoolSize) < 0) {
            QLOGE(LOG_TAG, "Invalid size: %" PRId32 " Max Threads allowed = %" PRId8, size, MAX_NUM_THREADS);
            return pool_new_size;
        }
        pool_new_size = addThreads(size, type);
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    return pool_new_size;
}

//method to offload task to a running dedicated thread
int8_t PerfThreadPool::placeTaskAt(std::function<void()> &&lambda, int32_t i) {
    int8_t rc = FAILED;
    int32_t pool_size = mPoolSize;
    if (i < 0 || i >= pool_size) {
        QLOGE(LOG_TAG, "Failed to offload task invalid thread index");
        return FAILED;
    }

    if (mThreadPool[i] != NULL && mThreadPool[i]->getIfAvailable(DEDICATED) == SUCCESS) {
        mThreadPool[i]->mFunc = new(std::nothrow) std::function<void()>(lambda);
        if (mThreadPool[i]->mFunc != NULL) {
            mThreadPool[i]->signal();
            QLOGI(LOG_TAG, "Task placed on Thread-%" PRId32, i);
            rc = SUCCESS;
        } else {
            mThreadPool[i]->setState(SLEEPING_STATE);
            QLOGE(LOG_TAG, "Failed to offload task to Thread-%" PRId32 " Memory allocation error", i);
        }
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to offload task, Thread-%" PRId32 " is busy.", i);
    }
    return rc;
}

//method to offload task to a running thread
int8_t PerfThreadPool::placeTask(std::function<void()> &&lambda) {
    int8_t rc = FAILED;
    int32_t pool_size = mPoolSize;
    int32_t currIndex = 0;
    if (pool_size == 0 || !mPoolCreated) {
        QLOGE(LOG_TAG, "Pool not created use resize() to add threads to pool");
    }
    try {
        for(int32_t i = 0 ;i < pool_size; i++) {
            {
                std::unique_lock<std::mutex> lock(mSizeMutex, std::try_to_lock);
                if (lock.owns_lock() == false) {
                    continue;
                }
                mLastThreadIndex = (mLastThreadIndex + 1) % pool_size;
                currIndex = mLastThreadIndex;
            }
            QLOGI(LOG_TAG, "Trying to Get Thread %" PRId32, currIndex);
            if (mThreadPool[currIndex] != NULL && mThreadPool[currIndex]->getIfAvailable() == SUCCESS) {
                QLOGI(LOG_TAG, "Got Thread %" PRId32, currIndex);
                mThreadPool[currIndex]->mFunc = new(std::nothrow) std::function<void()>(lambda);
                if (mThreadPool[currIndex]->mFunc != NULL) {
                    mThreadPool[currIndex]->signal();
                    QLOGI(LOG_TAG, "Waking up Thread-%" PRId32, currIndex);
                    QLOGI(LOG_TAG, "Task placed on thread-%" PRId32, currIndex);
                    rc = SUCCESS;
                    break;
                } else {
                    mThreadPool[currIndex]->setState(SLEEPING_STATE);
                    QLOGE(LOG_TAG, "Failed to offload task to Thread-%" PRId32 " Memory allocation error", currIndex);
                }
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
    if (rc == FAILED) {
        QLOGE(LOG_TAG, "Failed to offload task all threads are busy.");
    }
    return rc;
}

PerfThreadPool &PerfThreadPool::getPerfThreadPool(int32_t size) {
    static PerfThreadPool singleton_pool_obj;

    if (!singleton_pool_obj.mPoolCreated) {
        singleton_pool_obj.create(size);
    }
    return singleton_pool_obj;
}

PerfThreadPool::~PerfThreadPool() {
    QLOGV(LOG_TAG, "%s ", __func__);
    try {
        std::scoped_lock lck(mSizeMutex);
        int32_t pool_size = mPoolSize;
        mPoolSize = 0;
        for(int32_t i = 0; i < pool_size; i++) {
            if (mThreadPool[i] != NULL) {
                mThreadPool[i]->setState(INVALID_STATE);
                mThreadPool[i]->signal();
                pthread_join(mThreadPool[i]->mThread,NULL);
                if (mThreadPool[i]->mFunc != NULL) {
                    delete mThreadPool[i]->mFunc;
                    mThreadPool[i]->mFunc = NULL;
                }
                QLOGI(LOG_TAG, "Thread-%" PRId32 " join", i);
                delete  mThreadPool[i];
                mThreadPool[i] = NULL;
            }
        }
    } catch (std::exception &e) {
        QLOGE(LOG_TAG, "Exception caught: %s in %s", e.what(), __func__);
    } catch (...) {
        QLOGE(LOG_TAG, "Exception caught in %s", __func__);
    }
}
