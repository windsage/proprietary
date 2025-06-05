/*
 * Copyright (c) 2019,2022 Qualcomm Technologies, Inc.
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

#include <mutex>
#include <unordered_map>

#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioPort.h>
#include <fmq/MessageQueue.h>
#include <hardware/audio.h>
#include <hidl/MQDescriptor.h>
#include "vendor/qti/hardware/bluetooth_audio/2.1/types.h"

namespace vendor {
namespace qti {
namespace bluetooth_audio {
namespace V2_1 {
using ::android::sp;
using ::android::hardware::kSynchronizedReadWrite;
using ::android::hardware::MessageQueue;
using ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode;
using ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams;
using ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType;
using AudioConfiguration_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration;
using CodecConfiguration_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration;
using BluetoothAudioStatus =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status;
using BluetoothAudioPort_2_0 =
    ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort;

using DataMQ = MessageQueue<uint8_t, kSynchronizedReadWrite>;

static constexpr uint16_t kObserversCookieSize = 0x0010;  // 0x0000 ~ 0x000f
constexpr uint16_t kObserversCookieUndefined =
    (static_cast<uint16_t>(SessionType::UNKNOWN) << 8 & 0xff00);
inline SessionType ObserversCookieGetSessionType(uint16_t cookie) {
  return static_cast<SessionType>(cookie >> 8 & 0x00ff);
}
inline uint16_t ObserversCookieGetInitValue(SessionType session_type) {
  return (static_cast<uint16_t>(session_type) << 8 & 0xff00);
}
inline uint16_t ObserversCookieGetUpperBound(SessionType session_type) {
  return (static_cast<uint16_t>(session_type) << 8 & 0xff00) +
         kObserversCookieSize;
}

// This presents the callbacks of started / suspended and session changed,
// and the bluetooth_audio module uses to receive the status notification
struct PortStatusCallbacks {
  // control_result_cb_ - when the Bluetooth stack reports results of
  // streamStarted or streamSuspended, the BluetoothAudioProvider will invoke
  // this callback to report to the bluetooth_audio module.
  // @param: cookie - indicates which bluetooth_audio output should handle
  // @param: start_resp - this report is for startStream or not
  // @param: status - the result of startStream
  std::function<void(uint16_t cookie, bool start_resp,
                     const BluetoothAudioStatus& status)>
      control_result_cb_;
  // session_changed_cb_ - when the Bluetooth stack start / end session, the
  // BluetoothAudioProvider will invoke this callback to notify to the
  // bluetooth_audio module.
  // @param: cookie - indicates which bluetooth_audio output should handle
  std::function<void(uint16_t cookie)> session_changed_cb_;
  // session_params_cb_ - when the Bluetooth stack updates the session
  // parameters like mtu, bit rate, BluetoothAudioProvider will invoke this
  // callback to notify to the bluetooth_audio module.
  // @param: cookie - indicates which bluetooth_audio output should handle
  // @param: session_param - indicates which session parameter is updated
  std::function<void(uint16_t cookie, const SessionParams &session_params)>
       session_params_cb_;
};

class BluetoothAudioSession {
 private:
  // using recursive_mutex to allow hwbinder to re-enter agian.
  std::recursive_mutex mutex_;
  SessionType session_type_;

  // audio control path to use for both software and offloading
  sp<IBluetoothAudioPort> stack_iface_;
  sp<BluetoothAudioPort_2_0> stack_iface_2_0_;
  // audio data path (FMQ) for software encoding
  std::unique_ptr<DataMQ> mDataMQ;
  // audio data configuration for both software and offloading
  AudioConfiguration audio_config_;
  AudioConfiguration_2_0 audio_config_2_0_;
  bool client_2_0_;
  static AudioConfiguration invalidSoftwareAudioConfiguration;
  static AudioConfiguration invalidOffloadAudioConfiguration;

  static AudioConfiguration_2_0 invalidSoftwareAudioConfiguration_2_0;
  static AudioConfiguration_2_0 invalidOffloadAudioConfiguration_2_0;

  // saving those registered bluetooth_audio's callbacks
  std::unordered_map<uint16_t, std::shared_ptr<struct PortStatusCallbacks>>
      observers_;

  bool UpdateDataPath(const DataMQ::Descriptor* dataMQ);
  bool UpdateAudioConfig(const AudioConfiguration& audio_config,
                                const AudioConfiguration_2_0& audio_config_2_0);
  // invoking the registered session_changed_cb_
  void ReportSessionStatus();

 public:
  BluetoothAudioSession(const SessionType& session_type);

  // The function helps to check if this session is ready or not
  // @return: true if the Bluetooth stack has started the specified session
  bool IsSessionReady();

  // The report function is used to report that the Bluetooth stack has started
  // this session without any failure, and will invoke session_changed_cb_ to
  // notify those registered bluetooth_audio outputs
  void OnSessionStarted(const sp<IBluetoothAudioPort> stack_iface,
                        const DataMQ::Descriptor* dataMQ,
                        const AudioConfiguration& audio_config,
                        const AudioConfiguration_2_0& audio_config_2_0,
                        const sp<BluetoothAudioPort_2_0> statck_iface_2_0,
                        bool client_version);

  // The report function is used to report that the Bluetooth stack has ended
  // the session, and will invoke session_changed_cb_ to notify registered
  // bluetooth_audio outputs
  void OnSessionEnded();

  // The report function is used to report that the Bluetooth stack has notified
  // the result of startStream or suspendStream, and will invoke
  // control_result_cb_ to notify registered bluetooth_audio outputs
  void ReportControlStatus(bool start_resp, const BluetoothAudioStatus& status);

  void OnSessionParamUpdate(const SessionParamType& paramType,
                      const SessionParams& sessionParams);

  // The control function helps the bluetooth_audio module to register
  // PortStatusCallbacks
  // @return: cookie - the assigned number to this bluetooth_audio output

  uint16_t RegisterStatusCback(const PortStatusCallbacks& cbacks);

  // The control function helps the bluetooth_audio module to unregister
  // PortStatusCallbacks
  // @param: cookie - indicates which bluetooth_audio output is
  void UnregisterStatusCback(uint16_t cookie);

  // The control function is for the bluetooth_audio module to get the current
  // AudioConfiguration
  const AudioConfiguration& GetAudioConfig();
  const AudioConfiguration_2_0& GetAudioConfig_2_0();

  bool isClient_2_0();
  // Those control functions are for the bluetooth_audio module to start,
  // suspend, stop stream, to check position, and to update metadata.
  bool StartStream();
  bool SuspendStream();
  void StopStream();
  bool UpdateSinkLatency(uint16_t sink_latency);
  bool GetPresentationPosition(uint64_t* remote_delay_report_ns,
                               uint64_t* total_bytes_readed,
                               timespec* data_position);
  void UpdateTracksMetadata(const struct source_metadata* source_metadata);

  // The control function writes stream to FMQ
  size_t OutWritePcmData(const void* buffer, size_t bytes);

  /*
   * This function reports the sink latency shared as delay reporting
   * Local values will be updated if data is already fetched once
   */
  bool a2dpGetSinkLatency(const SessionType& session_type,
                                      uint64_t* remote_delay_report_ns,
                                      uint64_t* total_bytes_readed,
                                      timespec* data_position);

  /*static const PcmParameters kInvalidPcmParameters = {
      .sampleRate = SampleRate::RATE_UNKNOWN,
      .bitsPerSample = BitsPerSample::BITS_UNKNOWN,
      .channelMode = ChannelMode::UNKNOWN};*/
  // can't be constexpr because of non-literal type
  static const CodecConfiguration kInvalidCodecConfiguration;
  static const PcmParameters kInvalidPcmParameters;

  static const CodecConfiguration_2_0 kInvalidCodecConfiguration_2_0;
  static constexpr AudioConfiguration& kInvalidSoftwareAudioConfiguration =
      invalidSoftwareAudioConfiguration;
  static constexpr AudioConfiguration& kInvalidOffloadAudioConfiguration =
      invalidOffloadAudioConfiguration;

  static constexpr AudioConfiguration_2_0& kInvalidSoftwareAudioConfiguration_2_0 =
      invalidSoftwareAudioConfiguration_2_0;
  static constexpr AudioConfiguration_2_0& kInvalidOffloadAudioConfiguration_2_0 =
      invalidOffloadAudioConfiguration_2_0;
};

class BluetoothAudioSessionInstance {
 public:
  // The API is to fetch the specified session of A2DP / Hearing Aid
  static std::shared_ptr<BluetoothAudioSession> GetSessionInstance(
      const SessionType& session_type);

 private:
  static std::unique_ptr<BluetoothAudioSessionInstance> instance_ptr;
  std::mutex mutex_;
  std::unordered_map<SessionType, std::shared_ptr<BluetoothAudioSession>>
      sessions_map_;
};
}  //V2_1
}  // namespace bluetooth_audio
}  // namespace qti
}  // namespace vendor
