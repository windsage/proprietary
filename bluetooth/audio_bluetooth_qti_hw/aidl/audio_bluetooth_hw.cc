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
#include <hardware/hardware.h>
#include <log/log.h>
#include <malloc.h>
#include <string.h>
#include <system/audio.h>

#include "stream_apis.h"
#include "utils.h"

using ::android::bluetooth::audio::utils::GetAudioParamString;
using ::android::bluetooth::audio::utils::ParseAudioParams;

int adev_set_parameters_aidl_1_0(struct audio_hw_device* dev,
                               const char* kvpairs) {
  LOG(VERBOSE) << __func__ << ": kevpairs=[" << kvpairs << "]";
  std::unordered_map<std::string, std::string> params =
      ParseAudioParams(kvpairs);
  if (params.empty()) return 0;

  LOG(VERBOSE) << __func__ << ": ParamsMap=[" << GetAudioParamString(params)
               << "]";
  if (params.find("A2dpSuspended") == params.end()) {
    return -ENOSYS;
  }

  auto* bluetooth_device = reinterpret_cast<BluetoothAudioDevice*>(dev);
  std::lock_guard<std::mutex> guard(bluetooth_device->mutex_);
  for (auto sout : bluetooth_device->opened_stream_outs_) {
    if (sout->stream_out_.common.set_parameters != nullptr) {
      sout->stream_out_.common.set_parameters(&sout->stream_out_.common,
                                              kvpairs);
    }
  }
  return 0;
}

char* adev_get_parameters_aidl_1_0(const struct audio_hw_device* dev,
                                 const char* keys) {
  LOG(VERBOSE) << __func__ << ": keys=[" << keys << "]";
  return strdup("");
}

int adev_init_check_aidl_1_0(const struct audio_hw_device* dev) { return 0; }

int adev_set_voice_volume(struct audio_hw_device* dev, float volume) {
  LOG(VERBOSE) << __func__ << ": volume=" << volume;
  return -ENOSYS;
}

int adev_set_master_volume_aidl_1_0(struct audio_hw_device* dev, float volume) {
  LOG(VERBOSE) << __func__ << ": volume=" << volume;
  return -ENOSYS;
}

int adev_get_master_volume_aidl_1_0(struct audio_hw_device* dev, float* volume) {
  return -ENOSYS;
}

int adev_set_master_mute_aidl_1_0(struct audio_hw_device* dev, bool muted) {
  LOG(VERBOSE) << __func__ << ": mute=" << muted;
  return -ENOSYS;
}

int adev_get_master_mute_aidl_1_0(struct audio_hw_device* dev, bool* muted) {
  return -ENOSYS;
}

int adev_set_mode_aidl_1_0(struct audio_hw_device* dev, audio_mode_t mode) {
  LOG(VERBOSE) << __func__ << ": mode=" << mode;
  return 0;
}

int adev_set_mic_mute_aidl_1_0(struct audio_hw_device* dev, bool state) {
  LOG(VERBOSE) << __func__ << ": state=" << state;
  return -ENOSYS;
}

int adev_get_mic_mute_aidl_1_0(const struct audio_hw_device* dev, bool* state) {
  return -ENOSYS;
}

int adev_dump_aidl_1_0(const audio_hw_device_t* device, int fd) { return 0; }

int adev_close_aidl_1_0(hw_device_t* device) {
  auto* bluetooth_device = reinterpret_cast<BluetoothAudioDevice*>(device);
  delete bluetooth_device;
  return 0;
}