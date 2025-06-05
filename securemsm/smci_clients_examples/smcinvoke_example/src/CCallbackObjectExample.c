/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/Log.h>

#include "CCallbackObjectExample_open.h"
#include "ICallbackObjectExample.h"
#include "ICallbackObjectExample_invoke.h"
#include "object.h"

typedef struct  {
  int refs;
} CCallbackObjectExample;

static int32_t CCallbackObjectExample_retain(CCallbackObjectExample *me)
{
  me->refs++;
  return Object_OK;
}

static int32_t CCallbackObjectExample_release(CCallbackObjectExample *me)
{
  if (--me->refs == 0) {
    free(me);
  }

  return Object_OK;
}

static int32_t CCallbackObjectExample_print(CCallbackObjectExample *me,
                                            const char *msg,
                                            size_t msgLen)
{
  (void)me;
  (void)msgLen;

  ALOGE("ICallbackObjectExample_print: %s\n", msg);

  return Object_OK;
}

static ICallbackObjectExample_DEFINE_INVOKE(CCallbackObjectExample_invoke,
                                            CCallbackObjectExample_,
                                            CCallbackObjectExample *);

int32_t CCallbackObjectExample_open(Object *objOut)
{
  CCallbackObjectExample *me = malloc(sizeof(*me));
  if (!me) {
    return Object_ERROR_MEM;
  }

  me->refs = 1;
  *objOut = (Object){CCallbackObjectExample_invoke, me};
  return Object_OK;
}
