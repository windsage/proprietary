/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
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

#ifndef __RADIO_SERVICE_1_1_H__
#define __RADIO_SERVICE_1_1_H__

#include "android/hardware/radio/1.1/IRadio.h"
#include "android/hardware/radio/1.1/IRadioResponse.h"
#include "android/hardware/radio/1.1/IRadioIndication.h"
#include "android/hardware/radio/1.1/types.h"

#include "interfaces/nas/RilRequestStopNetworkScanMessage.h"
#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/uim/UimCardPowerReqMsg.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.0/radio_service_1_0.h"
#include "hidl_impl/1.1/radio_service_utils_1_1.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {

extern void grabPartialWakeLock();

namespace hardware {
namespace radio {
namespace V1_1 {
namespace implementation {

// V1_1::IRadio implementation
template <typename T>
class RadioServiceImpl : public V1_0::implementation::RadioServiceImpl<T> {
 private:
  ::android::sp<android::hardware::radio::V1_1::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_1::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_1::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_1::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_1::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
    V1_0::implementation::RadioServiceImpl<T>::clearCallbacks_nolock();
  }

  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }

  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_1::setResponseFunctions_nolock");
    mRadioResponse = V1_1::IRadioResponse::castFrom(respCb).withDefault(nullptr);
    mRadioIndication = V1_1::IRadioIndication::castFrom(indCb).withDefault(nullptr);
    V1_0::implementation::RadioServiceImpl<T>::setResponseFunctions_nolock(respCb, indCb);
  }

  virtual void sendResponseForStartNetworkScan(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("startNetworkScanResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->startNetworkScanResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForStopNetworkScan(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("stopNetworkScanResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->stopNetworkScanResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendStartKeepAliveResponse(V1_0::RadioResponseInfo responseInfo, V1_1::KeepaliveStatus result) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending StartKeepAliveRequestMessage V1_1 Response");
      auto ret = respCb->startKeepaliveResponse(responseInfo, result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void sendCarrierInfoForImsiEncryptionResponse(V1_0::RadioResponseInfo responseInfo) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending sendCarrierInfoForImsiEncryptionResponse V1_1 Response");
      auto ret = respCb->setCarrierInfoForImsiEncryptionResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void sendStopKeepAliveResponse(V1_0::RadioResponseInfo responseInfo) {
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("Sending StopKeepAliveRequestMessage V1_1 Response");
      auto ret = respCb->stopKeepaliveResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    } else {
        QCRIL_LOG_ERROR("Response Callback is Nullptr");
    }
  }

  virtual void sendResponseForSetSimCardPower_1_1(int32_t serial, V1_0::RadioError errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial, errorCode };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setSimCardPowerResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setSimCardPowerResponse_1_1(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

 public:
  RadioServiceImpl() = default;

  virtual ~RadioServiceImpl() = default;

  static const HalServiceImplVersion& getVersion();

  ::android::hardware::Return<void> setResponseFunctions(
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

  ::android::hardware::Return<void> setCarrierInfoForImsiEncryption(
      int32_t serial,
      const ::android::hardware::radio::V1_1::ImsiEncryptionInfo& imsiEncryptionInfo) {
    QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
    using namespace rildata;
    using namespace V1_0;
    ImsiEncryptionInfo_t imsiData = {};
    imsiData.mcc = imsiEncryptionInfo.mcc;
    imsiData.mnc = imsiEncryptionInfo.mnc;
    for (int i = 0; i < imsiEncryptionInfo.carrierKey.size(); i++) {
        imsiData.carrierKey.push_back(imsiEncryptionInfo.carrierKey[i]);
    }
    imsiData.keyIdentifier = imsiEncryptionInfo.keyIdentifier;
    imsiData.expiryTime = imsiEncryptionInfo.expirationTime;
    auto msg = std::make_shared<rildata::SetCarrierInfoImsiEncryptionMessage>(imsiData);
    if (msg) {
        GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> msg,
                                                         Message::Callback::Status status,
                                                         std::shared_ptr<RIL_Errno> rsp) -> void {
            RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::REQUEST_NOT_SUPPORTED};
            if (rsp) {
                RadioError e = RadioError::NONE;
                if ((status != Message::Callback::Status::SUCCESS) ||
                   (*rsp != RIL_E_SUCCESS)) {
                        e = static_cast<RadioError>(*rsp);
                }
                responseInfo = {.serial = serial, .error = e};
                QCRIL_LOG_DEBUG("CarrierInfoForImsiEncryption cb invoked status %d respErr %d", status, *rsp);
            }
            this->sendCarrierInfoForImsiEncryptionResponse(responseInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        QCRIL_LOG_DEBUG("startKeepAlivecb responding NO_MEMORY");
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        this->sendCarrierInfoForImsiEncryptionResponse(rsp);
    }
    return Void();
  }

  ::android::hardware::Return<void> setSimCardPower_1_1(
      int32_t serial, ::android::hardware::radio::V1_1::CardPowerState powerUp) {
    QCRIL_LOG_DEBUG("setSimCardPower_1_1: serial=%d, powerUp=%d", serial, powerUp);
    auto msg = std::make_shared<UimCardPowerReqMsg>(this->getInstanceId(),
                                                    static_cast<RIL_UIM_CardPowerState>(powerUp));
    if (msg != nullptr) {
      GenericCallback<RIL_UIM_Errno> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_Errno> responseDataPtr) -> void {
            V1_0::RadioError errorCode{ V1_0::RadioError::INTERNAL_ERR };
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr != nullptr) {
              errorCode = static_cast<V1_0::RadioError>(*responseDataPtr);
            }
            this->sendResponseForSetSimCardPower_1_1(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetSimCardPower_1_1(serial, V1_0::RadioError::NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> startNetworkScan(
      int32_t serial, const ::android::hardware::radio::V1_1::NetworkScanRequest& request) {
    QCRIL_LOG_DEBUG("startNetworkScan: serial=%d", serial);
    this->mIsScanRequested = true;
    std::shared_ptr<RilRequestStartNetworkScanMessage> msg{};
    RIL_NetworkScanRequest scanRequest{};
    auto errResp = utils::fillNetworkScanRequest_1_1(request, scanRequest);
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

  ::android::hardware::Return<void> stopNetworkScan(int32_t serial) {
    QCRIL_LOG_DEBUG("stopNetworkScan: serial=%d", serial);
    auto msg = std::make_shared<RilRequestStopNetworkScanMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->mIsScanRequested = false;
            this->sendResponseForStopNetworkScan(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForStopNetworkScan(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  ::android::hardware::Return<void> startKeepalive(
      int32_t serial, const ::android::hardware::radio::V1_1::KeepaliveRequest& keepalive) {
    QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
    using namespace rildata;
    using namespace V1_0;
    switch((KeepaliveType_t)keepalive.type) {
        case KeepaliveType_t::NATT_IPV4:
            if (keepalive.sourceAddress.size() != 4 ||
                    keepalive.destinationAddress.size() != 4) {
                QCRIL_LOG_DEBUG("Invalid address for keepalive!");
                RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::INVALID_ARGUMENTS};
                ::android::hardware::radio::V1_1::KeepaliveStatus ka_status = {};
                this->sendStartKeepAliveResponse(responseInfo, ka_status);
                return Void();
            }
            break;
        case KeepaliveType_t::NATT_IPV6:
            if (keepalive.sourceAddress.size() != 16 ||
                    keepalive.destinationAddress.size() != 16) {
                QCRIL_LOG_DEBUG("Invalid address for keepalive!");
                RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::INVALID_ARGUMENTS};
                ::android::hardware::radio::V1_1::KeepaliveStatus ka_status = {};
                this->sendStartKeepAliveResponse(responseInfo, ka_status);
                return Void();
            }
            break;
        default:
            QCRIL_LOG_DEBUG("Unknown packet keepalive type!");
            RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::INVALID_ARGUMENTS};
            ::android::hardware::radio::V1_1::KeepaliveStatus ka_status = {};
            this->sendStartKeepAliveResponse(responseInfo, ka_status);
            return Void();
    }

    KeepaliveRequest_t req;
    req.type = (KeepaliveType_t)keepalive.type;
    req.sourceAddress = keepalive.sourceAddress;
    req.sourcePort = keepalive.sourcePort;
    req.destinationAddress = keepalive.destinationAddress;
    req.destinationPort = keepalive.destinationPort;
    req.maxKeepaliveIntervalMillis = keepalive.maxKeepaliveIntervalMillis;
    req.cid = keepalive.cid;
    auto msg = std::make_shared<StartKeepAliveRequestMessage>(serial, req);

    QCRIL_LOG_DEBUG("KeepAlive type %d", req.type);
    QCRIL_LOG_DEBUG("KeepAlive sourceAddress %s", ::android::hardware::toString(keepalive.sourceAddress).c_str());
    QCRIL_LOG_DEBUG("KeepAlive sourcePort %d", req.sourcePort);
    QCRIL_LOG_DEBUG("KeepAlive destinationAddress %s", ::android::hardware::toString(keepalive.destinationAddress).c_str());
    QCRIL_LOG_DEBUG("KeepAlive destinationPort %d", req.destinationPort);
    QCRIL_LOG_DEBUG("KeepAlive maxKeepaliveIntervalMillis %d", req.maxKeepaliveIntervalMillis);
    QCRIL_LOG_DEBUG("KeepAlive cid %d", req.cid);

    if (msg) {
        GenericCallback<StartKeepAliveResp_t> cb([this, serial](std::shared_ptr<Message> msg,
                                                         Message::Callback::Status status,
                                                         std::shared_ptr<StartKeepAliveResp_t> rsp) -> void {
            RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::REQUEST_NOT_SUPPORTED};
            ::android::hardware::radio::V1_1::KeepaliveStatus ka_status = {};
            if (msg && rsp) {
                QCRIL_LOG_DEBUG("startKeepAlivecb msg & rsp not null");
                RadioError e = RadioError::NONE;
                if ((status != Message::Callback::Status::SUCCESS) ||
                   (rsp->error != ResponseError_t::NO_ERROR)) {
                    switch(rsp->error) {
                        case ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
                        default: e = RadioError::NO_RESOURCES; break;
                    }
                }
                responseInfo = {.serial = serial, .error = e};
                QCRIL_LOG_DEBUG("startKeepAlivecb cb invoked status %d respErr %d", status, rsp->error);
                ka_status.sessionHandle = rsp->handle;
                ka_status.code = static_cast<V1_1::KeepaliveStatusCode>(rsp->status);
            }
            QCRIL_LOG_DEBUG("startKeepAlivecb here");
            this->sendStartKeepAliveResponse(responseInfo, ka_status);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else  {
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        ::android::hardware::radio::V1_1::KeepaliveStatus ka_status = {};
        QCRIL_LOG_DEBUG("startKeepAlivecb NO_MEMORY");
        this->sendStartKeepAliveResponse(rsp, ka_status);
    }
    return Void();
  }
  ::android::hardware::Return<void> stopKeepalive(int32_t serial, int32_t sessionHandle) {
    QCRIL_LOG_DEBUG("%s(): %d", __FUNCTION__, serial);
    using namespace rildata;
    using namespace V1_0;
    auto msg =
        std::make_shared<StopKeepAliveRequestMessage>(
            serial,
            sessionHandle);

    if (msg) {
        GenericCallback<ResponseError_t> cb([this, serial](std::shared_ptr<Message> msg,
                                                         Message::Callback::Status status,
                                                         std::shared_ptr<ResponseError_t> rsp) -> void {
            RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::REQUEST_NOT_SUPPORTED};
            if (msg && rsp) {
                RadioError e = RadioError::NONE;
                if ((status != Message::Callback::Status::SUCCESS) ||
                   (*rsp != ResponseError_t::NO_ERROR)) {
                        e = RadioError::INVALID_ARGUMENTS;
                }
                responseInfo = {.serial = serial, .error = e};
                QCRIL_LOG_DEBUG("stopKeepAlivecb cb invoked status %d respErr %d", status, *rsp);
            }
            QCRIL_LOG_DEBUG("stopKeepAlivecb sending resp");
            this->sendStopKeepAliveResponse(responseInfo);
        });
        msg->setCallback(&cb);
        msg->dispatch();
    } else {
        QCRIL_LOG_DEBUG("startKeepAlivecb responding NO_MEMORY");
        RadioResponseInfo rsp{RadioResponseType::SOLICITED, serial, RadioError::NO_MEMORY};
        this->sendStopKeepAliveResponse(rsp);
    }
    return Void();
  }

  /**
   * Notifies networkScanResult indication.
   * V1_1::IRadioIndication API
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
      return V1_0::implementation::RadioServiceImpl<T>::sendNetworkScanResult(msg);
    }
    V1_1::NetworkScanResult result{};
    result.status = static_cast<V1_1::ScanStatus>(msg->getStatus());
    result.error = static_cast<V1_0::RadioError>(msg->getError());

    utils::convertRilCellInfoListToHal(msg->getNetworkInfo(), result.networkInfos);
    QCRIL_LOG_DEBUG("UNSOL: networkScanResult");
    Return<void> ret = ind->networkScanResult(V1_0::RadioIndicationType::UNSOLICITED_ACK_EXP, result);
    if(result.status == ::android::hardware::radio::V1_1::ScanStatus::COMPLETE){
      this->mIsScanRequested = false;
    }
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
    return 0;
  }

  int sendCarrierInfoForImsiEncryptionInd(std::shared_ptr<Message> msg) {
    QCRIL_LOG_DEBUG("%s(): V1_1", __FUNCTION__);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind) {
      Return<void> ret = ind->carrierInfoForImsiEncryption(V1_0::RadioIndicationType::UNSOLICITED);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        return 1;
      }
    }
    else {
      QCRIL_LOG_ERROR("Ind cb is nullptr");
      return 1;
    }
    return 0;
  }

  int sendKeepAliveStatusInd(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg) {
    QCRIL_LOG_DEBUG("%s(): V1_1", __FUNCTION__);
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return 1;
    }
    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (ind) {
      V1_1::KeepaliveStatus status = {};
      status.sessionHandle = msg->getHandle();
      status.code = (::android::hardware::radio::V1_1::KeepaliveStatusCode)(msg->getStatusCode());
      Return<void> ret = ind->keepaliveStatus(V1_0::RadioIndicationType::UNSOLICITED, status);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
        return 1;
      }
    }
    else {
      QCRIL_LOG_ERROR("Ind cb is nullptr");
      return 1;
    }
    return 0;
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_1::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_1
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_1_H__
