/*
 * Copyright (c) 2018,2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __RADIO_SERVICE_1_0_H__
#define __RADIO_SERVICE_1_0_H__

#include "utils/SystemClock.h"

#include "android/hardware/radio/1.0/IRadio.h"
#include "android/hardware/radio/1.0/IRadioResponse.h"
#include "android/hardware/radio/1.0/IRadioIndication.h"
#include "android/hardware/radio/1.0/types.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.0/radio_service_utils_1_0.h"
#include "HalServiceImplFactory.h"

#include "interfaces/dms/dms_types.h"
#include "interfaces/dms/RilRequestRadioPowerMessage.h"
#include "interfaces/dms/RilRequestGetModemActivityMessage.h"
#include "interfaces/dms/RilRequestGetBaseBandVersionMessage.h"
#include "interfaces/dms/RilRequestGetDeviceIdentityMessage.h"
#include "interfaces/gstk/GstkSendEnvelopeRequestMsg.h"
#include "interfaces/gstk/GstkSendTerminalResponseRequestMsg.h"
#include "interfaces/gstk/GstkSendSetupCallResponseRequestMsg.h"
#include "interfaces/gstk/GstkReportSTKIsRunningMsg.h"
#include "interfaces/cellinfo/cellinfo.h"
#include "interfaces/cellinfo/RilRequestGetCellInfoListMessage.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/nas/RilRequestGetVoiceRegistrationMessage.h"
#include "interfaces/nas/RilRequestGetDataRegistrationMessage.h"
#include "interfaces/nas/RilRequestOperatorMessage.h"
#include "interfaces/nas/RilRequestQueryNetworkSelectModeMessage.h"
#include "interfaces/nas/RilRequestAllowDataMessage.h"
#include "interfaces/nas/RilRequestQueryAvailNetworkMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h"
#include "interfaces/nas/RilRequestGetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestSetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestGetSignalStrengthMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRadioTechMessage.h"
#include "interfaces/nas/RilRequestSetCdmaSubscriptionSourceMessage.h"
#include "interfaces/nas/RilRequestExitEmergencyCallbackMessage.h"
#include "interfaces/nas/RilRequestGetNeighborCellIdsMessage.h"
#include "interfaces/nas/RilRequestCdmaSubscriptionMessage.h"
#include "interfaces/nas/RilRequestSetCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestSetLocationUpdateMessage.h"
#include "interfaces/nas/RilRequestQueryAvailBandModeMessage.h"
#include "interfaces/nas/RilRequestSetBandModeMessage.h"
#include "interfaces/nas/RilRequestSetUiccSubsMessage.h"
#include "interfaces/nas/RilRequestGetRadioCapMessage.h"
#include "interfaces/nas/RilRequestSetRadioCapMessage.h"
#include "interfaces/nas/RilRequestShutDownMessage.h"
#include "interfaces/nas/RilRequestSetUnsolCellInfoListRateMessage.h"
#include "interfaces/nas/RilRequestSetUnsolRespFilterMessage.h"
#include "interfaces/nas/RilRequestQueryCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestGetCdmaSubscriptionSourceMessage.h"
#include "interfaces/sms/RilRequestSendSmsMessage.h"
#include "interfaces/sms/RilRequestCdmaSendSmsMessage.h"
#include "interfaces/sms/RilRequestAckGsmSmsMessage.h"
#include "interfaces/sms/RilRequestAckCdmaSmsMessage.h"
#include "interfaces/sms/RilRequestDeleteSmsOnSimMessage.h"
#include "interfaces/sms/RilRequestWriteSmsToSimMessage.h"
#include "interfaces/sms/RilRequestCdmaWriteSmsToRuimMessage.h"
#include "interfaces/sms/RilRequestCdmaDeleteSmsOnRuimMessage.h"
#include "interfaces/sms/RilRequestGetSmscAddressMessage.h"
#include "interfaces/sms/RilRequestSetSmscAddressMessage.h"
#include "interfaces/sms/RilRequestGetGsmBroadcastConfigMessage.h"
#include "interfaces/sms/RilRequestGetCdmaBroadcastConfigMessage.h"
#include "interfaces/sms/RilRequestGsmSetBroadcastSmsConfigMessage.h"
#include "interfaces/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h"
#include "interfaces/sms/RilRequestGsmSmsBroadcastActivateMessage.h"
#include "interfaces/sms/RilRequestCdmaSmsBroadcastActivateMessage.h"
#include "interfaces/sms/RilRequestReportSmsMemoryStatusMessage.h"
#include "interfaces/sms/RilRequestGetImsRegistrationMessage.h"
#include "interfaces/sms/RilRequestImsSendSmsMessage.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/uim/UimGetFacilityLockRequestMsg.h"
#include "interfaces/uim/UimSetFacilityLockRequestMsg.h"
#include "interfaces/uim/UimGetCardStatusRequestMsg.h"
#include "interfaces/uim/UimEnterSimPinRequestMsg.h"
#include "interfaces/uim/UimEnterSimPukRequestMsg.h"
#include "interfaces/uim/UimChangeSimPinRequestMsg.h"
#include "interfaces/uim/UimEnterDePersoRequestMsg.h"
#include "interfaces/uim/UimSIMAuthenticationRequestMsg.h"
#include "interfaces/uim/UimISIMAuthenticationRequestMsg.h"
#include "interfaces/uim/UimGetImsiRequestMsg.h"
#include "interfaces/uim/UimSIMIORequestMsg.h"
#include "interfaces/uim/UimTransmitAPDURequestMsg.h"
#include "interfaces/uim/UimSIMOpenChannelRequestMsg.h"
#include "interfaces/uim/UimSIMCloseChannelRequestMsg.h"
#include "interfaces/uim/UimCardPowerReqMsg.h"
#include "interfaces/voice/QcRilRequestConferenceMessage.h"
#include "interfaces/voice/QcRilRequestLastCallFailCauseMessage.h"
#include "interfaces/voice/QcRilRequestExplicitCallTransferMessage.h"
#include "interfaces/voice/QcRilRequestGetCallWaitingMessage.h"
#include "interfaces/voice/QcRilRequestGetClipMessage.h"
#include "interfaces/voice/QcRilRequestGetClirMessage.h"
#include "interfaces/voice/QcRilRequestHangupForegroundResumeBackgroundMessage.h"
#include "interfaces/voice/QcRilRequestSendUssdMessage.h"
#include "interfaces/voice/QcRilRequestCancelUssdMessage.h"
#include "interfaces/voice/QcRilRequestHangupMessage.h"
#include "interfaces/voice/QcRilRequestGetCurrentCallsMessage.h"
#include "interfaces/voice/QcRilRequestDialMessage.h"
#include "interfaces/voice/QcRilRequestAnswerMessage.h"
#include "interfaces/voice/QcRilRequestHangupWaitingOrBackgroundMessage.h"
#include "interfaces/voice/QcRilRequestQueryCallForwardMessage.h"
#include "interfaces/voice/QcRilRequestGetCallBarringMessage.h"
#include "interfaces/voice/QcRilRequestSendDtmfMessage.h"
#include "interfaces/voice/QcRilRequestCdmaBurstDtmfMessage.h"
#include "interfaces/voice/QcRilRequestCdmaFlashMessage.h"
#include "interfaces/voice/QcRilRequestGetPreferredVoicePrivacyMessage.h"
#include "interfaces/voice/QcRilRequestSetPreferredVoicePrivacyMessage.h"
#include "interfaces/voice/QcRilRequestSetTtyModeMessage.h"
#include "interfaces/voice/QcRilRequestGetTtyModeMessage.h"
#include "interfaces/voice/QcRilRequestSeparateConnectionMessage.h"
#include "interfaces/voice/QcRilRequestSetCallBarringPasswordMessage.h"
#include "interfaces/voice/QcRilRequestSetCallForwardMessage.h"
#include "interfaces/voice/QcRilRequestSetCallWaitingMessage.h"
#include "interfaces/voice/QcRilRequestSetClirMessage.h"
#include "interfaces/voice/QcRilRequestSetSuppSvcNotificationMessage.h"
#include "interfaces/voice/QcRilRequestSetSupsServiceMessage.h"
#include "interfaces/voice/QcRilRequestStartDtmfMessage.h"
#include "interfaces/voice/QcRilRequestStopDtmfMessage.h"
#include "interfaces/voice/QcRilRequestSwitchWaitingOrHoldingAndActiveMessage.h"
#include "interfaces/voice/QcRilRequestUdubMessage.h"
#include "interfaces/voice/QcRilRequestSetMuteMessage.h"
#include "interfaces/voice/QcRilRequestGetMuteMessage.h"

#include "request/SetLinkCapRptCriteriaMessage.h"

#undef TAG
#define TAG "RILQ"

extern RIL_RadioFunctions* s_vendorFunctions;

namespace android {

extern void grabPartialWakeLock();
extern void releaseWakeLock();
extern void onNewCommandConnect();
extern void storeNITZTimeData(const std::string& time);

namespace hardware {
namespace radio {
namespace V1_0 {
namespace implementation {

// V1_0::IRadio implementation
template <typename T>
class RadioServiceImpl : public T, public RadioServiceBase {
 private:
  ::android::sp<android::hardware::radio::V1_0::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_0::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_0::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_0::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_0::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
  }
  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }
  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_0::setResponseFunctions_nolock");
    if (mRadioResponse) {
      mRadioResponse->unlinkToDeath(this);
    }
    mRadioResponse = respCb;
    mRadioIndication = indCb;
    if (mRadioResponse != nullptr) {
      mRadioResponse->linkToDeath(this, 0);
    }
  }

  virtual void sendResponseForGetIccCardStatus(int32_t serial, V1_0::RadioError errorCode,
                                               std::shared_ptr<RIL_UIM_CardStatus> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    V1_0::CardStatus cardStatus{};
    if (errorCode == V1_0::RadioError::NONE) {
      responseInfo.error = utils::convertGetIccCardStatusResponse(cardStatus, respData);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getIccCardStatusResponse: serial=%d", serial);
      auto ret = respCb->getIccCardStatusResponse(responseInfo, cardStatus);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSupplyIccPinForApp(int32_t serial, V1_0::RadioError errorCode,
                                                 std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("supplyIccPinForAppResponse: serial=%d", serial);
      auto ret = respCb->supplyIccPinForAppResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendStartLceResponse(RadioResponseInfo responseInfo, LceStatusInfo result) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending StartLCERequestMessage V1_0 Response");
      auto ret = respCb->startLceServiceResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void sendStopLceResponse(RadioResponseInfo responseInfo, LceStatusInfo result) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending stopLceServiceResponse V1_0 Response");
      auto ret = respCb->stopLceServiceResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void sendPullLceResponse(RadioResponseInfo responseInfo, LceDataInfo result) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending pullLceResponse V1_0 Response");
      auto ret = respCb->pullLceDataResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void getDataCallListResponse(std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr,
                                       int serial, Message::Callback::Status status) {
    QCRIL_LOG_DEBUG("GetDataCallListResponse V1_0");
    using namespace rildata;
    using namespace V1_0;
    auto respCb = this->getResponseCallback();
    if (respCb) {
        QCRIL_LOG_DEBUG("Sending getDataCallList V1_0 Response");
        ::android::hardware::hidl_vec<V1_0::SetupDataCallResult> dcResultList;
        RadioResponseInfo responseInfo{.type = RadioResponseType::SOLICITED, .serial = serial, .error = RadioError::NO_MEMORY};
        if (responseDataPtr)
        {
            RadioError e = RadioError::NONE;
            if ((status == Message::Callback::Status::SUCCESS) &&
                (responseDataPtr->respErr == ResponseError_t::NO_ERROR))
            {
                QCRIL_LOG_DEBUG("getDataCallList cb invoked status %d respErr %d", status, responseDataPtr->respErr);
                QCRIL_LOG_DEBUG("Call list size = %d", responseDataPtr->call.size());

                dcResultList.resize(responseDataPtr->call.size());
                int i = 0;
                for (rildata::DataCallResult_t entry : responseDataPtr->call)
                {
                    utils::convertRilDataCallToHal(entry, dcResultList[i]);
                    i++;
                }
            }
            else
            {
                switch (responseDataPtr->respErr)
                {
                case ResponseError_t::NOT_SUPPORTED:
                    e = RadioError::REQUEST_NOT_SUPPORTED;
                    break;
                case ResponseError_t::INVALID_ARGUMENT:
                    e = RadioError::INVALID_ARGUMENTS;
                    break;
                default:
                    e = RadioError::GENERIC_FAILURE;
                    break;
                }
            }
            responseInfo = {.type = RadioResponseType::SOLICITED, .serial = serial, .error = e};
        }
        auto ret = respCb->getDataCallListResponse(responseInfo, dcResultList);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
        }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void setInitialAttachApnResponse(RadioResponseInfo responseInfo) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending setInitialAttachApnResponse V1_0 Response");
      auto ret = respCb->setInitialAttachApnResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void setDataProfileResponse(RadioResponseInfo responseInfo) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending setDataProfileResponse V1_0 Response");
      auto ret = respCb->setDataProfileResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void sendResponseForSupplyIccPukForApp(int32_t serial, V1_0::RadioError errorCode,
                                                 std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("supplyIccPukForAppResponse: serial=%d", serial);
      auto ret = respCb->supplyIccPukForAppResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSupplyIccPin2ForApp(int32_t serial, V1_0::RadioError errorCode,
                                                  std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("supplyIccPin2ForAppResponse: serial=%d", serial);
      auto ret = respCb->supplyIccPin2ForAppResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSupplyIccPuk2ForApp(int32_t serial, V1_0::RadioError errorCode,
                                                  std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("supplyIccPuk2ForAppResponse: serial=%d", serial);
      auto ret = respCb->supplyIccPuk2ForAppResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForChangeIccPinForApp(int32_t serial, V1_0::RadioError errorCode,
                                                 std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("changeIccPinForAppResponse: serial=%d", serial);
      auto ret = respCb->changeIccPinForAppResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForChangeIccPin2ForApp(int32_t serial, V1_0::RadioError errorCode,
                                                  std::shared_ptr<RIL_UIM_SIM_PIN_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("changeIccPin2ForAppResponse: serial=%d", serial);
      auto ret = respCb->changeIccPin2ForAppResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSupplyNetworkDepersonalization(
      int32_t serial, V1_0::RadioError errorCode, std::shared_ptr<RIL_UIM_PersoResponse> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    int retries{ -1 };
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      retries = respData->no_of_retries;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("supplyNetworkDepersonalizationResponse: serial=%d", serial);
      auto ret = respCb->supplyNetworkDepersonalizationResponse(responseInfo, retries);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCurrentCalls(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<V1_0::Call> calls{};
    if (errorCode == RIL_E_SUCCESS && currentCalls) {
      auto callList = currentCalls->getCallInfoList();
      if (!callList.empty()) {
        calls.resize(callList.size());
        for (uint32_t i = 0; i < callList.size(); i++) {
          utils::convertToHidl(calls[i], callList[i]);
        }
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCurrentCallsResponse: serial=%d, errorCode=%d, calls = %s", serial,
                      errorCode, toString(calls).c_str());
      auto ret = respCb->getCurrentCallsResponse(responseInfo, calls);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForDial(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("dialResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->dialResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetImsiForApp(int32_t serial, V1_0::RadioError errorCode,
                                            std::shared_ptr<RIL_UIM_IMSI_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    ::android::hardware::hidl_string IMSI{};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      IMSI = respData->IMSI;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getIMSIForAppResponse: serial=%d", serial);
      auto ret = respCb->getIMSIForAppResponse(responseInfo, IMSI);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForHangup(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("hangupConnectionResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->hangupConnectionResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForHangupWaitingOrBackground(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("hangupWaitingOrBackgroundResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->hangupWaitingOrBackgroundResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForHangupForegroundResumeBackground(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("hangupForegroundResumeBackgroundResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->hangupForegroundResumeBackgroundResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSwitchWaitingOrHoldingAndActive(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("switchWaitingOrHoldingAndActiveResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->switchWaitingOrHoldingAndActiveResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForConference(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("conferenceResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->conferenceResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForRejectCall(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("rejectCallResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->rejectCallResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetLastCallFailCause(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::LastCallFailCauseInfo> failCause) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    LastCallFailCauseInfo info{};

    if (errorCode == RIL_E_SUCCESS && failCause) {
      if (failCause->hasCallFailCause()) {
        info.causeCode = static_cast<LastCallFailCause>(failCause->getCallFailCause());
      }
      if (failCause->hasCallFailCauseDescription()) {
        info.vendorCause = failCause->getCallFailCauseDescription();
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getLastCallFailCauseResponse: serial=%d, error=%d, info = %s", serial,
                      errorCode, toString(info).c_str());
      auto ret = respCb->getLastCallFailCauseResponse(responseInfo, info);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetVoiceRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    VoiceRegStateResult voiceRegResponse{};

    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillVoiceRegistrationStateResponse(voiceRegResponse, rilRegResult->respData);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getVoiceRegistrationStateResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getVoiceRegistrationStateResponse(responseInfo, voiceRegResponse);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetDataRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    DataRegStateResult dataRegResponse{};

    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillDataRegistrationStateResponse(dataRegResponse, rilRegResult->respData);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getDataRegistrationStateResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getDataRegistrationStateResponse(responseInfo, dataRegResponse);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetSignalStrength(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    SignalStrength signalStrength{};

    if (errorCode == RIL_E_SUCCESS && rilSigResult) {
      utils::convertRilSignalStrengthToHal(signalStrength, rilSigResult->respData);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getSignalStrengthResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getSignalStrengthResponse(responseInfo, signalStrength);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetOperator(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetOperatorResult_t> rilOperatorResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_string longName{};
    ::android::hardware::hidl_string shortName{};
    ::android::hardware::hidl_string numeric{};

    if (errorCode == RIL_E_SUCCESS && rilOperatorResult) {
      longName = rilOperatorResult->longName;
      shortName = rilOperatorResult->shortName;
      numeric = rilOperatorResult->numeric;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getOperatorResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getOperatorResponse(responseInfo, longName, shortName, numeric);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetRadioPower(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setRadioPowerResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setRadioPowerResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendDtmf(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendDtmfResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendDtmfResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendSms(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<RilSendSmsResult_t> smsResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::SendSmsResult result{ -1, ::android::hardware::hidl_string(), -1 };
    if (smsResult) {
      result = utils::makeSendSmsResult(smsResult);
    }

    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendSmsResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendSmsResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendSMSExpectMore(int32_t serial, RIL_Errno errorCode,
                                                std::shared_ptr<RilSendSmsResult_t> smsResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::SendSmsResult result{ -1, ::android::hardware::hidl_string(), -1 };
    if (smsResult) {
      result = utils::makeSendSmsResult(smsResult);
    }

    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendSMSExpectMoreResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendSMSExpectMoreResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForIccIOForApp(int32_t serial, V1_0::RadioError errorCode,
                                          std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    IccIoResult result{};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      result.sw1 = respData->sw1;
      result.sw2 = respData->sw2;
      result.simResponse = respData->simResponse;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("iccIOForAppResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->iccIOForAppResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendUssd(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendUssdResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendUssdResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForCancelPendingUssd(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("cancelPendingUssdResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->cancelPendingUssdResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetClir(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<qcril::interfaces::ClirInfo> clirInfo) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    int n = -1, m = -1;
    if (errorCode == RIL_E_SUCCESS && clirInfo) {
      if (clirInfo->hasAction()) {
        n = clirInfo->getAction();
      }
      if (clirInfo->hasPresentation()) {
        m = clirInfo->getPresentation();
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getClirResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getClirResponse(responseInfo, n, m);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetClir(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setClirResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setClirResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCallForwardStatus(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<CallForwardInfo> callForwardInfos;
    if (errorCode == RIL_E_SUCCESS && data) {
      auto cfInfoList = data->getCallForwardInfo();
      if (!cfInfoList.empty()) {
        uint32_t count = cfInfoList.size();
        callForwardInfos.resize(count);
        for (uint32_t i = 0; i < count; i++) {
          auto& resp = cfInfoList[i];
          if (resp.hasStatus()) {
            callForwardInfos[i].status = utils::convertCallForwardInfoStatus(resp.getStatus());
          }
          if (resp.hasReason()) {
            callForwardInfos[i].reason = resp.getReason();
          }
          if (resp.hasServiceClass()) {
            callForwardInfos[i].serviceClass = resp.getServiceClass();
          }
          if (resp.hasToa()) {
            callForwardInfos[i].toa = resp.getToa();
          }
          if (resp.hasNumber() && !resp.getNumber().empty()) {
            callForwardInfos[i].number = resp.getNumber();
          }
          if (resp.hasTimeSeconds()) {
            callForwardInfos[i].timeSeconds = resp.getTimeSeconds();
          }
        }
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCallForwardStatusResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCallForwardStatusResponse(responseInfo, callForwardInfos);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCallForward(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCallForwardResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCallForwardResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCallWaiting(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::CallWaitingInfo> cwInfo) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool enable = false;
    int serviceClass = -1;
    if (errorCode == RIL_E_SUCCESS && cwInfo) {
      if (cwInfo->hasStatus()) {
        enable = ((cwInfo->getStatus() == qcril::interfaces::ServiceClassStatus::ENABLED) ? true
                                                                                          : false);
      }
      if (cwInfo->hasServiceClass()) {
        serviceClass = cwInfo->getServiceClass();
      }
    }

    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCallWaitingResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCallWaitingResponse(responseInfo, enable, serviceClass);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCallWaiting(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCallWaitingResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCallWaitingResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForAcknowledgeLastIncomingGsmSms(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("acknowledgeLastIncomingGsmSmsResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->acknowledgeLastIncomingGsmSmsResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForAcceptCall(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("acceptCallResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->acceptCallResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetFacilityLockForApp(int32_t serial, RIL_Errno errorCode,
                                                    int32_t response) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getFacilityLockForAppResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getFacilityLockForAppResponse(responseInfo, response);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetFacilityLockForApp(int32_t serial, RIL_Errno errorCode,
                                                    int32_t retry) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setFacilityLockForAppResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setFacilityLockForAppResponse(responseInfo, retry);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetBarringPassword(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setBarringPasswordResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setBarringPasswordResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetNetworkSelectionMode(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool manual = false;
    if (errorCode == RIL_E_SUCCESS && rilModeResult) {
      manual = rilModeResult->bManual;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getNetworkSelectionModeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getNetworkSelectionModeResponse(responseInfo, manual);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetNetworkSelectionModeAutomatic(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setNetworkSelectionModeAutomaticResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->setNetworkSelectionModeAutomaticResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetNetworkSelectionModeManual(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setNetworkSelectionModeManualResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->setNetworkSelectionModeManualResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetAvailableNetworks(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetAvailNetworkResult_t> rilNetworkResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<OperatorInfo> networks;
    if (errorCode == RIL_E_SUCCESS && rilNetworkResult) {
      utils::convertRilNetworkResultToHidl(rilNetworkResult->info, networks);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getAvailableNetworksResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getAvailableNetworksResponse(responseInfo, networks);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForStartDtmf(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("startDtmfResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->startDtmfResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForStopDtmf(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("stopDtmfResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->stopDtmfResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetBasebandVersion(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetBaseBandResult_t> rilResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_string version{};
    if (errorCode == RIL_E_SUCCESS && rilResult) {
      version = rilResult->version;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getBasebandVersionResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getBasebandVersionResponse(responseInfo, version);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSeparateConnection(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("separateConnectionResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->separateConnectionResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetMute(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setMuteResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setMuteResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetMute(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<qcril::interfaces::MuteInfo> muteInfo) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool enable = false;
    if (errorCode == RIL_E_SUCCESS && muteInfo) {
      if (muteInfo->hasEnable()) {
        enable = muteInfo->getEnable();
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getMuteResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getMuteResponse(responseInfo, enable);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetClip(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<qcril::interfaces::ClipInfo> clipInfo) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::ClipStatus clip = V1_0::ClipStatus::UNKNOWN;
    if (errorCode == RIL_E_SUCCESS && clipInfo) {
      if (clipInfo->hasClipStatus()) {
        // The radio/1.0/types.hal defines the enum ClipStatus as below.
        // enum ClipStatus : int32_t {
        //     CLIP_PROVISIONED,                     // CLIP provisioned
        //     CLIP_UNPROVISIONED,                   // CLIP not provisioned
        //     UNKNOWN,                              // unknown, e.g. no network etc
        // };
        // But telephony is expecting the values as below.
        // 0 : CLIP not provisioned
        // 1 : Provisioned
        // Since all the previous IRadio implementations and the telephony clients are
        // expecting the clip status as mentioned above, intentionally setting the values as
        // below.
        if (clipInfo->getClipStatus() == qcril::interfaces::ClipStatus::NOT_PROVISIONED) {
          clip = static_cast<V1_0::ClipStatus>(0);  // 0 : CLIP not provisioned
        } else if (clipInfo->getClipStatus() == qcril::interfaces::ClipStatus::PROVISIONED) {
          clip = static_cast<V1_0::ClipStatus>(1);  // 1 : Provisioned
        }
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getClipResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getClipResponse(responseInfo, clip);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetSuppServiceNotifications(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setSuppServiceNotificationsResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setSuppServiceNotificationsResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForWriteSmsToSim(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    int32 recordNumber = -1;
    if (errorCode == RIL_E_SUCCESS && writeSmsResponse) {
      recordNumber = writeSmsResponse->recordNumber;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("writeSmsToSimResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->writeSmsToSimResponse(responseInfo, recordNumber);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForDeleteSmsOnSim(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("deleteSmsOnSimResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->deleteSmsOnSimResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetBandMode(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setBandModeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setBandModeResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetAvailableBandModes(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilQueryAvailBandsResult_t> rilBandResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<V1_0::RadioBandMode> modes;
    if (errorCode == RIL_E_SUCCESS && rilBandResult) {
      modes.resize(rilBandResult->bandList.size());
      for (unsigned int i = 0; i < rilBandResult->bandList.size(); i++) {
        modes[i] = static_cast<V1_0::RadioBandMode>(rilBandResult->bandList[i]);
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getAvailableBandModesResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getAvailableBandModesResponse(responseInfo, modes);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendEnvelope(int32_t serial, V1_0::RadioError errorCode,
                                           std::shared_ptr<RIL_GSTK_EnvelopeResponse> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    ::android::hardware::hidl_string rsp_data{};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      rsp_data = respData->rsp;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendEnvelopeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendEnvelopeResponse(responseInfo, rsp_data);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendTerminalResponseToSim(int32_t serial, V1_0::RadioError errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendTerminalResponseToSimResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendTerminalResponseToSimResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForHandleStkCallSetupRequestFromSim(int32_t serial,
                                                               V1_0::RadioError errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("handleStkCallSetupRequestFromSimResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->handleStkCallSetupRequestFromSimResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForExplicitCallTransfer(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("explicitCallTransferResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->explicitCallTransferResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetPreferredNetworkType(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setPreferredNetworkTypeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setPreferredNetworkTypeResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetPreferredNetworkType(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::PreferredNetworkType nwType = V1_0::PreferredNetworkType::GSM_WCDMA;
    if (errorCode == RIL_E_SUCCESS && prefResult) {
      nwType = utils::convertRadioAccessFamilyToNwType(prefResult->pref);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getPreferredNetworkTypeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getPreferredNetworkTypeResponse(responseInfo, nwType);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetNeighboringCids(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetNeighborCellIdResult_t> rilCellResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<NeighboringCell> cells;
    if (errorCode == RIL_E_SUCCESS && rilCellResult) {
      utils::convertRilNeighboringCidResultToHidl(rilCellResult->cellList, cells);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getNeighboringCidsResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getNeighboringCidsResponse(responseInfo, cells);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetLocationUpdates(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setLocationUpdatesResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setLocationUpdatesResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCdmaSubscriptionSource(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCdmaSubscriptionSourceResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCdmaSubscriptionSourceResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCdmaRoamingPreference(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCdmaRoamingPreferenceResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCdmaRoamingPreferenceResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCdmaRoamingPreference(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t> roamPref) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::CdmaRoamingType pref = V1_0::CdmaRoamingType::HOME_NETWORK;
    if (errorCode == RIL_E_SUCCESS && roamPref) {
      pref = static_cast<V1_0::CdmaRoamingType>(roamPref->mPrefType);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCdmaRoamingPreferenceResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCdmaRoamingPreferenceResponse(responseInfo, pref);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetTTYMode(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setTTYModeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setTTYModeResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

#undef OFF
  virtual void sendResponseForGetTTYMode(int32_t serial, RIL_Errno errorCode,
                                         std::shared_ptr<qcril::interfaces::TtyModeResp> ttyModeResp) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::TtyMode mode = V1_0::TtyMode::OFF;
    if (errorCode == RIL_E_SUCCESS && ttyModeResp) {
      if (ttyModeResp->hasTtyMode()) {
        switch (ttyModeResp->getTtyMode()) {
          case qcril::interfaces::TtyMode::MODE_OFF:
            mode = V1_0::TtyMode::OFF;
            break;
          case qcril::interfaces::TtyMode::FULL:
            mode = V1_0::TtyMode::FULL;
            break;
          case qcril::interfaces::TtyMode::HCO:
            mode = V1_0::TtyMode::HCO;
            break;
          case qcril::interfaces::TtyMode::VCO:
            mode = V1_0::TtyMode::VCO;
            break;
          case qcril::interfaces::TtyMode::UNKNOWN:
            mode = V1_0::TtyMode::OFF;
            break;
        }
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getTTYModeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getTTYModeResponse(responseInfo, mode);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetPreferredVoicePrivacy(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setPreferredVoicePrivacyResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setPreferredVoicePrivacyResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  void sendResponseForGetPreferredVoicePrivacy(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::PrivacyModeResp> privacyModeResp) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool enable = false;
    if (errorCode == RIL_E_SUCCESS && privacyModeResp) {
      if (privacyModeResp->hasPrivacyMode()) {
        if (privacyModeResp->getPrivacyMode() == qcril::interfaces::PrivacyMode::ENHANCED) {
          enable = true;
        }
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getPreferredVoicePrivacyResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getPreferredVoicePrivacyResponse(responseInfo, enable);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendCDMAFeatureCode(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendCDMAFeatureCodeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendCDMAFeatureCodeResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendBurstDtmf(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendBurstDtmfResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendBurstDtmfResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendCdmaSms(int32_t serial, RIL_Errno errorCode,
                                          std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::SendSmsResult result{ -1, ::android::hardware::hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }

    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendCdmaSmsResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendCdmaSmsResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForAcknowledgeLastIncomingCdmaSms(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("acknowledgeLastIncomingCdmaSmsResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->acknowledgeLastIncomingCdmaSmsResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetGsmBroadcastConfig(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilGetGsmBroadcastConfigResult_t> broadcastConfigResp) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<GsmBroadcastSmsConfigInfo> configs;
    if (errorCode == RIL_E_SUCCESS && broadcastConfigResp) {
      // generate config list
      auto num = broadcastConfigResp->configList.size();
      configs.resize(num);
      for (unsigned int i = 0; i < num; i++) {
        auto& item = broadcastConfigResp->configList[i];
        configs[i].fromServiceId = item.fromServiceId;
        configs[i].toServiceId = item.toServiceId;
        configs[i].fromCodeScheme = item.fromCodeScheme;
        configs[i].toCodeScheme = item.toCodeScheme;
        configs[i].selected = item.selected == 1 ? true : false;
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getGsmBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getGsmBroadcastConfigResponse(responseInfo, configs);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetGsmBroadcastConfig(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setGsmBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setGsmBroadcastConfigResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetGsmBroadcastActivation(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setGsmBroadcastActivationResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setGsmBroadcastActivationResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCdmaBroadcastConfig(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilGetCdmaBroadcastConfigResult_t> broadcastConfigResp) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<CdmaBroadcastSmsConfigInfo> configs;
    if (errorCode == RIL_E_SUCCESS && broadcastConfigResp) {
      // generate config list
      auto num = broadcastConfigResp->configList.size();
      configs.resize(num);
      for (unsigned int i = 0; i < num; i++) {
        auto& item = broadcastConfigResp->configList[i];
        configs[i].serviceCategory = item.service_category;
        configs[i].language = item.language;
        configs[i].selected = item.selected == 1 ? true : false;
      }
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCdmaBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCdmaBroadcastConfigResponse(responseInfo, configs);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCdmaBroadcastConfig(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCdmaBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCdmaBroadcastConfigResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCdmaBroadcastActivation(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCdmaBroadcastActivationResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCdmaBroadcastActivationResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCDMASubscription(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionResult_t> rilCdmaSubResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_string mdns{};
    ::android::hardware::hidl_string hSid{};
    ::android::hardware::hidl_string hNid{};
    ::android::hardware::hidl_string min{};
    ::android::hardware::hidl_string prl{};
    if (errorCode == RIL_E_SUCCESS && rilCdmaSubResult) {
      mdns = rilCdmaSubResult->mdn;
      hSid = rilCdmaSubResult->hSid;
      hNid = rilCdmaSubResult->hNid;
      min = rilCdmaSubResult->min;
      prl = rilCdmaSubResult->prl;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCDMASubscriptionResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCDMASubscriptionResponse(responseInfo, mdns, hSid, hNid, min, prl);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForWriteSmsToRuim(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    int32 recordNumber = -1;
    if (errorCode == RIL_E_SUCCESS && writeSmsResponse) {
      recordNumber = writeSmsResponse->recordNumber;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("writeSmsToRuimResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->writeSmsToRuimResponse(responseInfo, recordNumber);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForDeleteSmsOnRuim(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("deleteSmsOnRuimResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->deleteSmsOnRuimResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetDeviceIdentity(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilDeviceIdentityResult_t> result) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_string imei{};
    ::android::hardware::hidl_string imeisv{};
    ::android::hardware::hidl_string esn{};
    ::android::hardware::hidl_string meid{};
    if (errorCode == RIL_E_SUCCESS && result) {
      imei = result->imei;
      imeisv = result->imeisv;
      esn = result->esn;
      meid = result->meid;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getDeviceIdentityResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getDeviceIdentityResponse(responseInfo, imei, imeisv, esn, meid);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForExitEmergencyCallbackMode(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("exitEmergencyCallbackModeResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->exitEmergencyCallbackModeResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetSmscAddress(int32_t serial, RIL_Errno errorCode,
                                             std::shared_ptr<RilGetSmscAddrResult_t> smscAddrResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_string smsc{};
    if (errorCode == RIL_E_SUCCESS && smscAddrResult) {
      smsc = smscAddrResult->smscAddr.c_str();
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getSmscAddressResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getSmscAddressResponse(responseInfo, smsc);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetSmscAddress(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setSmscAddressResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setSmscAddressResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForReportSmsMemoryStatus(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("reportSmsMemoryStatusResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->reportSmsMemoryStatusResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForReportStkServiceIsRunning(int32_t serial, V1_0::RadioError errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("reportStkServiceIsRunningResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->reportStkServiceIsRunningResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCdmaSubscriptionSource(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCdmaSubscriptionSourceResult_t> srcResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::CdmaSubscriptionSource pref = V1_0::CdmaSubscriptionSource::RUIM_SIM;
    if (errorCode == RIL_E_SUCCESS && srcResult) {
      pref = static_cast<V1_0::CdmaSubscriptionSource>(srcResult->mSource);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCdmaSubscriptionSourceResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCdmaSubscriptionSourceResponse(responseInfo, pref);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForRequestIsimAuthentication(
      int32_t serial, V1_0::RadioError errorCode,
      std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    ::android::hardware::hidl_string rsp{};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      rsp = respData->simResponse;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("requestIsimAuthenticationResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->requestIsimAuthenticationResponse(responseInfo, rsp);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForAcknowledgeIncomingGsmSmsWithPdu(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("acknowledgeIncomingGsmSmsWithPduResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->acknowledgeIncomingGsmSmsWithPduResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendEnvelopeWithStatus(
      int32_t serial, V1_0::RadioError errorCode,
      std::shared_ptr<RIL_GSTK_EnvelopeResponse> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    IccIoResult rsp_data{};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      rsp_data.simResponse = respData->rsp;
      rsp_data.sw1 = respData->sw1;
      rsp_data.sw2 = respData->sw2;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendEnvelopeWithStatusResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendEnvelopeWithStatusResponse(responseInfo, rsp_data);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetVoiceRadioTechnology(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetVoiceTechResult_t> ratResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    RadioTechnology rat = RadioTechnology::UNKNOWN;
    if (errorCode == RIL_E_SUCCESS && ratResult) {
      rat = static_cast<RadioTechnology>(ratResult->rat);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getVoiceRadioTechnologyResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getVoiceRadioTechnologyResponse(responseInfo, rat);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetCellInfoList(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_vec<V1_0::CellInfo> cellInfos;
    if (errorCode == RIL_E_SUCCESS && cellInfoListResult) {
      utils::convertRilCellInfoListToHal(cellInfoListResult->cellInfos, cellInfos);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getCellInfoListResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getCellInfoListResponse(responseInfo, cellInfos);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetCellInfoListRate(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setCellInfoListRateResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setCellInfoListRateResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetImsRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilGetImsRegistrationResult_t> regStateResp) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool isRegistered{ false };
    V1_0::RadioTechnologyFamily radioTechFamily{ V1_0::RadioTechnologyFamily::THREE_GPP };
    if (errorCode == RIL_E_SUCCESS && regStateResp) {
      isRegistered = regStateResp->isRegistered;
      radioTechFamily = regStateResp->ratFamily == RADIO_TECH_3GPP
                            ? V1_0::RadioTechnologyFamily::THREE_GPP
                            : V1_0::RadioTechnologyFamily::THREE_GPP2;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getImsRegistrationStateResponse: serial=%d, error=%d", serial, errorCode);
      auto ret =
          respCb->getImsRegistrationStateResponse(responseInfo, isRegistered, radioTechFamily);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSendImsSms(int32_t serial, RIL_Errno errorCode,
                                         std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    SendSmsResult result{ -1, ::android::hardware::hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }

    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendImsSmsResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->sendImsSmsResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForIccTransmitApduBasicChannel(
      int32_t serial, V1_0::RadioError errorCode,
      std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    IccIoResult result = {};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      result.sw1 = respData->sw1;
      result.sw2 = respData->sw2;
      result.simResponse = respData->simResponse;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("iccTransmitApduBasicChannelResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->iccTransmitApduBasicChannelResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForIccOpenLogicalChannel(
      int32_t serial, V1_0::RadioError errorCode,
      std::shared_ptr<RIL_UIM_OpenChannelResponse> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };

    ::android::hardware::hidl_vec<int8_t> selectResponse{};
    int32_t ch_id = -1;

    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      selectResponse.resize(respData->selectResponse.size());
      for (uint32_t i = 0; i < respData->selectResponse.size(); i++) {
        selectResponse[i] = respData->selectResponse[i];
      }
      ch_id = respData->channel_id;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("iccOpenLogicalChannelResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->iccOpenLogicalChannelResponse(responseInfo, ch_id, selectResponse);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForIccCloseLogicalChannel(int32_t serial, V1_0::RadioError errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial, errorCode };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("iccCloseLogicalChannelResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->iccCloseLogicalChannelResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForIccTransmitApduLogicalChannel(
      int32_t serial, V1_0::RadioError errorCode,
      std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    IccIoResult result = {};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      result.sw1 = respData->sw1;
      result.sw2 = respData->sw2;
      result.simResponse = respData->simResponse;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("iccTransmitApduLogicalChannelResponse: serial=%d, error=%d", serial,
                      errorCode);
      auto ret = respCb->iccTransmitApduLogicalChannelResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForNvReadItem(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("nvReadItemResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->nvReadItemResponse(responseInfo, ::android::hardware::hidl_string());
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForNvWriteItem(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("nvWriteItemResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->nvWriteItemResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForNvWriteCdmaPrl(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("nvWriteCdmaPrlResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->nvWriteCdmaPrlResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForNvResetConfig(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("nvResetConfigResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->nvResetConfigResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetUiccSubscription(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setUiccSubscriptionResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setUiccSubscriptionResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetDataAllowed(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setDataAllowedResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setDataAllowedResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetHardwareConfig(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    hidl_vec<HardwareConfig> result{};
    if (respCb) {
      QCRIL_LOG_DEBUG("getHardwareConfigResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getHardwareConfigResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForRequestIccSimAuthentication(
      int32_t serial, V1_0::RadioError errorCode,
      std::shared_ptr<RIL_UIM_SIM_IO_Response> respData) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          V1_0::RadioError::INTERNAL_ERR };
    IccIoResult result{};
    if (errorCode == V1_0::RadioError::NONE && respData) {
      responseInfo.error = static_cast<V1_0::RadioError>(respData->err);
      result.sw1 = respData->sw1;
      result.sw2 = respData->sw2;
      result.simResponse = respData->simResponse;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("requestIccSimAuthenticationResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->requestIccSimAuthenticationResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForRequestShutdown(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("requestShutdownResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->requestShutdownResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetRadioCapability(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    RadioCapability result{};
    if (errorCode == RIL_E_SUCCESS && rilCapResult) {
      utils::convertRilRadioCapabilityToHal(result, rilCapResult->respData);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getRadioCapabilityResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getRadioCapabilityResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetRadioCapability(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    RadioCapability result{};
    if (errorCode == RIL_E_SUCCESS && rilCapResult) {
      utils::convertRilRadioCapabilityToHal(result, rilCapResult->respData);
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setRadioCapabilityResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setRadioCapabilityResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetModemActivityInfo(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetModemActivityResult_t> result) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_0::ActivityStatsInfo info{};
    if (errorCode == RIL_E_SUCCESS && result) {
      RIL_ActivityStatsInfo resp = result->respData;
      info.sleepModeTimeMs = resp.sleep_mode_time_ms;
      info.idleModeTimeMs = resp.idle_mode_time_ms;
      for (int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
        info.txmModetimeMs[i] = resp.tx_mode_time_ms[i];
      }
      info.rxModeTimeMs = resp.rx_mode_time_ms;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getModemActivityInfoResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getModemActivityInfoResponse(responseInfo, info);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetAllowedCarriers(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setAllowedCarriersResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setAllowedCarriersResponse(responseInfo, 0);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetAllowedCarriers(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    bool allAllowed{ false };
    ::android::hardware::radio::V1_0::CarrierRestrictions carriers{};
    if (respCb) {
      QCRIL_LOG_DEBUG("getAllowedCarriersResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getAllowedCarriersResponse(responseInfo, allAllowed, carriers);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetIndicationFilter(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setIndicationFilterResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setIndicationFilterResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetSimCardPower(int32_t serial, V1_0::RadioError errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial, errorCode };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setSimCardPowerResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setSimCardPowerResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

 public:
  RadioServiceImpl() = default;

  virtual ~RadioServiceImpl() = default;

  static const HalServiceImplVersion& getVersion();

  bool registerService(qcril_instance_id_e_type instId) {
    this->setInstanceId(instId);

    std::string serviceName = "slot" + std::to_string(this->getInstanceId() + 1);
    android::status_t ret = T::registerAsService(serviceName);
    Log::getInstance().d(std::string("[Radio]: ") + T::descriptor +
                         (ret == android::OK ? " registered" : " failed to register"));
    return (ret == android::OK);
  }

  bool mIsScanRequested = false;

  Return<void> setResponseFunctions(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    {
      std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
      this->setResponseFunctions_nolock(respCb, indCb);
    }
#ifndef QMI_RIL_UTF
    // client is connected. Send initial indications.
    android::onNewCommandConnect();
#endif
    return Void();
  }

  Return<void> getIccCardStatus(int32_t serial) {
    QCRIL_LOG_DEBUG("getIccCardStatus: serial=%d", serial);
    auto msg = std::make_shared<UimGetCardStatusRequestMsg>(this->getInstanceId());
    if (msg) {
      GenericCallback<RIL_UIM_CardStatus> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_CardStatus> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForGetIccCardStatus(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetIccCardStatus(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> supplyIccPinForApp(int32_t serial, const ::android::hardware::hidl_string& pin,
                                  const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("supplyIccPinForApp: serial=%d", serial);
    auto msg = std::make_shared<UimEnterSimPinRequestMsg>(RIL_UIM_SIM_PIN1, pin, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSupplyIccPinForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSupplyIccPinForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> supplyIccPukForApp(int32_t serial, const ::android::hardware::hidl_string& puk,
                                  const ::android::hardware::hidl_string& pin,
                                  const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("supplyIccPukForApp: serial=%d", serial);
    auto msg = std::make_shared<UimEnterSimPukRequestMsg>(RIL_UIM_SIM_PUK1, puk, pin, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSupplyIccPukForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSupplyIccPukForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> supplyIccPin2ForApp(int32_t serial, const ::android::hardware::hidl_string& pin2,
                                   const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("supplyIccPin2ForApp: serial=%d", serial);
    auto msg = std::make_shared<UimEnterSimPinRequestMsg>(RIL_UIM_SIM_PIN2, pin2, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSupplyIccPin2ForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSupplyIccPin2ForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> supplyIccPuk2ForApp(int32_t serial, const ::android::hardware::hidl_string& puk2,
                                   const ::android::hardware::hidl_string& pin2,
                                   const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("supplyIccPuk2ForApp: serial=%d", serial);
    auto msg = std::make_shared<UimEnterSimPukRequestMsg>(RIL_UIM_SIM_PUK2, puk2, pin2, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSupplyIccPuk2ForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSupplyIccPuk2ForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> changeIccPinForApp(int32_t serial, const ::android::hardware::hidl_string& oldPin,
                                  const ::android::hardware::hidl_string& newPin,
                                  const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("changeIccPinForApp: serial=%d", serial);
    auto msg = std::make_shared<UimChangeSimPinRequestMsg>(RIL_UIM_SIM_PIN1, newPin, oldPin, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForChangeIccPinForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForChangeIccPinForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> changeIccPin2ForApp(int32_t serial, const ::android::hardware::hidl_string& oldPin2,
                                   const ::android::hardware::hidl_string& newPin2,
                                   const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("changeIccPin2ForApp: serial=%d", serial);
    auto msg = std::make_shared<UimChangeSimPinRequestMsg>(RIL_UIM_SIM_PIN2, newPin2, oldPin2, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_PIN_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_PIN_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForChangeIccPin2ForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForChangeIccPin2ForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> supplyNetworkDepersonalization(int32_t serial,
                                              const ::android::hardware::hidl_string& netPin) {
    QCRIL_LOG_DEBUG("supplyNetworkDepersonalization: serial=%d", serial);
    auto msg =
        std::make_shared<UimEnterDePersoRequestMsg>(netPin, RIL_UIM_PERSOSUBSTATE_SIM_NETWORK);
    if (msg) {
      GenericCallback<RIL_UIM_PersoResponse> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSupplyNetworkDepersonalization(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSupplyNetworkDepersonalization(serial, V1_0::RadioError::NO_MEMORY,
                                                          nullptr);
    }
    return Void();
  }

  Return<void> getCurrentCalls(int32_t serial) {
    QCRIL_LOG_DEBUG("getCurrentCalls: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestGetCurrentCallsMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              currentCalls =
                  std::static_pointer_cast<qcril::interfaces::GetCurrentCallsRespData>(resp->data);
            }
            this->sendResponseForGetCurrentCalls(serial, errorCode, currentCalls);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetCurrentCalls(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> dial(int32_t serial, const V1_0::Dial& dialInfo) {
    QCRIL_LOG_DEBUG("dial: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestDialMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      if (!dialInfo.address.empty()) {
        msg->setAddress(dialInfo.address.c_str());
      }
      msg->setClir((int)dialInfo.clir);
      msg->setCallDomain(qcril::interfaces::CallDomain::CS);
      if (dialInfo.uusInfo.size() != 0) {
        auto uusInfo = std::make_shared<qcril::interfaces::UusInfo>();
        if (uusInfo) {
          uusInfo->setType((RIL_UUS_Type)dialInfo.uusInfo[0].uusType);
          uusInfo->setDcs((RIL_UUS_DCS)dialInfo.uusInfo[0].uusDcs);
          if (dialInfo.uusInfo[0].uusData.size() > 0) {
            uusInfo->setData(std::string(dialInfo.uusInfo[0].uusData.c_str(),
                                         dialInfo.uusInfo[0].uusData.size()));
          }
          msg->setUusInfo(uusInfo);
        }
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForDial(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForDial(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getImsiForApp(int32_t serial, const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("getImsiForApp: serial=%d", serial);
    auto msg = std::make_shared<UimGetImsiRequestMsg>(this->getInstanceId(), aid);
    if (msg) {
      GenericCallback<RIL_UIM_IMSI_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_IMSI_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForGetImsiForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetImsiForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> hangup(int32_t serial, int32_t gsmIndex) {
    QCRIL_LOG_DEBUG("hangup: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestHangupMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      msg->setCallIndex(gsmIndex);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForHangup(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForHangup(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> hangupWaitingOrBackground(int32_t serial) {
    QCRIL_LOG_DEBUG("hangupWaitingOrBackground: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg =
          std::make_shared<QcRilRequestHangupWaitingOrBackgroundMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForHangupWaitingOrBackground(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForHangupWaitingOrBackground(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> hangupForegroundResumeBackground(int32_t serial) {
    QCRIL_LOG_DEBUG("hangupForegroundResumeBackground: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestHangupForegroundResumeBackgroundMessage>(
          this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForHangupForegroundResumeBackground(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForHangupForegroundResumeBackground(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> switchWaitingOrHoldingAndActive(int32_t serial) {
    QCRIL_LOG_DEBUG("switchWaitingOrHoldingAndActive: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestSwitchWaitingOrHoldingAndActiveMessage>(
          this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSwitchWaitingOrHoldingAndActive(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSwitchWaitingOrHoldingAndActive(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> conference(int32_t serial) {
    QCRIL_LOG_DEBUG("conference: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestConferenceMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForConference(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForConference(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> rejectCall(int32_t serial) {
    QCRIL_LOG_DEBUG("rejectCall: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestUdubMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForRejectCall(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForRejectCall(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getLastCallFailCause(int32_t serial) {
    QCRIL_LOG_DEBUG("getLastCallFailCause: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestLastCallFailCauseMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::LastCallFailCauseInfo> failCause{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              failCause =
                  std::static_pointer_cast<qcril::interfaces::LastCallFailCauseInfo>(resp->data);
            }
            this->sendResponseForGetLastCallFailCause(serial, errorCode, failCause);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetLastCallFailCause(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getVoiceRegistrationState(int32_t serial) {
    QCRIL_LOG_DEBUG("getVoiceRegistrationState: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetVoiceRegistrationMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilRegResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetVoiceRegResult_t>(resp->data);
            }
            this->sendResponseForGetVoiceRegistrationState(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetVoiceRegistrationState(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getDataRegistrationState(int32_t serial) {
    QCRIL_LOG_DEBUG("getDataRegistrationState: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetDataRegistrationMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilRegResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetDataRegResult_t>(resp->data);
            }
            this->sendResponseForGetDataRegistrationState(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetDataRegistrationState(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getSignalStrength(int32_t serial) {
    QCRIL_LOG_DEBUG("getSignalStrength: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetSignalStrengthMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilSigResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetSignalStrengthResult_t>(
                      resp->data);
            }
            this->sendResponseForGetSignalStrength(serial, errorCode, rilSigResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetSignalStrength(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getOperator(int32_t serial) {
    QCRIL_LOG_DEBUG("getOperator: serial=%d", serial);
    auto msg = std::make_shared<RilRequestOperatorMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetOperatorResult_t> rilOperatorResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilOperatorResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetOperatorResult_t>(resp->data);
            }
            this->sendResponseForGetOperator(serial, errorCode, rilOperatorResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetOperator(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> setRadioPower(int32_t serial, bool on) {
    QCRIL_LOG_DEBUG("setRadioPower: serial=%d on %d", serial, on);
    auto msg = std::make_shared<RilRequestRadioPowerMessage>(this->getContext(serial), on);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetRadioPower(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetRadioPower(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> sendDtmf(int32_t serial, const ::android::hardware::hidl_string& s) {
    QCRIL_LOG_DEBUG("sendDtmf: serial=%d", serial);
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      auto msg = std::make_shared<QcRilRequestSendDtmfMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      if (s.empty()) {
        QCRIL_LOG_ERROR("Invalid parameter: s");
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
      }
      msg->setDigit(s.c_str()[0]);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSendDtmf(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForSendDtmf(serial, errResp);
    }
    return Void();
  }

  Return<void> sendSms(int32_t serial, const GsmSmsMessage& message) {
    QCRIL_LOG_DEBUG("sendSms: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSendSmsMessage>(this->getContext(serial), message.smscPdu,
                                                          message.pdu);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilSendSmsResult_t> sendSmsResult = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
            }
            this->sendResponseForSendSms(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendSms(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> sendSMSExpectMore(int32_t serial, const GsmSmsMessage& message) {
    QCRIL_LOG_DEBUG("sendSMSExpectMore: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSendSmsMessage>(this->getContext(serial), message.smscPdu,
                                                          message.pdu, true);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilSendSmsResult_t> sendSmsResult = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
            }
            this->sendResponseForSendSMSExpectMore(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendSMSExpectMore(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> setupDataCall(int32_t serial, RadioTechnology radioTechnology,
                             const DataProfileInfo& dataProfileInfo, bool modemCognitive,
                             bool roamingAllowed, bool isRoaming) {
    QCRIL_LOG_DEBUG("setupDataCall: serial %d", serial);
    using namespace rildata;
    using namespace utils;
    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;

    AccessNetwork_t accnet = convertRadioTechToAccessNetwork(radioTechnology);
    DataProfileInfo_t profile = convertHidlDataProfileInfoToRil_1_0(dataProfileInfo);
    profile.persistent = modemCognitive;

    auto msg =
      std::make_shared<SetupDataCallRequestMessage>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        profile,
        roamingAllowed,
        DataRequestReason_t::NORMAL,
        radioAddresses,
        radioDnses,
        nullptr);

      if (msg) {
        GenericCallback<SetupDataCallResponse_t> cb([serial](std::shared_ptr<Message>,
                                                             Message::Callback::Status status,
                                                             std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
            if (rsp != nullptr) {
                auto indMsg = std::make_shared<SetupDataCallRadioResponseIndMessage>(*rsp, serial, status);
                if (indMsg != nullptr) {
                    indMsg->broadcast();
                } else {
                    QCRIL_LOG_DEBUG("setup data call cb failed to allocate message status %d respErr %d", status, rsp->respErr);
                }
            } else {
                QCRIL_LOG_ERROR("setupDataCall_1_0 resp is nullptr");
            }
         });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  Return<void> iccIOForApp(int32_t serial, const IccIo& iccIo) {
    QCRIL_LOG_DEBUG("iccIOForApp: serial=%d", serial);

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
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForIccIOForApp(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForIccIOForApp(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> sendUssd(int32_t serial, const ::android::hardware::hidl_string& ussd) {
    QCRIL_LOG_DEBUG("sendUssd: serial=%d", serial);
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      auto msg = std::make_shared<QcRilRequestSendUssdMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      if (ussd.empty()) {
        QCRIL_LOG_ERROR("Invalid parameter: ussd");
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
      }
      if (utils::isUssdOverImsSupported()) {
        msg->setIsDomainAuto(false);
      } else {
        msg->setIsDomainAuto(true);
      }
      msg->setUssd(ussd.c_str());
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSendUssd(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForSendUssd(serial, errResp);
    }
    return Void();
  }

  Return<void> cancelPendingUssd(int32_t serial) {
    QCRIL_LOG_DEBUG("cancelPendingUssd: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestCancelUssdMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForCancelPendingUssd(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForCancelPendingUssd(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getClir(int32_t serial) {
    QCRIL_LOG_DEBUG("getClir: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestGetClirMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::ClirInfo> clirInfo{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              clirInfo = std::static_pointer_cast<qcril::interfaces::ClirInfo>(resp->data);
            }
            this->sendResponseForGetClir(serial, errorCode, clirInfo);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetClir(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setClir(int32_t serial, int32_t status) {
    QCRIL_LOG_DEBUG("setClir: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestSetClirMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      msg->setParamN(status);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetClir(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetClir(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getCallForwardStatus(int32_t serial, const CallForwardInfo& callInfo) {
    QCRIL_LOG_DEBUG("getCallForwardStatus: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      // Set parameters
      if (callInfo.reason != INT32_MAX) {
        msg->setReason(callInfo.reason);
      }
      if (callInfo.serviceClass != INT32_MAX) {
        msg->setServiceClass(callInfo.serviceClass);
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::GetCallForwardRespData> callFwdData{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              callFwdData =
                  std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
            }
            this->sendResponseForGetCallForwardStatus(serial, errorCode, callFwdData);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetCallForwardStatus(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setCallForward(int32_t serial, const CallForwardInfo& callInfo) {
    QCRIL_LOG_DEBUG("setCallForward: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestSetCallForwardMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      // Set parameters
      msg->setStatus(utils::convertCallForwardInfoStatus(callInfo.status));
      msg->setReason(callInfo.reason);
      msg->setServiceClass(callInfo.serviceClass);
      msg->setToa(callInfo.toa);
      if (!callInfo.number.empty()) {
        msg->setNumber(callInfo.number.c_str());
      }
      msg->setTimeSeconds(callInfo.timeSeconds);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetCallForward(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetCallForward(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getCallWaiting(int32_t serial, int32_t serviceClass) {
    QCRIL_LOG_DEBUG("getCallWaiting: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestGetCallWaitingMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      // Set parameters
      msg->setServiceClass(serviceClass);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::CallWaitingInfo> cwInfo{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              cwInfo = std::static_pointer_cast<qcril::interfaces::CallWaitingInfo>(resp->data);
            }
            this->sendResponseForGetCallWaiting(serial, errorCode, cwInfo);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetCallWaiting(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) {
    QCRIL_LOG_DEBUG("setCallWaiting: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestSetCallWaitingMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      // Set parameters
      if (enable) {
        msg->setServiceStatus(qcril::interfaces::ServiceClassStatus::ENABLED);
      } else {
        msg->setServiceStatus(qcril::interfaces::ServiceClassStatus::DISABLED);
      }
      msg->setServiceClass(serviceClass);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetCallWaiting(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetCallWaiting(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> acknowledgeLastIncomingGsmSms(int32_t serial, bool success,
                                             SmsAcknowledgeFailCause cause) {
    QCRIL_LOG_DEBUG("acknowledgeLastIncomingGsmSms: serial=%d", serial);
    auto msg = std::make_shared<RilRequestAckGsmSmsMessage>(this->getContext(serial), success,
                                                            static_cast<int32_t>(cause));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForAcknowledgeLastIncomingGsmSms(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForAcknowledgeLastIncomingGsmSms(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> acceptCall(int32_t serial) {
    QCRIL_LOG_DEBUG("acceptCall: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestAnswerMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForAcceptCall(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForAcceptCall(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> deactivateDataCall(int32_t serial, int32_t cid, bool reasonRadioShutDown) {
    QCRIL_LOG_DEBUG("deactivateDataCall: serial %d", serial);
    using namespace rildata;
    DataRequestReason_t reason = DataRequestReason_t::NORMAL;
    if (reasonRadioShutDown == true)
    {
        QCRIL_LOG_DEBUG("Deactivate data call initiated with request reason SHUTDOWN");
        reason = DataRequestReason_t::SHUTDOWN;
    }

    auto msg = std::make_shared<DeactivateDataCallRequestMessage>(
        serial,
        cid,
        reason,
        nullptr);
    if (msg)
    {
        GenericCallback<ResponseError_t> cb([serial](std::shared_ptr<Message>,
                                                     Message::Callback::Status status,
                                                     std::shared_ptr<ResponseError_t> rsp) -> void {
            if (rsp != nullptr)
            {
                auto indMsg = std::make_shared<DeactivateDataCallRadioResponseIndMessage>(*rsp, serial, status);
                if (indMsg != nullptr)
                {
                    indMsg->broadcast();
                }
                else
                {
                    QCRIL_LOG_DEBUG("deactivate data call cb failed to allocate message status %d respErr %d", status, *rsp);
                }
            }
            else
            {
                QCRIL_LOG_ERROR("deactivate data call resp is nullptr");
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  Return<void> getFacilityLockForApp(int32_t serial,
                                     const ::android::hardware::hidl_string& facility,
                                     const ::android::hardware::hidl_string& password,
                                     int32_t serviceClass,
                                     const ::android::hardware::hidl_string& appId) {
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
              this->sendResponseForGetFacilityLockForApp(serial, errorCode, lockStatus);
            });
        msg->setCallback(&cb);
        msg->dispatch();
        break;
      }
      qcril::interfaces::FacilityType rilFacility = utils::convertFacilityType(facility);
      if (rilFacility != qcril::interfaces::FacilityType::UNKNOWN) {
        auto queryMsg =
            std::make_shared<QcRilRequestGetCallBarringMessage>(this->getContext(serial));
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
              this->sendResponseForGetFacilityLockForApp(serial, errorCode, serviceClass);
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
      this->sendResponseForGetFacilityLockForApp(serial, errResp, 0);
    }
    return Void();
  }

  Return<void> setFacilityLockForApp(int32_t serial,
                                     const ::android::hardware::hidl_string& facility,
                                     bool lockState,
                                     const ::android::hardware::hidl_string& password,
                                     int32_t serviceClass,
                                     const ::android::hardware::hidl_string& appId) {
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
      qcril::interfaces::FacilityType rilFacility = utils::convertFacilityType(facility);
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
    return Void();
  }

  Return<void> setBarringPassword(int32_t serial, const ::android::hardware::hidl_string& facility,
                                  const ::android::hardware::hidl_string& oldPassword,
                                  const ::android::hardware::hidl_string& newPassword) {
    QCRIL_LOG_DEBUG("setBarringPassword: serial=%d, facility=%s", serial, facility.c_str());
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      auto msg =
          std::make_shared<QcRilRequestSetCallBarringPasswordMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      // Set parameters
      if (!facility.empty()) {
        msg->setFacilityType(utils::convertFacilityType(facility));
      } else {
        QCRIL_LOG_ERROR("invalid parameter: facility");
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
      }
      if (!oldPassword.empty()) {
        msg->setOldPassword(oldPassword.c_str());
      }
      if (!newPassword.empty()) {
        msg->setNewPassword(newPassword.c_str());
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetBarringPassword(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForSetBarringPassword(serial, errResp);
    }
    return Void();
  }

  Return<void> getNetworkSelectionMode(int32_t serial) {
    QCRIL_LOG_DEBUG("getNetworkSelectionMode: serial=%d", serial);
    auto msg = std::make_shared<RilRequestQueryNetworkSelectModeMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilModeResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetSelectModeResult_t>(resp->data);
            }
            this->sendResponseForGetNetworkSelectionMode(serial, errorCode, rilModeResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetNetworkSelectionMode(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> setNetworkSelectionModeAutomatic(int32_t serial) {
    QCRIL_LOG_DEBUG("setNetworkSelectionModeAutomatic: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetNetworkSelectionAutoMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetNetworkSelectionModeAutomatic(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetNetworkSelectionModeAutomatic(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> setNetworkSelectionModeManual(
      int32_t serial, const ::android::hardware::hidl_string& operatorNumeric) {
    QCRIL_LOG_DEBUG("setNetworkSelectionModeManual: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(
        this->getContext(serial), operatorNumeric);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetNetworkSelectionModeManual(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetNetworkSelectionModeManual(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getAvailableNetworks(int32_t serial) {
    QCRIL_LOG_DEBUG("getAvailableNetworks: serial=%d", serial);
    auto msg = std::make_shared<RilRequestQueryAvailNetworkMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetAvailNetworkResult_t> rilNetworkResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilNetworkResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetAvailNetworkResult_t>(
                      resp->data);
            }
            this->sendResponseForGetAvailableNetworks(serial, errorCode, rilNetworkResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetAvailableNetworks(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> startDtmf(int32_t serial, const ::android::hardware::hidl_string& s) {
    QCRIL_LOG_DEBUG("startDtmf: serial=%d, s=%s", serial, s.c_str());
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      auto msg = std::make_shared<QcRilRequestStartDtmfMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      if (s.empty()) {
        QCRIL_LOG_ERROR("Invalid parameter: s");
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
      }
      msg->setDigit(s.c_str()[0]);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            } else if (status == Message::Callback::Status::CANCELLED) {
              errorCode = RIL_E_CANCELLED;
            }
            this->sendResponseForStartDtmf(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForStartDtmf(serial, errResp);
    }
    return Void();
  }

  Return<void> stopDtmf(int32_t serial) {
    QCRIL_LOG_DEBUG("stopDtmf: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestStopDtmfMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            } else if (status == Message::Callback::Status::CANCELLED) {
              errorCode = RIL_E_CANCELLED;
            }
            this->sendResponseForStopDtmf(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForStopDtmf(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getBasebandVersion(int32_t serial) {
    QCRIL_LOG_DEBUG("getBasebandVersion: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetBaseBandVersionMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetBaseBandResult_t> result{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              result =
                  std::static_pointer_cast<qcril::interfaces::RilGetBaseBandResult_t>(resp->data);
            }
            this->sendResponseForGetBasebandVersion(serial, errorCode, result);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetBasebandVersion(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> separateConnection(int32_t serial, int32_t gsmIndex) {
    QCRIL_LOG_DEBUG("separateConnection: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestSeparateConnectionMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      msg->setCallId(gsmIndex);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSeparateConnection(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSeparateConnection(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> setMute(int32_t serial, bool enable) {
    QCRIL_LOG_DEBUG("setMute: serial=%d, enable=%d", serial, enable);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestSetMuteMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      msg->setEnable(enable);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetMute(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetMute(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getMute(int32_t serial) {
    QCRIL_LOG_DEBUG("getMute: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestGetMuteMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::MuteInfo> muteInfo{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              muteInfo = std::static_pointer_cast<qcril::interfaces::MuteInfo>(resp->data);
            }
            this->sendResponseForGetMute(serial, errorCode, muteInfo);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetMute(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getClip(int32_t serial) {
    QCRIL_LOG_DEBUG("getClip: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestGetClipMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::ClipInfo> clipInfo{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              clipInfo = std::static_pointer_cast<qcril::interfaces::ClipInfo>(resp->data);
            }
            this->sendResponseForGetClip(serial, errorCode, clipInfo);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetClip(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getDataCallList(int32_t serial) {
    QCRIL_LOG_DEBUG("getDataCallList: serial %d", serial);
    using namespace rildata;
    QCRIL_LOG_DEBUG("RIL_REQUEST_DATA_CALL_LIST");
    auto msg = std::make_shared<GetRadioDataCallListRequestMessage>();
    GenericCallback<DataCallListResult_t> cb(
        ([serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
            auto msg = std::make_shared<rildata::GetDataCallListResponseIndMessage>
                             (responseDataPtr, serial, status);
            msg->broadcast();
        }));
    if (msg)
    {
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  Return<void> setSuppServiceNotifications(int32_t serial, bool enable) {
    QCRIL_LOG_DEBUG("setSuppServiceNotifications: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg =
          std::make_shared<QcRilRequestSetSuppSvcNotificationMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      // Set parameters
      msg->setStatus(enable);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetSuppServiceNotifications(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetSuppServiceNotifications(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> writeSmsToSim(int32_t serial, const SmsWriteArgs& smsWriteArgs) {
    QCRIL_LOG_DEBUG("writeSmsToSim: serial=%d", serial);
    auto msg = std::make_shared<RilRequestWriteSmsToSimMessage>(
        this->getContext(serial), smsWriteArgs.smsc, smsWriteArgs.pdu,
        static_cast<int>(smsWriteArgs.status));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              writeSmsResponse = std::static_pointer_cast<RilWriteSmsToSimResult_t>(resp->data);
            }
            this->sendResponseForWriteSmsToSim(serial, errorCode, writeSmsResponse);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForWriteSmsToSim(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> deleteSmsOnSim(int32_t serial, int32_t index) {
    QCRIL_LOG_DEBUG("deleteSmsOnSim: serial=%d", serial);
    auto msg = std::make_shared<RilRequestDeleteSmsOnSimMessage>(this->getContext(serial), index);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForDeleteSmsOnSim(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForDeleteSmsOnSim(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> setBandMode(int32_t serial, RadioBandMode mode) {
    QCRIL_LOG_DEBUG("setBandMode: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetBandModeMessage>(this->getContext(serial),
                                                              static_cast<int>(mode));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetBandMode(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetBandMode(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> getAvailableBandModes(int32_t serial) {
    QCRIL_LOG_DEBUG("getAvailableBandModes: serial=%d", serial);
    auto msg = std::make_shared<RilRequestQueryAvailBandModeMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilQueryAvailBandsResult_t> rilBandResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilBandResult =
                  std::static_pointer_cast<qcril::interfaces::RilQueryAvailBandsResult_t>(
                      resp->data);
            }
            this->sendResponseForGetAvailableBandModes(serial, errorCode, rilBandResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetAvailableBandModes(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> sendEnvelope(int32_t serial, const ::android::hardware::hidl_string& command) {
    QCRIL_LOG_DEBUG("sendEnvelope: serial=%d", serial);
    auto msg = std::make_shared<GstkSendEnvelopeRequestMsg>(serial, command);
    if (msg) {
      GenericCallback<RIL_GSTK_EnvelopeResponse> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_GSTK_EnvelopeResponse> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSendEnvelope(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendEnvelope(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> sendTerminalResponseToSim(int32_t serial,
                                         const ::android::hardware::hidl_string& commandResponse) {
    QCRIL_LOG_DEBUG("sendTerminalResponseToSim: serial=%d", serial);
    auto msg = std::make_shared<GstkSendTerminalResponseRequestMsg>(serial, commandResponse);
    if (msg) {
      GenericCallback<RIL_GSTK_Errno> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
              errorCode = static_cast<V1_0::RadioError>(*responseDataPtr);
            }
            this->sendResponseForSendTerminalResponseToSim(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendTerminalResponseToSim(serial, V1_0::RadioError::NO_MEMORY);
    }
    return Void();
  }

  Return<void> handleStkCallSetupRequestFromSim(int32_t serial, bool accept) {
    QCRIL_LOG_DEBUG("handleStkCallSetupRequestFromSim: serial=%d", serial);
    auto msg = std::make_shared<GstkSendSetupCallResponseRequestMsg>(accept);
    if (msg) {
      GenericCallback<RIL_GSTK_Errno> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
              errorCode = static_cast<V1_0::RadioError>(*responseDataPtr);
            }
            this->sendResponseForHandleStkCallSetupRequestFromSim(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForHandleStkCallSetupRequestFromSim(serial, V1_0::RadioError::NO_MEMORY);
    }
    return Void();
  }

  Return<void> explicitCallTransfer(int32_t serial) {
    QCRIL_LOG_DEBUG("explicitCallTransfer: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestExplicitCallTransferMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForExplicitCallTransfer(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForExplicitCallTransfer(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> setPreferredNetworkType(int32_t serial, PreferredNetworkType nwType) {
    QCRIL_LOG_DEBUG("setPreferredNetworkType: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetPrefNetworkTypeMessage>(
        this->getContext(serial), utils::convertNwTypeToRadioAccessFamily(nwType));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetPreferredNetworkType(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetPreferredNetworkType(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> getPreferredNetworkType(int32_t serial) {
    QCRIL_LOG_DEBUG("getPreferredNetworkType: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetPrefNetworkTypeMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult{};
            if (status == Message::Callback::Status::SUCCESS) {
              if (resp) {
                errorCode = resp->errorCode;
                prefResult =
                    std::static_pointer_cast<qcril::interfaces::RilGetModePrefResult_t>(resp->data);
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
            this->sendResponseForGetPreferredNetworkType(serial, errorCode, prefResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetPreferredNetworkType(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> getNeighboringCids(int32_t serial) {
    QCRIL_LOG_DEBUG("getNeighboringCids: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetNeighborCellIdsMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetNeighborCellIdResult_t> rilCellResult{};
            if (status == Message::Callback::Status::SUCCESS) {
              if (resp) {
                errorCode = resp->errorCode;
                rilCellResult =
                    std::static_pointer_cast<qcril::interfaces::RilGetNeighborCellIdResult_t>(
                        resp->data);
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
            this->sendResponseForGetNeighboringCids(serial, errorCode, rilCellResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetNeighboringCids(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  Return<void> setLocationUpdates(int32_t serial, bool enable) {
    QCRIL_LOG_DEBUG("setLocationUpdates: serial=%d", serial);
    auto msg =
        std::make_shared<RilRequestSetLocationUpdateMessage>(this->getContext(serial), enable);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetLocationUpdates(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetLocationUpdates(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> setCdmaSubscriptionSource(int32_t serial, V1_0::CdmaSubscriptionSource cdmaSub) {
    QCRIL_LOG_DEBUG("setCdmaSubscriptionSource: serial=%d", serial);
    auto source = (cdmaSub == V1_0::CdmaSubscriptionSource::RUIM_SIM)
                      ? RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource::SIM
                      : RilRequestSetCdmaSubscriptionSourceMessage::SubscriptionSource::NV;
    auto msg = std::make_shared<RilRequestSetCdmaSubscriptionSourceMessage>(
        this->getContext(serial), source);
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

    return Void();
  }

  Return<void> setCdmaRoamingPreference(int32_t serial, V1_0::CdmaRoamingType type) {
    QCRIL_LOG_DEBUG("setCdmaRoamingPreference: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetCdmaRoamingPrefMessage>(this->getContext(serial),
                                                                     static_cast<int>(type));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetCdmaRoamingPreference(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetCdmaRoamingPreference(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> getCdmaRoamingPreference(int32_t serial) {
    QCRIL_LOG_DEBUG("getCdmaRoamingPreference: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<RilRequestQueryCdmaRoamingPrefMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t> roamPref{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              roamPref =
                  std::static_pointer_cast<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t>(
                      resp->data);
            }
            this->sendResponseForGetCdmaRoamingPreference(serial, errorCode, roamPref);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetCdmaRoamingPreference(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setTTYMode(int32_t serial, TtyMode mode) {
    QCRIL_LOG_DEBUG("setTTYMode: serial=%d", serial);
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      auto msg = std::make_shared<QcRilRequestSetTtyModeMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        errResp = RIL_E_NO_MEMORY;
        break;
      }
#undef OFF
      qcril::interfaces::TtyMode ttyMode = qcril::interfaces::TtyMode::UNKNOWN;
      switch (mode) {
        case TtyMode::OFF:
          ttyMode = qcril::interfaces::TtyMode::MODE_OFF;
          break;
        case TtyMode::FULL:
          ttyMode = qcril::interfaces::TtyMode::FULL;
          break;
        case TtyMode::HCO:
          ttyMode = qcril::interfaces::TtyMode::HCO;
          break;
        case TtyMode::VCO:
          ttyMode = qcril::interfaces::TtyMode::VCO;
          break;
        default:
          QCRIL_LOG_ERROR("Invlid TtyMode");
          errResp = RIL_E_INVALID_ARGUMENTS;
          break;
      }
      if (errResp != RIL_E_SUCCESS) {
        break;
      }
      msg->setTtyMode(ttyMode);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetTTYMode(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForSetTTYMode(serial, errResp);
    }
    return Void();
  }

  Return<void> getTTYMode(int32_t serial) {
    QCRIL_LOG_DEBUG("getTTYMode: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestGetTtyModeMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::TtyModeResp> ttyModeResp{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              ttyModeResp = std::static_pointer_cast<qcril::interfaces::TtyModeResp>(resp->data);
            }
            this->sendResponseForGetTTYMode(serial, errorCode, ttyModeResp);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetTTYMode(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setPreferredVoicePrivacy(int32_t serial, bool enable) {
    QCRIL_LOG_DEBUG("setPreferredVoicePrivacy: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg =
          std::make_shared<QcRilRequestSetPreferredVoicePrivacyMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      if (enable) {
        msg->setPrivacyMode(qcril::interfaces::PrivacyMode::ENHANCED);
      } else {
        msg->setPrivacyMode(qcril::interfaces::PrivacyMode::STANDARD);
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetPreferredVoicePrivacy(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetPreferredVoicePrivacy(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getPreferredVoicePrivacy(int32_t serial) {
    QCRIL_LOG_DEBUG("getPreferredVoicePrivacy: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg =
          std::make_shared<QcRilRequestGetPreferredVoicePrivacyMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::PrivacyModeResp> privacyModeResp{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              privacyModeResp =
                  std::static_pointer_cast<qcril::interfaces::PrivacyModeResp>(resp->data);
            }
            this->sendResponseForGetPreferredVoicePrivacy(serial, errorCode, privacyModeResp);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetPreferredVoicePrivacy(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> sendCDMAFeatureCode(int32_t serial,
                                   const ::android::hardware::hidl_string& featureCode) {
    QCRIL_LOG_DEBUG("sendCDMAFeatureCode: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestCdmaFlashMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      if (!featureCode.empty()) {
        msg->setFeatureCode(featureCode.c_str());
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSendCDMAFeatureCode(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSendCDMAFeatureCode(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> sendBurstDtmf(int32_t serial, const ::android::hardware::hidl_string& dtmf,
                             int32_t on, int32_t off) {
    QCRIL_LOG_DEBUG("sendBurstDtmf: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg = std::make_shared<QcRilRequestCdmaBurstDtmfMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      if (!dtmf.empty()) {
        msg->setDigitBuffer(dtmf.c_str());
      }
      msg->setDtmfOnLength(on);
      msg->setDtmfOffLength(off);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSendBurstDtmf(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSendBurstDtmf(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> sendCdmaSms(int32_t serial, const CdmaSmsMessage& sms) {
    QCRIL_LOG_DEBUG("sendCdmaSms: serial=%d", serial);
    RIL_CDMA_SMS_Message rcsm{};
    utils::constructCdmaSms(rcsm, sms);
    auto msg = std::make_shared<RilRequestCdmaSendSmsMessage>(this->getContext(serial), rcsm);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilSendSmsResult_t> sendSmsResult = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
            }
            this->sendResponseForSendCdmaSms(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendCdmaSms(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> acknowledgeLastIncomingCdmaSms(int32_t serial, const CdmaSmsAck& smsAck) {
    QCRIL_LOG_DEBUG("acknowledgeLastIncomingCdmaSms: serial=%d", serial);
    auto msg = std::make_shared<RilRequestAckCdmaSmsMessage>(
        this->getContext(serial), smsAck.errorClass == V1_0::CdmaSmsErrorClass::NO_ERROR,
        smsAck.smsCauseCode);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForAcknowledgeLastIncomingCdmaSms(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForAcknowledgeLastIncomingCdmaSms(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> getGsmBroadcastConfig(int32_t serial) {
    QCRIL_LOG_DEBUG("getGsmBroadcastConfig: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetGsmBroadcastConfigMessage>(this->getContext(serial));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilGetGsmBroadcastConfigResult_t> broadcastConfigResp{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              broadcastConfigResp =
                  std::static_pointer_cast<RilGetGsmBroadcastConfigResult_t>(resp->data);
            }
            this->sendResponseForGetGsmBroadcastConfig(serial, errorCode, broadcastConfigResp);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetGsmBroadcastConfig(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setGsmBroadcastConfig(int32_t serial,
                                     const hidl_vec<GsmBroadcastSmsConfigInfo>& configInfo) {
    QCRIL_LOG_DEBUG("setGsmBroadcastConfig: serial=%d", serial);
    int num = configInfo.size();
    std::vector<RIL_GSM_BroadcastSmsConfigInfo> configVec(num);

    for (int i = 0; i < num; i++) {
      configVec[i].fromServiceId = configInfo[i].fromServiceId;
      configVec[i].toServiceId = configInfo[i].toServiceId;
      configVec[i].fromCodeScheme = configInfo[i].fromCodeScheme;
      configVec[i].toCodeScheme = configInfo[i].toCodeScheme;
      configVec[i].selected = (configInfo[i].selected ? 1 : 0);
    }

    auto msg = std::make_shared<RilRequestGsmSetBroadcastSmsConfigMessage>(this->getContext(serial),
                                                                           std::move(configVec));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetGsmBroadcastConfig(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetGsmBroadcastConfig(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  Return<void> setGsmBroadcastActivation(int32_t serial, bool activate) {
    QCRIL_LOG_DEBUG("setGsmBroadcastActivation: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGsmSmsBroadcastActivateMessage>(this->getContext(serial),
                                                                          activate);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetGsmBroadcastActivation(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetGsmBroadcastActivation(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getCdmaBroadcastConfig(int32_t serial) {
    QCRIL_LOG_DEBUG("getCdmaBroadcastConfig: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetCdmaBroadcastConfigMessage>(this->getContext(serial));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilGetCdmaBroadcastConfigResult_t> broadcastConfigResp{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              broadcastConfigResp =
                  std::static_pointer_cast<RilGetCdmaBroadcastConfigResult_t>(resp->data);
            }
            this->sendResponseForGetCdmaBroadcastConfig(serial, errorCode, broadcastConfigResp);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetCdmaBroadcastConfig(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setCdmaBroadcastConfig(int32_t serial,
                                      const hidl_vec<CdmaBroadcastSmsConfigInfo>& configInfo) {
    QCRIL_LOG_DEBUG("setCdmaBroadcastConfig: serial=%d", serial);
    int num = configInfo.size();
    std::vector<RIL_CDMA_BroadcastSmsConfigInfo> configVec(num);

    for (int i = 0; i < num; i++) {
      configVec[i].service_category = configInfo[i].serviceCategory;
      configVec[i].language = configInfo[i].language;
      configVec[i].selected = (configInfo[i].selected ? 1 : 0);
    }

    auto msg = std::make_shared<RilRequestCdmaSetBroadcastSmsConfigMessage>(
        this->getContext(serial), std::move(configVec));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetCdmaBroadcastConfig(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetCdmaBroadcastConfig(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> setCdmaBroadcastActivation(int32_t serial, bool activate) {
    QCRIL_LOG_DEBUG("setCdmaBroadcastActivation: serial=%d", serial);
    auto msg = std::make_shared<RilRequestCdmaSmsBroadcastActivateMessage>(this->getContext(serial),
                                                                           activate);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetCdmaBroadcastActivation(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetCdmaBroadcastActivation(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getCDMASubscription(int32_t serial) {
    QCRIL_LOG_DEBUG("getCDMASubscription: serial=%d", serial);
    auto msg = std::make_shared<RilRequestCdmaSubscriptionMessage>(this->getContext(serial));
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
            this->sendResponseForGetCDMASubscription(serial, errorCode, rilCdmaSubResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetCDMASubscription(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> writeSmsToRuim(int32_t serial, const CdmaSmsWriteArgs& cdmaSms) {
    QCRIL_LOG_DEBUG("writeSmsToRuim: serial=%d", serial);
    RIL_CDMA_SMS_Message rcsm = {};
    utils::constructCdmaSms(rcsm, cdmaSms.message);

    auto msg = std::make_shared<RilRequestCdmaWriteSmsToRuimMessage>(
        this->getContext(serial), rcsm, static_cast<int>(cdmaSms.status));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              writeSmsResponse = std::static_pointer_cast<RilWriteSmsToSimResult_t>(resp->data);
            }
            this->sendResponseForWriteSmsToRuim(serial, errorCode, writeSmsResponse);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForWriteSmsToRuim(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> deleteSmsOnRuim(int32_t serial, int32_t index) {
    QCRIL_LOG_DEBUG("deleteSmsOnRuim: serial=%d", serial);
    auto msg =
        std::make_shared<RilRequestCdmaDeleteSmsOnRuimMessage>(this->getContext(serial), index);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForDeleteSmsOnRuim(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForDeleteSmsOnRuim(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getDeviceIdentity(int32_t serial) {
    QCRIL_LOG_DEBUG("getDeviceIdentity: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetDeviceIdentityMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilDeviceIdentityResult_t> result{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              result =
                  std::static_pointer_cast<qcril::interfaces::RilDeviceIdentityResult_t>(resp->data);
            }
            this->sendResponseForGetDeviceIdentity(serial, errorCode, result);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetDeviceIdentity(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> exitEmergencyCallbackMode(int32_t serial) {
    QCRIL_LOG_DEBUG("exitEmergencyCallbackMode: serial=%d", serial);
    auto msg = std::make_shared<RilRequestExitEmergencyCallbackMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForExitEmergencyCallbackMode(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForExitEmergencyCallbackMode(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getSmscAddress(int32_t serial) {
    QCRIL_LOG_DEBUG("getSmscAddress: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetSmscAddressMessage>(this->getContext(serial));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilGetSmscAddrResult_t> smscAddrResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              smscAddrResult = std::static_pointer_cast<RilGetSmscAddrResult_t>(resp->data);
            }
            this->sendResponseForGetSmscAddress(serial, errorCode, smscAddrResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetSmscAddress(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setSmscAddress(int32_t serial, const ::android::hardware::hidl_string& smsc) {
    QCRIL_LOG_DEBUG("setSmscAddress: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetSmscAddressMessage>(this->getContext(serial), smsc);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetSmscAddress(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetSmscAddress(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> reportSmsMemoryStatus(int32_t serial, bool available) {
    QCRIL_LOG_DEBUG("reportSmsMemoryStatus: serial=%d", serial);
    auto msg = std::make_shared<RilRequestReportSmsMemoryStatusMessage>(this->getContext(serial),
                                                                        available);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForReportSmsMemoryStatus(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForReportSmsMemoryStatus(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> reportStkServiceIsRunning(int32_t serial) {
    QCRIL_LOG_DEBUG("reportStkServiceIsRunning: serial=%d", serial);
    auto msg = std::make_shared<GstkReportSTKIsRunningMsg>();
    if (msg) {
      GenericCallback<RIL_GSTK_Errno> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_GSTK_Errno> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
              errorCode = static_cast<V1_0::RadioError>(*responseDataPtr);
            }
            this->sendResponseForReportStkServiceIsRunning(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForReportStkServiceIsRunning(serial, V1_0::RadioError::NO_MEMORY);
    }
    return Void();
  }

  Return<void> getCdmaSubscriptionSource(int32_t serial) {
    QCRIL_LOG_DEBUG("getCdmaSubscriptionSource: serial=%d", serial);
    bool sendFailure = false;
    do {
      auto msg =
          std::make_shared<RilRequestGetCdmaSubscriptionSourceMessage>(this->getContext(serial));
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
            this->sendResponseForGetCdmaSubscriptionSource(serial, errorCode, srcResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForGetCdmaSubscriptionSource(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> requestIsimAuthentication(int32_t serial,
                                         const ::android::hardware::hidl_string& challenge) {
    QCRIL_LOG_DEBUG("requestIsimAuthentication: serial=%d", serial);
    auto msg = std::make_shared<UimISIMAuthenticationRequestMsg>(challenge);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_IO_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForRequestIsimAuthentication(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForRequestIsimAuthentication(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                                const ::android::hardware::hidl_string& ackPdu) {
    QCRIL_LOG_DEBUG("acknowledgeIncomingGsmSmsWithPdu: serial=%d", serial);
    (void)success;
    (void)ackPdu;
    this->sendResponseForAcknowledgeIncomingGsmSmsWithPdu(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> sendEnvelopeWithStatus(int32_t serial,
                                      const ::android::hardware::hidl_string& contents) {
    QCRIL_LOG_DEBUG("sendEnvelopeWithStatus: serial=%d", serial);
    auto msg = std::make_shared<GstkSendEnvelopeRequestMsg>(serial, contents);
    if (msg) {
      GenericCallback<RIL_GSTK_EnvelopeResponse> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_GSTK_EnvelopeResponse> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForSendEnvelopeWithStatus(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendEnvelopeWithStatus(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getVoiceRadioTechnology(int32_t serial) {
    QCRIL_LOG_DEBUG("getVoiceRadioTechnology: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetVoiceRadioTechMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetVoiceTechResult_t> ratResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              ratResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetVoiceTechResult_t>(resp->data);
            }
            this->sendResponseForGetVoiceRadioTechnology(serial, errorCode, ratResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetVoiceRadioTechnology(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> getCellInfoList(int32_t serial) {
    QCRIL_LOG_DEBUG("getCellInfoList: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetCellInfoListMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              cellInfoListResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetCellInfoListResult_t>(
                      resp->data);
            }
            this->sendResponseForGetCellInfoList(serial, errorCode, cellInfoListResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetCellInfoList(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setCellInfoListRate(int32_t serial, int32_t rate) {
    QCRIL_LOG_DEBUG("setCellInfoListRate: serial=%d, rate = %d", serial, rate);
    bool sendFailure = false;
    do {
      auto msg =
          std::make_shared<RilRequestSetUnsolCellInfoListRateMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        sendFailure = true;
        break;
      }
      msg->setRate(rate);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetCellInfoListRate(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForSetCellInfoListRate(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> setInitialAttachApn(int32_t serial, const DataProfileInfo& dataProfileInfo,
                                   bool modemCognitive, bool isRoaming) {
    QCRIL_LOG_DEBUG("setInitialAttachApn V1_0: serial %d", serial);
    using namespace utils;
    rildata::DataProfileInfo_t profile = convertHidlDataProfileInfoToRil_1_0(dataProfileInfo);
    profile.persistent = modemCognitive;
    auto msg = std::make_shared<rildata::SetInitialAttachApnRequestMessage>(serial, profile, nullptr);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
                RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
                if (solicitedMsg && responseDataPtr) {
                    Log::getInstance().d(
                        "[setInitialAttachApn_1_4 cb invoked: [msg = " +
                        solicitedMsg->dump() + "] executed. client data = " +
                        "set-ril-service-cb-token status = " + std::to_string((int)status));
                    if(status == Message::Callback::Status::SUCCESS) {
                        RadioError e = convertMsgToRadioError(status, *responseDataPtr);
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                    }
                    else {
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::GENERIC_FAILURE};
                    }
                }
                this->setInitialAttachApnResponse(responseInfo);
            }));
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  Return<void> getImsRegistrationState(int32_t serial) {
    QCRIL_LOG_DEBUG("getImsRegistrationState: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetImsRegistrationMessage>(this->getContext(serial));
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                          Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilGetImsRegistrationResult_t> regStateResp{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              regStateResp = std::static_pointer_cast<RilGetImsRegistrationResult_t>(resp->data);
            }
            this->sendResponseForGetImsRegistrationState(serial, errorCode, regStateResp);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetImsRegistrationState(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> sendImsSms(int32_t serial, const ImsSmsMessage& message) {
    QCRIL_LOG_DEBUG("sendImsSms: serial=%d", serial);
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      RIL_RadioTechnologyFamily tech = utils::convertHidlRadioTechToRil(message.tech);
      auto msg = std::make_shared<RilRequestImsSendSmsMessage>(
          this->getContext(serial), message.messageRef, tech, message.retry);
      if (msg == nullptr) {
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      if (tech == RADIO_TECH_3GPP) {
        std::string payload;
        if (message.gsmMessage.size() != 1) {
          errResp = RIL_E_INVALID_ARGUMENTS;
          break;
        }
        std::string gsmPdu;
        std::string gsmSmsc;
        QCRIL_LOG_INFO("sendImsSms: pdu size = %d", message.gsmMessage[0].pdu.size());

        if (message.gsmMessage[0].pdu.size() > 0) {
          gsmPdu = message.gsmMessage[0].pdu.c_str();
        }

        if (message.gsmMessage[0].smscPdu.size() > 0) {
          gsmSmsc = message.gsmMessage[0].smscPdu.c_str();
        }
        msg->setGsmPayload(gsmSmsc, gsmPdu);
      } else {
        // set cmda payload
        if (message.cdmaMessage.size() != 1) {
          errResp = RIL_E_INVALID_ARGUMENTS;
          break;
        }

        RIL_CDMA_SMS_Message rcsm = {};
        utils::constructCdmaSms(rcsm, message.cdmaMessage[0]);
        msg->setCdmaPayload(rcsm);
      }

      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilSendSmsResult_t> sendSmsResult = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
            }
            this->sendResponseForSendImsSms(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } while (false);

    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForSendImsSms(serial, errResp, nullptr);
    }
    return Void();
  }

  Return<void> iccTransmitApduBasicChannel(int32_t serial, const SimApdu& message) {
    QCRIL_LOG_DEBUG("iccTransmitApduBasicChannel: serial=%d", serial);

    RIL_UIM_SIM_APDU data = {};
    data.sessionid = message.sessionId;
    data.cla = message.cla;
    data.instruction = message.instruction;
    data.p1 = message.p1;
    data.p2 = message.p2;
    data.p3 = message.p3;
    data.data = message.data;

    auto msg = std::make_shared<UimTransmitAPDURequestMsg>(this->getInstanceId(), true, true, data);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_IO_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForIccTransmitApduBasicChannel(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForIccTransmitApduBasicChannel(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> iccOpenLogicalChannel(int32_t serial, const ::android::hardware::hidl_string& aid,
                                     int32_t p2) {
    QCRIL_LOG_DEBUG("iccOpenLogicalChannel: serial=%d", serial);
    auto msg = std::make_shared<UimSIMOpenChannelRequestMsg>(aid, p2);
    if (msg) {
      GenericCallback<RIL_UIM_OpenChannelResponse> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_OpenChannelResponse> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForIccOpenLogicalChannel(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForIccOpenLogicalChannel(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> iccCloseLogicalChannel(int32_t serial, int32_t channelId) {
    QCRIL_LOG_DEBUG("iccCloseLogicalChannel: serial=%d", serial);
    auto msg = std::make_shared<UimSIMCloseChannelRequestMsg>(channelId);
    if (msg) {
      GenericCallback<RIL_UIM_Errno> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
              errorCode = static_cast<V1_0::RadioError>(*responseDataPtr);
            }
            this->sendResponseForIccCloseLogicalChannel(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForIccCloseLogicalChannel(serial, V1_0::RadioError::NO_MEMORY);
    }
    return Void();
  }

  Return<void> iccTransmitApduLogicalChannel(int32_t serial, const SimApdu& message) {
    QCRIL_LOG_DEBUG("iccTransmitApduLogicalChannel: serial=%d", serial);

    RIL_UIM_SIM_APDU data = {};
    data.sessionid = message.sessionId;
    data.cla = message.cla;
    data.instruction = message.instruction;
    data.p1 = message.p1;
    data.p2 = message.p2;
    data.p3 = message.p3;
    data.data = message.data;

    auto msg = std::make_shared<UimTransmitAPDURequestMsg>(this->getInstanceId(), false, true, data);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_IO_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForIccTransmitApduLogicalChannel(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForIccTransmitApduLogicalChannel(serial, V1_0::RadioError::NO_MEMORY,
                                                         nullptr);
    }
    return Void();
  }

  Return<void> nvReadItem(int32_t serial, NvItem itemId) {
    QCRIL_LOG_DEBUG("nvReadItem: serial=%d", serial);
    this->sendResponseForNvReadItem(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> nvWriteItem(int32_t serial, const NvWriteItem& item) {
    QCRIL_LOG_DEBUG("nvWriteItem: serial=%d", serial);
    this->sendResponseForNvWriteItem(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> nvWriteCdmaPrl(int32_t serial, const ::android::hardware::hidl_vec<uint8_t>& prl) {
    QCRIL_LOG_DEBUG("nvWriteCdmaPrl: serial=%d", serial);
    this->sendResponseForNvWriteCdmaPrl(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> nvResetConfig(int32_t serial, ResetNvType resetType) {
    QCRIL_LOG_DEBUG("nvResetConfig: serial=%d", serial);
    this->sendResponseForNvResetConfig(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> setUiccSubscription(int32_t serial, const SelectUiccSub& uiccSub) {
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
    return Void();
  }

  Return<void> setDataAllowed(int32_t serial, bool allow) {
    QCRIL_LOG_DEBUG("setDataAllowed: serial=%d", serial);
    auto msg = std::make_shared<RilRequestAllowDataMessage>(this->getContext(serial), allow);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetDataAllowed(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetDataAllowed(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getHardwareConfig(int32_t serial) {
    QCRIL_LOG_DEBUG("getHardwareConfig: serial=%d", serial);
    this->sendResponseForGetHardwareConfig(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> requestIccSimAuthentication(int32_t serial, int32_t authContext,
                                           const ::android::hardware::hidl_string& authData,
                                           const ::android::hardware::hidl_string& aid) {
    QCRIL_LOG_DEBUG("requestIccSimAuthentication: serial=%d", serial);
    auto msg = std::make_shared<UimSIMAuthenticationRequestMsg>(authContext, authData, aid);
    if (msg) {
      GenericCallback<RIL_UIM_SIM_IO_Response> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_SIM_IO_Response> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS) {
              errorCode = V1_0::RadioError::NONE;
            }
            this->sendResponseForRequestIccSimAuthentication(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForRequestIccSimAuthentication(serial, V1_0::RadioError::NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setDataProfile(int32_t serial,
                              const ::android::hardware::hidl_vec<DataProfileInfo>& profiles,
                              bool isRoaming) {
    QCRIL_LOG_DEBUG("setDataProfile: serial %d", serial);
    using namespace rildata;
    using namespace utils;
    vector<rildata::DataProfileInfo_t> p;
    rildata::DataProfileInfo_t t;
    for (size_t i = 0; i < profiles.size(); i++)
    {
        memset(&t, 0, sizeof(t));
        t = convertHidlDataProfileInfoToRil_1_0(profiles[i]);
        t.persistent = true;
        p.push_back(t);
    }

    auto msg = std::make_shared<rildata::SetDataProfileRequestMessage>(serial,p);
    if (msg)
    {
        GenericCallback<RIL_Errno> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
                RadioResponseInfo responseInfo{.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
                if (solicitedMsg && responseDataPtr)
                {
                    Log::getInstance().d(
                        "[SetDataProfile cb invoked: [msg = " +
                        solicitedMsg->dump() + "] executed. client data = " +
                        "set-ril-service-cb-token status = " + std::to_string((int)status));
                    if (status == Message::Callback::Status::SUCCESS)
                    {
                        RadioError e = (RadioError)(*responseDataPtr);
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                    }
                    else
                    {
                        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::GENERIC_FAILURE};
                    }
                }
                this->setDataProfileResponse(responseInfo);
            }));
        msg->setCallback(&cb);
        msg->dispatch();
    }
    else
    {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        this->setDataProfileResponse(rsp);
    }
    return Void();
  }

  Return<void> requestShutdown(int32_t serial) {
    QCRIL_LOG_DEBUG("requestShutdown: serial=%d", serial);
    auto msg = std::make_shared<RilRequestShutDownMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForRequestShutdown(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForRequestShutdown(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getRadioCapability(int32_t serial) {
    QCRIL_LOG_DEBUG("getRadioCapability: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetRadioCapMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilCapResult =
                  std::static_pointer_cast<qcril::interfaces::RilRadioCapResult_t>(resp->data);
            }
            this->sendResponseForGetRadioCapability(serial, errorCode, rilCapResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetRadioCapability(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setRadioCapability(int32_t serial, const RadioCapability& rc) {
    QCRIL_LOG_DEBUG("setRadioCapability: serial=%d", serial);
    // convert HIDL structure to internel data structure
    RIL_RadioCapability rilRc{};

    // TODO : set rilRc.version using HIDL version ?
    rilRc.session = rc.session;
    rilRc.phase = static_cast<int>(rc.phase);
    rilRc.rat = static_cast<int>(rc.raf);
    rilRc.status = static_cast<int>(rc.status);
    strlcpy(rilRc.logicalModemUuid, rc.logicalModemUuid.c_str(), sizeof(rilRc.logicalModemUuid));

    // Create message and callback
    auto msg = std::make_shared<RilRequestSetRadioCapMessage>(this->getContext(serial), rilRc);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilRadioCapResult_t> rilCapResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilCapResult =
                  std::static_pointer_cast<qcril::interfaces::RilRadioCapResult_t>(resp->data);
            }
            this->sendResponseForSetRadioCapability(serial, errorCode, rilCapResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetRadioCapability(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> startLceService(int32_t serial, int32_t reportInterval, bool pullMode) {
    QCRIL_LOG_DEBUG("startLceService: serial %d", serial);
    using namespace rildata;
    auto msg =
        std::make_shared<StartLCERequestMessage>(
            serial,
            reportInterval,
            (int32_t)pullMode,
            nullptr);
    if (msg) {
        GenericCallback<RIL_LceStatusInfo> cb([this, serial](std::shared_ptr<Message>,
                                                             Message::Callback::Status status,
                                                             std::shared_ptr<RIL_LceStatusInfo> rilLceStatusInfo) -> void {
            RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
            LceStatusInfo result = {};
            if ( rilLceStatusInfo != nullptr ) {
                RadioError errorCode = RadioError::NONE;
                if (status == Message::Callback::Status::FAILURE)
                {
                    errorCode = RadioError::GENERIC_FAILURE;
                }
                result.lceStatus = (LceStatus)(rilLceStatusInfo->lce_status);
                result.actualIntervalMs = (uint8_t)(rilLceStatusInfo->actual_interval_ms);
                responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = errorCode};
            }
            this->sendStartLceResponse(responseInfo, result);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        LceStatusInfo result = {};
        this->sendStartLceResponse(rsp, result);
    }
    return Void();
  }

  Return<void> stopLceService(int32_t serial) {
    QCRIL_LOG_DEBUG("stopLceService: serial %d", serial);
    using namespace rildata;
    auto msg = std::make_shared<StopLCERequestMessage>(
            serial,
            nullptr);
    if (msg) {
        GenericCallback<RIL_LceStatusInfo> cb([this, serial](std::shared_ptr<Message>,
                                                             Message::Callback::Status status,
                                                             std::shared_ptr<RIL_LceStatusInfo> rilLceStatusInfo) -> void {
            RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
            LceStatusInfo result = {};
            if ( rilLceStatusInfo != nullptr ) {
                RadioError errorCode = RadioError::NONE;
                if (status == Message::Callback::Status::FAILURE)
                {
                    errorCode = RadioError::GENERIC_FAILURE;
                }

                result.lceStatus = (LceStatus)(rilLceStatusInfo->lce_status);
                result.actualIntervalMs = (uint8_t)(rilLceStatusInfo->actual_interval_ms);
                responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = errorCode};
            }
            this->sendStopLceResponse(responseInfo, result);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        LceStatusInfo result = {};
        this->sendStopLceResponse(rsp, result);
    }
    return Void();
  }

  Return<void> pullLceData(int32_t serial) {
    QCRIL_LOG_DEBUG("pullLceData: serial %d", serial);
    using namespace rildata;
    auto msg =
        std::make_shared<PullLCEDataRequestMessage>(
            serial,
            nullptr);
    if (msg)
    {
        GenericCallback<RIL_LceDataInfo> cb([this, serial](std::shared_ptr<Message>,
                                                     Message::Callback::Status status,
                                                     std::shared_ptr<RIL_LceDataInfo> rilLceDataInfo) -> void {
            RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
            LceDataInfo result = {};
            if ( rilLceDataInfo )
            {
                RadioError errorCode = RadioError::NONE;
                if (status == Message::Callback::Status::FAILURE)
                {
                    errorCode = RadioError::GENERIC_FAILURE;
                }
                result.lastHopCapacityKbps = rilLceDataInfo->last_hop_capacity_kbps;
                result.confidenceLevel = rilLceDataInfo->confidence_level;
                result.lceSuspended = rilLceDataInfo->lce_suspended;
                responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = errorCode};
            }
            this->sendPullLceResponse(responseInfo, result);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    else
    {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        LceDataInfo result = {};
        this->sendPullLceResponse(rsp, result);
    }
    return Void();
  }

  Return<void> getModemActivityInfo(int32_t serial) {
    QCRIL_LOG_DEBUG("getModemActivityInfo: serial=%d", serial);

    auto msg = std::make_shared<RilRequestGetModemActivityMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetModemActivityResult_t> result{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              result = std::static_pointer_cast<qcril::interfaces::RilGetModemActivityResult_t>(
                  resp->data);
            }
            this->sendResponseForGetModemActivityInfo(serial, errorCode, result);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetModemActivityInfo(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setAllowedCarriers(int32_t serial, bool allAllowed,
                                  const CarrierRestrictions& carriers) {
    QCRIL_LOG_DEBUG("setAllowedCarriers: serial=%d", serial);
    this->sendResponseForSetAllowedCarriers(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> getAllowedCarriers(int32_t serial) {
    QCRIL_LOG_DEBUG("getAllowedCarriers: serial=%d", serial);
    this->sendResponseForGetAllowedCarriers(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  Return<void> sendDeviceState(int32_t serial, DeviceStateType deviceStateType, bool state) {
    QCRIL_LOG_DEBUG("sendDeviceState: serial=%d", serial);

    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(RIL_E_SUCCESS) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("sendDeviceStateResponse: serial=%d, error=%d", serial, RIL_E_SUCCESS);
      auto ret = respCb->sendDeviceStateResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
    return Void();
  }

  Return<void> setIndicationFilter(int32_t serial, int32_t indicationFilter) {
    QCRIL_LOG_DEBUG("setIndicationFilter: serial=%d, indicationFilter=%d", serial, indicationFilter);
    RIL_Errno errResp = RIL_E_SUCCESS;
    do {
      if (s_vendorFunctions->version < 15) {
        errResp = RIL_E_REQUEST_NOT_SUPPORTED;
        break;
      }

      auto msg = std::make_shared<RilRequestSetUnsolRespFilterMessage>(this->getContext(serial),
                                                                       indicationFilter);
      if (msg == nullptr) {
        errResp = RIL_E_NO_MEMORY;
        break;
      }
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetIndicationFilter(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (errResp != RIL_E_SUCCESS) {
      this->sendResponseForSetIndicationFilter(serial, errResp);
    }
    return Void();
  }

  Return<void> setSimCardPower(int32_t serial, bool powerUp) {
    QCRIL_LOG_DEBUG("setSimCardPower: serial=%d", serial);

    RIL_UIM_CardPowerState state = RIL_UIM_CARD_POWER_DOWN;

    if (powerUp) {
      state = RIL_UIM_CARD_POWER_UP;
    }

    auto msg = std::make_shared<UimCardPowerReqMsg>(this->getInstanceId(), state);
    if (msg) {
      GenericCallback<RIL_UIM_Errno> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
              errorCode = static_cast<V1_0::RadioError>(*responseDataPtr);
            }
            this->sendResponseForSetSimCardPower(serial, errorCode);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetSimCardPower(serial, V1_0::RadioError::NO_MEMORY);
    }
    return Void();
  }

  Return<void> responseAcknowledgement() {
    QCRIL_LOG_DEBUG("responseAcknowledgement");
    android::releaseWakeLock();
    return Void();
  }

  int sendAcknowledgeRequest(std::shared_ptr<RilAcknowledgeRequestMessage> msg) override {
    if (msg) {
      auto ctx = msg->getContext();
      auto respCb = this->getResponseCallback();
      if (respCb && ctx) {
        QCRIL_LOG_DEBUG("acknowledgeRequest: serial=%d", ctx->serial);
        auto ret = respCb->acknowledgeRequest(ctx->serial);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
        }
      }
    }
    return 0;
  }

  int sendSetupDataCallResponse(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg) {
    QCRIL_LOG_DEBUG("Sending SetupDataCallRadioResponseIndMessage V1_0 %s",msg->dump().c_str());
    auto respCb = this->getResponseCallback();
    if (msg && respCb) {
      auto rsp = msg->getResponse();
      auto status = msg->getStatus();
      auto serial = msg->getSerial();
      RadioResponseInfo responseInfo{.serial = serial, .error = RadioError::NO_MEMORY};
      V1_0::SetupDataCallResult dcResult = {};
      dcResult.status = V1_0::DataCallFailCause::ERROR_UNSPECIFIED;
      dcResult.suggestedRetryTime = -1;
      RadioError e = RadioError::NONE;
      if (status == Message::Callback::Status::SUCCESS)
      {
        QCRIL_LOG_DEBUG("setup data call cb invoked status %d respErr %d", status, rsp.respErr);
        QCRIL_LOG_DEBUG("cause = %d", rsp.call.cause);
        QCRIL_LOG_DEBUG("suggestedRetryTime = %d", rsp.call.suggestedRetryTime);
        QCRIL_LOG_DEBUG("cid = %d", rsp.call.cid);
        QCRIL_LOG_DEBUG("active = %d", rsp.call.active);
        QCRIL_LOG_DEBUG("type = %s", rsp.call.type.c_str());
        QCRIL_LOG_DEBUG("ifname = %s", rsp.call.ifname.c_str());
        QCRIL_LOG_DEBUG("addresses = %s", rsp.call.addresses.c_str());
        QCRIL_LOG_DEBUG("dnses = %s", rsp.call.dnses.c_str());
        QCRIL_LOG_DEBUG("gateways = %s", rsp.call.gateways.c_str());
        QCRIL_LOG_DEBUG("pcscf = %s", rsp.call.pcscf.c_str());
        QCRIL_LOG_DEBUG("mtu = %d", rsp.call.mtu);
        utils::convertRilDataCallToHal(rsp.call, dcResult);
      }
      else
      {
        switch (rsp.respErr)
        {
        case rildata::ResponseError_t::NOT_SUPPORTED:
            e = RadioError::REQUEST_NOT_SUPPORTED;
            break;
        case rildata::ResponseError_t::INVALID_ARGUMENT:
            e = RadioError::INVALID_ARGUMENTS;
            break;
        default:
            e = RadioError::GENERIC_FAILURE;
            break;
        }
      }
      responseInfo = {.serial = serial, .error = e};
      auto ret = respCb->setupDataCallResponse(responseInfo, dcResult);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
    else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
        return 1;
    }
    return 0;
  }

  int sendDeactivateDataCallResponse(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg) {
    QCRIL_LOG_DEBUG("Sending DeactivateDataCallRadioResponseIndMessage V1_0 %s",msg->dump().c_str());
    auto respCb = this->getResponseCallback();
    if (msg && respCb) {
      auto rsp = msg->getResponse();
      auto status = msg->getStatus();
      auto serial = msg->getSerial();
      RadioResponseInfo responseInfo{.serial = serial, .error = RadioError::NO_MEMORY};
      RadioError e = RadioError::NONE;
      if ((status != Message::Callback::Status::SUCCESS) ||
            (rsp != rildata::ResponseError_t::NO_ERROR))
      {
          switch (rsp)
          {
          case rildata::ResponseError_t::NOT_SUPPORTED:
              e = RadioError::REQUEST_NOT_SUPPORTED;
              break;
          case rildata::ResponseError_t::INVALID_ARGUMENT:
              e = RadioError::INVALID_ARGUMENTS;
              break;
          case rildata::ResponseError_t::CALL_NOT_AVAILABLE:
              e = RadioError::INVALID_CALL_ID;
              break;
          default:
              e = RadioError::GENERIC_FAILURE;
              break;
          }
      }
      responseInfo = {.serial = serial, .error = e};
      QCRIL_LOG_DEBUG("deactivate data call cb invoked status %d respErr %d", status, rsp);
      auto ret = respCb->deactivateDataCallResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
    else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
        return 1;
    }
    return 0;
  }

  // Indications

  int sendRilPCODataInd(std::shared_ptr<rildata::RilPCODataMessage> msg) {
    QCRIL_LOG_DEBUG("Sending RilPCODataChangeInd V1_0 %s",msg->dump().c_str());
    auto indCb = this->getIndicationCallback();
    if (msg == nullptr) {
        QCRIL_LOG_DEBUG("msg is nullptr");
        return 1;
    }
    if(indCb) {
      QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
      rildata::PcoDataInfo_t pcoData = msg->getPcoDataInfo();
      ::android::hardware::radio::V1_0::PcoDataInfo pco = {};
      QCRIL_LOG_DEBUG("pcoData.cid %d", pcoData.cid);
      QCRIL_LOG_DEBUG("pcoData.bearerProto %s", pcoData.bearerProto.c_str());
      QCRIL_LOG_DEBUG("pcoData.pcoId %d", pcoData.pcoId);
      QCRIL_LOG_DEBUG("pcoData.contents size %d", pcoData.contents.size());
      for (const auto &content : pcoData.contents) {
          QCRIL_LOG_DEBUG("pcoData.content %d", content);
      }
      pco.cid = pcoData.cid;
      pco.bearerProto = pcoData.bearerProto;
      pco.pcoId = pcoData.pcoId;
      pco.contents = pcoData.contents;
      auto ret = indCb->pcoData(RadioIndicationType::UNSOLICITED, pco);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        return 1;
      }
    }
    else {
        QCRIL_LOG_ERROR("Indication Callback is Nullptr");
        return 1;
    }
    return 0;
  }

  int sendRadioDataCallListChangeInd(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg) {
    QCRIL_LOG_DEBUG("Sending RadioDataCallListChangeInd V1_0 %s",msg->dump().c_str());
    auto indCb = this->getIndicationCallback();
    if (msg == nullptr) {
        QCRIL_LOG_DEBUG("msg is nullptr");
        return 1;
    }
    if(indCb) {
      QCRIL_LOG_DEBUG("Handling handleRadioDataCallListChangeIndMessage %s", msg->dump().c_str());
      std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
      ::android::hardware::hidl_vec<V1_0::SetupDataCallResult> dcResultList;
      QCRIL_LOG_DEBUG("dcList %d",dcList.size());
      dcResultList.resize(dcList.size());
      int i=0;
      for (rildata::DataCallResult_t entry : dcList)
      {
        utils::convertRilDataCallToHal(entry, dcResultList[i]);
        i++;
      }
      auto ret = indCb->dataCallListChanged(RadioIndicationType::UNSOLICITED, dcResultList);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        return 1;
      }
    }
    else {
        QCRIL_LOG_ERROR("Indication Callback is Nullptr");
        return 1;
    }
    return 0;
  }

  /**
   * Notifies rilConnected indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::rilConnected
   */
  int sendRilConnected(std::shared_ptr<RilUnsolRilConnectedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: rilConnected");
      Return<void> ret = ind->rilConnected(RadioIndicationType::UNSOLICITED);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies newSms indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newSms
   */
  int sendNewSms(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      hidl_vec<uint8_t> pdu;
      pdu.setToExternal(msg->getPayload().data(), msg->getPayload().size());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: newSms");
      Return<void> ret = ind->newSms(RadioIndicationType::UNSOLICITED_ACK_EXP, pdu);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }

    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaNewSms indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaNewSms
   */
  int sendNewCdmaSms(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      CdmaSmsMessage smsMsg{};
      utils::makeCdmaSmsMessage(smsMsg, msg);
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaNewSms");
      Return<void> ret = ind->cdmaNewSms(RadioIndicationType::UNSOLICITED_ACK_EXP, smsMsg);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }

    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies newSmsOnSim indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newSmsOnSim
   */
  int sendNewSmsOnSim(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: newSmsOnSim");
      Return<void> ret = ind->newSmsOnSim(RadioIndicationType::UNSOLICITED_ACK_EXP, msg->getRecordNumber());
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies newSmsStatusReport indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newSmsStatusReport
   */
  int sendNewSmsStatusReport(std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      hidl_vec<uint8_t> pdu;
      pdu.setToExternal(msg->getPayload().data(), msg->getPayload().size());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: newSmsStatusReport");
      Return<void> ret = ind->newSmsStatusReport(RadioIndicationType::UNSOLICITED_ACK_EXP, pdu);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies newBroadcastSms indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::newBroadcastSms
   */
  int sendNewBroadcastSms(std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      hidl_vec<uint8_t> pdu;
      pdu.setToExternal(msg->getPayload().data(), msg->getPayload().size());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: newBroadcastSms");
      Return<void> ret = ind->newBroadcastSms(RadioIndicationType::UNSOLICITED_ACK_EXP, pdu);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies stkCallControlAlphaNotify indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::stkCallControlAlphaNotify
   */
  int sendStkCCAlphaNotify(std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      ::android::hardware::hidl_string payload(msg->getPayload());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: stkCallControlAlphaNotify");
      Return<void> ret = ind->stkCallControlAlphaNotify(RadioIndicationType::UNSOLICITED_ACK_EXP, payload);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaRuimSmsStorageFull indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaRuimSmsStorageFull
   */
  int sendCdmaRuimSmsStorageFull(std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaRuimSmsStorageFull");
      Return<void> ret = ind->cdmaRuimSmsStorageFull(RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies simSmsStorageFull indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::simSmsStorageFull
   */
  int sendSimSmsStorageFull(std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: simSmsStorageFull");
      Return<void> ret = ind->simSmsStorageFull(RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies imsNetworkStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::imsNetworkStateChanged
   */
  int sendImsNetworkStateChanged(std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: imsNetworkStateChanged");
      Return<void> ret = ind->imsNetworkStateChanged(RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies callRing indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::callRing
   */
  int sendCallRing(std::shared_ptr<QcRilUnsolCallRingingMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      bool isGsm;
      CdmaSignalInfoRecord record = {};
      if (!msg->hasCdmaSignalInfoRecord()) {
        isGsm = true;
      } else {
        isGsm = false;
        if (msg->getCdmaSignalInfoRecord() == nullptr) {
          QCRIL_LOG_ERROR("callRing: invalid indication");
          return 1;
        }
        utils::convertRilCdmaSignalInfoRecord(record, msg->getCdmaSignalInfoRecord());
      }
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: callRing");
      Return<void> ret = ind->callRing(RadioIndicationType::UNSOLICITED_ACK_EXP, isGsm, record);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies onUssd indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::onUssd
   */
  int sendOnUssd(std::shared_ptr<QcRilUnsolOnUssdMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    // TODO: change to msg->isCsDomain() when AOSP feature is enabled.
    if (ind && msg && !msg->isUnknownDomain()) {
      if (msg->hasMode()) {
        UssdModeType modeType = utils::convertUssdModeType(msg->getMode());
        ::android::hardware::hidl_string ussdMsg = "";
        if (msg->hasMessage() && !msg->getMessage().empty()) {
          ussdMsg = msg->getMessage().c_str();
        }
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();
        QCRIL_LOG_DEBUG("UNSOL: onUssd");
        Return<void> ret = ind->onUssd(RadioIndicationType::UNSOLICITED_ACK_EXP, modeType, ussdMsg);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies suppSvcNotify indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::suppSvcNotify
   */
  int sendSuppSvcNotify(std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      SuppSvcNotification suppSvc{};
      utils::makeSuppSvcNotification(suppSvc, msg);
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: suppSvcNotify");
      Return<void> ret = ind->suppSvcNotify(RadioIndicationType::UNSOLICITED_ACK_EXP, suppSvc);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaCallWaiting indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaCallWaiting
   */
  int sendCdmaCallWaiting(std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      CdmaCallWaiting callWaitingRecord{};
      utils::makeCdmaCallWaiting(callWaitingRecord, msg);
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaCallWaiting");
      Return<void> ret = ind->cdmaCallWaiting(RadioIndicationType::UNSOLICITED_ACK_EXP, callWaitingRecord);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaOtaProvisionStatus indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaOtaProvisionStatus
   */
  int sendCdmaOtaProvisionStatus(std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      if (msg && msg->hasStatus()) {
        CdmaOtaProvisionStatus status = static_cast<CdmaOtaProvisionStatus>(msg->getStatus());
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();
        QCRIL_LOG_DEBUG("UNSOL: cdmaOtaProvisionStatus");
        Return<void> ret = ind->cdmaOtaProvisionStatus(RadioIndicationType::UNSOLICITED_ACK_EXP, status);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies indicateRingbackTone indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::indicateRingbackTone
   */
  int sendIndicateRingbackTone(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      if (msg->hasRingBackToneOperation()) {
        bool start =
            (msg->getRingBackToneOperation() == qcril::interfaces::RingBackToneOperation::START)
                ? true
                : false;
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();
        QCRIL_LOG_DEBUG("UNSOL: indicateRingbackTone");
        Return<void> ret = ind->indicateRingbackTone(RadioIndicationType::UNSOLICITED_ACK_EXP, start);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies srvccStateNotify indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::srvccStateNotify
   */
  int sendSrvccStateNotify(std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg) override {
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      if (msg->hasState()) {
        SrvccState state = static_cast<SrvccState>(msg->getState());  // TODO map this?
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();
        QCRIL_LOG_DEBUG("UNSOL: srvccStateNotify");
        Return<void> ret = ind->srvccStateNotify(RadioIndicationType::UNSOLICITED_ACK_EXP, state);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies onSupplementaryServiceIndication indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::onSupplementaryServiceIndication
   */
  int sendOnSupplementaryServiceIndication(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      StkCcUnsolSsResult ss = {};
      utils::makeStkCcUnsolSsResult(ss, msg);
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: onSupplementaryServiceIndication");
      Return<void> ret = ind->onSupplementaryServiceIndication(RadioIndicationType::UNSOLICITED_ACK_EXP, ss);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies callStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::callStateChanged
   */
  int sendCallStateChanged(std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) override {
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: callStateChanged");
      Return<void> ret = ind->callStateChanged(RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaInfoRec indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaInfoRec
   */
  int sendCdmaInfoRec(std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) override {
    QCRIL_LOG_FUNC_ENTRY();
    auto ind = this->getIndicationCallback();
    if (ind && msg) {
      CdmaInformationRecords records{};
      auto valid = utils::makeCdmaInformationRecords(records, msg);
      if (valid) {
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();
        QCRIL_LOG_DEBUG("UNSOL: cdmaInfoRec");
        Return<void> ret = ind->cdmaInfoRec(RadioIndicationType::UNSOLICITED_ACK_EXP, records);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
      }
    } else {
      QCRIL_LOG_INFO("ind null: %s, msg null: %s", ind ? "false" : "true", msg ? "false" : "true");
    }
    QCRIL_LOG_FUNC_RETURN();

    return 0;
  }

  /**
   * Notifies networkStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::networkStateChanged
   */
  int sendNetworkStateChanged(std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: networkStateChanged");
      Return<void> ret = ind->networkStateChanged(RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies nitzTimeReceived indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::nitzTimeReceived
   */
  int sendNitzTimeReceived(std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (msg) {
      if (ind) {
        auto& time = msg->getNitzTime();
        int64_t timeReceived = 0;
#ifndef QMI_RIL_UTF  // TODO: find a way to get elapsedRealtime in host compilation
        timeReceived = android::elapsedRealtime();
#endif
#if VDBG
        QCRIL_LOG_DEBUG("nitzTimeReceivedInd: nitzTime %s receivedTime %" PRId64, time.c_str(),
                        timeReceived);
#endif
        // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
        android::grabPartialWakeLock();
        QCRIL_LOG_DEBUG("UNSOL: nitzTimeReceived");
        Return<void> ret =
            ind->nitzTimeReceived(RadioIndicationType::UNSOLICITED_ACK_EXP, time, timeReceived);
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
      } else {
        QCRIL_LOG_INFO("Client is not connected, store the last reported NITZ time");
        android::storeNITZTimeData(msg->getNitzTime());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies voiceRadioTechChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::voiceRadioTechChanged
   */
  int sendVoiceRadioTechChanged(std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: voiceRadioTechChanged");
      Return<void> ret = ind->voiceRadioTechChanged(
          RadioIndicationType::UNSOLICITED_ACK_EXP, static_cast<RadioTechnology>(msg->getVoiceRat()));
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies currentSignalStrength indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   * V1_0::IRadioIndication::currentSignalStrength
   */
  int sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      SignalStrength signalStrength = {};
      utils::convertRilSignalStrengthToHal(signalStrength, msg->getSignalStrength());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: currentSignalStrength");
      Return<void> ret =
          ind->currentSignalStrength(RadioIndicationType::UNSOLICITED_ACK_EXP, signalStrength);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies simStatusChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::simStatusChanged
   */
  int sendSimStatusChanged(std::shared_ptr<UimSimStatusChangedInd> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: simStatusChanged");
      Return<void> ret = ind->simStatusChanged(RadioIndicationType::UNSOLICITED_ACK_EXP);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies simRefresh indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::simRefresh
   */
  int sendSimRefresh(std::shared_ptr<UimSimRefreshIndication> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      SimRefreshResult refreshResult = {};
      RIL_UIM_SIM_RefreshIndication simRefreshResponse = msg->get_refresh_ind();
      refreshResult.type = static_cast<V1_0::SimRefreshType>(simRefreshResponse.result);
      refreshResult.efId = simRefreshResponse.ef_id;
      refreshResult.aid = simRefreshResponse.aid;

      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: simRefresh type %d efId %d", refreshResult.type, refreshResult.efId);
      Return<void> ret = ind->simRefresh(RadioIndicationType::UNSOLICITED_ACK_EXP, refreshResult);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies STK related indications.
   *   V1_0::IRadioIndication::stkCallSetup
   *   V1_0::IRadioIndication::stkEventNotify
   *   V1_0::IRadioIndication::stkProactiveCommand
   *   V1_0::IRadioIndication::stkSessionEnd
   */
  int sendGstkIndication(std::shared_ptr<GstkUnsolIndMsg> msg) override {
    auto ind = this->getIndicationCallback();

    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      Return<void> ret = {};
      RIL_GSTK_UnsolData stk_ind = msg->get_unsol_data();

      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      switch (stk_ind.type) {
        case RIL_GSTK_UNSOL_SESSION_END:
          QCRIL_LOG_DEBUG("UNSOL: stkSessionEnd");
          ret = ind->stkSessionEnd(RadioIndicationType::UNSOLICITED_ACK_EXP);
          break;
        case RIL_GSTK_UNSOL_PROACTIVE_COMMAND:
          QCRIL_LOG_DEBUG("UNSOL: stkProactiveCommand");
          ret = ind->stkProactiveCommand(RadioIndicationType::UNSOLICITED_ACK_EXP, stk_ind.cmd);
          break;
        case RIL_GSTK_UNSOL_EVENT_NOTIFY:
          QCRIL_LOG_DEBUG("UNSOL: stkEventNotify");
          ret = ind->stkEventNotify(RadioIndicationType::UNSOLICITED_ACK_EXP, stk_ind.cmd);
          break;
        case RIL_GSTK_UNSOL_STK_CALL_SETUP:
          // For call setup send event notify followed by call setup
          QCRIL_LOG_DEBUG("UNSOL: stkEventNotify");
          ret = ind->stkEventNotify(RadioIndicationType::UNSOLICITED_ACK_EXP, stk_ind.cmd);
          QCRIL_LOG_DEBUG("UNSOL: stkCallSetup");
          ret = ind->stkCallSetup(RadioIndicationType::UNSOLICITED_ACK_EXP, stk_ind.setupCallTimeout);
          break;
        default:
          QCRIL_LOG_FUNC_RETURN();
          return 0;
      }
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies Emergency Callback Mode indications.
   *   V1_0::IRadioIndication::enterEmergencyCallbackMode
   *   V1_0::IRadioIndication::exitEmergencyCallbackMode
   */
  int sendEmergencyCallbackMode(std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      auto mode = msg->getEmergencyCallbackMode();
      Return<void> ret{};
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      if (mode == RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::ENTER) {
        QCRIL_LOG_DEBUG("UNSOL: enterEmergencyCallbackMode");
        ret = ind->enterEmergencyCallbackMode(RadioIndicationType::UNSOLICITED_ACK_EXP);
      } else {
        QCRIL_LOG_DEBUG("UNSOL: exitEmergencyCallbackMode");
        ret = ind->exitEmergencyCallbackMode(RadioIndicationType::UNSOLICITED_ACK_EXP);
      }
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies radioCapabilityIndication indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::radioCapabilityIndication
   */
  int sendRadioCapability(std::shared_ptr<RilUnsolRadioCapabilityMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      RadioCapability rc = {};
      utils::convertRilRadioCapabilityToHal(rc, msg->getRadioCapability());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: radioCapabilityIndication");
      Return<void> ret = ind->radioCapabilityIndication(RadioIndicationType::UNSOLICITED_ACK_EXP, rc);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaSubscriptionSourceChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaSubscriptionSourceChanged
   */
  int sendCdmaSubscriptionSourceChanged(
      std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaSubscriptionSourceChanged");
      Return<void> ret = ind->cdmaSubscriptionSourceChanged(
          RadioIndicationType::UNSOLICITED_ACK_EXP,
          static_cast<V1_0::CdmaSubscriptionSource>(msg->getSource()));
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cdmaPrlChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cdmaPrlChanged
   */
  int sendCdmaPrlChanged(std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      int32_t version = msg->getPrl();
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cdmaPrlChanged");
      Return<void> ret = ind->cdmaPrlChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, version);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies restrictedStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::restrictedStateChanged
   */
  int sendRestrictedStateChanged(std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      PhoneRestrictedState state = static_cast<PhoneRestrictedState>(msg->getState());
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: restrictedStateChanged");
      Return<void> ret = ind->restrictedStateChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, state);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies subscriptionStatusChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::subscriptionStatusChanged
   */
  int sendUiccSubsStatusChanged(std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      bool activate = msg->getStatus();
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: subscriptionStatusChanged");
      Return<void> ret = ind->subscriptionStatusChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, activate);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies radioStateChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::radioStateChanged
   */
  int sendRadioStateChanged(std::shared_ptr<RilUnsolRadioStateChangedMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      RadioState radioState = RadioState::UNAVAILABLE;
      if (s_vendorFunctions) {
        radioState = static_cast<RadioState>(s_vendorFunctions->onStateRequest());
      }
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: radioStateChanged: radioState %d", radioState);
      Return<void> ret = ind->radioStateChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, radioState);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies modemReset indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::modemReset
   */
  virtual int sendModemReset(std::shared_ptr<RilUnsolModemRestartMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      ::android::hardware::hidl_string reason = msg->getReason();
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: modemReset");
      Return<void> ret = ind->modemReset(RadioIndicationType::UNSOLICITED_ACK_EXP, reason);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }

  /**
   * Notifies cellInfoList indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cellInfoList
   */
  int sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg) override {
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind && msg) {
      ::android::hardware::hidl_vec<V1_0::CellInfo> records{};
      utils::convertRilCellInfoListToHal(msg->getCellInfoList(), records);
      // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
      android::grabPartialWakeLock();
      QCRIL_LOG_DEBUG("UNSOL: cellInfoList");
      Return<void> ret = ind->cellInfoList(RadioIndicationType::UNSOLICITED_ACK_EXP, records);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_0::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_0
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_0_H__
