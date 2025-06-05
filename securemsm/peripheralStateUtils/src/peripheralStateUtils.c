/**
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "libperipheralStateUtils"
#include <log/log.h>

#include "CPeripheralAccessControl.h"
#include "CPeripheralState.h"
#include "IClientEnv.h"
#include "IOpener.h"
#include "IPeripheralState.h"
#include "IPeripheralState_invoke.h"
#include "ITzdRegister.h"
#include "TZCom.h"
#include "heap.h"
#include "minkipc.h"
#include "object.h"
#include "peripheralStateUtils.h"

#define SSGTZD_SOCKET "/dev/socket/ssgtzd"

#define PSU_COND(error_code)                                                   \
  ret = (error_code);                                                          \
  ALOGE("%s::%d err=%d", __func__, __LINE__, error_code);                      \
  goto end;

#define PSU_CHECK_COND(cond, error_code)                                       \
  if (!(cond)) {                                                               \
    PSU_COND(error_code)                                                       \
  }

typedef struct {
  int32_t refs;
  int32_t periState;
  PeripheralStateCB notifyStateCB;
  uint32_t peripheral;
  Object remoteObj;
  MinkIPC *ssgtzdClient;
  bool notifyReset;
} notifyCB;

static inline int atomic_add(int *pn, int n) {
  return __sync_add_and_fetch(pn, n); // GCC builtin
}

static int32_t CPeripheralStateCB_release(notifyCB *me) {
  if (atomic_add(&me->refs, -1) == 0) {
    if (me->notifyReset) {
      /**
       * deregisterPeripheralCB() not called by client,
       * notifying client about it.
       */
      me->notifyStateCB(me->peripheral, STATE_RESET_CONNECTION);
    }
  }
  return Object_OK;
}

static int32_t CPeripheralStateCB_retain(notifyCB *me) {
  atomic_add(&me->refs, 1);
  return Object_OK;
}

static int32_t
CPeripheralStateCB_notifyPeripheralStateChange(notifyCB *me,
                                               uint8_t state_val) {
  int32_t ret = Object_OK;
  /* Calling the registered call back */
  ALOGD("%s: Notification from TZ UID[0x%x] state[%d]\n", __func__,
        me->peripheral, state_val);
  PSU_CHECK_COND((me->notifyStateCB != NULL), Object_ERROR);

  me->notifyStateCB(me->peripheral, state_val);

end:
  return ret;
}
static IPeripheralStateCB_DEFINE_INVOKE(PeripheralStateCB_invoke,
                                        CPeripheralStateCB_, notifyCB *);

static int32_t getPeripheralStateUIDObj(MinkIPC *ssgtzdClient,
                                        Object *remoteObj) {

  int32_t ret = PRPHRL_SUCCESS;
  Object ssgtzdOpener = Object_NULL;
  Object clientEnv = Object_NULL;
  uint8_t credentials[] = {0x24, 0x53, 0x40, 0x53, 0x45, 0x25}; /** $T@TE% */

  /** Get ClientEnv Object using ssgtzd socket */
  ssgtzdClient = MinkIPC_connect(SSGTZD_SOCKET, &ssgtzdOpener);
  if (!ssgtzdClient) {
    ALOGE("MinkIPC_connect failed");
    return PRPHRL_ERROR;
  }

  ret = ITzdRegister_getTrustedClientEnv(ssgtzdOpener, "peripheralstate",
                                         strlen("peripheralstate"), credentials,
                                         sizeof(credentials), &clientEnv);
  PSU_CHECK_COND(!Object_isNull(clientEnv), PRPHRL_ERROR);

  /** clientEnv is obtained. Now, get peripheral state Object */
  ret = IClientEnv_open(clientEnv, CPeripheralState_UID, remoteObj);
  PSU_CHECK_COND(!Object_isNull(*remoteObj), ret);
  ALOGD("%s: connected to TZ PCService...", __func__);

end:
  Object_ASSIGN_NULL(clientEnv);
  Object_ASSIGN_NULL(ssgtzdOpener);
  if ((ret != PRPHRL_SUCCESS) && ssgtzdClient) {
    MinkIPC_release(ssgtzdClient);
    ssgtzdClient = NULL;
  }
  return ret;
}

void *registerPeripheralCB(uint32_t peripheral, PeripheralStateCB notifyState) {
  int ret = PRPHRL_SUCCESS;
  notifyCB *clientCtx = NULL;
  Object objNotify = Object_NULL;

  PSU_CHECK_COND((notifyState != NULL), PRPHRL_ERROR);
  /* Creating the CallBack Object */
  clientCtx = HEAP_ZALLOC_TYPE(notifyCB);
  PSU_CHECK_COND((clientCtx != NULL), PRPHRL_ERROR);

  /** Get CPeripheralState_UID object from TZ */
  ret =
      getPeripheralStateUIDObj(clientCtx->ssgtzdClient, &clientCtx->remoteObj);
  PSU_CHECK_COND((ret == PRPHRL_SUCCESS), ret);

  /* Assign callback function from clients */
  clientCtx->notifyStateCB = notifyState;
  /* Couple the peripheral UID to the ctx */
  clientCtx->peripheral = peripheral;
  /* Creating call back Object and register with TZ service */
  objNotify = (Object){PeripheralStateCB_invoke, clientCtx};

  ret = IPeripheralState_registerCallBack(clientCtx->remoteObj, peripheral,
                                          objNotify);
  PSU_CHECK_COND((ret == PRPHRL_SUCCESS), ret);
  /* After successful registration reset values for reconnection */
  clientCtx->periState = PRPHRL_SUCCESS;
  clientCtx->notifyReset = true;

  ALOGD("%s: registered to TZ PCService [%d], ret[%d]", __func__, peripheral,
        ret);

end:
  if (ret == IPeripheralState_ERROR_PERIPHERAL_NOT_FOUND ||
      ret == IOpener_ERROR_NOT_SUPPORTED) {
    ALOGD("%s: Peripheral not supported [%d]", __func__, peripheral);
    clientCtx->periState = PRPHRL_ERROR_NOT_SUPPORTED;
  } else if (ret != PRPHRL_SUCCESS) {
    if (clientCtx != NULL) {
      Object_ASSIGN_NULL(clientCtx->remoteObj);
      if (clientCtx->ssgtzdClient)
        MinkIPC_release(clientCtx->ssgtzdClient);
      HEAP_FREE_PTR(clientCtx);
      clientCtx = NULL;
    }
  }
  return clientCtx;
}

int32_t deregisterPeripheralCB(void *ctx) {

  int ret = PRPHRL_SUCCESS;
  notifyCB *clientCtx = ctx;

  PSU_CHECK_COND((clientCtx != NULL), PRPHRL_ERROR);
  /** Confirming that deregister is called by client */
  clientCtx->notifyReset = false;

  /** Free remoteObj on a successful registration will release clientCtx */
  Object_ASSIGN_NULL(clientCtx->remoteObj);
  if (clientCtx->ssgtzdClient)
    MinkIPC_release(clientCtx->ssgtzdClient);
  HEAP_FREE_PTR(clientCtx);
  clientCtx = NULL;

end:
  return ret;
}

int32_t getPeripheralState(void *ctx) {
  int ret = PRPHRL_SUCCESS;
  uint8_t state = 0;
  notifyCB *clientCtx = ctx;

  PSU_CHECK_COND((clientCtx != NULL), PRPHRL_ERROR);
  if (clientCtx->periState == PRPHRL_ERROR_NOT_SUPPORTED) {
    return STATE_NONSECURE;
  }

  PSU_CHECK_COND(!Object_isNull(clientCtx->remoteObj), PRPHRL_ERROR);

  ret = IPeripheralState_getPeripheralState(clientCtx->remoteObj,
                                            clientCtx->peripheral, &state);
  PSU_CHECK_COND(ret == PRPHRL_SUCCESS, PRPHRL_ERROR);
  ret = state;

end:
  return ret;
}
