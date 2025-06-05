/******************************************************************************
#  Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RilSocketIndicationModule"

#include <framework/Log.h>
#include <telephony/ril.h>
#include <telephony/ril_ims.h>
#include <telephony/ril_call.h>
#include <Marshal.h>
#include <marshal/SignalStrength.h>
#include <marshal/NetworkScan.h>
#include <ril_socket_api.h>
#include <interfaces/gstk/GstkUnsolIndMsg.h>
#include "RilSocketIndicationModule.h"
#include "ril_utils.h"
#include <interfaces/voice/voice.h>
#include <interfaces/voice/QcRilUnsolCallStateChangeMessage.h>
#include <interfaces/voice/QcRilUnsolImsHandoverMessage.h>
#include <interfaces/voice/QcRilUnsolImsConferenceInfoMessage.h>
#include <interfaces/voice/QcRilUnsolImsSrtpEncryptionStatus.h>
#include <interfaces/ims/QcRilUnsolImsPendingMultiLineStatus.h>
#include <interfaces/ims/QcRilUnsolImsMultiIdentityStatusMessage.h>
#include <interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h>
#include "qcril_qmi_oem_reqlist.h"
#include "qcril_qmi_oemhook_utils.h"
#include "ril_socket.h"
#include "interfaces/nas/RilUnsolOemNetworkScanMessage.h"

#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/UimHttpModemEndPoint.h"
#include "modules/qmi/QmiUimHttpSetupRequestCallback.h"
#include "qcril_legacy_apis.h"
static load_module<RilSocketIndicationModule> rilSocketIndicationModule;

RilSocketIndicationModule::RilSocketIndicationModule() {
    mName = "RilSocketIndicationModule";

    using std::placeholders::_1;
    mMessageHandler = {
      HANDLER(QcrilInitMessage, RilSocketIndicationModule::handleQcrilInit),
      HANDLER(RilUnsolSignalStrengthMessage, RilSocketIndicationModule::handleSignalStrengthMessage),
      HANDLER(QcRilUnsolRingbackToneMessage, RilSocketIndicationModule::handleRingbackToneMessage),
      HANDLER(RilUnsolEmergencyCallbackModeMessage,
              RilSocketIndicationModule::handleEmergencyCallBackModeUnsolMessage),
      HANDLER(RilUnsolEmergencyListIndMessage,
              RilSocketIndicationModule::handleEmergencyListIndMessage),
      HANDLER(RilUnsolRadioStateChangedMessage,
              RilSocketIndicationModule::handleRadioStateChangedMessage),
      HANDLER(RilUnsolModemRestartMessage, RilSocketIndicationModule::handleModemRestartMessage),
      HANDLER(RilUnsolCellInfoListMessage, RilSocketIndicationModule::handleCellInfoListMessage),
      HANDLER(RilUnsolCdmaPrlChangedMessage, RilSocketIndicationModule::handleCdmaPrlChangedMessage),
      HANDLER(RilUnsolUiccSubsStatusChangedMessage,
              RilSocketIndicationModule::handleUiccSubsStatusChangedMessage),
      HANDLER(RilUnsolCdmaSubscriptionSourceChangedMessage,
              RilSocketIndicationModule::handleCdmaSubscriptionSourceChangedMessage),
      HANDLER(RilUnsolUiccAppsStatusChangedMessage,
              RilSocketIndicationModule::handleUiccAppsStatusChangedMessage),
      HANDLER(RilUnsolImsNetworkStateChangedMessage,
              RilSocketIndicationModule::handleImsNetworkStateChangedMessage),
      HANDLER(RilUnsolNetworkStateChangedMessage,
              RilSocketIndicationModule::handleNetworkStateChangeMessage),
      HANDLER(RilUnsolVoiceRadioTechChangedMessage,
              RilSocketIndicationModule::handleVoiceRadioTechChangeMessage),
      HANDLER(RilUnsolNitzTimeReceivedMessage,
              RilSocketIndicationModule::handleNitzTimeChangeMessage),
      HANDLER(RilUnsolRestrictedStateChangedMessage,
              RilSocketIndicationModule::handleRestrictedStateChangedMessage),
      HANDLER(QcRilUnsolCallRingingMessage, RilSocketIndicationModule::handleCallRingingMessage),
      HANDLER(rildata::RadioDataCallListChangeIndMessage,
              RilSocketIndicationModule::handleRadioDataCallListChangeIndMessage),
      HANDLER(GstkUnsolIndMsg, RilSocketIndicationModule::handleGstkUnsolIndMsg),
      HANDLER(QcRilUnsolOnUssdMessage, RilSocketIndicationModule::handleUnsolOnUssdMessage),
      HANDLER(QcRilUnsolSuppSvcNotificationMessage,
              RilSocketIndicationModule::handleUnsolSuppSvcNotificationMessage),
      HANDLER(QcRilUnsolCdmaCallWaitingMessage,
              RilSocketIndicationModule::handleUnsolCdmaCallWaitingMessage),
      HANDLER(QcRilUnsolCdmaOtaProvisionStatusMessage,
              RilSocketIndicationModule::handleUnsolCdmaOtaProvisionStatusMessage),
      HANDLER(QcRilUnsolCdmaInfoRecordMessage,
              RilSocketIndicationModule::handleUnsolCdmaInfoRecordMessage),
      HANDLER(QcRilUnsolSrvccStatusMessage,
              RilSocketIndicationModule::handleUnsolSrvccStatusMessage),
      HANDLER(QcRilUnsolSupplementaryServiceMessage,
              RilSocketIndicationModule::handleUnsolSupplementaryServiceMessage),
      HANDLER(RilUnsolNewSmsOnSimMessage, RilSocketIndicationModule::handleUnsolNewSmsOnSimMessage),
      HANDLER(RilUnsolSimSmsStorageFullMessage,
              RilSocketIndicationModule::handleUnsolSimSmsStorageFullMessage),
      HANDLER(RilUnsolNewSmsStatusReportMessage,
              RilSocketIndicationModule::handleUnsolNewSmsStatusReportMessage),
      HANDLER(RilUnsolCdmaRuimSmsStorageFullMessage,
              RilSocketIndicationModule::handleUnsolCdmaRuimSmsStorageFullMessage),
      HANDLER(QcRilUnsolImsRegStateMessage,
              RilSocketIndicationModule::handleUnsolImsRegStateMessage),
      HANDLER(QcRilUnsolImsSrvStatusIndication,
              RilSocketIndicationModule::handleUnsolImsSrvStatusIndication),
      HANDLER(QcRilUnsolImsHandoverMessage,
              RilSocketIndicationModule::handleQcRilUnsolImsHandoverMessage),
      HANDLER(RilUnsolIncoming3Gpp2SMSMessage,
              RilSocketIndicationModule::handleIncoming3Gpp2SMSMessage),
      HANDLER(RilUnsolIncoming3GppSMSMessage,
              RilSocketIndicationModule::handleIncoming3GppSMSMessage),
      HANDLER(RilUnsolNewBroadcastSmsMessage,
              RilSocketIndicationModule::handleNewBroadcastSmsMessage),
      HANDLER(rildata::RilPCODataMessage, RilSocketIndicationModule::handleRilPCODataMessage),
      HANDLER(rildata::LinkCapIndMessage, RilSocketIndicationModule::handleLCEDataUpdateMessage),
      HANDLER(rildata::DataNrIconTypeIndMessage,
              RilSocketIndicationModule::handleDataNrIconTypeIndMessage),
      HANDLER(rildata::RadioKeepAliveStatusIndMessage,
              RilSocketIndicationModule::handleRadioKeepAliveStatusIndMessage),
      HANDLER(QcRilUnsolCallStateChangeMessage,
              RilSocketIndicationModule::handleQcRilUnsolCallStateChangeMessage),
      HANDLER(QcRilUnsolImsModifyCallMessage,
              RilSocketIndicationModule::handleQcRilUnsolImsModifyCallMessage),
      HANDLER(QcRilUnsolMessageWaitingInfoMessage,
              RilSocketIndicationModule::handleQcRilUnsolMessageWaitingInfoMessage),
      HANDLER(rildata::PhysicalConfigStructUpdateMessage,
              RilSocketIndicationModule::handlePhysicalConfigStructUpdateMessage),
      HANDLER(rildata::CarrierInfoForImsiEncryptionRefreshMessage,
              RilSocketIndicationModule::handleCarrierInfoForImsiMessage),
      HANDLER(rildata::ThrottledApnTimerExpirationMessage,
              RilSocketIndicationModule::handleThrottledApnTimerExpirationMessage),
      HANDLER(QcRilUnsolImsVopsIndication,
              RilSocketIndicationModule::handleQcRilUnsolImsVopsChangedMessage),
      HANDLER(QcRilUnsolConfParticipantStatusInfoMessage,
              RilSocketIndicationModule::handleQcRilUnsolConfParticipantStatusInfoMessage),
      HANDLER(QcRilUnsolImsRegBlockStatusMessage,
              RilSocketIndicationModule::handleQcRilUnsolImsRegBlockStatusMessage),
      HANDLER(QcRilUnsolAutoCallRejectionMessage,
              RilSocketIndicationModule::handleQcRilUnsolAutoCallRejectionMessage),
      HANDLER(QcRilUnsolImsWfcRoamingConfigIndication,
              RilSocketIndicationModule::handleQcRilUnsolImsWfcRoamingConfigIndication),
      HANDLER(QcRilUnsolImsVoiceInfo, RilSocketIndicationModule::handleQcRilUnsolImsVoiceInfo),
      HANDLER(QcRilUnsolImsGeoLocationInfo,
              RilSocketIndicationModule::handleUnsolImsGeoLocationInfo),
      HANDLER(QcRilUnsolImsGeoLocationDataStatus,
              RilSocketIndicationModule::handleUnsolImsGeoLocationDataStatus),
      HANDLER(QcRilUnsolImsRttMessage, RilSocketIndicationModule::handleUnsolImsRttMessage),
      HANDLER(QcRilUnsolImsVowifiCallQuality,
              RilSocketIndicationModule::handleUnsolImsVowifiCallQuality),
      HANDLER(RilUnsolIncomingImsSMSMessage,
              RilSocketIndicationModule::handleUnsolIncomingImsSmsMessage),
      HANDLER(RilUnsolNewImsSmsStatusReportMessage,
              RilSocketIndicationModule::handleUnsolNewImsSmsStatusReportMessage),
      HANDLER(QcRilUnsolImsViceInfoMessage,
              RilSocketIndicationModule::handleUnsolImsViceInfoMessage),
      HANDLER(QcRilUnsolImsTtyNotificationMessage,
              RilSocketIndicationModule::handleUnsolImsTtyNotificationMessage),
      HANDLER(QcRilUnsolAudioStateChangedMessage,
              RilSocketIndicationModule::handleQcRilUnsolAudioStateChangedMessage),
      HANDLER(QcRilUnsolAdnInitDoneMessage,
              RilSocketIndicationModule::handleUnsolAdnInitDoneMessage),
      HANDLER(UimSimStatusChangedInd, RilSocketIndicationModule::handleUimSimStatusChangedInd),
      HANDLER(UimSimRefreshIndication, RilSocketIndicationModule::handleUimSimRefreshIndication),
      HANDLER(QcRilUnsolSuppSvcErrorCodeMessage,
              RilSocketIndicationModule::handleQcRilUnsolSuppSvcErrorCodeMessage),
      HANDLER(QcRilUnsolPhonebookRecordsUpdatedMessage,
              RilSocketIndicationModule::handlePhonebookRecordsChangedMessage),
      HANDLER(RilUnsolNetworkScanResultMessage,
              RilSocketIndicationModule::handleUnsolNetworkScanResultMessage),
      HANDLER(QcRilUnsolImsMultiSimVoiceCapabilityChanged,
              RilSocketIndicationModule::handleQcRilUnsolImsMultiSimVoiceCapabilityChanged),
      HANDLER(QcRilUnsolSpeechCodecInfoMessage,
              RilSocketIndicationModule::handleQcRilUnsolSpeechCodecInfoMessage),
      HANDLER(RilUnsolMaxActiveDataSubsChangedMessage,
              RilSocketIndicationModule::handleRilUnsolMaxActiveDataSubsChangedMessage),
      HANDLER(RilUnsolCsgIdChangedMessage,
              RilSocketIndicationModule::handleRilUnsolCsgIdChangedMessage),
      HANDLER(RilUnsolEngineerModeMessage,
              RilSocketIndicationModule::handleRilUnsolEngineerModeMessage),
      HANDLER(RilUnsolOemNetworkScanMessage,
              RilSocketIndicationModule::handleRilUnsolOemNetworkScanMessage),
      HANDLER(RilUnsolSubProvisioningStatusMessage,
              RilSocketIndicationModule::handleRilUnsolSubProvisionStatusMessage),
      HANDLER(QcRilUnsolImsPreAlertingCallInfo,
              RilSocketIndicationModule::handleQcRilUnsolImsPreAlertingCallInfo),
      HANDLER(UimLpaIndicationMsg, RilSocketIndicationModule::handleAddProfProgInd),
      HANDLER(LpaQmiUimHttpIndicationMsg, RilSocketIndicationModule::handleLpaHttpsTxnInd),
      HANDLER(rildata::SlicingConfigChangedIndMessage, RilSocketIndicationModule::handleSlicingConfigChangedInd),
      HANDLER(QcRilUnsolImsConferenceInfoMessage, RilSocketIndicationModule::handleQcRilUnsolImsConferenceInfoMessage),
      HANDLER(QcRilUnsolImsSrtpEncryptionStatus,
              RilSocketIndicationModule::handleQcRilUnsolImsSrtpEncryptionStatus)
    };
}

void RilSocketIndicationModule::handleUnsolNetworkScanResultMessage(
         std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    return;
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p != nullptr) {
    Marshal::Result writeResp = Marshal::Result::SUCCESS;
    p->write(static_cast<uint64_t>(msg->getStatus()));
    auto networkList = msg->getNetworkInfo();
    auto num = networkList.size();
    p->write(static_cast<uint64_t>(num));
    for(int i=0;i<num;i++) {
      writeResp = p->write(networkList[i]);
      if (writeResp != Marshal::Result::SUCCESS) {
        break;
      }
    }
    p->write(static_cast<uint64_t>(msg->getError()));
    if (writeResp == Marshal::Result::SUCCESS) {
      int ret = sendUnsolResponse(RIL_UNSOL_NETWORK_SCAN_RESULT, p);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_NETWORK_SCAN_RESULT: %d", ret);
      }
    }
  }
  else {
      QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
  }
  QCRIL_LOG_FUNC_RETURN();

}

void RilSocketIndicationModule::handleRadioKeepAliveStatusIndMessage(
    std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg) {
    QCRIL_LOG_INFO("RilSocketIndicationModule::handleRadioKeepAliveStatusIndMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    int ret = -1;
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    int val = -1;
    int code_marshal = -1;
    if (p != nullptr) {
        val = static_cast<int>(msg->getHandle());
        code_marshal = static_cast<int>(msg->getStatusCode());
        p->write(val);
        p->write(code_marshal);
    }
    else {
        QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
    }
    ret = sendUnsolResponse(RIL_UNSOL_KEEPALIVE_STATUS, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_KEEPALIVE_STATUS: %d", ret);
    }
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleCarrierInfoForImsiMessage(
    std::shared_ptr<rildata::CarrierInfoForImsiEncryptionRefreshMessage> msg) {
    QCRIL_LOG_INFO("RilSocketIndicationModule::handlehandleCarrierInfoForImsiMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    int ret = -1;
    ret = sendUnsolResponse(RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION, nullptr);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION: %d", ret);
    }
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleThrottledApnTimerExpirationMessage(
    std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg) {
  if (!msg || msg->getSrc() == rildata::RequestSource_t::IWLAN) {
      return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int ret = -1;
  auto p = std::make_shared<Marshal>();
  if (p) {
    if ( p->write(msg->getApn()) == Marshal::Result::FAILURE) {
      QCRIL_LOG_INFO("UnthrottleApnIndication could not be sent to Client. Marshal Write Failed.");
      ret = sendUnsolResponse(RIL_UNSOL_UNTHROTTLE_APN, nullptr);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_UNTHROTTLE_APN: %d", ret);
      }
    } else {
      ret = sendUnsolResponse(RIL_UNSOL_UNTHROTTLE_APN, p);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_UNTHROTTLE_APN: %d", ret);
      }
    }
  }
  ret = -1;
  auto parcel = std::make_shared<Marshal>();
  if (parcel) {
    RIL_DataProfileInfo info = {};
    rildata::DataProfileInfo_t profile = msg->getProfileInfo();
    ril::socket::utils::convertToSocket(info, profile);
    if ( parcel->write(info) == Marshal::Result::FAILURE) {
      QCRIL_LOG_INFO("UnthrottleApnIndication could not be sent to Client. Marshal Write Failed.");
      ret = sendUnsolResponse(RIL_UNSOL_UNTHROTTLE_APN_TD, nullptr);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_UNTHROTTLE_APN_TD: %d", ret);
      }
    } else {
      ret = sendUnsolResponse(RIL_UNSOL_UNTHROTTLE_APN_TD, parcel);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_UNTHROTTLE_APN_TD: %d", ret);
      }
    }
  }
}

void RilSocketIndicationModule::handlePhysicalConfigStructUpdateMessage(
    std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg) {
    QCRIL_LOG_INFO("RilSocketIndicationModule::handlePhysicalConfigStructUpdateMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    int ret = -1;
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
        std::vector<rildata::PhysicalConfigStructInfo_t> physconfig = msg->getPhysicalConfigStructInfo();
        if ( p->write(static_cast<int>(physconfig.size())) == Marshal::Result::FAILURE) {
            QCRIL_LOG_INFO("PhysicalConfigStructUpdate Response could not be sent to Client. Marshal Write Failed.");
            ret = sendUnsolResponse(RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG, nullptr);
            if (ret != 0) {
                QCRIL_LOG_INFO("Failed to send RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG: %d", ret);
            }
            return;
        }
        for (int i = 0; i < physconfig.size(); i++) {
            RIL_PhysicalChannelConfig data = {};
            data.status = static_cast<RIL_CellConnectionStatus>(physconfig[i].status);
            data.cellBandwidthDownlink = physconfig[i].cellBandwidthDownlink;
            data.rat = static_cast<RIL_RadioTechnology>(physconfig[i].rat);
            data.rfInfo.range = static_cast<RIL_FrequencyRange> (physconfig[i].rfInfo.range);
            data.rfInfo.channelNumber = physconfig[i].rfInfo.channelNumber;
            data.num_context_ids = physconfig[i].contextIds.size();
            for (int j = 0; j < physconfig[i].contextIds.size(); j++) {
                data.contextIds[j] = physconfig[i].contextIds[j];
            }
            data.physicalCellId = physconfig[i].physicalCellId;
            if (p->write(data) == Marshal::Result::FAILURE) {
                QCRIL_LOG_INFO("PhysicalConfigStructUpdate Response could not be sent to Client. Marshal Write Failed.");
                ret = sendUnsolResponse(RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG, nullptr);
                if (ret != 0) {
                    QCRIL_LOG_INFO("Failed to send RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG: %d", ret);
                }
                return;
            }
            p->release(data);
        }
    }
    else {
        QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
    }
    ret = sendUnsolResponse(RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG: %d", ret);
    }
    QCRIL_LOG_FUNC_RETURN();
}


void RilSocketIndicationModule::handleDataNrIconTypeIndMessage(
    std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg) {
    QCRIL_LOG_INFO("RilSocketIndicationModule::handleDataNrIconTypeIndMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    five_g_icon_type rilIconType = five_g_icon_type::FIVE_G_ICON_TYPE_INVALID;
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    int ret = -1;
    if (p != nullptr) {
        int val;
        if(msg->isNone()){
            rilIconType = FIVE_G_ICON_TYPE_NONE;
        } else if(msg->isBasic()) {
            rilIconType = FIVE_G_ICON_TYPE_BASIC;
        } else if(msg->isUwb()) {
            rilIconType = FIVE_G_ICON_TYPE_UWB;
        } else {
            rilIconType = FIVE_G_ICON_TYPE_INVALID;
        }
        val = static_cast<int>(rilIconType);
        p->write(val);
    }
    else {
        QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
    }
    ret = sendUnsolResponse(RIL_UNSOL_DATA_NR_ICON_TYPE, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_DATA_NR_ICON_TYPE: %d", ret);
    }
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleLCEDataUpdateMessage(
    std::shared_ptr<rildata::LinkCapIndMessage> msg ) {
    QCRIL_LOG_INFO("RilSocketIndicationModule:: LinkCapIndMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    rildata::LinkCapEstimate_t lceData = msg->getParams();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    RIL_LinkCapacityEstimate data = {};
    int ret = -1;
    if (p != nullptr) {
        QCRIL_LOG_INFO("Downlink capacity in kbps = %d", lceData.downlinkCapacityKbps);
        QCRIL_LOG_INFO("Uplink capacity in kbps = %d", lceData.uplinkCapacityKbps);
        data.downlinkCapacityKbps = lceData.downlinkCapacityKbps;
        data.uplinkCapacityKbps = lceData.uplinkCapacityKbps;
        p->write(data);
    }
    else {
        QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
    }
    ret = sendUnsolResponse(RIL_UNSOL_LCEDATA_RECV, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_LCEDATA_RECV: %d", ret);
    }
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleRilPCODataMessage(
    std::shared_ptr<rildata::RilPCODataMessage> msg ) {
    QCRIL_LOG_INFO("RilSocketIndicationModule:: handleRilPCODataMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    rildata::PcoDataInfo_t PcoData = msg->getPcoDataInfo();
    RIL_PCO_Data data = {};
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    int ret = -1;
    if (p != nullptr) {
        QCRIL_LOG_INFO("RilSocketIndicationModule:: retrieving pco data");
        QCRIL_LOG_INFO("RilSocketIndicationModule:: cid = %d", PcoData.cid);
        QCRIL_LOG_INFO("RilSocketIndicationModule:: BearerProtocol = %s", PcoData.bearerProto.c_str());
        QCRIL_LOG_INFO("RilSocketIndicationModule:: PcoId = %d", PcoData.pcoId);
        for (int i = 0; i < PcoData.contents.size(); i++) {
            QCRIL_LOG_INFO("RilSocketIndicationModule:: content %d = %d",i,PcoData.contents[i]);
        }
        data.cid = PcoData.cid;

        data.bearer_proto = new char[PcoData.bearerProto.length() + 1]{};
        if (data.bearer_proto != NULL) {
            strlcpy(data.bearer_proto, PcoData.bearerProto.c_str(),PcoData.bearerProto.length() + 1);
        }
        else {
            QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
        }
        data.pco_id = PcoData.pcoId;

        data.contents_length = PcoData.contents.size();

        data.contents = new char[PcoData.contents.size()]{};
        if (data.contents != NULL) {
            for (int i = 0; i < PcoData.contents.size(); i++) {
                data.contents[i] = static_cast<char>(PcoData.contents[i]);
            }
        }
        else {
            QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
        }
        p->write(data);
        p->release(data);
    }
    else {
        QCRIL_LOG_INFO("RilSocketIndicationModule:: Device has no memory");
    }
    ret = sendUnsolResponse(RIL_UNSOL_PCO_DATA, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_PCO_DATA: %d", ret);
    }
    QCRIL_LOG_FUNC_RETURN();
}


void RilSocketIndicationModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
    if (!msg) {
        QCRIL_LOG_ERROR("Received null message!");
        return;
    }
    QCRIL_LOG_INFO("Handling QcrilInitMessage %s", msg->dump().c_str());
    socket_start(msg->get_instance_id());
#ifdef RIL_FOR_MDM_LE
    /** Registering UimHttp Client for Sending/Receiving Http transaction for LpaModule*/
    QmiUimHttpSetupRequestCallback qmiSetupCallback("Uim-Module-Token");
    ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().
               buildEndPoint()->requestSetup("Client-Uim-http-Serv-Token",
                       qmi_ril_get_process_instance_id(),&qmiSetupCallback);
#endif
}

void RilSocketIndicationModule::handleSignalStrengthMessage(
        std::shared_ptr<RilUnsolSignalStrengthMessage> msg) {
    if (msg == nullptr) {
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
        p->write(msg->getSignalStrength());
    }

    int ret = sendUnsolResponse(RIL_UNSOL_SIGNAL_STRENGTH, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_SIGNAL_STRENGTH: %d", ret);
    }
}

void RilSocketIndicationModule::handleCdmaPrlChangedMessage(
    std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg == nullptr) {
        return;
    }

    int prl = msg->getPrl();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr &&
        p->write(prl) == Marshal::Result::SUCCESS) {
        int ret = sendUnsolResponse(RIL_UNSOL_CDMA_PRL_CHANGED, p);
        if (ret != 0) {
            QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CDMA_PRL_CHANGED: %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUiccSubsStatusChangedMessage(
    std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg == nullptr) {
        return;
    }

    int status = msg->getStatus();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr &&
        p->write(status) == Marshal::Result::SUCCESS) {
        int ret = sendUnsolResponse(RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED, p);
        if (ret != 0) {
            QCRIL_LOG_INFO("Failed to send RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleCdmaSubscriptionSourceChangedMessage(
    std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg == nullptr) {
        return;
    }

    RIL_CdmaSubscriptionSource source = msg->getSource();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr &&
        p->write(source) == Marshal::Result::SUCCESS) {
        int ret = sendUnsolResponse(RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED, p);
        if (ret != 0) {
            QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUiccAppsStatusChangedMessage (
    std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg == nullptr) {
        return;
    }

    bool status = msg->getStatus();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr &&
        p->write(status) == Marshal::Result::SUCCESS) {
        int ret = sendUnsolResponse(RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED, p);
        if (ret != 0) {
            QCRIL_LOG_INFO("Failed to send RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED: %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleEmergencyCallBackModeUnsolMessage(
    std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg == nullptr || (msg->getEmergencyCallbackMode() ==
                           RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::INVALID)) {
      return;
    }
    int unsol_req_id = (msg->getEmergencyCallbackMode() ==
                        RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::ENTER)
                           ? RIL_CALL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE
                           : RIL_CALL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE;

    int ret = sendUnsolResponse(unsol_req_id, nullptr);
    if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send UNSOL EMERGENCY_CALLBACK_MODE: %d", ret);
    }

    // FR76398: DEPRECTED: START
    // Remove in the next release
    if (!msg->isEmergencyIp()) {
      unsol_req_id = (msg->getEmergencyCallbackMode() ==
                      RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::ENTER)
                         ? RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE
                         : RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE;
    } else {
      unsol_req_id = (msg->getEmergencyCallbackMode() ==
               RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::ENTER)
                  ? RIL_IMS_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE
                  : RIL_IMS_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE;
    }
    ret = sendUnsolResponse(unsol_req_id, nullptr);
    if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send UNSOL EMERGENCY_CALLBACK_MODE: %d", ret);
    }
    // FR76398: DEPRECTED: END

    QCRIL_LOG_FUNC_RETURN();
}

void convertToSocketFormat(uint64_t in, uint32_t &out) {
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::UNKNOWN)) {
        out |= RIL_EMERGENCY_CATEGORY_UNSPECIFIED;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::POLICE)) {
        out |= RIL_EMERGENCY_CATEGORY_POLICE;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::AMBULANCE)) {
        out |= RIL_EMERGENCY_CATEGORY_AMBULANCE;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::FIRE_BRIGADE)) {
        out |= RIL_EMERGENCY_CATEGORY_FIRE_BRIGADE;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::MARINE_GUARD)) {
        out |= RIL_EMERGENCY_CATEGORY_MARINE_GUARD;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::MOUNTAIN_RESCUE)) {
        out |= RIL_EMERGENCY_CATEGORY_MOUNTAIN_RESCUE;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::MANUAL_ECALL)) {
        out |= RIL_EMERGENCY_CATEGORY_MIEC;
    }
    if(in & static_cast<uint64_t>(qcril::interfaces::RIL_EccCategoryMask::AUTO_ECALL)) {
        out |= RIL_EMERGENCY_CATEGORY_AIEC;
    }
}

void RilSocketIndicationModule::handleEmergencyListIndMessage(
    std::shared_ptr<RilUnsolEmergencyListIndMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();
    if (msg == nullptr) {
        return;
    }

    RIL_EmergencyList list {};
    qcril::interfaces::RIL_EccList ecclist = msg->getEmergencyList();
    list.emergency_list_length = std::min<size_t>(ecclist.size(), MAX_EMERGENCY_NUMBERS);
    for(int i=0; i < list.emergency_list_length; i++) {
        strlcpy(list.emergency_numbers[i].number,
            ecclist[i]->number.c_str(),
            MAX_EMERGENCY_NUMBER_LENGTH);
        strlcpy(list.emergency_numbers[i].mcc,
             ecclist[i]->mcc.c_str(),
             MAX_RIL_MCC_MNC_SIZE);
        strlcpy(list.emergency_numbers[i].mnc,
             ecclist[i]->mnc.c_str(),
             MAX_RIL_MCC_MNC_SIZE);
        convertToSocketFormat(ecclist[i]->categories, list.emergency_numbers[i].category);
        list.emergency_numbers[i].sources =
            static_cast<RIL_EmergencyNumberSource>(ecclist[i]->sources);
        for(int uentry=0; uentry < ecclist[i]->urns.size(); uentry++) {
            strlcpy(list.emergency_numbers[i].urn[uentry],
                ecclist[i]->urns[uentry].c_str(),
                MAX_URN_LENGTH);
        }
    }

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr &&
        p->write(list) == Marshal::Result::SUCCESS) {
        int ret = sendUnsolResponse(RIL_UNSOL_EMERGENCY_NUMBERS_LIST, p);
        if (ret != 0) {
            QCRIL_LOG_INFO("Failed to send RIL_UNSOL_EMERGENCY_NUMBERS_LIST: %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleRadioStateChangedMessage(
    std::shared_ptr<RilUnsolRadioStateChangedMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (msg && p) {
    RIL_RadioState state = ril::socket::api::getRadioState();
    Marshal::Result writeResp = p->write(static_cast<int>(state));
    if (writeResp == Marshal::Result::SUCCESS) {
      int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, p);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: %d", ret);
      }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleModemRestartMessage(
    std::shared_ptr<RilUnsolModemRestartMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (msg && p) {
    //std::string reason{"Modem Restart"};
    Marshal::Result writeResp = p->write(msg->getReason());
    if (writeResp == Marshal::Result::SUCCESS) {
      int ret = sendUnsolResponse(RIL_UNSOL_MODEM_RESTART, p);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_MODEM_RESTART: %d", ret);
      }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleCellInfoListMessage(
    std::shared_ptr<RilUnsolCellInfoListMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (msg && p) {
    auto cellInfoList = msg->getCellInfoList();
    if (cellInfoList.size()) {
      Marshal::Result writeResp = p->write(static_cast<uint64_t>(cellInfoList.size()));
      if (writeResp == Marshal::Result::SUCCESS) {
        for (auto& cellInfo : cellInfoList) {
          writeResp = p->write(cellInfo);
          if (writeResp != Marshal::Result::SUCCESS) {
            break;
          }
        }
      }
      if (writeResp == Marshal::Result::SUCCESS) {
        int ret = sendUnsolResponse(RIL_UNSOL_CELL_INFO_LIST, p);
        if (ret != 0) {
          QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CELL_INFO_LIST: %d", ret);
        }
      }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleRingbackToneMessage(
    std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  if (!msg->hasRingBackToneOperation() ||
      msg->getRingBackToneOperation() == qcril::interfaces::RingBackToneOperation::UNKNOWN) {
    QCRIL_LOG_ERROR("Invalid data in msg");
    return;
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }

  int unsol_msg = RIL_CALL_UNSOL_RINGBACK_TONE;
  RIL_ToneOperation operation = RIL_TONE_OP_STOP;
  if (msg->getRingBackToneOperation() == qcril::interfaces::RingBackToneOperation::START) {
    operation = RIL_TONE_OP_START;
  }
  p->write(operation);

  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send %s: %d", "RIL_CALL_UNSOL_RINGBACK_TONE", ret);
  }

  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
  if (msg->isIms()) {
    unsol_msg = RIL_IMS_UNSOL_RINGBACK_TONE;
    RIL_IMS_ToneOperation operation = RIL_IMS_TONE_OP_STOP;
    if (msg->getRingBackToneOperation() == qcril::interfaces::RingBackToneOperation::START) {
      operation = RIL_IMS_TONE_OP_START;
    }
    p->write(operation);
  } else {
    unsol_msg = RIL_UNSOL_RINGBACK_TONE;
    int operation = 0;
    if (msg->getRingBackToneOperation() == qcril::interfaces::RingBackToneOperation::START) {
      operation = 1;
    }
    p->write(operation);
  }

  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send %s: %d",
                   (unsol_msg == RIL_UNSOL_RINGBACK_TONE ? "RIL_UNSOL_RINGBACK_TONE"
                                                         : "RIL_IMS_UNSOL_RINGBACK_TONE"),
                   ret);
  }
  }  // FR76398: DEPRECTED: END

  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleImsNetworkStateChangedMessage(
    std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> /*msg*/) {
    QCRIL_LOG_FUNC_ENTRY();

    int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED, nullptr);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: %d", ret);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleNetworkStateChangeMessage(
        std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg) {
    if (msg == nullptr) {
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, nullptr);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: %d", ret);
    }
}

void RilSocketIndicationModule::handleVoiceRadioTechChangeMessage(
        std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg) {
    if (msg == nullptr) {
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p == nullptr) {
        QCRIL_LOG_INFO("failed to allocate memory");
        return;
    }

    p->write(msg->getVoiceRat());

    int ret = sendUnsolResponse(RIL_UNSOL_VOICE_RADIO_TECH_CHANGED, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: %d", ret);
    }
}

void RilSocketIndicationModule::handleNitzTimeChangeMessage(
        std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg) {
    if (msg == nullptr) {
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p == nullptr) {
        QCRIL_LOG_INFO("failed to allocate memory");
        return;
    }

    p->write(msg->getNitzTime().c_str());

    int ret = sendUnsolResponse(RIL_UNSOL_NITZ_TIME_RECEIVED, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_NITZ_TIME_RECEIVED: %d", ret);
    }
}

void RilSocketIndicationModule::handleRestrictedStateChangedMessage(
    std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg) {
    QCRIL_LOG_FUNC_ENTRY();

    if (msg == nullptr) {
    return;
    }
    int state = msg->getState();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
    p->write(state);
    }
    int ret = sendUnsolResponse(RIL_UNSOL_RESTRICTED_STATE_CHANGED, p);
    if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESTRICTED_STATE_CHANGED: %d", ret);
    }

    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleCallRingingMessage(
    std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }

  int unsol_msg = RIL_CALL_UNSOL_CALL_RING;
  std::shared_ptr<RIL_CDMA_SignalInfoRecord> record = nullptr;
  if (msg->hasCdmaSignalInfoRecord()) {
    record = msg->getCdmaSignalInfoRecord();
  }
  if (record != nullptr) {
    auto marshalResult = p->write(*record);
    if (marshalResult != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to write to marshal");
      return;
    }
  }

  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send %s: %d", "RIL_CALL_UNSOL_CALL_RING", ret);
  }

  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
  if (msg->isIms()) {
    unsol_msg = RIL_IMS_UNSOL_CALL_RING;
  } else {
    unsol_msg = RIL_UNSOL_CALL_RING;
    std::shared_ptr<RIL_CDMA_SignalInfoRecord> record = nullptr;
    if (msg->hasCdmaSignalInfoRecord()) {
      record = msg->getCdmaSignalInfoRecord();
    }
    if (record != nullptr) {
      p->write(*record);
    }
  }
  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO(
        "Failed to send %s: %d",
        (unsol_msg == RIL_UNSOL_CALL_RING ? "RIL_UNSOL_CALL_RING" : "RIL_IMS_UNSOL_CALL_RING"), ret);
  }
  }  // FR76398: DEPRECTED: END

  QCRIL_LOG_FUNC_RETURN();
}

static uint8_t convertUssdModeTypeToInt(qcril::interfaces::UssdModeType in) {
  switch(in) {
    case qcril::interfaces::UssdModeType::NOTIFY:
      return 0;
    case qcril::interfaces::UssdModeType::REQUEST:
      return 1;
    case qcril::interfaces::UssdModeType::NW_RELEASE:
      return 2;
    case qcril::interfaces::UssdModeType::LOCAL_CLIENT:
      return 3;
    case qcril::interfaces::UssdModeType::NOT_SUPPORTED:
      return 4;
    case qcril::interfaces::UssdModeType::NW_TIMEOUT:
      return 5;
    default:
      return 0;
  }
  return 0;
}

static RIL_UssdModeType convertUssdModeType(qcril::interfaces::UssdModeType in) {
  switch(in) {
    case qcril::interfaces::UssdModeType::NOTIFY:
      return RIL_USSD_NOTIFY;
    case qcril::interfaces::UssdModeType::REQUEST:
      return RIL_USSD_REQUEST;
    case qcril::interfaces::UssdModeType::NW_RELEASE:
      return RIL_USSD_NW_RELEASE;
    case qcril::interfaces::UssdModeType::LOCAL_CLIENT:
      return RIL_USSD_LOCAL_CLIENT;
    case qcril::interfaces::UssdModeType::NOT_SUPPORTED:
      return RIL_USSD_NOT_SUPPORTED;
    case qcril::interfaces::UssdModeType::NW_TIMEOUT:
      return RIL_USSD_NW_TIMEOUT;
    default:
      return RIL_USSD_NOTIFY;
  }
  return RIL_USSD_NOTIFY;
}

void RilSocketIndicationModule::handleUnsolOnUssdMessage(std::shared_ptr<QcRilUnsolOnUssdMessage> msg)
{
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  Marshal::Result marshalResult;
  if (msg->hasMode()) {
    RIL_UssdModeType mode = convertUssdModeType(msg->getMode());
    marshalResult = p->write(mode);
    if (marshalResult != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to write to marshal");
      return;
    }
  }
  std::string message;
  if (msg->hasMessage()) {
    message = msg->getMessage();
  }
  if (message.size()) {
    marshalResult = p->write(message);
  } else {
    marshalResult = p->write((int32_t)-1);
  }
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_CALL_UNSOL_ON_USSD, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send RIL_CALL_UNSOL_ON_USSD: %d", ret);
  }

  // FR76398: DEPRECATED: START
  // Remove in the next release
  {
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int unsol_msg;
  if (msg->hasErrorDetails()) {
    unsol_msg = RIL_IMS_UNSOL_USSD_FAILED;
    RIL_IMS_UssdInfo ussdInfo{};
    if (msg->hasMode()) {
      ussdInfo.mode = convertUssdModeType(msg->getMode());
    }
    if (msg->hasErrorDetails()) {
      ril::socket::utils::convertToSocket(ussdInfo.errorDetails, msg->getErrorDetails());
    }
    auto marshalResult = p->write(ussdInfo);
    if (marshalResult != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to write to marshal");
      return;
    }
    ril::socket::utils::release(ussdInfo);
  } else {
    unsol_msg = RIL_UNSOL_ON_USSD;
    if(ril::socket::utils::isUssdOverImsSupported() && msg->isImsDomain()) {
      unsol_msg = RIL_IMS_UNSOL_ON_USSD;
    }
    char mode = 0;
    if (msg->hasMode()) {
      mode = convertUssdModeTypeToInt(msg->getMode());
    }
    auto marshalResult = p->write(mode);
    if (marshalResult != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to write to marshal");
      return;
    }
    std::string message;
    if (msg->hasMessage()) {
      message = msg->getMessage();
    }
    if (message.size()) {
      marshalResult = p->write(message);
    } else {
      marshalResult = p->write((int32_t)-1);
    }
    if (marshalResult != Marshal::Result::SUCCESS) {
      QCRIL_LOG_ERROR("Failed to write to marshal");
      return;
    }
  }
  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO(
        "Failed to send %s: %d",
        (unsol_msg == RIL_UNSOL_ON_USSD ? "RIL_UNSOL_ON_USSD" : "RIL_IMS_UNSOL_USSD_FAILED"), ret);
  }
  }
  // FR76398: DEPRECATED: END

  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolSuppSvcNotificationMessage(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }

  int unsol_msg = RIL_CALL_UNSOL_SUPP_SVC_NOTIFICATION;
  RIL_SuppSvcNotificationInfo suppSvc = {};
  if (msg->hasNotificationType()) {
    if (msg->getNotificationType() == qcril::interfaces::NotificationType::MT) {
      suppSvc.notificationType = 1;
    } else {
      suppSvc.notificationType = 0;
    }
  }
  if (msg->hasCode()) {
    suppSvc.code = msg->getCode();
  }
  if (msg->hasIndex()) {
    suppSvc.index = msg->getIndex();
  }
  if (msg->hasType()) {
    suppSvc.type = msg->getType();
  }
  if (msg->hasNumber()) {
    ril::socket::utils::convertToSocket(suppSvc.number, msg->getNumber());
  }
  if (msg->hasConnectionId()) {
    suppSvc.connId = msg->getConnectionId();
  }
  if (msg->hasHistoryInfo() && !msg->getHistoryInfo().empty()) {
    ril::socket::utils::convertToSocket(suppSvc.historyInfo, msg->getHistoryInfo());
  }
  if (msg->hasHoldTone()) {
    suppSvc.hasHoldTone = TRUE;
    suppSvc.holdTone = msg->getHoldTone();
  }

  auto marshalResult = p->write(suppSvc);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }

  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send %s: %d", "RIL_CALL_UNSOL_SUPP_SVC_NOTIFICATION", ret);
  }

  // free suppSvc
  delete[] suppSvc.number;
  delete[] suppSvc.historyInfo;

  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
  if (msg->isIms()) {
    unsol_msg = RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION;
    RIL_IMS_SuppSvcNotification suppSvc = {};
    if (msg->hasNotificationType()) {
      if (msg->getNotificationType() == qcril::interfaces::NotificationType::MT) {
        suppSvc.notificationType = 1;
      } else {
        suppSvc.notificationType = 0;
      }
    }
    if (msg->hasCode()) {
      suppSvc.code = msg->getCode();
    }
    if (msg->hasIndex()) {
      suppSvc.index = msg->getIndex();
    }
    if (msg->hasType()) {
      suppSvc.type = msg->getType();
    }
    if (msg->hasNumber()) {
      ril::socket::utils::convertToSocket(suppSvc.number, msg->getNumber());
    }
    if (msg->hasConnectionId()) {
      suppSvc.connId = msg->getConnectionId();
    }
    if (msg->hasHistoryInfo() && !msg->getHistoryInfo().empty()) {
      ril::socket::utils::convertToSocket(suppSvc.historyInfo, msg->getHistoryInfo());
    }
    if (msg->hasHoldTone()) {
      suppSvc.hasHoldTone = TRUE;
      suppSvc.holdTone = msg->getHoldTone();
    }

    p->write(suppSvc);
    // free suppSvc
    delete[] suppSvc.number;
    delete[] suppSvc.historyInfo;
  } else {
    unsol_msg = RIL_UNSOL_SUPP_SVC_NOTIFICATION;
    RIL_SuppSvcNotification suppSvc = {};

    if (msg->hasNotificationType()) {
      if (msg->getNotificationType() == qcril::interfaces::NotificationType::MT) {
        suppSvc.notificationType = 1;
      } else {
        suppSvc.notificationType = 0;
      }
    }
    if (msg->hasCode()) {
      suppSvc.code = msg->getCode();
    }
    if (msg->hasIndex()) {
      suppSvc.index = msg->getIndex();
    }
    if (msg->hasType()) {
      suppSvc.type = msg->getType();
    }
    if (msg->hasNumber()) {
      const std::string number = msg->getNumber();
      if (!number.empty()) {
        const size_t len = number.size();
        suppSvc.number = new char [len+1]();
        number.copy(suppSvc.number, len);
      }
    }
    if (msg->hasConnectionId()) {
      suppSvc.connId = msg->getConnectionId();
    }

    p->write(suppSvc);
    // free suppSvc
    delete[] suppSvc.number;
  }
  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO(
        "Failed to send %s: %d",
        (unsol_msg == RIL_UNSOL_SUPP_SVC_NOTIFICATION ? "RIL_UNSOL_SUPP_SVC_NOTIFICATION"
                                                      : "RIL_IMS_UNSOL_SUPP_SVC_NOTIFICATION"),
        ret);
  }
  }  // FR76398: DEPRECTED: END
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolCdmaCallWaitingMessage(
    std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    RIL_CDMA_CallWaiting_v6 callWaitingRecord = {};

    if (msg->hasNumber()) {
      const std::string number = msg->getNumber();
      if (!number.empty()) {
        const size_t len = number.size();
        callWaitingRecord.number = new char [len+1]();
        number.copy(callWaitingRecord.number, len);
      }
    }

    if (msg->hasNumberPresentation()) {
      callWaitingRecord.numberPresentation = msg->getNumberPresentation();
    }

    if (msg->hasName()) {
      const std::string name = msg->getName();
      if (!name.empty()) {
        const size_t len = name.size();
        callWaitingRecord.name = new char [len+1]();
        name.copy(callWaitingRecord.name, len);
      }
    }

    if (msg->hasNumberType()) {
      callWaitingRecord.number_type = msg->getNumberType();
    }

    if (msg->hasNumberPlan()) {
      callWaitingRecord.number_plan = msg->getNumberPlan();
    }

    if (msg->hasSignalInfoRecord() && msg->getSignalInfoRecord() != nullptr) {
      callWaitingRecord.signalInfoRecord = *(msg->getSignalInfoRecord());
    }

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
      p->write(callWaitingRecord);
    }
    int ret = sendUnsolResponse(RIL_UNSOL_CDMA_CALL_WAITING, p);
    if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CDMA_CALL_WAITING: %d", ret);
    }
    // Free CallWaitingRecord
    delete[] callWaitingRecord.number;
    delete[] callWaitingRecord.name;
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolCdmaOtaProvisionStatusMessage(
    std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg) {
    int status = 0;
    if (msg->hasStatus()) {
      status = msg->getStatus();
    }
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
      p->write(status);
    }
    int ret = sendUnsolResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS, p);
    if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: %d", ret);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolCdmaInfoRecordMessage(
    std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg && msg->hasCdmaInfoRecords()) {
    auto records = msg->getCdmaInfoRecords();
    if (records) {
      std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
      if (p != nullptr) {
        p->write(*records);
      }
      int ret = sendUnsolResponse(RIL_UNSOL_CDMA_INFO_REC, p);
      if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CDMA_INFO_REC: %d", ret);
      }
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolSrvccStatusMessage(
    std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg && msg->hasState()) {
    RIL_SrvccState state = msg->getState();
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
      p->write(state);
    }
    int ret = sendUnsolResponse(RIL_UNSOL_SRVCC_STATE_NOTIFY, p);
    if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_SRVCC_STATE_NOTIFY: %d", ret);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolSupplementaryServiceMessage(
    std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }

  int unsol_msg = RIL_CALL_UNSOL_ON_SS;
  RIL_StkCcSsInfo ss = {};

  if (msg->hasServiceType()) {
    ss.serviceType = msg->getServiceType();
  }
  if (msg->hasRequestType()) {
    ss.requestType = msg->getRequestType();
  }
  if (msg->hasTeleserviceType()) {
    ss.teleserviceType = msg->getTeleserviceType();
  }
  if (msg->hasServiceClass()) {
    ss.serviceClass = (RIL_SuppServiceClass)msg->getServiceClass();
  }
  if (msg->hasResult()) {
    ss.result = msg->getResult();
  }
  if (isServiceTypeCfQuery(msg->getServiceType(), msg->getRequestType()) &&
      msg->hasCallForwardInfoList() && !msg->getCallForwardInfoList().empty()) {
    const std::vector<qcril::interfaces::CallForwardInfo> rilCfInfo = msg->getCallForwardInfoList();
    /* number of call info's */
    ss.cfData.numValidIndexes = rilCfInfo.size();
    if (ss.cfData.numValidIndexes > NUM_SERVICE_CLASSES) {
      ss.cfData.numValidIndexes = NUM_SERVICE_CLASSES;
    }
    for (int i = 0; i < rilCfInfo.size() && i < NUM_SERVICE_CLASSES; i++) {
      ril::socket::utils::convertToSocket(ss.cfData.cfInfo[i], rilCfInfo[i]);
    }
  } else {
    if (msg->hasSuppSrvInfoList() && !msg->getSuppSrvInfoList().empty()) {
      /* each int */
      for (int i = 0; i < msg->getSuppSrvInfoList().size() && i < SS_INFO_MAX; i++) {
        ss.ssInfo[i] = msg->getSuppSrvInfoList()[i];
      }
    }
  }

  auto marshalResult = p->write(ss);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  p->release(ss);
  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send %s: %d", "RIL_CALL_UNSOL_ON_SS", ret);
  }

  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
  if (msg->isIms()) {
    unsol_msg = RIL_IMS_UNSOL_ON_SS;
    RIL_IMS_StkCcUnsolSsResponse ss = {};

    if (msg->hasServiceType()) {
      ss.serviceType = msg->getServiceType();
    }
    if (msg->hasRequestType()) {
      ss.requestType = msg->getRequestType();
    }
    if (msg->hasTeleserviceType()) {
      ss.teleserviceType = msg->getTeleserviceType();
    }
    if (msg->hasServiceClass()) {
      ss.serviceClass = (RIL_SuppServiceClass)msg->getServiceClass();
    }
    if (msg->hasResult()) {
      ss.result = msg->getResult();
    }
    if (isServiceTypeCfQuery(msg->getServiceType(), msg->getRequestType()) &&
        msg->hasCallForwardInfoList() && !msg->getCallForwardInfoList().empty()) {
      const std::vector<qcril::interfaces::CallForwardInfo> rilCfInfo =
          msg->getCallForwardInfoList();
      /* number of call info's */
      ss.cfData.numValidIndexes = rilCfInfo.size();
      if (ss.cfData.numValidIndexes > NUM_SERVICE_CLASSES) {
        ss.cfData.numValidIndexes = NUM_SERVICE_CLASSES;
      }
      for (int i = 0; i < rilCfInfo.size() && i < NUM_SERVICE_CLASSES; i++) {
        ril::socket::utils::convertToSocket(ss.cfData.cfInfo[i], rilCfInfo[i]);
      }
    } else {
      if (msg->hasSuppSrvInfoList() && !msg->getSuppSrvInfoList().empty()) {
        /* each int */
        for (int i = 0; i < msg->getSuppSrvInfoList().size() && i < SS_INFO_MAX; i++) {
          ss.ssInfo[i] = msg->getSuppSrvInfoList()[i];
        }
      }
    }

    p->write(ss);
    p->release(ss);
  } else {
    unsol_msg = RIL_UNSOL_ON_SS;
    RIL_StkCcUnsolSsResponse ss = {};

    if (msg->hasServiceType()) {
      ss.serviceType = msg->getServiceType();
    }
    if (msg->hasRequestType()) {
      ss.requestType = msg->getRequestType();
    }
    if (msg->hasTeleserviceType()) {
      ss.teleserviceType = msg->getTeleserviceType();
    }
    if (msg->hasServiceClass()) {
      ss.serviceClass = (RIL_SuppServiceClass)msg->getServiceClass();
    }
    if (msg->hasResult()) {
      ss.result = msg->getResult();
    }
    if (isServiceTypeCfQuery(msg->getServiceType(), msg->getRequestType()) &&
        msg->hasCallForwardInfoList() && !msg->getCallForwardInfoList().empty()) {
      std::vector<qcril::interfaces::CallForwardInfo> rilCfInfo = msg->getCallForwardInfoList();
      /* number of call info's */
      ss.cfData.numValidIndexes = rilCfInfo.size();
      if (ss.cfData.numValidIndexes > NUM_SERVICE_CLASSES) {
        ss.cfData.numValidIndexes = NUM_SERVICE_CLASSES;
      }
      for (int i = 0; i < rilCfInfo.size() && i < NUM_SERVICE_CLASSES; i++) {
        qcril::interfaces::CallForwardInfo cf = rilCfInfo[i];
        RIL_CallForwardInfo* cfInfo = &ss.cfData.cfInfo[i];

        if (cf.hasStatus()) {
          cfInfo->status = cf.getStatus();
        }
        if (cf.hasReason()) {
          cfInfo->reason = cf.getReason();
        }
        if (cf.hasServiceClass()) {
          cfInfo->serviceClass = cf.getServiceClass();
        }
        if (cf.hasToa()) {
          cfInfo->toa = cf.getToa();
        }
        const std::string number = cf.getNumber();
        if (!number.empty()) {
          const size_t len = number.size();
          cfInfo->number = new char[len + 1]();  // TODO free cfInfo->number
          number.copy(cfInfo->number, len);
        }
        if (cf.hasTimeSeconds()) {
          cfInfo->timeSeconds = cf.getTimeSeconds();
        }
      }
    } else {
      if (msg->hasSuppSrvInfoList() && !msg->getSuppSrvInfoList().empty()) {
        /* each int */
        for (int i = 0; i < msg->getSuppSrvInfoList().size() && i < SS_INFO_MAX; i++) {
          ss.ssInfo[i] = msg->getSuppSrvInfoList()[i];
        }
      }
    }

    p->write(ss);
    p->release(ss);
  }
  int ret = sendUnsolResponse(unsol_msg, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send %s: %d",
                   (unsol_msg == RIL_UNSOL_ON_SS ? "RIL_UNSOL_ON_SS" : "RIL_IMS_UNSOL_ON_SS"), ret);
  }
  }  // FR76398: DEPRECTED: END
  QCRIL_LOG_FUNC_RETURN();
}

bool writeDataCall(std::shared_ptr<Marshal> p, const rildata::DataCallResult_t& entry) {
  RIL_Data_Call_Response_v11 dataCall = {};
  ril::socket::utils::convertToSocket(dataCall, entry);
  if (p->write(dataCall) != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write data call info");
    return false;
  }
  p->release(dataCall);
  return true;
}

void writeDataCallList(std::shared_ptr<Marshal> p, const std::vector<rildata::DataCallResult_t>& dcList)
{
  QCRIL_LOG_DEBUG("dcList %d",dcList.size());
  uint64_t _dataCallListLen = static_cast<uint64_t>(dcList.size());
  if (p->write(_dataCallListLen) != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write length");
    return;
  }
  for (rildata::DataCallResult_t entry: dcList) {
    QCRIL_LOG_INFO("RilSocketIndicationModule:: retrieving call data");
    if (!writeDataCall(p, entry)) {
      return;
    }
  }
}

void RilSocketIndicationModule::handleRadioDataCallListChangeIndMessage(
    std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg ) {
    QCRIL_LOG_INFO("RilSocketIndicationModule:: handleRadioDataCallListChangeIndMessage Indication received");
    if (msg == nullptr) {
        return;
    }
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
    std::shared_ptr<Marshal> marshal = std::make_shared<Marshal>();
    if (marshal != nullptr) {
        writeDataCallList(marshal, dcList);
        int ret = sendUnsolResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, marshal);
        if (ret != 0) {
            QCRIL_LOG_INFO("Failed to send RIL_UNSOL_DATA_CALL_LIST_CHANGED: %d", ret);
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleGstkUnsolIndMsg(
        std::shared_ptr<GstkUnsolIndMsg> msg) {
    if (msg == nullptr) {
        return;
    }
    RIL_GSTK_UnsolData stk_ind = msg->get_unsol_data();

    switch(stk_ind.type) {
        case RIL_GSTK_UNSOL_SESSION_END:
            {
                int ret = sendUnsolResponse(RIL_UNSOL_STK_SESSION_END, nullptr);
                if (ret != 0) {
                    QCRIL_LOG_INFO("Failed to send RIL_UNSOL_STK_SESSION_END: %d", ret);
                }
                break;
            }
        case RIL_GSTK_UNSOL_PROACTIVE_COMMAND:
            {
                std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
                if (p && p->write(stk_ind.cmd) == Marshal::Result::SUCCESS) {
                    int ret = sendUnsolResponse(RIL_UNSOL_STK_PROACTIVE_COMMAND, p);
                    if (ret != 0) {
                        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_STK_PROACTIVE_COMMAND: %d", ret);
                    }
                } else {
                    QCRIL_LOG_ERROR("Unable to allocate memory for Marshal or failure in write");
                }
                break;
            }
        case RIL_GSTK_UNSOL_EVENT_NOTIFY:
            {
                std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
                if (p && p->write(stk_ind.cmd) == Marshal::Result::SUCCESS) {
                    int ret = sendUnsolResponse(RIL_UNSOL_STK_EVENT_NOTIFY, p);
                    if (ret != 0) {
                        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_STK_EVENT_NOTIFY: %d", ret);
                    }
                } else {
                    QCRIL_LOG_ERROR("Unable to allocate memory for Marshal or failure in write");
                }
                break;
            }
        case RIL_GSTK_UNSOL_STK_CALL_SETUP:
            {
                std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
                if (p && p->write(stk_ind.cmd) == Marshal::Result::SUCCESS) {
                    int ret = sendUnsolResponse(RIL_UNSOL_STK_EVENT_NOTIFY, p);
                    if (ret != 0) {
                        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_STK_EVENT_NOTIFY: %d", ret);
                    }
                } else {
                    QCRIL_LOG_ERROR("Unable to allocate memory for Marshal or failure in write");
                }

                p = std::make_shared<Marshal>();
                if (p && p->write(stk_ind.setupCallTimeout) == Marshal::Result::SUCCESS) {
                    int ret = sendUnsolResponse(RIL_UNSOL_STK_CALL_SETUP, p);
                    if (ret != 0) {
                        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_STK_CALL_SETUP: %d", ret);
                    }
                } else {
                    QCRIL_LOG_ERROR("Unable to allocate memory for Marshal or failure in write");
                }
                break;
            }
    }
}

void RilSocketIndicationModule::handleUnsolNewSmsOnSimMessage(
    std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg) {
  if (!msg) {
      return;
  }
  auto p = std::make_shared<Marshal>();
  if (p) {
    p->write(msg->getRecordNumber());
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, p);
  if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: %d", ret);
  }
}

void RilSocketIndicationModule::handleUnsolSimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg) {
  if (!msg) {
      return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int ret = sendUnsolResponse(RIL_UNSOL_SIM_SMS_STORAGE_FULL, nullptr);
  if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_SIM_SMS_STORAGE_FULL: %d", ret);
  }
}

void RilSocketIndicationModule::handleUnsolNewSmsStatusReportMessage(
    std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg) {
  if (!msg) {
      return;
  }
  auto p = std::make_shared<Marshal>();
  if(p) {
    for(auto &i:msg->getPayload()) {
      p->write(static_cast<char>(i));
    }
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, p);
  if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: %d", ret);
  }
}

void RilSocketIndicationModule::handleUnsolCdmaRuimSmsStorageFullMessage(
        std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg) {
  if (msg == nullptr) {
      return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int ret = sendUnsolResponse(RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL, nullptr);
  if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: %d", ret);
  }
}

void RilSocketIndicationModule::handleUnsolImsRegStateMessage(
    std::shared_ptr<QcRilUnsolImsRegStateMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    return;
  }
  std::shared_ptr<qcril::interfaces::Registration> rilRegInfo = msg->getRegistration();
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p != nullptr && rilRegInfo != nullptr) {
    RIL_IMS_Registration regInfo{};
    ril::socket::utils::convertToSocket(regInfo, *rilRegInfo);
    p->write(regInfo);
    ril::socket::utils::release(regInfo);
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send RIL_IMS_UNSOL_REGISTRATION_STATE_CHANGED: %d", ret);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolImsSrvStatusIndication
    (std::shared_ptr<QcRilUnsolImsSrvStatusIndication> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    return;
  }
  std::shared_ptr<qcril::interfaces::ServiceStatusInfoList> ssInfoList = msg->getServiceStatus();
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p != nullptr && ssInfoList != nullptr) {
    auto rilSsInfos = ssInfoList->getServiceStatusInfo();
    int32_t numSsInfos = rilSsInfos.size();
    p->write(numSsInfos);
    for (const auto& rilSs : rilSsInfos) {
      RIL_IMS_ServiceStatusInfo ss{};
      ril::socket::utils::convertToSocket(ss, rilSs);
      p->write(ss);
      ril::socket::utils::release(ss);
    }
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send RIL_IMS_UNSOL_SERVICE_STATUS_CHANGED: %d", ret);
  }

  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolImsGeoLocationInfo(
    std::shared_ptr<QcRilUnsolImsGeoLocationInfo> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (!msg) {
    return;
  }
  std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
  if (!parcel) {
    return;
  }
  double latitude;
  double longitude;
  if (msg->hasLatitude() && msg->hasLongitude()) {
    latitude = msg->getLatitude();
    longitude = msg->getLongitude();
  }
  if (parcel->write(latitude) == Marshal::Result::SUCCESS &&
      parcel->write(longitude) == Marshal::Result::SUCCESS) {
    if (sendUnsolResponse(RIL_IMS_UNSOL_REQUEST_GEOLOCATION, parcel)) {
      QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_REQUEST_GEOLOCATION.");
    }
  } else {
    QCRIL_LOG_ERROR("Failed to write the parcel.");
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolImsGeoLocationDataStatus(
    std::shared_ptr<QcRilUnsolImsGeoLocationDataStatus> msg) {
  if (!msg) {
    return;
  }
  std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
  if (!parcel) {
    return;
  }
  RIL_IMS_GeoLocationDataStatus geoLocDataStatus = RIL_IMS_GEO_LOC_DATA_STATUS_UNKNOWN;
  ril::socket::utils::convertToSocket(geoLocDataStatus, msg->getGeoLocationDataStatus());
  if (parcel->write(geoLocDataStatus) != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write the parcel.");
    return;
  }
  if (sendUnsolResponse(RIL_IMS_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS, parcel)) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_RETRIEVING_GEOLOCATION_DATA_STATUS.");
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolImsRttMessage
        (std::shared_ptr<QcRilUnsolImsRttMessage> msg) {
    if (!msg) {
        return;
    }
    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (!parcel) {
        return;
    }
    std::string rttMessage = msg->getRttMessage();

    if (parcel->write(rttMessage) == Marshal::Result::SUCCESS) {
        if (sendUnsolResponse(RIL_CALL_UNSOL_RTT_MSG_RECEIVED, parcel)) {
            QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_RTT_MSG_RECEIVED.");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to write the parcel.");
    }
    // FR76398: DEPRECTED: START
    // Remove in the next release
    {
    std::string rttMessage = msg->getRttMessage();

    if (parcel->write(rttMessage) == Marshal::Result::SUCCESS) {
        if (sendUnsolResponse(RIL_IMS_UNSOL_RTT_MSG_RECEIVED, parcel)) {
            QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_RTT_MSG_RECEIVED.");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to write the parcel.");
    }
    } // FR76398: DEPRECTED: END
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleUnsolImsVowifiCallQuality
        (std::shared_ptr<QcRilUnsolImsVowifiCallQuality> msg) {
    if (!msg) {
        return;
    }
    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (!parcel) {
        return;
    }

    RIL_VowifiCallQuality vowifiQuality;

    switch(msg->getCallQuality()) {
        case qcril::interfaces::VowifiQuality::UNKNOWN:
            vowifiQuality = RIL_CALL_QUALITY_UNKNOWN;
            break;
        case qcril::interfaces::VowifiQuality::NONE:
            vowifiQuality = RIL_CALL_QUALITY_NONE;
            break;
        case qcril::interfaces::VowifiQuality::EXCELLENT:
            vowifiQuality = RIL_CALL_QUALITY_EXCELLENT;
            break;
        case qcril::interfaces::VowifiQuality::FAIR:
            vowifiQuality = RIL_CALL_QUALITY_FAIR;
            break;
        case qcril::interfaces::VowifiQuality::BAD:
            vowifiQuality = RIL_CALL_QUALITY_BAD;
            break;
        default:
            QCRIL_LOG_ERROR("Unknown or invalid vowifi call quality family.");
            return;
    }

    if (parcel->write(vowifiQuality) == Marshal::Result::SUCCESS) {
        if (sendUnsolResponse(RIL_CALL_UNSOL_VOWIFI_CALL_QUALITY, parcel)) {
            QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_VOWIFI_CALL_QUALITY.");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to write the parcel.");
    }
    // FR76398: DEPRECTED: START
    // Remove in the next release
    {
    RIL_IMS_VowifiCallQuality vowifiQuality;

    switch(msg->getCallQuality()) {
        case qcril::interfaces::VowifiQuality::UNKNOWN:
            vowifiQuality = RIL_IMS_CALL_QUALITY_UNKNOWN;
            break;
        case qcril::interfaces::VowifiQuality::NONE:
            vowifiQuality = RIL_IMS_CALL_QUALITY_NONE;
            break;
        case qcril::interfaces::VowifiQuality::EXCELLENT:
            vowifiQuality = RIL_IMS_CALL_QUALITY_EXCELLENT;
            break;
        case qcril::interfaces::VowifiQuality::FAIR:
            vowifiQuality = RIL_IMS_CALL_QUALITY_FAIR;
            break;
        case qcril::interfaces::VowifiQuality::BAD:
            vowifiQuality = RIL_IMS_CALL_QUALITY_BAD;
            break;
        default:
            QCRIL_LOG_ERROR("Unknown or invalid vowifi call quality family.");
            return;
    }

    if (parcel->write(vowifiQuality) == Marshal::Result::SUCCESS) {
        if (sendUnsolResponse(RIL_IMS_UNSOL_VOWIFI_CALL_QUALITY, parcel)) {
            QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_VOWIFI_CALL_QUALITY.");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to write the parcel.");
    }
    } // FR76398: DEPRECTED: END
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleIncoming3Gpp2SMSMessage(
        std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg) {
    if (msg == nullptr) {
        return;
    }
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p != nullptr) {
        p->write(msg->getCdmaSms());
    }

    int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, p);
    if (ret != 0) {
        QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: %d", ret);
    }
}

void RilSocketIndicationModule::handleIncoming3GppSMSMessage(
        std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg) {
    if (!msg || msg->getPayload().empty()) {
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p) {
        if (p->write(msg->getPayload().data(), msg->getPayload().size()) !=
                Marshal::Result::SUCCESS) {
            QCRIL_LOG_ERROR("Failed to marshal indication data.");
            return;
        }
    } else {
        QCRIL_LOG_ERROR("Failed to create parcel.");
        return;
    }

    int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_NEW_SMS, p);
    if (ret != 0) {
        QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_RESPONSE_NEW_SMS: %d", ret);
    }
}

void RilSocketIndicationModule::handleNewBroadcastSmsMessage(
        std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg) {
    if (!msg || msg->getPayload().empty()) {
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if (p) {
        if (p->write(msg->getPayload()) != Marshal::Result::SUCCESS) {
            QCRIL_LOG_ERROR("Failed to marshal indication data.");
            return;
        }
    } else {
        QCRIL_LOG_ERROR("Failed to create parcel.");
        return;
    }

    int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, p);
    if (ret != 0) {
        QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: %d", ret);
    }
}

void RilSocketIndicationModule::handleQcRilUnsolCallStateChangeMessage(
    std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if(p) {
    auto &callInfos = msg->getCallInfo();
    uint32_t sz = callInfos.size();
    p->write(sz);
    for (auto &callInfo : callInfos) {
      RIL_CallInfo rilCall{};
      ril::socket::utils::convertToSocket(rilCall, callInfo);
      p->write(rilCall);
      ril::socket::utils::release(rilCall);
    }
    int ret = sendUnsolResponse(RIL_CALL_UNSOL_CALL_STATE_CHANGED, p);
    if (ret != 0) {
      QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_CALL_STATE_CHANGED: %d", ret);
    }
  } else {
    QCRIL_LOG_ERROR("Failed to allocate memory for parcel");
  }

  // FR76398: DEPRECATED: START
  // Remove in the next release
  {
  if (msg->getCallInfo().empty() || msg->hasCsCalls())
  {
    int ret = sendUnsolResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, nullptr);
    if (ret != 0) {
      QCRIL_LOG_INFO("Failed to send RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: %d", ret);
    }
  }

  if (msg->hasImsCalls()) {
    RIL_IMS_CallInfo *calls = nullptr;
    size_t sz = 0;
    int ret = -1;
    std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
    if(p) {
      ril::socket::utils::convertToSocket(calls, sz, msg->getCallInfo());
      p->write(calls, sz);
      ret = sendUnsolResponse(RIL_IMS_UNSOL_CALL_STATE_CHANGED, p);
      ril::socket::utils::release(calls, sz);
      if (ret != 0) {
        QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_CALL_STATE_CHANGED: %d", ret);
      }
    } else {
      QCRIL_LOG_ERROR("Failed to allocate memory for parcel");
    }
  }
  }
  // FR76398: DEPRECATED: END
}

void RilSocketIndicationModule::handleQcRilUnsolImsModifyCallMessage(
    std::shared_ptr<QcRilUnsolImsModifyCallMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  RIL_CallModifyInfo modifyInfo{};
  if (msg->hasCallIndex()) {
    modifyInfo.callId = msg->getCallIndex();
  }
  if (msg->hasCallType()) {
    ril::socket::utils::convertToSocket(modifyInfo.callType, msg->getCallType());
  }
  if (msg->hasCallDomain()) {
    ril::socket::utils::convertToSocket(modifyInfo.callDomain, msg->getCallDomain());
  }
  if (msg->hasRttMode()) {
    modifyInfo.hasRttMode = TRUE;
    ril::socket::utils::convertToSocket(modifyInfo.rttMode, msg->getRttMode());
  }
  if (msg->hasCallModifyFailCause()) {
    modifyInfo.hasCallModifyFailCause = TRUE;
    ril::socket::utils::convertToSocket(modifyInfo.callModifyFailCause,
                                        msg->getCallModifyFailCause());
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(modifyInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_CALL_UNSOL_MODIFY_CALL, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_MODIFY_CALL: %d", ret);
  }
  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
  RIL_IMS_CallModifyInfo modifyInfo{};
  if (msg->hasCallIndex()) {
    modifyInfo.callId = msg->getCallIndex();
  }
  if (msg->hasCallType()) {
    ril::socket::utils::convertToSocket(modifyInfo.callType, msg->getCallType());
  }
  if (msg->hasCallDomain()) {
    ril::socket::utils::convertToSocket(modifyInfo.callDomain, msg->getCallDomain());
  }
  if (msg->hasRttMode()) {
    modifyInfo.hasRttMode = TRUE;
    ril::socket::utils::convertToSocket(modifyInfo.rttMode, msg->getRttMode());
  }
  if (msg->hasCallModifyFailCause()) {
    modifyInfo.hasCallModifyFailCause = TRUE;
    ril::socket::utils::convertToSocket(modifyInfo.callModifyFailCause,
                                        msg->getCallModifyFailCause());
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(modifyInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_MODIFY_CALL, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_MODIFY_CALL: %d", ret);
  }
  } // FR76398: DEPRECTED: START
}

void RilSocketIndicationModule::handleQcRilUnsolMessageWaitingInfoMessage(
    std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  RIL_IMS_MessageWaitingInfo mwi{};

  if (msg->hasMessageSummary() && !msg->getMessageSummary().empty()) {
    ril::socket::utils::convertToSocket(mwi.messageSummary, mwi.messageSummaryLen,
                                        msg->getMessageSummary());
  }
  if (msg->hasUeAddress()) {
    ril::socket::utils::convertToSocket(mwi.ueAddress, msg->getUeAddress());
  }
  if (msg->hasMessageDetails() && !msg->getMessageDetails().empty()) {
    ril::socket::utils::convertToSocket(mwi.messageDetails, mwi.messageDetailsLen,
                                        msg->getMessageDetails());
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(mwi);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  ril::socket::utils::release(mwi);
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_MESSAGE_WAITING, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_MESSAGE_WAITING: %d", ret);
  }
}

void RilSocketIndicationModule::handleQcRilUnsolImsVopsChangedMessage(
    std::shared_ptr<QcRilUnsolImsVopsIndication> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  uint8_t vopsStatus = msg->getVopsStatus() ? 1 : 0;
  auto marshalResult = p->write(vopsStatus);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_VOPS_CHANGED, p);
  if (ret != 0) {
    QCRIL_LOG_INFO("Failed to send RIL_IMS_UNSOL_VOPS_CHANGED: %d", ret);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleQcRilUnsolConfParticipantStatusInfoMessage(
    std::shared_ptr<QcRilUnsolConfParticipantStatusInfoMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

  RIL_ParticipantStatusInfo participantStatusInfo{};

  if (msg->hasCallId()) {
    participantStatusInfo.callId = msg->getCallId();
  }
  if (msg->hasOperation()) {
    ril::socket::utils::convertToSocket(participantStatusInfo.operation, msg->getOperation());
  }
  if (msg->hasSipStatus()) {
    participantStatusInfo.sipStatus = msg->getSipStatus();
  }
  if (msg->hasParticipantUri() && !msg->getParticipantUri().empty()) {
    ril::socket::utils::convertToSocket(participantStatusInfo.participantUri,
                                        msg->getParticipantUri());
  }
  if (msg->hasIsEct()) {
    participantStatusInfo.hasIsEct = TRUE;
    participantStatusInfo.isEct = msg->getIsEct();
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(participantStatusInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  delete []participantStatusInfo.participantUri;
  int ret = sendUnsolResponse(RIL_CALL_UNSOL_PARTICIPANT_STATUS_INFO, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_PARTICIPANT_STATUS_INFO: %d", ret);
  }

  // FR76398: DEPRECATED: START
  // Remove in the next release
  {
  RIL_IMS_ParticipantStatusInfo participantStatusInfo{};

  if (msg->hasCallId()) {
    participantStatusInfo.callId = msg->getCallId();
  }
  if (msg->hasOperation()) {
    ril::socket::utils::convertToSocket(participantStatusInfo.operation, msg->getOperation());
  }
  if (msg->hasSipStatus()) {
    participantStatusInfo.sipStatus = msg->getSipStatus();
  }
  if (msg->hasParticipantUri() && !msg->getParticipantUri().empty()) {
    ril::socket::utils::convertToSocket(participantStatusInfo.participantUri,
                                        msg->getParticipantUri());
  }
  if (msg->hasIsEct()) {
    participantStatusInfo.hasIsEct = TRUE;
    participantStatusInfo.isEct = msg->getIsEct();
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(participantStatusInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  delete []participantStatusInfo.participantUri;
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_PARTICIPANT_STATUS_INFO: %d", ret);
  }
  }
  // FR76398: DEPRECATED: END
}

void RilSocketIndicationModule::handleQcRilUnsolImsRegBlockStatusMessage(
    std::shared_ptr<QcRilUnsolImsRegBlockStatusMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  std::shared_ptr<qcril::interfaces::RegistrationBlockStatus> data = msg->getBlockStatus();
  if (data == nullptr) {
    QCRIL_LOG_ERROR("indication is null.");
    return;
  }
  RIL_IMS_RegistrationBlockStatus regBlockStatus{};

  if (data->hasBlockStatusOnWwan()) {
    regBlockStatus.hasBlockStatusOnWwan = TRUE;
    ril::socket::utils::convertToSocket(regBlockStatus.blockStatusOnWwan,
                                        data->getBlockStatusOnWwan());
  }
  if (data->hasBlockStatusOnWlan()) {
    regBlockStatus.hasBlockStatusOnWlan = true;
    ril::socket::utils::convertToSocket(regBlockStatus.blockStatusOnWlan,
                                        data->getBlockStatusOnWlan());
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(regBlockStatus);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_REGISTRATION_BLOCK_STATUS: %d", ret);
  }
}
void RilSocketIndicationModule::handleQcRilUnsolAutoCallRejectionMessage(
            std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  RIL_AutoCallRejectionInfo autoRejInfo{};
  if (msg->hasCallType()) {
    ril::socket::utils::convertToSocket(autoRejInfo.callType, msg->getCallType());
  }
  if (msg->hasCallFailCause()) {
    ril::socket::utils::convertToSocket(autoRejInfo.cause, msg->getCallFailCause());
  }
  if (msg->hasSipErrorCode()) {
    autoRejInfo.sipErrorCode = msg->getSipErrorCode();
  }
  if (msg->hasNumber()) {
    ril::socket::utils::convertToSocket(autoRejInfo.number, msg->getNumber());
  }
  if (msg->hasVerificationStatus()) {
    ril::socket::utils::convertToSocket(autoRejInfo.verificationStatus,
                                        msg->getVerificationStatus());
  }
  if (msg->hasComposerInfo()) {
    ril::socket::utils::convertToSocket(autoRejInfo.ccInfo, msg->getComposerInfo());
  }
  if (msg->hasEcnamInfo()) {
    ril::socket::utils::convertToSocket(autoRejInfo.ecnamInfo, msg->getEcnamInfo());
  }

  auto marshalResult = p->write(autoRejInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  ril::socket::utils::release(autoRejInfo);

  int ret = sendUnsolResponse(RIL_CALL_UNSOL_INCOMING_CALL_AUTO_REJECTED, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_INCOMING_CALL_AUTO_REJECTED: %d", ret);
  }


  // FR76398: DEPRECATED: START
  // Remove in the next release
  {
  RIL_IMS_AutoCallRejectionInfo autoRejInfo{};
  if (msg->hasCallType()) {
    ril::socket::utils::convertToSocket(autoRejInfo.callType, msg->getCallType());
  }
  if (msg->hasCallFailCause()) {
    ril::socket::utils::convertToSocket(autoRejInfo.cause, msg->getCallFailCause());
  }
  if (msg->hasSipErrorCode()) {
    autoRejInfo.sipErrorCode = msg->getSipErrorCode();
  }
  if (msg->hasNumber()) {
    ril::socket::utils::convertToSocket(autoRejInfo.number, msg->getNumber());
  }
  if (msg->hasVerificationStatus()) {
    ril::socket::utils::convertToSocket(autoRejInfo.verificationStatus,
                                        msg->getVerificationStatus());
  }
  if (msg->hasComposerInfo()) {
    ril::socket::utils::convertToSocket(autoRejInfo.ccInfo, msg->getComposerInfo());
  }
  if (msg->hasEcnamInfo()) {
    ril::socket::utils::convertToSocket(autoRejInfo.ecnamInfo, msg->getEcnamInfo());
  }

  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(autoRejInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  ril::socket::utils::release(autoRejInfo);

  int ret = sendUnsolResponse(RIL_IMS_UNSOL_INCOMING_CALL_AUTO_REJECTED, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_INCOMING_CALL_AUTO_REJECTED: %d", ret);
  }
  }
}

void RilSocketIndicationModule::handleQcRilUnsolImsWfcRoamingConfigIndication(
            std::shared_ptr<QcRilUnsolImsWfcRoamingConfigIndication> msg) {
  if (!msg)  {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  uint8_t wfcRoamingConfigurationSupport = (msg->getWfcRoamingConfigSupport() ? TRUE : FALSE);
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  auto marshalResult = p->write(wfcRoamingConfigurationSupport);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_WFC_ROAMING_MODE_CONFIG_SUPPORT: %d", ret);
  }
}

void RilSocketIndicationModule::handleQcRilUnsolImsVoiceInfo(
            std::shared_ptr<QcRilUnsolImsVoiceInfo> msg) {
  if (!msg)  {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  RIL_VoiceInfoType voiceInfo;
  ril::socket::utils::convertToSocket(voiceInfo, msg->getVoiceInfo());
  auto marshalResult = p->write(voiceInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_CALL_UNSOL_VOICE_INFO, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_VOICE_INFO: %d", ret);
  }
  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
  RIL_IMS_VoiceInfoType voiceInfo;
  ril::socket::utils::convertToSocket(voiceInfo, msg->getVoiceInfo());
  auto marshalResult = p->write(voiceInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_IMS_UNSOL_VOICE_INFO, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_VOICE_INFO: %d", ret);
  }
  } // FR76398: DEPRECTED: END
}

void RilSocketIndicationModule::handleUnsolIncomingImsSmsMessage(
        std::shared_ptr<RilUnsolIncomingImsSMSMessage> msg) {
    if (!msg) {
        return;
    }

    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (!parcel) {
        return;
    }

    RIL_IMS_IncomingSms incomingSms {};
    std::vector<uint8_t> payload;

    switch (msg->getTech()) {
        case RIL_RadioTechnologyFamily::RADIO_TECH_3GPP:
            incomingSms.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
            incomingSms.pdu = const_cast<uint8_t*>(msg->getGsmPayload().data());
            incomingSms.pduLength = msg->getGsmPayload().size();
            incomingSms.verificationStatus = ril::socket::utils::convertVerificationStatus(msg->getVerificationStatus());
            break;
        case RIL_RadioTechnologyFamily::RADIO_TECH_3GPP2:
            ril::socket::utils::smsConvertCdmaFormatToPseudoPdu(msg->getCdmaPayload(), payload);
            incomingSms.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP2;
            incomingSms.pdu = payload.data();
            incomingSms.pduLength = payload.size();
            incomingSms.verificationStatus = RIL_IMS_VerificationStatus::RIL_IMS_VERSTAT_NONE;
            break;
        default:
            QCRIL_LOG_ERROR("Unknown or invalid radio technology family.");
            return;
    }

    if (parcel->write(incomingSms) == Marshal::Result::SUCCESS) {
        if (sendUnsolResponse(RIL_IMS_UNSOL_INCOMING_IMS_SMS, parcel)) {
            QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_INCOMING_IMS_SMS.");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to write to parcel.");
    }
}

void RilSocketIndicationModule::handleUnsolNewImsSmsStatusReportMessage(
        std::shared_ptr<RilUnsolNewImsSmsStatusReportMessage> msg) {
    if (!msg) {
        return;
    }

    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (!parcel) {
        return;
    }

    RIL_IMS_SmsStatusReport smsStatusReport {};
    std::vector<uint8_t> payload;

    switch (msg->getRadioTech()) {
        case RIL_RadioTechnologyFamily::RADIO_TECH_3GPP:
            smsStatusReport.messageRef = msg->getMessageRef();
            smsStatusReport.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP;
            smsStatusReport.pdu = msg->getGsmPayload().data();
            smsStatusReport.pduLength = msg->getGsmPayload().size();
            break;
        case RIL_RadioTechnologyFamily::RADIO_TECH_3GPP2:
            ril::socket::utils::smsConvertCdmaFormatToPseudoPdu(msg->getCdmaPayload(), payload);
            smsStatusReport.messageRef = msg->getMessageRef();
            smsStatusReport.format = RIL_IMS_SmsFormat::RIL_IMS_SMS_FORMAT_3GPP2;
            smsStatusReport.pdu = payload.data();
            smsStatusReport.pduLength = payload.size();
            break;
        default:
            QCRIL_LOG_ERROR("Unknown or invalid radio technology family.");
            return;
    }

    if (parcel->write(smsStatusReport) == Marshal::Result::SUCCESS) {
        if (sendUnsolResponse(RIL_IMS_UNSOL_SMS_STATUS_REPORT, parcel)) {
            QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_SMS_STATUS_REPORT.");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to write to parcel.");
    }
}

void RilSocketIndicationModule::handleUnsolImsViceInfoMessage(
        std::shared_ptr<QcRilUnsolImsViceInfoMessage> msg) {
    if (!msg) {
        return;
    }

    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (!parcel) {
        return;
    }

    if (msg->hasViceInfoUri()) {
        const std::vector<uint8_t>& viceInfoUri = msg->getViceInfoUri();
        if (!viceInfoUri.empty()) {
            if (parcel->write(viceInfoUri) == Marshal::Result::SUCCESS) {
                if (sendUnsolResponse(RIL_CALL_UNSOL_REFRESH_VICE_INFO, parcel)) {
                    QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_REFRESH_VICE_INFO.");
                }
            } else {
                QCRIL_LOG_ERROR("Failed to write to parcel.");
            }
        }
    }
    // FR76398: DEPRECTED: START
    // Remove in the next release
    {
    if (msg->hasViceInfoUri()) {
        const std::vector<uint8_t>& viceInfoUri = msg->getViceInfoUri();
        if (!viceInfoUri.empty()) {
            if (parcel->write(viceInfoUri) == Marshal::Result::SUCCESS) {
                if (sendUnsolResponse(RIL_IMS_UNSOL_REFRESH_VICE_INFO, parcel)) {
                    QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_REFRESH_VICE_INFO.");
                }
            } else {
                QCRIL_LOG_ERROR("Failed to write to parcel.");
            }
        }
    }
    } // FR76398: DEPRECTED: END
}

void RilSocketIndicationModule::handleQcRilUnsolImsHandoverMessage(
        std::shared_ptr<QcRilUnsolImsHandoverMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

  RIL_HandoverInfo handoverInfo{};
  std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }

  ril::socket::utils::convertToSocket(handoverInfo, msg);
  auto marshalResult = p->write(handoverInfo);
  if (marshalResult != Marshal::Result::SUCCESS) {
    QCRIL_LOG_ERROR("Failed to write to marshal");
    return;
  }

  int ret = sendUnsolResponse(RIL_CALL_UNSOL_HANDOVER, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_HANDOVER: %d", ret);
  }
  ril::socket::utils::release(handoverInfo);

  // FR76398: DEPRECTED: START
  // Remove in the next release
  {
    RIL_IMS_HandoverInfo handoverInfo{};
    std::shared_ptr<Marshal> p = nullptr;
    if (msg) {
        p = std::make_shared<Marshal>();
        if (p) {
            ril::socket::utils::convertToSocket(handoverInfo, msg);
            if(p->write(handoverInfo) == Marshal::Result::SUCCESS) {
                sendUnsolResponse(RIL_IMS_UNSOL_HANDOVER, p);
            }
            ril::socket::utils::release(handoverInfo);
        }
    }
  }  // FR76398: DEPRECTED: END
}

void RilSocketIndicationModule::handleQcRilUnsolImsConferenceInfoMessage(
        std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg) {
    QCRIL_LOG_INFO("handleQcRilUnsolImsConferenceInfoMessage");
    RIL_RefreshConferenceInfo conferenceInfo{};
    std::shared_ptr<Marshal> p = nullptr;
    if (msg) {
        p = std::make_shared<Marshal>();
        if (p) {
            ril::socket::utils::convertToSocket(conferenceInfo, msg);
            if (p->write(conferenceInfo) == Marshal::Result::SUCCESS) {
                sendUnsolResponse(RIL_CALL_UNSOL_REFRESH_CONF_INFO, p);
            }
            ril::socket::utils::release(conferenceInfo);
        }
    }

    // FR76398: DEPRECTED: START
    // Remove in the next release
    {
      RIL_IMS_RefreshConferenceInfo conferenceInfo{};
      std::shared_ptr<Marshal> p = nullptr;
      if (msg) {
          p = std::make_shared<Marshal>();
          if (p) {
            ril::socket::utils::convertToSocket(conferenceInfo, msg);
            if (p->write(conferenceInfo) == Marshal::Result::SUCCESS) {
              sendUnsolResponse(RIL_IMS_UNSOL_REFRESH_CONF_INFO, p);
            }
            ril::socket::utils::release(conferenceInfo);
        }
    }
    } //FR76398: DEPRECTED: END
}

void RilSocketIndicationModule::handleQcRilUnsolImsSrtpEncryptionStatus(
        std::shared_ptr<QcRilUnsolImsSrtpEncryptionStatus> msg) {
    if (!msg) {
        QCRIL_LOG_ERROR("msg is null");
        return;
    }

    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (parcel) {
        RIL_IMS_SrtpEncryptionStatus info  = ril::socket::utils::convertToSocket(*msg);
        if (parcel->write(info) == Marshal::Result::SUCCESS) {
            sendUnsolResponse(RIL_IMS_UNSOL_SRTP_ENCRYPTION_STATUS, parcel);
        } else {
            QCRIL_LOG_ERROR("Failed to write to the marshal");
        }
    } else {
        QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    }
}

void RilSocketIndicationModule::handleUnsolImsTtyNotificationMessage(
    std::shared_ptr<QcRilUnsolImsTtyNotificationMessage> msg) {
    if (!msg) {
        return;
    }

    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (!parcel) {
        return;
    }

    if (msg->hasTtyMode()) {
        RIL_TtyModeType ttyMode = ril::socket::utils::convertCallTtyMode(msg->getTtyMode());
        if (parcel->write(ttyMode) == Marshal::Result::SUCCESS) {
            if (sendUnsolResponse(RIL_CALL_UNSOL_TTY_NOTIFICATION, parcel)) {
                QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_TTY_NOTIFICATION.");
            }
        } else {
            QCRIL_LOG_ERROR("Failed to write to parcel.");
        }
    }
    // FR76398: DEPRECTED: START
    // Remove in the next release
    {
    if (msg->hasTtyMode()) {
        RIL_IMS_TtyModeType ttyMode = ril::socket::utils::convertTtyMode(msg->getTtyMode());
        if (parcel->write(ttyMode) == Marshal::Result::SUCCESS) {
            if (sendUnsolResponse(RIL_IMS_UNSOL_TTY_NOTIFICATION, parcel)) {
                QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_TTY_NOTIFICATION.");
            }
        } else {
            QCRIL_LOG_ERROR("Failed to write to parcel.");
        }
    }
    } // FR76398: DEPRECTED: END
}
void RilSocketIndicationModule::handlePendingMultiLineStatus(
            std::shared_ptr<QcRilUnsolImsPendingMultiLineStatus> /*msg*/) {
    sendUnsolResponse(RIL_IMS_UNSOL_MULTI_IDENTITY_PENDING_INFO, nullptr);
}

void RilSocketIndicationModule::handleImsMultiIdentityStatusMessage(
            std::shared_ptr<QcRilUnsolImsMultiIdentityStatusMessage> msg) {
    std::vector<RIL_IMS_MultiIdentityLineInfo> lines;
    if (msg) {
        std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
        if (p) {
            ril::socket::utils::convertToSocket(lines, msg->getLineInfo());
            if (p->write(lines) == Marshal::Result::SUCCESS) {
                sendUnsolResponse(RIL_IMS_UNSOL_MULTI_IDENTITY_REGISTRATION_STATUS, p);
            }
            ril::socket::utils::release(lines);
        }
    }
}

void RilSocketIndicationModule::handleUnsolAdnInitDoneMessage(
            std::shared_ptr<QcRilUnsolAdnInitDoneMessage> msg) {
    std::vector<RIL_IMS_MultiIdentityLineInfo> lines;
    if (msg) {
        sendUnsolResponse(RIL_UNSOL_RESPONSE_ADN_INIT_DONE, nullptr);
    }
}

void RilSocketIndicationModule::handleQcRilUnsolAdnRecordsOnSimMessage(
            std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg) {
    if (msg) {
        auto incomingRecords = msg->getAdnRecords();
        RIL_AdnRecords records{};
        if (incomingRecords->record_elements < 0 || incomingRecords->record_elements > RIL_NUM_ADN_RECORDS) {
            QCRIL_LOG_ERROR("Invalid incomingRecords->record_elements %d", incomingRecords->record_elements);
            return;
        }
        records.record_elements = incomingRecords->record_elements;
        for (size_t i = 0; i < records.record_elements; i++) {
            records.adn_record_info[i].record_id =
                incomingRecords->adn_record_info[i].record_id;
            records.adn_record_info[i].name =
                const_cast<char *>(incomingRecords->adn_record_info[i].name.c_str());
            records.adn_record_info[i].number =
                const_cast<char *>(incomingRecords->adn_record_info[i].number.c_str());
            records.adn_record_info[i].email_elements =
                incomingRecords->adn_record_info[i].email_elements;
            if (records.adn_record_info[i].email_elements > RIL_MAX_NUM_EMAIL_COUNT) {
                QCRIL_LOG_ERROR("Invalid email_elements %d",
                        incomingRecords->adn_record_info[i].email_elements);
                return;
            }
            for (int j = 0; j < records.adn_record_info[i].email_elements; j++) {
                records.adn_record_info[i].email[j] =
                    const_cast<char *>(incomingRecords->adn_record_info[i].email[j].c_str());
            }
            records.adn_record_info[i].anr_elements =
                incomingRecords->adn_record_info[i].anr_elements;

            if (records.adn_record_info[i].anr_elements > RIL_MAX_NUM_AD_COUNT) {
                QCRIL_LOG_ERROR("Invalid anr_elements %d",
                        incomingRecords->adn_record_info[i].anr_elements);
                return;
            }
            for (int j = 0; j < records.adn_record_info[i].anr_elements; j++) {
                records.adn_record_info[i].ad_number[j] =
                    const_cast<char *>(incomingRecords->adn_record_info[i].ad_number[j].c_str());
            }

        }
        std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();

        if (parcel && parcel->write(records) == Marshal::Result::SUCCESS) {
            sendUnsolResponse(RIL_UNSOL_RESPONSE_ADN_RECORDS, parcel);
        }
    }
}

void RilSocketIndicationModule::handleUimSimStatusChangedInd(
            std::shared_ptr<UimSimStatusChangedInd> msg) {
    if (msg) {
        sendUnsolResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, nullptr);
    }
}

void RilSocketIndicationModule::handleStkCcAlphaNotify(
            std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg) {
    if (msg) {
        std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
        if (parcel && parcel->write(msg->getPayload()) == Marshal::Result::SUCCESS) {
            sendUnsolResponse(RIL_UNSOL_STK_CC_ALPHA_NOTIFY, parcel);
        }
    }
}

/*
 * Sends RIL_UNSOL_SIM_REFRESH
 */
void RilSocketIndicationModule::handleUimSimRefreshIndication(
        std::shared_ptr<UimSimRefreshIndication> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    if (msg) {
        std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
        if (parcel) {
            auto ind = msg->get_refresh_ind();
            RIL_SimRefreshResponse_v7 unsol{
                .result = static_cast<RIL_SimRefreshResult>(ind.result),
                    .ef_id = ind.ef_id,
                    .aid = const_cast<char *>(ind.aid.c_str()),
            };
            if (parcel->write(unsol) == Marshal::Result::SUCCESS) {
                sendUnsolResponse(RIL_UNSOL_SIM_REFRESH, parcel);
            }
        }
    }
}

static std::shared_ptr<Marshal> constructMarshalForUnsolOemhookRaw(int unsol_event, uint8_t* data,
                                                                   uint32_t data_len) {
  uint32_t bufLen = 0;
  char* buf = (char*)oemhook::utils::constructOemHookRaw(&bufLen, unsol_event, data, data_len);
  std::shared_ptr<Marshal> p = nullptr;
  if (buf && bufLen) {
    p = std::make_shared<Marshal>();
    if (p != nullptr) {
      auto marshalResult = p->write(buf, bufLen);
      if (marshalResult != Marshal::Result::SUCCESS) {
        QCRIL_LOG_ERROR("Failed to write to marshal");
        p = nullptr;
      }
    } else {
      QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    }
  }
  if (buf) {
    delete[] buf;
  }
  return p;
}

/*
 * Sends RIL_UNSOL_OEM_HOOK_RAW for QCRIL_REQ_HOOK_UNSOL_SS_ERROR_CODE
 */
void RilSocketIndicationModule::handleQcRilUnsolSuppSvcErrorCodeMessage(
    std::shared_ptr<QcRilUnsolSuppSvcErrorCodeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int payload[2] = { 0 };
  payload[0] = msg->getCallId();
  payload[1] = msg->getFailureCause();
  std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_SS_ERROR_CODE, (uint8_t*)payload, sizeof(payload));
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
}

/*
 * Sends RIL_UNSOL_OEM_HOOK_RAW for QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO
 */
void RilSocketIndicationModule::handleQcRilUnsolSpeechCodecInfoMessage(
    std::shared_ptr<QcRilUnsolSpeechCodecInfoMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  int payload[3] = { 0 };
  payload[0] = msg->getCallId();
  payload[1] = msg->getSpeechCodec();
  payload[2] = msg->getNetworkMode();
  std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO, (uint8_t*)payload, sizeof(payload));
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
}


/**
 * Sends RIL_UNSOL_OEM_HOOK_RAW for QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND
 */
void RilSocketIndicationModule::handleRilUnsolMaxActiveDataSubsChangedMessage(
    std::shared_ptr<RilUnsolMaxActiveDataSubsChangedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  auto payload = msg->getMaxActiveDataSubscriptions();
  std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND, (uint8_t*)&payload, sizeof(payload));
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
}

/**
 * Sends RIL_UNSOL_OEM_HOOK_RAW for QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND
 */
void RilSocketIndicationModule::handleRilUnsolCsgIdChangedMessage(
    std::shared_ptr<RilUnsolCsgIdChangedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  auto payload = msg->getCsgId();
  std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND, (uint8_t*)&payload, sizeof(payload));
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
}

/**
 * Sends RIL_UNSOL_OEM_HOOK_RAW for QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE
 */
void RilSocketIndicationModule::handleRilUnsolEngineerModeMessage(
    std::shared_ptr<RilUnsolEngineerModeMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  uint8_t* payload = nullptr;
  size_t payload_len = 0;
  if (msg->isCdmaFtmDataValid()) {
    cdma_ftm_data& data = msg->getCdmaFtmData();
    payload = (uint8_t *)&data;
    payload_len = sizeof(data);
  }
  if (msg->isGsmFtmDataValid()) {
    gsm_ftm_data& data = msg->getGsmFtmData();
    payload = (uint8_t *)&data;
    payload_len = sizeof(data);
  }
  if (msg->isWcdmaFtmDataValid()) {
    wcdma_ftm_data& data = msg->getWcdmaFtmData();
    payload = (uint8_t *)&data;
    payload_len = sizeof(data);
  }

  if (payload && payload_len) {
    std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
        QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE, payload, payload_len);
    if (p == nullptr) {
      QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
      return;
    }
    int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
    if (ret != 0) {
      QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
    }
  }
}

void RilSocketIndicationModule::handlePhonebookRecordsChangedMessage(
    std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg) {
    if (msg == nullptr) {
        return;
    }
    QCRIL_LOG_INFO("Handling msg: %s", msg->dump().c_str());
}

void RilSocketIndicationModule::handleQcRilUnsolImsMultiSimVoiceCapabilityChanged(
    std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg) {
  if (!msg) {
    return;
  }

  std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
  if (!parcel) {
    return;
  }

  RIL_IMS_MultiSimVoiceCapability voiceCapability =
      ril::socket::utils::convertMultiSimVoiceCapability(msg->getMultiSimVoiceCapability());
  if (parcel->write(voiceCapability) == Marshal::Result::SUCCESS) {
    if (sendUnsolResponse(RIL_IMS_UNSOL_MULTI_SIM_VOICE_CAPABILITY_CHANGED, parcel)) {
      QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_MULTI_SIM_VOICE_CAPABILITY_CHANGED.");
    }
  } else {
    QCRIL_LOG_ERROR("Failed to write to parcel.");
  }
}

/**
 * Sends RIL_UNSOL_OEM_HOOK_RAW for QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED
 */
void RilSocketIndicationModule::handleQcRilUnsolAudioStateChangedMessage(
    std::shared_ptr<QcRilUnsolAudioStateChangedMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

  auto payload = msg->getAudioParams();

  std::shared_ptr<Marshal> parcel = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED, (uint8_t*)payload.c_str(), payload.size() + 1);

  if (parcel == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }

  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, parcel);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
  QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleRilUnsolOemNetworkScanMessage(
  std::shared_ptr<RilUnsolOemNetworkScanMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  const char *data = msg->getData();
  uint16_t dataLen = msg->getDataLen();
  std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND, (uint8_t *)data, dataLen);
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
}

void RilSocketIndicationModule::handleRilUnsolSubProvisionStatusMessage(
  std::shared_ptr<RilUnsolSubProvisioningStatusMessage> msg) {
  QCRIL_LOG_FUNC_ENTRY();

  if (msg == nullptr) {
    return;
  }
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  qcril::interfaces::RILSubProvStatus_t prefResp = msg->getStatus();
  int32_t resp[2];
  resp[0] = prefResp.user_preference;
  resp[1] = prefResp.current_sub_preference;

  std::shared_ptr<Marshal> p = constructMarshalForUnsolOemhookRaw(
      QCRIL_REQ_HOOK_UNSOL_SUB_PROVISION_STATUS, (uint8_t *)resp, sizeof(resp));
  if (p == nullptr) {
    QCRIL_LOG_ERROR("Failed to allocate memory for marshal");
    return;
  }
  int ret = sendUnsolResponse(RIL_UNSOL_OEM_HOOK_RAW, p);
  if (ret != 0) {
    QCRIL_LOG_ERROR("Failed to send RIL_UNSOL_OEM_HOOK_RAW: %d", ret);
  }
}

void RilSocketIndicationModule::handleQcRilUnsolImsPreAlertingCallInfo(
    std::shared_ptr<QcRilUnsolImsPreAlertingCallInfo> msg) {
  if (!msg) {
    return;
  }

  std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
  if (!parcel) {
    return;
  }

  RIL_PreAlertingCallInfo preAlertingInfo{};
  bool has_preAleringInfo = false;

  preAlertingInfo.callId = msg->getPreAlertingInfo().getCallId();
  if (msg->getPreAlertingInfo().hasCallComposerInfo()) {
    ril::socket::utils::convertToSocket(preAlertingInfo.ccInfo,
                                        msg->getPreAlertingInfo().getCallComposerInfo());
    has_preAleringInfo = true;
  }
  if (msg->getPreAlertingInfo().hasEcnamInfo()) {
    ril::socket::utils::convertToSocket(preAlertingInfo.ecnamInfo,
                                        msg->getPreAlertingInfo().getEcnamInfo());
    has_preAleringInfo = true;
  }

  if (has_preAleringInfo) {
    if (parcel->write(preAlertingInfo) == Marshal::Result::SUCCESS) {
      if (sendUnsolResponse(RIL_CALL_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE, parcel)) {
        QCRIL_LOG_ERROR("Failed to send RIL_CALL_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE.");
      }
    } else {
      QCRIL_LOG_ERROR("Failed to write to parcel.");
    }
  }
  ril::socket::utils::release(preAlertingInfo);

  // FR76398: DEPRECATED: START
  // Remove in the next release
  {
  RIL_IMS_PreAlertingCallInfo preAlertingInfo{};
  bool has_preAleringInfo = false;

  preAlertingInfo.callId = msg->getPreAlertingInfo().getCallId();
  if (msg->getPreAlertingInfo().hasCallComposerInfo()) {
    ril::socket::utils::convertToSocket(preAlertingInfo.ccInfo,
                                        msg->getPreAlertingInfo().getCallComposerInfo());
    has_preAleringInfo = true;
  }
  if (msg->getPreAlertingInfo().hasEcnamInfo()) {
    ril::socket::utils::convertToSocket(preAlertingInfo.ecnamInfo,
                                        msg->getPreAlertingInfo().getEcnamInfo());
    has_preAleringInfo = true;
  }

  if (has_preAleringInfo) {
    if (parcel->write(preAlertingInfo) == Marshal::Result::SUCCESS) {
      if (sendUnsolResponse(RIL_IMS_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE, parcel)) {
        QCRIL_LOG_ERROR("Failed to send RIL_IMS_UNSOL_PRE_ALERTING_CALL_INFO_AVAILABLE.");
      }
    } else {
      QCRIL_LOG_ERROR("Failed to write to parcel.");
    }
  }
  ril::socket::utils::release(preAlertingInfo);
  }
}
/*
* Sends Add profile progress indication for Lpa
*/
void RilSocketIndicationModule::handleAddProfProgInd(
        std::shared_ptr<UimLpaIndicationMsg> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    if (msg) {
        std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
        if (parcel) {
            if(msg->get_data_ptr() != nullptr) {
               auto ind_data_ptr = (lpa_service_add_profile_progress_ind_type *)msg->get_data_ptr();
               UimLpaAddProfProgressInd unsol{
                   .status = (UimAddProfStatusType)ind_data_ptr->status,
                     .cause = (UimLpaAddProfFailCause)ind_data_ptr->cause,
                       .progress =  ind_data_ptr->progress,
                        .policyMask = (UimLpaProfilePolicyMask)ind_data_ptr->policyMask,
                        .userConsentRequired = ind_data_ptr->userConsentRequired,
                        .profileName = ind_data_ptr->profile_name,
               };
               if (parcel->write(unsol) == Marshal::Result::SUCCESS) {
                   sendUnsolResponse(RIL_UNSOL_LPA_ADD_PROF_PROG_IND, parcel);
               }
            }
        }
    }
    return;
}

void RilSocketIndicationModule::handleLpaHttpsTxnInd(
        std::shared_ptr<LpaQmiUimHttpIndicationMsg> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    if (msg) {
        QCRIL_LOG_INFO("msg is not null");
        std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
        if (parcel) {
            QCRIL_LOG_INFO("QMI UIM HTTP Indication : %d", msg->get_ind_id());
            uim_http_transaction_ind_msg  * ind_ptr = (uim_http_transaction_ind_msg *)msg->get_message();
            UimLpaHttpTxnIndype unsol;
            int i = 0;
            if(ind_ptr != nullptr) {
                unsol.tokenId =  ind_ptr->token_id;
                unsol.payload =  ind_ptr->payload_ptr;
                    unsol.payload_len =   ind_ptr->payload_len;
                 if (ind_ptr->url_valid){
                     unsol.url = ind_ptr->url;
                     QCRIL_LOG_INFO("QMI UIM HTTP Indication url : %s", unsol.url);
                 }
                 if(ind_ptr->headers_valid) {
                     unsol.contentType = ind_ptr->headers.content_type;
                     unsol.no_of_headers =  ind_ptr->headers.custom_header_len;
                     unsol.customHeaders = new UimLpaHttpCustomHdrType[unsol.no_of_headers];
                     for(i = 0;i<unsol.no_of_headers;i++){
                         unsol.customHeaders[i].headerName = ind_ptr->headers.custom_header[i].name;
                         unsol.customHeaders[i].headerValue = ind_ptr->headers.custom_header[i].value;
                     }
                 }
                 if (parcel->write(unsol) == Marshal::Result::SUCCESS) {
                     QCRIL_LOG_INFO("Sending data onto the socket");
                     int ret = sendUnsolResponse(RIL_UNSOL_LPA_HTTP_TXN_IND, parcel);
                     if(ret < 0) {
                         QCRIL_LOG_INFO("Failed to send indication %d",ret);
                     }
                 }
            }
        }
    }
    QCRIL_LOG_FUNC_RETURN();
}

void RilSocketIndicationModule::handleSlicingConfigChangedInd(std::shared_ptr<rildata::SlicingConfigChangedIndMessage> msg)
{
    if (msg == nullptr) {
        QCRIL_LOG_INFO("msg is nullptr");
        return;
    }
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
    std::shared_ptr<Marshal> parcel = std::make_shared<Marshal>();
    if (parcel != nullptr) {
        if (msg->getSlicingConfig().has_value()) {
            rilSlicingConfig slicingConfig = {};
            rildata::SlicingConfig_t config = *(msg->getSlicingConfig());
            ril::socket::utils::convertToSocket(slicingConfig, config);
            if (parcel->write(slicingConfig) == Marshal::Result::SUCCESS) {
                QCRIL_LOG_INFO("Sending data onto the socket");
                int ret = sendUnsolResponse(RIL_UNSOL_SLICING_CONFIG_CHANGED_IND, parcel);
                if(ret < 0) {
                    QCRIL_LOG_INFO("Failed to send indication %d",ret);
                }
            } else {
                QCRIL_LOG_INFO("Parcel Write Failed");
            }
        } else {
            QCRIL_LOG_INFO("No Data in the indication");
        }
    } else {
        QCRIL_LOG_INFO("parcel is nullptr");
    }
    QCRIL_LOG_FUNC_RETURN();
}
