/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "RILQ"

#include "cstring"
#include "android/binder_manager.h"
#include "android/binder_process.h"
#include "framework/Log.h"

#include "RadioDataModule.h"
#include "RadioServiceUtils.h"

static load_module<RadioDataModule> sRadioDataModule;

RadioDataModule* getRadioDataModule() {
  return &(sRadioDataModule.get_module());
}

RadioDataModule::RadioDataModule() {
  mName = "RadioDataModule";
  mMessageHandler = {
    HANDLER(QcrilInitMessage, RadioDataModule::handleQcrilInit),
    HANDLER(rildata::SlicingConfigChangedIndMessage, RadioDataModule::handleSlicingConfigChangedIndMessage),
    HANDLER(rildata::SetupDataCallRadioResponseIndMessage, RadioDataModule::handleSetupDataCallRadioResponseIndMessage),
    HANDLER(rildata::DeactivateDataCallRadioResponseIndMessage, RadioDataModule::handleDeactivateDataCallRadioResponseIndMessage),
    HANDLER(rildata::RadioDataCallListChangeIndMessage, RadioDataModule::handleRadioDataCallListChangeIndMessage),
    HANDLER(rildata::ThrottledApnTimerExpirationMessage, RadioDataModule::handleUnthrottleApnMessage),
    HANDLER(rildata::RadioKeepAliveStatusIndMessage,RadioDataModule::handleRadioKeepAliveStatusIndMessage),
    HANDLER(rildata::RilPCODataMessage, RadioDataModule::handlePCODataMessage),
  };
}

RadioDataModule::~RadioDataModule() {
}

void RadioDataModule::init() {
  Module::init();
}

void RadioDataModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void RadioDataModule::handleSlicingConfigChangedIndMessage(std::shared_ptr<rildata::SlicingConfigChangedIndMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendSlicingConfigChange(msg);
  }
}

void RadioDataModule::registerAidlService(qcril_instance_id_e_type instance_id) {
  if (mRadioDataServiceImpl == nullptr) {
    mRadioDataServiceImpl = ndk::SharedRefBase::make<RadioDataServiceImpl>(instance_id);
    const std::string instance = std::string(RadioDataServiceImpl::descriptor) + "/slot" + std::to_string(instance_id + 1);
    Log::getInstance().d("[" + mName + "] : instance=" + instance);
    binder_status_t status = AServiceManager_addService(mRadioDataServiceImpl->asBinder().get(), instance.c_str());
    Log::getInstance().d("[" + mName + "] RadioDataService Stable AIDL addService status = " + std::to_string(status));
    if (status != STATUS_OK) {
        mRadioDataServiceImpl = nullptr;
        Log::getInstance().d("RadioDataService failed to add service " + instance);
    }
  }
}

//Response APIs
void  RadioDataModule::handleSetupDataCallRadioResponseIndMessage(std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendResponseForSetupDataCall(msg);
  }
}

void RadioDataModule::handleDeactivateDataCallRadioResponseIndMessage(std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendResponseForDeactivateDataCall(msg);
  }
}

//Indication APIs
void RadioDataModule::handleRadioDataCallListChangeIndMessage(std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendRadioDataCallListChangeInd(msg);
  }
}

void RadioDataModule::handleUnthrottleApnMessage(std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendUnthrottleApnMessage(msg);
  }
}

void RadioDataModule::handleRadioKeepAliveStatusIndMessage(std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendKeepAliveStatusInd(msg);
  }
}

void RadioDataModule::handlePCODataMessage(std::shared_ptr<rildata::RilPCODataMessage> msg) {
  Log::getInstance().d("[" + mName + "] msg = " + (msg ? msg->dump() : ""));
  if(mRadioDataServiceImpl != nullptr) {
    mRadioDataServiceImpl->sendRilPCODataInd(msg);
  }
}
