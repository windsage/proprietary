/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "VoiceAidlModule.h"
#include "voice_aidl_service_utils.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>

static load_module<VoiceAidlModule> sVoiceAidlModule;

VoiceAidlModule* getVoiceAidlModule() {
  return &(sVoiceAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
VoiceAidlModule::VoiceAidlModule() {
  mName = "VoiceAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, VoiceAidlModule::handleQcrilInit),
    HANDLER(QcRilUnsolCallRingingMessage,
        VoiceAidlModule::handleQcRilUnsolCallRingingMessage),
    HANDLER(QcRilUnsolCallStateChangeMessage,
        VoiceAidlModule::handleQcRilUnsolCallStateChangeMessage),
    HANDLER(QcRilUnsolCdmaCallWaitingMessage,
        VoiceAidlModule::handleQcRilUnsolCdmaCallWaitingMessage),
    HANDLER(QcRilUnsolCdmaInfoRecordMessage,
        VoiceAidlModule::handleQcRilUnsolCdmaInfoRecordMessage),
    HANDLER(QcRilUnsolCdmaOtaProvisionStatusMessage,
        VoiceAidlModule::handleQcRilUnsolCdmaOtaProvisionStatusMessage),
    HANDLER(RilUnsolEmergencyCallbackModeMessage,
        VoiceAidlModule::handleRilUnsolEmergencyCallbackModeMessage),
    HANDLER(QcRilUnsolRingbackToneMessage,
        VoiceAidlModule::handleQcRilUnsolRingbackToneMessage),
    HANDLER(QcRilUnsolSupplementaryServiceMessage,
        VoiceAidlModule::handleQcRilUnsolSupplementaryServiceMessage),
    HANDLER(QcRilUnsolSrvccStatusMessage,
        VoiceAidlModule::handleQcRilUnsolSrvccStatusMessage),
    HANDLER(RilUnsolStkCCAlphaNotifyMessage,
        VoiceAidlModule::handleRilUnsolStkCCAlphaNotifyMessage),
    HANDLER(RilUnsolEmergencyListIndMessage,
        VoiceAidlModule::handleRilUnsolEmergencyListIndMessage),
    HANDLER(GstkUnsolIndMsg, VoiceAidlModule::handleGstkUnsolIndMsg),
    HANDLER(QcRilUnsolOnUssdMessage,
        VoiceAidlModule::handleQcRilUnsolOnUssdMessage),
  };
}

/* Follow RAII.
 */
VoiceAidlModule::~VoiceAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void VoiceAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void VoiceAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void VoiceAidlModule::handleQcRilUnsolCallRingingMessage(
    std::shared_ptr<QcRilUnsolCallRingingMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && !msg->isIms()) {
    if (mIRadioVoiceAidlImpl != nullptr) {
      mIRadioVoiceAidlImpl->callRing(msg);
    }
  }
}

void VoiceAidlModule::handleQcRilUnsolCallStateChangeMessage(
    std::shared_ptr<QcRilUnsolCallStateChangeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && (msg->getCallInfo().empty() || msg->hasCsCalls())) {
    if (mIRadioVoiceAidlImpl != nullptr) {
      mIRadioVoiceAidlImpl->callStateChanged(msg);
    }
  }
}

void VoiceAidlModule::handleQcRilUnsolCdmaCallWaitingMessage(
    std::shared_ptr<QcRilUnsolCdmaCallWaitingMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendCdmaCallWaiting(msg);
  }
}

void VoiceAidlModule::handleQcRilUnsolCdmaInfoRecordMessage(
    std::shared_ptr<QcRilUnsolCdmaInfoRecordMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendCdmaInfoRec(msg);
  }
}

void VoiceAidlModule::handleQcRilUnsolCdmaOtaProvisionStatusMessage(
    std::shared_ptr<QcRilUnsolCdmaOtaProvisionStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendCdmaOtaProvisionStatus(msg);
  }
}

void VoiceAidlModule::handleRilUnsolEmergencyCallbackModeMessage(
    std::shared_ptr<RilUnsolEmergencyCallbackModeMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (!msg->isEmergencyIp()) {
    if (mIRadioVoiceAidlImpl != nullptr) {
      mIRadioVoiceAidlImpl->sendEmergencyCallbackMode(msg);
    }
  }
}

void VoiceAidlModule::handleQcRilUnsolRingbackToneMessage(
    std::shared_ptr<QcRilUnsolRingbackToneMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && !msg->isIms()) {
    if (mIRadioVoiceAidlImpl != nullptr) {
      mIRadioVoiceAidlImpl->sendIndicateRingbackTone(msg);
    }
  }
}

void VoiceAidlModule::handleQcRilUnsolSupplementaryServiceMessage(
    std::shared_ptr<QcRilUnsolSupplementaryServiceMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && !msg->isIms()) {
    if (mIRadioVoiceAidlImpl != nullptr) {
      mIRadioVoiceAidlImpl->sendOnSupplementaryServiceIndication(msg);
    }
  }
}

void VoiceAidlModule::handleQcRilUnsolSrvccStatusMessage(
    std::shared_ptr<QcRilUnsolSrvccStatusMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendSrvccStateNotify(msg);
  }
}

void VoiceAidlModule::handleRilUnsolStkCCAlphaNotifyMessage(
    std::shared_ptr<RilUnsolStkCCAlphaNotifyMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendStkCCAlphaNotify(msg);
  }
}

void VoiceAidlModule::handleRilUnsolEmergencyListIndMessage(
    std::shared_ptr<RilUnsolEmergencyListIndMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendCurrentEmergencyNumberList(msg);
  }
}

void VoiceAidlModule::handleGstkUnsolIndMsg(
    std::shared_ptr<GstkUnsolIndMsg> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioVoiceAidlImpl != nullptr) {
    mIRadioVoiceAidlImpl->sendGstkIndication(msg);
  }
}

void VoiceAidlModule::handleQcRilUnsolOnUssdMessage(
    std::shared_ptr<QcRilUnsolOnUssdMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (msg && (msg->isCsDomain() || !isUssdOverImsSupported())) {
    if (mIRadioVoiceAidlImpl) {
      mIRadioVoiceAidlImpl->sendOnUssd(msg);
    }
  }
}

void VoiceAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
  // Register Stable AIDL Interface.
  if (mIRadioVoiceAidlImpl == nullptr) {
    mIRadioVoiceAidlImpl = ndk::SharedRefBase::make<IRadioVoiceImpl>(instance_id);
    const std::string instance =
        std::string(IRadioVoiceImpl::descriptor) + "/slot" + std::to_string(instance_id + 1);
    Log::getInstance().d("instance=" + instance);
    binder_status_t status =
        AServiceManager_addService(mIRadioVoiceAidlImpl->asBinder().get(), instance.c_str());
    QCRIL_LOG_INFO("IRadioVoice Stable AIDL addService, status= %d", status);
    if (status != STATUS_OK) {
      mIRadioVoiceAidlImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s slot%d",
                     IRadioVoiceImpl::descriptor,
                     instance_id + 1);
    }
  }
}
