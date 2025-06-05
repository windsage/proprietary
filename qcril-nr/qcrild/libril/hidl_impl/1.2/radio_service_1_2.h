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

#ifndef __RADIO_SERVICE_1_2_H__
#define __RADIO_SERVICE_1_2_H__

#include "android/hardware/radio/1.2/IRadio.h"
#include "android/hardware/radio/1.2/IRadioResponse.h"
#include "android/hardware/radio/1.2/IRadioIndication.h"
#include "android/hardware/radio/1.2/types.h"

#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/nas/NasSetSignalStrengthCriteria.h"
#include "interfaces/nas/NasEnablePhysChanConfigReporting.h"
#include "interfaces/nas/NasPhysChanConfigMessage.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.1/radio_service_1_1.h"
#include "hidl_impl/1.2/radio_service_utils_1_2.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {

extern void grabPartialWakeLock();

namespace hardware {
namespace radio {
namespace V1_2 {
namespace implementation {

// V1_2::IRadio implementation
template <typename T>
class RadioServiceImpl : public V1_1::implementation::RadioServiceImpl<T> {
 private:
  ::android::sp<android::hardware::radio::V1_2::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_2::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_2::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_2::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_2::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
    V1_1::implementation::RadioServiceImpl<T>::clearCallbacks_nolock();
  }
  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }
  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_2::setResponseFunctions_nolock");
    mRadioResponse = V1_2::IRadioResponse::castFrom(respCb).withDefault(nullptr);
    mRadioIndication = V1_2::IRadioIndication::castFrom(indCb).withDefault(nullptr);
    V1_1::implementation::RadioServiceImpl<T>::setResponseFunctions_nolock(respCb, indCb);
  }

  virtual void sendResponseForSetSignalStrengthReportingCriteria(int32_t serial,
                                                                 RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("setSignalStrengthReportingCriteriaResponse: serial=%d, error=%d", serial,
                    errorCode);
    auto ret = respCb->setSignalStrengthReportingCriteriaResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetCurrentCalls(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_1::implementation::RadioServiceImpl<T>::sendResponseForGetCurrentCalls(serial, errorCode,
                                                                                currentCalls);
      return;
    }
    ::android::hardware::hidl_vec<V1_2::Call> calls;
    if (errorCode == RIL_E_SUCCESS && currentCalls) {
      auto callList = currentCalls->getCallInfoList();
      if (!callList.empty()) {
        calls.resize(callList.size());
        for (uint32_t i = 0; i < callList.size(); i++) {
          utils::convertToHidl(calls[i], callList[i]);
        }
      }
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getCurrentCallsResponse_1_2: serial=%d, errorCode=%d, calls = %s", serial,
                    errorCode, toString(calls).c_str());
    auto ret = respCb->getCurrentCallsResponse_1_2(responseInfo, calls);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetSignalStrength(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_1::implementation::RadioServiceImpl<T>::sendResponseForGetSignalStrength(serial, errorCode,
                                                                                  rilSigResult);
      return;
    }
    V1_2::SignalStrength signalStrength{};
    if (errorCode == RIL_E_SUCCESS && rilSigResult) {
      utils::convertRilSignalStrengthToHal(signalStrength, rilSigResult->respData);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getSignalStrengthResponse_1_2: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getSignalStrengthResponse_1_2(responseInfo, signalStrength);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void setLinkCapacityReportingCriteriaResponse(V1_0::RadioResponseInfo responseInfo) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending setLinkCapacityReportingCriteriaResponse");
      auto ret = respCb->setLinkCapacityReportingCriteriaResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }
  virtual void sendResponseForGetVoiceRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_1::implementation::RadioServiceImpl<T>::sendResponseForGetVoiceRegistrationState(
          serial, errorCode, rilRegResult);
      return;
    }
    V1_2::VoiceRegStateResult voiceRegResponse{};
    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillVoiceRegistrationStateResponse(voiceRegResponse, rilRegResult->respData);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getVoiceRegistrationStateResponse_1_2: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getVoiceRegistrationStateResponse_1_2(responseInfo, voiceRegResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetDataRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_1::implementation::RadioServiceImpl<T>::sendResponseForGetDataRegistrationState(
          serial, errorCode, rilRegResult);
      return;
    }
    V1_2::DataRegStateResult dataRegResponse{};
    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillDataRegistrationStateResponse(dataRegResponse, rilRegResult->respData);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getDataRegistrationStateResponse_1_2: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getDataRegistrationStateResponse_1_2(responseInfo, dataRegResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetCellInfoList(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_1::implementation::RadioServiceImpl<T>::sendResponseForGetCellInfoList(
          serial, errorCode, cellInfoListResult);
      return;
    }
    ::android::hardware::hidl_vec<V1_2::CellInfo> cellInfos{};
    if (errorCode == RIL_E_SUCCESS && cellInfoListResult) {
      utils::convertRilCellInfoListToHal_1_2(cellInfoListResult->cellInfos, cellInfos);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getCellInfoListResponse_1_2: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCellInfoListResponse_1_2(responseInfo, cellInfos);
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
      V1_1::implementation::RadioServiceImpl<T>::sendResponseForGetIccCardStatus(serial, errorCode,
                                                                                 respData);
      return;
    }
    V1_2::CardStatus cardStatus{};
    if (errorCode == V1_0::RadioError::NONE) {
      responseInfo.error = utils::convertGetIccCardStatusResponse(cardStatus.base, respData);
      cardStatus.physicalSlotId = respData->physical_slot_id;
      cardStatus.atr = respData->atr;
      cardStatus.iccid = respData->iccid;
    }
    QCRIL_LOG_DEBUG("Sending Card_status response");
    auto ret = respCb->getIccCardStatusResponse_1_2(responseInfo, cardStatus);
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
      setResponseFunctions_nolock(respCb, indCb);
    }
#ifndef QMI_RIL_UTF
    // client is connected. Send initial indications.
    android::onNewCommandConnect();
#endif
    return Void();
  }

  ::android::hardware::Return<void> startNetworkScan_1_2(
      int32_t serial, const ::android::hardware::radio::V1_2::NetworkScanRequest& request) {
    QCRIL_LOG_DEBUG("startNetworkScan_1_2: serial=%d", serial);
    this->mIsScanRequested = true;
    std::shared_ptr<RilRequestStartNetworkScanMessage> msg;
    RIL_NetworkScanRequest scanRequest{};
    auto errResp = utils::fillNetworkScanRequest_1_2(request, scanRequest);
    if (errResp == RIL_E_SUCCESS) {
      msg = std::make_shared<RilRequestStartNetworkScanMessage>(this->getContext(serial),
                                                                scanRequest);
      if (msg == nullptr) {
        errResp = RIL_E_NO_MEMORY;
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
            this->sendResponseForStartNetworkScan(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForStartNetworkScan(serial, errResp);
    }
    return Void();
  }

  ::android::hardware::Return<void> setIndicationFilter_1_2(
      int32_t serial,
      ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_2::IndicationFilter>
          indicationFilter) {
    QCRIL_LOG_DEBUG("setIndicationFilter_1_2: serial=%d, indicationFilter=%d", serial,
                    indicationFilter);
    int32_t int32Filter = indicationFilter;
    auto msg = std::make_shared<NasEnablePhysChanConfigReporting>(
        (int32Filter & V1_2::IndicationFilter::PHYSICAL_CHANNEL_CONFIG) != 0,
        this->getContext(serial),
        [this, serial, indicationFilter](
            std::shared_ptr<Message> msg, Message::Callback::Status status,
            std::shared_ptr<qcril::interfaces::NasSettingResultCode> rc) -> void {
          QCRIL_LOG_DEBUG("Callback for NasEnablePhysChanConfigReporting. rc: %d",
                          rc ? static_cast<int>(*rc) : -1);
          V1_0::RadioError err = utils::convertMsgToRadioError(
              status, rc ? static_cast<RIL_Errno>(*rc) : RIL_E_INTERNAL_ERR);
          if (err != V1_0::RadioError::NONE) {
            QCRIL_LOG_ERROR("Error enabling ChanConfigReporting: %d", static_cast<int>(err));
          } else {
            std::shared_ptr<NasPhysChanConfigReportingStatus> phyChanConfigReportingStatusMsg =
                std::make_shared<NasPhysChanConfigReportingStatus>(
                    (int32_t)indicationFilter & V1_2::IndicationFilter::PHYSICAL_CHANNEL_CONFIG);
            if (phyChanConfigReportingStatusMsg) {
              phyChanConfigReportingStatusMsg->broadcast();
            } else {
              QCRIL_LOG_ERROR("Failed to create message NasPhysChanConfigReportingStatus.");
            }
          }

          int32_t int32Filter = indicationFilter &
                                (RIL_UR_SIGNAL_STRENGTH | RIL_UR_FULL_NETWORK_STATE |
                                 RIL_UR_DATA_CALL_DORMANCY_CHANGED | RIL_UR_LINK_CAPACITY_ESTIMATE);
          this->setIndicationFilter(serial, int32Filter);
        });
    msg->dispatch();
    return Void();
  }

  ::android::hardware::Return<void> setSignalStrengthReportingCriteria(
      int32_t serial, int32_t hysteresisMs, int32_t hysteresisDb,
      const ::android::hardware::hidl_vec<int32_t>& thresholdsDbm,
      ::android::hardware::radio::V1_2::AccessNetwork accessNetwork) {
    QCRIL_LOG_DEBUG("setSignalStrengthReportingCriteria: serial=%d", serial);
    RIL_Errno ret = RIL_E_SUCCESS;
    std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> args;

    string dump;
    dump += " hysteresis Ms: " + std::to_string(hysteresisMs);
    dump += " hysteresis db: " + std::to_string(hysteresisDb);
    dump += " thresholds: { ";
    for (int32_t th : thresholdsDbm) {
      dump += std::to_string(th) + ", ";
    }
    dump += "}";
    QCRIL_LOG_DEBUG("Arguments : %s", dump.c_str());

    ret = utils::sanityCheckSignalStrengthCriteriaParams(hysteresisMs, hysteresisDb, thresholdsDbm,
                                                         accessNetwork);
    if (ret == RIL_E_SUCCESS) {
      utils::fillInSignalStrengthCriteria(args, hysteresisMs, hysteresisDb, thresholdsDbm,
                                          accessNetwork);

      auto msg = std::make_shared<NasSetSignalStrengthCriteria>(this->getContext(serial), args);
      if (msg) {
        GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> msg,
                                                     Message::Callback::Status status,
                                                     std::shared_ptr<RIL_Errno> rsp) -> void {
          V1_0::RadioError errorCode =
              utils::convertMsgToRadioError(status, rsp ? *rsp : RIL_E_INTERNAL_ERR);
          this->sendResponseForSetSignalStrengthReportingCriteria(
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
      this->sendResponseForSetSignalStrengthReportingCriteria(serial, ret);
    }
    return Void();
  }

  ::android::hardware::Return<void> setLinkCapacityReportingCriteria(
      int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps, int32_t hysteresisUlKbps,
      const ::android::hardware::hidl_vec<int32_t>& thresholdsDownlinkKbps,
      const ::android::hardware::hidl_vec<int32_t>& thresholdsUplinkKbps,
      ::android::hardware::radio::V1_2::AccessNetwork ran) {
    QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
    using namespace rildata;
    using namespace V1_0;
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
    c.ran = utils::convertHidlAccessNetworkToDataAccessNetwork(ran);
    auto msg = std::make_shared<rildata::SetLinkCapRptCriteriaMessage>(c);
    if (msg) {
        GenericCallback<rildata::LinkCapCriteriaResult_t> cb(
        ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<rildata::LinkCapCriteriaResult_t> responseDataPtr) -> void {
            if (solicitedMsg && responseDataPtr) {
                RIL_Errno re = utils::convertLcResultToRilError(*responseDataPtr);
                RadioError e = utils::convertMsgToRadioError(status, re);
                RadioResponseInfo responseInfo = {
                    .serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
                Log::getInstance().d(
                    "[RilSvcDataSetLinkCapCriteriaCallback]: Callback[msg = " +
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
  ::android::hardware::Return<void> setupDataCall_1_2(
      int32_t serial, ::android::hardware::radio::V1_2::AccessNetwork accessNetwork,
      const ::android::hardware::radio::V1_0::DataProfileInfo& dataProfileInfo, bool modemCognitive,
      bool roamingAllowed, bool isRoaming,
      ::android::hardware::radio::V1_2::DataRequestReason reason,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& addresses,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& dnses) {
    QCRIL_LOG_DEBUG("SetupDataCall V1_2");
    using namespace rildata;
    using namespace utils;

    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;

    for (size_t i = 0 ; i < addresses.size(); i++ ) {
        radioAddresses.push_back(addresses[i]);
    }
    for (size_t i = 0 ; i < dnses.size(); i++ ) {
        radioDnses.push_back(dnses[i]);
    }
    AccessNetwork_t accnet = AccessNetwork_t::UNKNOWN;
    switch (accessNetwork) {
        case V1_2::AccessNetwork::GERAN:
            accnet = AccessNetwork_t::GERAN;
            break;
        case V1_2::AccessNetwork::UTRAN:
            accnet = AccessNetwork_t::UTRAN;
            break;
        case V1_2::AccessNetwork::EUTRAN:
            accnet = AccessNetwork_t::EUTRAN;
            break;
        case V1_2::AccessNetwork::CDMA2000:
            accnet = AccessNetwork_t::CDMA2000;
            break;
        case V1_2::AccessNetwork::IWLAN:
            accnet = AccessNetwork_t::IWLAN;
            break;
        default:
            Log::getInstance().d("Unknown AccessNetwork ran = "+std::to_string((int)accnet));
            break;
    }
    DataProfileInfo_t profileInfo = convertHidlDataProfileInfoToRil_1_0(dataProfileInfo);
    profileInfo.persistent = modemCognitive;

    auto msg =
      std::make_shared<SetupDataCallRequestMessage>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        profileInfo,
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
                QCRIL_LOG_ERROR("setupDataCall_1_4 resp is nullptr");
            }
         });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }
  ::android::hardware::Return<void> deactivateDataCall_1_2(
      int32_t serial, int32_t cid, ::android::hardware::radio::V1_2::DataRequestReason reason) {
    QCRIL_LOG_DEBUG("DeactivateDataCall V1_2");
    using namespace rildata;
    auto msg =
        std::make_shared<DeactivateDataCallRequestMessage>(
            serial,
            cid,
            (DataRequestReason_t)reason,
            nullptr);
    if (msg) {
        GenericCallback<ResponseError_t> cb([serial](std::shared_ptr<Message>,
                                                     Message::Callback::Status status,
                                                     std::shared_ptr<ResponseError_t> rsp) -> void {
            if (rsp != nullptr) {
                auto indMsg = std::make_shared<DeactivateDataCallRadioResponseIndMessage>(*rsp, serial, status);
                if (indMsg != nullptr) {
                    indMsg->broadcast();
                } else {
                    QCRIL_LOG_DEBUG("deactivate data call cb failed to allocate message status %d respErr %d", status, *rsp);
                }
            } else {
                QCRIL_LOG_ERROR("deactivate data call resp is nullptr");
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  /**
   * Notifies networkScanResult indication.
   * V1_2::IRadioIndication::networkScanResult_1_2
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
      return V1_1::implementation::RadioServiceImpl<T>::sendNetworkScanResult(msg);
    }
    V1_2::NetworkScanResult resultV1_2{};
    resultV1_2.status = static_cast<V1_1::ScanStatus>(msg->getStatus());
    resultV1_2.error = static_cast<V1_0::RadioError>(msg->getError());

    utils::convertRilCellInfoListToHal_1_2(msg->getNetworkInfo(), resultV1_2.networkInfos);
    QCRIL_LOG_DEBUG("UNSOL: networkScanResult_1_2");
    auto ret = ind->networkScanResult_1_2(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, resultV1_2);
    if(resultV1_2.status == ::android::hardware::radio::V1_1::ScanStatus::COMPLETE){
      this->mIsScanRequested = false;
    }
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  /**
   * Notifies networkScanResult indication.
   * V1_2::IRadioIndication::currentSignalStrength_1_2
   */
  int sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_1::implementation::RadioServiceImpl<T>::sendSignalStrength(msg);
    }
    V1_2::SignalStrength signalStrength{};
    utils::convertRilSignalStrengthToHal(signalStrength, msg->getSignalStrength());
    QCRIL_LOG_DEBUG("UNSOL: currentSignalStrength_1_2");
    auto ret =
        ind->currentSignalStrength_1_2(V1_0::RadioIndicationType::UNSOLICITED, signalStrength);
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
   */
  int sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_1::implementation::RadioServiceImpl<T>::sendCellInfoList(msg);
    }
    ::android::hardware::hidl_vec<V1_2::CellInfo> records{};
    utils::convertRilCellInfoListToHal_1_2(msg->getCellInfoList(), records);
    QCRIL_LOG_DEBUG("UNSOL: cellInfoList_1_2");
    auto ret = ind->cellInfoList_1_2(V1_0::RadioIndicationType::UNSOLICITED, records);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }
  int sendLinkCapInd(std::shared_ptr<rildata::LinkCapIndMessage> msg) {
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind == nullptr) {
      QCRIL_LOG_ERROR("Indcb is nullptr");
      return 1;
    }
    rildata::LinkCapEstimate_t &ilce = msg->getParams();
    V1_2::LinkCapacityEstimate lce = {
        .downlinkCapacityKbps = ilce.downlinkCapacityKbps,
        .uplinkCapacityKbps = ilce.uplinkCapacityKbps };
    QCRIL_LOG_DEBUG("Sending CURRENT_LINK_CAPACITY_ESTIMATE dl: %d ul: %d",
                    ilce.downlinkCapacityKbps, ilce.uplinkCapacityKbps );
    auto ret = ind->currentLinkCapacityEstimate(V1_0::RadioIndicationType::UNSOLICITED, lce);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      return 1;
    }
    return 0;
  }
  int sendPhysicalConfigStructUpdateInd(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg) {
    QCRIL_LOG_DEBUG("Sending sendPhysicalConfigStructUpdateInd V1_2 %s",msg->dump().c_str());
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind == nullptr) {
      QCRIL_LOG_ERROR("Indcb is nullptr");
      return 1;
    }
    hidl_vec<V1_2::PhysicalChannelConfig> physList;
    auto dataPhysList = msg->getPhysicalConfigStructInfo();
    physList.resize(dataPhysList.size());
    for (int i = 0; i < dataPhysList.size(); i++) {
        physList[i].status = static_cast<V1_2::CellConnectionStatus>(dataPhysList[i].status);
        physList[i].cellBandwidthDownlink = dataPhysList[i].cellBandwidthDownlink;
    }
    auto ret = ind->currentPhysicalChannelConfigs(V1_0::RadioIndicationType::UNSOLICITED, physList);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      return 1;
    }
    return 0;
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_2::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_2
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_2_H__
