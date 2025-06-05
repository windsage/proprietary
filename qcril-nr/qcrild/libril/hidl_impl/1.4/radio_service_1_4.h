/*
 * Copyright (c) 2019-2023 Qualcomm Technologies, Inc.
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

#ifndef __RADIO_SERVICE_1_4_H__
#define __RADIO_SERVICE_1_4_H__

#include "android/hardware/radio/1.4/IRadio.h"
#include "android/hardware/radio/1.4/IRadioResponse.h"
#include "android/hardware/radio/1.4/IRadioIndication.h"
#include "android/hardware/radio/1.4/types.h"

#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilRequestGetSignalStrengthMessage.h"
#include "interfaces/nas/RilRequestGetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestSetPrefNetworkTypeMessage.h"
#include "interfaces/voice/QcRilRequestDialMessage.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.3/radio_service_1_3.h"
#include "hidl_impl/1.4/radio_service_utils_1_4.h"
#include "HalServiceImplFactory.h"

#ifdef QMI_RIL_UTF
#include <cutils/properties.h>
#endif

#undef TAG
#define TAG "RILQ"

namespace android {

extern void grabPartialWakeLock();

namespace hardware {
namespace radio {
namespace V1_4 {
namespace implementation {

// V1_4::IRadio implementation
template <typename T>
class RadioServiceImpl : public V1_3::implementation::RadioServiceImpl<T> {
 private:
  ::android::sp<android::hardware::radio::V1_4::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_4::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_4::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_4::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_4::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
    V1_3::implementation::RadioServiceImpl<T>::clearCallbacks_nolock();
  }
  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }

#ifdef QMI_RIL_UTF
  string qcril_get_property_value(string name, string defaultValue) {
    char cPropValue[PROPERTY_VALUE_MAX] = {'\0'};
    property_get(name.c_str(), cPropValue, defaultValue.c_str());
    return string(cPropValue);
  }
#endif

  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_4::setResponseFunctions_nolock");
    mRadioResponse = V1_4::IRadioResponse::castFrom(respCb).withDefault(nullptr);
    mRadioIndication = V1_4::IRadioIndication::castFrom(indCb).withDefault(nullptr);
    V1_3::implementation::RadioServiceImpl<T>::setResponseFunctions_nolock(respCb, indCb);
  }

  virtual void sendResponseForGetCellInfoList(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_3::implementation::RadioServiceImpl<T>::sendResponseForGetCellInfoList(
          serial, errorCode, cellInfoListResult);
      return;
    }
    ::android::hardware::hidl_vec<V1_4::CellInfo> cellInfos{};
    if (errorCode == RIL_E_SUCCESS && cellInfoListResult) {
      utils::convertRilCellInfoListToHal_1_4(cellInfoListResult->cellInfos, cellInfos);
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getCellInfoListResponse_1_4: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCellInfoListResponse_1_4(responseInfo, cellInfos);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForEmergencyDial(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("emergencyDialResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->emergencyDialResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForstartNetworkScan_1_4(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("startNetworkScanResponse_1_4: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->startNetworkScanResponse_1_4(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetDataRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult) override {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      V1_3::implementation::RadioServiceImpl<T>::sendResponseForGetDataRegistrationState(
          serial, errorCode, rilRegResult);
      return;
    }

    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_4::DataRegStateResult dataRegResponse{};

    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillDataRegistrationStateResponse(dataRegResponse, rilRegResult->respData);
    }
    QCRIL_LOG_DEBUG("getDataRegistrationStateResponse_1_4: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getDataRegistrationStateResponse_1_4(responseInfo, dataRegResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetPreferredNetworkTypeBitmap(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    ::android::hardware::hidl_bitfield<RadioAccessFamily> networkTypeBitmap{ 0 };

    if (errorCode == RIL_E_SUCCESS && prefResult) {
      networkTypeBitmap = utils::convertRilNetworkTypeBitmapToHidl_1_4(prefResult->pref);
    }
    QCRIL_LOG_DEBUG("getPreferredNetworkTypeBitmapResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getPreferredNetworkTypeBitmapResponse(responseInfo, networkTypeBitmap);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetPreferredNetworkTypeBitmap(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("setPreferredNetworkTypeBitmapResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setPreferredNetworkTypeBitmapResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetAllowedCarriers_1_4(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("setAllowedCarriersResponse_1_4: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setAllowedCarriersResponse_1_4(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetAllowedCarriers_1_4(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_4::CarrierRestrictionsWithPriority carriers{};
    V1_4::SimLockMultiSimPolicy multiSimPolicy{ V1_4::SimLockMultiSimPolicy::NO_MULTISIM_POLICY };
    QCRIL_LOG_DEBUG("getAllowedCarriersResponse_1_4: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getAllowedCarriersResponse_1_4(responseInfo, carriers, multiSimPolicy);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForgetSignalStrength_1_4(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    V1_4::SignalStrength signalStrength{};
    if (errorCode == RIL_E_SUCCESS && rilSigResult) {
      utils::convertRilSignalStrengthToHal(rilSigResult->respData, signalStrength);
    }
    QCRIL_LOG_DEBUG("getSignalStrengthResponse_1_4: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getSignalStrengthResponse_1_4(responseInfo, signalStrength);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void getDataCallListResponse(std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr,
                                         int serial, Message::Callback::Status status) {
    QCRIL_LOG_DEBUG("GetDataCallListResponse V1_4");
    using namespace rildata;
    using namespace V1_0;
    auto respCb = this->getResponseCallback();
    if (respCb) {
        ::android::hardware::hidl_vec<V1_4::SetupDataCallResult> dcResultList;
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
                    dcResultList[i] = utils::convertDcResultToHidlDcResult_1_4(entry);
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
        auto ret = respCb->getDataCallListResponse_1_4(responseInfo, dcResultList);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
        }
    } else {
        return V1_3::implementation::RadioServiceImpl<T>::getDataCallListResponse(responseDataPtr, serial, status);
    }
 }

 virtual void sendResponseForGetIccCardStatus(int32_t serial, V1_0::RadioError errorCode,
                                              std::shared_ptr<RIL_UIM_CardStatus> respData) override {
   V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                         static_cast<V1_0::RadioError>(errorCode) };

   auto respCb = this->getResponseCallback();
   if (!respCb) {
     V1_3::implementation::RadioServiceImpl<T>::sendResponseForGetIccCardStatus(serial, errorCode,
                                                                                respData);
     return;
   }
   V1_4::CardStatus cardStatus{};
   if (errorCode == V1_0::RadioError::NONE) {
     responseInfo.error = utils::convertGetIccCardStatusResponse(cardStatus.base.base, respData);
     cardStatus.base.physicalSlotId = respData->physical_slot_id;
     cardStatus.base.atr = respData->atr;
     cardStatus.base.iccid = respData->iccid;
   }
   if (!respData->eid.empty()) {
     cardStatus.eid = respData->eid;
   }
   QCRIL_LOG_ERROR("Sending Card_status_reposne");
   auto ret = respCb->getIccCardStatusResponse_1_4(responseInfo, cardStatus);
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

  ::android::hardware::Return<void> setupDataCall_1_4(
      int32_t serial, ::android::hardware::radio::V1_4::AccessNetwork accessNetwork,
      const ::android::hardware::radio::V1_4::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
      ::android::hardware::radio::V1_2::DataRequestReason reason,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& addresses,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& dnses) {
    QCRIL_LOG_DEBUG("SetupDataCall V1_4");
    using namespace utils;
    using namespace rildata;
    using namespace V1_0;
    std::vector<std::string> radioAddresses;
    std::vector<std::string> radioDnses;

    for (size_t i = 0 ; i < addresses.size(); i++ ) {
        radioAddresses.push_back(addresses[i]);
    }
    for (size_t i = 0 ; i < dnses.size(); i++ ) {
        radioDnses.push_back(dnses[i]);
    }

    AccessNetwork_t accnet = convertHidlAccessNetworkToDataAccessNetwork_1_4(accessNetwork);

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
                QCRIL_LOG_ERROR("setupDataCall_1_4 resp is nullptr");
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  ::android::hardware::Return<void> setInitialAttachApn_1_4(
      int32_t serial, const ::android::hardware::radio::V1_4::DataProfileInfo& dataProfileInfo) {
    QCRIL_LOG_DEBUG("SetInitialAttachApn V1_4");
    using namespace utils;
    using namespace rildata;
    using namespace V1_0;
    rildata::DataProfileInfo_t profile = convertHidlDataProfileInfoToRil(dataProfileInfo);
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

  ::android::hardware::Return<void> setDataProfile_1_4(
      int32_t serial,
      const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::DataProfileInfo>&
          profiles) {
    QCRIL_LOG_DEBUG("SetDataProfile V1_4");
    using namespace utils;
    using namespace rildata;
    using namespace V1_0;
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
                    "[SetDataProfile_1_4 cb invoked: [msg = " +
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

  ::android::hardware::Return<void> emergencyDial(
      int32_t serial, const ::android::hardware::radio::V1_0::Dial& dialInfo,
      ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::EmergencyServiceCategory>
          categories,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& urns,
      ::android::hardware::radio::V1_4::EmergencyCallRouting routing, bool fromEmergencyDialer,
      bool isTesting) {
    QCRIL_LOG_DEBUG("emergencyDial: serial=%d", serial);
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
      msg->setClir(static_cast<int>(dialInfo.clir));
      msg->setCallDomain(qcril::interfaces::CallDomain::CS);
      if (dialInfo.uusInfo.size() != 0) {
        auto uusInfo = std::make_shared<qcril::interfaces::UusInfo>();
        if (uusInfo) {
          uusInfo->setType(static_cast<RIL_UUS_Type>(dialInfo.uusInfo[0].uusType));
          uusInfo->setDcs(static_cast<RIL_UUS_DCS>(dialInfo.uusInfo[0].uusDcs));
          if (dialInfo.uusInfo[0].uusData.size() > 0) {
            uusInfo->setData(std::string(dialInfo.uusInfo[0].uusData.c_str(),
                                         dialInfo.uusInfo[0].uusData.size()));
          }
          msg->setUusInfo(uusInfo);
        }
      }

      msg->setIsEmergency(true);
      msg->setCategories(utils::toRilCategories(categories));
      msg->setRouting(utils::toRilRouting(routing));
      msg->setIsForEccTesting(isTesting);
      msg->setIsIntentionEcc(false);
      msg->setIsIntentionEcc(fromEmergencyDialer);

      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForEmergencyDial(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (sendFailure) {
      this->sendResponseForEmergencyDial(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> startNetworkScan_1_4(
      int32_t serial, const ::android::hardware::radio::V1_2::NetworkScanRequest& request) {
    QCRIL_LOG_DEBUG("startNetworkScan_1_4: serial=%d", serial);
    this->mIsScanRequested = true;

    std::shared_ptr<RilRequestStartNetworkScanMessage> msg;
    RIL_NetworkScanRequest scanRequest = {};
    auto res = utils::fillNetworkScanRequest_1_2(request, scanRequest);
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
            this->sendResponseForstartNetworkScan_1_4(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForstartNetworkScan_1_4(serial, res);
    }
    return Void();
  }

  ::android::hardware::Return<void> getPreferredNetworkTypeBitmap(int32_t serial) {
    QCRIL_LOG_DEBUG("getPreferredNetworkTypeBitmap: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetPrefNetworkTypeMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              prefResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetModePrefResult_t>(resp->data);
            }
            this->sendResponseForGetPreferredNetworkTypeBitmap(serial, errorCode, prefResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetPreferredNetworkTypeBitmap(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> setPreferredNetworkTypeBitmap(
      int32_t serial,
      ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily>
          networkTypeBitmap) {
    QCRIL_LOG_DEBUG("setPreferredNetworkTypeBitmap: serial=%d", serial);
    auto msg = std::make_shared<RilRequestSetPrefNetworkTypeMessage>(
        this->getContext(serial), utils::convertHidlNetworkTypeBitmapToRil_1_4(networkTypeBitmap));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForSetPreferredNetworkTypeBitmap(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetPreferredNetworkTypeBitmap(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> setAllowedCarriers_1_4(
      int32_t serial,
      const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& carriers,
      ::android::hardware::radio::V1_4::SimLockMultiSimPolicy multiSimPolicy) {
    QCRIL_LOG_DEBUG("setAllowedCarriers_1_4: serial=%d", serial);
    this->sendResponseForSetAllowedCarriers_1_4(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> getAllowedCarriers_1_4(int32_t serial) {
    QCRIL_LOG_DEBUG("getAllowedCarriers_1_4: serial=%d", serial);
    this->sendResponseForGetAllowedCarriers_1_4(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> getSignalStrength_1_4(int32_t serial) {
    QCRIL_LOG_DEBUG("getSignalStrength_1_4: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetSignalStrengthMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilSigResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetSignalStrengthResult_t>(
                      resp->data);
            }
            this->sendResponseForgetSignalStrength_1_4(serial, errorCode, rilSigResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForgetSignalStrength_1_4(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  /**
   * Notifies currentEmergencyNumberList indication.
   * V1_4::IRadioIndication::currentEmergencyNumberList
   */
  int sendCurrentEmergencyNumberList(std::shared_ptr<RilUnsolEmergencyListIndMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_3::implementation::RadioServiceImpl<T>::sendCurrentEmergencyNumberList(msg);
    }
    ::android::hardware::hidl_vec<V1_4::EmergencyNumber> ecc_list;
    utils::convertRilEccListToHal(msg->getEmergencyList(), ecc_list);
    QCRIL_LOG_DEBUG("UNSOL: currentEmergencyNumberList");
    auto ret = ind->currentEmergencyNumberList(V1_0::RadioIndicationType::UNSOLICITED, ecc_list);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  /**
   * Notifies networkScanResult indication.
   * V1_4::IRadioIndication::networkScanResult_1_4
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
      return V1_3::implementation::RadioServiceImpl<T>::sendNetworkScanResult(msg);
    }
    V1_4::NetworkScanResult resultV1_4{};
    resultV1_4.status = static_cast<V1_1::ScanStatus>(msg->getStatus());
    resultV1_4.error = static_cast<V1_0::RadioError>(msg->getError());

    utils::convertRilCellInfoListToHal_1_4(msg->getNetworkInfo(), resultV1_4.networkInfos);
    QCRIL_LOG_DEBUG("UNSOL: networkScanResult_1_4");
    auto ret = ind->networkScanResult_1_4(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, resultV1_4);
    if(resultV1_4.status == ::android::hardware::radio::V1_1::ScanStatus::COMPLETE){
      this->mIsScanRequested = false;
    }
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  /**
   * Notifies networkScanResult indication.
   * V1_4::IRadioIndication::currentSignalStrength_1_4
   */
  int sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_3::implementation::RadioServiceImpl<T>::sendSignalStrength(msg);
    }
    V1_4::SignalStrength signalStrength{};
    utils::convertRilSignalStrengthToHal(msg->getSignalStrength(), signalStrength);
    QCRIL_LOG_DEBUG("UNSOL: currentSignalStrength_1_4");
    auto ret =
        ind->currentSignalStrength_1_4(V1_0::RadioIndicationType::UNSOLICITED, signalStrength);
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
   */
  int sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_3::implementation::RadioServiceImpl<T>::sendCellInfoList(msg);
    }
    ::android::hardware::hidl_vec<V1_4::CellInfo> records{};
    utils::convertRilCellInfoListToHal_1_4(msg->getCellInfoList(), records);
    QCRIL_LOG_DEBUG("UNSOL: cellInfoList_1_4");
    auto ret = ind->cellInfoList_1_4(V1_0::RadioIndicationType::UNSOLICITED, records);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      return 1;
    }
    return 0;
  }
  int sendSetupDataCallResponse(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg) {
    QCRIL_LOG_DEBUG("Sending SetupDataCallRadioResponseIndMessage V1_4 %s",msg->dump().c_str());
    using namespace rildata;
    using namespace V1_0;
    auto respCb = this->getResponseCallback();
    if (msg && respCb) {
        auto rsp = msg->getResponse();
        auto status = msg->getStatus();
        auto serial = msg->getSerial();

        RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::NO_MEMORY};
        V1_4::SetupDataCallResult dcResult = {};
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
            QCRIL_LOG_DEBUG("mtu = %d", rsp.call.mtu);

            dcResult = utils::convertDcResultToHidlDcResult_1_4(rsp.call);
        }
        else {
            switch(rsp.respErr) {
                case rildata::ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                case rildata::ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                default: e = RadioError::GENERIC_FAILURE; break;
            }
        }
        responseInfo = {.serial = serial, .error = e};
        auto ret = respCb->setupDataCallResponse_1_4(responseInfo, dcResult);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
            return 1;
        }
    }
    else {
        return V1_3::implementation::RadioServiceImpl<T>::sendSetupDataCallResponse(msg);
    }
    return 0;
  }

  int sendPhysicalConfigStructUpdateInd(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg) {
    QCRIL_LOG_DEBUG("Sending sendPhysicalConfigStructUpdateInd V1_4 %s",msg->dump().c_str());
    if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind == nullptr) {
        return V1_2::implementation::RadioServiceImpl<T>::sendPhysicalConfigStructUpdateInd(msg);
    }
    hidl_vec<V1_4::PhysicalChannelConfig> physList;
    auto dataPhysList = msg->getPhysicalConfigStructInfo();
    physList.resize(dataPhysList.size());
    for (int i = 0; i < dataPhysList.size(); i++) {
        physList[i].base.status = static_cast<V1_2::CellConnectionStatus>(dataPhysList[i].status);
        physList[i].base.cellBandwidthDownlink = dataPhysList[i].cellBandwidthDownlink;
        physList[i].rat = utils::convertRadioAccessFamilyTypeToRadioTechnology(dataPhysList[i].rat);
        V1_4::FrequencyRange range = utils::convertFrequencyRangeToHalType(dataPhysList[i].rfInfo.range);
        physList[i].rfInfo.range(range);
        int j=0;
        physList[i].contextIds.resize(dataPhysList[i].contextIds.size());
        for (const auto &cid : dataPhysList[i].contextIds) {
            physList[i].contextIds[j] = cid;
            j++;
        }
        physList[i].physicalCellId = dataPhysList[i].physicalCellId;
    }
    auto ret = ind->currentPhysicalChannelConfigs_1_4(V1_0::RadioIndicationType::UNSOLICITED, physList);
    if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        return 1;
    }
    return 0;
  }

  int sendRadioDataCallListChangeInd(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg) {
    QCRIL_LOG_DEBUG("Sending RadioDataCallListChangeInd V1_4 %s",msg->dump().c_str());
    auto indCb = this->getIndicationCallback();
    if (msg == nullptr) {
        QCRIL_LOG_DEBUG("msg is nullptr");
        return 1;
    }
    if(indCb) {
      QCRIL_LOG_DEBUG("Handling handleRadioDataCallListChangeIndMessage %s", msg->dump().c_str());
      std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
      ::android::hardware::hidl_vec<V1_4::SetupDataCallResult> dcResultList;
      QCRIL_LOG_DEBUG("dcList %d",dcList.size());
      dcResultList.resize(dcList.size());
      int i=0;
      for (rildata::DataCallResult_t entry : dcList)
      {
        dcResultList[i] = utils::convertDcResultToHidlDcResult_1_4(entry);
        i++;
      }
      auto ret = indCb-> dataCallListChanged_1_4(V1_0::RadioIndicationType::UNSOLICITED, dcResultList);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    else {
        return V1_0::implementation::RadioServiceImpl<T>::sendRadioDataCallListChangeInd(msg);
    }
    return 0;
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_4::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_4
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_4_H__
