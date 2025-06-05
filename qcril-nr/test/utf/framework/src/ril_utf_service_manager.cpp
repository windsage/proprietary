/******************************************************************************
#  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------*/

#include "ril_utf_service_manager.h"

class ServiceManagerWrapper {
public:
    ServiceManagerWrapper() : spServiceManager(&serviceManager) {
        //serviceManager = new android::ServiceManager();
        ::android::setDefaultServiceManager(spServiceManager);
    }

    android::sp<android::IServiceManager> getServiceManager() {
        return spServiceManager;
    }

private:
    android::FakeServiceManager serviceManager;
    android::sp<android::IServiceManager> spServiceManager;
};

[[clang::no_destroy]] static ServiceManagerWrapper srvMgrWrapper;

android::sp<android::IServiceManager> getServiceManager()
{
    return srvMgrWrapper.getServiceManager();
}
