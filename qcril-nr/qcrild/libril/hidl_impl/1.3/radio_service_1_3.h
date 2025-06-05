/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
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

#ifndef __RADIO_SERVICE_1_3_H__
#define __RADIO_SERVICE_1_3_H__

#include "android/hardware/radio/1.3/IRadio.h"
#include "android/hardware/radio/1.3/IRadioResponse.h"
#include "android/hardware/radio/1.3/IRadioIndication.h"

#include "interfaces/nas/nas_types.h"
#include "interfaces/nas/RilRequestEnableModemMessage.h"
#include "interfaces/nas/RilRequestGetModemStackStatusMessage.h"
#include "interfaces/nas/RilRequestSetSysSelChannelsMessage.h"

#include "telephony/ril.h"
#include "hidl_impl/radio_service_base.h"
#include "hidl_impl/1.2/radio_service_1_2.h"
#include "hidl_impl/1.3/radio_service_utils_1_3.h"
#include "HalServiceImplFactory.h"

#undef TAG
#define TAG "RILQ"

namespace android {

extern void grabPartialWakeLock();

namespace hardware {
namespace radio {
namespace V1_3 {
namespace implementation {

// V1_3::IRadio implementation
template <typename T>
class RadioServiceImpl : public V1_2::implementation::RadioServiceImpl<T> {
 private:
  ::android::sp<android::hardware::radio::V1_3::IRadioResponse> mRadioResponse;
  ::android::sp<android::hardware::radio::V1_3::IRadioIndication> mRadioIndication;

  ::android::sp<android::hardware::radio::V1_3::IRadioResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioResponse;
  }

  ::android::sp<android::hardware::radio::V1_3::IRadioIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    return mRadioIndication;
  }

 protected:
  void clearCallbacks_nolock() {
    QCRIL_LOG_DEBUG("V1_3::clearCallbacks_nolock");
    mRadioResponse = nullptr;
    mRadioIndication = nullptr;
    V1_2::implementation::RadioServiceImpl<T>::clearCallbacks_nolock();
  }
  virtual void clearCallbacks() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(this->mCallbackLock);
    clearCallbacks_nolock();
  }
  void setResponseFunctions_nolock(
      const ::android::sp<android::hardware::radio::V1_0::IRadioResponse>& respCb,
      const ::android::sp<android::hardware::radio::V1_0::IRadioIndication>& indCb) {
    QCRIL_LOG_DEBUG("V1_3::setResponseFunctions_nolock");
    mRadioResponse = V1_3::IRadioResponse::castFrom(respCb).withDefault(nullptr);
    mRadioIndication = V1_3::IRadioIndication::castFrom(indCb).withDefault(nullptr);
    V1_2::implementation::RadioServiceImpl<T>::setResponseFunctions_nolock(respCb, indCb);
  }

  virtual void sendResponseForEnableModem(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("enableModemResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->enableModemResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForGetModemStackStatus(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetModemStackResult_t> rilModemStackResult) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    bool on{ false };
    if (errorCode == RIL_E_SUCCESS && rilModemStackResult) {
      on = rilModemStackResult->state;
    }
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("getModemStackStatusResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->getModemStackStatusResponse(responseInfo, on);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }

  virtual void sendResponseForSetSystemSelectionChannels(int32_t serial, RIL_Errno errorCode) {
    V1_0::RadioResponseInfo responseInfo{ V1_0::RadioResponseType::SOLICITED, serial,
                                          static_cast<V1_0::RadioError>(errorCode) };
    auto respCb = this->getResponseCallback();
    if (respCb) {
      QCRIL_LOG_DEBUG("setSystemSelectionChannelsResponse: serial=%d, error=%d", serial, errorCode);
      auto ret = respCb->setSystemSelectionChannelsResponse(responseInfo);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
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

  Return<void> enableModem(int32_t serial, bool on) {
    QCRIL_LOG_DEBUG("enableModem: serial=%d on=%d", serial, on);
    auto msg = std::make_shared<RilRequestEnableModemMessage>(this->getContext(serial), on);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            this->sendResponseForEnableModem(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForEnableModem(serial, RIL_E_NO_MEMORY);
    }
    return Void();
  }

  Return<void> getModemStackStatus(int32_t serial) {
    QCRIL_LOG_DEBUG("getModemStackStatus: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetModemStackStatusMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::RilGetModemStackResult_t> rilModemStackResult{};
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
              rilModemStackResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetModemStackResult_t>(resp->data);
            }
            this->sendResponseForGetModemStackStatus(serial, errorCode, rilModemStackResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetModemStackStatus(serial, RIL_E_NO_MEMORY, nullptr);
    }
    return Void();
  }

  Return<void> setSystemSelectionChannels(int32_t serial, bool specifyChannels,
                                          const hidl_vec<V1_1::RadioAccessSpecifier>& specifiers) {
    QCRIL_LOG_DEBUG("setSystemSelectionChannels: serial=%d, channel info valid = %d", serial,
                    specifyChannels);
    std::shared_ptr<RilRequestSetSysSelChannelsMessage> msg{};
    RIL_SysSelChannels ril_info{};
    memset(&ril_info, 0, sizeof(ril_info));

    RIL_Errno res = RIL_E_SUCCESS;

    // If specifyChannels is true scan specific bands otherwise scan all bands
    // If specifier length is zero, RIL should scan all bands.
    if (specifyChannels == true) {
      res = utils::fillSetSystemSelectionChannelRequest(specifiers, ril_info);
    }

    if (res == RIL_E_SUCCESS) {
      msg = std::make_shared<RilRequestSetSysSelChannelsMessage>(this->getContext(serial), ril_info);
      if (msg != nullptr) {
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
            this->sendResponseForSetSystemSelectionChannels(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForSetSystemSelectionChannels(serial, res);
    }
    return Void();
  }
};

template <>
const HalServiceImplVersion& RadioServiceImpl<V1_3::IRadio>::getVersion();

}  // namespace implementation
}  // namespace V1_3
}  // namespace radio
}  // namespace hardware
}  // namespace android

#endif  // __RADIO_SERVICE_1_3_H__
