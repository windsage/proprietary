/******************************************************************************
#  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qti_radio_stable_aidl_service.h"
#include <framework/Log.h>
#include <cutils/properties.h>
#include "qcril_other.h"
#include "qcril_legacy_apis.h"
#include <inttypes.h>

#ifndef QMI_RIL_UTF
#include "request/SetNrUCIconConfigureMessage.h"
#endif

#include "interfaces/ims/QcRilRequestImsQueryCIWLANConfigSyncMessage.h"
#include "interfaces/nas/RilRequestEnableEndcMessage.h"
#include "interfaces/nas/RilRequestQueryEndcStatusMessage.h"
#include "interfaces/nas/RilRequestSetNrDisableModeMessage.h"
#include "interfaces/nas/RilRequestQueryNrDisableModeMessage.h"
#include "interfaces/nas/RilRequestGetEnhancedRadioCapabilityMessage.h"
#include "interfaces/voice/QcRilRequestQueryCallForwardMessage.h"
#include "interfaces/voice/QcRilRequestGetCallBarringMessage.h"
#include "interfaces/dms/RilRequestGetImeiMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilRequestStopNetworkScanMessage.h"
#include "interfaces/nas/RilRequestQueryNetworkSelectModeMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h"
#include "interfaces/uim/UimSimPerSoUnlockStatusInd.h"
#include "interfaces/uim/UimGetPersoLockStatusRequestMsg.h"
#include "request/GetDataNrIconTypeMessage.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace implementation {

void QtiRadioStable::setInstanceId(qcril_instance_id_e_type instId) {
  mInstanceId = instId;
}
qcril_instance_id_e_type QtiRadioStable::getInstanceId() {
  return mInstanceId;
}

std::shared_ptr<QtiRadioContext> QtiRadioStable::getContext(uint32_t serial) {
  return std::make_shared<QtiRadioContext>(mInstanceId, serial);
}

std::shared_ptr<aidlimports::IQtiRadioResponse> QtiRadioStable::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mResponseCb;
}

std::shared_ptr<aidlimports::IQtiRadioIndication> QtiRadioStable::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

void QtiRadioStable::clearCallbacks_nolock() {
  QCRIL_LOG_DEBUG("clearCallbacks_nolock");
  mIndicationCb = nullptr;
  mResponseCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

void QtiRadioStable::clearCallbacks() {
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  clearCallbacks_nolock();
}

void QtiRadioStable::deathNotifier(void* /*cookie*/) {
  QCRIL_LOG_DEBUG("QtiRadioStable::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  QtiRadioStable* qtiRadioStable = static_cast<QtiRadioStable*>(cookie);
  if (qtiRadioStable != nullptr) {
    qtiRadioStable->deathNotifier(cookie);
  }
}

void QtiRadioStable::setCallback_nolock(
    const std::shared_ptr<aidlimports::IQtiRadioResponse>& in_responseCallback,
    const std::shared_ptr<aidlimports::IQtiRadioIndication>& in_indicationCallback) {
  QCRIL_LOG_DEBUG("QtiRadioStable::setCallback_nolock");
  mResponseCb = in_responseCallback;
  mIndicationCb = in_indicationCallback;
}

QtiRadioStable::EpdgSupport QtiRadioStable::getEpdgSupport() {
  return mEpdgSupport;
}

void QtiRadioStable::setEpdgSupport(bool support) {
  if (support) {
    mEpdgSupport = EpdgSupport::ENABLE;
  } else {
    mEpdgSupport = EpdgSupport::DISABLE;
  }
}

::ndk::ScopedAStatus QtiRadioStable::setCallbacks(
    const std::shared_ptr<aidlimports::IQtiRadioResponse>& in_responseCallback,
    const std::shared_ptr<aidlimports::IQtiRadioIndication>& in_indicationCallback) {
  QCRIL_LOG_DEBUG("QtiRadioStable::setCallbacks");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
#ifndef QMI_RIL_UTF
  if (mResponseCb != nullptr) {
    AIBinder_unlinkToDeath(mResponseCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
  }
#endif

  setCallback_nolock(in_responseCallback, in_indicationCallback);

#ifndef QMI_RIL_UTF
  if (mResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(mResponseCb->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }
#endif

  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::notifyOnNrIconTypeChange(NrIconType iconType) {
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("notifyOnNrIconTypeChange: iconType=%d", iconType);
    auto ret = indCb->onNrIconTypeChange(iconType);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyOnImeiChange(aidlimports::ImeiInfo info) {
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("onImeiChange: imeiType=%d", info.type);
    auto ret = indCb->onImeiChange(info);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyOnPdcRefresh(std::shared_ptr<QcRilUnsolPdcRefreshMessage> msg) {
  auto indCb = getIndicationCallback();
  QCRIL_LOG_DEBUG("notifyOnPdcRefresh ");
  if (indCb) {
    aidlimports::McfgRefreshState  state =
        aidl::vendor::qti::hardware::radio::qtiradio::utils::
            convertPdcRefreshToAidlMcfgRefresh(msg->getEvent());
    QCRIL_LOG_DEBUG("notifyOnPdcRefresh: event=%d subscription_id", state,
        msg->getSubscriptionId());
    auto ret = indCb->onMcfgRefresh(state, msg->getSubscriptionId());
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyDdsSwitchCapabilityChange() {
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("notifyDdsSwitchCapabilityChange");
    auto ret = indCb->onDdsSwitchCapabilityChange();
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyDdsSwitchCriteriaChange(bool telephonyDdsSwitch) {
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("notifyDdsSwitchCriteriaChange: telephonyDdsSwitch:%d", telephonyDdsSwitch);
    auto ret = indCb->onDdsSwitchCriteriaChange(telephonyDdsSwitch);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyDdsSwitchRecommendation(int32_t recommendedSlotId) {
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("notifyDdsSwitchRecommendation: recommendedSlotId=%d", recommendedSlotId);
    auto ret = indCb->onDdsSwitchRecommendation(recommendedSlotId);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyConfigureDeactivateDelayTime(int64_t delayTime) {
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("notifyConfigureDeactivateDelayTime: delayTime:%" PRId64, delayTime);
    auto ret = indCb->onDataDeactivateDelayTime(delayTime);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::notifyDataCapabilityChange(bool change) {
  setEpdgSupport(change);
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("notifyDataCapabilityChange: change:%d", change);
    auto ret = indCb->onEpdgOverCellularDataSupported(change);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForQueryNrIconType(int32_t serial, RIL_Errno errorCode,
                                                    NrIconType iconType) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("onNrIconTypeResponse: serial=%d, errorCode=%d, iconType=%d", serial, errorCode,
                    iconType);
    auto ret = respCb->onNrIconTypeResponse(serial, static_cast<uint32_t>(errorCode), iconType);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForEnableEndc(int32_t serial, RIL_Errno errorCode,
                                               aidlimports::Status status) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("onEnableEndcResponse: serial=%d", serial);
    auto ret = respCb->onEnableEndcResponse(serial, static_cast<uint32_t>(errorCode), status);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForQueryEndcStatus(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilQueryEndcStatusResult_t> payload) {
  EndcStatus endcStatus = EndcStatus::INVALID;
  if (errorCode == RIL_E_SUCCESS && payload != nullptr) {
    endcStatus = utils::convert_endc_status(payload);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("onEndcStatusResponse: serial=%d errorCode=%d endcStatus=%d", serial, errorCode,
                    endcStatus);
    auto ret = respCb->onEndcStatusResponse(serial, static_cast<uint32_t>(errorCode), endcStatus);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForSetNrDisableMode(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("setNrConfigResponse: serial=%d", serial);
    auto ret = respCb->setNrConfigResponse(serial, static_cast<uint32_t>(errorCode),
                                           aidlimports::Status::SUCCESS);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForQueryNrDisableMode(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilQueryNrDisableModeResult_t> payload) {
  NrConfig config = NrConfig::NR_CONFIG_INVALID;
  if (errorCode == RIL_E_SUCCESS && payload) {
    config = utils::convert_nr_disable_mode(payload->mode);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("onNrConfigResponse: serial=%d errorcode=%d config=%d", serial, errorCode,
                    config);
    auto ret = respCb->onNrConfigResponse(serial, static_cast<uint32_t>(errorCode), config);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForGetQtiRadioCapability(
    int serial, int errorCode,
    std::shared_ptr<qcril::interfaces::RilGetRadioAccessFamilyResult_t> payload) {
  RadioAccessFamily raf = RadioAccessFamily::UNKNOWN;
  if (payload) {
    raf = static_cast<aidlimports::RadioAccessFamily>(payload->radioAccessFamily);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getQtiRadioCapabilityResponse: serial=%d", serial);
    auto ret = respCb->getQtiRadioCapabilityResponse(serial, static_cast<uint32_t>(errorCode), raf);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForGetCallForwardStatus(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data) {
  std::vector<aidlimports::CallForwardInfo> outCfInfoList;
  if (errorCode == RIL_E_SUCCESS && data) {
    utils::convertCallForwardInfoList(outCfInfoList, data->getCallForwardInfo());
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCallForwardStatusResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getCallForwardStatusResponse(serial, static_cast<uint32_t>(errorCode),
                                                    outCfInfoList);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForGetFacilityLockForApp(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> response) {
  int32_t serviceClass = 0;
  if (errorCode == RIL_E_SUCCESS && response) {
    if (response->hasServiceClass()) {
      serviceClass = response->getServiceClass();
    }
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getFacilityLockForAppResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getFacilityLockForAppResponse(serial, static_cast<uint32_t>(errorCode),
                                                     serviceClass);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForGetImei(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> payload) {
  aidlimports::ImeiInfo imeiInfo;
  if (errorCode == RIL_E_SUCCESS && payload != nullptr) {
    utils::convert_imei_info(imeiInfo, payload);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getImeiResponse: serial=%d errorcode=%d imei=%s type %d", serial, errorCode,
                    PII(imeiInfo.imei.c_str()), imeiInfo.type);
    auto ret = respCb->getImeiResponse(serial, static_cast<uint32_t>(errorCode), imeiInfo);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForGetDdsSwitchCapability(int32_t serial,
        RIL_Errno errorCode, bool support) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getDdsSwitchCapabilityResponse: serial=%d", serial);
    auto ret = respCb->getDdsSwitchCapabilityResponse(serial,
            static_cast<uint32_t>(errorCode), support);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendResponseForSendUserPreferenceForDataDuringVoiceCall(int32_t serial,
        RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendUserPreferenceForDataDuringVoiceCallResponse: serial=%d", serial);
    auto ret = respCb->sendUserPreferenceForDataDuringVoiceCallResponse(serial,
            static_cast<uint32_t>(errorCode));
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

// Functions from IQtiRadioStable
ndk::ScopedAStatus QtiRadioStable::queryNrIconType(int32_t serial) {
  QCRIL_LOG_DEBUG("serial=%d", serial);
  auto msg = std::make_shared<rildata::GetDataNrIconTypeMessage>();
  if (msg) {
    GenericCallback<rildata::NrIconType_t> cb(
        ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<rildata::NrIconType_t> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          NrIconType iconType = NrIconType::INVALID;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            iconType = utils::convert_nrIconType_from_response(resp);
            errorCode = RIL_E_SUCCESS;
          }
          sendResponseForQueryNrIconType(serial, errorCode, iconType);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::enableEndc(int32_t in_serial, bool in_enable) {
  QCRIL_LOG_DEBUG("serial=%d enable:%d", in_serial, in_enable);
  auto msg = std::make_shared<RilRequestEnableEndcMessage>(getContext(in_serial), in_enable);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_SYSTEM_ERR;
          if (status == Message::Callback::Status::SUCCESS) {
            if (resp) {
              errorCode = resp->errorCode;
            } else {
              errorCode = RIL_E_NO_MEMORY;
            }
          }
          sendResponseForEnableEndc(in_serial, errorCode, aidlimports::Status::SUCCESS);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForEnableEndc(in_serial, RIL_E_NO_MEMORY, aidlimports::Status::FAILURE);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::queryEndcStatus(int32_t in_serial) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestQueryEndcStatusMessage>(this->getContext(in_serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_SYSTEM_ERR;
          std::shared_ptr<qcril::interfaces::RilQueryEndcStatusResult_t> payload;
          if (status == Message::Callback::Status::SUCCESS) {
            if (resp) {
              errorCode = resp->errorCode;
              payload = std::static_pointer_cast<qcril::interfaces::RilQueryEndcStatusResult_t>(
                  resp->data);
            } else {
              errorCode = RIL_E_NO_MEMORY;
            }
          } else if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
            errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
          }
          sendResponseForQueryEndcStatus(in_serial, errorCode, payload);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForQueryEndcStatus(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::getPropertyValue(const std::string& in_prop,
                                                      const std::string& in_def,
                                                      std::string* _aidl_return) {
  QCRIL_LOG_DEBUG("in_prop=%s in_def=%d", in_prop.c_str(), in_def.c_str());
  std::string prop_val = in_def;
  if (!in_prop.empty()) {
    property_id_type prop_id = utils::getPropertyId(in_prop);
    QCRIL_LOG_DEBUG("Get property from %s", (prop_id != PROPERTY_ID_MAX ? "QCRILDB" : "ADB"));
    if (prop_id != PROPERTY_ID_MAX) {
      std::string config_value;
      if (qcril_config_get(prop_id, config_value) == E_SUCCESS) {
        prop_val = config_value;
      }
    } else {
      char string_val[PROPERTY_VALUE_MAX]{0};
      qmi_ril_get_property_value_from_string(in_prop.c_str(), string_val, in_def.c_str());
      prop_val = string_val;
    }
  }
  QCRIL_LOG_DEBUG("Property: %s, Value: %s", in_prop.c_str(), prop_val.c_str());
  if (_aidl_return != nullptr) {
    *_aidl_return = prop_val;
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::setNrConfig(int32_t in_serial,
                                                 aidlimports::NrConfig in_config) {
  QCRIL_LOG_DEBUG("serial=%d NrConfig=%d", in_serial, in_config);
  auto msg = std::make_shared<RilRequestSetNrDisableModeMessage>(this->getContext(in_serial),
                                                            utils::convert_nr_config(in_config));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status /*status*/,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_NO_MEMORY;
          if (resp) {
            errorCode = resp->errorCode;
          }
          sendResponseForSetNrDisableMode(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForSetNrDisableMode(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::queryNrConfig(int32_t in_serial) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestQueryNrDisableModeMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status /*status*/,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_NO_MEMORY;
          std::shared_ptr<qcril::interfaces::RilQueryNrDisableModeResult_t> payload;
          if (resp) {
            errorCode = resp->errorCode;
            payload =
                std::static_pointer_cast<qcril::interfaces::RilQueryNrDisableModeResult_t>(resp->data);
          }
          sendResponseForQueryNrDisableMode(in_serial, errorCode, payload);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForQueryNrDisableMode(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::getQtiRadioCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);
  auto msg =
      std::make_shared<RilRequestGetEnhancedRadioCapabilityMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status /*status*/,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          std::shared_ptr<qcril::interfaces::RilGetRadioAccessFamilyResult_t> payload = nullptr;
          RIL_Errno errorCode = RIL_E_NO_MEMORY;
          if (resp != nullptr) {
            errorCode = resp->errorCode;
            payload = std::static_pointer_cast<qcril::interfaces::RilGetRadioAccessFamilyResult_t>(
                resp->data);
          }
          sendResponseForGetQtiRadioCapability(in_serial, errorCode, payload);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetQtiRadioCapability(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::getCallForwardStatus(
    int32_t in_serial, const aidlimports::CallForwardInfo& in_callForwardInfo) {
  QCRIL_LOG_DEBUG("getCallForwardStatus: serial=%d", in_serial);
  bool sendFailure = false;
  do {
    auto msg = std::make_shared<QcRilRequestQueryCallForwardMessage>(this->getContext(in_serial));
    if (msg == nullptr) {
      QCRIL_LOG_ERROR("msg is nullptr");
      sendFailure = true;
      break;
    }
    // Set parameters
    if (in_callForwardInfo.reason != INT32_MAX) {
      msg->setReason(in_callForwardInfo.reason);
    }
    if (in_callForwardInfo.serviceClass != INT32_MAX) {
      msg->setServiceClass(in_callForwardInfo.serviceClass);
    }
    msg->setExpectMore(in_callForwardInfo.expectMore);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::GetCallForwardRespData> callFwdData{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            callFwdData =
                std::static_pointer_cast<qcril::interfaces::GetCallForwardRespData>(resp->data);
          }
          this->sendResponseForGetCallForwardStatus(in_serial, errorCode, callFwdData);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } while (FALSE);
  if (sendFailure) {
    this->sendResponseForGetCallForwardStatus(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::getFacilityLockForApp(
    int32_t serial, const aidlimports::FacilityLockInfo& in_facilityLockInfo) {
  QCRIL_LOG_DEBUG("getFacilityLockForApp: serial=%d, facility=%s, serviceClass=%d", serial,
                  in_facilityLockInfo.facility.c_str(), in_facilityLockInfo.serviceClass);
  bool sendFailure = false;
  RIL_Errno errResp = RIL_E_NO_MEMORY;

  do {
    qcril::interfaces::FacilityType rilFacility =
        utils::convertFacilityType(in_facilityLockInfo.facility);
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
      queryMsg->setServiceClass(in_facilityLockInfo.serviceClass);
      queryMsg->setExpectMore(in_facilityLockInfo.expectMore);
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> suppSvcData{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              suppSvcData =
                  std::static_pointer_cast<qcril::interfaces::SuppServiceStatusInfo>(resp->data);
            }
            sendResponseForGetFacilityLockForApp(serial, errorCode, suppSvcData);
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
    sendResponseForGetFacilityLockForApp(serial, errResp, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::getImei(int32_t in_serial) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetImeiMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status /*status*/,
                          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_NO_MEMORY;
          std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> payload;
          if (resp) {
            errorCode = resp->errorCode;
            payload =
                std::static_pointer_cast<qcril::interfaces::RilGetImeiInfoResult_t>(resp->data);
          }
          sendResponseForGetImei(in_serial, errorCode, payload);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetImei(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
 }

::ndk::ScopedAStatus QtiRadioStable::getDdsSwitchCapability(int32_t in_serial) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);
#ifndef QMI_RIL_UTF
  auto msg = std::make_shared<rildata::GetSmartTempDdsSwitchCapabilityMessage>();
    if (msg) {
      GenericCallback<rildata::SmartDdsSwitchCapability_t> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<rildata::SmartDdsSwitchCapability_t> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            bool caps = false;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              caps = utils::convertSmartDdsSwitchCapabilityFromResponse(resp);
              errorCode = RIL_E_SUCCESS;
            }
            sendResponseForGetDdsSwitchCapability(in_serial, errorCode, caps);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForGetDdsSwitchCapability(in_serial, RIL_E_NO_MEMORY, false);
    }
#endif
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::sendUserPreferenceForDataDuringVoiceCall(
        int32_t in_serial, bool userPreference) {
  QCRIL_LOG_DEBUG("serial=%d", in_serial);
#ifndef QMI_RIL_UTF
  auto msg = std::make_shared<rildata::RegisterDataDuringVoiceMessage>(
          utils::convertQcRilInstanceId(QtiRadioStable::getInstanceId()),
          utils::convertUserPeferenceForDataDuringVoiceCall(userPreference));
    if (msg) {
      GenericCallback<RIL_Errno> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                            std::shared_ptr<RIL_Errno> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS) {
              if (resp) {
                errorCode = *resp;
              } else {
                errorCode = RIL_E_NO_MEMORY;
              }
            }
            sendResponseForSendUserPreferenceForDataDuringVoiceCall(in_serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForSendUserPreferenceForDataDuringVoiceCall(in_serial, RIL_E_NO_MEMORY);
    }
#endif
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::isEpdgOverCellularDataSupported(bool* _aidl_return) {
  QCRIL_LOG_DEBUG("isEpdgOverCellularDataSupported: %d ", getEpdgSupport());

  if (_aidl_return != nullptr) {
    if (getEpdgSupport() == EpdgSupport::ENABLE) {
      *_aidl_return = true;
    } else {
      *_aidl_return = false;
    }
  }

  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::sendResponseForsetNrUltraWidebandIconConfig(int32_t serial,
        RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("sendUserPreferenceForDataDuringVoiceCallResponse: serial=%d", serial);
    auto ret = respCb->setNrUltraWidebandIconConfigResponse(serial,
            static_cast<uint32_t>(errorCode));
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

::ndk::ScopedAStatus QtiRadioStable::getNetworkSelectionMode(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getNetworkSelectionMode: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestQueryNetworkSelectModeMessage>(getContext(in_serial));
  if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilModeResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetSelectModeResult_t>(resp->data);
            }
            sendResponseForGetNetworkSelectionMode(in_serial, errorCode, rilModeResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
  } else {
      sendResponseForGetNetworkSelectionMode(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::sendResponseForGetNetworkSelectionMode(int32_t serial,
        RIL_Errno errorCode,
        std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult) {
  aidlimports::NetworkSelectionMode networkSelectionMode{};
  if (errorCode == RIL_E_SUCCESS && rilModeResult) {
    networkSelectionMode.isManual = rilModeResult->bManual;
    networkSelectionMode.accessMode = utils::convertAccessModeFromRil(rilModeResult->accessMode);
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getNetworkSelectionModeResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getNetworkSelectionModeResponse(serial, errorCode, networkSelectionMode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

::ndk::ScopedAStatus QtiRadioStable::setNrUltraWidebandIconConfig(
    int32_t in_serial,
    int32_t in_sib2Value,
    const std::optional<aidlimports::NrUwbIconBandInfo>& in_saBandInfo,
    const std::optional<aidlimports::NrUwbIconBandInfo>& in_nsaBandInfo,
    const std::optional<std::vector<std::optional<aidlimports::NrUwbIconRefreshTime>>>& in_refreshTime,
    const std::optional<aidlimports::NrUwbIconBandwidthInfo>& in_minAggregateBwInfo) {

#ifndef QMI_RIL_UTF
  auto msg = std::make_shared<rildata::SetNrUCIconConfigureMessage>();
  if (msg) {
    if(in_sib2Value >= 0 && in_sib2Value != INT_MAX) {
      msg->setSibTwoValue(in_sib2Value);
    }
    if(in_saBandInfo.has_value()) {
      rildata::NrUCIconBandInfo_t rilBandInfo {};
      utils::convertNrUCIconBandInfo(rilBandInfo, *in_saBandInfo);
      msg->setSaBandList(rilBandInfo);
    }
    if(in_nsaBandInfo.has_value()) {
      rildata::NrUCIconBandInfo_t rilBandInfo {};
      utils::convertNrUCIconBandInfo(rilBandInfo, *in_nsaBandInfo);
      msg->setNsaBandList(rilBandInfo);
    }
    if(in_refreshTime.has_value()) {
      std::vector<rildata::NrUCIconRefreshTime_t> refreshList;
      refreshList.resize(in_refreshTime->size());
      for(int i=0, j=0; i<in_refreshTime->size(); i++) {
        if((*in_refreshTime)[i].has_value()) {
          refreshList[j].timerType = utils::convertNrUCIconRefreshTimer((*in_refreshTime)[i]->timerType);
          refreshList[j].timeValue = (*in_refreshTime)[i]->timeValue;
          j++;
        }
      }
      msg->setRefreshTimeList(refreshList);
    }
    if(in_minAggregateBwInfo.has_value()) {
      rildata::NrUCIconBandwidthInfo_t bandwidthInfo {};
      utils::convertNrUCIconBandwidthInfo(bandwidthInfo, *in_minAggregateBwInfo);
      msg->setMinBandwidth(bandwidthInfo);
    }

    GenericCallback<RIL_Errno> cb(
      [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<RIL_Errno> resp) -> void {
        RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
        if (status == Message::Callback::Status::SUCCESS) {
          if (resp) {
            errorCode = *resp;
          } else {
            errorCode = RIL_E_NO_MEMORY;
          }
        }
        sendResponseForsetNrUltraWidebandIconConfig(in_serial, errorCode);
      });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForsetNrUltraWidebandIconConfig(in_serial, RIL_E_NO_MEMORY);
  }
#endif

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::setNetworkSelectionModeAutomatic(int32_t in_serial,
        aidlimports::AccessMode in_mode) {
  QCRIL_LOG_DEBUG("setNetworkSelectionModeAutomatic: serial=%d, AccessMode=%d", in_serial, in_mode);

  auto msg = std::make_shared<RilRequestSetNetworkSelectionAutoMessage>(getContext(in_serial));
  if (msg) {
    msg->setAccessMode(utils::convertAccessMode(in_mode));
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponseForSetNetworkSelectionModeAutomatic(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForSetNetworkSelectionModeAutomatic(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::sendResponseForSetNetworkSelectionModeAutomatic(int32_t serial,
    RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("QtiRadio: getResponseCallback Failed");
    return;
  }
  QCRIL_LOG_DEBUG("setNetworkSelectionModeAutomaticResponse: serial=%d, error=%d", serial,
                   errorCode);
  auto ret = respCb->setNetworkSelectionModeAutomaticResponse(serial,
                                                              static_cast<uint32_t>(errorCode));
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus QtiRadioStable::setNetworkSelectionModeManual(int32_t serial,
        const aidlimports::SetNetworkSelectionMode& in_setNetworkSelectionMode) {
  QCRIL_LOG_INFO("setNetworkSelectionModeManual: serial=%d, ran=%d,"
                  " AccessMode=%d, ", serial, in_setNetworkSelectionMode.ran);

  RIL_RadioTechnology rat = utils::getRilRadioTechnologyFromRan(in_setNetworkSelectionMode.ran);
  auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(getContext(serial),
      in_setNetworkSelectionMode.operatorNumeric, rat);
  if (msg) {
    RIL_AccessMode rilAccessMode = utils::convertAccessMode(in_setNetworkSelectionMode.accessMode);
    if (rilAccessMode != RIL_ACCESS_MODE_INVALID) {
      msg->setAccessMode(rilAccessMode);
    }
    if (in_setNetworkSelectionMode.cagInfo) {
      auto &cagInfo = *(in_setNetworkSelectionMode.cagInfo);
      msg->setCagId(cagInfo.cagId);
    }
    if (!in_setNetworkSelectionMode.snpnNid.empty()) {
      msg->setSnpnId(in_setNetworkSelectionMode.snpnNid);
    }
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            sendResponseForSetNetworkSelectionModeManual(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForSetNetworkSelectionModeManual(serial, RIL_E_NO_MEMORY);
    }
  }
  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::sendResponseForSetNetworkSelectionModeManual(int32_t serial,
                                                                  RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("QtiRadio: getResponseCallback Failed");
    return;
  }
  QCRIL_LOG_DEBUG("setNetworkSelectionModeManualResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setNetworkSelectionModeManualResponse(serial,
            static_cast<uint32_t>(errorCode));
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus QtiRadioStable::startNetworkScan(int32_t serial,
        const aidlimports::QtiNetworkScanRequest& request) {
  QCRIL_LOG_DEBUG("startNetworkScan: serial=%d, %s", serial, request.toString().c_str());

  mIsScanRequested = true;
  RIL_NetworkScanRequest scanRequest{};
  RIL_Errno res = utils::fillNetworkScanRequest(request, scanRequest);

  if (res != RIL_E_SUCCESS) {
    sendResponseForStartNetworkScan(serial, res);
    return ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<RilRequestStartNetworkScanMessage>(getContext(serial), scanRequest);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponseForStartNetworkScan(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForStartNetworkScan(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::sendResponseForStartNetworkScan(int32_t serial,
        RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("startNetworkScanResponse: serial=%d errorCode=%d", serial, errorCode);
    auto ret = respCb->startNetworkScanResponse(serial, static_cast<uint32_t>(errorCode));
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

::ndk::ScopedAStatus QtiRadioStable::stopNetworkScan(int32_t serial) {
  QCRIL_LOG_DEBUG("stopNetworkScan: serial=%d",serial);

  auto msg = std::make_shared<RilRequestStopNetworkScanMessage>(getContext(serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          mIsScanRequested = false;
          sendResponseForStopNetworkScan(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForStopNetworkScan(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::sendResponseForStopNetworkScan(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("stopNetworkScanResponse: serial=%d errorCode=%d", serial, errorCode);
    auto ret = respCb->stopNetworkScanResponse(serial, static_cast<uint32_t>(errorCode));
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void QtiRadioStable::sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {

  QCRIL_LOG_DEBUG("sendNetworkScanResult: mIsScanRequested=%d", mIsScanRequested);
  if(!mIsScanRequested){
      return;
  }
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("QtiRadioStable: getIndicationCallback Failed");
    return;
  }

  if (indCb) {
    aidlimports::QtiNetworkScanResult result {};
    result.status = static_cast<int32_t>(msg->getStatus());
    result.error = static_cast<int32_t>(msg->getError());
    utils::convert(msg->getNetworkInfo(), result.networkInfos);
    if(result.status == aidlimports::QtiNetworkScanResult::SCAN_STATUS_COMPLETE) {
      mIsScanRequested = false;
    }

    QCRIL_LOG_DEBUG("UNSOL: networkScanResult");
    auto ret = indCb->networkScanResult(result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

::ndk::ScopedAStatus QtiRadioStable::getCiwlanConfig(aidlimports::CiwlanConfig* _aidl_return) {
  QCRIL_LOG_DEBUG("QtiRadioStable: getCiwlanConfig");
  auto msg = std::make_shared<QcRilRequestImsQueryCIWLANConfigSyncMessage>();
  if (msg) {
    if (_aidl_return != nullptr) {
      std::shared_ptr<qcril::interfaces::ImsCiWlanConfig> ciwlanConfigResult = nullptr;
      auto ret = msg->dispatchSync(ciwlanConfigResult);
      if (ret == Message::Callback::Status::SUCCESS && ciwlanConfigResult != nullptr) {
          _aidl_return->homeMode = utils::convertCiwlanModeFromRil(
              ciwlanConfigResult->c_iwlan_mode_in_home);
          _aidl_return->roamMode = utils::convertCiwlanModeFromRil(
              ciwlanConfigResult->c_iwlan_mode_in_roam);
      } else {
          QCRIL_LOG_ERROR("QtiRadioStable: getCiwlanConfig - no memory");
          _aidl_return->homeMode = aidlimports::CiwlanMode::INVALID;
          _aidl_return->roamMode = aidlimports::CiwlanMode::INVALID;
      }
    } else {
        QCRIL_LOG_DEBUG("QtiRadioStable: getCiwlanConfig - null arg");
    }
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus QtiRadioStable::getSimPersoUnlockStatus(
    aidlimports::PersoUnlockStatus* _aidl_return)
{
  QCRIL_LOG_DEBUG("QtiRadioStable: getSimPersoUnlockStatus");
  aidlimports::PersoUnlockStatus persoUnlockStatus = aidlimports::PersoUnlockStatus::UNKNOWN;
  auto msg = std::make_shared<UimGetPersoLockStatusRequestMsg>(this->getInstanceId());
  if (msg) {
    std::shared_ptr<RIL_UIM_Perso_lock_Status> response;
    auto result = msg->dispatchSync(response);
    if (result == Message::Callback::Status::SUCCESS && response) {
        persoUnlockStatus = utils::convertRilPersolockStatus(*response);
    }
  }

  if (_aidl_return) {
    *_aidl_return = persoUnlockStatus;
  }
  return ndk::ScopedAStatus::ok();
}

void QtiRadioStable::notifyOnSimPersoLockStatus(std::shared_ptr<UimSimPersoLockStatusInd> msg)
{
    auto indCb = getIndicationCallback();
    if (indCb && msg) {
      RIL_UIM_Perso_lock_Status persoLockStatus = msg->get_persolock_ind();
      aidlimports::PersoUnlockStatus result =
          utils::convertRilPersolockStatus(persoLockStatus);
      QCRIL_LOG_DEBUG("notifyOnSimPersoLockStatus: persoUnlockStatus=%d", result);
      auto ret = indCb->onSimPersoUnlockStatusChange(result);
      if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
      }
    }
}

void QtiRadioStable::cleanup()
{
  mIsScanRequested = false;
}

}  // namespace implementation
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
