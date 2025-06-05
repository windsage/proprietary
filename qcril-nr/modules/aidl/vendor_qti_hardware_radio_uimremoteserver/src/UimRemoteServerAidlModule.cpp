/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#define TAG "RILQ"

#include "UimRemoteServerAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <framework/Log.h>

static load_module<UimRemoteServerAidlModule> sUimRemoteServerAidlModule;

UimRemoteServerAidlModule::UimRemoteServerAidlModule()
{
  mName = "UimRemoteServerAidlModule";
  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, UimRemoteServerAidlModule::handleQcrilInit),
    HANDLER(UimSapStatusIndMsg, UimRemoteServerAidlModule::handleSAPInd),
  };
}

UimRemoteServerAidlModule::~UimRemoteServerAidlModule()
{
}

void UimRemoteServerAidlModule::init()
{
  Module::init();
}

void UimRemoteServerAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg)
{
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg" + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));

  if (mIUimRemoteServiceServerImpl == nullptr) {
    mIUimRemoteServiceServerImpl =
        ndk::SharedRefBase::make<aidlimplimports::IUimRemoteServiceServerImpl>();
    std::string serviceInstance =
        std::string(aidlimplimports::IUimRemoteServiceServerImpl::descriptor) + "/uimRemoteServer" +
        std::to_string(instance_id);

    binder_status_t status = AServiceManager_addService(
        mIUimRemoteServiceServerImpl->asBinder().get(), serviceInstance.c_str());

    QCRIL_LOG_INFO("UimService stable AIDL addService, status = %d", status);
    if (status != STATUS_OK) {
      mIUimRemoteServiceServerImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s", serviceInstance.c_str());
    }
  }
}

void UimRemoteServerAidlModule::handleSAPInd(std::shared_ptr<UimSapStatusIndMsg> msg)
{
  QCRIL_LOG_DEBUG("UimRemoteServerAidlModule::handleSAPInd");
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mIUimRemoteServiceServerImpl == nullptr) {
    Log::getInstance().d("[" + mName + "]: UimRemoteServiceServer service is not created");
    return;
  }
  switch (msg->get_status()) {
    case RIL_UIM_SAP_DISCONNECT_GRACEFUL:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerDisconnectIndication(
          aidlimports::UimRemoteServiceServerDisconnectType::
              UIM_REMOTE_SERVICE_SERVER_DISCONNECT_GRACEFUL);
      break;

    case RIL_UIM_SAP_DISCONNECT_IMMEDIATE:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerDisconnectIndication(
          aidlimports::UimRemoteServiceServerDisconnectType::
              UIM_REMOTE_SERVICE_SERVER_DISCONNECT_IMMEDIATE);
      break;

    case RIL_UIM_SAP_STATUS_CARD_RESET:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerStatusIndication(
          aidlimports::UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_CARD_RESET);
      break;

    case RIL_UIM_SAP_STATUS_NOT_ACCESSIABLE:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerStatusIndication(
          aidlimports::UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSIBLE);
      break;

    case RIL_UIM_SAP_STATUS_CARD_REMOVED:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerStatusIndication(
          aidlimports::UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED);
      break;

    case RIL_UIM_SAP_STATUS_CARD_INSERTED:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerStatusIndication(
          aidlimports::UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_CARD_INSERTED);
      break;

    case RIL_UIM_SAP_STATUS_RECOVERED:
      mIUimRemoteServiceServerImpl->uimRemoteServiceServerStatusIndication(
          aidlimports::UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_RECOVERED);
      break;

    default:
      /* Note that for a succesfully connected case, if the connection response
      isnt sent yet, the STATUS_IND is sent after response callback comes */
      QCRIL_LOG_DEBUG("Unknown Status: 0x%x", msg->get_status());
      return;
  }
}
