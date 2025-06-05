/********************************************************************
 Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef _ConnEventHandler_H
#define _ConnEventHandler_H

#include "VmOsal.h"
#include "object.h"

#if defined (__cplusplus)
extern "C" {
#endif

#define EVENT_CONN_CLOSE       (0x00000020u)
#define EVENT_CONN_CRASH       (EVENT_CONN_CLOSE - 1)
#define EVENT_CONN_DELETE      (EVENT_CONN_CLOSE - 2)
#define EVENT_CONN_DETACH      (EVENT_CONN_CLOSE - 3)
#define EVENT_CONN_UNKNOWN     (0x0000F000u)

#define isCloseEvent(event)    (((event) & EVENT_CONN_CLOSE) != 0)

typedef void (*HandlerFunc)(int32_t data);

typedef struct ConnEventHandler ConnEventHandler;

bool isConnEventHandler(Object obj);
int32_t ConnEventHandler_new(HandlerFunc func, Object *objOut);

#if defined (__cplusplus)
}
#endif

#endif // _ConnEventHandler_H

