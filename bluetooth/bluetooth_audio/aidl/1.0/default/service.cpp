/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "BtAudioAIDLService"

#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <utils/Log.h>
#include <cutils/properties.h>

#include "BluetoothAudioProviderFactory.h"

using ::aidl::android::hardware::bluetooth::audio::
    BluetoothAudioProviderFactory;

static pthread_mutex_t aidl_register_lock;
static bool aidl_hal_registered = false;
static bool aidl_hal_disabled = false;

binder_status_t createIBluetoothAudioProviderFactory() {
  binder_status_t aidl_status = STATUS_OK;
  char aidl_reg_val[PROPERTY_VALUE_MAX] = "false";
  if ((!aidl_hal_disabled &&
      property_get("persist.vendor.qcom.bluetooth.aidl_hal",
      aidl_reg_val, "true") && !strcmp(aidl_reg_val, "false")) ||
      aidl_hal_disabled) {
    ALOGD("%s: aidl hal is disabled", __func__);
    aidl_hal_disabled = true;
    return aidl_status;
  }

  // lock it
  pthread_mutex_lock(&aidl_register_lock);
  if(!aidl_hal_registered) {
    ALOGD("%s: Registering the AIDL 1.0 service ", __func__);
    auto factory = ::ndk::SharedRefBase::make<BluetoothAudioProviderFactory>();
    const std::string instance_name =
        std::string() + BluetoothAudioProviderFactory::descriptor + "/default";
    binder_status_t aidl_status = AServiceManager_addService(
        factory->asBinder().get(), instance_name.c_str());
    if(aidl_status != STATUS_OK) {
      ALOGD("%s: Could not register %s, status=%d", __func__,
                  instance_name.c_str(), aidl_status);
    } else {
      ALOGD("%s: Registered AIDL 1.0 service %s, status=%d", __func__,
                  instance_name.c_str(), aidl_status);
      aidl_hal_registered = true;
    }
  } else {
    ALOGD("%s: Already registered the AIDL 1.0 service ", __func__);
  }
  // unlock it
  pthread_mutex_unlock(&aidl_register_lock);
  return aidl_status;
}

bool isIBluetoothAudioProviderFactoryAvailable() {
  return aidl_hal_registered;
}