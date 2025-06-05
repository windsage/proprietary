/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "OemHookStableAidlModule.h"

#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <cstring>

#include "qcril_legacy_apis.h"

#undef  TAG
#define TAG "RILQ"

static load_module<OemHookStableAidlModule> sOemHookStableAidlModule;

OemHookStableAidlModule& getOemHookStableAidlModule() {
  return (sOemHookStableAidlModule.get_module());
}

OemHookStableAidlModule::OemHookStableAidlModule() {
    mName = "OemHookStableAidlModule";

    using std::placeholders::_1;
    mMessageHandler = {
        HANDLER(QcrilInitMessage, OemHookStableAidlModule::handleQcrilInit),
        HANDLER(QcRilUnsolAdnRecordsOnSimMessage, OemHookStableAidlModule::handleAdnRecordsOnSimMessage),
        HANDLER(QcRilUnsolAdnInitDoneMessage, OemHookStableAidlModule::handleAdnInitDoneMessage),
        HANDLER(QcRilUnsolMbnConfigResultMessage, OemHookStableAidlModule::handleMbnConfigResultMessage),
        HANDLER(QcRilUnsolMbnConfigClearedMessage, OemHookStableAidlModule::handleMbnConfigClearedMessage),
        HANDLER(QcRilUnsolMbnValidateDumpedMessage, OemHookStableAidlModule::handleMbnValidateDumpedMessage),
        HANDLER(QcRilUnsolMbnConfigListMessage, OemHookStableAidlModule::handleMbnConfigListMessage),
        HANDLER(QcRilUnsolMbnValidateConfigMessage, OemHookStableAidlModule::handleMbnValidateConfigMessage),
        HANDLER(UimSimlockTempUnlockExpireInd, OemHookStableAidlModule::handleUimSimlockTempUnlockExpireInd),
        HANDLER(UimCardStateChangeInd, OemHookStableAidlModule::handleUimCardStateChangeInd),
        HANDLER(UimSlotStatusInd, OemHookStableAidlModule::handleUimSlotStatusInd),
        HANDLER(UimSimRefreshIndication, OemHookStableAidlModule::handleUimSimRefreshIndication),
        HANDLER(UimVoltageStatusInd, OemHookStableAidlModule::handleUimVoltageStatusInd),
        HANDLER(UimRmtRemoteSimStatusIndMsg, OemHookStableAidlModule::handleUimRmtRemoteSimStatusIndMsg),
        HANDLER(QcRilUnsolDtmfMessage, OemHookStableAidlModule::handleQcRilUnsolDtmfMessage),
        HANDLER(QcRilUnsolExtBurstIntlMessage, OemHookStableAidlModule::handleQcRilUnsolExtBurstIntlMessage),
        HANDLER(QcRilUnsolNssReleaseMessage, OemHookStableAidlModule::handleQcRilUnsolNssReleaseMessage),
        HANDLER(QcRilUnsolSuppSvcErrorCodeMessage, OemHookStableAidlModule::handleQcRilUnsolSuppSvcErrorCodeMessage),
        HANDLER(QcRilUnsolSpeechCodecInfoMessage, OemHookStableAidlModule::handleQcRilUnsolSpeechCodecInfoMessage),
        HANDLER(QcRilUnsolAudioStateChangedMessage, OemHookStableAidlModule::handleQcRilUnsolAudioStateChangedMessage),
        HANDLER(QcRilUnsolWmsReadyMessage, OemHookStableAidlModule::handleQcRilUnsolWmsReadyMessage),
        HANDLER(RilUnsolMaxActiveDataSubsChangedMessage,
                OemHookStableAidlModule::handleRilUnsolMaxActiveDataSubsChangedMessage),
        HANDLER(RilUnsolCsgIdChangedMessage, OemHookStableAidlModule::handleRilUnsolCsgIdChangedMessage),
        HANDLER(RilUnsolEngineerModeMessage, OemHookStableAidlModule::handleRilUnsolEngineerModeMessage),
        HANDLER(RilUnsolOemNetworkScanMessage,
                OemHookStableAidlModule::handleRilUnsolOemNetworkScanMessage),
        HANDLER(RilUnsolSubProvisioningStatusMessage,
                OemHookStableAidlModule::handleRilUnsolSubProvisionStatusMessage),

    };
}

void OemHookStableAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));

  if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_OEM_SOCKET)) {
    if (mOemHookStable == nullptr) {
      mOemHookStable = ndk::SharedRefBase::make<aidlimplimports::OemHookStable>();
      std::string serviceInstance = std::string(aidlimplimports::OemHookStable::descriptor) + "/oemhook" +
        std::to_string(instance_id);
      binder_status_t status =
        AServiceManager_addService(mOemHookStable->asBinder().get(), serviceInstance.c_str());
       QCRIL_LOG_INFO("OemHook Stable AIDL addService, status=%d", status);

       if (status != STATUS_OK) {
         mOemHookStable = nullptr;
         QCRIL_LOG_ERROR("Error registering service %s", serviceInstance.c_str());
       }
    }
  }
}

void OemHookStableAidlModule::handleAdnRecordsOnSimMessage(
        std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendAdnRecords(msg);
    }
}

void OemHookStableAidlModule::handleAdnInitDoneMessage(
        std::shared_ptr<QcRilUnsolAdnInitDoneMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendAdnInitDone(msg);
    }
}

void OemHookStableAidlModule::handleQcRilUnsolDtmfMessage(
        std::shared_ptr<QcRilUnsolDtmfMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
      switch (msg->getDtmfEvent()) {
        case qcril::interfaces::DtmfEvent::FWD_BURST:
          mOemHookStable->sendUnsolCdmaBurstDtmf(msg);
          break;
        case qcril::interfaces::DtmfEvent::FWD_START_CONT:
          mOemHookStable->sendUnsolCdmaContDtmfStart(msg);
          break;
        case qcril::interfaces::DtmfEvent::FWD_STOP_CONT:
          mOemHookStable->sendUnsolCdmaContDtmfStop(msg);
          break;
        case qcril::interfaces::DtmfEvent::UNKNOWN:
        default:
          break;
      }
    }
}

void OemHookStableAidlModule::handleQcRilUnsolExtBurstIntlMessage(
        std::shared_ptr<QcRilUnsolExtBurstIntlMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendUnsolExtendedDbmIntl(msg);
    }
}

void OemHookStableAidlModule::handleQcRilUnsolNssReleaseMessage(
        std::shared_ptr<QcRilUnsolNssReleaseMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendUnsolNssRelease(msg);
    }
}

void OemHookStableAidlModule::handleQcRilUnsolSuppSvcErrorCodeMessage(
        std::shared_ptr<QcRilUnsolSuppSvcErrorCodeMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendUnsolSsErrorCode(msg);
    }
}

void OemHookStableAidlModule::handleQcRilUnsolSpeechCodecInfoMessage(
        std::shared_ptr<QcRilUnsolSpeechCodecInfoMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendUnsolSpeechCodecInfo(msg);
    }
}

void OemHookStableAidlModule::handleMbnConfigResultMessage(
        std::shared_ptr<QcRilUnsolMbnConfigResultMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendMbnConfigResult(msg);
    }
}

void OemHookStableAidlModule::handleMbnConfigClearedMessage(
        std::shared_ptr<QcRilUnsolMbnConfigClearedMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendMbnConfigCleared(msg);
    }
}

void OemHookStableAidlModule::handleMbnValidateDumpedMessage(
        std::shared_ptr<QcRilUnsolMbnValidateDumpedMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendMbnValidateDumped(msg);
    }
}

void OemHookStableAidlModule::handleMbnConfigListMessage(
        std::shared_ptr<QcRilUnsolMbnConfigListMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendMbnConfigList(msg);
    }
}

void OemHookStableAidlModule::handleMbnValidateConfigMessage(
        std::shared_ptr<QcRilUnsolMbnValidateConfigMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendMbnValidateConfig(msg);
    }
}

void OemHookStableAidlModule::handleUimSimlockTempUnlockExpireInd(
        std::shared_ptr<UimSimlockTempUnlockExpireInd> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->uimSimlockTempUnlockExpireInd(msg);
    }
}

void OemHookStableAidlModule::handleUimCardStateChangeInd(
        std::shared_ptr<UimCardStateChangeInd> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->uimCardStateChangeInd(msg);
    }
}

void OemHookStableAidlModule::handleUimSlotStatusInd(
        std::shared_ptr<UimSlotStatusInd> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->uimSlotStatusInd(msg);
    }
}

void OemHookStableAidlModule::handleUimSimRefreshIndication(
        std::shared_ptr<UimSimRefreshIndication> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->uimSimRefreshIndication(msg);
    }
}

void OemHookStableAidlModule::handleUimVoltageStatusInd(
        std::shared_ptr<UimVoltageStatusInd> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->uimVoltageStatusInd(msg);
    }
}

void OemHookStableAidlModule::handleUimRmtRemoteSimStatusIndMsg(
        std::shared_ptr<UimRmtRemoteSimStatusIndMsg> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->uimRmtRemoteSimStatusIndMsg(msg);
    }
}

void OemHookStableAidlModule::handleQcRilUnsolAudioStateChangedMessage(
     std::shared_ptr<QcRilUnsolAudioStateChangedMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendUnsolAudioStateChanged(msg);
    }
}

void OemHookStableAidlModule::handleQcRilUnsolWmsReadyMessage(
        std::shared_ptr<QcRilUnsolWmsReadyMessage> msg) {
    QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());

    if (mOemHookStable) {
        mOemHookStable->sendUnsolWmsReady(msg);
    }
}

void OemHookStableAidlModule::handleRilUnsolMaxActiveDataSubsChangedMessage(
    std::shared_ptr<RilUnsolMaxActiveDataSubsChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  if (mOemHookStable) {
    mOemHookStable->sendUnsolMaxActiveDataSubsChanged(msg);
  }
}

void OemHookStableAidlModule::handleRilUnsolCsgIdChangedMessage(
    std::shared_ptr<RilUnsolCsgIdChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  if (mOemHookStable) {
    mOemHookStable->sendUnsolCsgIdChanged(msg);
  }
}

void OemHookStableAidlModule::handleRilUnsolEngineerModeMessage(
    std::shared_ptr<RilUnsolEngineerModeMessage> msg) {
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  if (mOemHookStable) {
    mOemHookStable->sendUnsolEngineerMode(msg);
  }
}

void OemHookStableAidlModule::handleRilUnsolOemNetworkScanMessage(
  std::shared_ptr<RilUnsolOemNetworkScanMessage> msg) {
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  if (mOemHookStable) {
    mOemHookStable->sendUnsolNetworkScanResult(msg);
  }
}

void OemHookStableAidlModule::handleRilUnsolSubProvisionStatusMessage(
  std::shared_ptr<RilUnsolSubProvisioningStatusMessage> msg) {
  QCRIL_LOG_INFO("Handling %s", msg->dump().c_str());
  if (mOemHookStable) {
    mOemHookStable->sendUnsolSubProvisionStatusChanged(msg);
  }
}

void OemHookStableAidlModule::sendIndication(uint8_t* buf, size_t bufLen) {
  QCRIL_LOG_INFO("sendIndication");
  if (mOemHookStable) {
    mOemHookStable->sendIndication(buf, bufLen);
  }
}
void OemHookStableAidlModule::sendResponse(int serial, RIL_Errno errorCode, uint8_t* buf,
                                        size_t bufLen) {
  QCRIL_LOG_INFO("sendResponse");
  if (mOemHookStable) {
    mOemHookStable->sendResponse(serial, errorCode, buf, bufLen);
  }
}

void sendOemhookAidlIndication(qcril_instance_id_e_type /*instId*/, uint8_t* buf,
                           size_t bufLen) {
  QCRIL_LOG_INFO("sendOemhookAidlIndication");
  getOemHookStableAidlModule().sendIndication(buf, bufLen);
}


void sendOemhookAidlResponse(qcril_instance_id_e_type /*instId*/, int serial, RIL_Errno errorCode,
                         uint8_t* buf, size_t bufLen) {
  QCRIL_LOG_INFO("sendOemhookAidlResponse");
  getOemHookStableAidlModule().sendResponse(serial, errorCode, buf, bufLen);
}
