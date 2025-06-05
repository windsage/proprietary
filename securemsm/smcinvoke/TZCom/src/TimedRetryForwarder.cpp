/*========================================================================
Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
#include <new>
#include <cerrno>
#include <stdint.h>
#include <memory>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "InvokeUtils.h"
#include "TimedRetryForwarder.h"

#ifdef ANDROID
#include <utils/Log.h>
#endif
#ifdef OE
#include <syslog.h>
#define ALOGV(...) syslog(LOG_NOTICE,"INFO:" __VA_ARGS__)
#define ALOGD(...) syslog(LOG_DEBUG,"DBG:" __VA_ARGS__)
#define ALOGE(...) syslog(LOG_ERR,"ERR:" __VA_ARGS__)
#endif

#define MS_IN_S (1000)

typedef struct {
  int32_t refs;
  size_t msTimeout;
  size_t msInterval;
  Object obj;
  pthread_mutex_t mutex;
} TimedRetryForwarder;

static long long get_clock_time_in_ms(long offset_in_ms)
{
  struct timespec current_time;
  long long time_in_ms;

  clock_gettime(CLOCK_REALTIME,&current_time);

  time_in_ms = (long long)current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000 + offset_in_ms;

  return time_in_ms;
}

static void ms_sleep(long time_in_ms)
{
  struct timespec sleep_time;

  sleep_time.tv_sec = time_in_ms / 1000;
  sleep_time.tv_nsec = (time_in_ms % 1000) * 1000000;

  nanosleep(&sleep_time, NULL);
}

static
int32_t TimedRetryForwarder_retain(TimedRetryForwarder* me)
{
  if (0 != pthread_mutex_lock(&me->mutex)) {
    ALOGE("Failed to acquire the state mutex: %s (%d)", strerror(errno), errno);
    return Object_ERROR_UNAVAIL;
  }
  me->refs++;

  pthread_mutex_unlock(&me->mutex);
  return Object_OK;
}

static
int32_t TimedRetryForwarder_release(TimedRetryForwarder* me)
{
  if (0 != pthread_mutex_lock(&me->mutex)) {
    ALOGE("Failed to acquire the state mutex: %s (%d)", strerror(errno), errno);
    return Object_ERROR_UNAVAIL;
  }
  if (--me->refs == 0) {
    ALOGD("%s:%d delete me:%llu", __func__, __LINE__, (unsigned long long)me);
    Object_ASSIGN_NULL(me->obj);
    pthread_mutex_unlock(&me->mutex);
    delete (me);
  } else {
    pthread_mutex_unlock(&me->mutex);
  }

  return Object_OK;
}

static
int32_t TimedRetryForwarder_invoke(ObjectCxt h, ObjectOp op, ObjectArg *a, ObjectCounts k)
{
  int32_t ret = Object_OK;

  TimedRetryForwarder* me = (TimedRetryForwarder*) h;

  if (ObjectOp_methodID(op) == Object_OP_release) {
    return TimedRetryForwarder_release(me);
  }

  if (ObjectOp_methodID(op) == Object_OP_retain) {
    return TimedRetryForwarder_retain(me);
  }

  if (0 != pthread_mutex_lock(&me->mutex)) {
    ALOGE("Failed to acquire the state mutex: %s (%d)", strerror(errno), errno);
    return Object_ERROR_UNAVAIL;
  }
  long long end = get_clock_time_in_ms(me->msTimeout);

  // retry loop
  do {

    // call the object
    ret = Object_invoke(me->obj, op, a, k);

    // taking a shortcut and not checking for errors.
    if (get_clock_time_in_ms(0) >= end)
      break;

    if (ret == Object_ERROR_BUSY) {
      ms_sleep(me->msInterval);
    }

  } while((get_clock_time_in_ms(0) < end) && (ret == Object_ERROR_BUSY));

  if (Object_ERROR_BUSY == ret) {
    ALOGE("%s: We got out with BUSY result, retried, but timed out.", __func__);
  }

  if (Object_isOK(ret)) {
    // and now the magic: wrap each received object in its own forwarder
    FOR_ARGS(i, k, OO) {
      if (a[i].o.invoke == me->obj.invoke) {
        // the object is remote. I'd want to check this against invoke_over_ioctl, but it's static in libMinkDescriptor
        Object forwarder = Object_NULL;
        ret = TimedRetryForwarder_new(me->msTimeout, me->msInterval, a[i].o, &forwarder);
        if (Object_isERROR(ret)) {
          ALOGE("%s: failed to create a TimedRetryForwarder, ret:%d.", __func__, ret);
          for (size_t ii = ObjectCounts_indexOO(k);
                 ii < i;
                ++ii) {
              Object_ASSIGN_NULL(a[i].o);
           }
           ret = (ret == Object_ERROR_KMEM) ? ret : Object_ERROR_UNAVAIL;
          break;
        }
        else {
          // replace in-place in the array of returned arguments
          a[i].o = forwarder;
        }
      }
    }
  }

  pthread_mutex_unlock(&me->mutex);
  return ret;
}

/*********************************************************************
 * Public interface
 * ******************************************************************/
int32_t TimedRetryForwarder_new(size_t msTimeout, size_t msInterval, OBJECT_CONSUMED Object obj, Object *objOut)
{
  TimedRetryForwarder *me = new (std::nothrow) TimedRetryForwarder;
  if (!me) {
    return Object_ERROR_KMEM;
  }
  me->msTimeout = msTimeout;
  me->msInterval = msInterval;
  // we take ownership, so the caller should release the reference it passes us
  me->obj = obj;
  me->refs = 1;
  *objOut = (Object){TimedRetryForwarder_invoke, me};
  me->mutex = PTHREAD_MUTEX_INITIALIZER;
  return Object_OK;
}
