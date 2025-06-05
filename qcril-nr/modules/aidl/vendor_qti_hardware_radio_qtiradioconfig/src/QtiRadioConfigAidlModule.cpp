/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "QtiRadioConfigAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>

static load_module<QtiRadioConfigAidlModule> sQtiRadioConfigAidlModule;

QtiRadioConfigAidlModule* getQtiRadioConfigAidlModule() {
  return &(sQtiRadioConfigAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
QtiRadioConfigAidlModule::QtiRadioConfigAidlModule() {
  mName = "RadioQtiRadioConfigAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, QtiRadioConfigAidlModule::handleQcrilInit),
    HANDLER(SecureModeStateChangeIndMessage, QtiRadioConfigAidlModule::handleSecureModeIndication),
    HANDLER(rildata::DataCapabilityChangeIndMessage,
        QtiRadioConfigAidlModule::handleCiwlanCapabilityChangeIndication),
    HANDLER(rildata::DataPPDataCapabilityChangeIndMessage,
        QtiRadioConfigAidlModule::handleDualDataCapabilityChangeIndication),
    HANDLER(rildata::DataSubRecommendationIndMessage,
        QtiRadioConfigAidlModule::handleDualDataSubRecommendationIndication),
  };
}

/* Follow RAII.
 */
QtiRadioConfigAidlModule::~QtiRadioConfigAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void QtiRadioConfigAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void QtiRadioConfigAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void QtiRadioConfigAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
    // Register Stable AIDL Interface.
  if (instance_id == QCRIL_DEFAULT_INSTANCE_ID) {
    if (mIQtiRadioConfigAidlImpl == nullptr) {
      mIQtiRadioConfigAidlImpl = ndk::SharedRefBase::make<IQtiRadioConfigImpl>(instance_id);
      const std::string instance =
          std::string(IQtiRadioConfigImpl::descriptor) + "/default";
      Log::getInstance().d("instance=" + instance);
      binder_status_t status =
          AServiceManager_addService(mIQtiRadioConfigAidlImpl->asBinder().get(), instance.c_str());
      QCRIL_LOG_INFO("IQtiRadioConfig Stable AIDL addService, status= %d", status);
      if (status != STATUS_OK) {
        mIQtiRadioConfigAidlImpl = nullptr;
        QCRIL_LOG_INFO("Error registering service %s %d",
                       IQtiRadioConfigImpl::descriptor,
                       instance_id);
      }
    }
  }
}

void QtiRadioConfigAidlModule::handleSecureModeIndication(std::shared_ptr<SecureModeStateChangeIndMessage> msg){
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIQtiRadioConfigAidlImpl) {
    mIQtiRadioConfigAidlImpl->sendOnSecureModeIndication(msg);
  }
}

void QtiRadioConfigAidlModule::handleCiwlanCapabilityChangeIndication(
    std::shared_ptr<rildata::DataCapabilityChangeIndMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIQtiRadioConfigAidlImpl) {
    mIQtiRadioConfigAidlImpl->sendCiwlanCapabilityChanged(msg);
  }
}

void QtiRadioConfigAidlModule::handleDualDataCapabilityChangeIndication(
    std::shared_ptr<rildata::DataPPDataCapabilityChangeIndMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIQtiRadioConfigAidlImpl) {
    mIQtiRadioConfigAidlImpl->sendDualDataCapabilityChanged(msg);
  }
}

void QtiRadioConfigAidlModule::handleDualDataSubRecommendationIndication(
    std::shared_ptr<rildata::DataSubRecommendationIndMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIQtiRadioConfigAidlImpl) {
    mIQtiRadioConfigAidlImpl->sendDualDataRecommendation(msg);
  }
}
