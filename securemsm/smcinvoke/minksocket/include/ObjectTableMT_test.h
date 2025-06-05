// Copyright (c) 2018, 2021-2022 Qualcomm Technologies, Inc.
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

#include <pthread.h>
#include <time.h>
#include "object.h"
#include "heap.h"

typedef struct {
  // An array of objects held by a remote domain.
  Object *objects;

  // An array of reference counts managed by the remote domain.
  int32_t *objectRefs;

  // Size of the objects[] and objectRefs[] arrays.
  size_t objectsLen;
  size_t objectsCount;
  //Mutex
  pthread_mutex_t mutex;
} ObjectTableMT;

static inline bool ObjectTableMT_isEmpty(ObjectTableMT *me)
{
  pthread_mutex_lock(&me->mutex);
  if(0 == me->objectsCount) {
    pthread_mutex_unlock(&me->mutex);
    return true;
  }

  pthread_mutex_unlock(&me->mutex);
  return false;
}

// Add an object to the table, assigning it a handle.  Set its 'ref' value
// to 1.
//
// On success, return the handle.
// On failure, return -1.
//
static inline int ObjectTableMT_addObject(ObjectTableMT *me, Object obj)
{
  pthread_mutex_lock(&me->mutex);
  for (int n = 0; n < (int) me->objectsLen; ++n) {
    if (Object_isNull(me->objects[n])) {
      me->objectRefs[n] = 1;
      me->objects[n] = obj;
      me->objectsCount++;
      Object_retain(obj);
      pthread_mutex_unlock(&me->mutex);
      return n;
    }
  }
  pthread_mutex_unlock(&me->mutex);
  return -1;
}


// Return the kernel object to which an outbound object forwards invokes.
// If there is no object at that slot, return Object_NULL.  Otherwise, the
// returned object has been retained, and the caller is repsonsible for
// releasing it.
//
static inline Object ObjectTableMT_recoverObject(ObjectTableMT *me, int h)
{
  usleep(5000);
  pthread_mutex_lock(&me->mutex);
  if (h >= 0 && h < (int) me->objectsLen) {
    Object o = me->objects[h];
    if (!Object_isNull(o)) {
      Object_retain(o);
      pthread_mutex_unlock(&me->mutex);
      return o;
    }
  }
  pthread_mutex_unlock(&me->mutex);
  return Object_NULL;
}

// Empty the object table entry and release the object.
// the function has to be invoked whlie holding mutex,
// just like ObjectTableMT_releaseHandle() does.
static inline void ObjectTableMT_closeHandle(ObjectTableMT *me, int h)
{

  if (!Object_isNull(me->objects[h])) {
    Object o = me->objects[h];
    me->objects[h].invoke = NULL;
    me->objectsCount--;
    Object_release(o);
    return;
  }

}

// Decrement the count in the references table, and release the associated
// object when it reaches zero.
//
static inline void ObjectTableMT_releaseHandle(ObjectTableMT *me, int h)
{
  pthread_mutex_lock(&me->mutex);
  if (h >=0 && h < (int) me->objectsLen) {
    int ref = --me->objectRefs[h];
    if (ref == 0) {
      ObjectTableMT_closeHandle(me, h);
    }
  }
  pthread_mutex_unlock(&me->mutex);
}


static inline void ObjectTableMT_closeAllHandles(ObjectTableMT *me)
{
  pthread_mutex_lock(&me->mutex);
  for (int h = 0; h < (int) me->objectsLen; ++h) {
    // _closeHandle
    if (!Object_isNull(me->objects[h])) {
      Object o = me->objects[h];
      me->objects[h].invoke = NULL;
      Object_release(o);
    }
  }
  me->objectsLen = 0;
  me->objectsCount = 0;
  pthread_mutex_unlock(&me->mutex);
}

static inline void ObjectTableMT_destruct(ObjectTableMT *me)
{
  ObjectTableMT_closeAllHandles(me);
  HEAP_FREE_PTR(me->objects);
  HEAP_FREE_PTR(me->objectRefs);
  pthread_mutex_destroy(&me->mutex);
}


static inline int ObjectTableMT_construct(ObjectTableMT *me, uint32_t size)
{
  me->objects = HEAP_ZALLOC_ARRAY(Object, size);
  me->objectRefs = HEAP_ZALLOC_ARRAY(int32_t, size);
  if (me->objects == NULL || me->objectRefs == NULL) {
    me->objectsLen = 0;
    return Object_ERROR;
  }
  me->objectsLen = size;
  me->objectsCount = 0;
  pthread_mutex_init(&me->mutex, NULL);
  return Object_OK;
}


#endif // __OBJECTTABLEMT_H
