/********************************************************************
 Copyright (c) 2022 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef _CONNECTIONEVENTRELEASER_H
#define _CONNECTIONEVENTRELEASER_H

#include "object.h"
#include "qlist.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct ConnEventReleaser {
  QNode node;
  int refs;
  Object handler;
  Object subReleaser;
} ConnEventReleaser;

int32_t ConnEventReleaser_new(Object handler, Object subReleaser, Object *objOut);

#if defined (__cplusplus)
}
#endif

#endif // _CONNECTIONEVENTRELEASER_H
