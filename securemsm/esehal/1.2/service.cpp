/**
 * Copyright (c) 2018, 2020, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "vendor.qti.secureelement@1.2-service"

#include <android/hardware/secure_element/1.2/ISecureElement.h>
#include <hidl/LegacySupport.h>
#include "eSEClient.h"
#include "SecureElement.h"
#include "QSEEComAPI.h"
#include "EseUtils/EseUtils.h"
#include <cutils/properties.h>

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using android::hardware::secure_element::V1_2::ISecureElement;
using android::hardware::secure_element::V1_2::implementation::SecureElement;
using android::sp;
using android::OK;

ese_update_state_t ese_update = ESE_UPDATE_COMPLETED;

static int initEseTrustedApp(void) {
    std::string app_uuid = "";
    char ese_hw_name[PROPERTY_VALUE_MAX] = {0};
    int len = 0;
    int ret = 0;
    struct QSEECom_handle* qseeComHandleeSEService = NULL;
    uint8_t ese_vendor_id = eseGetVendorId();

    if (ese_vendor_id == ESE_VENDOR_NXP) {
        app_uuid = TRUSTED_APP_UUID_NXP;
    } else if (ese_vendor_id == ESE_VENDOR_STM) {
        app_uuid = TRUSTED_APP_UUID_STM;
    } else {
        ALOGE("Unknown ese vendor");
        return -1;
    }

    // Power ON eSE for RoT establishment usecase during trusted application loading
    ret = eseSetPower(true);
    if (ret != 0) {
        ALOGE("Failed setEsePower on");
        return -1;
    }

    ret = QSEECom_start_app(&qseeComHandleeSEService, TA_APP_PATH, app_uuid.c_str(), 1024);
    if (ret) {
        ALOGD("%s application loading failed", app_uuid.c_str());
    }

    eseSetPower(false);

    return ret;
}

int main() {
    ALOGE("vendor::qti::secure_element V1.2 service starts");
    android::status_t status;
    int ret;

    configureRpcThreadpool(10, true /*callerWillJoin*/);

    ret = initEseTrustedApp();
    if (ret != 0) {
        ALOGE("Failed on initalize eSE TA");
        return ret;
    }

#if(ESE_CLIENT_LIB_PRESENT == true)
    checkEseClientUpdate();
#endif

    sp<ISecureElement> se_service = new SecureElement();
    status = se_service->registerAsService("eSE1");

    LOG_ALWAYS_FATAL_IF(status != OK, "Error while registering secure_element V1.2 service: %d", status);

#if(ESE_CLIENT_LIB_PRESENT == true)
    perform_eSEClientUpdate();
#endif

    joinRpcThreadpool();
    return status;
}
