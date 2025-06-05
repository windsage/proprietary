/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "ConfigAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>

static load_module<ConfigAidlModule> sConfigAidlModule;

ConfigAidlModule* getConfigAidlModule() {
  return &(sConfigAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
ConfigAidlModule::ConfigAidlModule() {
  mName = "RadioConfigAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, ConfigAidlModule::handleQcrilInit),
    HANDLER(UimSlotStatusInd, ConfigAidlModule::handleSlotStatusIndication),
  };
}

/* Follow RAII.
 */
ConfigAidlModule::~ConfigAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void ConfigAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void ConfigAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void ConfigAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
    // Register Stable AIDL Interface.
  if (instance_id == QCRIL_DEFAULT_INSTANCE_ID) {
    if (mIRadioConfigAidlImpl == nullptr) {
      mIRadioConfigAidlImpl = ndk::SharedRefBase::make<IRadioConfigImpl>(instance_id);
      const std::string instance =
          std::string(IRadioConfigImpl::descriptor) + "/default";
      Log::getInstance().d("instance=" + instance);
      binder_status_t status =
          AServiceManager_addService(mIRadioConfigAidlImpl->asBinder().get(), instance.c_str());
      QCRIL_LOG_INFO("IConfig Stable AIDL addService, status= %d", status);
      if (status != STATUS_OK) {
        mIRadioConfigAidlImpl = nullptr;
        QCRIL_LOG_INFO("Error registering service %s %d",
                       IRadioConfigImpl::descriptor,
                       instance_id);
      }
    }
  }
}

void ConfigAidlModule::handleSlotStatusIndication(std::shared_ptr<UimSlotStatusInd> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIRadioConfigAidlImpl) {
    mIRadioConfigAidlImpl->sendSlotStatusIndication(msg);
  }
}
