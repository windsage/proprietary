/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "SimAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>

static load_module<SimAidlModule> sSimAidlModule;

SimAidlModule* getSimAidlModule() {
  return &(sSimAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
SimAidlModule::SimAidlModule() {
  mName = "SimAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, SimAidlModule::handleQcrilInit),
    HANDLER(rildata::CarrierInfoForImsiEncryptionRefreshMessage,
            SimAidlModule::handleCarrierInfoForImsiEncryptionRefreshMessage),
    HANDLER(RilUnsolCdmaSubscriptionSourceChangedMessage,
            SimAidlModule::handleCdmaSubscriptionSourceChangedMessage),
    HANDLER(QcRilUnsolPhonebookRecordsUpdatedMessage,
            SimAidlModule::handlePhonebookRecordsChangedMessage),
    HANDLER(QcRilUnsolAdnRecordsOnSimMessage, SimAidlModule::handleAdnRecordsOnSimMessage),
    HANDLER(UimSimRefreshIndication, SimAidlModule::handleUimSimRefreshIndication),
    HANDLER(UimSimStatusChangedInd, SimAidlModule::handleUimSimStatusChangedInd),
    HANDLER(GstkUnsolIndMsg, SimAidlModule::handleGstkUnsolIndMsg),
    HANDLER(RilUnsolUiccSubsStatusChangedMessage, SimAidlModule::handleUiccSubsStatusChangedMessage),
    HANDLER(RilUnsolUiccAppsStatusChangedMessage, SimAidlModule::handleUiccAppsStatusChangedMessage),
  };
}

/* Follow RAII.
 */
SimAidlModule::~SimAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void SimAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void SimAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  QCRIL_LOG_DEBUG(" Handling msg %s ", msg->dump().c_str());
  QCRIL_LOG_DEBUG(" get_instance_id = %d ", msg->get_instance_id());
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void SimAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
  // Register Stable AIDL Interface.
  if (mIRadioSimAidlImpl == nullptr) {
    mIRadioSimAidlImpl = ndk::SharedRefBase::make<IRadioSimImpl>(instance_id);
    const std::string instance =
        std::string(IRadioSimImpl::descriptor) + "/slot" + std::to_string(instance_id + 1);
    QCRIL_LOG_DEBUG(" Instance %s ", instance.c_str());
    binder_status_t status =
        AServiceManager_addService(mIRadioSimAidlImpl->asBinder().get(), instance.c_str());
    QCRIL_LOG_INFO("IRadioSim Stable AIDL addService, status= %d", status);
    if (status != STATUS_OK) {
      mIRadioSimAidlImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s slot%d", IRadioSimImpl::descriptor,
                     instance_id + 1);
    }
  }
}

void SimAidlModule::handleCarrierInfoForImsiEncryptionRefreshMessage(
    std::shared_ptr<rildata::CarrierInfoForImsiEncryptionRefreshMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendCarrierInfoForImsiEncryptionInd(msg);
  }
}

void SimAidlModule::handleCdmaSubscriptionSourceChangedMessage(
    std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendCdmaSubscriptionSourceChanged(msg);
  }
}

void SimAidlModule::handlePhonebookRecordsChangedMessage(
    std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendSimPhonebookChanged(msg);
  }
}

void SimAidlModule::handleAdnRecordsOnSimMessage(
    std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendSimPhonebookRecords(msg);
  }
}

void SimAidlModule::handleUimSimRefreshIndication(std::shared_ptr<UimSimRefreshIndication> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendSimRefresh(msg);
  }
}

void SimAidlModule::handleUimSimStatusChangedInd(std::shared_ptr<UimSimStatusChangedInd> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendSimStatusChanged(msg);
  }
}

void SimAidlModule::handleGstkUnsolIndMsg(std::shared_ptr<GstkUnsolIndMsg> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendGstkIndication(msg);
  }
}

void SimAidlModule::handleUiccSubsStatusChangedMessage(
    std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendUiccSubsStatusChanged(msg);
  }
}

void SimAidlModule::handleUiccAppsStatusChangedMessage(
    std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioSimAidlImpl) {
    mIRadioSimAidlImpl->sendUiccAppsStatusChanged(msg);
  }
}
