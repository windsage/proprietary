/*
 * Copyright (c) 2018, 2020-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "dlfcn.h"
#include "framework/Log.h"
#include "framework/ModuleLooper.h"
#include "framework/QcrilInitMessage.h"

#include "BearerAllocationUpdateMessage.h"
#include "DataHalServiceImplFactory.h"
#include "DataConnectionServiceModule.h"

#include "UnSolMessages/AidlDeathMessage.h"
#include "UnSolMessages/DataInActivityTimerExpiredMessage.h"
#include "UnSolMessages/TcpKeepAliveIndMessage.h"
#include "android/binder_manager.h"
#include "android/binder_process.h"

#ifdef QMI_RIL_UTF
#include <binder/Binder.h>
#include <ibinder_internal.h>
#include "ril_utf_service_manager.h"

using android::sp;
using android::String16;
using android::status_t;
using android::IServiceManager;
#endif

using namespace rildata;
using namespace aidl::vendor::qti::hardware::data::connectionaidl;

static load_module<rildata::DataConnectionServiceModule> DataConnectionServiceModule;

/**
 * DataConnectionServiceModule::DataConnectionServiceModule()
 *
 * @brief
 * Initializes DataConnectionServiceModule
 */
DataConnectionServiceModule::DataConnectionServiceModule() {
    Log::getInstance().d("DataConnectionServiceModule()");
    mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
    mName = "DataConnectionServiceModule";
    mMessageHandler = {
        HANDLER(QcrilInitMessage, DataConnectionServiceModule::handleQcrilInit),
        HANDLER(BearerAllocationUpdateMessage, DataConnectionServiceModule::handleBearerAllocationUpdate),
        HANDLER(AidlDeathMessage, DataConnectionServiceModule::handlAidlDeathMessage),
        HANDLER(DataInActivityTimerExpiredMessage, DataConnectionServiceModule::handlDataInactiviyTimerExipry),
        HANDLER(TcpKeepAliveIndMessage, DataConnectionServiceModule::handleTcpKeepAliveInd)
    };
    mDataAidlLibInstance = nullptr;
    mService = nullptr;
}

void DataConnectionServiceModule::init() {
    Log::getInstance().d("DataConnectionServiceModule::init");
    Module::init();
}

/**
 * DataConnectionServiceModule::handleQcrilInit()
 *
 * @brief
 * Handler for QcrilInit message. Registers DataConnection HAL service
 */
void DataConnectionServiceModule::handleQcrilInit(std::shared_ptr<Message> msg)
{
    if (msg != nullptr) {
        std::shared_ptr<QcrilInitMessage> initMsg = std::static_pointer_cast<QcrilInitMessage>(msg);
        if (initMsg != nullptr) {
            Log::getInstance().d("[" + mName + "]: Handling msg = " + initMsg->dump() +
                         ", instance = " + std::to_string((int)initMsg->get_instance_id()));
            registerService((int)initMsg->get_instance_id());
        }
    }
}

/*
 * Register DataConnection service with Service Manager
 */
void DataConnectionServiceModule::registerService(int instanceId)
{
    Log::getInstance().d("DataConnectionServiceModule::registerService");
    if (!mService) {
        mService = ndk::SharedRefBase::make<DataConnectionServiceBase>();
        std::string instance = std::string(DataConnectionServiceBase::descriptor) + "/slot" + std::to_string(instanceId + 1);
        Log::getInstance().d("[" + mName + "] : instance=" + instance);
        binder_status_t status = AServiceManager_addService(mService->asBinder().get(), instance.c_str());
        Log::getInstance().d("[" + mName + "] DataConnectionServiceBase Stable AIDL addService status = " + std::to_string(status));
        if (status != STATUS_OK) {
            mService = nullptr;
            Log::getInstance().d("DataConnectionServiceBase failed to add service " + instance);
        }
    }
    if(mDataAidlLibInstance == nullptr) {
      loadAidllib();
      #ifdef QMI_RIL_UTF
          dataAidlInit(instanceId);
      #endif
      if(mDataAidlInit != nullptr) {
          mDataAidlInit(instanceId);
      }
    }
}

void DataConnectionServiceModule::loadAidllib()
{
  bool ret = false;
#ifdef QMI_RIL_UTF
  return;
#endif
  do {
    mDataAidlLibInstance = dlopen("libqcrildataaidl.so", RTLD_LAZY);
    if( NULL == mDataAidlLibInstance ) {
      Log::getInstance().d("[" + mName + "]: Unable to load libqcrildataaidl.so");
      break;
    }
    dlerror();
    mDataAidlInit = (dataAidlInitFnPtr)dlsym(mDataAidlLibInstance, "dataAidlInit");
    const char *dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find dataAidlInit symbol");
      break;
    }
    mDataAidlCleanup = (dataAidlCleanUpFnPtr)dlsym(mDataAidlLibInstance, "dataAidlCleanUp");
    dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find dataAidlCleanUp symbol");
      break;
    }
    mhandleAidlDeathMessage = (dataAidlCallbackFnPtr)dlsym(mDataAidlLibInstance, "handleDeathMessage");
    dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find handleDeathMessage symbol");
      break;
    }
    mhandleDataInactiviyTimerExipry = (dataAidlCallbackFnPtr)dlsym(mDataAidlLibInstance, "handleDataInactiviyTimerExipry");
    dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find handleDataInactiviyTimerExipry symbol");
      break;
    }
    mhandleTcpKeepAliveInd = (dataAidlCallbackFnPtr)dlsym(mDataAidlLibInstance, "handleTcpKeepAliveIndication");
    dlsym_error = dlerror();
    if( dlsym_error ) {
      Log::getInstance().d("[" + mName + "]: Cannot find handleTcpKeepAliveInd symbol");
      break;
    }

    ret = true;
  }
  while( 0 );
  if( ret == false ) {
    unLoadAidllib();
  }
}
void DataConnectionServiceModule::unLoadAidllib()
{
#ifdef QMI_RIL_UTF
   dataAidlCleanUp();
   return;
#endif
  if(mDataAidlLibInstance != nullptr) {
    if(mDataAidlCleanup != nullptr) {
      mDataAidlCleanup();
    }
    mDataAidlInit = nullptr;
    mDataAidlCleanup = nullptr;
    dlclose(mDataAidlLibInstance);
    mDataAidlLibInstance = nullptr;
  }
}

/**
 * DataConnectionServiceModule::handleQcrilInit()
 *
 * @brief
 * Handler for BearerAllocationUpdate message. Sends updated bearer list to
 * registered HAL clients
 */
void DataConnectionServiceModule::handleBearerAllocationUpdate(std::shared_ptr<Message> msg)
{
    if (msg != nullptr) {
        std::shared_ptr<BearerAllocationUpdateMessage> updateMsg = std::static_pointer_cast<BearerAllocationUpdateMessage>(msg);
        if (updateMsg != nullptr) {
            Log::getInstance().d("[" + mName + "]: Handling msg = " + updateMsg->dump());
            if (mService != nullptr) {
                mService->notifyBearerAllocationUpdate(updateMsg->getBearers());
            }
        }
    }
}

void DataConnectionServiceModule::handlAidlDeathMessage(std::shared_ptr<Message> msg)
{
#ifdef QMI_RIL_UTF
  handleDeathMessage(msg);
  return;
#endif
  if(msg != nullptr && mhandleAidlDeathMessage != nullptr) {
    mhandleAidlDeathMessage(msg);
  }
}

void DataConnectionServiceModule::handlDataInactiviyTimerExipry(std::shared_ptr<Message> msg)
{
#ifdef QMI_RIL_UTF
  handleDataInactiviyTimerExipry(msg);
  return;
#endif
  if(msg != nullptr && mhandleDataInactiviyTimerExipry != nullptr) {
    mhandleDataInactiviyTimerExipry(msg);
  }
}
void DataConnectionServiceModule::handleTcpKeepAliveInd(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("DataConnectionServiceModule::handleTcpKeepAliveInd()");
#ifdef QMI_RIL_UTF
  Log::getInstance().d("DataConnectionServiceModule::handleTcpKeepAliveInd() UTF");
  handleTcpKeepAliveIndication(msg);
  return;
#endif
  if(msg != nullptr && mhandleTcpKeepAliveInd != nullptr) {
    mhandleTcpKeepAliveInd(msg);
  }
}
