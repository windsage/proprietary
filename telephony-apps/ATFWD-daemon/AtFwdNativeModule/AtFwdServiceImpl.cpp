/******************************************************************************
  @file    AtFwdServiceImpl.cpp
  @brief   ATFwd Stable AIDL service implementation for interacting with the
           Android module.
  DESCRIPTION
    Implements the server side of the IAtFwd interface. Handles interactions
    between the ATFWD-daemon and the AtFwd app.

  ---------------------------------------------------------------------------

  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_NDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0
#define LOG_TAG "AtFwdServiceImpl"

#include "AtFwdServiceImpl.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <utils/Log.h>
#include <string.h>
#include "common_log.h"
#include <mutex>

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace atfwd {
namespace implementation {

/*
 * Callback invoked when the client connected to this service has died.
 */
static void deathRecpCallback(void* cookie) {
  LOGD("AtFwdServiceImpl::deathRecpCallback");
  AtFwdServiceImpl* atFwdServiceImpl = static_cast<AtFwdServiceImpl*>(cookie);
  if (atFwdServiceImpl != nullptr) {
    atFwdServiceImpl->deathNotifier(cookie);
  }
}

/*
 * Handles the client's death
 */
void AtFwdServiceImpl::deathNotifier(void* /*cookie*/) {
  LOGD("AtFwdServiceImpl::deathNotifier: Client died. Informing daemon and cleaning up callbacks");
  if (onClientDied) {
    onClientDied();
  }
  clearCallbacks();
}

/*
 * Sets a callback interface that would receive AT command indications.
 * Called from the client.
 */
::ndk::ScopedAStatus AtFwdServiceImpl::setIndicationCallback(
    const std::shared_ptr<aidlimports::IAtFwdIndication>& indicationCallback) {
  LOGD("AtFwdServiceImpl::setIndicationCallback");
  std::lock_guard<std::mutex> lock(indicationCallbackMutex);

  // Unlink previous death recipient if the callback was valid
  if (mIndicationCb != nullptr) {
    AIBinder_unlinkToDeath(mIndicationCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
  }

  mIndicationCb = indicationCallback;

  // Register the death recipient to know if the client dies
  if (mIndicationCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient != nullptr) {
      AIBinder_linkToDeath(mIndicationCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }

  return ndk::ScopedAStatus::ok();
}

/*
 * Returns the callback interface that forwards indications to the clients.
 */
std::shared_ptr<aidlimports::IAtFwdIndication> AtFwdServiceImpl::getIndicationCallback() {
  return mIndicationCb;
}

/*
 * Forwards an AT command to the client
 *
 * @return true if AIDL call is successful, false otherwise
 */
bool AtFwdServiceImpl::onAtCommandForwarded(int32_t serial, const aidlimports::AtCmd& cmd) {
  LOGI("AtFwdServiceImpl::onAtCommandForwarded - serial: %d", serial);
  std::lock_guard<std::mutex> lock(indicationCallbackMutex);
  auto indCb = this->getIndicationCallback();
  if (indCb == nullptr) {
    LOGE("AtFwdServiceImpl::onAtCommandForwarded - IndicationCb is null.");
    return false;
  }

  auto ret = indCb->onAtCommandForwarded(serial, cmd);
  if (!ret.isOk()) {
    LOGE("Unable to forward AT command. Exception : %s", ret.getDescription().c_str());
    return false;
  }
  return true;
}

/*
 * Sets the response callback method to the function pointer received as parameter.
 * This function pointer is invoked when the client has died.
 */
void AtFwdServiceImpl::setClientDeathRecipientFuncPointer(DeathRecipientCallback deathCallback) {
  LOGI("setClientDeathRecipientFuncPointer");
  onClientDied = deathCallback;
}

/*
 * Sets the response callback method to the function pointer received as parameter.
 * This function pointer is invoked when the client has sent back the AT command results.
 */
void AtFwdServiceImpl::setAtCommandResponseFuncPointer(ResponseCallback responseCallback) {
  LOGI("setAtCommandResponseFuncPointer");
  onAtCommandResponse = responseCallback;
}

/*
 * Receives the AT command results from the client, and relays it to the daemon.
 */
::ndk::ScopedAStatus AtFwdServiceImpl::sendAtCommandProcessedState(int32_t serial,
    const aidlimports::AtCmdResponse& atcmdResp) {
  LOGI("sendAtCommandProcessedState serial: %d, result: %d, response: %s",
      serial, atcmdResp.result, atcmdResp.response.c_str());
  if (onAtCommandResponse) {
    onAtCommandResponse(serial, atcmdResp);
  } else {
    LOGE("sendAtCommandProcessedState serial: %d, response callback is null");
  }
  return ndk::ScopedAStatus::ok();
}

/*
 * Resets the indication callback upon client's death
 */
void AtFwdServiceImpl::clearCallbacks() {
  LOGD("AtFwdServiceImpl::clearCallbacks");
  std::lock_guard<std::mutex> lock(indicationCallbackMutex);
  if (mIndicationCb != nullptr) {
    mIndicationCb = nullptr;
  }
  if (mDeathRecipient != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
}


} // namespace implementation
} // namespace deviceinfo
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
} // namespace aidl