/******************************************************************************
#  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#  All rights reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "interfaces/nas/RilRequestSetMsimPreferenceMessage.h"
#include "request/GetDataPPDataCapabilityMessage.h"
#include "request/UpdateDataPPDataUIOptionMessage.h"
#include "interfaces/common.h"
#include "interfaces/ims/QcRilRequestImsFeatureSupportedSyncMessage.h"

#include "framework/Log.h"
#include "qti_radio_config_aidl_service.h"

qcril_instance_id_e_type IQtiRadioConfigImpl::getInstanceId() {
  return mInstanceId;
}

std::shared_ptr<IQtiRadioConfigContext> IQtiRadioConfigImpl::getContext(uint32_t serial) {
  return std::make_shared<IQtiRadioConfigContext>(mInstanceId, serial);
}

std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> IQtiRadioConfigImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mResponseCb;
}

std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication> IQtiRadioConfigImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

void IQtiRadioConfigImpl::clearCallbacks_nolock() {
  mIndicationCb = nullptr;
  mResponseCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

void IQtiRadioConfigImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void IQtiRadioConfigImpl::deathNotifier(void* /*cookie*/) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IQtiRadioConfigImpl* iQtiRadioConfigImpl = static_cast<IQtiRadioConfigImpl*>(cookie);
  if (iQtiRadioConfigImpl != nullptr) {
    iQtiRadioConfigImpl->deathNotifier(cookie);
  }
}

IQtiRadioConfigImpl::IQtiRadioConfigImpl(qcril_instance_id_e_type instance): mInstanceId(instance) {}

IQtiRadioConfigImpl::~IQtiRadioConfigImpl(){}

void IQtiRadioConfigImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse>& in_qtiRadioConfigResponse,
    const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication>& in_qtiRadioConfigIndication) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig::setResponseFunctions_nolock");
  mResponseCb = in_qtiRadioConfigResponse;
  mIndicationCb = in_qtiRadioConfigIndication;
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::setCallbacks(
    const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse>& in_qtiRadioConfigResponse,
    const std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigIndication>& in_qtiRadioConfigIndication) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig::setResponseFunctions");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
#ifndef QMI_RIL_UTF
  if (mResponseCb != nullptr) {
    AIBinder_unlinkToDeath(mResponseCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
  }
#endif

  setResponseFunctions_nolock(in_qtiRadioConfigResponse, in_qtiRadioConfigIndication);

#ifndef QMI_RIL_UTF
  if (mResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(mResponseCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }
#endif
  return ndk::ScopedAStatus::ok();
}

// Send Response Functions

/**
 * @param status : True indicates device is in secure mode
 */

void IQtiRadioConfigImpl::sendResponseForGetSecureModeStatus(int32_t in_serial,
    RIL_Errno errorCode, bool status) {

  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  QCRIL_LOG_DEBUG("getSecureModeStatusResponse: serial=%d, error=%d, status= %s ",
      in_serial, errorCode, status ? "true":"false");
  auto ret = respCb->getSecureModeStatusResponse(in_serial, static_cast<int32_t>(errorCode), status);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IQtiRadioConfigImpl::sendResponseForSetMsimPreference(int32_t in_serial,
    RIL_Errno errorCode) {

  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  QCRIL_LOG_DEBUG("setMsimPreferenceResponse: serial=%d, error=%d",
      in_serial, errorCode);
  auto ret = respCb->setMsimPreferenceResponse(in_serial, static_cast<int32_t>(errorCode));
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IQtiRadioConfigImpl::sendResponseForGetSimTypeInfo(
    int32_t in_serial, RIL_Errno errorCode, std::shared_ptr<RIL_UIM_GetSimTypeResp> respData) {
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  std::vector<aidlqtiradioconfig::SimTypeInfo> simTypeInfo{};
  if (errorCode == RIL_E_SUCCESS && (respData != nullptr)) {
    aidlqtiradioconfig::utils::convertGetSimTypeInfoResponse(simTypeInfo, respData);
  }

  QCRIL_LOG_DEBUG("getSimTypeInfoResponse: serial=%d, error=%d", in_serial, errorCode);
  auto ret = respCb->getSimTypeInfoResponse(in_serial, static_cast<int32_t>(errorCode), simTypeInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IQtiRadioConfigImpl::sendResponseForSetSimType(int32_t in_serial, RIL_Errno errorCode) {
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  QCRIL_LOG_DEBUG("setSimTypeResponse: serial=%d, error=%d", in_serial, errorCode);
  auto ret = respCb->setSimTypeResponse(in_serial, static_cast<int32_t>(errorCode));
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

/**
 * Get device secure mode status.
 * @param serial Serial number of request
 */

::ndk::ScopedAStatus IQtiRadioConfigImpl::getSecureModeStatus(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: getSecureModeStatus: serial=%d", in_serial);
  std::shared_ptr<IQtiRadioConfigContext> ctx = this->getContext(in_serial);
  auto msg = std::make_shared<RilRequestGetSecureModeStatusMessage>();
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          bool secureModeStatus = false;
          std::shared_ptr<qcril::interfaces::RilSecureModeStatus_t> secureMode {};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto secureMode =
              std::static_pointer_cast<qcril::interfaces::RilSecureModeStatus_t>(resp->data);
            // telephony is only concerned with status as secure or nonsecure.
            if (secureMode->mState == qcril::interfaces::PeripheralStatus::SECURE) {
              secureModeStatus = true;
            }
          }
          sendResponseForGetSecureModeStatus(in_serial, errorCode, secureModeStatus);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetSecureModeStatus(in_serial, RIL_E_NO_MEMORY, false);
  }
  return ndk::ScopedAStatus::ok();
}

/**
 * Set MSIM preference to either DSDA or DSDS
 * @param serial Serial number of request
 * @param pref Pereferece to set either DSDA or DSDS
 */
::ndk::ScopedAStatus IQtiRadioConfigImpl::setMsimPreference(int32_t in_serial,
    aidlqtiradioconfig::MsimPreference pref) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: setMsimPreference: serial=%d MsimPreference:%d",
      in_serial, pref);
    auto msg = std::make_shared<RilRequestSetMsimPreferenceMessage>(this->getContext(in_serial),
        aidlqtiradioconfig::utils::convert_msim_preference(pref));
    if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno rilErr = RIL_E_SYSTEM_ERR;
          if(status != Message::Callback::Status::SUCCESS) {
            QCRIL_LOG_ERROR("Message::Callback::Status : %d", status);
          } else {
            rilErr = (resp == nullptr) ? RIL_E_NO_MEMORY : resp->errorCode;
          }
          sendResponseForSetMsimPreference(in_serial, rilErr);
      });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForSetMsimPreference(in_serial, RIL_E_NO_MEMORY);
    }
    return ndk::ScopedAStatus::ok();
}

/**
 * Get SimType information.
 * @param serial Serial number of request
 */
::ndk::ScopedAStatus IQtiRadioConfigImpl::getSimTypeInfo(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: getSimTypeInfo: serial=%d ", in_serial);
  auto msg = std::make_shared<UimGetSimTypeRequestMsg>();
  if (msg) {
    GenericCallback<RIL_UIM_GetSimTypeResp> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_GetSimTypeResp> resp) -> void {
          RIL_Errno rilErr = RIL_E_SYSTEM_ERR;
          if (status != Message::Callback::Status::SUCCESS) {
            QCRIL_LOG_ERROR("Message::Callback::Status : %d", status);
          } else {
            rilErr = (resp == nullptr) ? RIL_E_NO_MEMORY : static_cast<RIL_Errno>(resp->err);
          }
          sendResponseForGetSimTypeInfo(in_serial, rilErr, resp);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetSimTypeInfo(in_serial, RIL_E_NO_MEMORY, NULL);
  }
  return ndk::ScopedAStatus::ok();
}

/**
 * Set SimType to either Physical/eSIM or iUICC
 * @param serial Serial number of request
 * @param simType to set either Physical/eSIM or iUICC
 */
::ndk::ScopedAStatus IQtiRadioConfigImpl::setSimType(
    int32_t in_serial, const std::vector<aidlqtiradioconfig::SimType>& in_simType) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: setSimType: serial=%d ", in_serial);
  if (in_simType.empty()) {
    QCRIL_LOG_ERROR("setSimType, empty sim type, serial=%d ", in_serial);
    sendResponseForSetSimType(in_serial, RIL_E_INVALID_ARGUMENTS);
    return ndk::ScopedAStatus::ok();
  }

  std::vector<uint32_t> simType{};
  aidlqtiradioconfig::utils::convertToUimSimType(simType, in_simType);

  auto msg = std::make_shared<UimSetSimTypeRequestMsg>(simType);
  if (msg) {
    GenericCallback<RIL_UIM_Errno> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
          RIL_Errno rilErr = RIL_E_SYSTEM_ERR;
          if (status != Message::Callback::Status::SUCCESS) {
            QCRIL_LOG_ERROR("Message::Callback::Status : %d", status);
          } else {
            rilErr = (responseDataPtr == nullptr)
                         ? RIL_E_NO_MEMORY
                         : static_cast<RIL_Errno>(*responseDataPtr);  // FIXME
          }
          sendResponseForSetSimType(in_serial, rilErr);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForSetSimType(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IQtiRadioConfigImpl::sendOnSecureModeIndication(const std::shared_ptr<SecureModeStateChangeIndMessage> msg){
    auto indCb = getIndicationCallback();
    QCRIL_LOG_INFO("indCb: %s", indCb ? "valid" : "invalid");
    if (indCb && msg) {
      auto ret = indCb->onSecureModeStatusChange(msg->isSecure());
      QCRIL_LOG_DEBUG("UNSOL: onSecureModeStatusChange");
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
      }
    }
}

void IQtiRadioConfigImpl::sendResponseForGetCiwlanCapability(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<rildata::CIWlanCapabilitySupport_t> resp) {
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  aidlqtiradiocommon::RadioResponseInfo responseInfo {};
  responseInfo.serial = in_serial;
  responseInfo.error = static_cast<aidlqtiradiocommon::RadioError>(errorCode);

  aidlqtiradioconfig::CiwlanCapability cap = aidlqtiradioconfig::CiwlanCapability::NONE;
  if (errorCode == RIL_E_SUCCESS && resp) {
    if (resp->cap) {
      aidlqtiradioconfig::utils::convert(resp->type, cap);
    }
  }
  QCRIL_LOG_DEBUG("getCiwlanCapabilityResponse: serial=%d, error=%d, capability=%d ",
      responseInfo.serial, responseInfo.error, cap);
  auto ret = respCb->getCiwlanCapabilityResponse(responseInfo, cap);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("IQtiRadioConfig: Unable to send response. Exception : %s",
        ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::getCiwlanCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: getCiwlanCapability: serial=%d", in_serial);
  auto msg = std::make_shared<rildata::GetCIWlanCapabilityMessage>();
    if (msg) {
      GenericCallback<rildata::CIWlanCapabilitySupport_t> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<rildata::CIWlanCapabilitySupport_t> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = RIL_E_SUCCESS;
            }
            sendResponseForGetCiwlanCapability(in_serial, errorCode, resp);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForGetCiwlanCapability(in_serial, RIL_E_NO_MEMORY, nullptr);
    }
  return ndk::ScopedAStatus::ok();
}

void IQtiRadioConfigImpl::sendResponseForGetDualDataCapability(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<bool> resp) {
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  aidlqtiradiocommon::RadioResponseInfo responseInfo {};
  responseInfo.serial = in_serial;
  responseInfo.error = static_cast<aidlqtiradiocommon::RadioError>(errorCode);

  bool cap = false;
  if (errorCode == RIL_E_SUCCESS && resp) {
    cap = *resp;
  }
  QCRIL_LOG_DEBUG("getDualDataCapabilityResponse: serial=%d, error=%d, capability=%d ",
      responseInfo.serial, responseInfo.error, cap);
  auto ret = respCb->getDualDataCapabilityResponse(responseInfo, cap);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("IQtiRadioConfig: Unable to send response. Exception : %s",
        ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::getDualDataCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: getDualDataCapability: serial=%d ", in_serial);
  auto msg = std::make_shared<rildata::GetDataPPDataCapabilityMessage>();
    if (msg) {
      GenericCallback<bool> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<bool> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = RIL_E_SUCCESS;
            }
            sendResponseForGetDualDataCapability(in_serial, errorCode, resp);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForGetDualDataCapability(in_serial, RIL_E_NO_MEMORY, nullptr);
    }
  return ndk::ScopedAStatus::ok();
}

void IQtiRadioConfigImpl::sendResponseForSetDualDataUserPreference(int32_t in_serial,
    RIL_Errno errorCode) {
  std::shared_ptr<aidlqtiradioconfig::IQtiRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: getResponseCallback Failed");
    return;
  }

  aidlqtiradiocommon::RadioResponseInfo responseInfo {};
  responseInfo.serial = in_serial;
  responseInfo.error = static_cast<aidlqtiradiocommon::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("IQtiRadioConfig: setDualDataUserPreferenceResponse: serial=%d, error=%d ",
      responseInfo.serial, responseInfo.error);
  auto ret = respCb->setDualDataUserPreferenceResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("IQtiRadioConfig: Unable to send response. Exception : %s",
        ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::setDualDataUserPreference(
        int32_t in_serial, bool userPreference) {
  QCRIL_LOG_DEBUG("IQtiRadioConfig: setDualDataUserPreference: serial=%d ", in_serial);
  auto msg = std::make_shared<rildata::UpdateDataPPDataUIOptionMessage>(userPreference);
    if (msg) {
      GenericCallback<RIL_Errno> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS) {
              if (resp) {
                errorCode = *resp;
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
            sendResponseForSetDualDataUserPreference(in_serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForSetDualDataUserPreference(in_serial, RIL_E_NO_MEMORY);
    }
  return ndk::ScopedAStatus::ok();
}

void IQtiRadioConfigImpl::sendCiwlanCapabilityChanged(
    std::shared_ptr<rildata::DataCapabilityChangeIndMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("IQtiRadioConfig: Indication null : %s", ind ? "false" : "true");
  if (ind && msg) {
    aidlqtiradioconfig::CiwlanCapability cap = aidlqtiradioconfig::CiwlanCapability::NONE;
    if (msg->getCIWlanSupported()) {
      aidlqtiradioconfig::utils::convert(msg->getCIWlanCapabilityType(), cap);
    }
    QCRIL_LOG_DEBUG("IQtiRadioConfig: UNSOL: onCiwlanCapabilityChanged: ciwlan capability=%d ",
        cap);
    auto ret = ind->onCiwlanCapabilityChanged(cap);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("IQtiRadioConfig: Unable to send indication. Exception : %s",
          ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IQtiRadioConfigImpl::sendDualDataCapabilityChanged(
    std::shared_ptr<rildata::DataPPDataCapabilityChangeIndMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("IQtiRadioConfig: Indication null : %s ", ind ? "false" : "true");
  if (ind && msg) {
    QCRIL_LOG_DEBUG("IQtiRadioConfig: UNSOL: onDualDataCapabilityChanged: dual data capability=%d ",
        msg->getDataPPDataSupported());
    auto ret = ind->onDualDataCapabilityChanged(msg->getDataPPDataSupported());
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("IQtiRadioConfig: Unable to send indication. Exception : %s",
          ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IQtiRadioConfigImpl::sendDualDataRecommendation(
    std::shared_ptr<rildata::DataSubRecommendationIndMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("IQtiRadioConfig: Indication null : %s ", ind ? "false" : "true");
  if (ind && msg) {
    aidlqtiradioconfig::DualDataRecommendation rec {};
    aidlqtiradioconfig::utils::convert(msg->getDataSubRecommendation(), rec);
    QCRIL_LOG_DEBUG("IQtiRadioConfig: UNSOL: onDualDataRecommendation: sub=%d, action=%d ",
        rec.sub, rec.action);
    auto ret = ind->onDualDataRecommendation(rec);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("IQtiRadioConfig: Unable to send indication. Exception : %s",
          ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IQtiRadioConfigImpl::sendResponseForGetDdsSwitchCapability(int32_t serial,
        RIL_Errno errorCode, bool support) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getDdsSwitchCapabilityResponse: serial=%d err=%d support=%d",
        serial, errorCode, support);
    aidlqtiradiocommon::RadioResponseInfo respInfo{
        .serial = serial, .error = static_cast<aidlqtiradiocommon::RadioError>(errorCode)};
    auto ret = respCb->getDdsSwitchCapabilityResponse(respInfo, support);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::getDdsSwitchCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);
  sendResponseForGetDdsSwitchCapability(in_serial, RIL_E_REQUEST_NOT_SUPPORTED, false);
  return ::ndk::ScopedAStatus::ok();
}

void IQtiRadioConfigImpl::sendResponseForSendUserPreferenceForDataDuringVoiceCall(int32_t serial,
        RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendUserPreferenceForDataDuringVoiceCallResponse: serial=%d err=%d",
        serial, errorCode);
    aidlqtiradiocommon::RadioResponseInfo respInfo {
        .serial = serial, .error = static_cast<aidlqtiradiocommon::RadioError>(errorCode)};
    respCb->sendUserPreferenceForDataDuringVoiceCallResponse(respInfo);
  }
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::sendUserPreferenceForDataDuringVoiceCall(
        int32_t in_serial, const std::vector<bool>& isAllowedOnSlot) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);
  sendResponseForSendUserPreferenceForDataDuringVoiceCall(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IQtiRadioConfigImpl::isFeatureSupported(int feature, bool* _aidl_return) {
  QCRIL_LOG_DEBUG("isFeatureSupported: %d ", feature);
  if (_aidl_return == nullptr) {
    return ndk::ScopedAStatus::ok();
  }
  switch (feature) {
    case qcril::interfaces::FeatureSupported::INTERNAL_AIDL_REORDERING :
    {
      auto msg = std::make_shared<QcRilRequestImsFeatureSupportedSyncMessage>(feature);
      *_aidl_return = isFeatureSupportedByModule(feature, msg);
      break;
    }
    default :
      *_aidl_return = false;
      break;
  }

  return ndk::ScopedAStatus::ok();
}

template <typename R>
bool IQtiRadioConfigImpl::isFeatureSupportedByModule(int feature, std::shared_ptr<R> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("Module message for feature supported is null");
    return false;
  }
  std::shared_ptr<bool> featureSupported;
  auto status = msg->dispatchSync(featureSupported);
  if (featureSupported == nullptr) {
    QCRIL_LOG_ERROR("featureSupported is null");
    return false;
  }
  QCRIL_LOG_DEBUG("featureSupported: %d, status: %d", *featureSupported, status);
  return (status == Message::Callback::Status::SUCCESS) && *featureSupported;
}
