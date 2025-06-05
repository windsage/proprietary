/******************************************************************************
#  Copyright (c) 2022, 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "framework/Log.h"
#include "config_aidl_service.h"
#include "config_aidl_service_utils.h"
#include "UnSolMessages/RadioConfigClientConnectedMessage.h"

#include <cutils/properties.h>

#define PERSIST_VENDOR_RADIO_DSDS_TO_SS  "persist.vendor.radio.dsds_to_ss"
#define FEATURE_DISABLED  0

qcril_instance_id_e_type IRadioConfigImpl::getInstanceId() {
  return mInstanceId;
}

std::shared_ptr<IRadioConfigContext> IRadioConfigImpl::getContext(uint32_t serial) {
  return std::make_shared<IRadioConfigContext>(mInstanceId, serial);
}

std::shared_ptr<aidlconfig::IRadioConfigResponse> IRadioConfigImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mResponseCb;
}

std::shared_ptr<aidlconfig::IRadioConfigIndication> IRadioConfigImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

void IRadioConfigImpl::clearCallbacks_nolock() {
  mIndicationCb = nullptr;
  mResponseCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

void IRadioConfigImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void IRadioConfigImpl::deathNotifier(void* /*cookie*/) {
  QCRIL_LOG_DEBUG("IRadioConfig::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IRadioConfigImpl* iRadioConfigImpl = static_cast<IRadioConfigImpl*>(cookie);
  if (iRadioConfigImpl != nullptr) {
    iRadioConfigImpl->deathNotifier(cookie);
  }
}

IRadioConfigImpl::IRadioConfigImpl(qcril_instance_id_e_type instance): mInstanceId(instance) {}

IRadioConfigImpl::~IRadioConfigImpl(){}

void IRadioConfigImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlconfig::IRadioConfigResponse>& in_radioConfigResponse,
    const std::shared_ptr<aidlconfig::IRadioConfigIndication>& in_radioConfigIndication) {
  QCRIL_LOG_DEBUG("IRadioConfig::setResponseFunctions_nolock");
  mResponseCb = in_radioConfigResponse;
  mIndicationCb = in_radioConfigIndication;
  auto msg = std::make_shared<rildata::RadioConfigClientConnectedMessage>();
  if (msg != nullptr) {
    QCRIL_LOG_DEBUG("IRadioConfig aidl: broadcasting client connected");
    msg->broadcast();
  }
  else {
    QCRIL_LOG_DEBUG("IRadioConfig aidl: failed to allocate RadioConfigClientConnectedMessage");
  }
}

::ndk::ScopedAStatus IRadioConfigImpl::setResponseFunctions(
    const std::shared_ptr<aidlconfig::IRadioConfigResponse>& in_radioConfigResponse,
    const std::shared_ptr<aidlconfig::IRadioConfigIndication>& in_radioConfigIndication) {
  QCRIL_LOG_DEBUG("IRadioConfig::setResponseFunctions");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mResponseCb != nullptr) {
    AIBinder_unlinkToDeath(mResponseCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
  }
  setResponseFunctions_nolock(in_radioConfigResponse, in_radioConfigIndication);
  if (mResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(mResponseCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

// Send Response Functions

/**
 * @param modemReducedFeatureSet : True indicates modem
 *        does NOT support some features
 */

void IRadioConfigImpl::sendResponseForGetHalDeviceCapabilities(int32_t in_serial,
    RIL_Errno errorCode, bool modemReducedFeatureSet) {

  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadiConfig: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, in_serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("getHalDeviceCapabilities: serial=%d, error=%d, modemReducedFeatureSet= %s ",
      in_serial, errorCode, modemReducedFeatureSet ? "true":"false");
  auto ret = respCb->getHalDeviceCapabilitiesResponse(resp, modemReducedFeatureSet);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

/**
 * @param numOfLiveModems <int8_t> indicates the number of live modems i.e. modems that
 *        are enabled and actively working as part of a working connectivity stack
 */

void IRadioConfigImpl::sendResponseForGetNumOfLiveModems(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<qcril::interfaces::ModemsConfigResp> result) {

  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadiConfig: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, in_serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  int8_t numOfLiveModems{0};
  if ( errorCode == RIL_E_SUCCESS && result) numOfLiveModems = result->numOfModems;

  QCRIL_LOG_DEBUG("getNumOfLiveModems: serial=%d, error=%d, numOfLiveModems=%d",
      in_serial, errorCode, numOfLiveModems);
  auto ret = respCb->getNumOfLiveModemsResponse(resp, numOfLiveModems);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

/**
 * @param payload <RilPhoneCapabilityResult_t> it defines modem's capability for example
 *        how many logical modems it has, how many data connections it supports.
 */

void IRadioConfigImpl::sendResponseForGetPhoneCapability(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<qcril::interfaces::RilPhoneCapabilityResult_t> payload) {

  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadiConfig: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, in_serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  aidlconfig::PhoneCapability aidl_phoneCap{};
  if (errorCode == RIL_E_SUCCESS && payload) {
    aidl_phoneCap = convertPhoneCapabilityToAidl(payload->phoneCap);
  }

  QCRIL_LOG_DEBUG("getPhoneCapabilityResponse: serial=%d, error=%d",in_serial, errorCode);
  auto ret = respCb->getPhoneCapabilityResponse(resp, aidl_phoneCap);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

/*
 * @param <RIL_UIM_SlotsStatusInfo> provides the slot status of all active
 *        and inactive SIMslots and whether card is present in the slots or not.
 */

void IRadioConfigImpl::sendResponseForGetSimSlotsStatus(int32_t in_serial,
    RIL_Errno errorCode, std::shared_ptr<RIL_UIM_SlotsStatusInfo> responseDataPtr) {
  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadiConfig: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, in_serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  std::vector<aidlconfig::SimSlotStatus> slot_status = {};
  if (errorCode == RIL_E_SUCCESS && responseDataPtr) {
    slot_status.resize(responseDataPtr->slot_status.size());
    for (uint8_t index = 0; index < responseDataPtr->slot_status.size(); index++) {
      slot_status[index] = convertUimSlotStatusToAidl(responseDataPtr->slot_status.at(index));
    }
  }

  QCRIL_LOG_DEBUG("getSimSlotsStatusResponse: serial=%d, error=%d",in_serial, errorCode);
  auto ret = respCb->getSimSlotsStatusResponse(resp, slot_status);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioConfigImpl::sendResponseForSetNumOfLiveModems(int32_t in_serial,
    RIL_Errno errorCode) {
  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
      QCRIL_LOG_DEBUG("IRadiConfig: getResponseCallback Failed");
      return;
    }

  aidlradio::RadioResponseInfo resp {
        aidlradio::RadioResponseType::SOLICITED, in_serial,
        static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("setNumOfLiveModemsResponse: serial=%d, error=%d", in_serial, errorCode);
  auto ret = respCb->setNumOfLiveModemsResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioConfigImpl::sendResponseForSetPreferredDataModem(int32_t in_serial,
  std::shared_ptr<rildata::SetPreferredDataModemResponse_t> response) {
  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadiConfig: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioError errorCode = aidlradio::RadioError::INTERNAL_ERR;
  if (response != nullptr) {
    convertRilDataResponseErrorToAidl(response->toResponseError(), errorCode);
  }
  QCRIL_LOG_DEBUG("setPreferredDataModemResponse: serial=%d, error=%d", in_serial, errorCode);

  aidlradio::RadioResponseInfo resp = {
    .type = aidlradio::RadioResponseType::SOLICITED,
    .serial = in_serial,
    .error = errorCode
  };

  QCRIL_LOG_DEBUG("setPreferredDataModemResponse: serial=%d, error=%d", in_serial, errorCode);
  auto ret = respCb->setPreferredDataModemResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioConfigImpl::sendResponseForSetSimSlotsMapping(int32_t in_serial,
    RIL_Errno errorCode) {
  std::shared_ptr<aidlconfig::IRadioConfigResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
      QCRIL_LOG_DEBUG("IRadioConfig: getResponseCallback Failed");
      return;
  }

  QCRIL_LOG_DEBUG("setSimSlotsMappingResponse: serial=%d, error=%d", in_serial, errorCode);

  aidlradio::RadioResponseInfo responseInfo {
        aidlradio::RadioResponseType::SOLICITED, in_serial,
        static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->setSimSlotsMappingResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

// Config Request APIs

/**
 * Gets the available Radio Hal capabilities on the current device.
 * This is called once per device boot up.
 * @param serial Serial number of request
 */

::ndk::ScopedAStatus IRadioConfigImpl::getHalDeviceCapabilities(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IRadiConfig: getHalDeviceCapabilities: serial=%d", in_serial);
  sendResponseForGetHalDeviceCapabilities(in_serial, RIL_E_SUCCESS, true);
  return ndk::ScopedAStatus::ok();
}

/**
 * Get the number of live modems (i.e modems that are
 * enabled and actively working as part of a working telephony stack)
 * @param serial Serial number of request
 */

::ndk::ScopedAStatus IRadioConfigImpl::getNumOfLiveModems(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IRadiConfig: getNumOfLiveModems: serial=%d", in_serial);
  std::shared_ptr<IRadioConfigContext> ctx = this->getContext(in_serial);
  auto msg = std::make_shared<QcRilRequestGetModemsConfigMessage>(ctx);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::ModemsConfigResp> result = nullptr;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            result = std::static_pointer_cast<qcril::interfaces::ModemsConfigResp>(resp->data);
          }
          sendResponseForGetNumOfLiveModems(in_serial, errorCode, result);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetNumOfLiveModems(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

/**
 * Request current phone capability
 * @param serial Serial number of request
 */

::ndk::ScopedAStatus IRadioConfigImpl::getPhoneCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IRadiConfig: getPhoneCapability: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestGetPhoneCapabilityMessage>();
  if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<qcril::interfaces::RilPhoneCapabilityResult_t> phoneCapability{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              phoneCapability =
                  std::static_pointer_cast<qcril::interfaces::RilPhoneCapabilityResult_t>(
                      resp->data);
            }
            sendResponseForGetPhoneCapability(in_serial, errorCode, phoneCapability);
      });
      msg->setCallback(&cb);
      msg->dispatch();
  } else {
      sendResponseForGetPhoneCapability(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioConfigImpl::getSimSlotsStatus(int32_t in_serial) {
  QCRIL_LOG_DEBUG("IRadioConfig: getSimSlotsStatus: serial=%d", in_serial);
  auto msg = std::make_shared<UimGetSlotStatusRequestMsg>();
  if (msg) {
      GenericCallback<RIL_UIM_SlotsStatusInfo> cb(
          [this, in_serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SlotsStatusInfo> responseDataPtr) -> void
      {
          RIL_Errno  errorCode = RIL_E_INTERNAL_ERR;
          if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS)
          {
            errorCode = static_cast<RIL_Errno>(responseDataPtr->err);
          }
          sendResponseForGetSimSlotsStatus(in_serial, errorCode, responseDataPtr);
      });
      msg->setCallback(&cb);
      msg->dispatch();
  }
  else {
    sendResponseForGetSimSlotsStatus(in_serial,
      RIL_E_NO_MEMORY,nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioConfigImpl::setNumOfLiveModems(int32_t in_serial, int8_t in_numOfLiveModems) {
  uint8_t numOfLiveModems = in_numOfLiveModems;

  QCRIL_LOG_DEBUG("IRadiConfig: setNumOfLiveModems: serial=%d, numOfLiveModems=%d",
            in_serial, numOfLiveModems);

    // Currently, we only support switch to single/multi sim mode i.e live modems = 1 or 2
    if (numOfLiveModems != 1 && numOfLiveModems != 2) {
      QCRIL_LOG_DEBUG("Invalid liveModems arg passed");
      sendResponseForSetNumOfLiveModems(in_serial, RIL_E_INVALID_ARGUMENTS);
      return ndk::ScopedAStatus::ok();
    }

    // If DSDS to SS feature enabled, send success response immediately without flashing MBN
    int32_t dsdsToSsConfig = property_get_int32(PERSIST_VENDOR_RADIO_DSDS_TO_SS, FEATURE_DISABLED);
    if (dsdsToSsConfig > 0) {
        QCRIL_LOG_DEBUG("DSDS to SS feature enabled, send success response immediately");
        sendResponseForSetNumOfLiveModems(in_serial, RIL_E_SUCCESS);
        return ndk::ScopedAStatus::ok();
    }

    std::shared_ptr<IRadioConfigContext> ctx = this->getContext(in_serial);
    auto msg = std::make_shared<QcRilRequestSetModemsConfigMessage>(
            ctx, numOfLiveModems);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, in_serial](std::shared_ptr<Message> /* msg */, Message::Callback::Status status,
                           std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
              RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
              if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
                  errorCode = resp->errorCode;
              }
              sendResponseForSetNumOfLiveModems(in_serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
        sendResponseForSetNumOfLiveModems(in_serial, RIL_E_NO_MEMORY);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioConfigImpl::setPreferredDataModem(int32_t in_serial, int8_t in_modemId) {
  QCRIL_LOG_DEBUG("IRadioConfig:: setPreferredDataModem serial=%d modemId=%d", in_serial, in_modemId);

  if (in_modemId != 0 && in_modemId != 1) {
      rildata::SetPreferredDataModemResponse_t errorResponse = {
        .errCode = rildata::SetPreferredDataModemResult_t::INVALID_ARG
      };
      sendResponseForSetPreferredDataModem(in_serial,
                         std::make_shared<rildata::SetPreferredDataModemResponse_t>(errorResponse));
      return ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<rildata::SetPreferredDataModemRequestMessage>(in_modemId);
  if (msg) {
    auto cb = std::bind(&IRadioConfigImpl::sendResponseForSetPreferredDataModem, this, in_serial, std::placeholders::_3);
    GenericCallback<rildata::SetPreferredDataModemResponse_t> responseCb(cb);
    msg->setCallback(&responseCb);
    msg->dispatch();
  } else {
    rildata::SetPreferredDataModemResponse_t errorResponse = {
      .errCode = rildata::SetPreferredDataModemResult_t::DDS_SWITCH_FAILED
    };
    sendResponseForSetPreferredDataModem(in_serial, std::make_shared<rildata::SetPreferredDataModemResponse_t>(errorResponse));
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioConfigImpl::setSimSlotsMapping(int32_t in_serial, const std::vector<aidlconfig::SlotPortMapping>& in_slotMap) {
  QCRIL_LOG_DEBUG("IRadioConfig: setSimSlotsMapping: serial=%d", in_serial);
  std::vector<RIL_UIM_SwitchSlot> slotMap{};

  //TODO Once MEP supported from below layers pass the port info along with physical slotId
  if (!in_slotMap.empty()) {
    slotMap.resize(in_slotMap.size());
    for (uint8_t index = 0; index < in_slotMap.size(); index++) {
      slotMap[index].physicalSlotId = in_slotMap.at(index).physicalSlotId;
      slotMap[index].portId = in_slotMap.at(index).portId;
    }
  }

  auto msg = std::make_shared<UimSwitchSlotPortRequestMsg>(slotMap);
  if (msg) {
    GenericCallback<RIL_UIM_Errno> cb(
      [this, in_serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                      std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void
      {
         RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
         if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS)
         {
           errorCode = static_cast<RIL_Errno>(*(responseDataPtr.get()));
         }
         sendResponseForSetSimSlotsMapping(in_serial, errorCode);
     });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
      sendResponseForSetSimSlotsMapping(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

// Send Indication Functions

void IRadioConfigImpl::sendSlotStatusIndication(const std::shared_ptr<UimSlotStatusInd> msg){

    auto indCb = getIndicationCallback();
    QCRIL_LOG_INFO("indCb: %s", indCb ? "valid" : "invalid");
    if (indCb && msg) {
      std::vector<aidlconfig::SimSlotStatus> slot_status = {};

      slot_status.resize(msg->get_status().size());

      for (uint8_t index = 0; index < slot_status.size(); index++)
      {
        slot_status[index] = convertUimSlotStatusToAidl(msg->get_status().at(index));
      }
      QCRIL_LOG_DEBUG("UNSOL: simSlotsStatusChanged");
      auto ret = indCb->simSlotsStatusChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, slot_status);

      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
