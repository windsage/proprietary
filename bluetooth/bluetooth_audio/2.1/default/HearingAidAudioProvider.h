/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

 /*
 * Copyright 2018 The Android Open Source Project
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

#pragma once

#include <fmq/MessageQueue.h>
#include <hidl/MQDescriptor.h>

#include "BluetoothAudioProvider.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth_audio {
namespace V2_1 {
namespace implementation {

using ::android::hardware::kSynchronizedReadWrite;
using ::android::hardware::MessageQueue;
using DataMQ = MessageQueue<uint8_t, kSynchronizedReadWrite>;
using ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType;
using AudioConfiguration_2_0 =
     ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration;

class HearingAidAudioProvider : public BluetoothAudioProvider {
 public:
  HearingAidAudioProvider();

  bool isValid(const SessionType& sessionType) override;
  Return<void> startSession(const sp<BluetoothAudioPort_2_0>& hostIf,
                            const AudioConfiguration_2_0& audioConfig,
                            startSession_cb _hidl_cb) override;

  Return<void> startSession_2_1(const sp<IBluetoothAudioPort>& hostIf,
                            const AudioConfiguration& audioConfig,
                            startSession_2_1_cb _hidl_cb) override;

 private:
  // audio data queue for software encoding
  std::unique_ptr<DataMQ> mDataMQ;
  //SessionType session_type_;
  Return<void> onSessionReady(startSession_cb _hidl_cb) override;
};

}  // namespace implementation
}  // namespace V2_0
}  // namespace bluetooth_audio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
