/******************************************************************************
  @file    sim_aidl_service.cpp
  @brief   sim_aidl_service

  DESCRIPTION
    Implements the server side of the IRadioSim interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------
  Copyright (c) 2018,2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define TAG "RILQ"

#include <framework/Log.h>
#include "sim_aidl_service.h"
#include "wake_lock_utils.h"

#include "sim_aidl_service_utils.h"

#include "interfaces/gstk/GstkReportSTKIsRunningMsg.h"
#include "interfaces/gstk/GstkSendEnvelopeRequestMsg.h"
#include "interfaces/gstk/GstkSendTerminalResponseRequestMsg.h"
#include "interfaces/nas/RilRequestCdmaSubscriptionMessage.h"
#include "interfaces/nas/RilRequestEnableUiccAppMessage.h"
#include "interfaces/nas/RilRequestGetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestGetUiccAppStatusMessage.h"
#include "interfaces/nas/RilRequestSetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestSetUiccSubsMessage.h"
#include "interfaces/pbm/QcRilRequestGetAdnCapacityMessage.h"
#include "interfaces/pbm/QcRilRequestGetPhonebookRecordsMessage.h"
#include "interfaces/pbm/QcRilRequestUpdatePhonebookRecordsMessage.h"
#include "interfaces/uim/UimCardPowerReqMsg.h"
#include "interfaces/uim/UimChangeSimPinRequestMsg.h"
#include "interfaces/uim/UimEnterDePersoRequestMsg.h"
#include "interfaces/uim/UimEnterSimPinRequestMsg.h"
#include "interfaces/uim/UimEnterSimPukRequestMsg.h"
#include "interfaces/uim/UimGetCardStatusRequestMsg.h"
#include "interfaces/uim/UimGetFacilityLockRequestMsg.h"
#include "interfaces/uim/UimGetFacilityLockRequestMsg.h"
#include "interfaces/uim/UimGetImsiRequestMsg.h"
#include "interfaces/uim/UimSIMAuthenticationRequestMsg.h"
#include "interfaces/uim/UimSIMCloseChannelRequestMsg.h"
#include "interfaces/uim/UimSIMCloseChannelSessionReqMsg.h"
#include "interfaces/uim/UimSIMIORequestMsg.h"
#include "interfaces/uim/UimSIMOpenChannelRequestMsg.h"
#include "interfaces/uim/UimSetFacilityLockRequestMsg.h"
#include "interfaces/uim/UimTransmitAPDURequestMsg.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/voice/QcRilRequestGetCallBarringMessage.h"
#include "interfaces/voice/QcRilRequestSetSupsServiceMessage.h"
#include "request/SetCarrierInfoImsiEncryptionMessage.h"
#include "interfaces/uim/QcrilSimClientConnected.h"

#ifdef QMI_RIL_UTF
/* For sending the SIM Status, if the UIM client connectes later */
static std::shared_ptr<UimSimStatusChangedInd> s_simStatusMsg = nullptr;
#endif

void IRadioSimImpl::notifySimClientConnected() {
  auto msg = std::make_shared<QcrilSimClientConnected>();
  if (msg != nullptr) {
    msg->broadcast();
  }
}

void IRadioSimImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlsim::IRadioSimResponse>& radioSimResponse,
    const std::shared_ptr<aidlsim::IRadioSimIndication>& radioSimIndication) {
  mIRadioSimResponse = radioSimResponse;
  mIRadioSimIndication = radioSimIndication;
}

void IRadioSimImpl::deathNotifier() {
  QCRIL_LOG_DEBUG("IRadioSimImpl: Client died, cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IRadioSimImpl* impl = static_cast<IRadioSimImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier();
  }
}

/*
 *   @brief
 *   Registers the callback for IRadioSim using the
 *   IRadioSimResponse and IRadioSimIndication objects
 *   being passed in by the client as parameters
 *
 */
::ndk::ScopedAStatus IRadioSimImpl::setResponseFunctions(
    const std::shared_ptr<aidlsim::IRadioSimResponse>& radioSimResponse,
    const std::shared_ptr<aidlsim::IRadioSimIndication>& radioSimIndication) {
  QCRIL_LOG_INFO("IRadioSimImpl::setResponseFunctions: Set client callback");

  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

    if (mIRadioSimResponse != nullptr) {
      AIBinder_unlinkToDeath(mIRadioSimResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
    }

    setResponseFunctions_nolock(radioSimResponse, radioSimIndication);

    if (mIRadioSimResponse != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mIRadioSimResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
  }
#ifdef QMI_RIL_UTF
  if (radioSimIndication != nullptr) {
    if (s_simStatusMsg) {
      sendSimStatusChanged(s_simStatusMsg);
      s_simStatusMsg = nullptr;
    }
  }
#endif

  if(radioSimResponse != nullptr && radioSimIndication != nullptr) {
    notifySimClientConnected();
  }
  return ndk::ScopedAStatus::ok();
}

std::shared_ptr<aidlsim::IRadioSimResponse> IRadioSimImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioSimResponse;
}

std::shared_ptr<aidlsim::IRadioSimIndication> IRadioSimImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioSimIndication;
}

IRadioSimImpl::IRadioSimImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

IRadioSimImpl::~IRadioSimImpl() {
}

void IRadioSimImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mIRadioSimResponse = nullptr;
    mIRadioSimIndication = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void IRadioSimImpl::sendResponseForChangeIccPin2ForApp(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
  std::shared_ptr<aidlsim::IRadioSimResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  int retries{ -1 };
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    retries = respData->no_of_retries;
  }
  auto ret = respCb->changeIccPin2ForAppResponse(responseInfo, retries);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForChangeIccPinForApp(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  int retries{ -1 };
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    retries = respData->no_of_retries;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("changeIccPinForAppResponse: serial=%d", serial);
    auto ret = respCb->changeIccPinForAppResponse(responseInfo, retries);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForAreUiccApplicationsEnabled(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetUiccAppStatusResult_t> result) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };

  bool state = false;
  if (errorCode == RIL_E_SUCCESS && result) {
    state = result->state;
  }
  QCRIL_LOG_DEBUG("areUiccApplicationsEnabledResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->areUiccApplicationsEnabledResponse(responseInfo, state);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForEnableUiccApplications(int32_t serial, RIL_Errno errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }
  QCRIL_LOG_DEBUG("enableUiccApplicationsResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->enableUiccApplicationsResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForGetAllowedCarriers(int32_t serial, RIL_Errno errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }
  aidlsim::CarrierRestrictions carriers{};
  aidlsim::SimLockMultiSimPolicy multiSimPolicy{ aidlsim::SimLockMultiSimPolicy::NO_MULTISIM_POLICY };
  QCRIL_LOG_DEBUG("getAllowedCarriersResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getAllowedCarriersResponse(responseInfo, carriers, multiSimPolicy);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForGetCdmaSubscription(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionResult_t> rilCdmaSubResult) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };

  std::string mdns{};
  std::string hSid{};
  std::string hNid{};
  std::string min{};
  std::string prl{};
  if (errorCode == RIL_E_SUCCESS && rilCdmaSubResult) {
    mdns = rilCdmaSubResult->mdn;
    hSid = rilCdmaSubResult->hSid;
    hNid = rilCdmaSubResult->hNid;
    min = rilCdmaSubResult->min;
    prl = rilCdmaSubResult->prl;  // FIXME assigning ??
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCdmaSubscriptionResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCdmaSubscriptionResponse(responseInfo, mdns, hSid, hNid, min, prl);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForGetCdmaSubscriptionSource(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionSourceResult_t> srcResult) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };

  aidlsim::CdmaSubscriptionSource pref = aidlsim::CdmaSubscriptionSource::RUIM_SIM;
  if (errorCode == RIL_E_SUCCESS && srcResult) {
    pref = static_cast<aidlsim::CdmaSubscriptionSource>(srcResult->mSource);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCdmaSubscriptionSourceResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCdmaSubscriptionSourceResponse(responseInfo, pref);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForGetFacilityLockForApp(int32_t serial, RIL_Errno errorCode,
                                                         int32_t response) {
  std::shared_ptr<aidlsim::IRadioSimResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };

  QCRIL_LOG_DEBUG("getFacilityLockForAppResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getFacilityLockForAppResponse(responseInfo, response);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForGetIccCardStatus(int32_t serial, aidlradio::RadioError errorCode,
                                                    std::shared_ptr<RIL_UIM_CardStatus> respData) {
  std::shared_ptr<aidlsim::IRadioSimResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };

  aidlsim::CardStatus cardStatus{};
  if (errorCode == aidlradio::RadioError::NONE) {
    responseInfo.error = convertGetIccCardStatusResponse(cardStatus, respData);  // FIXME
  }
  QCRIL_LOG_DEBUG("getIccCardStatusResponse: serial=%d", serial);
  auto ret = respCb->getIccCardStatusResponse(responseInfo, cardStatus);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForGetImsiForApp(int32_t serial, aidlradio::RadioError errorCode,
                                                 std::shared_ptr<RIL_UIM_IMSI_Response> respData) {
  std::shared_ptr<aidlsim::IRadioSimResponse> respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };

  std::string IMSI{};
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    IMSI = respData->IMSI;
  }
  QCRIL_LOG_DEBUG("getIMSIForAppResponse: serial=%d", serial);
  auto ret = respCb->getImsiForAppResponse(responseInfo, IMSI);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForGetSimPhonebookCapacity(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adnCountInfo) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }

  aidlsim::PhonebookCapacity capacity;
  convertRilPbCapacity(adnCountInfo, capacity);

  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };

  QCRIL_LOG_DEBUG("getSimPhonebookCapacityResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getSimPhonebookCapacityResponse(responseInfo, capacity);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForGetSimPhonebookRecords(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioSim: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  QCRIL_LOG_DEBUG("getSimPhonebookRecordsResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getSimPhonebookRecordsResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForIccCloseLogicalChannel(int32_t serial,
                                                          aidlradio::RadioError errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             errorCode };
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("iccCloseLogicalChannelResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->iccCloseLogicalChannelResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForIccIOForApp(int32_t serial, aidlradio::RadioError errorCode,
                                               std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };

  aidlsim::IccIoResult result{};
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    result.sw1 = respData->sw1;
    result.sw2 = respData->sw2;
    result.simResponse = respData->simResponse;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("iccIOForAppResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->iccIoForAppResponse(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForIccOpenLogicalChannel(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_OpenChannelResponse> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };

  std::vector<uint8_t> selectResponse{};  // FIXME
  int32_t ch_id = -1;

  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    selectResponse.resize(respData->selectResponse.size());
    for (uint32_t i = 0; i < respData->selectResponse.size(); i++) {
      selectResponse[i] = respData->selectResponse[i];
    }
    ch_id = respData->channel_id;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("iccOpenLogicalChannelResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->iccOpenLogicalChannelResponse(responseInfo, ch_id, selectResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForIccTransmitApduBasicChannel(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  aidlsim::IccIoResult result = {};  // FIXME
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    result.sw1 = respData->sw1;
    result.sw2 = respData->sw2;
    result.simResponse = respData->simResponse;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("iccTransmitApduBasicChannelResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->iccTransmitApduBasicChannelResponse(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}
void IRadioSimImpl::sendResponseForIccTransmitApduLogicalChannel(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  aidlsim::IccIoResult result = {};  // FIXME
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    result.sw1 = respData->sw1;
    result.sw2 = respData->sw2;
    result.simResponse = respData->simResponse;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("iccTransmitApduLogicalChannelResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->iccTransmitApduLogicalChannelResponse(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForReportStkServiceIsRunning(int32_t serial,
                                                             aidlradio::RadioError errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             errorCode };
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("reportStkServiceIsRunningResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->reportStkServiceIsRunningResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForRequestIccSimAuthentication(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  aidlsim::IccIoResult result{};  // FIXME
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    result.sw1 = respData->sw1;
    result.sw2 = respData->sw2;
    result.simResponse = respData->simResponse;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("requestIccSimAuthenticationResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->requestIccSimAuthenticationResponse(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSendEnvelope(int32_t serial, aidlradio::RadioError errorCode,
                                                std::shared_ptr<RIL_GSTK_EnvelopeResponse> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  std::string rsp_data{};
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    rsp_data = respData->rsp;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendEnvelopeResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendEnvelopeResponse(responseInfo, rsp_data);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSendEnvelopeWithStatus(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_GSTK_EnvelopeResponse> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  aidlsim::IccIoResult rsp_data{};
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    rsp_data.simResponse = respData->rsp;
    rsp_data.sw1 = respData->sw1;
    rsp_data.sw2 = respData->sw2;
  }
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendEnvelopeWithStatusResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendEnvelopeWithStatusResponse(responseInfo, rsp_data);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSendTerminalResponseToSim(int32_t serial,
                                                             aidlradio::RadioError errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendTerminalResponseToSimResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendTerminalResponseToSimResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSetAllowedCarriers(int32_t serial, RIL_Errno errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setAllowedCarriersResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setAllowedCarriersResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendCarrierInfoForImsiEncryptionResponse(
    aidlradio::RadioResponseInfo responseInfo) {
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("Sending sendCarrierInfoForImsiEncryptionResponse Response");
    auto ret = respCb->setCarrierInfoForImsiEncryptionResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("Response Callback is Nullptr");
  }
}

void IRadioSimImpl::sendResponseForSetCdmaSubscriptionSource(int32_t serial, RIL_Errno errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setCdmaSubscriptionSourceResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setCdmaSubscriptionSourceResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSetFacilityLockForApp(int32_t serial, RIL_Errno errorCode,
                                                         int32_t retry) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setFacilityLockForAppResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setFacilityLockForAppResponse(responseInfo, retry);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSetSimCardPower(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  QCRIL_LOG_DEBUG("setSimCardPowerResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setSimCardPowerResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForSetUiccSubscription(int32_t serial, RIL_Errno errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setUiccSubscriptionResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setUiccSubscriptionResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSupplyIccPin2ForApp(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  int retries{ -1 };
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    retries = respData->no_of_retries;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("supplyIccPin2ForAppResponse: serial=%d", serial);
    auto ret = respCb->supplyIccPin2ForAppResponse(responseInfo, retries);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSupplyIccPinForApp(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  int retries{ -1 };
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    retries = respData->no_of_retries;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("supplyIccPinForAppResponse: serial=%d", serial);
    auto ret = respCb->supplyIccPinForAppResponse(responseInfo, retries);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSupplyIccPuk2ForApp(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  int retries{ -1 };
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    retries = respData->no_of_retries;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("supplyIccPuk2ForAppResponse: serial=%d", serial);
    auto ret = respCb->supplyIccPuk2ForAppResponse(responseInfo, retries);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSupplyIccPukForApp(
    int32_t serial, aidlradio::RadioError errorCode,
    std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             aidlradio::RadioError::INTERNAL_ERR };
  int retries{ -1 };
  if (errorCode == aidlradio::RadioError::NONE && respData) {
    responseInfo.error = static_cast<aidlradio::RadioError>(respData->err);
    retries = respData->no_of_retries;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("supplyIccPukForAppResponse: serial=%d", serial);
    auto ret = respCb->supplyIccPukForAppResponse(responseInfo, retries);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioSimImpl::sendResponseForSimDepersonalization(int32_t serial,
                                                        aidlradio::RadioError errorCode,
                                                        aidlsim::PersoSubstate persotype,
                                                        int retries) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = getResponseCallback();

  if (!respCb) {
    return;
  }
  auto ret = respCb->supplySimDepersonalizationResponse(responseInfo, persotype, retries);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioSimImpl::sendResponseForUpdateSimPhonebookRecords(int32_t serial, RIL_Errno errorCode,
                                                             int32_t recordIndex) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  QCRIL_LOG_DEBUG("updateSimPhonebookRecordsResponse: serial=%d, error=%d, recordId=%d", serial,
                  errorCode, recordIndex);
  auto ret = respCb->updateSimPhonebookRecordsResponse(responseInfo, recordIndex);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

/** Start of Request messages  **/

::ndk::ScopedAStatus IRadioSimImpl::areUiccApplicationsEnabled(int32_t serial) {
  //  return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION); FIXME
  QCRIL_LOG_DEBUG("areUiccApplicationsEnabled: serial=%d", serial);
  std::shared_ptr<IRadioSimContext> ctx = this->getContext(serial);
  auto msg = std::make_shared<RilRequestGetUiccAppStatusMessage>(ctx);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetUiccAppStatusResult_t> result{};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            result =
                std::static_pointer_cast<qcril::interfaces::RilGetUiccAppStatusResult_t>(resp->data);
          }
          sendResponseForAreUiccApplicationsEnabled(serial, errorCode, result);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForAreUiccApplicationsEnabled(serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::changeIccPin2ForApp(int32_t serial, const std::string& oldPin2,
                                                        const std::string& newPin2,
                                                        const std::string& aid) {
  QCRIL_LOG_DEBUG("changeIccPin2ForApp: serial=%d", serial);
  auto msg = std::make_shared<UimChangeSimPinRequestMsg>(RIL_UIM_SIM_PIN2, newPin2, oldPin2, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          sendResponseForChangeIccPin2ForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForChangeIccPin2ForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::changeIccPinForApp(int32_t serial, const std::string& oldPin,
                                                       const std::string& newPin,
                                                       const std::string& aid) {
  QCRIL_LOG_DEBUG("changeIccPinForApp: serial=%d", serial);
  auto msg = std::make_shared<UimChangeSimPinRequestMsg>(RIL_UIM_SIM_PIN1, newPin, oldPin, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          sendResponseForChangeIccPinForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForChangeIccPinForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::enableUiccApplications(int32_t serial, bool enable) {
  QCRIL_LOG_DEBUG("enableUiccApplications: serial=%d enable=%d", serial, enable);
  std::shared_ptr<IRadioSimContext> ctx = this->getContext(serial);
  auto msg = std::make_shared<RilRequestEnableUiccAppMessage>(ctx, enable);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponseForEnableUiccApplications(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForEnableUiccApplications(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getAllowedCarriers(int32_t serial) {
  QCRIL_LOG_DEBUG("getAllowedCarriers: serial=%d", serial);
  sendResponseForGetAllowedCarriers(serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getCdmaSubscription(int32_t serial) {
  QCRIL_LOG_DEBUG("getCdmaSubscription: serial=%d", serial);
  std::shared_ptr<IRadioSimContext> ctx = this->getContext(serial);
  auto msg = std::make_shared<RilRequestCdmaSubscriptionMessage>(ctx);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionResult_t> rilCdmaSubResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            rilCdmaSubResult =
                std::static_pointer_cast<qcril::interfaces::RilGetCdmaSubscriptionResult_t>(
                    resp->data);
          }
          sendResponseForGetCdmaSubscription(serial, errorCode, rilCdmaSubResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetCdmaSubscription(serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getCdmaSubscriptionSource(int32_t serial) {
  QCRIL_LOG_DEBUG("getCdmaSubscriptionSource: serial=%d", serial);
  bool sendFailure = false;
  do {
    std::shared_ptr<IRadioSimContext> ctx = this->getContext(serial);
    auto msg = std::make_shared<RilRequestGetCdmaSubscriptionSourceMessage>(ctx);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionSourceResult_t> srcResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            srcResult =
                std::static_pointer_cast<qcril::interfaces::RilGetCdmaSubscriptionSourceResult_t>(
                    resp->data);
          }
          sendResponseForGetCdmaSubscriptionSource(serial, errorCode, srcResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    sendResponseForGetCdmaSubscriptionSource(serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getFacilityLockForApp(int32_t serial,
                                                          const std::string& facility,
                                                          const std::string& password,
                                                          int32_t serviceClass,
                                                          const std::string& appId) {
  QCRIL_LOG_DEBUG("getFacilityLockForApp: serial=%d, facility=%s, serviceClass=%d", serial,
                  facility.c_str(), serviceClass);
  bool sendFailure = false;
  RIL_Errno errResp = RIL_E_NO_MEMORY;

  do {
    if (facility == "SC" || facility == "FD") {
      auto msg = std::make_shared<UimGetFacilityLockRequestMsg>(password, appId,
                                                                (facility == "SC" ? PIN1 : FDN));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        sendFailure = true;
        break;
      }
      GenericCallback<RIL_UIM_GetFacilityLockResponseMsg> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_GetFacilityLockResponseMsg> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            int32_t lockStatus = 0;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = static_cast<RIL_Errno>(resp->ril_err);
              lockStatus = resp->lock_status;
            }
            sendResponseForGetFacilityLockForApp(serial, errorCode, lockStatus);
          });
      msg->setCallback(&cb);
      msg->dispatch();
      break;
    }
    qcril::interfaces::FacilityType rilFacility = convertFacilityType(facility);
    if (rilFacility != qcril::interfaces::FacilityType::UNKNOWN) {
      auto queryMsg = std::make_shared<QcRilRequestGetCallBarringMessage>(this->getContext(serial));
      if (queryMsg == nullptr) {
        QCRIL_LOG_ERROR("queryMsg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        sendFailure = true;
        break;
      }
      // Set parameters
      queryMsg->setFacilityType(rilFacility);
      queryMsg->setServiceClass(serviceClass);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            int32_t serviceClass = 0;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              auto data =
                  std::static_pointer_cast<qcril::interfaces::SuppServiceStatusInfo>(resp->data);
              if (data->hasServiceClass()) {
                serviceClass = data->getServiceClass();
              }
            }
            sendResponseForGetFacilityLockForApp(serial, errorCode, serviceClass);
          });
      queryMsg->setCallback(&cb);
      queryMsg->dispatch();
    } else {
      QCRIL_LOG_ERROR("Invalid Facility Type");
      errResp = RIL_E_INVALID_ARGUMENTS;
      sendFailure = true;
    }
  } while (FALSE);
  if (sendFailure) {
    sendResponseForGetFacilityLockForApp(serial, errResp, 0);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getIccCardStatus(int32_t serial) {
  QCRIL_LOG_DEBUG("getIccCardStatus: serial=%d", serial);
  auto msg = std::make_shared<UimGetCardStatusRequestMsg>(this->getInstanceId());
  if (msg) {
    GenericCallback<RIL_UIM_CardStatus> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_CardStatus> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          sendResponseForGetIccCardStatus(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetIccCardStatus(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getImsiForApp(int32_t serial, const std::string& aid) {
  QCRIL_LOG_DEBUG("getImsiForApp: serial=%d", serial);
  auto msg = std::make_shared<UimGetImsiRequestMsg>(this->getInstanceId(), aid);
  if (msg) {
    GenericCallback<RIL_UIM_IMSI_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_IMSI_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };

          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          sendResponseForGetImsiForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetImsiForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getSimPhonebookCapacity(int32_t serial) {
  QCRIL_LOG_DEBUG("getSimPhonebookCapacity: serial=%d", serial);
  auto msg = std::make_shared<QcRilRequestGetAdnCapacityMessage>(this->getContext(serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          qcril::interfaces::qcril_pbm_adn_count_info adnCount;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            adnCount = std::static_pointer_cast<qcril::interfaces::AdnCountInfoResp>(resp->data)
                           ->getAdnCountInfo();
          }
          std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adn_count_info =
              std::make_shared<qcril::interfaces::qcril_pbm_adn_count_info>(adnCount);
          this->sendResponseForGetSimPhonebookCapacity(serial, errorCode, adn_count_info);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adn_count_info;
    this->sendResponseForGetSimPhonebookCapacity(serial, RIL_E_NO_MEMORY, adn_count_info);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::getSimPhonebookRecords(int32_t serial) {
  QCRIL_LOG_DEBUG("getSimPhonebookRecords: serial=%d", serial);
  auto msg = std::make_shared<QcRilRequestGetPhonebookRecordsMessage>(this->getContext(serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForGetSimPhonebookRecords(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetSimPhonebookRecords(serial, RIL_E_GENERIC_FAILURE);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::iccCloseLogicalChannel(int32_t serial, int32_t channelId) {
  QCRIL_LOG_DEBUG("iccCloseLogicalChannel: serial=%d", serial);
  auto msg = std::make_shared<UimSIMCloseChannelRequestMsg>(channelId);
  if (msg) {
    GenericCallback<RIL_UIM_Errno> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
            errorCode = static_cast<aidlradio::RadioError>(*responseDataPtr);
          }
          this->sendResponseForIccCloseLogicalChannel(serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForIccCloseLogicalChannel(serial, aidlradio::RadioError::NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::iccIoForApp(int32_t serial, const aidlsim::IccIo& iccIo) {
  RIL_UIM_SIM_IO data = {};
  data.command = iccIo.command;
  data.fileid = iccIo.fileId;
  data.path = iccIo.path;
  data.p1 = iccIo.p1;
  data.p2 = iccIo.p2;
  data.p3 = iccIo.p3;
  data.data = iccIo.data;
  data.pin2 = iccIo.pin2;
  data.aidPtr = iccIo.aid;

  auto msg = std::make_shared<UimSIMIORequestMsg>(this->getInstanceId(), data);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_IO_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForIccIOForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForIccIOForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::iccOpenLogicalChannel(int32_t serial, const std::string& aid,
                                                          int32_t p2) {
  QCRIL_LOG_DEBUG("iccOpenLogicalChannel: serial=%d", serial);
  auto msg = std::make_shared<UimSIMOpenChannelRequestMsg>(aid, p2);
  if (msg) {
    GenericCallback<RIL_UIM_OpenChannelResponse> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_OpenChannelResponse> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForIccOpenLogicalChannel(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForIccOpenLogicalChannel(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::iccTransmitApduBasicChannel(int32_t serial,
                                                                const aidlsim::SimApdu& message) {
  QCRIL_LOG_DEBUG("iccTransmitApduBasicChannel: serial=%d", serial);

  RIL_UIM_SIM_APDU data = {};
  data.sessionid = message.sessionId;
  data.cla = message.cla;
  data.instruction = message.instruction;
  data.p1 = message.p1;
  data.p2 = message.p2;
  data.p3 = message.p3;
  data.data = message.data;
  data.isEs10 = message.isEs10;

  auto msg = std::make_shared<UimTransmitAPDURequestMsg>(this->getInstanceId(), true, true, data);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_IO_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForIccTransmitApduBasicChannel(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForIccTransmitApduBasicChannel(serial, aidlradio::RadioError::NO_MEMORY,
                                                     nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::iccTransmitApduLogicalChannel(int32_t serial,
                                                                  const aidlsim::SimApdu& message) {
  QCRIL_LOG_DEBUG("iccTransmitApduLogicalChannel: serial=%d", serial);

  RIL_UIM_SIM_APDU data = {};
  data.sessionid = message.sessionId;
  data.cla = message.cla;
  data.instruction = message.instruction;
  data.p1 = message.p1;
  data.p2 = message.p2;
  data.p3 = message.p3;
  data.data = message.data;
  data.isEs10 = message.isEs10;

  auto msg = std::make_shared<UimTransmitAPDURequestMsg>(this->getInstanceId(), false, true, data);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_IO_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForIccTransmitApduLogicalChannel(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForIccTransmitApduLogicalChannel(serial, aidlradio::RadioError::NO_MEMORY,
                                                       nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::reportStkServiceIsRunning(int32_t serial) {
  QCRIL_LOG_DEBUG("reportStkServiceIsRunning: serial=%d", serial);
  auto msg = std::make_shared<GstkReportSTKIsRunningMsg>();
  if (msg) {
    GenericCallback<RIL_GSTK_Errno> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
            errorCode = static_cast<aidlradio::RadioError>(*responseDataPtr);
          }
          this->sendResponseForReportStkServiceIsRunning(serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForReportStkServiceIsRunning(serial, aidlradio::RadioError::NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::requestIccSimAuthentication(int32_t serial, int32_t authContext,
                                                                const std::string& authData,
                                                                const std::string& aid) {
  QCRIL_LOG_DEBUG("requestIccSimAuthentication: serial=%d", serial);
  auto msg = std::make_shared<UimSIMAuthenticationRequestMsg>(authContext, authData, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_IO_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForRequestIccSimAuthentication(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForRequestIccSimAuthentication(serial, aidlradio::RadioError::NO_MEMORY,
                                                     nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

/*FIXME
  ::ndk::ScopedAStatus IRadioSimImpl::requestIsimAuthentication(int32_t serial, const std::string&
  challenge) { return ::ndk::ScopedAStatus::ok();
  }
*/

::ndk::ScopedAStatus IRadioSimImpl::responseAcknowledgement() {
  QCRIL_LOG_DEBUG("responseAcknowledgement");
  qti::ril::utils::releaseWakeLock();
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::sendEnvelope(int32_t serial, const std::string& command) {
  QCRIL_LOG_DEBUG("sendEnvelope: serial=%d", serial);
  auto msg = std::make_shared<GstkSendEnvelopeRequestMsg>(serial, command);
  if (msg) {
    GenericCallback<RIL_GSTK_EnvelopeResponse> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_EnvelopeResponse> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForSendEnvelope(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendEnvelope(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::sendEnvelopeWithStatus(int32_t serial,
                                                           const std::string& contents) {
  QCRIL_LOG_DEBUG("sendEnvelopeWithStatus: serial=%d", serial);
  auto msg = std::make_shared<GstkSendEnvelopeRequestMsg>(serial, contents);
  if (msg) {
    GenericCallback<RIL_GSTK_EnvelopeResponse> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_EnvelopeResponse> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForSendEnvelopeWithStatus(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendEnvelopeWithStatus(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::sendTerminalResponseToSim(int32_t serial,
                                                              const std::string& commandResponse) {
  QCRIL_LOG_DEBUG("sendTerminalResponseToSim: serial=%d", serial);
  auto msg = std::make_shared<GstkSendTerminalResponseRequestMsg>(serial, commandResponse);
  if (msg) {
    GenericCallback<RIL_GSTK_Errno> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
            errorCode = static_cast<aidlradio::RadioError>(*responseDataPtr);
          }
          this->sendResponseForSendTerminalResponseToSim(serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendTerminalResponseToSim(serial, aidlradio::RadioError::NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::setAllowedCarriers(
    int32_t serial, const aidlsim::CarrierRestrictions& carriers,
    aidlsim::SimLockMultiSimPolicy multiSimPolicy) {
  QCRIL_LOG_DEBUG("setAllowedCarriers: serial=%d", serial);
  this->sendResponseForSetAllowedCarriers(serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::setCarrierInfoForImsiEncryption(
    int32_t serial, const aidlsim::ImsiEncryptionInfo& imsiEncryptionInfo) {
  QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
  using namespace rildata;  // FIXME
  ImsiEncryptionInfo_t imsiData = {};
  imsiData.mcc = imsiEncryptionInfo.mcc;
  imsiData.mnc = imsiEncryptionInfo.mnc;
  for (int i = 0; i < imsiEncryptionInfo.carrierKey.size(); i++) {
    imsiData.carrierKey.push_back(imsiEncryptionInfo.carrierKey[i]);
  }
  imsiData.keyIdentifier = imsiEncryptionInfo.keyIdentifier;
  imsiData.expiryTime = imsiEncryptionInfo.expirationTime;
  imsiData.keyType = (rildata::PublicKeyType_t)imsiEncryptionInfo.keyType;
  auto msg = std::make_shared<rildata::SetCarrierInfoImsiEncryptionMessage>(imsiData);
  if (msg) {
    GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> msg,
                                                 Message::Callback::Status status,
                                                 std::shared_ptr<RIL_Errno> resp) -> void {
      aidlradio::RadioResponseInfo responseInfo{.serial = serial,
                                                .error = aidlradio::RadioError::GENERIC_FAILURE };
      if (resp) {
        aidlradio::RadioError errorCode = aidlradio::RadioError::GENERIC_FAILURE;
        if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
          errorCode = aidlradio::RadioError::REQUEST_NOT_SUPPORTED;
        } else if (status == Message::Callback::Status::SUCCESS) {
          errorCode = static_cast<aidlradio::RadioError>(*resp);
        }
        responseInfo = {.serial = serial, .error = errorCode };
        QCRIL_LOG_DEBUG("setCarrierInfoForImsiEncryption cb invoked status %d respErr %d", status,
                        *resp);
      }
      this->sendCarrierInfoForImsiEncryptionResponse(responseInfo);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    QCRIL_LOG_ERROR("Unable to create msg SetCarrierInfoImsiEncryptionMessage");
    aidlradio::RadioResponseInfo rsp{ aidlradio::RadioResponseType::SOLICITED, serial,
                                      aidlradio::RadioError::NO_MEMORY };
    this->sendCarrierInfoForImsiEncryptionResponse(rsp);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::setCdmaSubscriptionSource(
    int32_t serial, aidlsim::CdmaSubscriptionSource cdmaSub) {
  QCRIL_LOG_DEBUG("setCdmaSubscriptionSource: serial=%d", serial);
  auto source = (cdmaSub == aidlsim::CdmaSubscriptionSource::RUIM_SIM)
                    ? RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource::SIM
                    : RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource::NV;
  auto msg = std::make_shared<RilRequestSetCdmaSubscriptionSourceMessage>(this->getContext(serial),
                                                                          source);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCdmaSubscriptionSource(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetCdmaSubscriptionSource(serial, RIL_E_NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::setFacilityLockForApp(
    int32_t serial, const std::string& facility, bool lockState, const std::string& password,
    int32_t serviceClass, const std::string& appId) {
  QCRIL_LOG_DEBUG("setFacilityLockForApp: serial=%d, facility=%s, lockState=%d, serviceClass=%d",
                  serial, facility.c_str(), lockState, serviceClass);
  bool sendFailure = false;
  RIL_Errno errResp = RIL_E_NO_MEMORY;
  do {
    if (facility == "SC" || facility == "FD") {
      auto msg = std::make_shared<UimSetFacilityLockRequestMsg>(password, appId, lockState,
                                                                (facility == "SC" ? PIN1 : FDN));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_SIM_PIN_Response> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            int32_t retry = 0;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = static_cast<RIL_Errno>(resp->err);
              retry = resp->no_of_retries;
            }
            this->sendResponseForSetFacilityLockForApp(serial, errorCode, retry);
          });
      msg->setCallback(&cb);
      msg->dispatch();
      break;
    }
    qcril::interfaces::FacilityType rilFacility = convertFacilityType(facility);
    if (rilFacility != qcril::interfaces::FacilityType::UNKNOWN) {
      auto setSupsMsg =
          std::make_shared<QcRilRequestSetSupsServiceMessage>(this->getContext(serial));
      if (setSupsMsg == nullptr) {
        QCRIL_LOG_ERROR("setSupsMsg is nullptr");
        sendFailure = true;
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      // Set parameters
      if (lockState) {
        setSupsMsg->setOperationType(1);  // activate
      } else {
        setSupsMsg->setOperationType(2);  // deactivate
      }
      setSupsMsg->setFacilityType(rilFacility);
      setSupsMsg->setServiceClass(serviceClass);
      if (password.size()) {
        setSupsMsg->setPassword(password.c_str());
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetFacilityLockForApp(serial, errorCode, -1);
          });
      setSupsMsg->setCallback(&cb);
      setSupsMsg->dispatch();
    } else {
      QCRIL_LOG_ERROR("Invalid Facility Type");
      sendFailure = true;
      errResp = RIL_E_INVALID_ARGUMENTS;
    }
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForSetFacilityLockForApp(serial, errResp, -1);
  }
  return ::ndk::ScopedAStatus::ok();
}

/*  ::ndk::ScopedAStatus IRadioSimImpl::setResponseFunctions(const
  std::shared_ptr<aidlsim::IRadioSimResponse>& radioSimResponse, const
  std::shared_ptr<aidlsim::IRadioSimIndication>& radioSimIndication) { return
  ::ndk::ScopedAStatus::ok();
  }*/
// FIXME

::ndk::ScopedAStatus IRadioSimImpl::setSimCardPower(int32_t serial,
                                                    aidlsim::CardPowerState powerUp) {
  QCRIL_LOG_DEBUG("setSimCardPower_1_6: serial=%d", serial);
  auto msg = std::make_shared<UimCardPowerReqMsg>(this->getInstanceId(),
                                                  static_cast<RIL_UIM_CardPowerState>(powerUp));
  if (msg != nullptr) {
    GenericCallback<RIL_UIM_Errno> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && responseDataPtr != nullptr) {
            errorCode = static_cast<RIL_Errno>(*responseDataPtr);
          }
          this->sendResponseForSetSimCardPower(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetSimCardPower(serial, RIL_E_NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::setUiccSubscription(int32_t serial,
                                                        const aidlsim::SelectUiccSub& uiccSub) {
  QCRIL_LOG_DEBUG("setUiccSubscription: serial=%d", serial);
  // convert HIDL type to RIL internal structure
  RIL_SelectUiccSub rilUiccSub{};
  rilUiccSub.slot = uiccSub.slot;
  rilUiccSub.app_index = uiccSub.appIndex;
  rilUiccSub.sub_type = static_cast<RIL_SubscriptionType>(uiccSub.subType);
  rilUiccSub.act_status = static_cast<RIL_UiccSubActStatus>(uiccSub.actStatus);

  // Create message and callback
  auto msg = std::make_shared<RilRequestSetUiccSubsMessage>(this->getContext(serial), rilUiccSub);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetUiccSubscription(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetUiccSubscription(serial, RIL_E_NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::supplyIccPin2ForApp(int32_t serial, const std::string& pin2,
                                                        const std::string& aid) {
  QCRIL_LOG_DEBUG("supplyIccPin2ForApp: serial=%d", serial);
  auto msg = std::make_shared<UimEnterSimPinRequestMsg>(RIL_UIM_SIM_PIN2, pin2, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForSupplyIccPin2ForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSupplyIccPin2ForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::supplyIccPinForApp(int32_t serial, const std::string& pin,
                                                       const std::string& aid) {
  QCRIL_LOG_DEBUG("supplyIccPinForApp: serial=%d", serial);
  auto msg = std::make_shared<UimEnterSimPinRequestMsg>(RIL_UIM_SIM_PIN1, pin, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForSupplyIccPinForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSupplyIccPinForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::supplyIccPuk2ForApp(int32_t serial, const std::string& puk2,
                                                        const std::string& pin2,
                                                        const std::string& aid) {
  QCRIL_LOG_DEBUG("supplyIccPuk2ForApp: serial=%d", serial);
  auto msg = std::make_shared<UimEnterSimPukRequestMsg>(RIL_UIM_SIM_PUK2, puk2, pin2, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForSupplyIccPuk2ForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSupplyIccPuk2ForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::supplyIccPukForApp(int32_t serial, const std::string& puk,
                                                       const std::string& pin,
                                                       const std::string& aid) {
  QCRIL_LOG_DEBUG("supplyIccPukForApp: serial=%d", serial);
  auto msg = std::make_shared<UimEnterSimPukRequestMsg>(RIL_UIM_SIM_PUK1, puk, pin, aid);
  if (msg) {
    GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{ aidlradio::RadioError::INTERNAL_ERR };
          if (status == Message::Callback::Status::SUCCESS) {
            errorCode = aidlradio::RadioError::NONE;
          }
          this->sendResponseForSupplyIccPukForApp(serial, errorCode, responseDataPtr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSupplyIccPukForApp(serial, aidlradio::RadioError::NO_MEMORY, nullptr);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::supplySimDepersonalization(int32_t serial,
                                                               aidlsim::PersoSubstate persoType,
                                                               const std::string& controlKey) {
  QCRIL_LOG_INFO("supplySimDepersonalization: serial=%d, persoType=%d ", serial, persoType);
  RIL_UIM_PersoSubstate rilPersoType = convertHidlToRilPersoType(persoType);
  auto msg = std::make_shared<UimEnterDePersoRequestMsg>(controlKey, rilPersoType);
  if (msg) {
    GenericCallback<RIL_UIM_PersoResponse> cb(
        [this, serial, persoType](std::shared_ptr<Message> solicitedMsg,
                                  Message::Callback::Status status,
                                  std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) -> void {
          aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED,
                                                     serial, aidlradio::RadioError::INTERNAL_ERR };
          int retries = -1;
          if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS) {
            responseInfo.error = static_cast<aidlradio::RadioError>(responseDataPtr->err);
            retries = responseDataPtr->no_of_retries;
          }
          this->sendResponseForSimDepersonalization(serial, responseInfo.error, persoType, retries);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSimDepersonalization(serial, aidlradio::RadioError::NO_MEMORY, persoType,
                                              -1);
  }
  return ::ndk::ScopedAStatus::ok();
}

void IRadioSimImpl::sendResponseForCloseLogicalChannelWithSessionInfoResponse(
    int32_t serial, aidlradio::RadioError errorCode) {
  auto respCb = this->getResponseCallback();
  if(respCb == nullptr) {
     QCRIL_LOG_INFO("IRadioSim: getResponseCallback Failed");
     return;
  }
  aidlradio::RadioResponseInfo responseInfo{
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};
  auto ret =
      respCb->iccCloseLogicalChannelWithSessionInfoResponse(responseInfo);

  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioSimImpl::updateSimPhonebookRecords(
    int32_t serial, const aidlsim::PhonebookRecordInfo& pbRecords) {
  QCRIL_LOG_DEBUG("updateSimPhonebookRecords: serial=%d", serial);
  qcril::interfaces::AdnRecordInfo records;
  RIL_Errno res = convertHidlPhonebookRecords(pbRecords, records);
  if (res != RIL_E_SUCCESS) {
    this->sendResponseForUpdateSimPhonebookRecords(serial, res, -1);
    return ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<QcRilRequestUpdatePhonebookRecordsMessage>(this->getContext(serial),
                                                                         records);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (msg && resp) {
            if (resp->data) {
              int32_t recordIndex =
                  std::static_pointer_cast<qcril::interfaces::AdnRecordUpdatedResp>(resp->data)
                      ->getRecordIndex();
              errorCode = resp->errorCode;
              this->sendResponseForUpdateSimPhonebookRecords(serial, errorCode, recordIndex);
            }
          }
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForUpdateSimPhonebookRecords(serial, RIL_E_GENERIC_FAILURE, -1);
  }
  return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioSimImpl::iccCloseLogicalChannelWithSessionInfo(
    int32_t serial, const aidlsim::SessionInfo &sessionInfo) {
  QCRIL_LOG_DEBUG("iccCloseLogicalChannel: serial=%d", serial);
  int32_t session_id = sessionInfo.sessionId;
  bool isEs10 = sessionInfo.isEs10;
  auto msg =
      std::make_shared<UimSIMCloseChannelSessionReqMsg>(session_id, isEs10);
  if (msg) {
    GenericCallback<RIL_UIM_Errno> cb((
        [this, serial](std::shared_ptr<Message> /*msg*/,
                       Message::Callback::Status status,
                       std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
          aidlradio::RadioError errorCode{aidlradio::RadioError::INTERNAL_ERR};
          if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
            errorCode = static_cast<aidlradio::RadioError>(*responseDataPtr);
          }
          this->sendResponseForCloseLogicalChannelWithSessionInfoResponse(
              serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForCloseLogicalChannelWithSessionInfoResponse(
        serial, aidlradio::RadioError::NO_MEMORY);
  }
  return ::ndk::ScopedAStatus::ok();
}

/** Handle Indications **/

int IRadioSimImpl::sendCarrierInfoForImsiEncryptionInd(
    std::shared_ptr<rildata::CarrierInfoForImsiEncryptionRefreshMessage> msg) {
  QCRIL_LOG_DEBUG("%s(): ", __FUNCTION__);
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return 1;
  }
  auto ind = getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind) {
    auto ret = ind->carrierInfoForImsiEncryption(
        aidlradio::RadioIndicationType::UNSOLICITED);  // FIXME Return
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
      return 1;
    }
  } else {
    QCRIL_LOG_ERROR("Ind cb is nullptr");
    return 1;
  }
  return 0;
}

/**
 * Notifies cdmaSubscriptionSourceChanged indication.
 * The implementation will invoke the latest version of the below the indication APIs based on
 * the version of the indication callback object set by the client.
 *   aidlsim::IRadioSimIndication::cdmaSubscriptionSourceChanged
 */
int IRadioSimImpl::sendCdmaSubscriptionSourceChanged(
    std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: cdmaSubscriptionSourceChanged");
    auto ret = ind->cdmaSubscriptionSourceChanged(
        aidlradio::RadioIndicationType::UNSOLICITED,
        static_cast<aidlsim::CdmaSubscriptionSource>(msg->getSource()));  // FIXME Return
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

/**
 * Notifies simPhonebookChanged indication.
 * The implementation will invoke the latest version of the below the indication APIs based on
 * the version of the indication callback object set by the client.
 *   aidlsim::IRadioSimIndication::simPhonebookChanged
 */
int IRadioSimImpl::sendSimPhonebookChanged(
    std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return 1;
  }
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    QCRIL_LOG_ERROR("Unable to send indication since no ind");
    return 1;
  }
  QCRIL_LOG_INFO("UNSOL: simPhonebookChanged");
  auto ret = ind->simPhonebookChanged(aidlradio::RadioIndicationType::UNSOLICITED);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
  return 0;
}

int IRadioSimImpl::sendSimPhonebookRecords(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return 1;
  }
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    QCRIL_LOG_ERROR("Unable to send indication since no ind");
    return 1;
  }
  std::vector<aidlsim::PhonebookRecordInfo> rilRecords;
  aidlsim::PbReceivedStatus status = convertRilPbReceivedStatus(msg->getSeqNum());
  convertRilPhonebookRecords(msg->getAdnRecords(), rilRecords);  // FIXME utils:: needed ??
  QCRIL_LOG_INFO("UNSOL: simPhonebookRecordsReceived");
  auto ret = ind->simPhonebookRecordsReceived(aidlradio::RadioIndicationType::UNSOLICITED, status,
                                              rilRecords);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
  return 0;
}

/**
 * Notifies simRefresh indication.
 * The implementation will invoke the latest version of the below the indication APIs based on
 * the version of the indication callback object set by the client.
 *   aidlsim::IRadioSimIndication::simRefresh
 */
int IRadioSimImpl::sendSimRefresh(std::shared_ptr<UimSimRefreshIndication> msg) {
  auto ind = getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    aidlsim::SimRefreshResult refreshResult = {};
    RIL_UIM_SIM_RefreshIndication simRefreshResponse = msg->get_refresh_ind();
    refreshResult.type = static_cast<int>(simRefreshResponse.result);  // FIXME type int ??
    refreshResult.efId = simRefreshResponse.ef_id;
    refreshResult.aid = simRefreshResponse.aid;

    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: simRefresh type %d efId %d", refreshResult.type, refreshResult.efId);
    auto ret = ind->simRefresh(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP,
                               refreshResult);  // FIXME Return
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

/**
 * Notifies simStatusChanged indication.
 * The implementation will invoke the latest version of the below the indication APIs based on
 * the version of the indication callback object set by the client.
 *   aidlsim::IRadioSimIndication::simStatusChanged
 */
int IRadioSimImpl::sendSimStatusChanged(std::shared_ptr<UimSimStatusChangedInd> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: simStatusChanged");
    auto ret =
        ind->simStatusChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP);  // FIXME Return
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
#ifdef QMI_RIL_UTF
  // To resend the SIM status notification once the client is connected
  if (!ind) {
    s_simStatusMsg = msg;
  }
#endif
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

/**
 * Notifies STK related indications.
 *   aidlsim::IRadioSimIndication::stkCallSetup
 *   aidlsim::IRadioSimIndication::stkEventNotify
 *   aidlsim::IRadioSimIndication::stkProactiveCommand
 *   aidlsim::IRadioSimIndication::stkSessionEnd
 */
int IRadioSimImpl::sendGstkIndication(std::shared_ptr<GstkUnsolIndMsg> msg) {
  auto ind = this->getIndicationCallback();

  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    ::ndk::ScopedAStatus ret = ::ndk::ScopedAStatus::ok();
    RIL_GSTK_UnsolData stk_ind = msg->get_unsol_data();

    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    switch (stk_ind.type) {
      case RIL_GSTK_UNSOL_SESSION_END:
        QCRIL_LOG_DEBUG("UNSOL: stkSessionEnd");
        ret = ind->stkSessionEnd(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP);
        break;
      case RIL_GSTK_UNSOL_PROACTIVE_COMMAND:
        QCRIL_LOG_DEBUG("UNSOL: stkProactiveCommand");
        ret = ind->stkProactiveCommand(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP,
                                       stk_ind.cmd);
        break;
      case RIL_GSTK_UNSOL_EVENT_NOTIFY:
        QCRIL_LOG_DEBUG("UNSOL: stkEventNotify");
        ret = ind->stkEventNotify(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, stk_ind.cmd);
        break;
      case RIL_GSTK_UNSOL_STK_CALL_SETUP:
        // For call setup send event notify followed by call setup
        QCRIL_LOG_DEBUG("UNSOL: stkEventNotify");
        ret = ind->stkEventNotify(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, stk_ind.cmd);
        break;
      default:
        QCRIL_LOG_FUNC_RETURN();
        return 0;
    }
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

/**
 * Notifies subscriptionStatusChanged indication.
 * The implementation will invoke the latest version of the below the indication APIs based on
 * the version of the indication callback object set by the client.
 *   aidlradio::IRadioSimIndication::subscriptionStatusChanged
 */
int IRadioSimImpl::sendUiccSubsStatusChanged(
    std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    bool activate = msg->getStatus();
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: subscriptionStatusChanged");
    auto ret = ind->subscriptionStatusChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP,
                                              activate);  // FIXME return
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return 0;
}

/**
 * Notifies uiccApplicationsEnablementChanged indication.
 * The implementation will invoke the latest version of the below the indication APIs based on
 * the version of the indication callback object set by the client.
 *   aidlradio::IRadioSimIndication::uiccApplicationsEnablementChanged
 */
int IRadioSimImpl::sendUiccAppsStatusChanged(
    std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return 1;
  }
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    return 1;
    // FIXME      return V1_4::implementation::RadioServiceImpl<T>::sendUiccAppsStatusChanged(msg);
  }
  bool state = msg->getStatus();
  QCRIL_LOG_DEBUG("UNSOL: uiccApplicationsEnablementChanged");
  auto ret = ind->uiccApplicationsEnablementChanged(
      aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, state);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
  return 0;
}
