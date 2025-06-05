/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <utils/Log.h>
#include <semaphore.h>

#include "impl_base.hpp"
#include "proxy_base.hpp"
#include "IHLOSListenerCBO.hpp"
#include "IHLOSListenerCBO_invoke.hpp"
#include "object.h"
#include "TAVMInterface.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "MultiVMsTUIClient:HLOSListenerCBO"

/**
 * @brief This class implements the HLOSListenerCBO.
 */

class CHLOSListenerCBO : public HLOSListenerCBOImplBase
{
public:
  CHLOSListenerCBO() = delete;
  explicit CHLOSListenerCBO(sem_t* sem, trusted_vm_event_t* vmStatus);
  virtual ~CHLOSListenerCBO(){};

  /*
   * ----------------------------------------------------------------------
   * Implementation of IHLOSListenerCBO
   * ----------------------------------------------------------------------
   */
  virtual int32_t onComplete();
  virtual int32_t onError(int32_t err);
  virtual int32_t handleCmd(const void* cmd_ptr,
                                size_t cmd_len,
                                IMemObject &handle_ref,
                                void* rsp_ptr,
                                size_t rsp_len,
                                size_t* rsp_lenout);
private:
  sem_t *mSessionWait = nullptr;
  trusted_vm_event_t *mVMStatus = nullptr;
};

CHLOSListenerCBO::CHLOSListenerCBO(sem_t *sem, trusted_vm_event_t* vmStatus)
{
  mSessionWait = sem;
  mVMStatus = vmStatus;
}

int32_t CHLOSListenerCBO::onComplete()
{
  ALOGD("%s ++ Session completion Notification", __func__);
  if (mSessionWait == nullptr || mVMStatus == nullptr) {
    ALOGE("Callback Object not initialized");
    return -1;
  }
  /* Notify client of session completion */
  *mVMStatus = TRUSTED_VM_EVENT_SESSION_COMPLETE;
  sem_post(mSessionWait);
  return Object_OK;
}

int32_t CHLOSListenerCBO::onError(int32_t err)
{
  ALOGD("%s ++ Session error Notification: %d:", __func__, err);
  if (mSessionWait == nullptr || mVMStatus == nullptr) {
    ALOGE("Callback Object not initialized");
    return -1;
  }
  if (err == TRUSTED_VM_OEM_ERROR) {
    ALOGE("Callback from TA with OEM Error !");
    /* Notfify the client for an OEM EVENT */
    *mVMStatus = TRUSTED_VM_EVENT_OEM_EVENT;
  } else {
    /* Notify client of session completion */
    *mVMStatus = TRUSTED_VM_EVENT_SESSION_CANCELLED;
  }

  sem_post(mSessionWait);
  return Object_OK;
}

/*****************************************************************************************************************/

/**
    @brief
    A function of CHLOSListenerCBO to handle commands from TVM, it's called by TVM TUI APP.
    Return the result of processing command.
*/
int32_t CHLOSListenerCBO::handleCmd(const void* cmd_ptr,
                              size_t cmd_len,
                              IMemObject &handle_ref,
                              void* rsp_ptr,
                              size_t rsp_len,
                              size_t* rsp_lenout)
{
    return -1;
}

/*****************************************************************************************************************/

int32_t CHLOSListenerCBO_open(Object *objOut, sem_t *sem, trusted_vm_event_t* vmStatus)
{
  if (sem == nullptr || vmStatus == nullptr) {
    ALOGE("%s: Invalid Input arguments ", __func__);
    return -1;
  }

  CHLOSListenerCBO *me = new CHLOSListenerCBO(sem, vmStatus);
  if (!me) {
    ALOGE("%s: Memory allocation for CHLOSListenerCBO failed!", __func__);
    return Object_ERROR_KMEM;
  }

  *objOut = (Object){ImplBase::invoke, me};
  return Object_OK;
}
