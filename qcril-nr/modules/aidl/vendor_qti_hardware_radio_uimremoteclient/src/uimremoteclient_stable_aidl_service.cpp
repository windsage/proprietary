/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "uimremoteclient_stable_aidl_service.h"
#include <framework/Log.h>
#include <cutils/properties.h>
#include "qcril_legacy_apis.h"
#include "UimRemoteClientAidlModule.h"
#include <aidl/vendor/qti/hardware/radio/uim_remote_client/UimRemoteEventReqType.h>

#include <inttypes.h>

#undef TAG
#define TAG "RILQ"

using android::sp;

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim_remote_client {
namespace implementation {

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::getInstanceId

===========================================================================*/
int IUimRemoteServiceClientImpl::getInstanceId()
{
  return mInstanceId;
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::IUimRemoteServiceClientImpl

===========================================================================*/
IUimRemoteServiceClientImpl::IUimRemoteServiceClientImpl(UimRemoteClientAidlModule* module)
{
  mModule = module;
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::getContext

===========================================================================*/
std::shared_ptr<UimRemoteClientContext> IUimRemoteServiceClientImpl::getContext(int32_t serial)
{
  return std::make_shared<UimRemoteClientContext>(
      static_cast<qcril_instance_id_e_type>(mInstanceId), serial);
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::getResponseCallback

===========================================================================*/
std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse>
IUimRemoteServiceClientImpl::getResponseCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mResponseCb;
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::getIndicationCallback

===========================================================================*/
std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication>
IUimRemoteServiceClientImpl::getIndicationCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::clearCallbacks_nolock

===========================================================================*/
void IUimRemoteServiceClientImpl::clearCallbacks_nolock()
{
  QCRIL_LOG_DEBUG("clearCallbacks_nolock");
  mResponseCb = nullptr;
  mIndicationCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::clearCallbacks

===========================================================================*/
void IUimRemoteServiceClientImpl::clearCallbacks()
{
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::deathNotifier

===========================================================================*/
void IUimRemoteServiceClientImpl::deathNotifier(void*)
{
  QCRIL_LOG_DEBUG("IUimRemoteServiceClient::serviceDied: Client died, Cleaning up callbacks");
  clearCallbacks();
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::deathRecpCallback

===========================================================================*/
static void deathRecpCallback(void* cookie)
{
  IUimRemoteServiceClientImpl* uimRemoteServiceClientImpl =
      static_cast<IUimRemoteServiceClientImpl*>(cookie);
  if (uimRemoteServiceClientImpl != nullptr) {
    uimRemoteServiceClientImpl->deathNotifier(cookie);
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::setResponseFunctions_nolock

===========================================================================*/
void IUimRemoteServiceClientImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse>& in_responseCallback,
    const std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication>& in_indicationCallback)
{
  QCRIL_LOG_DEBUG("IUimRemoteServiceClient::setResponseFunctions_nolock");
  mResponseCb = in_responseCallback;
  mIndicationCb = in_indicationCallback;
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::setCallback

===========================================================================*/
::ndk::ScopedAStatus IUimRemoteServiceClientImpl::setCallback(
    const std::shared_ptr<aidlimports::IUimRemoteServiceClientResponse>& in_responseCallback,
    const std::shared_ptr<aidlimports::IUimRemoteServiceClientIndication>& in_indicationCallback)
{
  QCRIL_LOG_DEBUG("IUimRemoteServiceClientImpl::setCallback");

  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

  if (mResponseCb != nullptr) {
    AIBinder_unlinkToDeath(
        mResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
  }

  setResponseFunctions_nolock(in_responseCallback, in_indicationCallback);

  if (mResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(
          mResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
    }
  }

  if (mModule != nullptr) {
    if (mModule->isReady() && mIndicationCb != nullptr) {
      QCRIL_LOG_INFO("Sending UimRemoteServiceClientServiceInd");
      auto ret = mIndicationCb->uimRemoteServiceClientServiceInd(true);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
      }
    }
  }
  return ndk::ScopedAStatus::ok();
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::setInstanceId

===========================================================================*/
void IUimRemoteServiceClientImpl::setInstanceId(int instanceId)
{
  mInstanceId = instanceId;
} /* IUimRemoteServiceClientImpl::setInstanceId */

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::uimRemoteServiceClientApduReq

===========================================================================*/
::ndk::ScopedAStatus IUimRemoteServiceClientImpl::uimRemoteServiceClientApduReq(
    int32_t token,
    aidlimports::UimRemoteClientApduStatus apduStatus,
    const std::vector<uint8_t>& apduResponse)
{
  if (mModule != nullptr) {
    mModule->qcril_uim_remote_client_request_apdu(
        token, apduStatus, apduResponse.size(), apduResponse.data());
  } else {
    sendUimRemoteClientApduResponse(
        token,
        static_cast<aidlimports::UimRemoteClientApduRsp>(QCRIL_UIM_REMOTE_CLIENT_APDU_FAILURE));
  }
  return ndk::ScopedAStatus::ok();
} /* IUimRemoteServiceClientImpl::UimRemoteServiceClientApduReq */

/*===========================================================================

FUNCTION:  UimRemoteClientImpl::UimRemoteServiceClientEventReq

===========================================================================*/
::ndk::ScopedAStatus IUimRemoteServiceClientImpl::uimRemoteServiceClientEventReq(
    int32_t token, const aidlimports::UimRemoteEventReqType& eventReq)
{
  qcril_uim_remote_event_req_type event_req;

  event_req.event_type = static_cast<qcril_uim_remote_event_type>(eventReq.event);
  event_req.atr_len = eventReq.atr.size();
  event_req.atr = eventReq.atr.data();
  event_req.has_wakeup_support = eventReq.has_wakeupSupport;
  event_req.wakeup_support = eventReq.wakeupSupport;
  event_req.has_error_code = eventReq.has_errorCode;
  event_req.error_code = static_cast<qcril_uim_remote_error_cause_type>(eventReq.errorCode);
  event_req.has_transport = eventReq.has_transport;
  event_req.transport = static_cast<qcril_uim_remote_transport_type>(eventReq.transport);
  event_req.has_usage = eventReq.has_usage;
  event_req.usage = static_cast<qcril_uim_remote_usage_type>(eventReq.usage);
  event_req.has_apdu_timeout = eventReq.has_apdu_timeout;
  event_req.apdu_timeout = eventReq.apduTimeout;
  event_req.has_disable_all_polling = eventReq.has_disable_all_polling;
  event_req.disable_all_polling = eventReq.disableAllPolling;
  event_req.has_poll_timer = eventReq.has_poll_timer;
  event_req.poll_timer = eventReq.pollTimer;

  if (mModule != nullptr) {
    mModule->qcril_uim_remote_client_request_event(token, &event_req);
  } else {
    sendUimRemoteClientEventResponse(
        token,
        static_cast<aidlimports::UimRemoteClientEventRsp>(QCRIL_UIM_REMOTE_CLIENT_EVENT_FAILURE));
  }
  return ndk::ScopedAStatus::ok();
} /* UimRemoteClientImpl::UimRemoteServiceClientEventReq */

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteClientEventResponse

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteClientEventResponse(
    int32_t token, aidlimports::UimRemoteClientEventRsp eventResp)
{
  auto respCb = getResponseCallback();

  if (respCb == NULL) {
    QCRIL_LOG_ERROR("sendUimRemoteClientEventResponse responseCb is null");
    return;
  }

  QCRIL_LOG_INFO("sendUimRemoteClientEventResponse token=%d eventResp=%d", token, eventResp);

  auto ret = respCb->uimRemoteServiceClientEventResp(token, eventResp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteClientApduResponse

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteClientApduResponse(
    int32_t token, aidlimports::UimRemoteClientApduRsp apduResp)
{
  auto respCb = getResponseCallback();
  if (respCb == NULL) {
    QCRIL_LOG_ERROR("sendUimRemoteClientApduResponse responseCb is null");
    return;
  }
  QCRIL_LOG_INFO("sendUimRemoteClientApduResponse token=%d eventResp=%d", token, apduResp);
  auto ret = respCb->uimRemoteServiceClientApduResp(token, apduResp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientApduResponse */

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientApduInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientApduInd(uint32_t apduLen,
                                                                    std::vector<uint8_t> apduInd)
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientApduInd apduLen=%d", apduLen);

  if (apduLen == 0) {
    return;
  }

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientApduInd(apduInd);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientApduInd indicationCb is null");
    return;
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientCardInitStatusInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientCardInitStatusInd(
    const UimRmtCardAppInitStatusIndData* cardAppInfoData)
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientCardInitStatusInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    std::vector<aidlimports::UimRemoteClientAppInfo> tempInfoVector;

    aidlimports::UimRemoteClientCardInitStatusType cardInitStatusInd = {};

    cardInitStatusInd.numOfActiveSlots = cardAppInfoData->numOfActiveSlots;
    cardInitStatusInd.numOfApps = cardAppInfoData->numOfApps;

    QCRIL_LOG_INFO(" numOfActiveSlots : %d  numOfApps : %d",
                   cardInitStatusInd.numOfActiveSlots,
                   cardInitStatusInd.numOfApps);

    for (uint8_t i = 0; i < cardInitStatusInd.numOfApps; i++) {
      aidlimports::UimRemoteClientAppInfo tempInfo = {};
      tempInfo.appType = (aidlimports::UimRemoteClientAppType)cardAppInfoData->appInfo[i].appType;
      tempInfo.appState = (aidlimports::UimRemoteClientAppState)cardAppInfoData->appInfo[i].appState;

      QCRIL_LOG_INFO(" i : %d appType : %d  appState : %d", i, tempInfo.appType, tempInfo.appState);

      tempInfoVector.push_back(tempInfo);
    }

    cardInitStatusInd.appInfo = tempInfoVector;

    auto ret = indCb->uimRemoteServiceClientCardInitStatusInd(cardInitStatusInd);

    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }

  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientApduInd indicationCb is null");
    return;
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientConnectInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientConnectInd()
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientConnectInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientConnectInd();
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientConnectInd indicationCb is null");
    return;
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientDisconnectInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientDisconnectInd()
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientConnectInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientDisconnectInd();
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientDisconnectInd indicationCb is null");
    return;
  }
}
/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientPowerUpInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientPowerUpInd(
    bool hasTimeOut,
    int32_t timeOut,
    bool hasVoltageClass,
    aidlimports::UimRemoteClientVoltageClass powerUpVoltageClass)
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientPowerUpInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientPowerUpInd(
        hasTimeOut, timeOut, hasVoltageClass, powerUpVoltageClass);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientPowerUpInd indicationCb is null");
    return;
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientPowerDownInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientPowerDownInd(
    bool hasPowerDownMode, aidlimports::UimRemoteClientPowerDownMode powerDownMode)
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientPowerDownInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientPowerDownInd(hasPowerDownMode, powerDownMode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientPowerDownInd indicationCb is null");
    return;
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientResetInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientResetInd()
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientResetInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientResetInd();
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientResetInd indicationCb is null");
    return;
  }
}

/*===========================================================================

FUNCTION:  IUimRemoteServiceClientImpl::sendUimRemoteServiceClientServiceInd

===========================================================================*/
void IUimRemoteServiceClientImpl::sendUimRemoteServiceClientServiceInd(bool status)
{
  QCRIL_LOG_INFO("sendUimRemoteServiceClientServiceInd");

  auto indCb = getIndicationCallback();
  if (indCb) {
    auto ret = indCb->uimRemoteServiceClientServiceInd(status);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("sendUimRemoteServiceClientServiceInd indicationCb is null");
    return;
  }
}

}  // namespace implementation
}  // namespace uim_remote_client
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
