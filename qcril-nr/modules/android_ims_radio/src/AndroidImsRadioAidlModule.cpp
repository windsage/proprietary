/******************************************************************************
#  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "RILQ"

#include <AndroidImsRadioAidlModule.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>
#include "qcril_other.h"

#include "aidl_impl/ims_radio_aidl_utils.h"

static load_module<AndroidImsRadioAidlModule> sAndroidImsRadioAidlModule;

AndroidImsRadioAidlModule* getAndroidImsRadioAidlModule() {
  return &(sAndroidImsRadioAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
AndroidImsRadioAidlModule::AndroidImsRadioAidlModule() {
  mName = "AndroidImsRadioAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, AndroidImsRadioAidlModule::handleQcrilInit),
    HANDLER(QcRilUnsolImsRegStateMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsRegStateMessage),
    HANDLER(QcRilUnsolImsRttMessage, AndroidImsRadioAidlModule::handleQcRilUnsolImsRttMessage),
    HANDLER(QcRilUnsolImsVowifiCallQuality,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsVowifiCallQuality),
    HANDLER(QcRilUnsolImsVopsIndication, AndroidImsRadioAidlModule::handleQcRilUnsolVopsMessage),
    HANDLER(QcRilUnsolImsSubConfigIndication,
            AndroidImsRadioAidlModule::handleQcRilUnsolSubConfigIndication),
    HANDLER(QcRilUnsolImsGeoLocationInfo,
            AndroidImsRadioAidlModule::handleQcRilUnsolGeoLocationChange),
    HANDLER(QcRilUnsolImsSrvStatusIndication,
            AndroidImsRadioAidlModule::handleQcRilUnsolServiceStatusChange),
    HANDLER(QcRilUnsolImsSsacInfoIndication,
            AndroidImsRadioAidlModule::handleQcRilUnsolSsacInfoIndication),
    HANDLER(QcRilUnsolImsRegBlockStatusMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolRegBlockChange),
    HANDLER(QcRilUnsolCallStateChangeMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolCallStateChangeMessage),
    HANDLER(QcRilUnsolImsModifyCallMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsModifyCallMessage),
    HANDLER(QcRilUnsolCallRingingMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolCallRingingMessage),
    HANDLER(QcRilUnsolRingbackToneMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolRingbackToneMessage),
    HANDLER(QcRilUnsolImsHandoverMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsHandoverMessage),
    HANDLER(QcRilUnsolImsTtyNotificationMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsTtyNotificationMessage),
    HANDLER(QcRilUnsolImsConferenceInfoMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsConferenceInfoMessage),
    HANDLER(QcRilUnsolImsViceInfoMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsViceInfoMessage),
    HANDLER(QcRilUnsolConfParticipantStatusInfoMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolConfParticipantStatusInfoMessage),
    HANDLER(QcRilUnsolAutoCallRejectionMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolAutoCallRejectionMessage),
    HANDLER(QcRilUnsolSuppSvcNotificationMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolSuppSvcNotificationMessage),
    HANDLER(QcRilUnsolSupplementaryServiceMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolSupplementaryServiceMessage),
    HANDLER(RilUnsolIncomingImsSMSMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolIncomingSmsMessage),
    HANDLER(RilUnsolNewImsSmsStatusReportMessage,
            AndroidImsRadioAidlModule::handleQcRilNewSmsStatusReportMessage),
    HANDLER(QcRilUnsolImsPendingMultiLineStatus,
            AndroidImsRadioAidlModule::handleQcRilUnsolPendingMultiLineStatus),
    HANDLER(QcRilUnsolImsMultiIdentityStatusMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolMultiIdentityStatus),
    HANDLER(QcRilUnsolImsVoiceInfo, AndroidImsRadioAidlModule::handleQcRilUnsolVoiceInfoChange),
    HANDLER(RilUnsolEmergencyCallbackModeMessage,
            AndroidImsRadioAidlModule::handleRilUnsolEmergencyCallbackModeMessage),
    HANDLER(QcRilUnsolImsRadioStateIndication,
            AndroidImsRadioAidlModule::handleQcRilUnsolRadioStateIndication),
    HANDLER(QcRilUnsolMessageWaitingInfoMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolMessageWaitingInfoMessage),
    HANDLER(QcRilUnsolImsWfcRoamingConfigIndication,
            AndroidImsRadioAidlModule::handleQcRilUnsolWfcRoamingConfigSupport),
    HANDLER(QcRilUnsolOnUssdMessage, AndroidImsRadioAidlModule::handleQcRilUnsolOnUssdMessage),
    HANDLER(QcRilUnsolImsGeoLocationDataStatus,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsGeoLocationDataStatus),
    HANDLER(QcRilUnsolOnSipDtmfMessage, AndroidImsRadioAidlModule::handleQcRilUnsolOnSipDtmfMessage),
    HANDLER(QcRilUnsolImsConferenceCallStateCompletedMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsConferenceCallStateCompletedMessage),
    HANDLER(RilUnsolNasSysInfo, AndroidImsRadioAidlModule::handleQcRilUnsolNasSysInfo),
    HANDLER(QcRilUnsolImsSmsCallbackModeMessage,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsSmsCallbackModeMessage),
    HANDLER(QcRilUnsolDtmfMessage, AndroidImsRadioAidlModule::handleQcRilUnsolDtmfMessage),
    HANDLER(QcRilUnsolImsMultiSimVoiceCapabilityChanged,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsMultiSimVoiceCapabilityChanged),
    HANDLER(QcRilUnsolImsPreAlertingCallInfo,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsPreAlertingCallInfo),
    HANDLER(QcRilUnsolImsCIwlanNotification,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsCIwlanNotification),
    HANDLER(QcRilUnsolImsSrtpEncryptionStatus,
            AndroidImsRadioAidlModule::handleQcRilUnsolImsSrtpEncryptionStatus)
  };
}

/* Follow RAII.
 */
AndroidImsRadioAidlModule::~AndroidImsRadioAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void AndroidImsRadioAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void AndroidImsRadioAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Init ims radio services.*/
  initImsRadio(msg->get_instance_id());
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsRegStateMessage(
    std::shared_ptr<QcRilUnsolImsRegStateMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRegistrationChanged(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsRttMessage(
    std::shared_ptr<QcRilUnsolImsRttMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRTTMessage(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsVowifiCallQuality(
    std::shared_ptr<QcRilUnsolImsVowifiCallQuality> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnVowifiCallQuality(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolVopsMessage(
    std::shared_ptr<QcRilUnsolImsVopsIndication> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnVopsChanged(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolSubConfigIndication(
    std::shared_ptr<QcRilUnsolImsSubConfigIndication> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnSubConfigChanged(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolGeoLocationChange(
    std::shared_ptr<QcRilUnsolImsGeoLocationInfo> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnGeoLocationChange(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolServiceStatusChange(
    std::shared_ptr<QcRilUnsolImsSrvStatusIndication> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnServiceStatusChange(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolSsacInfoIndication(
    std::shared_ptr<QcRilUnsolImsSsacInfoIndication> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnSsacInfoChange(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolRegBlockChange(
    std::shared_ptr<QcRilUnsolImsRegBlockStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRegBlockStatusChange(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolCallStateChangeMessage(
    std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && msg->hasImsCalls()) {
    if (mImsRadio != nullptr) {
      mImsRadio->notifyOnCallStateChanged(msg);
    }
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsModifyCallMessage(
    std::shared_ptr<QcRilUnsolImsModifyCallMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnModifyCall(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolCallRingingMessage(
    std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && msg->isIms()) {
    if (mImsRadio != nullptr) {
      mImsRadio->notifyOnRing(msg);
    }
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolRingbackToneMessage(
    std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && msg->isIms()) {
    if (mImsRadio != nullptr) {
      mImsRadio->notifyOnRingbackTone(msg);
    }
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsHandoverMessage(
    std::shared_ptr<QcRilUnsolImsHandoverMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnHandover(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsTtyNotificationMessage(
    std::shared_ptr<QcRilUnsolImsTtyNotificationMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnTtyNotification(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsConferenceInfoMessage(
    std::shared_ptr<QcRilUnsolImsConferenceInfoMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRefreshConferenceInfo(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsViceInfoMessage(
    std::shared_ptr<QcRilUnsolImsViceInfoMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRefreshViceInfo(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolConfParticipantStatusInfoMessage(
    std::shared_ptr<QcRilUnsolConfParticipantStatusInfoMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnParticipantStatusInfo(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolAutoCallRejectionMessage(
    std::shared_ptr<QcRilUnsolAutoCallRejectionMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnIncomingCallAutoRejected(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolSuppSvcNotificationMessage(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio && msg && msg->isIms()) {
    mImsRadio->notifyOnSuppServiceNotification(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolSupplementaryServiceMessage(
    std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio && msg && msg->isIms()) {
    mImsRadio->notifyOnSupplementaryServiceIndication(msg);
  }
}
void AndroidImsRadioAidlModule::handleQcRilUnsolPendingMultiLineStatus(
    std::shared_ptr<QcRilUnsolImsPendingMultiLineStatus> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnPendingMultiIdentityStatus(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolMultiIdentityStatus(
    std::shared_ptr<QcRilUnsolImsMultiIdentityStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnMultiIdentityLineStatus(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolIncomingSmsMessage(
    std::shared_ptr<RilUnsolIncomingImsSMSMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyIncomingSms(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilNewSmsStatusReportMessage(
    std::shared_ptr<RilUnsolNewImsSmsStatusReportMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyNewSmsStatusReport(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolVoiceInfoChange(
    std::shared_ptr<QcRilUnsolImsVoiceInfo> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnVoiceInfoStatusChange(msg);
  }
}

void AndroidImsRadioAidlModule::handleRilUnsolEmergencyCallbackModeMessage(
    std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg->isEmergencyIp()) {
    if (mImsRadio != nullptr) {
      if (msg->getEmergencyCallbackMode() ==
          RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::ENTER) {
        mImsRadio->notifyOnEnterEcbmIndication(msg);
      } else if (msg->getEmergencyCallbackMode() ==
                 RilUnsolEmergencyCallbackModeMessage::EmergencyCallbackMode::LEAVE) {
        mImsRadio->notifyOnExitEcbmIndication(msg);
      }
    }
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolRadioStateIndication(
    std::shared_ptr<QcRilUnsolImsRadioStateIndication> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRadioStateChanged(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolMessageWaitingInfoMessage(
    std::shared_ptr<QcRilUnsolMessageWaitingInfoMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnMessageWaiting(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolWfcRoamingConfigSupport(
    std::shared_ptr<QcRilUnsolImsWfcRoamingConfigIndication> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnModemSupportsWfcRoamingModeConfiguration(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolOnUssdMessage(
    std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    if (vendor::qti::hardware::radio::ims::utils::isUssdOverImsSupported()) {
      if (msg->isImsDomain()) {
        mImsRadio->notifyOnUssdMessage(msg);
      }
    } else {
      if (msg->hasErrorDetails()) {
        mImsRadio->notifyOnUssdMessageFailed(msg);
      }
    }
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsGeoLocationDataStatus(
    std::shared_ptr<QcRilUnsolImsGeoLocationDataStatus> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnRetrievingGeoLocationDataStatus(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolOnSipDtmfMessage(
    std::shared_ptr<QcRilUnsolOnSipDtmfMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnSipDtmfReceived(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsConferenceCallStateCompletedMessage(
    std::shared_ptr<QcRilUnsolImsConferenceCallStateCompletedMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnConferenceCallStateCompleted(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolNasSysInfo(
    std::shared_ptr<RilUnsolNasSysInfo> msg) {
   Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
   if (mImsRadio != nullptr) {
     mImsRadio->notifyOnServiceDomainChanged(msg);
   }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsSmsCallbackModeMessage(
    std::shared_ptr<QcRilUnsolImsSmsCallbackModeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnSmsCallBackModeChanged(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolDtmfMessage(
    std::shared_ptr<QcRilUnsolDtmfMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio == nullptr) {
    Log::getInstance().d("mImsRadio is null.");
    return;
  }
  switch (msg->getDtmfEvent()) {
    case qcril::interfaces::DtmfEvent::IP_INCOMING_DTMF_START:
      mImsRadio->notifyOnIncomingDtmfStart(msg);
      break;
    case qcril::interfaces::DtmfEvent::IP_INCOMING_DTMF_STOP:
      mImsRadio->notifyOnIncomingDtmfStop(msg);
      break;
    case qcril::interfaces::DtmfEvent::UNKNOWN:
    default:
      break;
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsMultiSimVoiceCapabilityChanged(
    std::shared_ptr<QcRilUnsolImsMultiSimVoiceCapabilityChanged> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnMultiSimVoiceCapabilityChanged(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsPreAlertingCallInfo(
    std::shared_ptr<QcRilUnsolImsPreAlertingCallInfo> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnPreAlertingCallInfo(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsCIwlanNotification(
    std::shared_ptr<QcRilUnsolImsCIwlanNotification> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifyOnCIWlanNotification(msg);
  }
}

void AndroidImsRadioAidlModule::handleQcRilUnsolImsSrtpEncryptionStatus(
    std::shared_ptr<QcRilUnsolImsSrtpEncryptionStatus> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mImsRadio != nullptr) {
    mImsRadio->notifySrtpEnscryptionStatus(msg);
  }
}

void AndroidImsRadioAidlModule::initImsRadio(qcril_instance_id_e_type instance_id) {
  // Register Stable AIDL Interface.
  if (mImsRadio == nullptr) {
    mImsRadio =
        ndk::SharedRefBase::make<vendor::qti::hardware::radio::ims::implementation::ImsRadioAidlImpl>(
            instance_id);
    const std::string instance =
        std::string() +
        vendor::qti::hardware::radio::ims::implementation::ImsRadioAidlImpl::descriptor +
        "/imsradio" + std::to_string(instance_id);
    Log::getInstance().d("instance=" + instance);
    binder_status_t status =
        AServiceManager_addService(mImsRadio->asBinder().get(), instance.c_str());
    QCRIL_LOG_INFO("IMS Stable AIDL addService, status= %d", status);
    if (status != STATUS_OK) {
      mImsRadio = nullptr;
      QCRIL_LOG_INFO("Error registering service %s %d",
                     vendor::qti::hardware::radio::ims::implementation::ImsRadioAidlImpl::descriptor,
                     instance_id);
    }
  }
}
