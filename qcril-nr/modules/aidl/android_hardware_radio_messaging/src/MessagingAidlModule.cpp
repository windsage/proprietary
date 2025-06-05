/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "MessagingAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>

static load_module<MessagingAidlModule> sMessagingAidlModule;

MessagingAidlModule* getMessagingAidlModule() {
  return &(sMessagingAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
MessagingAidlModule::MessagingAidlModule() {
  mName = "MessagingAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, MessagingAidlModule::handleQcrilInit),
    HANDLER(RilUnsolIncoming3GppSMSMessage, MessagingAidlModule::handleIncoming3GppSMSMessage),
    HANDLER(RilUnsolIncoming3Gpp2SMSMessage, MessagingAidlModule::handleIncoming3Gpp2SMSMessage),
    HANDLER(RilUnsolNewSmsOnSimMessage, MessagingAidlModule::handleNewSmsOnSimMessage),
    HANDLER(RilUnsolNewBroadcastSmsMessage, MessagingAidlModule::handleNewBroadcastSmsMessage),
    HANDLER(RilUnsolNewSmsStatusReportMessage, MessagingAidlModule::handleNewSmsStatusReportMessage),
    HANDLER(RilUnsolCdmaRuimSmsStorageFullMessage,
            MessagingAidlModule::handleCdmaRuimSmsStorageFullMessage),
    HANDLER(RilUnsolSimSmsStorageFullMessage, MessagingAidlModule::handleSimSmsStorageFullMessage),
  };
}

/* Follow RAII.
 */
MessagingAidlModule::~MessagingAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void MessagingAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void MessagingAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void MessagingAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
  // Register Stable AIDL Interface.
  if (mIRadioMessagingAidlImpl == nullptr) {
    mIRadioMessagingAidlImpl = ndk::SharedRefBase::make<IRadioMessagingImpl>(instance_id);
    const std::string instance =
        std::string(IRadioMessagingImpl::descriptor) + "/slot" + std::to_string(instance_id + 1);
    Log::getInstance().d("instance=" + instance);
    binder_status_t status =
        AServiceManager_addService(mIRadioMessagingAidlImpl->asBinder().get(), instance.c_str());
    QCRIL_LOG_INFO("IRadioMessaging Stable AIDL addService, status= %d", status);
    if (status != STATUS_OK) {
      mIRadioMessagingAidlImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s slot%d",
                     IRadioMessagingImpl::descriptor,
                     instance_id + 1);
    }
  }
}

void MessagingAidlModule::handleIncoming3GppSMSMessage(
    std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendNewSms(msg);
  }
}

void MessagingAidlModule::handleIncoming3Gpp2SMSMessage(
    std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendNewCdmaSms(msg);
  }
}

void MessagingAidlModule::handleNewSmsOnSimMessage(
    std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendNewSmsOnSim(msg);
  }
}

void MessagingAidlModule::handleNewBroadcastSmsMessage(
    std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendNewBroadcastSms(msg);
  }
}

void MessagingAidlModule::handleNewSmsStatusReportMessage(
    std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendNewSmsStatusReport(msg);
  }
}

void MessagingAidlModule::handleCdmaRuimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendCdmaRuimSmsStorageFull(msg);
  }
}

void MessagingAidlModule::handleSimSmsStorageFullMessage(
    std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioMessagingAidlImpl) {
    mIRadioMessagingAidlImpl->sendSimSmsStorageFull(msg);
  }
}
