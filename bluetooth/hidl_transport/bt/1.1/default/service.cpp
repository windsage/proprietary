/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */
//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <sys/types.h>
#include <sys/stat.h>
#include <android/hardware/bluetooth/1.0/IBluetoothHci.h>
#include <android/hardware/bluetooth/1.1/IBluetoothHci.h>
#include <hidl/LegacySupport.h>
#include <cutils/properties.h>
#include <hwbinder/ProcessState.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#ifdef QCOM_FM_SUPPORTED
#include <vendor/qti/hardware/fm/1.0/IFmHci.h>
#endif
#ifdef QCOM_ANT_SUPPORTED
#include <com/dsi/ant/1.0/IAnt.h>
#endif

#ifdef QTI_BT_SAR_SUPPORTED
#include <vendor/qti/hardware/bluetooth_sar/1.0/IBluetoothSar.h>
#include <vendor/qti/hardware/bluetooth_sar/1.1/IBluetoothSar.h>
#endif

#ifdef QTI_BT_CONFIGSTORE_SUPPORTED
#include <vendor/qti/hardware/btconfigstore/1.0/IBTConfigStore.h>
#include <vendor/qti/hardware/btconfigstore/2.0/IBTConfigStore.h>
#endif

#include "bttpi.h"
#ifdef QTI_BT_XPANPROVIDER_SUPPORTED
#include "XpanProviderService.h"
#endif

#ifdef QTI_BT_QCV_SUPPORTED
#include  "soc_properties.h"
extern "C" {
#include "libsoc_helper.h"
}
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifdef LAZY_SERVICE
#define LOG_TAG "android.hardware.bluetooth@1.1-service-lazy"
#else
#define LOG_TAG "android.hardware.bluetooth@1.1-service"
#endif

// Generated HIDL files
using IBluetoothHci_V1_0 = android::hardware::bluetooth::V1_0::IBluetoothHci;
using IBluetoothHci_V1_1 = android::hardware::bluetooth::V1_1::IBluetoothHci;

#ifdef QCOM_FM_SUPPORTED
using vendor::qti::hardware::fm::V1_0::IFmHci;
#endif

#ifdef QCOM_ANT_SUPPORTED
using namespace com::dsi::ant::V1_0;
#endif

#ifdef QTI_BT_SAR_SUPPORTED
using vendor::qti::hardware::bluetooth_sar::V1_1::IBluetoothSar;
#endif

#ifdef QTI_BT_CONFIGSTORE_SUPPORTED
using IBTConfigStore_V1_0 = ::vendor::qti::hardware::btconfigstore::V1_0::IBTConfigStore;
using IBTConfigStore_V2_0 = ::vendor::qti::hardware::btconfigstore::V2_0::IBTConfigStore;
#endif

#ifdef QTI_BT_XPANPROVIDER_SUPPORTED
using aidl::vendor::qti::hardware::bluetooth::xpanprovider::XpanProviderService;
#endif
using android::hardware::registerLazyPassthroughServiceImplementation;
using android::hardware::registerPassthroughServiceImplementation;
using android::hardware::defaultPassthroughServiceImplementation;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::OK;

#ifdef QTI_VND_FWK_DETECT_SUPPORTED
#ifdef __cplusplus
extern "C" {
#include "vndfwk-detect.h"
}
#endif
#endif

static const int BT_TX_RT_PRIORITY = 1;

int main() {
    ALOGI("BT-Transport driver main");
    (void)umask(S_IWGRP | S_IWOTH);

    struct sched_param rt_params;
    rt_params.sched_priority = BT_TX_RT_PRIORITY;
/*  TBD: For future
    if (sched_setscheduler(gettid(), SCHED_FIFO, &rt_params)) {
     ALOGE("%s unable to set SCHED_FIFO for pid %d, tid %d, error %s", __func__,
            getpid(), gettid(), strerror(errno));
    }
*/

    //Initialize Mmap size the moment the process starts
    android::hardware::ProcessState::initWithMmapSize((size_t)(256144));

    char prop_value[PROPERTY_VALUE_MAX];
    bool isTpiSupported = false;
    property_get("persist.vendor.qcom.bluetooth.tpi_supported", prop_value, "1");
    if (!strcmp(prop_value, "1")) {
      ALOGI("BTTPI: TPI is supported");
      isTpiSupported = true;
    } else
      ALOGI("BT TPI service not supported");

#ifdef QTI_BT_XPANPROVIDER_SUPPORTED
    char xpan_prop[PROPERTY_VALUE_MAX];
    bool isXpanSupported = false;
    property_get("persist.vendor.qcom.btadvaudio.target.support.xpan", xpan_prop, "false");
    if (!strcmp(xpan_prop, "true")) {
      ALOGI("XpanProvider: vendor HAL supported");
      isXpanSupported = true;
    } else {
      ALOGI("XpanProvider: vendor HAL not supported");
    }
#endif

    if (isTpiSupported) {
      bool success = ABinderProcess_setThreadPoolMaxThreadCount(1);
      ALOGI("BTTPI: Main sethMaxThread SAIDL %d", success);
      ABinderProcess_startThreadPool();
    } else {
#ifdef QTI_BT_XPANPROVIDER_SUPPORTED
      if (isXpanSupported) {
        bool success = ABinderProcess_setThreadPoolMaxThreadCount(1);
        ALOGI("Main setThreadPoolMaxThreadCount for stable AIDL %d", success);
        ABinderProcess_startThreadPool();
      }
#endif
    }

    configureRpcThreadpool(1, true /* caller Will Join*/);

    bool isVendorEnhancedFramework;
#ifdef QTI_BT_QCV_SUPPORTED
    setVendorPropertiesDefault();
#endif

#ifdef BT_CP_CONNECTED
   ALOGI("calling isCpsupported");
   isCpsupported();
#endif

#ifdef QTI_VND_FWK_DETECT_SUPPORTED
    /* 0: Pure AOSP for both system and odm
       1: Pure AOSP for system and QC Value-adds for odm
       2: QC value-adds for system and Pure AOSP for odm
       3: QC value-adds for both system and odm
    */
    int vendorEnhancedInfo = getVendorEnhancedInfo();
    ALOGI("vendorEnhancedInfo: %d", vendorEnhancedInfo);
    isVendorEnhancedFramework = (vendorEnhancedInfo & 1) ? true : false;
#else
    isVendorEnhancedFramework = false;
#endif

    ALOGI("isVendorEnhancedFramework: %d", isVendorEnhancedFramework);
    ALOGI("Registering BT Service");

    android::status_t status;

#ifdef LAZY_SERVICE
    status = registerLazyPassthroughServiceImplementation<IBluetoothHci_V1_1>();
#else
    status = registerPassthroughServiceImplementation<IBluetoothHci_V1_1>();
#endif
    if (status != OK) {
        ALOGI("Error while registering BluetoothHci 1.1 service: %d", status);

#ifdef LAZY_SERVICE
        status = registerLazyPassthroughServiceImplementation<IBluetoothHci_V1_0>();
#else
        status = registerPassthroughServiceImplementation<IBluetoothHci_V1_0>();
#endif
        if (status != OK)
          ALOGI("Error while registering BluetoothHci 1.0 service: %d", status);
    }

#ifdef QCOM_FM_SUPPORTED
    if (isVendorEnhancedFramework) {
      ALOGI("Registering FM Service");

#ifdef LAZY_SERVICE
      status = registerLazyPassthroughServiceImplementation<IFmHci>();
#else
      status = registerPassthroughServiceImplementation<IFmHci>();
#endif

      if (status != OK)
        ALOGI("Error while registering FM service: %d", status);
    }
#endif

#ifdef QCOM_ANT_SUPPORTED
    if (isVendorEnhancedFramework) {
      ALOGI("Registering ANT Service");

#ifdef LAZY_SERVICE
      status = registerLazyPassthroughServiceImplementation<IAnt>();
#else
      status = registerPassthroughServiceImplementation<IAnt>();
#endif

      if (status != OK)
        ALOGI("Error while registering ANT service: %d", status);
    }
#endif

#ifdef QTI_BT_SAR_SUPPORTED
    ALOGI("Registering SAR service");

#ifdef LAZY_SERVICE
    status = registerLazyPassthroughServiceImplementation<IBluetoothSar>();
#else
    status = registerPassthroughServiceImplementation<IBluetoothSar>();
#endif

    if (status != OK)
      ALOGI("Error while registering BT SAR service: %d", status);
#endif

#ifdef QTI_BT_CONFIGSTORE_SUPPORTED
    if (isVendorEnhancedFramework) {
      ALOGI("Registering BT Config store Service");

#ifdef LAZY_SERVICE
      status = registerLazyPassthroughServiceImplementation<IBTConfigStore_V2_0>();
#else
      status = registerPassthroughServiceImplementation<IBTConfigStore_V2_0>();
#endif
      if (status != OK) {
        ALOGI("Error while registering BT Configstore 2.0 service: %d", status);

#ifdef LAZY_SERVICE
        status = registerLazyPassthroughServiceImplementation<IBTConfigStore_V1_0>();
#else
        status = registerPassthroughServiceImplementation<IBTConfigStore_V1_0>();
#endif
        if (status != OK)
          ALOGI("Error while registering BT Configstore 1.0 service: %d", status);
      }
    }
#endif

    if (isVendorEnhancedFramework and isTpiSupported) {
      ALOGI("Registering BT TPI service");

      std::shared_ptr<BtTpi> mAidlService = ndk::SharedRefBase::make<BtTpi>();
      const std::string instance = std::string() + BtTpi::descriptor + "/default";
      binder_status_t status =  AServiceManager_addService(
         mAidlService->asBinder().get(), instance.c_str());
      if (status != STATUS_OK) {
        ALOGE("BtTpi::Error registering AIDL service [%s]", instance.c_str());
      } else {
        ALOGI("BtTpi::Success registering AIDL service[%s]", instance.c_str());
      }
    }

#ifdef QTI_BT_XPANPROVIDER_SUPPORTED
    if (isXpanSupported) {
      ALOGI("XPAN: Registering XPAN AIDL Service ..");

      std::shared_ptr<XpanProviderService> mAidlService =
          ndk::SharedRefBase::make<XpanProviderService>();
      const std::string instance =
          std::string() + XpanProviderService::descriptor + "/default";

      binder_status_t bstatus =  AServiceManager_addService(
         mAidlService->asBinder().get(), instance.c_str());

      if (bstatus != STATUS_OK) {
        ALOGE("XpanProvider: Error registering AIDL service [%s]", instance.c_str());
      } else {
        ALOGI("XpanProviderService: Successfully registered AIDL service[%s]",
              instance.c_str());
      }
    }
#endif

    ALOGI("Main, joinRpcThreadpool for HIDL");
    joinRpcThreadpool();
    if (isTpiSupported) {
      ALOGI("Main, joinThreadpool for AIDL");
      ABinderProcess_joinThreadPool();
    } else {
#ifdef QTI_BT_XPANPROVIDER_SUPPORTED
    if (isXpanSupported) {
      ALOGI("Main, joinThreadpool for AIDL");
      ABinderProcess_joinThreadPool();
    }
#endif
    }

    return status;
}
