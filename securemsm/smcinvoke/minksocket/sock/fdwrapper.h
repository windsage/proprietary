/********************************************************************
 Copyright (c) 2018, 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#ifndef __FDWRAPPER_H
#define __FDWRAPPER_H

#if defined (__cplusplus)
extern "C" {
#endif

#include "object.h"
#include "VmOsal.h"

typedef struct FdWrapper {
  int refs;
  int handle;
  int descriptor;
  Object dependency;
  Object confinement;
} FdWrapper;

Object FdWrapper_new(int fd);
FdWrapper *FdWrapperFromObject(Object obj);

bool isWrappedFd(Object obj, int* fd);

#if defined (__cplusplus)
}
#endif

#endif // __FdWrapper_H