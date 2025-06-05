/*
 * Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
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

#ifndef __RADIO_SERVICE_1_5_H__
#define __RADIO_SERVICE_1_5_H__

#include "android/hardware/radio/1.5/IRadio.h"
#include "android/hardware/radio/1.5/IRadioResponse.h"
#include "android/hardware/radio/1.5/IRadioIndication.h"
#include "android/hardware/radio/1.5/types.h"

#include "interfaces/dms/RilRequestRadioPowerMessage.h"
#include "interfaces/nas/NasSetSignalStrengthCriteria.h"
#include "interfaces/nas/RilRequestEnableUiccAppMessage.h"
#include "interfaces/nas/RilRequestGetUiccAppStatusMessage.h"
#include "interfaces/nas/RilRequestSetSysSelChannelsMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRegistrationMessage.h"
#include "interfaces/nas/RilRequestGetDataRegistrationMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h"
#include "interfaces/nas/RilRequestGetBarringInfoMessage.h"
#include "interfaces/sms/RilRequestCdmaSendSmsMessage.h"
#include "interfaces/sms/RilRequestCdmaSendSmsMessage.h"
#include "interfaces/uim/UimEnterDePersoRequestMsg.h"

#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.4/radio_service_1_4.h"
#include "hidl_impl/1.5/radio_service_utils_1_5.h"
#include "RadioServiceModule.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {

extern void grabPartialWakeLock();

namespace hardware {
namespace radio {
namespace V1_5 {
namespace implementation {

// V1_5::IRadio implementation
template <typename T>
class RadioServiceImpl : public V1_4::implementation::RadioServiceImpl<T> {
 private:
  ::android::sp<android::hardware::radio::V1_5::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_5::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_5::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_5::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_5::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
    V1_4::implementation::RadioServiceImpl<T>::clearCallbacks_nolock();
  }
  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }

  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_5::setResponseFunctions_nolock");
    mRadioResponse = V1_5::IRadioResponse::castFrom(respCb).withDefault(nullptr);
    mRadioIndication = V1_5::IRadioIndication::castFrom(indCb).withDefault(nullptr);
    V1_4::implementation::RadioServiceImpl<T>::setResponseFunctions_nolock(respCb, indCb);

    RadioServiceModule& rilModule = getRadioServiceModule();
    QCRIL_LOG_DEBUG("New client connected. Resetting the counts for LTE and nr5g");
    rilModule.resetLteCounts();
    rilModule.reset5gnrCounts();
  }

  virtual void sendResponseForGetCellInfoList(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_4::implementation::RadioServiceImpl<T>::sendResponseForGetCellInfoList(
          serial, errorCode, cellInfoListResult);
      return;
    }
    ::android::hardware::hidl_vec<V1_5::CellInfo> cellInfos{};
    if (errorCode == RIL_E_SUCCESS && cellInfoListResult) {
      utils::convertRilCellInfoList(cellInfoListResult->cellInfos, cellInfos);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getCellInfoListResponse_1_5: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCellInfoListResponse_1_5(responseInfo, cellInfos);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetSignalStrengthReportingCriteria_1_5(int32_t serial,
                                                                     RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("setSignalStrengthReportingCriteriaResponse_1_5: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->setSignalStrengthReportingCriteriaResponse_1_5(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForEnableUiccApplications(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("enableUiccApplicationsResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->enableUiccApplicationsResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForAreUiccApplicationsEnabled(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetUiccAppStatusResult_t> result) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool state = false;
    if (errorCode == RIL_E_SUCCESS && result) {
      state = result->state;
    }
    QCRIL_LOG_DEBUG("areUiccApplicationsEnabledResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->areUiccApplicationsEnabledResponse(responseInfo, state);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetSystemSelectionChannels_1_5(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("setSystemSelectionChannelsResponse_1_5: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->setSystemSelectionChannelsResponse_1_5(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForStartNetworkScan_1_5(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("startNetworkScanResponse_1_5: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->startNetworkScanResponse_1_5(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetRadioPower_1_5(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("setRadioPowerResponse_1_5: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setRadioPowerResponse_1_5(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetIndicationFilter_1_5(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("setIndicationFilterResponse_1_5: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setIndicationFilterResponse_1_5(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetBarringInfo(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilCellBarringInfo_t> rilCellBarring) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_5::CellIdentity cellIdentity{};
    hidl_vec<V1_5::BarringInfo> barringInfo{};
    if (errorCode == RIL_E_SUCCESS) {
      if (rilCellBarring) {
        utils::convertCellIdentityRilToHidl(rilCellBarring->rilCellIdentity, cellIdentity);
        utils::convertRilBarringInfoList(rilCellBarring->barring_info, barringInfo);
      } else {
        errorCode = RIL_E_NO_MEMORY;
      }
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getBarringInfoResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getBarringInfoResponse(responseInfo, cellIdentity, barringInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetVoiceRegistrationState_1_5(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_5::RegStateResult regResponse{};
    if (errorCode == RIL_E_SUCCESS) {
      if (rilRegResult) {
        utils::fillVoiceRegistrationStateResponse(regResponse, rilRegResult->respData);
      } else {
        errorCode = RIL_E_NO_MEMORY;
      }
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getVoiceRegistrationStateResponse_1_5: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getVoiceRegistrationStateResponse_1_5(responseInfo, regResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetDataRegistrationState_1_5(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_5::RegStateResult regResponse{};
    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillDataRegistrationStateResponse(regResponse, rilRegResult->respData);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getDataRegistrationStateResponse_1_5: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getDataRegistrationStateResponse_1_5(responseInfo, regResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetNetworkSelectionModeManual_1_5(int32_t serial,
                                                                RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("setNetworkSelectionModeManualResponse_1_5: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->setNetworkSelectionModeManualResponse_1_5(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void setLinkCapacityReportingCriteriaResponse(V1_0::RadioResponseInfo responseInfo) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending setLinkCapacityReportingCriteriaResponse V1_5");
      auto ret = respCb->setLinkCapacityReportingCriteriaResponse_1_5(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        return V1_4::implementation::RadioServiceImpl<T>::setLinkCapacityReportingCriteriaResponse(responseInfo);
    }
  }

  virtual void setInitialAttachApnResponse(V1_0::RadioResponseInfo responseInfo) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.5") {
        return V1_4::implementation::RadioServiceImpl<T>::setInitialAttachApnResponse(responseInfo);
    }
#endif
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending setInitialAttachApnResponse V1_5 Response");
      auto ret = respCb->setInitialAttachApnResponse_1_5(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        return V1_4::implementation::RadioServiceImpl<T>::setInitialAttachApnResponse(responseInfo);
    }
  }

  virtual void getDataCallListResponse(std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr,
                                         int serial, Message::Callback::Status status) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.5") {
        return V1_4::implementation::RadioServiceImpl<T>::getDataCallListResponse(responseDataPtr, serial, status);
    }
#endif
    QCRIL_LOG_DEBUG("GetDataCallListResponse V1_5");
    using namespace rildata;
    using namespace V1_0;
    auto respCb = this->getResponseCallback();
    if (respCb) {
        ::android::hardware::hidl_vec<V1_5::SetupDataCallResult> dcResultList;
        RadioResponseInfo responseInfo {.type = RadioResponseType::SOLICITED, .serial = serial, .error = RadioError::NO_MEMORY};
        if (responseDataPtr) {
            RadioError e = RadioError::NONE;
            if ((status == Message::Callback::Status::SUCCESS) &&
               (responseDataPtr->respErr == ResponseError_t::NO_ERROR)) {
                QCRIL_LOG_DEBUG("getDataCallList cb invoked status %d respErr %d", status, responseDataPtr->respErr);
                QCRIL_LOG_DEBUG("Call list size = %d", responseDataPtr->call.size());

                dcResultList.resize(responseDataPtr->call.size());
                int i=0;
                for (rildata::DataCallResult_t entry: responseDataPtr->call) {
                    QCRIL_LOG_DEBUG("cid %d", entry.cid);
                    QCRIL_LOG_DEBUG("ifname %s", entry.ifname.c_str());
                    QCRIL_LOG_DEBUG("addresses %s", entry.addresses.c_str());
                    dcResultList[i] = utils::convertDcResultToHidlDcResult_1_5(entry);
                    i++;
                }
            }
            else {
                switch (responseDataPtr->respErr) {
                    case ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                    case ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                    default: e = RadioError::GENERIC_FAILURE; break;
                }
            }
            responseInfo = {.type = RadioResponseType::SOLICITED, .serial = serial, .error = e};
        }
        auto ret = respCb->getDataCallListResponse_1_5(responseInfo, dcResultList);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
        }
    } else {
        return V1_4::implementation::RadioServiceImpl<T>::getDataCallListResponse(responseDataPtr, serial, status);
    }
  }

  virtual void setDataProfileResponse(V1_0::RadioResponseInfo responseInfo) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.5") {
        return V1_4::implementation::RadioServiceImpl<T>::setDataProfileResponse(responseInfo);
    }
#endif
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending setDataProfileResponse V1_5 Response");
      auto ret = respCb->setDataProfileResponse_1_5(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        return V1_4::implementation::RadioServiceImpl<T>::setDataProfileResponse(responseInfo);
    }
  }

  virtual void sendResponseForSendCdmaSmsExpectMore(
      int32_t serial, RIL_Errno errorCode, std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_0::SendSmsResult result = { -1, hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("sendCdmaSmsExpectMoreResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendCdmaSmsExpectMoreResponse(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSimDepersonalization(
      int32_t serial, V1_0::RadioError errorCode,
      ::android::hardware::radio::V1_5::PersoSubstate persotype, int retries) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();

    if (!respCb) {
      return;
    }
    auto ret = respCb->supplySimDepersonalizationResponse(responseInfo, persotype, retries);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetIccCardStatus(int32_t serial, V1_0::RadioError errorCode,
                                               std::shared_ptr<RIL_UIM_CardStatus> respData) override {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };

    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_4::implementation::RadioServiceImpl<T>::sendResponseForGetIccCardStatus(serial, errorCode,
                                                                                 respData);
      return;
    }
    V1_5::CardStatus cardStatus{};
    if (errorCode == V1_0::RadioError::NONE) {
      responseInfo.error = utils::convertGetIccCardStatusResponse_1_5(cardStatus, respData);
    }
    QCRIL_LOG_ERROR("Sending card_status_response");
    auto ret = respCb->getIccCardStatusResponse_1_5(responseInfo, cardStatus);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

 public:
  RadioServiceImpl() = default;

  virtual ~RadioServiceImpl() = default;

  static const HalServiceImplVersion& getVersion();

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

  ::android::hardware::Return<void> setSignalStrengthReportingCriteria_1_5(
      int32_t serial,
      const ::android::hardware::radio::V1_5::SignalThresholdInfo& signalThresholdInfo,
      ::android::hardware::radio::V1_5::AccessNetwork accessNetwork) {
    QCRIL_LOG_DEBUG("setSignalStrengthReportingCriteria_1_5: serial=%d", serial);

    RIL_Errno ret = RIL_E_SUCCESS;
    std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> args;
    bool disableAllTypes = false;

    string dump;
    dump += " hysteresis Ms: " + std::to_string(signalThresholdInfo.hysteresisMs);
    dump += " hysteresis db: " + std::to_string(signalThresholdInfo.hysteresisDb);
    dump += " thresholds: { ";
    for (int32_t th : signalThresholdInfo.thresholds) {
      dump += std::to_string(th) + ", ";
    }
    dump += "}";
    QCRIL_LOG_DEBUG("Arguments : %s", dump.c_str());

    ret = utils::sanityCheckSignalStrengthCriteriaParams(signalThresholdInfo, accessNetwork);
    if (ret == RIL_E_SUCCESS) {
      utils::fillInSignalStrengthCriteria(args, signalThresholdInfo, accessNetwork, disableAllTypes);

      auto msg = std::make_shared<NasSetSignalStrengthCriteria>(this->getContext(serial), args);
      if (msg) {
        if (disableAllTypes) {
          msg->setDisableAllTypes();
        }
        GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> msg,
                                                     Message::Callback::Status status,
                                                     std::shared_ptr<RIL_Errno> rsp) -> void {
          V1_0::RadioError errorCode =
              utils::convertMsgToRadioError(status, rsp ? *rsp : RIL_E_INTERNAL_ERR);
          this->sendResponseForSetSignalStrengthReportingCriteria_1_5(
              serial, static_cast<RIL_Errno>(errorCode));
        });
        msg->setCallback(&cb);
        msg->dispatch();
      } else {
        ret = RIL_E_NO_MEMORY;
      }
    } else {
      ret = RIL_E_INVALID_ARGUMENTS;
    }

    if (ret != RIL_E_SUCCESS) {
      this->sendResponseForSetSignalStrengthReportingCriteria_1_5(serial, ret);
    }
    return Void();
  }

  ::android::hardware::Return<void> setLinkCapacityReportingCriteria_1_5(
      int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps, int32_t hysteresisUlKbps,
      const ::android::hardware::hidl_vec<int32_t>& thresholdsDownlinkKbps,
      const ::android::hardware::hidl_vec<int32_t>& thresholdsUplinkKbps,
      ::android::hardware::radio::V1_5::AccessNetwork ran) {
    QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
    using namespace V1_0;
    using namespace utils;
    QCRIL_LOG_DEBUG("serial: %d, hysteresisMs: %d, hysteresisDlKbps: %d, hysteresisUlKbps: %d, ran: %d",
                    serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps, ran);
    QCRIL_LOG_DEBUG("thresholdsDownlinkKbps: {");
    for (int thrD : thresholdsDownlinkKbps) {
        QCRIL_LOG_DEBUG("| %d", thrD);
    }
    QCRIL_LOG_DEBUG("}");
#ifndef QMI_RIL_UTF
    rildata::LinkCapCriteria_t c = {
        .hysteresisMs = hysteresisMs,
        .hysteresisUlKbps = hysteresisUlKbps,
        .hysteresisDlKbps = hysteresisDlKbps,
        .thresholdsUplinkKbps = thresholdsUplinkKbps,
        .thresholdsDownlinkKbps = thresholdsDownlinkKbps,
    };
    c.ran = convertHidlAccessNetworkToDataAccessNetwork_1_5(ran);
    auto msg = std::make_shared<rildata::SetLinkCapRptCriteriaMessage>(c);
    if (msg) {
      GenericCallback<rildata::LinkCapCriteriaResult_t> cb(
        ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<rildata::LinkCapCriteriaResult_t> responseDataPtr) -> void {
            if (solicitedMsg && responseDataPtr) {
                RIL_Errno re = convertLcResultToRilError(*responseDataPtr);
                RadioError e = convertMsgToRadioError(status, re);
                RadioResponseInfo responseInfo = {
                    .serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                Log::getInstance().d(
                    "setLinkCapacityReportingCriteria_1_5 resp invoked Callback[msg = " +
                    solicitedMsg->dump() + "] executed. client data = " +
                    "set-ril-service-cb-token status = " + std::to_string((int)status));
                this->setLinkCapacityReportingCriteriaResponse(responseInfo);
            }
        }));
        msg->setCallback(&cb);
        msg->dispatch();
    }
#endif
    return Void();
  }

  ::android::hardware::Return<void> enableUiccApplications(int32_t serial, bool enable) {
    QCRIL_LOG_DEBUG("enableUiccApplications: serial=%d enable=%d", serial, enable);
    auto msg = std::make_shared<RilRequestEnableUiccAppMessage>(this->getContext(serial), enable);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForEnableUiccApplications(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForEnableUiccApplications(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> areUiccApplicationsEnabled(int32_t serial) {
    QCRIL_LOG_DEBUG("areUiccApplicationsEnabled: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetUiccAppStatusMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetUiccAppStatusResult_t> result{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              result = std::static_pointer_cast<qcril::interfaces::RilGetUiccAppStatusResult_t>(
                  resp->data);
            }
            this->sendResponseForAreUiccApplicationsEnabled(serial, errorCode, result);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForAreUiccApplicationsEnabled(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> setSystemSelectionChannels_1_5(
      int32_t serial, bool specifyChannels,
      const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::RadioAccessSpecifier>&
          specifiers) {
    QCRIL_LOG_DEBUG("setSystemSelectionChannels_1_5: serial=%d, channel info valid=%d", serial,
                    specifyChannels);
    std::shared_ptr<RilRequestSetSysSelChannelsMessage> msg;
    RIL_SysSelChannels ril_info{};
    RIL_Errno res = RIL_E_SUCCESS;

    // If specifyChannels is true scan specific bands otherwise scan all bands
    // If specifier length is zero, RIL should scan all bands.
    if (specifyChannels == true) {
      res = utils::fillSetSystemSelectionChannelRequest_1_5(specifiers, ril_info);
    }
    if (res == RIL_E_SUCCESS) {
      msg = std::make_shared<RilRequestSetSysSelChannelsMessage>(this->getContext(serial), ril_info);
      if (msg == nullptr) {
        res = RIL_E_NO_MEMORY;
      }
    }
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetSystemSelectionChannels_1_5(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetSystemSelectionChannels_1_5(serial, res);
    }
    return Void();
  }

  ::android::hardware::Return<void> startNetworkScan_1_5(
      int32_t serial, const ::android::hardware::radio::V1_5::NetworkScanRequest& request) {
    QCRIL_LOG_DEBUG("startNetworkScan_1_5: serial=%d", serial);
    this->mIsScanRequested = true;
    std::shared_ptr<RilRequestStartNetworkScanMessage> msg = nullptr;
    RIL_NetworkScanRequest scanRequest{};
    RIL_Errno res = utils::fillNetworkScanRequest_1_5(request, scanRequest);
    if (res == RIL_E_SUCCESS) {
      msg = std::make_shared<RilRequestStartNetworkScanMessage>(this->getContext(serial),
                                                                scanRequest);
      if (msg == nullptr) {
        res = RIL_E_NO_MEMORY;
      }
    }
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForStartNetworkScan_1_5(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForStartNetworkScan_1_5(serial, res);
    }
    return Void();
  }

  ::android::hardware::Return<void> setupDataCall_1_5(
      int32_t serial, ::android::hardware::radio::V1_5::AccessNetwork accessNetwork,
      const ::android::hardware::radio::V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
      ::android::hardware::radio::V1_2::DataRequestReason reason,
      const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>& addresses,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& dnses) {
    QCRIL_LOG_DEBUG("setupDataCall_1_5: serial %d", serial);
    using namespace rildata;
    using namespace utils;
    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;

    for (size_t i = 0 ; i < addresses.size(); i++ ) {
        radioAddresses.push_back(addresses[i].address);
    }
    for (size_t i = 0 ; i < dnses.size(); i++ ) {
        radioDnses.push_back(dnses[i]);
    }
    AccessNetwork_t accnet = convertHidlAccessNetworkToDataAccessNetwork_1_5(accessNetwork);

    auto msg =
      std::make_shared<SetupDataCallRequestMessage>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        convertHidlDataProfileInfoToRil(dataProfileInfo),
        roamingAllowed,
        (DataRequestReason_t)reason,
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
                QCRIL_LOG_ERROR("setupDataCall_1_5 resp is nullptr");
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  ::android::hardware::Return<void> setInitialAttachApn_1_5(
      int32_t serial, const ::android::hardware::radio::V1_5::DataProfileInfo& dataProfileInfo) {
    using namespace V1_0;
    using namespace utils;
    QCRIL_LOG_ERROR("setInitialAttachApn_1_5");
    rildata::DataProfileInfo_t profile = convertHidlDataProfileInfoToRil(dataProfileInfo);
    auto msg = std::make_shared<rildata::SetInitialAttachApnRequestMessage>(serial, profile, nullptr);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
                RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
                if (solicitedMsg && responseDataPtr) {
                    Log::getInstance().d(
                        "[setInitialAttachApn_1_5 cb invoked: [msg = " +
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

  ::android::hardware::Return<void> setDataProfile_1_5(
      int32_t serial,
      const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::DataProfileInfo>&
          profiles) {
    using namespace V1_0;
    using namespace utils;
    vector<rildata::DataProfileInfo_t> p;

    for (size_t i = 0; i < profiles.size(); i++) {
        /* copy all the profiles */
        rildata::DataProfileInfo_t t = convertHidlDataProfileInfoToRil(profiles[i]);
        p.push_back(t);
    }

    auto msg = std::make_shared<rildata::SetDataProfileRequestMessage>(serial, p);
    if(msg) {
        GenericCallback<RIL_Errno> cb(
            ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
                RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
                if (solicitedMsg && responseDataPtr) {
                    Log::getInstance().d(
                        "[SetDataProfile_1_5 cb invoked: [msg = " +
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
                this->setDataProfileResponse(responseInfo);
            }));
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  ::android::hardware::Return<void> setRadioPower_1_5(int32_t serial, bool powerOn,
                                                      bool forEmergencyCall,
                                                      bool preferredForEmergencyCall) {
    QCRIL_LOG_DEBUG("setRadioPower_1_5: serial=%d, powerOn=%d", serial, powerOn);

    auto msg = std::make_shared<RilRequestRadioPowerMessage>(
        this->getContext(serial), powerOn, forEmergencyCall, preferredForEmergencyCall);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetRadioPower_1_5(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetRadioPower_1_5(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> setIndicationFilter_1_5(
      int32_t serial,
      ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::IndicationFilter>
          indicationFilter) {
    QCRIL_LOG_DEBUG("setIndicationFilter_1_5: serial=%d, indicationFilter=%d", serial,
                    indicationFilter);
    bool is_barring = false;

#ifndef QMI_RIL_UTF
    auto regFailureReportingStatusMsg =
        std::make_shared<rildata::RegistrationFailureReportingStatusMessage>(
            (int32_t)indicationFilter & V1_5::IndicationFilter::REGISTRATION_FAILURE);
    if (regFailureReportingStatusMsg) {
      regFailureReportingStatusMsg->broadcast();
    } else {
      QCRIL_LOG_ERROR("Failed to create message RegistrationFailureReportingStatusMessage.");
    }
#endif

    if (indicationFilter & V1_5::IndicationFilter::BARRING_INFO) {
      is_barring = true;
    }

    auto msg = std::make_shared<RilRequestSetUnsolBarringFilterMessage>(
        this->getContext(serial), is_barring, indicationFilter);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            std::shared_ptr<qcril::interfaces::RilUnsolBarringFilterResult_t> int32Filter;
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              if (errorCode == RIL_E_SUCCESS) {
                int32Filter =
                    std::static_pointer_cast<qcril::interfaces::RilUnsolBarringFilterResult_t>(
                        resp->data);
                if (int32Filter != nullptr) {
                  this->setIndicationFilter_1_2(serial, int32Filter->mIndicationFilter);
                  errorCode = RIL_E_SUCCESS;
                }
              }
            }
            if (errorCode != RIL_E_SUCCESS) {
              this->sendResponseForSetIndicationFilter_1_5(serial, errorCode);
            }
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetIndicationFilter_1_5(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> getBarringInfo(int32_t serial) {
    QCRIL_LOG_INFO("getBarringInfo: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetBarringInfoMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilCellBarringInfo_t> rilCellBarring{};
            if (status == Message::Callback::Status::SUCCESS) {
              if (resp) {
                errorCode = resp->errorCode;
                rilCellBarring =
                    std::static_pointer_cast<qcril::interfaces::RilCellBarringInfo_t>(resp->data);
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
            this->sendResponseForGetBarringInfo(serial, errorCode, rilCellBarring);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetBarringInfo(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  ::android::hardware::Return<void> getVoiceRegistrationState_1_5(int32_t serial) {
    QCRIL_LOG_INFO("getVoiceRegistrationState_1_5: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetVoiceRegistrationMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilRegResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetVoiceRegResult_t>(resp->data);
            }
            this->sendResponseForGetVoiceRegistrationState_1_5(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetVoiceRegistrationState_1_5(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getDataRegistrationState_1_5(int32_t serial) {
    QCRIL_LOG_INFO("getDataRegistrationState_1_5: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetDataRegistrationMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilRegResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetDataRegResult_t>(resp->data);
            }
            this->sendResponseForGetDataRegistrationState_1_5(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetDataRegistrationState_1_5(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> setNetworkSelectionModeManual_1_5(
      int32_t serial, const ::android::hardware::hidl_string& operatorNumeric,
      ::android::hardware::radio::V1_5::RadioAccessNetworks ran) {
    QCRIL_LOG_INFO("setNetworkSelectionModeManual_1_5: serial=%d", serial);
    RIL_RadioTechnology rat = utils::getRilRadioTechnologyFromRan(ran);
    auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(
        this->getContext(serial), operatorNumeric, rat);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetNetworkSelectionModeManual_1_5(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetNetworkSelectionModeManual_1_5(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> sendCdmaSmsExpectMore(
      int32_t serial, const ::android::hardware::radio::V1_0::CdmaSmsMessage& sms) {
    QCRIL_LOG_INFO("sendCdmaSmsExpectMore: serial=%d", serial);
    RIL_CDMA_SMS_Message rcsm{};
    utils::constructCdmaSms(rcsm, sms);
    rcsm.expectMore = 1;
    auto msg = std::make_shared<RilRequestCdmaSendSmsMessage>(this->getContext(serial), rcsm);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial]([[maybe_unused]] std::shared_ptr<Message> msg,
                         Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<RilSendSmsResult_t> rilRegResult = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilRegResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
            }
            this->sendResponseForSendCdmaSmsExpectMore(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendCdmaSmsExpectMore(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> supplySimDepersonalization(
      int32_t serial, ::android::hardware::radio::V1_5::PersoSubstate persoType,
      const ::android::hardware::hidl_string& controlKey) {
    QCRIL_LOG_INFO("supplySimDepersonalization: serial=%d, persoType=%d ", serial, persoType);
    RIL_UIM_PersoSubstate rilPersoType = utils::convertHidlToRilPersoType(persoType);
    auto msg = std::make_shared<UimEnterDePersoRequestMsg>(controlKey, rilPersoType);
    if (msg) {
      GenericCallback<RIL_UIM_PersoResponse> cb(
          [this, serial, persoType](
              std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
              std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) -> void {
            V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                                  V1_0::RadioError::INTERNAL_ERR };
            int retries = -1;
            if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS) {
              responseInfo.error = static_cast<V1_0::RadioError>(responseDataPtr->err);
              retries = responseDataPtr->no_of_retries;
            }
            this->sendResponseForSimDepersonalization(serial, responseInfo.error, persoType,
                                                      retries);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSimDepersonalization(serial, V1_0::RadioError::NO_MEMORY, persoType, -1);
    }
    return Void();
  }

  /**
   * Notifies uiccApplicationsEnablementChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::uiccApplicationsEnablementChanged
   */
  int sendUiccAppsStatusChanged(std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_4::implementation::RadioServiceImpl<T>::sendUiccAppsStatusChanged(msg);
    }
    bool state = msg->getStatus();
    QCRIL_LOG_DEBUG("UNSOL: uiccApplicationsEnablementChanged");
    auto ret = ind->uiccApplicationsEnablementChanged(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, state);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  /**
   * Notifies registrationFailed indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::registrationFailed
   */
  int sendNwRegistrationReject(std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_4::implementation::RadioServiceImpl<T>::sendNwRegistrationReject(msg);
    }
    V1_5::CellIdentity hidl_cellIdentity;
    auto rejInfo = msg->getNwRegistrationRejectInfo();
    utils::convertCellIdentityRilToHidl(rejInfo.rilCellIdentity, hidl_cellIdentity);
    QCRIL_LOG_DEBUG("UNSOL: registrationFailed");
    auto ret = ind->registrationFailed(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, hidl_cellIdentity,
                                       hidl_string(rejInfo.choosenPlmn),
                                       static_cast<hidl_bitfield<V1_5::Domain>>(rejInfo.domain),
                                       rejInfo.causeCode, rejInfo.additionalCauseCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  /**
   * Notifies barringInfoChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::barringInfoChanged
   */
  int sendCellBarring(std::shared_ptr<RilUnsolCellBarringMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_4::implementation::RadioServiceImpl<T>::sendCellBarring(msg);
    }
    V1_5::CellIdentity hidl_cellIdentity{};
    hidl_vec<V1_5::BarringInfo> hidl_barringInfo{};
    auto barringInfo = msg->getCellBarringInfo();
    utils::convertCellIdentityRilToHidl(barringInfo.rilCellIdentity, hidl_cellIdentity);
    utils::convertRilBarringInfoList(barringInfo.barring_info, hidl_barringInfo);
    QCRIL_LOG_DEBUG("UNSOL: barringInfoChanged");
    auto ret = ind->barringInfoChanged(V1_0::RadioIndicationType::UNSOLICITED, hidl_cellIdentity,
                                       hidl_barringInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  /**
   * Notifies networkScanResult indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_5::IRadioIndication::networkScanResult_1_5
   *   V1_4::IRadioIndication::networkScanResult_1_4
   *   V1_2::IRadioIndication::networkScanResult_1_2
   *   V1_1::IRadioIndication::networkScanResult
   */
  int sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) override {
    if(this->mIsScanRequested == false){
      return 1;
    }
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_4::implementation::RadioServiceImpl<T>::sendNetworkScanResult(msg);
    }
    V1_5::NetworkScanResult resultV1_5;
    resultV1_5.status = static_cast<V1_1::ScanStatus>(msg->getStatus());
    resultV1_5.error = static_cast<V1_0::RadioError>(msg->getError());

    utils::convertRilCellInfoList(msg->getNetworkInfo(), resultV1_5.networkInfos);

    QCRIL_LOG_DEBUG("UNSOL: networkScanResult_1_5");
    auto ret = ind->networkScanResult_1_5(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, resultV1_5);
    if(resultV1_5.status == ::android::hardware::radio::V1_1::ScanStatus::COMPLETE){
      this->mIsScanRequested = false;
    }
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }
  /**
   * Notifies cellInfoList indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cellInfoList
   *   V1_2::IRadioIndication::cellInfoList_1_2
   *   V1_4::IRadioIndication::cellInfoList_1_4
   *   V1_5::IRadioIndication::cellInfoList_1_5
   */
  int sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_4::implementation::RadioServiceImpl<T>::sendCellInfoList(msg);
    }
    ::android::hardware::hidl_vec<V1_5::CellInfo> records{};
    utils::convertRilCellInfoList(msg->getCellInfoList(), records);
    QCRIL_LOG_DEBUG("UNSOL: cellInfoList_1_5");
    auto ret = ind->cellInfoList_1_5(V1_0::RadioIndicationType::UNSOLICITED, records);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }
  int sendSetupDataCallResponse(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.5") {
        return V1_4::implementation::RadioServiceImpl<T>::sendSetupDataCallResponse(msg);
    }
#endif
    QCRIL_LOG_DEBUG("Sending SetupDataCallRadioResponseIndMessage V1_5 %s",msg->dump().c_str());
    using namespace rildata;
    using namespace V1_0;
    if (msg == nullptr) {
        QCRIL_LOG_DEBUG("msg is nullptr");
        return 1;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
        auto rsp = msg->getResponse();
        auto status = msg->getStatus();
        auto serial = msg->getSerial();

        RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::NO_MEMORY};
        V1_5::SetupDataCallResult dcResult = {};
        dcResult.cause = V1_4::DataCallFailCause::ERROR_UNSPECIFIED;
        dcResult.suggestedRetryTime = -1;
        RadioError e = RadioError::NONE;
        if (status == Message::Callback::Status::SUCCESS) {
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
            QCRIL_LOG_DEBUG("mtuV4 = %d", rsp.call.mtuV4);
            QCRIL_LOG_DEBUG("mtuV6 = %d", rsp.call.mtuV6);

            dcResult = utils::convertDcResultToHidlDcResult_1_5(rsp.call);
        }
        else {
            switch(rsp.respErr) {
                case rildata::ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                case rildata::ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                default: e = RadioError::GENERIC_FAILURE; break;
            }
        }
        responseInfo = {.serial = serial, .error = e};
        auto ret = respCb->setupDataCallResponse_1_5(responseInfo, dcResult);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
            return 1;
        }
    }
    else {
        return V1_4::implementation::RadioServiceImpl<T>::sendSetupDataCallResponse(msg);
    }
    return 0;
  }

  int sendRadioDataCallListChangeInd(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.5") {
        return V1_4::implementation::RadioServiceImpl<T>::sendRadioDataCallListChangeInd(msg);
    }
#endif
    QCRIL_LOG_DEBUG("Sending RadioDataCallListChangeInd V1_5 %s",msg->dump().c_str());
    auto indCb = this->getIndicationCallback();
    if (msg == nullptr) {
        QCRIL_LOG_DEBUG("msg is nullptr");
        return 1;
    }
    if(indCb) {
      QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
      std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
      ::android::hardware::hidl_vec<V1_5::SetupDataCallResult> dcResultList;
      QCRIL_LOG_DEBUG("dcList %d",dcList.size());
      dcResultList.resize(dcList.size());
      int i=0;
      for (rildata::DataCallResult_t entry : dcList)
      {
        dcResultList[i] = utils::convertDcResultToHidlDcResult_1_5(entry);
        i++;
      }
      auto ret = indCb->dataCallListChanged_1_5(V1_0::RadioIndicationType::UNSOLICITED, dcResultList);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    else {
        return V1_4::implementation::RadioServiceImpl<T>::sendRadioDataCallListChangeInd(msg);
    }
    return 0;
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_5::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_5
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_5_H__
