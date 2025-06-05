/********************************************
Copyright (c) 2022-2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <BufferAllocator/BufferAllocatorWrapper.h>
#include "object.h"
#include "IListenerCBO_invoke.h"
#include "CListenersmci.h"
#include "listenerMngr.h"
#include "heap.h"
#ifdef ANDROID
#include <utils/Log.h>
#include <common_log.h>
#include <android/log.h>
#include <cutils/properties.h>
#endif
#ifdef OE
#include <syslog.h>
#define LOGI(...) syslog(LOG_NOTICE, "INFO:" __VA_ARGS__)
#define LOGV(...) syslog(LOG_NOTICE,"VERB:" __VA_ARGS__)
#define LOGD(...) syslog(LOG_DEBUG,"DBG:" __VA_ARGS__)
#define LOGE(...) syslog(LOG_ERR,"ERR:" __VA_ARGS__)
#define LOGW(...) syslog(LOG_WARNING,"WRN:" __VA_ARGS__)
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CListenerCBO"
#endif

static inline int atomic_add(int *pn, int n)
{
  return __sync_add_and_fetch(pn, n);  // GCC builtin
}

static int32_t CListenerCBO_retain(ListenerCBO* me)
{
  atomic_add(&me->refs, 1);
  return Object_OK;
}

static int32_t CListenerCBO_release(ListenerCBO* me)
{
  if (atomic_add(&me->refs, -1) == 0) {
    Object_ASSIGN_NULL(me->smo);
    pthread_mutex_destroy(&me->waitMutex);
    delete(me->listWaitCond);
    HEAP_FREE_PTR(me);
    me = NULL;
  }
  return Object_OK;
}

static int32_t CListenerCBO_request(ListenerCBO* me,
                                    uint32_t *embeddedBufOffsets_ptr,
                                    size_t embeddedBufOffsets_len,
                                    size_t *embeddedBufOffsets_lenout,
                                    uint32_t *is64_ptr,
                                    Object *smo1_ptr, Object *smo2_ptr,
                                    Object *smo3_ptr, Object *smo4_ptr)
{
  int ret = 0;
  unsigned char *dmaBuffer = nullptr;

  /* setting listener_state to true when the listener is busy */
  me->listenerBusy = true;

  ret = DmabufHeapCpuSyncStart(me->dmaBufMgr.dmaBufAllocator,
                  me->dmaBufMgr.dmaFd, kSyncReadWrite, NULL, NULL);
  if (ret) {
    LOGE("Failed CPU Sync Start %d\n", ret);
    goto exit;
  }

  dmaBuffer = (unsigned char *)mmap(NULL, me->dmaBufLen, PROT_READ | PROT_WRITE,
                  MAP_SHARED, me->dmaBufMgr.dmaFd, 0);
  if (dmaBuffer == MAP_FAILED) {
    LOGE("mmap operation failed %d\n",ret);
    ret = -1;
    goto exit;
  }

  ret = me->smciDispatchFunc(dmaBuffer, me->dmaBufLen);

  munmap(dmaBuffer, me->dmaBufLen);

  ret = DmabufHeapCpuSyncEnd(me->dmaBufMgr.dmaBufAllocator,
                  me->dmaBufMgr.dmaFd, kSyncReadWrite, NULL, NULL);
  if (ret) {
    LOGE("Failed CPU Sync End operation %d\n", ret);
    goto exit;
  }

exit:
  /* setting listener_state to false when the listener is processed */
  me->listenerBusy = false;

  /* signal the TAs in QTEE waiting on this listener for availability */
  signal_waiting_listener(me);

  return ret;
}
/*
 * Description: QTEE/TA which is waiting for a listener is pushed into
 *              a queue.The client pushed into the queue is present there
 *              for a maximum for 4.5 seconds, since the SMCInvoke driver
 *              timout is 5 seconds, otherwise it results in a timeout
 *              error.
 *
 * IN:          me : Object
 *
 * Out:         void
 *
 * Return:      on success : Object_OK
 *              on failure : Object_ERROR
 *
 */
static int32_t CListenerCBO_wait(ListenerCBO *me)
{
    int32_t ret = 0;
    int32_t cond_ret_val = 0;
    int32_t rv = Object_OK;
    int pid = getpid();
    pthread_cond_t *wait_cond = new pthread_cond_t;

     /*  Initialize time-out variable. */
    struct timespec wakeup_time;

    /* Only if the listener is busy, wait*/
    if (me->listenerBusy == false) {
        goto exit_wait;
    }

    /* Now wait... */
    ret =  pthread_mutex_lock(&me->waitMutex);
    if (ret != 0) {
        ALOGE("[%s], PID : %d, Failed to acquire the wait mutex: %s (%d) for"
            "lid : %d",  __FUNCTION__, pid, strerror(errno), errno,
             me->listenerId);
        rv = Object_ERROR;
        goto exit_wait;
    }

    /* Create a condition variable for each client that calls into
       wait in the QTEE for specific listener.
    */
    ret = pthread_cond_init(wait_cond, NULL);
    if (ret != 0) {
        ALOGE("[%s], PID : %d, Failed to init wait cond: %s (%d) for lid : %d",
            __FUNCTION__, pid, strerror(errno), errno, me->listenerId);
        rv = Object_ERROR;
        goto exit;
    }

    /* Client waiting for the listener is pushed into a list */
    me->listWaitCond->push_back(wait_cond);

    /* Current client, which called into wait, will wait for the
       listener to be be freed till signalled.
       The client in the queue will wait for a maximum of given duration
     */
    clock_gettime(CLOCK_REALTIME, &wakeup_time);

    /* SMC invoke driver timeout is 5 seconds.
       So within 4.5 seconds the request that is waiting to be signalled
       should be addessed, else it will result in timeout error.
     */
    wakeup_time.tv_sec += 4.5;

    cond_ret_val = pthread_cond_timedwait(wait_cond,
                           &me->waitMutex, &wakeup_time);
    if(cond_ret_val != 0) {
        if(cond_ret_val == ETIMEDOUT) {
            ALOGE("[%s], PID : %d, Timed out: The max limit on wait timedout :"
            "%s (%d) for lid : %d", __FUNCTION__, pid, strerror(errno), errno,
            me->listenerId);
            /* In case of a timeout, dont send error back to TZ. Instead, we want the
             * request to be lined up again.
             */
            rv =  Object_OK;
            me->listWaitCond->remove(wait_cond);
        } else {
            ALOGE("[%s], PID : %d, pthread_cond_timedwait failed : %s, (%d)"
            "for lid : %d", __FUNCTION__, pid, strerror(errno), errno,
            me->listenerId);
            me->listWaitCond->remove(wait_cond);
            rv = Object_ERROR;
        }
    }

exit:
    pthread_mutex_unlock(&me->waitMutex);
exit_wait:
    delete(wait_cond);
    return rv;
}

static IListenerCBO_DEFINE_INVOKE(CListenerCBO_invoke,
                                      CListenerCBO_, ListenerCBO*)

int32_t CListenerCBO_new(Object *objOut, Object smo,
                               struct listenerServices *listener)
{
  int ret = 0;
  dispatchEntry dispEntry;
  ListenerCBO* me = NULL;

  me = HEAP_ZALLOC_TYPE(ListenerCBO);
  if (!me) {
    return Object_ERROR_KMEM;
  }

  if(listener->libHandle == NULL) {
    listener->libHandle  = dlopen(listener->fileName, RTLD_NOW);
    if (listener->libHandle == NULL) {
      LOGE("Init dlopen(%s, RLTD_NOW) is failed.... %s\n",
                                 listener->fileName, dlerror());
      ret = Object_ERROR;
      goto bail;
    }
  }

  dispEntry = (dispatchEntry)dlsym(listener->libHandle,
                  listener->smciDispatch);
  if (dispEntry == NULL) {
    LOGE("dlsym(%s) not found in lib %s, dlerror msg: %s\n",
                    listener->smciDispatch, listener->fileName, dlerror());
    ret = Object_ERROR;
    goto bail;
  }

  me->refs = 1;
  me->smciDispatchFunc = dispEntry;
  me->dmaBufMgr = listener->dmaBufMgr;
  me->dmaBufLen = listener->listenerLen;
  me->listenerId = listener->id;
  Object_INIT(me->smo, smo);

  me->listWaitCond = new std::list<pthread_cond_t *>();
  if(!me->listWaitCond)
      goto bail;
  me->listenerBusy = false;
  if (pthread_mutex_init(&me->waitMutex, NULL)) {
        ALOGE("[%s (%p)] Failed to init mutex for CwaitOnlistenerCbo %d ctx",
              __FUNCTION__, me, me->listenerId);
        ret = Object_ERROR;
        goto bail;
  }

  *objOut = (Object) { CListenerCBO_invoke, me };
  return Object_OK;

bail:
  LOGD("Ctor of CListenerCBO failed and is bailing due to error");
  if(me->listWaitCond)
   delete(me->listWaitCond);
  HEAP_FREE_PTR(me);
  return ret;
}

/* Description: The QTEE/TA waiting for listener in the queue are freed in
 *              FIFO order to access the listener, after the listener is free
 *              from the client accessing it.
 *
 * IN:          me : Object
 *
 * Out:         void
 *
 * Return:      void
 *
 */
void signal_waiting_listener(ListenerCBO *me)
{
    int ret = 0;
    int pid = getpid();

    if (!me) {
        ALOGE("[%s], cbo is null.", __FUNCTION__);
        return;
    }

    ret = pthread_mutex_lock(&me->waitMutex);
    if (ret != 0) {
        /* Nothing we can do about this waiter, proceed */
        ALOGE("[%s], PID : %d Failed to acquire waiter mutex,"
              "skipping waiter: %s (%d) for lid : %d",
               __FUNCTION__, pid, strerror(errno), errno, me->listenerId);
        return;
    }

    if (!me->listWaitCond->empty()) {
        /* Client in the front of the queue, waiting for
        listener is freed first. On the basis of FIFO.
        */
        pthread_cond_t* cond = me->listWaitCond->front();
        me->listWaitCond->pop_front();
        pthread_cond_signal(cond);
    }
    pthread_mutex_unlock(&me->waitMutex);
}
