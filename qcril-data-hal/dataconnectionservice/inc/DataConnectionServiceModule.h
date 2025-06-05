/*
 * Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "framework/Module.h"
#include "DataConnectionServiceBase.h"

extern "C" {
void dataAidlInit(int instanceId);
void dataAidlCleanUp();
void handleDeathMessage(std::shared_ptr<Message> msg);
void handleDataInactiviyTimerExipry(std::shared_ptr<Message> msg);
void handleTcpKeepAliveIndication(std::shared_ptr<Message> msg);
} //extern C

namespace rildata {
typedef void (*dataAidlInitFnPtr)(int);
typedef void (*dataAidlCleanUpFnPtr)();
typedef void (*dataAidlCallbackFnPtr)(std::shared_ptr<Message>);

class DataConnectionServiceModule : public Module {
public:
    DataConnectionServiceModule();
    ~DataConnectionServiceModule() {}
    void init();

private:
    void registerService(int instanceId);
    void handleQcrilInit(std::shared_ptr<Message> msg);
    void handleBearerAllocationUpdate(std::shared_ptr<Message> msg);
    void handlAidlDeathMessage(std::shared_ptr<Message> msg);
    void handlDataInactiviyTimerExipry(std::shared_ptr<Message> msg);
    void handleTcpKeepAliveInd(std::shared_ptr<Message> msg);
    void loadAidllib();
    void unLoadAidllib();
    std::shared_ptr<aidl::vendor::qti::hardware::data::connectionaidl::DataConnectionServiceBase> mService;
    dataAidlInitFnPtr mDataAidlInit;
    dataAidlCleanUpFnPtr mDataAidlCleanup;
    dataAidlCallbackFnPtr mhandleAidlDeathMessage;
    dataAidlCallbackFnPtr mhandleDataInactiviyTimerExipry;
    dataAidlCallbackFnPtr mhandleTcpKeepAliveInd;
    void* mDataAidlLibInstance;
};

} //namespace rildata