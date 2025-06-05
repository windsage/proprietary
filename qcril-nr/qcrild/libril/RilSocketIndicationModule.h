/******************************************************************************
#  Copyright (c) 2019-2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "interfaces/nas/RilUnsolSignalStrengthMessage.h"
#include "interfaces/nas/RilUnsolCdmaPrlChangedMessage.h"
#include "interfaces/nas/RilUnsolNetworkStateChangedMessage.h"
#include "interfaces/nas/RilUnsolVoiceRadioTechChangedMessage.h"
#include "interfaces/nas/RilUnsolNitzTimeReceivedMessage.h"
#include "interfaces/nas/RilUnsolUiccSubsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccAppsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolCdmaSubscriptionSourceChangedMessage.h"
#include "interfaces/nas/RilUnsolEmergencyCallbackModeMessage.h"
#include "interfaces/nas/RilUnsolEmergencyListIndMessage.h"
#include "interfaces/nas/RilUnsolRadioStateChangedMessage.h"
#include "interfaces/nas/RilUnsolModemRestartMessage.h"
#include "interfaces/cellinfo/RilUnsolCellInfoListMessage.h"

#include "interfaces/voice/QcRilUnsolAudioStateChangedMessage.h"
#include "interfaces/voice/QcRilUnsolRingbackToneMessage.h"
#include "interfaces/voice/QcRilUnsolCallRingingMessage.h"
#include "interfaces/sms/RilUnsolImsNetworkStateChangedMessage.h"
#include "interfaces/nas/RilUnsolRestrictedStateChangedMessage.h"
#include "interfaces/voice/QcRilUnsolCallStateChangeMessage.h"
#include "interfaces/voice/QcRilUnsolOnUssdMessage.h"
#include "interfaces/voice/QcRilUnsolSuppSvcNotificationMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaCallWaitingMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaOtaProvisionStatusMessage.h"
#include "interfaces/voice/QcRilUnsolCdmaInfoRecordMessage.h"
#include "interfaces/voice/QcRilUnsolSrvccStatusMessage.h"
#include "interfaces/voice/QcRilUnsolSupplementaryServiceMessage.h"
#include <interfaces/voice/QcRilUnsolImsViceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolImsTtyNotificationMessage.h>
#include <interfaces/voice/QcRilUnsolImsHandoverMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolImsSrtpEncryptionStatus.h>
#include <interfaces/voice/QcRilUnsolSuppSvcErrorCodeMessage.h>
#include <interfaces/voice/QcRilUnsolSpeechCodecInfoMessage.h>
#include <interfaces/ims/QcRilUnsolImsPendingMultiLineStatus.h>
#include <interfaces/ims/QcRilUnsolImsMultiIdentityStatusMessage.h>

#include "interfaces/ims/QcRilUnsolImsPreAlertingCallInfo.h"
#include "interfaces/ims/QcRilUnsolImsRegStateMessage.h"
#include "interfaces/ims/QcRilUnsolImsSrvStatusIndication.h"
#include "interfaces/ims/QcRilUnsolImsVopsIndication.h"
#include "interfaces/ims/QcRilUnsolImsRegBlockStatusMessage.h"
#include "interfaces/ims/QcRilUnsolImsMultiSimVoiceCapabilityChanged.h"
#include "interfaces/ims/QcRilUnsolImsWfcRoamingConfigIndication.h"
#include "interfaces/ims/QcRilUnsolImsVoiceInfo.h"
#include "interfaces/ims/QcRilUnsolImsGeoLocationInfo.h"
#include "interfaces/ims/QcRilUnsolImsGeoLocationDataStatus.h"
#include "interfaces/ims/QcRilUnsolImsRttMessage.h"
#include "interfaces/ims/QcRilUnsolImsVowifiCallQuality.h"
#include "interfaces/dms/RilUnsolMaxActiveDataSubsChangedMessage.h"
#include "interfaces/nas/RilUnsolCsgIdChangedMessage.h"
#include "interfaces/nas/RilUnsolEngineerModeMessage.h"
#include "UnSolMessages/RadioDataCallListChangeIndMessage.h"
#include <interfaces/gstk/GstkUnsolIndMsg.h>

#include "interfaces/sms/RilUnsolNewSmsOnSimMessage.h"
#include "interfaces/sms/RilUnsolSimSmsStorageFullMessage.h"
#include "interfaces/sms/RilUnsolNewSmsStatusReportMessage.h"
#include "interfaces/sms/RilUnsolCdmaRuimSmsStorageFullMessage.h"
#include "interfaces/sms/RilUnsolIncoming3Gpp2SMSMessage.h"
#include "interfaces/sms/RilUnsolIncoming3GppSMSMessage.h"
#include "interfaces/sms/RilUnsolNewBroadcastSmsMessage.h"
#include <interfaces/sms/RilUnsolIncomingImsSMSMessage.h>
#include <interfaces/sms/RilUnsolNewImsSmsStatusReportMessage.h>
#include "UnSolMessages/RilPCODataMessage.h"
#include "UnSolMessages/LinkCapIndMessage.h"
#include "UnSolMessages/DataNrIconTypeIndMessage.h"
#include "UnSolMessages/RadioKeepAliveStatusIndMessage.h"
#include "UnSolMessages/SlicingConfigChangedIndMessage.h"
#include <interfaces/voice/QcRilUnsolImsModifyCallMessage.h>
#include <interfaces/voice/QcRilUnsolConfParticipantStatusInfoMessage.h>
#include <interfaces/voice/QcRilUnsolAutoCallRejectionMessage.h>
#include <interfaces/sms/QcRilUnsolMessageWaitingInfoMessage.h>
#include "UnSolMessages/PhysicalConfigStructUpdateMessage.h"
#include "UnSolMessages/CarrierInfoForImsiEncryptionRefreshMessage.h"
#include <interfaces/pbm/pbm.h>
#include <interfaces/pbm/QcRilUnsolAdnInitDoneMessage.h>
#include <interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h>
#include <interfaces/pbm/QcRilUnsolPhonebookRecordsUpdatedMessage.h>
#include <interfaces/uim/UimSimStatusChangedInd.h>
#include <interfaces/sms/RilUnsolStkCCAlphaNotifyMessage.h>
#include <interfaces/uim/UimSimRefreshIndication.h>
#include "UnSolMessages/ThrottledApnTimerExpirationMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"
#include "interfaces/nas/RilUnsolOemNetworkScanMessage.h"
#include "interfaces/nas/RilUnsolSubProvisioningStatusMessage.h"
#include "modules/qmi/LpaQmiUimHttpIndicationMsg.h"
#include <interfaces/uim/UimLpaIndicationMsg.h>
#include "interfaces/lpa/lpa_service_types.h"

class RilSocketIndicationModule: public Module {
    public:
        RilSocketIndicationModule();

        virtual void init() {
            Module::init();
        }

    protected:
        virtual void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);

    private:
        void handleSignalStrengthMessage(std::shared_ptr<RilUnsolSignalStrengthMessage> msg);

        void handleQcRilUnsolAudioStateChangedMessage(
            std::shared_ptr<QcRilUnsolAudioStateChangedMessage> msg);
        void handleRingbackToneMessage(std::shared_ptr<QcRilUnsolRingbackToneMessage> msg);
        void handleCdmaPrlChangedMessage(std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg);
        void handleUiccSubsStatusChangedMessage(
            std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg);
        void handleUiccAppsStatusChangedMessage(
            std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg);
        void handleCdmaSubscriptionSourceChangedMessage(
            std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg);
        void handleEmergencyCallBackModeUnsolMessage(
            std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg);
        void handleEmergencyListIndMessage(
            std::shared_ptr<RilUnsolEmergencyListIndMessage> msg);
        void handleRadioStateChangedMessage(std::shared_ptr<RilUnsolRadioStateChangedMessage> msg);
        void handleModemRestartMessage(std::shared_ptr<RilUnsolModemRestartMessage> msg);
        void handleCellInfoListMessage(std::shared_ptr<RilUnsolCellInfoListMessage> msg);
        void handleCallRingingMessage(std::shared_ptr<QcRilUnsolCallRingingMessage> msg);
        void handleImsNetworkStateChangedMessage(
            std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> msg);
        void handleNetworkStateChangeMessage(
            std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg);
        void handleVoiceRadioTechChangeMessage(
            std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg);
        void handleNitzTimeChangeMessage(
            std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg);
        void handleRestrictedStateChangedMessage(
            std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg);
        void handleGstkUnsolIndMsg(
                std::shared_ptr<GstkUnsolIndMsg> msg);
        void handleUnsolOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg);
        void handleUnsolSuppSvcNotificationMessage(
            std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg);
        void handleUnsolCdmaCallWaitingMessage(
            std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg);
        void handleUnsolCdmaOtaProvisionStatusMessage(
            std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg);
        void handleUnsolCdmaInfoRecordMessage(std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg);
        void handleUnsolSrvccStatusMessage(std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg);
        void handleUnsolSupplementaryServiceMessage(
            std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg);
        void handleUnsolNewSmsOnSimMessage(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg);
        void handleUnsolSimSmsStorageFullMessage(
            std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg);
        void handleUnsolNewSmsStatusReportMessage(
            std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg);
        void handleUnsolCdmaRuimSmsStorageFullMessage(
            std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg);
        void handleUnsolImsRegStateMessage(std::shared_ptr<QcRilUnsolImsRegStateMessage> msg);
        void handleUnsolImsSrvStatusIndication(
            std::shared_ptr<QcRilUnsolImsSrvStatusIndication> msg);
        void handleRadioDataCallListChangeIndMessage(
            std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg);
        void handleIncoming3Gpp2SMSMessage(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg);
        void handleIncoming3GppSMSMessage(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg);
        void handleNewBroadcastSmsMessage(std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg);
        void handleRilPCODataMessage(std::shared_ptr<rildata::RilPCODataMessage> msg);
        void handleLCEDataUpdateMessage(std::shared_ptr<rildata::LinkCapIndMessage> msg);
        void handleDataNrIconTypeIndMessage(std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg);
        void handleRadioKeepAliveStatusIndMessage(
            std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg);
        void handleQcRilUnsolCallStateChangeMessage(
            std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg);
        void handleQcRilUnsolImsModifyCallMessage(
            std::shared_ptr<QcRilUnsolImsModifyCallMessage> msg);
        void handleQcRilUnsolMessageWaitingInfoMessage(
            std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg);
        void handlePhysicalConfigStructUpdateMessage(
            std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg);
        void handleCarrierInfoForImsiMessage(
            std::shared_ptr<rildata::CarrierInfoForImsiEncryptionRefreshMessage> msg);
        void handleQcRilUnsolImsVopsChangedMessage(
            std::shared_ptr<QcRilUnsolImsVopsIndication> msg);
        void handleQcRilUnsolConfParticipantStatusInfoMessage(
            std::shared_ptr<QcRilUnsolConfParticipantStatusInfoMessage> msg);
        void handleQcRilUnsolImsRegBlockStatusMessage(
            std::shared_ptr<QcRilUnsolImsRegBlockStatusMessage> msg);
        void handleQcRilUnsolAutoCallRejectionMessage(
            std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg);
        void handleQcRilUnsolImsWfcRoamingConfigIndication(
            std::shared_ptr<QcRilUnsolImsWfcRoamingConfigIndication> msg);
        void handleQcRilUnsolImsVoiceInfo(std::shared_ptr<QcRilUnsolImsVoiceInfo> msg);
        void handleUnsolImsGeoLocationInfo(std::shared_ptr<QcRilUnsolImsGeoLocationInfo> msg);
        void handleUnsolImsGeoLocationDataStatus(
            std::shared_ptr<QcRilUnsolImsGeoLocationDataStatus> msg);
        void handleUnsolImsRttMessage(std::shared_ptr<QcRilUnsolImsRttMessage> msg);
        void handleUnsolImsVowifiCallQuality(std::shared_ptr<QcRilUnsolImsVowifiCallQuality> msg);
        void handleUnsolIncomingImsSmsMessage(std::shared_ptr<RilUnsolIncomingImsSMSMessage> msg);
        void handleUnsolNewImsSmsStatusReportMessage(
            std::shared_ptr<RilUnsolNewImsSmsStatusReportMessage> msg);
        void handleUnsolImsViceInfoMessage(std::shared_ptr<QcRilUnsolImsViceInfoMessage> msg);
        void handleUnsolImsTtyNotificationMessage(
            std::shared_ptr<QcRilUnsolImsTtyNotificationMessage> msg);
        void handleQcRilUnsolImsMultiSimVoiceCapabilityChanged(
            std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg);
        void handleQcRilUnsolImsHandoverMessage(
            std::shared_ptr<QcRilUnsolImsHandoverMessage> msg);
        void handleQcRilUnsolImsConferenceInfoMessage(
            std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg);
        void handlePendingMultiLineStatus(
            std::shared_ptr<QcRilUnsolImsPendingMultiLineStatus> msg);
        void handleImsMultiIdentityStatusMessage(
            std::shared_ptr<QcRilUnsolImsMultiIdentityStatusMessage> msg);
        void handleUnsolAdnInitDoneMessage(
            std::shared_ptr<QcRilUnsolAdnInitDoneMessage> msg);
        void handleQcRilUnsolAdnRecordsOnSimMessage(
            std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg);
        void handlePhonebookRecordsChangedMessage(
            std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg);
        void handleUimSimStatusChangedInd(
            std::shared_ptr<UimSimStatusChangedInd> msg);
        void handleStkCcAlphaNotify(
            std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg);
        void handleUimSimRefreshIndication(
            std::shared_ptr<UimSimRefreshIndication> msg);
        void handleQcRilUnsolSuppSvcErrorCodeMessage(
            std::shared_ptr<QcRilUnsolSuppSvcErrorCodeMessage> msg);
        void handleQcRilUnsolSpeechCodecInfoMessage(
            std::shared_ptr<QcRilUnsolSpeechCodecInfoMessage> msg);
        void handleRilUnsolMaxActiveDataSubsChangedMessage(
            std::shared_ptr<RilUnsolMaxActiveDataSubsChangedMessage> msg);
        void handleRilUnsolCsgIdChangedMessage(
            std::shared_ptr<RilUnsolCsgIdChangedMessage> msg);
        void handleRilUnsolEngineerModeMessage(
            std::shared_ptr<RilUnsolEngineerModeMessage> msg);
        void handleThrottledApnTimerExpirationMessage(
            std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg);
        void handleUnsolNetworkScanResultMessage(
            std::shared_ptr<RilUnsolNetworkScanResultMessage> msg);
        void handleRilUnsolOemNetworkScanMessage(
            std::shared_ptr<RilUnsolOemNetworkScanMessage> msg);
        void handleRilUnsolSubProvisionStatusMessage(
            std::shared_ptr<RilUnsolSubProvisioningStatusMessage> msg);
        void handleQcRilUnsolImsPreAlertingCallInfo(
            std::shared_ptr<QcRilUnsolImsPreAlertingCallInfo> msg);
        void handleAddProfProgInd(std::shared_ptr<UimLpaIndicationMsg> msg);
        void handleLpaHttpsTxnInd(std::shared_ptr<LpaQmiUimHttpIndicationMsg> msg);
        void handleSlicingConfigChangedInd(std::shared_ptr<rildata::SlicingConfigChangedIndMessage> msg);
        void handleQcRilUnsolImsSrtpEncryptionStatus(
            std::shared_ptr<QcRilUnsolImsSrtpEncryptionStatus> msg);
};
