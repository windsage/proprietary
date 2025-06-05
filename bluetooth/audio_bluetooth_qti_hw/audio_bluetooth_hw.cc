/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

/*
 * Copyright 2019 The Android Open Source Project
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

#define LOG_TAG "BTAudioHw"

#include <android-base/logging.h>
#include <errno.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <log/log.h>
#include <malloc.h>
#include <string.h>
#include <system/audio.h>
#include "btaudio_offload_qti.h"
#include "btaudio_offload_qti_2_1.h"

#include "audio_bluetooth_hw.h"
#include "audio_bluetooth_qti_hw.h"
#include "audio_bluetooth_qti_hw_2_1.h"

#include "BluetoothAudioProviderFactory.h"
#include <android/binder_manager.h>
#include "aidl/stream_apis.h"

using ::aidl::android::hardware::bluetooth::audio::BluetoothAudioProviderFactory;

extern binder_status_t createIBluetoothAudioProviderFactory();
extern bool isIBluetoothAudioProviderFactoryAvailable();

bool hal_2_1 = false;
bool aidl_1_0 = false;

int audio_set_parameters(struct audio_hw_device* dev,
                               const char* kvpairs) {
  if(aidl_1_0) {
    return adev_set_parameters_aidl_1_0(dev, kvpairs);
  } else if (hal_2_1)
    return adev_set_parameters_2_1(dev, kvpairs);
  else
    return adev_set_parameters(dev, kvpairs);
}

char* audio_get_parameters(const struct audio_hw_device* dev __unused,
                                 const char* keys) {
  LOG(VERBOSE) << __func__ << ": keys=[" << keys << "]";
  return strdup("");
}

int audio_init_check(const struct audio_hw_device* dev __unused) { return 0; }

int audio_set_voice_volume(struct audio_hw_device* dev __unused, float volume) {
  LOG(VERBOSE) << __func__ << ": volume=" << volume;
  return -ENOSYS;
}

int audio_set_master_volume(struct audio_hw_device* dev __unused, float volume) {
  LOG(VERBOSE) << __func__ << ": volume=" << volume;
  return -ENOSYS;
}

int audio_get_master_volume(struct audio_hw_device* dev __unused, float* volume __unused) {
  return -ENOSYS;
}

int audio_set_master_mute(struct audio_hw_device* dev __unused, bool muted) {
  LOG(VERBOSE) << __func__ << ": mute=" << muted;
  return -ENOSYS;
}

int audio_get_master_mute(struct audio_hw_device* dev __unused, bool* muted __unused) {
  return -ENOSYS;
}

int audio_set_mode(struct audio_hw_device* dev __unused, audio_mode_t mode) {
  LOG(VERBOSE) << __func__ << ": mode=" << mode;
  return 0;
}

int audio_set_mic_mute(struct audio_hw_device* dev __unused, bool state) {
  LOG(VERBOSE) << __func__ << ": state=" << state;
  return -ENOSYS;
}

int audio_get_mic_mute(const struct audio_hw_device* dev __unused, bool* state __unused) {
  return -ENOSYS;
}

size_t audio_get_input_buffer_size(const struct audio_hw_device* dev,
                                  const struct audio_config* config) {
  if(aidl_1_0) {
    return adev_get_input_buffer_size_aidl_1_0(dev, config);
  } else if (hal_2_1)
    return adev_get_input_buffer_size_2_1(dev, config);
  else
    return adev_get_input_buffer_size(dev, config);
}

int audio_open_output_stream(struct audio_hw_device* dev,
                            audio_io_handle_t handle, audio_devices_t devices,
                            audio_output_flags_t flags,
                            struct audio_config* config,
                            struct audio_stream_out** stream_out,
                            const char* address __unused) {
  LOG(INFO) << __func__;

  if (!adev_open_output_stream_aidl_1_0(dev, handle, devices, flags,
                                          config, stream_out, address)) {
    aidl_1_0 = true;
    return 0;
  } else if(!adev_open_output_stream_2_1(dev, handle, devices, flags,
                                          config, stream_out, address)) {
    hal_2_1 = true;
    return 0;
  } else {
    return adev_open_output_stream(dev, handle, devices, flags,
                                 config, stream_out, address);
  }
}

void audio_close_output_stream(struct audio_hw_device* dev,
                              struct audio_stream_out* stream){
  if(aidl_1_0) {
    adev_close_output_stream_aidl_1_0(dev, stream);
  } else if (hal_2_1)
    adev_close_output_stream_2_1(dev, stream);
  else
    adev_close_output_stream(dev, stream);
}

int audio_open_input_stream(struct audio_hw_device* dev,
                           audio_io_handle_t handle, audio_devices_t devices,
                           struct audio_config* config,
                           struct audio_stream_in** stream_in,
                           audio_input_flags_t flags __unused,
                           const char* address __unused,
                           audio_source_t source __unused){
  if(aidl_1_0) {
    return adev_open_input_stream_aidl_1_0(dev, handle, devices, config,
                                  stream_in, flags, address, source);
  } else if (hal_2_1)
    return adev_open_input_stream_2_1(dev, handle, devices, config,
                                  stream_in, flags, address, source);
  else
    return adev_open_input_stream(dev, handle, devices, config,
                                  stream_in, flags, address, source);
}

void audio_close_input_stream(struct audio_hw_device* dev,
                             struct audio_stream_in* in){
  if(aidl_1_0) {
    adev_close_input_stream_aidl_1_0(dev, in);
  } else if (hal_2_1)
    adev_close_input_stream_2_1(dev, in);
  else
    adev_close_input_stream(dev, in);
}

int audio_dump(const audio_hw_device_t* device __unused, int fd __unused) {
  if(aidl_1_0) {
    return adev_dump_aidl_1_0(device, fd);
  } else if (hal_2_1)
    return adev_dump_2_1(device, fd);
  else
    return adev_dump(device, fd);
}

int audio_close(hw_device_t* device) {
  free(device);
  return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device) {
  LOG(VERBOSE) << __func__ << ": name=[" << name << "]";
  if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0) return -EINVAL;

  createIBluetoothAudioProviderFactory();

  if(isIBluetoothAudioProviderFactoryAvailable()) {
    auto bluetooth_audio_device = new BluetoothAudioDevice{};
    struct audio_hw_device* adev = &bluetooth_audio_device->audio_device_;
    if (!adev) return -ENOMEM;

    adev->common.tag = HARDWARE_DEVICE_TAG;
    adev->common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->common.module = (struct hw_module_t*)module;
    adev->common.close = adev_close;

    adev->init_check = audio_init_check;
    adev->set_voice_volume = audio_set_voice_volume;
    adev->set_master_volume = audio_set_master_volume;
    adev->get_master_volume = audio_get_master_volume;
    adev->set_mode = audio_set_mode;
    adev->set_mic_mute = audio_set_mic_mute;
    adev->get_mic_mute = audio_get_mic_mute;
    adev->set_parameters = audio_set_parameters;
    adev->get_parameters = audio_get_parameters;
    adev->get_input_buffer_size = audio_get_input_buffer_size;
    adev->open_output_stream = audio_open_output_stream;
    adev->close_output_stream = audio_close_output_stream;
    adev->open_input_stream = audio_open_input_stream;
    adev->close_input_stream = audio_close_input_stream;
    adev->dump = audio_dump;
    adev->set_master_mute = audio_set_master_mute;
    adev->get_master_mute = audio_get_master_mute;
    *device = &adev->common;
  } else {
    struct bluetooth_audio_device* adev =
        (struct bluetooth_audio_device*)
         calloc(1, sizeof(struct bluetooth_audio_device));

    if (!adev) return -ENOMEM;

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->device.common.module = (struct hw_module_t*)module;
    adev->device.common.close = adev_close;

    adev->device.init_check = audio_init_check;
    adev->device.set_voice_volume = audio_set_voice_volume;
    adev->device.set_master_volume = audio_set_master_volume;
    adev->device.get_master_volume = audio_get_master_volume;
    adev->device.set_mode = audio_set_mode;
    adev->device.set_mic_mute = audio_set_mic_mute;
    adev->device.get_mic_mute = audio_get_mic_mute;
    adev->device.set_parameters = audio_set_parameters;
    adev->device.get_parameters = audio_get_parameters;
    adev->device.get_input_buffer_size = audio_get_input_buffer_size;
    adev->device.open_output_stream = audio_open_output_stream;
    adev->device.close_output_stream = audio_close_output_stream;
    adev->device.open_input_stream = audio_open_input_stream;
    adev->device.close_input_stream = audio_close_input_stream;
    adev->device.dump = audio_dump;
    adev->device.set_master_mute = audio_set_master_mute;
    adev->device.get_master_mute = audio_get_master_mute;
    *device = &adev->device.common;
  }
  vendor::qti::btoffload::bt_audio_pre_init_qti();
  vendor::qti::btoffload::V2_1::bt_audio_pre_init_qti();

  return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common =
        {
            .tag = HARDWARE_MODULE_TAG,
            .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
            .hal_api_version = HARDWARE_HAL_API_VERSION,
            .id = AUDIO_HARDWARE_MODULE_ID,
            .name = "Bluetooth Audio HW HAL",
            .author = "The Android Open Source Project",
            .methods = &hal_module_methods,
        },
};