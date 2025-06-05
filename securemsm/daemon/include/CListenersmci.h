/********************************************
Copyright (c) 2022-2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/
#ifndef __CLISTENERCBO_H
#define __CLISTENERCBO_H

#include <stdint.h>
#include "object.h"
#include "listenerMngr.h"
#include <list>

typedef int (*dispatchEntry)(void*,size_t);

typedef struct {
  int refs;
  size_t dmaBufLen;
  int listenerId;
  struct dmaBufManager dmaBufMgr;
  Object smo;
  dispatchEntry smciDispatchFunc;
  pthread_mutex_t waitMutex;
  std::list <pthread_cond_t *> *listWaitCond;
  bool listenerBusy;
} ListenerCBO;

int32_t CListenerCBO_new(Object *objOut, Object smo, struct listenerServices *listener);

/* Description: The TA/QSEE waiting in the queue for listener are freed in FIFO order
 *              so that the listener can be accessed.
 *
 * IN:          me : Object
 *
 * Out:         void
 *
 * Return type: void
 */
void signal_waiting_listener(ListenerCBO *me);


#endif // __CLISTENERCBO_H
