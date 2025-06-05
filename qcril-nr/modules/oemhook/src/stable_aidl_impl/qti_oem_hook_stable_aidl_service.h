/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __OEMHOOK_STABLE_AIDL_SERVICE_H__
#define __OEMHOOK_STABLE_AIDL_SERVICE_H__

#include "QtiMutex.h"
#include "framework/legacy.h"

#include "OemHookContext.h"

#include <aidl/vendor/qti/hardware/radio/qcrilhook/BnQtiOemHook.h>
#include <aidl/vendor/qti/hardware/radio/qcrilhook/RadioError.h>

#include "telephony/ril.h"
#include <interfaces/pbm/pbm.h>
#include <interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h>
#include <interfaces/pbm/QcRilUnsolAdnInitDoneMessage.h>
#include <interfaces/mbn/QcRilUnsolMbnConfigResultMessage.h>
#include <interfaces/mbn/QcRilUnsolMbnConfigClearedMessage.h>
#include <interfaces/mbn/QcRilUnsolMbnValidateDumpedMessage.h>
#include <interfaces/mbn/QcRilUnsolMbnConfigListMessage.h>
#include <interfaces/mbn/QcRilUnsolMbnValidateConfigMessage.h>
#include <interfaces/uim/UimSimlockTempUnlockExpireInd.h>
#include <interfaces/uim/UimCardStateChangeInd.h>
#include <interfaces/uim/UimSlotStatusInd.h>
#include <interfaces/uim/UimSimRefreshIndication.h>
#include <interfaces/uim/UimVoltageStatusInd.h>
#include <modules/uim_remote/UimRmtRemoteSimStatusIndMsg.h>
#include <interfaces/uim/UimOemHook.h>
#include <interfaces/voice/QcRilUnsolDtmfMessage.h>
#include <interfaces/voice/QcRilUnsolExtBurstIntlMessage.h>
#include <interfaces/voice/QcRilUnsolNssReleaseMessage.h>
#include <interfaces/voice/QcRilUnsolSuppSvcErrorCodeMessage.h>
#include <interfaces/voice/QcRilUnsolSpeechCodecInfoMessage.h>
#include <interfaces/voice/QcRilUnsolAudioStateChangedMessage.h>
#include <interfaces/sms/QcRilUnsolWmsReadyMessage.h>
#include "interfaces/dms/RilUnsolMaxActiveDataSubsChangedMessage.h"
#include "interfaces/nas/RilUnsolCsgIdChangedMessage.h"
#include "interfaces/nas/RilUnsolEngineerModeMessage.h"
#include "interfaces/nas/RilUnsolOemNetworkScanMessage.h"
#include "interfaces/nas/RilUnsolSubProvisioningStatusMessage.h"

namespace aidlimports {
  using namespace aidl::vendor::qti::hardware::radio::qcrilhook;
}

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qcrilhook {
namespace implementation {

class OemHookStable : public aidlimports::BnQtiOemHook {
 public:
  void deathNotifier(void* cookie);
  ::ndk::ScopedAStatus setCallback(const std::shared_ptr<aidlimports::IQtiOemHookResponse>& respCb,
                                   const std::shared_ptr<aidlimports::IQtiOemHookIndication>& indCb);

  void sendResponse(int32_t /*serial*/, aidlimports::RadioError /*errorCode*/,
                    const std::vector<uint8_t>& /*respData*/);

  void sendResponse(int32_t /*serial*/, aidlimports::RadioError /*errorCode*/);

  void sendResponse(int /*serial*/, RIL_Errno /*errorCode*/, uint8_t* /*buf*/, size_t /*bufLen*/);

  void sendIndication(const std::vector<uint8_t>& /*respData*/);

  void sendIndication(uint8_t* /*buf*/, size_t /*bufLen*/);

  ::ndk::ScopedAStatus oemHookRawRequest(int32_t serial, const std::vector<uint8_t>& data);

  void sendAdnRecords(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg);
  void sendAdnInitDone(std::shared_ptr<QcRilUnsolAdnInitDoneMessage> msg);
  void sendMbnConfigResult(std::shared_ptr<QcRilUnsolMbnConfigResultMessage> msg);
  void sendMbnConfigCleared(std::shared_ptr<QcRilUnsolMbnConfigClearedMessage> msg);
  void sendMbnValidateDumped(std::shared_ptr<QcRilUnsolMbnValidateDumpedMessage> msg);
  void sendMbnConfigList(std::shared_ptr<QcRilUnsolMbnConfigListMessage> msg);
  void sendMbnValidateConfig(std::shared_ptr<QcRilUnsolMbnValidateConfigMessage> msg);
  void uimSimlockTempUnlockExpireInd(std::shared_ptr<UimSimlockTempUnlockExpireInd> msg);
  void uimCardStateChangeInd(std::shared_ptr<UimCardStateChangeInd> msg);
  void uimSlotStatusInd(std::shared_ptr<UimSlotStatusInd> msg);
  void uimSimRefreshIndication(std::shared_ptr<UimSimRefreshIndication> msg);
  void uimVoltageStatusInd(std::shared_ptr<UimVoltageStatusInd> msg);
  void uimRmtRemoteSimStatusIndMsg(std::shared_ptr<UimRmtRemoteSimStatusIndMsg> msg);
  void sendUnsolCdmaBurstDtmf(std::shared_ptr<QcRilUnsolDtmfMessage> msg);
  void sendUnsolCdmaContDtmfStart(std::shared_ptr<QcRilUnsolDtmfMessage> msg);
  void sendUnsolCdmaContDtmfStop(std::shared_ptr<QcRilUnsolDtmfMessage> msg);
  void sendUnsolExtendedDbmIntl(std::shared_ptr<QcRilUnsolExtBurstIntlMessage> msg);
  void sendUnsolNssRelease(std::shared_ptr<QcRilUnsolNssReleaseMessage> msg);
  void sendUnsolSsErrorCode(std::shared_ptr<QcRilUnsolSuppSvcErrorCodeMessage> msg);
  void sendUnsolSpeechCodecInfo(std::shared_ptr<QcRilUnsolSpeechCodecInfoMessage> msg);
  void sendUnsolAudioStateChanged(std::shared_ptr<QcRilUnsolAudioStateChangedMessage> msg);
  void sendUnsolWmsReady(std::shared_ptr<QcRilUnsolWmsReadyMessage> msg);
  void sendUnsolMaxActiveDataSubsChanged(
      std::shared_ptr<RilUnsolMaxActiveDataSubsChangedMessage> msg);
  void sendUnsolCsgIdChanged(std::shared_ptr<RilUnsolCsgIdChangedMessage> msg);
  void sendUnsolEngineerMode(std::shared_ptr<RilUnsolEngineerModeMessage> msg);
  void sendUnsolNetworkScanResult(std::shared_ptr<RilUnsolOemNetworkScanMessage> msg);
  void sendUnsolSubProvisionStatusChanged(std::shared_ptr<RilUnsolSubProvisioningStatusMessage> msg);
 
 private:
  qcril_instance_id_e_type mInstanceId;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qtimutex::QtiSharedMutex mCallbackLock;
  std::shared_ptr<aidlimports::IQtiOemHookResponse> mResponseCb;
  std::shared_ptr<aidlimports::IQtiOemHookIndication> mIndicationCb;

  /**
   * Clean up the callback handlers.
   */
  void clearCallbacks_nolock();
  void clearCallbacks();

  void setCallback_nolock(
      const std::shared_ptr<aidlimports::IQtiOemHookResponse>& respCb,
      const std::shared_ptr<aidlimports::IQtiOemHookIndication>& indCb);

  // Utils APIs
  /**
   * Generate OemHookContext
   */
  std::shared_ptr<OemHookContext> getContext(uint32_t serial);

  /**
   * Set instance id
   */
  void setInstanceId(qcril_instance_id_e_type instId);

  /**
   * Returns the instance id
   */
  qcril_instance_id_e_type getInstanceId();

  std::shared_ptr<aidlimports::IQtiOemHookResponse> getResponseCallback();
  std::shared_ptr<aidlimports::IQtiOemHookIndication> getIndicationCallback();

  void processOemHookRawRequest(int32_t serial, const std::vector<uint8_t>& data);

  bool genericMessageHandler(int32_t serial, const uint8_t* reqData, uint32_t reqDataLen);

  void getAdnRecord(int32_t serial);
  void updateAdnRecord(int32_t serial, uint8_t* data, uint32_t dataLen);
  void setMbnConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getMbnConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getAvailableMbnConfig(int32_t serial);
  void cleanupMbnConfig(int32_t serial);
  void selectMbnConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getMetaInfo(int32_t serial, uint8_t* data, uint32_t dataLen);
  void deactivateMbnConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getQcVersionOfFile(int32_t serial, uint8_t* data, uint32_t dataLen);
  void validateMbnConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getQcVersionOfConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getOemVersionOfFile(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getOemVersionOfConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void activateMbnConfig(int32_t serial, uint8_t* data, uint32_t dataLen);
  void enableModemUpdate(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimGetVoltageStatus(int32_t serial);
  void uimSendCardPowerReq(int32_t serial, uint32_t cmd_id, uint8_t* data, uint32_t dataLen);
  void uimSendPersoReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimGetPersoStatus(int32_t serial);
  void uimSendDePersoReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimGetCardStateReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimGetCardAtrReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimSendApduReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimSwitchSlotReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimGetSlotMapping(int32_t serial);
  void uimSetApduBehaviorReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void uimGetSlotStatus(int32_t serial);
  void uimGetLPATC(int32_t serial);
  void uimSetLPATC(int32_t serial, uint8_t* data, uint32_t dataLen);
  void rejectIncomingCallWithCause21(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getCurrentSetupCalls(int32_t serial, uint8_t* data, uint32_t dataLen);
  void requestSetupAnswer(int32_t serial, uint8_t* data, uint32_t dataLen);
  void startIncrementalScanRequest(int32_t serial);
  void cancelIncrementalScanRequest(int32_t serial);
  void getMaxDataAllowed(int32_t serial);
  void getLPlusLFeatureSupportStatus(int32_t serial);
  void setUiStatus(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getPreferredNetworkBandPref(int32_t serial, uint8_t* data, uint32_t dataLen);
  void setPreferredNetworkBandPref(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getCsgId(int32_t serial);
  void enableEngineerMode(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getSarRevKey(int32_t serial);
  void setSarTransmitPower(int32_t serial, uint8_t* data, uint32_t dataLen);
  void performCsgNeworkScan(int32_t serial, uint8_t* data, uint32_t dataLen);
  void setCsgSystemSelectionPreference(int32_t serial, uint8_t* data, uint32_t dataLen);
  void nvRead(int32_t serial, uint8_t* data, uint32_t dataLen);
  void nvWrite(int32_t serial, uint8_t* data, uint32_t dataLen);
  void setRfmScenarioReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getRfmScenarioReq(int32_t serial, uint8_t* data, uint32_t dataLen);
  void getProvisionedTableRevisionReq(int32_t serial, uint8_t* data, uint32_t dataLen);
};

}  // namespace implementation
}  // namespace qcrilhook
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl

#endif  // __OEMHOOK_STABLE_AIDL_SERVICE_H__
