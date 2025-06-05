/******************************************************************************
#  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef __RADIO_SERVICE_MODULE_H__

#include "hidl_impl/radio_service_base.h"
#include "framework/Module.h"
#include "SsrIndicationMessage.h"
#include "framework/QcrilInitMessage.h"

#include "interfaces/RilAcknowledgeRequestMessage.h"
#include "interfaces/RilUnsolRilConnectedMessage.h"
#include "interfaces/gstk/GstkUnsolIndMsg.h"
#include "interfaces/nas/RilUnsolNetworkStateChangedMessage.h"
#include "interfaces/nas/RilUnsolNitzTimeReceivedMessage.h"
#include "interfaces/nas/RilUnsolVoiceRadioTechChangedMessage.h"
#include "interfaces/nas/RilUnsolSignalStrengthMessage.h"
#include "interfaces/nas/RilUnsolEmergencyCallbackModeMessage.h"
#include "interfaces/nas/RilUnsolRadioCapabilityMessage.h"
#include "interfaces/nas/RilUnsolCdmaPrlChangedMessage.h"
#include "interfaces/nas/RilUnsolRestrictedStateChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccSubsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolRadioStateChangedMessage.h"
#include "interfaces/nas/RilUnsolModemRestartMessage.h"
#include "interfaces/nas/RilUnsolCdmaSubscriptionSourceChangedMessage.h"
#include "interfaces/nas/RilUnsolEmergencyListIndMessage.h"
#include "interfaces/cellinfo/RilUnsolCellInfoListMessage.h"
#include "interfaces/pbm/QcRilUnsolPhonebookRecordsUpdatedMessage.h"
#include "interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h"
#include "interfaces/sms/RilUnsolIncoming3GppSMSMessage.h"
#include "interfaces/sms/RilUnsolIncoming3Gpp2SMSMessage.h"
#include "interfaces/sms/RilUnsolNewSmsStatusReportMessage.h"
#include "interfaces/sms/RilUnsolNewBroadcastSmsMessage.h"
#include "interfaces/sms/RilUnsolStkCCAlphaNotifyMessage.h"
#include "interfaces/sms/RilUnsolImsNetworkStateChangedMessage.h"
#include "interfaces/uim/UimSimStatusChangedInd.h"
#include "interfaces/uim/UimSimRefreshIndication.h"
#include "interfaces/voice/QcRilUnsolCallStateChangeMessage.h"
#include "interfaces/voice/QcRilUnsolCallRingingMessage.h"
#include "interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h"
#include "interfaces/voice/QcRilUnsolSrvccStatusMessage.h"
#include "interfaces/voice/QcRilUnsolRingbackToneMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaOtaProvisionStatusMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h"
#include "interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h"
#include "interfaces/voice/QcRilUnsolOnUssdMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h"
#include "UnSolMessages/ThrottledApnTimerExpirationMessage.h"
#include "UnSolMessages/GetDataCallListResponseIndMessage.h"

class RadioServiceModule : public Module {
 private:
  ::android::sp<RadioServiceBase> mRadioService = nullptr;

  static constexpr unsigned int LTE_EXP_COUNT{3};
  unsigned int lte_req_count{0};
  unsigned int lte_disabled_count{0};
  static constexpr unsigned int NR5G_EXP_COUNT{3};
  unsigned int nr5g_req_count{0};
  unsigned int nr5g_disabled_count{0};

  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleSsrIndicationMessage(std::shared_ptr<SsrIndicationMessage> msg);

  void handleRilUnsolRilConnectedMessage(std::shared_ptr<RilUnsolRilConnectedMessage> msg);

  void handleIncoming3GppSMSMessage(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg);
  void handleIncoming3Gpp2SMSMessage(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg);
  void handleNewSmsOnSimMessage(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg);
  void handleNewSmsStatusReportMessage(std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg);
  void handleNewBroadcastSmsMessage(std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg);
  void handleStkCCAlphaNotifyMessage(std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg);
  void handleCdmaRuimSmsStorageFullMessage(
      std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg);
  void handleSimSmsStorageFullMessage(std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg);
  void handleImsNetworkStateChangedMessage(
      std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> msg);

  void handleQcRilUnsolCallStateChangeMessage(std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg);
  void handleQcRilUnsolCallRingingMessage(std::shared_ptr<QcRilUnsolCallRingingMessage> msg);
  void handleQcRilUnsolSupplementaryServiceMessage(
      std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
  void handleQcRilUnsolSrvccStatusMessage(std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg);
  void handleQcRilUnsolRingbackToneMessage(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg);
  void handleQcRilUnsolCdmaOtaProvisionStatusMessage(
      std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg);
  void handleQcRilUnsolCdmaCallWaitingMessage(std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg);
  void handleQcRilUnsolSuppSvcNotificationMessage(
      std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg);
  void handleQcRilUnsolOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg);
  void handleQcRilUnsolCdmaInfoRecordMessage(std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg);

  void handleUimSimStatusChangedInd(std::shared_ptr<UimSimStatusChangedInd> msg);
  void handleUimSimRefreshIndication(std::shared_ptr<UimSimRefreshIndication> msg);
  void handleGstkUnsolIndMsg(std::shared_ptr<GstkUnsolIndMsg> msg);

  void handleAcknowledgeRequestMessage(std::shared_ptr<RilAcknowledgeRequestMessage> msg);
  void handleNetworkStateChangedMessage(std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg);
  void handleNitzTimeReceivedMessage(std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg);
  void handleVoiceRadioTechChangedMessage(std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg);
  void handleSignalStrengthMessage(std::shared_ptr<RilUnsolSignalStrengthMessage> msg);
  void handleEmergencyCallbackModeMessage(std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg);
  void handlelRadioCapabilityMessage(std::shared_ptr<RilUnsolRadioCapabilityMessage> msg);
  void handleCdmaPrlChangedMessage(std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg);
  void handleRestrictedStateChangedMessage(
      std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg);
  void handleUiccSubsStatusChangedMessage(std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg);
  void handleRadioStateChangedMessage(std::shared_ptr<RilUnsolRadioStateChangedMessage> msg);
  void handleModemRestartMessage(std::shared_ptr<RilUnsolModemRestartMessage> msg);
  void handleCdmaSubscriptionSourceChangedMessage(
      std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg);
  void handleEmergencyListIndMessage(std::shared_ptr<RilUnsolEmergencyListIndMessage> msg);
  void handleCellInfoListMessage(std::shared_ptr<RilUnsolCellInfoListMessage> msg);
  void handleNetworkScanResultMessage(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg);
  void handleUiccAppsStatusChangedMessage(std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg);
  void handleNwRegistrationRejectMessage(std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg);
  void handleCellBarringMessage(std::shared_ptr<RilUnsolCellBarringMessage> msg);
  void handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg);
  void handleDeactivateDataCallRadioResponseIndMessage(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg);
  void handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg);
  void handleCarrierInfoForImsiEncryptionRefreshMessage(std::shared_ptr<Message> msg);
  void handleRadioKeepAliveStatusIndMessage(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg);
  void handleLinkCapIndMessage(std::shared_ptr<rildata::LinkCapIndMessage> msg);
  void handlePCODataMessage(std::shared_ptr<rildata::RilPCODataMessage> msg);
  void handlePhysicalConfigStructUpdateMessage(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg);
  void handleUnthrottleApnMessage(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg);

  void handlePhonebookRecordsChangedMessage(std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg);
  void handleAdnRecordsOnSimMessage(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg);
  void handleGetDataCallListResponseIndMessage(std::shared_ptr<rildata::GetDataCallListResponseIndMessage> msg);
 public:
  RadioServiceModule();
  ~RadioServiceModule();
  void init();

  /**
   * Resets the request and disabled count for LTE.
   * Must reset the counts when:
   *  1) a new telephony client connects.
   *  2) on Sub System Restart
   *  3) Received LTE request count equals expected count
   */
  void resetLteCounts();

  /**
   * Resets the request and disabled count for NR5G.
   * Must reset the counts when:
   *  1) a new telephony client connects.
   *  2) on Sub System Restart
   *  3) Received NR5G request count equals expected count
   */
  void reset5gnrCounts();

  /**
   * Increment the request count when a call to
   * setSignalStrengthReportingCriteria_1_5 is made to enable/disable
   * a signal measurement type for LTE.
   */
  void incLteReqCnt();

  /**
   * Increment the disabled count when a call to
   * setSignalStrengthReportingCriteria_1_5 is made to disable a
   * signal measurement type for LTE.
   */
  void incLteDisCnt();

  /**
   * Increment the request count when a call to
   * setSignalStrengthReportingCriteria_1_5 is made to enable/disable
   * a signal measurement type for NR5G.
   */
  void incNr5gReqCnt();

  /**
   * Increment the disabled count when a call to
   * setSignalStrengthReportingCriteria_1_5 is made to disable a
   * signal measurement type for NR5G.
   */
  void incNr5gDisCnt();

  /**
   * Checks if all signal types for LTE are disabled. If all disabled,
   * resets the request and disabled counts for LTE.
   *
   * @return true if all signal types are disabled, else false.
   */
  bool checkLteDisCnt();

  /**
   * Checks if all signal types for NR5G are disabled. If all disabled,
   * resets the request and disabled counts for NR5G.
   *
   * @return true if all signal types are disabled, else false.
   */
  bool checkNr5gDisCnt();

  /**
   * check if all requests for LTE are received and reset request and
   * disable count.
   */
  void checkLteReqCnt();

  /**
   * check if all requests for NR5G are received and reset request and
   * disable count.
   */
  void checkNr5gReqCnt();
};

RadioServiceModule &getRadioServiceModule();

#endif  // __RADIO_SERVICE_MODULE_H__
