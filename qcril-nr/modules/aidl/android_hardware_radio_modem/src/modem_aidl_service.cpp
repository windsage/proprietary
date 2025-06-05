/******************************************************************************
  @file    modem_aidl_service.cpp
  @brief   modem_aidl_service

  DESCRIPTION
    Implements the server side of the IRadioModem interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define TAG "RILQ"

#include <string>
#include <vector>

#include <framework/Log.h>
#include <framework/GenericCallback.h>

#include "modem_aidl_service.h"
#include "modem_aidl_service_utils.h"
#include "wake_lock_utils.h"

#include "interfaces/QcRilRequestMessage.h"

#include "interfaces/nas/RilRequestEnableModemMessage.h"
#include "interfaces/nas/RilRequestGetModemStackStatusMessage.h"
#include "interfaces/nas/RilRequestGetRadioCapMessage.h"
#include "interfaces/nas/RilRequestSetRadioCapMessage.h"
#include "interfaces/nas/RilRequestShutDownMessage.h"
#include "interfaces/dms/RilRequestGetBaseBandVersionMessage.h"
#include "interfaces/dms/RilRequestGetDeviceIdentityMessage.h"
#include "interfaces/dms/RilRequestGetModemActivityMessage.h"
#include "interfaces/dms/RilRequestRadioPowerMessage.h"
#include "interfaces/dms/RilRequestGetImeiMessage.h"

#include "modules/android/RequestRadioStateMessage.h"

using namespace qti::aidl::android::hardware::radio::modem::utils;

void IRadioModemImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlmodem::IRadioModemResponse>& radioModemResponse,
    const std::shared_ptr<aidlmodem::IRadioModemIndication>& radioModemIndication) {
  mIRadioModemResponse = radioModemResponse;
  mIRadioModemIndication = radioModemIndication;
}

void IRadioModemImpl::deathNotifier() {
  QCRIL_LOG_DEBUG("IRadioModemImpl: Client died, cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IRadioModemImpl* impl = static_cast<IRadioModemImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier();
  }
}

/*
 *   @brief
 *   Registers the callback for IRadioModem using the
 *   IRadioModemResponse and IRadioModemIndication objects
 *   being passed in by the client as parameters
 *
 */
::ndk::ScopedAStatus IRadioModemImpl::setResponseFunctions(
    const std::shared_ptr<aidlmodem::IRadioModemResponse>& in_radioModemResponse,
    const std::shared_ptr<aidlmodem::IRadioModemIndication>& in_radioModemIndication) {
  QCRIL_LOG_INFO("IRadioModemImpl::setResponseFunctions: Set client callback");

  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

    if (mIRadioModemResponse != nullptr) {
      AIBinder_unlinkToDeath(mIRadioModemResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
    }

    setResponseFunctions_nolock(in_radioModemResponse, in_radioModemIndication);

    if (mIRadioModemResponse != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mIRadioModemResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
  }

  #ifndef QMI_RIL_UTF
  auto rilConnectedMsg = std::make_shared<RilUnsolRilConnectedMessage>();
  Dispatcher::getInstance().dispatchSync(rilConnectedMsg);

  auto radioStateMsg = std::make_shared<RilUnsolRadioStateChangedMessage>();
  Dispatcher::getInstance().dispatchSync(radioStateMsg);
  #endif

  return ndk::ScopedAStatus::ok();
}

std::shared_ptr<aidlmodem::IRadioModemResponse> IRadioModemImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioModemResponse;
}

std::shared_ptr<aidlmodem::IRadioModemIndication> IRadioModemImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioModemIndication;
}

IRadioModemImpl::IRadioModemImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

IRadioModemImpl::~IRadioModemImpl() {
}

void IRadioModemImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mIRadioModemResponse = nullptr;
    mIRadioModemIndication = nullptr;

    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

::ndk::ScopedAStatus IRadioModemImpl::enableModem(int32_t in_serial, bool in_on) {
  QCRIL_LOG_DEBUG("enableModem: serial=%d on=%d", in_serial, in_on);

  auto msg = std::make_shared<RilRequestEnableModemMessage>(this->getContext(in_serial), in_on);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForEnableModem(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForEnableModem(in_serial, RIL_E_NO_MEMORY);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForEnableModem(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("enableModemResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->enableModemResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getBasebandVersion(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getBasebandVersion: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetBaseBandVersionMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetBaseBandResult_t> result {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            result =
                std::static_pointer_cast<qcril::interfaces::RilGetBaseBandResult_t>(resp->data);
          }
          this->sendResponseForGetBasebandVersion(in_serial, errorCode, result);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetBasebandVersion(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetBasebandVersion(int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetBaseBandResult_t> rilResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  std::string version {};
  if (errorCode == RIL_E_SUCCESS && rilResult) {
    version = rilResult->version;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getBasebandVersionResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getBasebandVersionResponse(responseInfo, version);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getDeviceIdentity(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getDeviceIdentity: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetDeviceIdentityMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilDeviceIdentityResult_t> result {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            result =
                std::static_pointer_cast<qcril::interfaces::RilDeviceIdentityResult_t>(resp->data);
          }
          this->sendResponseForGetDeviceIdentity(in_serial, errorCode, result);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetDeviceIdentity(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetDeviceIdentity(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilDeviceIdentityResult_t> result) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  std::string imei {};
  std::string imeisv {};
  std::string esn {};
  std::string meid {};

  if (errorCode == RIL_E_SUCCESS && result) {
    imei = result->imei;
    imeisv = result->imeisv;
    esn = result->esn;
    meid = result->meid;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getDeviceIdentityResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getDeviceIdentityResponse(responseInfo, imei, imeisv, esn, meid);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getHardwareConfig(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getHardwareConfig: serial=%d", in_serial);
  this->sendResponseForGetHardwareConfig(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetHardwareConfig(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  std::vector<aidlmodem::HardwareConfig> result {};

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getHardwareConfigResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getHardwareConfigResponse(responseInfo, result);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getModemActivityInfo(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getModemActivityInfo: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetModemActivityMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetModemActivityResult_t> result {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            result = std::static_pointer_cast<qcril::interfaces::RilGetModemActivityResult_t>(
                resp->data);
          }
          this->sendResponseForGetModemActivityInfo(in_serial, errorCode, result);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetModemActivityInfo(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetModemActivityInfo(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetModemActivityResult_t> result) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::ActivityStatsInfo activityStats {};
  if (errorCode == RIL_E_SUCCESS && result) {
    if (!convert(result->respData, activityStats)) {
      QCRIL_LOG_ERROR("RIL to AIDL conversion failed.");
      errorCode = RIL_E_SYSTEM_ERR;
    }
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getModemActivityInfoResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getModemActivityInfoResponse(responseInfo, activityStats);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getModemStackStatus(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getModemStackStatus: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetModemStackStatusMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetModemStackResult_t> rilModemStackResult {};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilModemStackResult =
                std::static_pointer_cast<qcril::interfaces::RilGetModemStackResult_t>(resp->data);
          }
          this->sendResponseForGetModemStackStatus(in_serial, errorCode, rilModemStackResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetModemStackStatus(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetModemStackStatus(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetModemStackResult_t> rilModemStackResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  bool on = false;
  if (errorCode == RIL_E_SUCCESS && rilModemStackResult) {
    on = rilModemStackResult->state;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getModemStackStatusResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getModemStackStatusResponse(responseInfo, on);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getRadioCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getRadioCapability: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetRadioCapMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            rilCapResult =
                std::static_pointer_cast<qcril::interfaces::RilRadioCapResult_t>(resp->data);
          }
          this->sendResponseForGetRadioCapability(in_serial, errorCode, rilCapResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetRadioCapability(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetRadioCapability(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilRadioCap) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioCapability radioCap {};
  if (errorCode == RIL_E_SUCCESS && rilRadioCap) {
    if (!convert(rilRadioCap->respData, radioCap)) {
      QCRIL_LOG_ERROR("RIL to AIDL conversion failed.");
      errorCode = RIL_E_SYSTEM_ERR;
    }
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getRadioCapabilityResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getRadioCapabilityResponse(responseInfo, radioCap);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::nvReadItem(int32_t in_serial, aidlmodem::NvItem in_itemId) {
  QCRIL_LOG_DEBUG("nvReadItem: serial=%d", in_serial);
  this->sendResponseForNvReadItem(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForNvReadItem(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("nvReadItemResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->nvReadItemResponse(responseInfo, "");
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::nvResetConfig(int32_t in_serial, aidlmodem::ResetNvType in_resetType) {
  QCRIL_LOG_DEBUG("nvResetConfig: serial=%d", in_serial);
  this->sendResponseForNvResetConfig(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForNvResetConfig(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("nvResetConfigResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->nvResetConfigResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::nvWriteCdmaPrl(int32_t in_serial, const std::vector<uint8_t>& in_prl) {
  QCRIL_LOG_DEBUG("nvWriteCdmaPrl: serial=%d", in_serial);
  this->sendResponseForNvWriteCdmaPrl(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForNvWriteCdmaPrl(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("nvWriteCdmaPrlResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->nvWriteCdmaPrlResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::nvWriteItem(int32_t in_serial, const aidlmodem::NvWriteItem& in_item) {
  QCRIL_LOG_DEBUG("nvWriteItem: serial=%d", in_serial);
  this->sendResponseForNvWriteItem(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForNvWriteItem(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("nvWriteItemResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->nvWriteItemResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::requestShutdown(int32_t in_serial) {
  QCRIL_LOG_DEBUG("requestShutdown: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestShutDownMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForRequestShutdown(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForRequestShutdown(in_serial, RIL_E_NO_MEMORY);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForRequestShutdown(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("requestShutdownResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->requestShutdownResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::responseAcknowledgement() {
  QCRIL_LOG_DEBUG("IRadioModem: responseAcknowledgement");
  qti::ril::utils::releaseWakeLock();
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioModemImpl::sendDeviceState(int32_t in_serial, aidlmodem::DeviceStateType in_deviceStateType, bool in_state) {
  QCRIL_LOG_DEBUG("sendDeviceState: serial=%d", in_serial);
  this->sendResponseForSendDeviceState(in_serial, RIL_E_SUCCESS);

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForSendDeviceState(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("sendDeviceStateResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->sendDeviceStateResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::setRadioCapability(int32_t in_serial, const aidlmodem::RadioCapability& in_rc) {
  QCRIL_LOG_DEBUG("setRadioCapability: serial=%d", in_serial);

  RIL_RadioCapability radioCap {};
  if (!convert(in_rc, radioCap)) {
    QCRIL_LOG_ERROR("AIDL to RIL conversion failed.");
    this->sendResponseForSetRadioCapability(in_serial, RIL_E_SYSTEM_ERR, nullptr);
    return ::ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<RilRequestSetRadioCapMessage>(this->getContext(in_serial), radioCap);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            rilCapResult =
                std::static_pointer_cast<qcril::interfaces::RilRadioCapResult_t>(resp->data);
          }
          this->sendResponseForSetRadioCapability(in_serial, errorCode, rilCapResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetRadioCapability(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForSetRadioCapability(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilRadioCap) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioCapability aidlRadioCap {};
  if (errorCode == RIL_E_SUCCESS && rilRadioCap) {
    if (!convert(rilRadioCap->respData, aidlRadioCap)) {
      QCRIL_LOG_ERROR("RIL to AIDL conversion failed.");
      errorCode = RIL_E_SYSTEM_ERR;
    }
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("setRadioCapabilityResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setRadioCapabilityResponse(responseInfo, aidlRadioCap);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::setRadioPower(int32_t in_serial,
    bool in_powerOn, bool in_forEmergencyCall, bool in_preferredForEmergencyCall) {
  QCRIL_LOG_DEBUG("setRadioPower: "
    "serial=%d,powerOn=%d,forEmergencyCall=%d,preferredForEmergencyCall=%d",
    in_serial, in_powerOn, in_forEmergencyCall, in_preferredForEmergencyCall);

  auto msg = std::make_shared<RilRequestRadioPowerMessage>(this->getContext(in_serial),
      in_powerOn, in_forEmergencyCall, in_preferredForEmergencyCall);

  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetRadioPower(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetRadioPower(in_serial, RIL_E_NO_MEMORY);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForSetRadioPower(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("setRadioPowerResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setRadioPowerResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioModemImpl::getImei(int32_t in_serial)
{
  QCRIL_LOG_DEBUG("getImei: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetImeiMessage>(this->getContext(in_serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> result{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            result = std::static_pointer_cast<qcril::interfaces::RilGetImeiInfoResult_t>(resp->data);
          }
          this->sendResponseForGetImei(in_serial, errorCode, result);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetImei(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioModemImpl::sendResponseForGetImei(
    int32_t serial,
    RIL_Errno error,
    std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> result)
{
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlmodem::ImeiInfo imeiPayload{};
  if (error == RIL_E_SUCCESS && result) {
    if (!convert(*result, imeiPayload)) {
      QCRIL_LOG_ERROR("Received INVALID IMEI type in response");
      error = RIL_E_MODEM_ERR;
    }
  }

  aidlmodem::RadioResponseInfo responseInfo{};
  responseInfo.type = aidlmodem::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlmodem::RadioError>(error);

  QCRIL_LOG_DEBUG("getImeiResponse: serial=%d, error=%d", serial, error);
  auto ret = respCb->getImeiResponse(responseInfo, imeiPayload);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s",
                    ret.getDescription().c_str());
  }
}

void IRadioModemImpl::sendModemReset(std::shared_ptr<RilUnsolModemRestartMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // ATEL will acknowledge this UNSOL, at which point the wake lock will be released
    qti::ril::utils::grabPartialWakeLock();

    QCRIL_LOG_DEBUG("UNSOL: modemReset");
    auto ret = ind->modemReset(aidlmodem::RadioIndicationType::UNSOLICITED_ACK_EXP, msg->getReason());
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioModemImpl::sendRadioCapability(std::shared_ptr<RilUnsolRadioCapabilityMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    aidlmodem::RadioCapability aidlRadioCap {};
    if (!convert(msg->getRadioCapability(), aidlRadioCap)) {
      QCRIL_LOG_ERROR("RIL to AIDL conversion failed.");
      return;
    }

    // ATEL will acknowledge this UNSOL, at which point the wake lock will be released
    qti::ril::utils::grabPartialWakeLock();

    QCRIL_LOG_DEBUG("UNSOL: radioCapabilityIndication");
    auto ret = ind->radioCapabilityIndication(
        aidlmodem::RadioIndicationType::UNSOLICITED_ACK_EXP, aidlRadioCap);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioModemImpl::sendRadioStateChanged(std::shared_ptr<RilUnsolRadioStateChangedMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    aidlmodem::RadioState radioState = aidlmodem::RadioState::UNAVAILABLE;

    auto msg = std::make_shared<RequestRadioStateMessage>(QCRIL_DEFAULT_INSTANCE_ID);
    if (msg) {
      std::shared_ptr<RIL_RadioState> rilDefaultInstRadioState;
      auto msgStatus = msg->dispatchSync(rilDefaultInstRadioState);

      if (msgStatus == Message::Callback::Status::SUCCESS && rilDefaultInstRadioState) {
        radioState = static_cast<aidlmodem::RadioState>(*rilDefaultInstRadioState);
      }
    }

    // ATEL will acknowledge this UNSOL, at which point the wake lock will be released
    qti::ril::utils::grabPartialWakeLock();

    QCRIL_LOG_DEBUG("UNSOL: radioStateChanged: radioState %d", radioState);
    auto ret = ind->radioStateChanged(aidlmodem::RadioIndicationType::UNSOLICITED_ACK_EXP,
        radioState);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioModemImpl::sendRilConnected(std::shared_ptr<RilUnsolRilConnectedMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // ATEL will acknowledge this UNSOL, at which point the wake lock will be released
    qti::ril::utils::grabPartialWakeLock();

    QCRIL_LOG_DEBUG("UNSOL: rilConnected");
    auto ret = ind->rilConnected(aidlmodem::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}
