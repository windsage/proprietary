/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "SapAidlModule.h"
#include "framework/Log.h"
#include "interfaces/uim/UimSapStatusIndMsg.h"
#include "interfaces/uim/qcril_uim_types.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>


static load_module<SapAidlModule> sSapAidlModule;

SapAidlModule::SapAidlModule() {
  mName = "SapAidlModule";
  using std::placeholders::_1;
  mMessageHandler = {HANDLER(QcrilInitMessage, SapAidlModule::handleQcrilInit),
                     HANDLER(UimSapStatusIndMsg, SapAidlModule::handleSAPInd)};
}

SapAidlModule::~SapAidlModule() {}

void SapAidlModule::init() { Module::init(); }

void SapAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg" + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(instance_id));

  if (mISapImpl == nullptr) {
    mISapImpl = ndk::SharedRefBase::make<ISapImpl>(msg->get_instance_id());
    std::string serviceInstance = std::string(ISapImpl::descriptor) + "/slot" +
                                  std::to_string(instance_id + 1);

    binder_status_t status = AServiceManager_addService(
        mISapImpl->asBinder().get(), serviceInstance.c_str());

    QCRIL_LOG_INFO("SapService stable AIDL addService, status = %d", status);
    if (status != STATUS_OK) {
      mISapImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s", serviceInstance.c_str());
    }
  }
}

void SapAidlModule::handleSAPInd(std::shared_ptr<UimSapStatusIndMsg> msg) {

  /* Only 2 unsol responses are send from the QMI IND & it depends on the
  sap_status:
  1. RIL_SIM_SAP_STATUS_IND, upon status for connection
  establishment/disconnection
  2. RIL_SIM_SAP_DISCONNECT_IND, upon disconnection request only */

  if (mISapImpl == nullptr || msg == NULL) {
    RLOGE("NULL sap_service or Null callback");
    return;
  }

  switch (msg->get_status()) {
  case RIL_UIM_SAP_DISCONNECT_GRACEFUL:
    mISapImpl->sendDisconnectInd(0, aidlimports::SapDisconnectType::GRACEFUL);
    break;

  case RIL_UIM_SAP_DISCONNECT_IMMEDIATE:
    mISapImpl->sendDisconnectInd(0, aidlimports::SapDisconnectType::IMMEDIATE);
    break;

  case RIL_UIM_SAP_STATUS_CARD_RESET:
    mISapImpl->sendStatusInd(0, aidlimports::SapStatus::CARD_RESET);
    break;

  case RIL_UIM_SAP_STATUS_NOT_ACCESSIABLE:
    mISapImpl->sendStatusInd(0, aidlimports::SapStatus::CARD_NOT_ACCESSIBLE);
    break;

  case RIL_UIM_SAP_STATUS_CARD_REMOVED:
    mISapImpl->sendStatusInd(0, aidlimports::SapStatus::CARD_REMOVED);
    break;

  case RIL_UIM_SAP_STATUS_CARD_INSERTED:
    mISapImpl->sendStatusInd(0, aidlimports::SapStatus::CARD_INSERTED);
    break;

  case RIL_UIM_SAP_STATUS_RECOVERED:
    mISapImpl->sendStatusInd(0, aidlimports::SapStatus::RECOVERED);
    break;

  default:
    /* Note that for a succesfully connected case, if the connection response
    isnt sent yet, the STATUS_IND is sent after response callback comes */
    QCRIL_LOG_INFO("Unknown Status: 0x%x", msg->get_status());
    return;
  }

} /* SapModule::handleSAPInd */
