/******************************************************************************
#  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_1_6_H__
#define __RADIO_SERVICE_1_6_H__

#include "android/hardware/radio/1.6/IRadio.h"
#include "android/hardware/radio/1.6/IRadioResponse.h"
#include "android/hardware/radio/1.6/IRadioIndication.h"
#include "android/hardware/radio/1.6/types.h"

#include "interfaces/nas/RilRequestGetSysSelChannelsMessage.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.5/radio_service_1_5.h"
#include "hidl_impl/1.6/radio_service_utils_1_6.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {

extern void grabPartialWakeLock();

namespace hardware {
namespace radio {
namespace V1_6 {
namespace implementation {

// V1_6::IRadio implementation
template <typename T>
class RadioServiceImpl : public V1_5::implementation::RadioServiceImpl<T> {
 private:
  ::android::sp<android::hardware::radio::V1_6::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_6::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_6::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_6::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_6::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
    V1_5::implementation::RadioServiceImpl<T>::clearCallbacks_nolock();
  }
  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }

  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_6::setResponseFunctions_nolock");
    mRadioResponse = V1_6::IRadioResponse::castFrom(respCb).withDefault(nullptr);
    mRadioIndication = V1_6::IRadioIndication::castFrom(indCb).withDefault(nullptr);
    V1_5::implementation::RadioServiceImpl<T>::setResponseFunctions_nolock(respCb, indCb);
  }

  virtual void sendResponseForSetRadioPower_1_6(int32_t serial, RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("setRadioPowerResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setRadioPowerResponse_1_6(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSendSms_1_6(int32_t serial, RIL_Errno errorCode,
                                          std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }

    V1_0::SendSmsResult result = { -1, hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }

    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };

    QCRIL_LOG_DEBUG("sendSmsResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendSmsResponse_1_6(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSendSMSExpectMore_1_6(
      int32_t serial, RIL_Errno errorCode, std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }

    V1_0::SendSmsResult result = { -1, hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }

    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };

    QCRIL_LOG_DEBUG("sendSmsExpectMoreResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendSmsExpectMoreResponse_1_6(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSendCdmaSms_1_6(int32_t serial, RIL_Errno errorCode,
                                              std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }

    V1_0::SendSmsResult result = { -1, hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }

    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };

    QCRIL_LOG_DEBUG("sendCdmaSmsResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendCdmaSmsResponse_1_6(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSendCdmaSmsExpectMore_1_6(
      int32_t serial, RIL_Errno errorCode, std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }

    V1_0::SendSmsResult result = { -1, hidl_string(), -1 };
    if (sendSmsResult) {
      result = utils::makeSendSmsResult(sendSmsResult);
    }

    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };

    QCRIL_LOG_DEBUG("sendCdmaSmsExpectMoreResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->sendCdmaSmsExpectMoreResponse_1_6(responseInfo, result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetSimCardPower_1_6(int32_t serial, RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("setSimCardPowerResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setSimCardPowerResponse_1_6(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetNrDualConnectivityState(int32_t serial, RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("setNrDualConnectivityStateResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setNrDualConnectivityStateResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForIsNrDualConnectivityEnabled(int32_t serial, RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("isNrDualConnectivityEnabledResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->isNrDualConnectivityEnabledResponse(responseInfo, false);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetAllowedNetworkTypesBitmap(int32_t serial, RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("setAllowedNetworkTypesBitmapResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setAllowedNetworkTypesBitmapResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetAllowedNetworkTypesBitmap(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    ::android::hardware::hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap{ 0 };

    if (errorCode == RIL_E_SUCCESS && prefResult) {
      networkTypeBitmap = utils::convertRilNetworkTypeBitmapToHidl_1_4(prefResult->pref);
    }
    QCRIL_LOG_DEBUG("getAllowedNetworkTypesBitmapResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getAllowedNetworkTypesBitmapResponse(responseInfo, networkTypeBitmap);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetSystemSelectionChannels(int32_t serial,
      RIL_Errno errorCode, std::shared_ptr<RilGetSysSelResult_t> info) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };

    hidl_vec<V1_5::RadioAccessSpecifier> specifiers {};
    if(errorCode == RIL_E_SUCCESS) {
      if(info != nullptr) {
        utils::convertToHidl(specifiers, info->respData);
      } else {
        errorCode = RIL_E_INTERNAL_ERR;
      }
    }
    QCRIL_LOG_DEBUG("getSystemSelectionChannelsResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getSystemSelectionChannelsResponse(responseInfo, specifiers);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetCellInfoList_1_6(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    ::android::hardware::hidl_vec<V1_6::CellInfo> cellInfos{};
    if (errorCode == RIL_E_SUCCESS && cellInfoListResult) {
      utils::convertRilCellInfoList(cellInfoListResult->cellInfos, cellInfos);
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getCellInfoListResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCellInfoListResponse_1_6(responseInfo, cellInfos);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetVoiceRegistrationState_1_6(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RegStateResult regResponse{};
    if (errorCode == RIL_E_SUCCESS) {
      if (rilRegResult) {
        utils::fillVoiceRegistrationStateResponse(regResponse, rilRegResult->respData);
      } else {
        errorCode = RIL_E_NO_MEMORY;
      }
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getVoiceRegistrationStateResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getVoiceRegistrationStateResponse_1_6(responseInfo, regResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetCurrentCalls_1_6(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
        return;
    }
    ::android::hardware::hidl_vec<V1_6::Call> calls;
    if (errorCode == RIL_E_SUCCESS && currentCalls) {
      auto callList = currentCalls->getCallInfoList();
      if (!callList.empty()) {
        std::vector<V1_6::Call> hidlCalls{};
        hidlCalls.reserve(callList.size());
        for (uint32_t i = 0; i < callList.size(); i++) {
          if (callList[i].getCallState() != qcril::interfaces::CallState::END) {
            V1_6::Call hidlCall{};
            utils::convertToHidl(hidlCall, callList[i]);
            hidlCalls.push_back(std::move(hidlCall));
          }
        }
        if (!hidlCalls.empty()) {
          calls = hidlCalls;
        }
      }
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getCurrentCallsResponse_1_6: serial=%d, errorCode=%d, calls = %s", serial,
                    errorCode, toString(calls).c_str());
    auto ret = respCb->getCurrentCallsResponse_1_6(responseInfo, calls);
    if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetDataRegistrationState_1_6(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RegStateResult regResponse{};
    if (errorCode == RIL_E_SUCCESS && rilRegResult) {
      utils::fillDataRegistrationStateResponse(regResponse, rilRegResult->respData);
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getDataRegistrationStateResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getDataRegistrationStateResponse_1_6(responseInfo, regResponse);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForgetSignalStrength_1_6(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    V1_6::SignalStrength signalStrength{};
    if (errorCode == RIL_E_SUCCESS && rilSigResult) {
      utils::convertRilSignalStrengthToHal(rilSigResult->respData, signalStrength);
    }
    QCRIL_LOG_DEBUG("getSignalStrengthResponse_1_6: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getSignalStrengthResponse_1_6(responseInfo, signalStrength);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void getDataCallListResponse(std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr,
                                       int serial, Message::Callback::Status status) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.6") {
        return V1_5::implementation::RadioServiceImpl<T>::getDataCallListResponse(responseDataPtr, serial, status);
    }
#endif
    QCRIL_LOG_DEBUG("GetDataCallListResponse V1_6");
    using namespace rildata;
    using namespace V1_0;
    auto respCb = this->getResponseCallback();
    if (!respCb) {
        return V1_5::implementation::RadioServiceImpl<T>::getDataCallListResponse(responseDataPtr, serial, status);
    }
    ::android::hardware::hidl_vec<V1_6::SetupDataCallResult> dcResultList;
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
                dcResultList[i] = utils::convertDcResultToHidlDcResult_1_6(entry);
                i++;
            }
        } else {
            switch (responseDataPtr->respErr) {
                case ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                case ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                default: e = RadioError::GENERIC_FAILURE; break;
            }
        }
        responseInfo = {.type = RadioResponseType::SOLICITED, .serial = serial, .error = e};
    }
    auto ret = respCb->getDataCallListResponse_1_6(responseInfo, dcResultList);
    if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
}

int sendSetupDataCallResponse(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.6") {
        return V1_5::implementation::RadioServiceImpl<T>::sendSetupDataCallResponse(msg);
    }
#endif
    QCRIL_LOG_DEBUG("Sending SetupDataCallRadioResponseIndMessage V1_6 %s",msg->dump().c_str());
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
        V1_6::SetupDataCallResult dcResult = {};
        dcResult.cause = V1_6::DataCallFailCause::ERROR_UNSPECIFIED;
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
            QCRIL_LOG_DEBUG("handoverFailureMode = %d", rsp.call.handoverFailureMode);
            QCRIL_LOG_DEBUG("tdSize = %d", rsp.call.trafficDescriptors.size());

            dcResult = utils::convertDcResultToHidlDcResult_1_6(rsp.call);
        }
        else {
            switch(rsp.respErr) {
                case rildata::ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
                case rildata::ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                default: e = RadioError::GENERIC_FAILURE; break;
            }
        }
        responseInfo = {.serial = serial, .error = e};
        auto ret = respCb->setupDataCallResponse_1_6(responseInfo, dcResult);
        if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
            return 1;
        }
    }
    else {
        return V1_5::implementation::RadioServiceImpl<T>::sendSetupDataCallResponse(msg);
    }
    return 0;
}

  int sendPhysicalConfigStructUpdateInd(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg) {
    // use currentPhysicalChannelConfigs_1_4 with modemReducedFeatureSet as true
    return V1_4::implementation::RadioServiceImpl<T>::sendPhysicalConfigStructUpdateInd(msg);
  }

  int sendRadioDataCallListChangeInd(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg) {
#ifdef QMI_RIL_UTF
    string rilVersion = V1_4::implementation::RadioServiceImpl<T>::qcril_get_property_value("vendor.radio.utf.version", "1.4");
    QCRIL_LOG_DEBUG("RadioImpl read property version %s", rilVersion.c_str());
    if (rilVersion != "1.6") {
        return V1_5::implementation::RadioServiceImpl<T>::sendRadioDataCallListChangeInd(msg);
    }
#endif
    QCRIL_LOG_DEBUG("Sending RadioDataCallListChangeInd V1_6 %s",msg->dump().c_str());
    auto indCb = this->getIndicationCallback();
    if (msg == nullptr) {
        QCRIL_LOG_DEBUG("msg is nullptr");
        return 1;
    }
    if(indCb) {
      QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
      std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
      ::android::hardware::hidl_vec<V1_6::SetupDataCallResult> dcResultList;
      QCRIL_LOG_DEBUG("dcList %d",dcList.size());
      dcResultList.resize(dcList.size());
      int i=0;
      for (rildata::DataCallResult_t entry : dcList)
      {
        dcResultList[i] = utils::convertDcResultToHidlDcResult_1_6(entry);
        i++;
      }
      auto ret = indCb->dataCallListChanged_1_6(V1_0::RadioIndicationType::UNSOLICITED, dcResultList);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
      }
    }
    else {
        return V1_5::implementation::RadioServiceImpl<T>::sendRadioDataCallListChangeInd(msg);
    }
    return 0;
  }

  virtual void sendResponseForAllocatePduSessionId(int32_t serial, RIL_Errno errorCode) {
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      QCRIL_LOG_DEBUG("sendResponseForAllocatePduSessionId: respCb is nullptr");
      return;
    }
    QCRIL_LOG_DEBUG("sendResponseForAllocatePduSessionId: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->allocatePduSessionIdResponse(responseInfo, 0);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForReleasePduSessionId(int32_t serial, RIL_Errno errorCode) {
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      QCRIL_LOG_DEBUG("sendResponseForReleasePduSessionId: respCb is nullptr");
      return;
    }
    QCRIL_LOG_DEBUG("sendResponseForReleasePduSessionId: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->releasePduSessionIdResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForStartHandover(int32_t serial, RIL_Errno errorCode) {
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      QCRIL_LOG_DEBUG("sendResponseForStartHandover: respCb is nullptr");
      return;
    }
    QCRIL_LOG_DEBUG("sendResponseForStartHandover: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->startHandoverResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetSimPhonebookCapacity(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adnCountInfo) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }

    V1_6::PhonebookCapacity capacity;
    utils::convertRilPbCapacity(adnCountInfo, capacity);

    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("getSimPhonebookCapacityResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getSimPhonebookCapacityResponse(responseInfo, capacity);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForCancelHandover(int32_t serial, RIL_Errno errorCode) {
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      QCRIL_LOG_DEBUG("sendResponseForCancelHandover: respCb is nullptr");
      return;
    }
    QCRIL_LOG_DEBUG("sendResponseForCancelHandover: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->cancelHandoverResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetSimPhonebookRecords(int32_t serial, RIL_Errno errorCode) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode)};
    QCRIL_LOG_DEBUG("getSimPhonebookRecordsResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getSimPhonebookRecordsResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForGetSlicingConfig(int32_t serial, RIL_Errno errorCode) {
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      QCRIL_LOG_DEBUG("sendResponseForGetSlicingConfig: respCb is nullptr");
      return;
    }
    QCRIL_LOG_DEBUG("sendResponseForGetSlicingConfig: serial=%d, error=%d", serial, errorCode);
    V1_6::SlicingConfig slicingConfig{};
    auto ret = respCb->getSlicingConfigResponse(responseInfo, slicingConfig);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForSetDataThrottling(int32_t serial, RIL_Errno errorCode) {
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    QCRIL_LOG_DEBUG("sendResponseForSetDataThrottling: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->setDataThrottlingResponse(responseInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }

  virtual void sendResponseForUpdateSimPhonebookRecords(
      int32_t serial, RIL_Errno errorCode, int32_t recordIndex) {
    auto respCb = this->getResponseCallback();
    if (!respCb) {
      return;
    }
    V1_6::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_6::RadioError>(errorCode) };
    QCRIL_LOG_DEBUG("updateSimPhonebookRecordsResponse: serial=%d, error=%d, recordId=%d",
            serial, errorCode, recordIndex);
    auto ret = respCb->updateSimPhonebookRecordsResponse(responseInfo, recordIndex);
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

  ::android::hardware::Return<void> setRadioPower_1_6(int32_t serial, bool powerOn,
                                                      bool forEmergencyCall,
                                                      bool preferredForEmergencyCall) {
    QCRIL_LOG_DEBUG(
        "setRadioPower_1_6: "
        "serial=%d,powerOn=%d,forEmergencyCall=%d,preferredForEmergencyCall=%d",
        serial, powerOn, forEmergencyCall, preferredForEmergencyCall);

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
            this->sendResponseForSetRadioPower_1_6(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetRadioPower_1_6(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> getDataCallList_1_6(int32_t serial) {
    QCRIL_LOG_DEBUG("getDataCallList_1_6: serial=%d", serial);
    using namespace rildata;
    QCRIL_LOG_DEBUG("RIL_REQUEST_DATA_CALL_LIST");
    auto msg = std::make_shared<GetRadioDataCallListRequestMessage>();
    if (msg != nullptr) {
      GenericCallback<DataCallListResult_t> cb(
          ([serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                          std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
              auto msg = std::make_shared<rildata::GetDataCallListResponseIndMessage>
                                  (responseDataPtr, serial, status);
              msg->broadcast();
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      auto msg = std::make_shared<rildata::GetDataCallListResponseIndMessage>
                          (nullptr, serial, Message::Callback::Status::SUCCESS);
      msg->broadcast();
    }
    return Void();
  }

  ::android::hardware::Return<void> setupDataCall_1_6(
      int32_t serial, ::android::hardware::radio::V1_5::AccessNetwork accessNetwork,
      const ::android::hardware::radio::V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
      ::android::hardware::radio::V1_2::DataRequestReason reason,
      const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>& addresses,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& dnses,
      int32_t pduSessionId, const ::android::hardware::radio::V1_6::OptionalSliceInfo& sliceInfo,
      const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& trafficDescriptor,
      bool matchAllRuleAllowed) {
    QCRIL_LOG_DEBUG("setupDataCall_1_6: serial=%d", serial);
    (void)pduSessionId;
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
      std::make_shared<SetupDataCallRequestMessage_1_6>(
        serial,
        RequestSource_t::RADIO,
        accnet,
        convertHidlDataProfileInfoToRil(dataProfileInfo),
        roamingAllowed,
        (DataRequestReason_t)reason,
        radioAddresses,
        radioDnses,
        matchAllRuleAllowed,
        nullptr);

    if (msg) {
      if(trafficDescriptor.getDiscriminator()
         == V1_6::OptionalTrafficDescriptor::hidl_discriminator::value) {
        TrafficDescriptor_t td;
        if(trafficDescriptor.value().dnn.getDiscriminator()
           == V1_6::OptionalDnn::hidl_discriminator::value) {
          td.dnn = trafficDescriptor.value().dnn.value();
        }
        if(trafficDescriptor.value().osAppId.getDiscriminator()
           == V1_6::OptionalOsAppId::hidl_discriminator::value) {
          std::vector<uint8_t> osAppId (trafficDescriptor.value().osAppId.value().osAppId.begin(),
                                        trafficDescriptor.value().osAppId.value().osAppId.end());
          td.osAppId = osAppId;
        }
        msg->setOptionalTrafficDescriptor(td);
      }

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
              QCRIL_LOG_ERROR("setupDataCall_1_6 resp is nullptr");
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return Void();
  }

  ::android::hardware::Return<void> sendSms_1_6(
      int32_t serial, const ::android::hardware::radio::V1_0::GsmSmsMessage& message) {
    QCRIL_LOG_DEBUG("sendSms_1_6: serial=%d", serial);
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
            this->sendResponseForSendSms_1_6(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendSms_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> sendSmsExpectMore_1_6(
      int32_t serial, const ::android::hardware::radio::V1_0::GsmSmsMessage& message) {
    QCRIL_LOG_DEBUG("sendSmsExpectMore_1_6: serial=%d", serial);
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
            this->sendResponseForSendSMSExpectMore_1_6(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendSMSExpectMore_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }

    return Void();
  }

  ::android::hardware::Return<void> sendCdmaSms_1_6(
      int32_t serial, const ::android::hardware::radio::V1_0::CdmaSmsMessage& sms) {
    QCRIL_LOG_DEBUG("sendCdmaSms_1_6: serial=%d", serial);
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
            this->sendResponseForSendCdmaSms_1_6(serial, errorCode, sendSmsResult);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendCdmaSms_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> sendCdmaSmsExpectMore_1_6(
      int32_t serial, const ::android::hardware::radio::V1_0::CdmaSmsMessage& sms) {
    QCRIL_LOG_DEBUG("sendCdmaSmsExpectMore_1_6: serial=%d", serial);
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
            std::shared_ptr<RilSendSmsResult_t> sendSmsResult = nullptr;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
            }
            this->sendResponseForSendCdmaSmsExpectMore_1_6(serial, errorCode, sendSmsResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSendCdmaSmsExpectMore_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> setSimCardPower_1_6(
      int32_t serial, ::android::hardware::radio::V1_1::CardPowerState powerUp) {
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
            this->sendResponseForSetSimCardPower_1_6(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetSimCardPower_1_6(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> setNrDualConnectivityState(
      int32_t serial,
      ::android::hardware::radio::V1_6::NrDualConnectivityState nrDualConnectivityState) {
    QCRIL_LOG_DEBUG("setNrDualConnectivityState: serial=%d", serial);
    // TODO: IRADIO_1_6: COMMON
    this->sendResponseForSetNrDualConnectivityState(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> isNrDualConnectivityEnabled(int32_t serial) {
    QCRIL_LOG_DEBUG("isNrDualConnectivityEnabled: serial=%d", serial);
    // TODO: IRADIO_1_6: COMMON
    this->sendResponseForIsNrDualConnectivityEnabled(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> allocatePduSessionId(int32_t serial) {
    QCRIL_LOG_DEBUG("allocatePduSessionId: serial=%d", serial);
    this->sendResponseForAllocatePduSessionId(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> releasePduSessionId(int32_t serial, int32_t id) {
    QCRIL_LOG_DEBUG("releasePduSessionId: serial=%d", serial);
    this->sendResponseForReleasePduSessionId(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> startHandover(int32_t serial, int32_t callId) {
    QCRIL_LOG_DEBUG("startHandover: serial=%d", serial);
    this->sendResponseForStartHandover(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> cancelHandover(int32_t serial, int32_t callId) {
    QCRIL_LOG_DEBUG("cancelHandover: serial=%d", serial);
    this->sendResponseForCancelHandover(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> setAllowedNetworkTypesBitmap(
      uint32_t serial,
      ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily>
          networkTypeBitmap) {
    QCRIL_LOG_DEBUG("setAllowedNetworkTypeBitmap: serial=%d", serial);
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
            this->sendResponseForSetAllowedNetworkTypesBitmap(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetAllowedNetworkTypesBitmap(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> getAllowedNetworkTypesBitmap(int32_t serial) {
    QCRIL_LOG_DEBUG("getAllowedNetworkTypeBitmap: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetPrefNetworkTypeMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              prefResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetModePrefResult_t>(resp->data);
            }
            this->sendResponseForGetAllowedNetworkTypesBitmap(serial, errorCode, prefResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetAllowedNetworkTypesBitmap(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> setDataThrottling(
      int32_t serial, ::android::hardware::radio::V1_6::DataThrottlingAction dataThrottlingAction,
      int64_t completionDurationMillis) {
    QCRIL_LOG_DEBUG("setDataThrottling: serial=%d", serial);

    using namespace rildata;
    using namespace utils;
    auto msg = std::make_shared<SetDataThrottlingRequestMessage>(
                                convertHidlDataThrottleActionToRil(dataThrottlingAction),
                                completionDurationMillis);

    if (msg != nullptr) {
      GenericCallback<RIL_Errno> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<RIL_Errno> resp) -> void {
            RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
              errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED;
            }
            else if (resp != nullptr && status == Message::Callback::Status::SUCCESS) {
              errorCode = *resp;
            }
            this->sendResponseForSetDataThrottling(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetDataThrottling(serial, RIL_E_NO_MEMORY);
    }

    return Void();
  }

  ::android::hardware::Return<void> emergencyDial_1_6(
      int32_t serial, const ::android::hardware::radio::V1_0::Dial& dialInfo,
      ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::EmergencyServiceCategory>
          categories,
      const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& urns,
      ::android::hardware::radio::V1_4::EmergencyCallRouting routing,
      bool hasKnownUserIntentEmergency, bool isTesting) {
    QCRIL_LOG_DEBUG("emergencyDial: serial=%d", serial);
    RIL_Errno result = RIL_E_SUCCESS;
    do {
      auto msg = std::make_shared<QcRilRequestDialMessage>(this->getContext(serial));
      if (msg == nullptr) {
        QCRIL_LOG_ERROR("msg is nullptr");
        result = RIL_E_NO_MEMORY;
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
          uusInfo->setType(utils::convertUssTypeFromHal(dialInfo.uusInfo[0].uusType));
          uusInfo->setDcs(utils::convertUusDcsTypeFromHal(dialInfo.uusInfo[0].uusDcs));
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
      msg->setIsIntentionEcc(hasKnownUserIntentEmergency);

      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForEmergencyDial(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } while (FALSE);
    if (result != RIL_E_SUCCESS) {
      this->sendResponseForEmergencyDial(serial, result);
    }
    return Void();
  }

  ::android::hardware::Return<void> getSystemSelectionChannels(int32_t serial) {
    QCRIL_LOG_DEBUG("getSystemSelectionChannels: serial=%d", serial);

    std::shared_ptr<RilRequestGetSysSelChannelsMessage> msg{};
    RIL_Errno res = RIL_E_SUCCESS;

    msg = std::make_shared<RilRequestGetSysSelChannelsMessage>(this->getContext(serial));
    if (msg != nullptr) {
      res = RIL_E_NO_MEMORY;
    }

    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<RilGetSysSelResult_t> result {};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
               result = std::static_pointer_cast<RilGetSysSelResult_t>(resp->data);
            }
            this->sendResponseForGetSystemSelectionChannels(serial, errorCode, result);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetSystemSelectionChannels(serial, res, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getCellInfoList_1_6(int32_t serial) {
    QCRIL_LOG_DEBUG("getCellInfoList_1_6: serial=%d", serial);
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
            this->sendResponseForGetCellInfoList_1_6(serial, errorCode, cellInfoListResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetCellInfoList_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getVoiceRegistrationState_1_6(int32_t serial) {
    QCRIL_LOG_DEBUG("getVoiceRegistrationState_1_6: serial=%d", serial);
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
            this->sendResponseForGetVoiceRegistrationState_1_6(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetVoiceRegistrationState_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getDataRegistrationState_1_6(int32_t serial) {
    QCRIL_LOG_DEBUG("getDataRegistrationState_1_6: serial=%d", serial);
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
            this->sendResponseForGetDataRegistrationState_1_6(serial, errorCode, rilRegResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetDataRegistrationState_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getSignalStrength_1_6(int32_t serial) {
    QCRIL_LOG_DEBUG("getSignalStrength_1_6: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetSignalStrengthMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilSigResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetSignalStrengthResult_t>(
                      resp->data);
            } else if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
              errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
            }
            this->sendResponseForgetSignalStrength_1_6(serial, errorCode, rilSigResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForgetSignalStrength_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getCurrentCalls_1_6(int32_t serial) {
    QCRIL_LOG_DEBUG("getCurrentCalls_1_6: serial=%d", serial);
    auto msg = std::make_shared<QcRilRequestGetCurrentCallsMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /* msg */, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::GetCurrentCallsRespData> currentCalls;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              currentCalls =
                  std::static_pointer_cast<qcril::interfaces::GetCurrentCallsRespData>(resp->data);
            }
            this->sendResponseForGetCurrentCalls_1_6(serial, errorCode, currentCalls);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetCurrentCalls_1_6(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  ::android::hardware::Return<void> getSlicingConfig(int32_t serial) {
    QCRIL_LOG_DEBUG("getSlicingConfig: serial=%d", serial);
    this->sendResponseForGetSlicingConfig(serial, RIL_E_REQUEST_NOT_SUPPORTED);
    return Void();
  }

  ::android::hardware::Return<void> getSimPhonebookCapacity(int32_t serial) {
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
              adnCount = std::static_pointer_cast<qcril::interfaces::AdnCountInfoResp>(
                      resp->data)
                      ->getAdnCountInfo();
            }
            std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adn_count_info =
                    make_shared<qcril::interfaces::qcril_pbm_adn_count_info>(adnCount);
            this->sendResponseForGetSimPhonebookCapacity(serial, errorCode, adn_count_info);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      std::shared_ptr<qcril::interfaces::qcril_pbm_adn_count_info> adn_count_info;
      this->sendResponseForGetSimPhonebookCapacity(serial, RIL_E_NO_MEMORY, adn_count_info);
    }
    return Void();
  }

  ::android::hardware::Return<void> getSimPhonebookRecords(int32_t serial) {
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
    return Void();
  }

  ::android::hardware::Return<void> updateSimPhonebookRecords(int32_t serial,
       const ::android::hardware::radio::V1_6::PhonebookRecordInfo& pbRecords) {
    QCRIL_LOG_DEBUG("updateSimPhonebookRecords: serial=%d", serial);
    qcril::interfaces::AdnRecordInfo records;
    utils::convertHidlPhonebookRecords(pbRecords, records);
    auto msg = std::make_shared<QcRilRequestUpdatePhonebookRecordsMessage>
        (this->getContext(serial), records);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (msg && resp) {
              if (resp->data) {
                  int32_t recordIndex = std::static_pointer_cast<qcril::interfaces::AdnRecordUpdatedResp>(
                          resp->data)->getRecordIndex();
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
    return Void();
  }

#if 0
  /**
   * Notifies cellInfoList indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_0::IRadioIndication::cellInfoList
   *   V1_2::IRadioIndication::cellInfoList_1_2
   *   V1_4::IRadioIndication::cellInfoList_1_4
   *   V1_5::IRadioIndication::cellInfoList_1_5
   *   V1_6::IRadioIndication::cellInfoList_1_6
   */
  int sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_5::implementation::RadioServiceImpl<T>::sendCellInfoList(msg);
    }
    ::android::hardware::hidl_vec<V1_6::CellInfo> records{};
    // TODO: IRADIO_1_6: COMMON
    // utils::convertRilCellInfoList(msg->getCellInfoList(), records);
    QCRIL_LOG_DEBUG("UNSOL: cellInfoList_1_6");
    auto ret = ind->cellInfoList_1_6(V1_0::RadioIndicationType::UNSOLICITED, records);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }
#endif

  int sendLinkCapInd(std::shared_ptr<rildata::LinkCapIndMessage> msg) {
    // use currentLinkCapacityEstimate along with modemReducedFeatureSet as true
    return V1_2::implementation::RadioServiceImpl<T>::sendLinkCapInd(msg);
  }

  /**
   * Notifies unthrottleApn Indication
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_6::IRadioIndication::unthrottleApn
   */
  int sendUnthrottleApn(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    if (msg->getSrc() != rildata::RequestSource_t::RADIO) {
      QCRIL_LOG_ERROR("request is for IWLAN");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return 1;
    }

    QCRIL_LOG_DEBUG("UNSOL: unthrottleApn");
    auto ret = ind->unthrottleApn(V1_0::RadioIndicationType::UNSOLICITED, msg->getApn());
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  ::android::hardware::Return<void> setCarrierInfoForImsiEncryption_1_6(int32_t serial,
        const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& imsiEncryptionInfo) {
    QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
    using namespace rildata;
    using namespace V1_0;
    ImsiEncryptionInfo_t imsiData = {};
    imsiData.mcc = imsiEncryptionInfo.base.mcc;
    imsiData.mnc = imsiEncryptionInfo.base.mnc;
    for (int i = 0; i < imsiEncryptionInfo.base.carrierKey.size(); i++) {
        imsiData.carrierKey.push_back(imsiEncryptionInfo.base.carrierKey[i]);
    }
    imsiData.keyIdentifier = imsiEncryptionInfo.base.keyIdentifier;
    imsiData.expiryTime = imsiEncryptionInfo.base.expirationTime;
    imsiData.keyType = (rildata::PublicKeyType_t)imsiEncryptionInfo.keyType;
    auto msg = std::make_shared<rildata::SetCarrierInfoImsiEncryptionMessage>(imsiData);
    if (msg) {
      GenericCallback<RIL_Errno> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<RIL_Errno> resp) -> void {
        V1_0::RadioResponseInfo responseInfo {.serial = serial,
                                              .error = V1_0::RadioError::GENERIC_FAILURE};
        if (resp) {
          V1_0::RadioError errorCode = V1_0::RadioError::GENERIC_FAILURE;
          if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
            errorCode = V1_0::RadioError::REQUEST_NOT_SUPPORTED;
          }
          else if (status == Message::Callback::Status::SUCCESS) {
            errorCode = static_cast<V1_0::RadioError>(*resp);
          }
          responseInfo = {.serial = serial, .error = errorCode};
          QCRIL_LOG_DEBUG("setCarrierInfoForImsiEncryption cb invoked status %d respErr %d",
                          status, *resp);
        }
        this->sendCarrierInfoForImsiEncryptionResponse(responseInfo);
      });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      QCRIL_LOG_ERROR("Unable to create msg SetCarrierInfoImsiEncryptionMessage");
      V1_0::RadioResponseInfo rsp {V1_0::RadioResponseType::SOLICITED,
                                   serial, V1_0::RadioError::NO_MEMORY};
      this->sendCarrierInfoForImsiEncryptionResponse(rsp);
    }
    return Void();
  }

  /**
   * Notifies networkScanResult indication.
   * V1_6::IRadioIndication::currentSignalStrength_1_6
   */
  int sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> msg) override {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return V1_4::implementation::RadioServiceImpl<T>::sendSignalStrength(msg);
    }
    V1_6::SignalStrength signalStrength{};
    utils::convertRilSignalStrengthToHal(msg->getSignalStrength(), signalStrength);
    QCRIL_LOG_DEBUG("UNSOL: currentSignalStrength_1_6");
    auto ret =
        ind->currentSignalStrength_1_6(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, signalStrength);
    if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        }
    return 0;
  }

  /**
   * Notifies simPhonebookChanged indication.
   * The implementation will invoke the latest version of the below the indication APIs based on
   * the version of the indication callback object set by the client.
   *   V1_6::IRadioIndication::simPhonebookChanged
   */
  int sendSimPhonebookChanged(std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg) override {
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
    auto ret = ind->simPhonebookChanged(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  int sendSimPhonebookRecords(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg) override {
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
    ::android::hardware::hidl_vec<V1_6::PhonebookRecordInfo> rilRecords;
    V1_6::PbReceivedStatus status = utils::convertRilPbReceivedStatus(msg->getSeqNum());
    utils::convertRilPhonebookRecords(msg->getAdnRecords(), rilRecords);
    QCRIL_LOG_INFO("UNSOL: simPhonebookRecordsReceived");
    auto ret = ind->simPhonebookRecordsReceived(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP,
            status,
            rilRecords);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_6::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_6
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_6_H__
