/********************************************************************
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#ifndef __FDWRAPPER_H
#define __FDWRAPPER_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "qlist.h"

typedef struct FdWrapper {
  QNode qn;
  int refs;
  int handle;
  int descriptor;
} FdWrapper;

Object FdWrapper_new(int fd);
FdWrapper *FdWrapperFromObject(Object obj);
int32_t FdWrapper_release(FdWrapper *me);
int32_t FdWrapper_invoke(void *cxt, ObjectOp op, ObjectArg *args, ObjectCounts k);

#if defined (__cplusplus)
}
#endif

#endif // __FdWrapper_H
