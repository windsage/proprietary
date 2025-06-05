/******************************************************************************
#  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_RADIO_STABLE_AIDL_SERVICE_H_
#define _QTI_RADIO_STABLE_AIDL_SERVICE_H_

#include <aidl/vendor/qti/hardware/radio/qtiradio/BnQtiRadio.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/CallForwardInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/FacilityLockInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadioResponse.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/IQtiRadioIndication.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrUwbIconBandInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrUwbIconRefreshTime.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NrUwbIconBandwidthInfo.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanRequest.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanResult.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/NetworkSelectionMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/QtiNetworkScanRequest.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/AccessMode.h>
#include <aidl/vendor/qti/hardware/radio/qtiradio/SetNetworkSelectionMode.h>
#include "interfaces/mbn/QcRilUnsolPdcRefreshMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/uim/UimSimPerSoUnlockStatusInd.h"
#include <QtiMutex.h>
#include "qti_radio_stable_aidl_service_utils.h"

#include <framework/legacy.h>
#include "QtiRadioContext.h"

#undef TAG
#define TAG "RILQ"

#include <memory>

namespace aidlimports {
using namespace aidl::vendor::qti::hardware::radio::qtiradio;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace implementation {

class QtiRadioStable : public aidlimports::BnQtiRadio {
 public:
  void deathNotifier(void* cookie);
  std::shared_ptr<QtiRadioContext> getContext(uint32_t serial);
  void setInstanceId(qcril_instance_id_e_type instanceId);
  qcril_instance_id_e_type getInstanceId();
  enum class EpdgSupport {
    INVALID = -1,
    ENABLE = 0,
    DISABLE = 1,
  };
  EpdgSupport getEpdgSupport();
  void setEpdgSupport(bool support);

  void setCallback_nolock(
      const std::shared_ptr<aidlimports::IQtiRadioResponse>& in_responseCallback,
      const std::shared_ptr<aidlimports::IQtiRadioIndication>& in_indicationCallback);

  // Unsol Message handlers
  void notifyOnNrIconTypeChange(NrIconType /*iconType*/);
  void notifyOnImeiChange(aidlimports::ImeiInfo /*info*/);
  void notifyDdsSwitchCapabilityChange();
  void notifyDdsSwitchCriteriaChange(bool telephonyDdsSwitch);
  void notifyDdsSwitchRecommendation(int32_t recommendedSlotId);
  void notifyConfigureDeactivateDelayTime(int64_t delayTime);
  void notifyDataCapabilityChange(bool change);
  void notifyOnPdcRefresh(std::shared_ptr<QcRilUnsolPdcRefreshMessage> msg);
  void sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg);
  void notifyOnSimPersoLockStatus(std::shared_ptr<UimSimPersoLockStatusInd> msg);

  // AIDL request functions
  ::ndk::ScopedAStatus setCallbacks(
      const std::shared_ptr<aidlimports::IQtiRadioResponse>& in_responseCallback,
      const std::shared_ptr<aidlimports::IQtiRadioIndication>& in_indicationCallback) override;
  ::ndk::ScopedAStatus queryNrIconType(int32_t serial) override;
  ::ndk::ScopedAStatus enableEndc(int32_t in_serial, bool in_enable) override;
  ::ndk::ScopedAStatus queryEndcStatus(int32_t in_serial) override;
  ::ndk::ScopedAStatus getPropertyValue(const std::string& in_prop, const std::string& in_def,
                                        std::string* _aidl_return) override;
  ::ndk::ScopedAStatus setNrConfig(int32_t in_serial, aidlimports::NrConfig in_config) override;
  ::ndk::ScopedAStatus queryNrConfig(int32_t in_serial) override;
  ::ndk::ScopedAStatus getQtiRadioCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCallForwardStatus(
      int32_t in_serial, const aidlimports::CallForwardInfo& in_callForwardInfo) override;
  ::ndk::ScopedAStatus getFacilityLockForApp(
      int32_t serial, const aidlimports::FacilityLockInfo& in_facilityLockInfo) override;
  ::ndk::ScopedAStatus getImei(int32_t in_serial) override;
  ::ndk::ScopedAStatus getDdsSwitchCapability(int32_t in_serial) override;
  ::ndk::ScopedAStatus sendUserPreferenceForDataDuringVoiceCall(int32_t in_serial,
      bool userPreference) override;
  ::ndk::ScopedAStatus isEpdgOverCellularDataSupported(bool* _aidl_return) override;
  ::ndk::ScopedAStatus setNrUltraWidebandIconConfig(
      int32_t in_serial,
      int32_t in_sib2Value,
      const std::optional<aidlimports::NrUwbIconBandInfo>& in_saBandInfo,
      const std::optional<aidlimports::NrUwbIconBandInfo>& in_nsaBandInfo,
      const std::optional<std::vector<std::optional<aidlimports::NrUwbIconRefreshTime>>>& in_refreshTime,
      const std::optional<aidlimports::NrUwbIconBandwidthInfo>& in_minAggregateBwInfo) override;
  ::ndk::ScopedAStatus getNetworkSelectionMode(int32_t in_serial) override;
  ::ndk::ScopedAStatus setNetworkSelectionModeAutomatic(int32_t in_serial,
      aidlimports::AccessMode in_mode) override;
  ::ndk::ScopedAStatus setNetworkSelectionModeManual(int32_t in_serial,
      const aidlimports::SetNetworkSelectionMode& in_setNetworkSelectionMode) override;
  ::ndk::ScopedAStatus startNetworkScan(int32_t in_serial,
      const aidlimports::QtiNetworkScanRequest& in_request) override;
  ::ndk::ScopedAStatus stopNetworkScan(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCiwlanConfig(aidlimports::CiwlanConfig* _aidl_return) override;
  ::ndk::ScopedAStatus getSimPersoUnlockStatus(
      aidlimports::PersoUnlockStatus* _aidl_return) override;

  void cleanup();

 private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient* mDeathRecipient{nullptr};
  std::shared_ptr<aidlimports::IQtiRadioResponse> mResponseCb;
  std::shared_ptr<aidlimports::IQtiRadioIndication> mIndicationCb;
  EpdgSupport mEpdgSupport = EpdgSupport::INVALID;
  bool mIsScanRequested = false;

 protected:
  std::shared_ptr<aidlimports::IQtiRadioResponse> getResponseCallback();
  std::shared_ptr<aidlimports::IQtiRadioIndication> getIndicationCallback();
  // Send Responses
  void sendResponseForQueryNrIconType(int32_t serial, RIL_Errno errorCode, NrIconType iconType);
  void sendResponseForEnableEndc(int32_t serial, RIL_Errno errorCode, aidlimports::Status status);
  void sendResponseForQueryEndcStatus(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilQueryEndcStatusResult_t> payload);
  void sendResponseForSetNrDisableMode(int32_t serial, RIL_Errno errorCode);
  void sendResponseForQueryNrDisableMode(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilQueryNrDisableModeResult_t> payload);
  void sendResponseForGetQtiRadioCapability(
      int serial, int errorCode,
      std::shared_ptr<qcril::interfaces::RilGetRadioAccessFamilyResult_t> payload);
  void sendResponseForGetCallForwardStatus(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::GetCallForwardRespData> data);
  void sendResponseForGetFacilityLockForApp(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::SuppServiceStatusInfo> response);
  void sendResponseForGetImei(int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetImeiInfoResult_t> payload);
  void sendResponseForGetDdsSwitchCapability(int32_t serial, RIL_Errno errorCode, bool support);
  void sendResponseForSendUserPreferenceForDataDuringVoiceCall(int32_t serial, RIL_Errno errorCode);
  void sendResponseForsetNrUltraWidebandIconConfig(int32_t in_serial, RIL_Errno errorCode);
  void sendResponseForStartNetworkScan(int32_t serial, RIL_Errno errorCode);
  void sendResponseForStopNetworkScan(int32_t, RIL_Errno);
  void sendResponseForGetNetworkSelectionMode(int32_t serial,
        RIL_Errno errorCode, std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult);
  void sendResponseForSetNetworkSelectionModeAutomatic(int32_t serial,
        RIL_Errno errorCode);
  void sendResponseForSetNetworkSelectionModeManual(int32_t serial, RIL_Errno errorCode);
  void clearCallbacks();
  void clearCallbacks_nolock();
};

}  // namespace implementation
}  // namespace qtiradio
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  //_QTI_RADIO_STABLE_AIDL_SERVICE_H_
