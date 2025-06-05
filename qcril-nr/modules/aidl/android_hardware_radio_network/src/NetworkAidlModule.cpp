/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "NetworkAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cstring>
#include <framework/Log.h>

static load_module<NetworkAidlModule> sNetworkAidlModule;

NetworkAidlModule* getNetworkAidlModule() {
  return &(sNetworkAidlModule.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
NetworkAidlModule::NetworkAidlModule() {
  mName = "NetworkAidlModule";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, NetworkAidlModule::handleQcrilInit),
    HANDLER(QcRilUnsolSuppSvcNotificationMessage,
        NetworkAidlModule::handleQcRilUnsolSuppSvcNotificationMessage),
    HANDLER(RilUnsolNetworkStateChangedMessage,
        NetworkAidlModule::handleNetworkStateChangedMessage),
    HANDLER(RilUnsolCdmaPrlChangedMessage,
        NetworkAidlModule::handleCdmaPrlChangedMessage),
    HANDLER(RilUnsolNetworkScanResultMessage,
        NetworkAidlModule::handleNetworkScanResultMessage),
    HANDLER(RilUnsolImsNetworkStateChangedMessage,
        NetworkAidlModule::handleImsNetworkStateChangedMessage),
    HANDLER(RilUnsolCellBarringMessage,
        NetworkAidlModule::handleCellBarringMessage),
    HANDLER(RilUnsolCellInfoListMessage,
        NetworkAidlModule::handleCellInfoListMessage),
    HANDLER(RilUnsolSignalStrengthMessage,
        NetworkAidlModule::handleSignalStrengthMessage),
    HANDLER(RilUnsolVoiceRadioTechChangedMessage,
        NetworkAidlModule::handleVoiceRadioTechChangedMessage),
    HANDLER(RilUnsolNitzTimeReceivedMessage,
        NetworkAidlModule::handleNitzTimeReceivedMessage),
    HANDLER(rildata::PhysicalConfigStructUpdateMessage,
        NetworkAidlModule::handlePhysicalConfigStructUpdateMessage),
    HANDLER(rildata::LinkCapIndMessage,
        NetworkAidlModule::handleLinkCapIndMessage),
    HANDLER(RilUnsolNwRegistrationRejectMessage,
            NetworkAidlModule::handleNwRegistrationRejectMessage),
    HANDLER(RilUnsolRestrictedStateChangedMessage,
            NetworkAidlModule::handleRestrictedStateChangedMessage),
  };
}

/* Follow RAII.
 */
NetworkAidlModule::~NetworkAidlModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void NetworkAidlModule::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void NetworkAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(msg->get_instance_id()));
  /* Register AIDL service. */
  registerAidlService(msg->get_instance_id());
}

void NetworkAidlModule::registerAidlService(qcril_instance_id_e_type instance_id) {
  // Register Stable AIDL Interface.
  if (mIRadioNetworkAidlImpl == nullptr) {
    mIRadioNetworkAidlImpl = ndk::SharedRefBase::make<IRadioNetworkImpl>(instance_id);
    const std::string instance =
        std::string(IRadioNetworkImpl::descriptor) + "/slot" + std::to_string(instance_id + 1);
    Log::getInstance().d("instance=" + instance);
    binder_status_t status =
        AServiceManager_addService(mIRadioNetworkAidlImpl->asBinder().get(), instance.c_str());
    QCRIL_LOG_INFO("IRadioNetwork Stable AIDL addService, status= %d", status);
    if (status != STATUS_OK) {
      mIRadioNetworkAidlImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s slot%d",
                     IRadioNetworkImpl::descriptor,
                     instance_id + 1);
    }
  }
}

void NetworkAidlModule::handleImsNetworkStateChangedMessage(
    std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendImsNetworkStateChanged(msg);
  }
}

void NetworkAidlModule::handleNetworkStateChangedMessage(
    std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendNetworkStateChanged(msg);
  }
}

void NetworkAidlModule::handleQcRilUnsolSuppSvcNotificationMessage(
    std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (msg && !msg->isIms()) {
    if (mIRadioNetworkAidlImpl) {
      mIRadioNetworkAidlImpl->sendSuppSvcNotify(msg);
    }
  }
}

void NetworkAidlModule::handleCdmaPrlChangedMessage(
    std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendCdmaPrlChanged(msg);
  }
}

void NetworkAidlModule::handleNetworkScanResultMessage(
    std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendNetworkScanResult(msg);
  }
}

void NetworkAidlModule::handleCellBarringMessage(
    std::shared_ptr<RilUnsolCellBarringMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendCellBarring(msg);
  }
}

void NetworkAidlModule::handleCellInfoListMessage(
    std::shared_ptr<RilUnsolCellInfoListMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendCellInfoList(msg);
  }
}

void NetworkAidlModule::handleSignalStrengthMessage(
    std::shared_ptr<RilUnsolSignalStrengthMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendSignalStrength(msg);
  }
}

void NetworkAidlModule::handleVoiceRadioTechChangedMessage(
    std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendVoiceRadioTechChanged(msg);
  }
}

void NetworkAidlModule::handleNitzTimeReceivedMessage(
    std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendNitzTimeReceived(msg);
  }
}
void NetworkAidlModule::handleLinkCapIndMessage(std::shared_ptr<rildata::LinkCapIndMessage> msg)
{
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendLinkCapInd(msg);
  }
}

void NetworkAidlModule::handlePhysicalConfigStructUpdateMessage(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg)
{
  if (msg == nullptr) {
    QCRIL_LOG_DEBUG("msg is NULL");
    return;
  }
  QCRIL_LOG_DEBUG("Handling %s", msg->dump().c_str());
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendPhysicalConfigStructUpdateInd(msg);
  }
}

void NetworkAidlModule::handleNwRegistrationRejectMessage(
    std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendNwRegistrationReject(msg);
  }
}

void NetworkAidlModule::handleRestrictedStateChangedMessage(
    std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg) {
  QCRIL_LOG_INFO("Handling msg: %s", (msg ? msg->dump().c_str() : "nullptr"));
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->sendRestrictedStateChanged(msg);
  }
}

#ifdef QMI_RIL_UTF
void NetworkAidlModule::cleanup()
{
  if (mIRadioNetworkAidlImpl) {
    mIRadioNetworkAidlImpl->cleanup();
  }
}

void radio_network_aidl_module_reboot_cleanup()
{
  auto module = getNetworkAidlModule();
  if (module) {
    module->cleanup();
  }
}
#endif
