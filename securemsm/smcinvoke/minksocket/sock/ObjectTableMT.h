// Copyright (c) 2017, 2021-2023 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

// ObjectTableMT
//
// An object table keeps track of local objects held by a remote domain,
// while will identify them with small non-negative integers.  The object
// table also counts reference to handles (not to be confused with the
// internal reference counting performed by each object).  When the handle
// reference count goes to zero, the object table slot is freed.
//
// The object table suports the following operations:
//
//  addObject()      Store an object in a slot in the table, yielding a handle.
//  recoverObject()  Recover an object, given a handle.
//  releaseHandle()  Decrement the reference count, and free the slot when it
//                   reaches zero.
//

#ifndef __OBJECTTABLEMT_H
#define __OBJECTTABLEMT_H

#include "VmOsal.h"
#include "object.h"
#include "heap.h"
#include "MinkTransportUtils.h"

typedef struct {
  // An array of objects held by a remote domain.
  Object *objects;
  // maximum number of the objects[]
  size_t objectsLen;
  // occupied number of the objects[]
  size_t objectsCount;
  //Mutex
  pthread_mutex_t mutex;
} ObjectTableMT;

static inline bool ObjectTableMT_isCleaned(ObjectTableMT *me)
{
  bool state = false;

  vm_osal_mutex_lock(&me->mutex);
  if ((1 == me->objectsCount) && !Object_isNull(me->objects[PRIMORDIAL_HANDLE])) {
    state = true;
  }
  vm_osal_mutex_unlock(&me->mutex);

  return state;
}

// actual step when adding primordial/generic object to objectTable
// the mutex is acquired and released in upper invocation for better performance
static inline int ObjectTableMT_add(ObjectTableMT *me, Object obj, int n)
{
  if (Object_isNull(me->objects[n])) {
    me->objects[n] = obj;
    me->objectsCount++;
    Object_retain(obj);
    return Object_OK;

  } else {
    return Object_ERROR;
  }
}

// Add primordial object to the table, of which the handle is alway the maximum
static inline int ObjectTableMT_AddPrimordial(ObjectTableMT *me, Object obj)
{
  int res;

  LOG_TRACE("add primordial obj to objectTableMT = %p, me->objects = %p, obj = %p\n",
            me, me->objects, &obj);
  vm_osal_mutex_lock(&me->mutex);
  res = ObjectTableMT_add(me, obj, PRIMORDIAL_HANDLE);
  vm_osal_mutex_unlock(&me->mutex);

  return res;
}

// Add a generic object to the table, assigning it a handle.
// On success, return the handle.
// On failure, return Object_ERROR.
static inline int ObjectTableMT_addObject(ObjectTableMT *me, Object obj)
{
  vm_osal_mutex_lock(&me->mutex);
  for (int n = GENERIC_HANDLE; n < PRIMORDIAL_HANDLE; ++n) {
    if (Object_OK == ObjectTableMT_add(me, obj, n)) {
      vm_osal_mutex_unlock(&me->mutex);
      LOG_TRACE("add obj to objectTableMT = %p, me->objects = %p, obj = %p, n = %d\n",
               me, me->objects, &obj, n);
      return n;
    }
  }
  vm_osal_mutex_unlock(&me->mutex);
  return Object_ERROR;
}

// Return the kernel object to which an outbound object forwards invokes.
// If there is no object at that slot, return Object_NULL.  Otherwise, the
// returned object has been retained, and the caller is repsonsible for
// releasing it.
static inline Object ObjectTableMT_recoverObject(ObjectTableMT *me, int h)
{
  vm_osal_mutex_lock(&me->mutex);
  if (h >= 0 && h < (int) me->objectsLen) {
    Object o = me->objects[h];
    if (!Object_isNull(o)) {
      Object_retain(o);
      vm_osal_mutex_unlock(&me->mutex);
      LOG_TRACE("recover obj from ObjectTableMT = %p, h = %d, objOut = %p\n", me,
               h, &o);
      return o;
    }
  }
  vm_osal_mutex_unlock(&me->mutex);
  return Object_NULL;
}

// actual step when releasing primordial/generic object from objectTable
// the mutex is acquired and released in upper invocation for better performance
static inline int ObjectTableMT_close(ObjectTableMT *me, int h)
{
  if (!Object_isNull(me->objects[h])) {
    LOG_TRACE("close objectTableMT = %p, me->objects = %p, h = %d\n", me, me->objects,
               h);
    Object o = me->objects[h];
    me->objects[h] = Object_NULL;
    me->objectsCount--;
    Object_release(o);
    return Object_OK;

  } else {
    return Object_ERROR;
  }
}

// release primordial from objectTable
static inline int ObjectTableMT_releasePrimordial(ObjectTableMT *me)
{
  int res;

  vm_osal_mutex_lock(&me->mutex);
  res = ObjectTableMT_close(me, PRIMORDIAL_HANDLE);
  vm_osal_mutex_unlock(&me->mutex);

  return res;
}

// release specific object (except primordial) from objectTable
static inline int ObjectTableMT_releaseHandle(ObjectTableMT *me, int h)
{
  int res = Object_ERROR;

  vm_osal_mutex_lock(&me->mutex);
  if (h >= GENERIC_HANDLE && h < PRIMORDIAL_HANDLE) {
    res = ObjectTableMT_close(me, h);
  } else {
    LOG_ERR("objectTableMT = %p, handle %d is illegal\n", me, h);
  }
  vm_osal_mutex_unlock(&me->mutex);

  return res;
}

// release all objects (except primordial) from objectTable
static inline void ObjectTableMT_closeAllHandles(ObjectTableMT *me)
{
  if (NULL == me->objects) {
    return;
  }

  vm_osal_mutex_lock(&me->mutex);
  for (int h = GENERIC_HANDLE; h < PRIMORDIAL_HANDLE; ++h) {
    ObjectTableMT_close(me, h);
  }
  vm_osal_mutex_unlock(&me->mutex);
}

static inline void ObjectTableMT_destruct(ObjectTableMT *me)
{
  vm_osal_mutex_lock(&me->mutex);
  for (int h = 0; h < me->objectsLen; h++) {
    if (!Object_isNull(me->objects[h])) {
      LOG_ERR("objectTableMT = %p, possible object leak on handle %d\n", me, h);
      ObjectTableMT_close(me, h);
#ifdef OFFTARGET
      abort();
#endif
    }
  }

  me->objectsLen = 0;
  me->objectsCount = 0;
  HEAP_FREE_PTR(me->objects);
  vm_osal_mutex_unlock(&me->mutex);

  vm_osal_mutex_deinit(&me->mutex);
}


static inline int ObjectTableMT_construct(ObjectTableMT *me, uint32_t size)
{
  me->objects = HEAP_ZALLOC_ARRAY(Object, size);
  if (me->objects == NULL) {
    me->objectsLen = 0;
    return Object_ERROR;
  }
  me->objectsLen = size;
  me->objectsCount = 0;
  vm_osal_mutex_init(&me->mutex, NULL);
  return Object_OK;
}


#endif // __OBJECTTABLEMT_H
