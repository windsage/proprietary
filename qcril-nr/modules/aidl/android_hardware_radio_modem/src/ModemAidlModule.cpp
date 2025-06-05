/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "ModemAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>

static load_module<ModemAidlModule> sModemAidlModule;

ModemAidlModule* getModemAidlModule() {
  return &(sModemAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
ModemAidlModule::ModemAidlModule() {
  mName = "ModemAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, ModemAidlModule::handleQcrilInit),
    HANDLER(RilUnsolModemRestartMessage, ModemAidlModule::handleModemRestartMessage),
    HANDLER(RilUnsolRadioCapabilityMessage, ModemAidlModule::handlelRadioCapabilityMessage),
    HANDLER(RilUnsolRadioStateChangedMessage, ModemAidlModule::handleRadioStateChangedMessage),
    HANDLER(RilUnsolRilConnectedMessage, ModemAidlModule::handleRilUnsolRilConnectedMessage),
  };
}

/* Follow RAII.
 */
ModemAidlModule::~ModemAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void ModemAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void ModemAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void ModemAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
  // Register Stable AIDL Interface.
  if (mIRadioModemAidlImpl == nullptr) {
    mIRadioModemAidlImpl = ndk::SharedRefBase::make<IRadioModemImpl>(instance_id);
    const std::string instance =
        std::string(IRadioModemImpl::descriptor) + "/slot" + std::to_string(instance_id + 1);
    Log::getInstance().d("instance=" + instance);
    binder_status_t status =
        AServiceManager_addService(mIRadioModemAidlImpl->asBinder().get(), instance.c_str());
    QCRIL_LOG_INFO("IRadioModem Stable AIDL addService, status= %d", status);
    if (status != STATUS_OK) {
      mIRadioModemAidlImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s slot%d",
                     IRadioModemImpl::descriptor,
                     instance_id + 1);
    }
  }
}

void ModemAidlModule::handleModemRestartMessage(
    std::shared_ptr<RilUnsolModemRestartMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioModemAidlImpl) {
    mIRadioModemAidlImpl->sendModemReset(msg);
  }
}

void ModemAidlModule::handlelRadioCapabilityMessage(
    std::shared_ptr<RilUnsolRadioCapabilityMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioModemAidlImpl) {
    mIRadioModemAidlImpl->sendRadioCapability(msg);
  }
}

void ModemAidlModule::handleRadioStateChangedMessage(
    std::shared_ptr<RilUnsolRadioStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioModemAidlImpl) {
    mIRadioModemAidlImpl->sendRadioStateChanged(msg);
  }
}

void ModemAidlModule::handleRilUnsolRilConnectedMessage(
    std::shared_ptr<RilUnsolRilConnectedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioModemAidlImpl) {
    mIRadioModemAidlImpl->sendRilConnected(msg);
  }
}
