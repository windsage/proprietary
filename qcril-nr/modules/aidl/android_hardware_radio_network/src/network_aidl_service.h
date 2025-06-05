/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <framework/legacy.h>
#include <QtiMutex.h>
#include <binder/IBinder.h>
#include "BnRadioNetworkDefault.h"
#include "IRadioNetworkContext.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "interfaces/nas/RilUnsolCdmaPrlChangedMessage.h"
#include "interfaces/sms/RilUnsolImsNetworkStateChangedMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/nas/RilUnsolNetworkStateChangedMessage.h"
#include "interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h"
#include "interfaces/cellinfo/cellinfo.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"
#include "interfaces/cellinfo/RilUnsolCellInfoListMessage.h"
#include "interfaces/nas/RilUnsolSignalStrengthMessage.h"
#include "interfaces/nas/RilUnsolVoiceRadioTechChangedMessage.h"
#include "interfaces/nas/RilUnsolNitzTimeReceivedMessage.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include "UnSolMessages/PhysicalConfigStructUpdateMessage.h"
#include "UnSolMessages/LinkCapIndMessage.h"
#include "interfaces/nas/RilUnsolRestrictedStateChangedMessage.h"
#include "interfaces/NwRegistration/RilUnsolNwRegistrationRejectMessage.h"

namespace aidlnetwork {
  using namespace aidl::android::hardware::radio::network;
}

namespace aidlradio {
  using namespace aidl::android::hardware::radio;
}

class IRadioNetworkImpl : public aidlnetwork::BnRadioNetworkDefault {
 private:
  std::shared_ptr<aidlnetwork::IRadioNetworkResponse> mIRadioNetworkResponse;
  std::shared_ptr<aidlnetwork::IRadioNetworkIndication> mIRadioNetworkIndication;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  bool mIsScanRequested = false;

  std::shared_ptr<aidlnetwork::IRadioNetworkResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mIRadioNetworkResponse;
  }
  std::shared_ptr<aidlnetwork::IRadioNetworkIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mIRadioNetworkIndication;
  }

  //send Response Functions
  void sendResponseForGetAllowedNetworkTypesBitmap(int32_t, RIL_Errno,
    std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t>);
  void sendResponseForGetAvailableBandModes(int32_t, RIL_Errno,
    std::shared_ptr<qcril::interfaces::RilQueryAvailBandsResult_t>);
  void sendResponseForGetAvailableNetworks(int32_t, RIL_Errno,
    std::shared_ptr<qcril::interfaces::RilGetAvailNetworkResult_t>);
  void sendResponseForSetAllowedNetworkTypesBitmap(int32_t, RIL_Errno);
  void sendResponseForSetBandMode(int32_t, RIL_Errno);
  void sendResponseForSetBarringPassword(int32_t, RIL_Errno);
  void sendResponseForSetSuppServiceNotifications(int32_t, RIL_Errno errorCode);
  void sendResponseForSetSystemSelectionChannels(int32_t, RIL_Errno);
  void sendResponseForStartNetworkScan(int32_t, RIL_Errno);
  void sendResponseForStopNetworkScan(int32_t, RIL_Errno);
  void sendResponseForSupplyNetworkDepersonalization(int32_t, RIL_Errno,
    std::shared_ptr<RIL_UIM_PersoResponse>);
  void sendResponseForGetBarringInfo(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilCellBarringInfo_t> rilCellBarring);
  void sendResponseForGetCdmaRoamingPreference(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t> roamPref);
  void sendResponseForGetCellInfoList(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult);
  void sendResponseForGetDataRegistrationState(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult);
  void sendResponseForGetSignalStrength(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult);
  void sendResponseForGetVoiceRadioTechnology(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetVoiceTechResult_t> ratResult);
  void sendResponseForGetVoiceRegistrationState(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult);
  void sendResponseForIsNrDualConnectivityEnabled(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetCdmaRoamingPreference(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetCellInfoListRate(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetSignalStrengthReportingCriteria(int32_t, RIL_Errno);
  void sendResponseForGetUsageSetting(int32_t, RIL_Errno,
    std::shared_ptr<qcril::interfaces::RilGetUsageSettingResult_t>);
  void sendResponseForSetUsageSetting(int32_t, RIL_Errno);
  void sendResponseForGetOperator(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetOperatorResult_t> rilOperatorResult);
  void sendResponseForGetImsRegistrationState(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilGetImsRegistrationResult_t> regStateResp);
  void sendResponseForGetNetworkSelectionMode(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult);
  void sendResponseForGetSystemSelectionChannels(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<qcril::interfaces::RilGetSysSelResult_t> getSysSelResult);
  void sendResponseForSetIndicationFilter(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetLinkCapacityReportingCriteria(int32_t serial, aidlradio::RadioError errorCode);
  void sendResponseForSetLocationUpdates(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetNetworkSelectionModeAutomatic(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetNetworkSelectionModeManual(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetNrDualConnectivityState(int32_t serial, RIL_Errno errorCode);

  void setUnsolRespFilter(int32_t serial, int32_t indicationFilter);
  void setPhysChanConfigReportingFilter(int32_t serial, int32_t indicationFilter);
public:
  IRadioNetworkImpl(qcril_instance_id_e_type instance);
  ~IRadioNetworkImpl();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlnetwork::IRadioNetworkResponse>& radioNetworkResponse,
      const std::shared_ptr<aidlnetwork::IRadioNetworkIndication>& radioNetworkIndication);

  void clearCallbacks();

  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }

  std::shared_ptr<IRadioNetworkContext> getContext(uint32_t serial) {
    std::shared_ptr<IRadioNetworkContext> ctx = std::make_shared<IRadioNetworkContext>(mInstanceId, serial);
    return ctx;
  }

  void deathNotifier(void* cookie);

  // AIDL APIs
  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<aidlnetwork::IRadioNetworkResponse>& in_radioNetworkResponse,
      const std::shared_ptr<aidlnetwork::IRadioNetworkIndication>& in_radioNetworkIndication
  ) override;

  ::ndk::ScopedAStatus responseAcknowledgement();
  ::ndk::ScopedAStatus getAllowedNetworkTypesBitmap(int32_t) override;
  ::ndk::ScopedAStatus getAvailableBandModes(int32_t) override;
  ::ndk::ScopedAStatus getAvailableNetworks(int32_t) override;
  ::ndk::ScopedAStatus setAllowedNetworkTypesBitmap(int32_t, int32_t ) override;
  ::ndk::ScopedAStatus setBandMode(int32_t, aidlnetwork::RadioBandMode) override;
  ::ndk::ScopedAStatus setBarringPassword(int32_t, const std::string&,
      const std::string&, const std::string&) override;
  ::ndk::ScopedAStatus setSignalStrengthReportingCriteria(int32_t,
      const std::vector<aidlnetwork::SignalThresholdInfo>&) override;
  ::ndk::ScopedAStatus setSuppServiceNotifications(int32_t, bool) override;
  ::ndk::ScopedAStatus setSystemSelectionChannels(int32_t, bool,
      const std::vector<aidlnetwork::RadioAccessSpecifier>&) override;
  ::ndk::ScopedAStatus startNetworkScan(int32_t,
      const aidlnetwork::NetworkScanRequest&) override;
  ::ndk::ScopedAStatus stopNetworkScan(int32_t) override;
  ::ndk::ScopedAStatus supplyNetworkDepersonalization(int32_t,
      const std::string&) override;
  ::ndk::ScopedAStatus getBarringInfo(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCdmaRoamingPreference(int32_t in_serial) override;
  ::ndk::ScopedAStatus getCellInfoList(int32_t in_serial) override;
  ::ndk::ScopedAStatus getDataRegistrationState(int32_t in_serial) override;
  ::ndk::ScopedAStatus getSignalStrength(int32_t in_serial) override;
  ::ndk::ScopedAStatus getVoiceRadioTechnology(int32_t in_serial) override;
  ::ndk::ScopedAStatus getVoiceRegistrationState(int32_t in_serial) override;
  ::ndk::ScopedAStatus isNrDualConnectivityEnabled(int32_t in_serial) override;
  ::ndk::ScopedAStatus setCdmaRoamingPreference(int32_t in_serial,
      aidlnetwork::CdmaRoamingType in_type) override;
  ::ndk::ScopedAStatus setCellInfoListRate(int32_t in_serial, int32_t in_rate) override;
  ::ndk::ScopedAStatus getUsageSetting(int32_t) override;
  ::ndk::ScopedAStatus setUsageSetting(int32_t, aidlnetwork::UsageSetting) override;
  ::ndk::ScopedAStatus getImsRegistrationState(int32_t in_serial) override;
  ::ndk::ScopedAStatus getNetworkSelectionMode(int32_t in_serial) override;
  ::ndk::ScopedAStatus getOperator(int32_t in_serial) override;
  ::ndk::ScopedAStatus getSystemSelectionChannels(int32_t in_serial) override;
  ::ndk::ScopedAStatus setIndicationFilter(int32_t in_serial, int32_t in_indicationFilter) override;
  ::ndk::ScopedAStatus setLinkCapacityReportingCriteria(
      int32_t in_serial, int32_t in_hysteresisMs, int32_t in_hysteresisDlKbps,
      int32_t in_hysteresisUlKbps, const std::vector<int32_t>& in_thresholdsDownlinkKbps,
      const std::vector<int32_t>& in_thresholdsUplinkKbps,
      ::aidl::android::hardware::radio::AccessNetwork in_accessNetwork) override;
  ::ndk::ScopedAStatus setLocationUpdates(int32_t in_serial, bool in_enable);
  ::ndk::ScopedAStatus setNetworkSelectionModeAutomatic(int32_t in_serial);
  ::ndk::ScopedAStatus setNetworkSelectionModeManual(int32_t in_serial,
    const std::string& in_operatorNumeric, aidlradio::AccessNetwork in_ran);
  ::ndk::ScopedAStatus setNrDualConnectivityState(int32_t in_serial,
    aidlnetwork::NrDualConnectivityState in_nrDualConnectivityState);
  ::ndk::ScopedAStatus setEmergencyMode(int32_t in_serial, ::aidl::android::hardware::radio::network::EmergencyMode in_emcModeType) override;
  ::ndk::ScopedAStatus triggerEmergencyNetworkScan(
      int32_t in_serial, const ::aidl::android::hardware::radio::network::EmergencyNetworkScanTrigger& in_request) override;
  ::ndk::ScopedAStatus cancelEmergencyNetworkScan(int32_t in_serial, bool in_resetScan) override;
  ::ndk::ScopedAStatus exitEmergencyMode(int32_t in_serial) override;
  ::ndk::ScopedAStatus setNullCipherAndIntegrityEnabled(int32_t in_serial, bool in_enabled) override;
  ::ndk::ScopedAStatus isNullCipherAndIntegrityEnabled(int32_t in_serial) override;
  ::ndk::ScopedAStatus isN1ModeEnabled(int32_t in_serial) override;
  ::ndk::ScopedAStatus setN1ModeEnabled(int32_t in_serial, bool in_enable) override;

  // Send Indication Functions
  void sendCdmaPrlChanged(std::shared_ptr<RilUnsolCdmaPrlChangedMessage>);
  void sendImsNetworkStateChanged(std::shared_ptr<RilUnsolImsNetworkStateChangedMessage>);
  void sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage>);
  void sendNetworkStateChanged(std::shared_ptr<RilUnsolNetworkStateChangedMessage>);
  void sendSuppSvcNotify(std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg);
  void sendCellBarring(std::shared_ptr<RilUnsolCellBarringMessage> msg);
  void sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg);
  void sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> msg);
  void sendVoiceRadioTechChanged(std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg);
  void sendNitzTimeReceived(std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg);
  void sendLinkCapInd(std::shared_ptr<rildata::LinkCapIndMessage> msg);
  void sendPhysicalConfigStructUpdateInd(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg);
  void sendNwRegistrationReject(std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg);
  void sendRestrictedStateChanged(std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg);
#ifdef QMI_RIL_UTF
  void cleanup();
#endif
};
