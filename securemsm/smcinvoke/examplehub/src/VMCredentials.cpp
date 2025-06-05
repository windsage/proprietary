/***********************************************************************
 * Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **********************************************************************/

#include "ICredentials_invoke.h"
#include "ICredentials.h"
#include "heap.h"
#include "memscpy.h"
#include "object.h"
#include "string.h"
#include "VMCredentials.h"

typedef struct {
  int32_t refs;
  uint8_t *vmuuid;
  size_t vmuuidLen;
} VMCredentials;

static
int32_t VMCredentials_retain(VMCredentials *me)
{
  me->refs++;
  return Object_OK;
}

static
int32_t VMCredentials_release(VMCredentials *me)
{
  if (--me->refs == 0) {
    HEAP_FREE_PTR(me->vmuuid);
    HEAP_FREE_PTR(me);
  }
  return Object_OK;
}

int32_t VMCredentials_getPropertyByIndex(VMCredentials *me, uint32_t index,
                                         void *name, size_t nameLen, size_t *nameLenOut,
                                         void *value, size_t valueLen, size_t *valueLenOut)
{
  /* Currently unsupported */
  return ICredentials_ERROR_NOT_FOUND;
}

int32_t VMCredentials_getValueByName(VMCredentials *me, const void *name, size_t nameLen,
                                     void *value, size_t valueLen, size_t *valueLenOut)
{
  /* We only support vmuuid now, and we use the same "name" as in QTEE */
  if (nameLen != 2 || strncmp((const char *)name, "vm", nameLen) != 0) {
    return ICredentials_ERROR_NOT_FOUND;
  }

  if (me->vmuuidLen == 0) {
    return ICredentials_ERROR_NOT_FOUND;
  }

  if (valueLen < me->vmuuidLen) {
    return ICredentials_ERROR_VALUE_SIZE;
  }

  *valueLenOut = memscpy(value, valueLen, me->vmuuid, me->vmuuidLen);

  return Object_OK;
}

static
ICredentials_DEFINE_INVOKE(VMCredentials_invoke, VMCredentials_, VMCredentials*);

int32_t VMCredentials_open(uint8_t const *vmuuid, size_t vmuuidLen, Object *objOut)
{
  VMCredentials *me = HEAP_ZALLOC_TYPE(VMCredentials);
  if (!me) {
    return Object_ERROR_KMEM;
  }
  me->refs = 1;
  if (vmuuidLen) {
    me->vmuuid = HEAP_ZALLOC_ARRAY(uint8_t, vmuuidLen);
    if (!me->vmuuid) {
      HEAP_FREE_PTR(me);
      return Object_ERROR_KMEM;;
    }
    me->vmuuidLen = memscpy(me->vmuuid, vmuuidLen, vmuuid, vmuuidLen);
  }
  *objOut = (Object){VMCredentials_invoke, me};
  return Object_OK;
}
