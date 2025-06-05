/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "UimAidlModule.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <framework/Log.h>

#include <cutils/properties.h>
static load_module<UimAidlModule> sUimAidlModule;

UimAidlModule::UimAidlModule()
{
  mName = "UimAidlModule";
  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, UimAidlModule::handleQcrilInit),
    HANDLER(UimGetRilPropertySyncMsg, UimAidlModule::handleQcrilUimGetProperty),
    HANDLER(UimSetRilPropertySyncMsg, UimAidlModule::handleQcrilUimSetProperty)
  };
}

UimAidlModule::~UimAidlModule()
{
}

void UimAidlModule::init()
{
  Module::init();
}

void UimAidlModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg)
{
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg" + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));

  if (mIUimImpl == nullptr) {
    mIUimImpl = ndk::SharedRefBase::make<aidlimplimports::IUimImpl>(instance_id);
    std::string serviceInstance =
        std::string(aidlimplimports::IUimImpl::descriptor) + "/Uim" + std::to_string(instance_id);

    binder_status_t status =
        AServiceManager_addService(mIUimImpl->asBinder().get(), serviceInstance.c_str());

    QCRIL_LOG_INFO("UimService stable AIDL addService, status = %d", status);
    if (status != STATUS_OK) {
      mIUimImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s", serviceInstance.c_str());
    }
  }
}

/*===========================================================================

FUNCTION:  UimServiceModule::handleQcrilUimGetProperty

===========================================================================*/
void UimAidlModule::handleQcrilUimGetProperty
(
  std::shared_ptr<UimGetRilPropertySyncMsg> req_ptr
)
{
  auto rsp_ptr = std::make_shared<std::string>();
  char prop_value[PROPERTY_VALUE_MAX] = {};

  QCRIL_LOG_INFO("Inside Function handleQcrilUimGetProperty");
  if (req_ptr != NULL)
  {
    property_get(req_ptr->get_property().c_str(), prop_value, "");

    QCRIL_LOG_INFO("%s - %s", req_ptr->get_property().c_str(), prop_value);

    if (rsp_ptr != NULL)
    {
      rsp_ptr->assign(prop_value);
    }
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);
  }
} /* UimServiceModule::handleQcrilUimGetProperty */


/*===========================================================================

FUNCTION:  UimServiceModule::handleQcrilUimSetProperty

===========================================================================*/
void UimAidlModule::handleQcrilUimSetProperty
(
  std::shared_ptr<UimSetRilPropertySyncMsg> req_ptr
)
{
  QCRIL_LOG_INFO("Inside Function handleQcrilUimGetProperty");
  if (req_ptr != NULL)
  {
    property_set(req_ptr->get_property().c_str(), req_ptr->get_value().c_str());

    req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, NULL);
  }
} /* UimServiceModule::handleQcrilUimSetProperty */
