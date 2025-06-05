/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a contribution.
 *
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

#define LOG_TAG "BTAudioCodecsAidl"

#include "BluetoothAudioCodecs.h"

#include <aidl/android/hardware/bluetooth/audio/AacCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/AacObjectType.h>
#include <aidl/android/hardware/bluetooth/audio/AptxCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/ChannelMode.h>
#include <aidl/android/hardware/bluetooth/audio/LdacCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/LdacChannelMode.h>
#include <aidl/android/hardware/bluetooth/audio/LdacQualityIndex.h>
#include <aidl/android/hardware/bluetooth/audio/LeAudioConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/LeAudioBroadcastConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/SbcCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/AptxAdaptiveCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/AptxAdaptiveLeCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/AptxAdaptiveLeConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/SbcChannelMode.h>
#include <aidl/android/hardware/bluetooth/audio/AptxSinkBuffering.h>
#include <aidl/android/hardware/bluetooth/audio/AptxAdaptiveTimeToPlay.h>
#include <android-base/logging.h>

#include "BluetoothLeAudioCodecsProvider.h"

namespace aidl {
namespace android {
namespace hardware {
namespace bluetooth {
namespace audio {

static const PcmCapabilities kDefaultSoftwarePcmCapabilities = {
    .sampleRateHz = {16000, 24000, 44100, 48000, 88200, 96000},
    .channelMode = {ChannelMode::MONO, ChannelMode::STEREO},
    .bitsPerSample = {16, 24, 32},
    .dataIntervalUs = {},
};

static const SbcCapabilities kDefaultOffloadSbcCapability = {
    .sampleRateHz = {44100},
    .channelMode = {SbcChannelMode::MONO, SbcChannelMode::JOINT_STEREO},
    .blockLength = {4, 8, 12, 16},
    .numSubbands = {8},
    .allocMethod = {SbcAllocMethod::ALLOC_MD_L},
    .bitsPerSample = {16},
    .minBitpool = 2,
    .maxBitpool = 250};

static const AacCapabilities kDefaultOffloadAacCapability = {
    .objectType = {AacObjectType::MPEG2_LC},
    .sampleRateHz = {44100},
    .channelMode = {ChannelMode::STEREO},
    .variableBitRateSupported = true,
    .bitsPerSample = {16}};

static const LdacCapabilities kDefaultOffloadLdacCapability = {
    .sampleRateHz = {44100, 48000, 88200, 96000},
    .channelMode = {LdacChannelMode::DUAL, LdacChannelMode::STEREO},
    .qualityIndex = {LdacQualityIndex::HIGH, LdacQualityIndex::ABR,
                     LdacQualityIndex::MID, LdacQualityIndex::LOW},
    .bitsPerSample = {16, 24, 32}};

static const AptxCapabilities kDefaultOffloadAptxCapability = {
    .sampleRateHz = {44100, 48000},
    .channelMode = {ChannelMode::STEREO},
    .bitsPerSample = {16},
};

static const AptxCapabilities kDefaultOffloadAptxHdCapability = {
    .sampleRateHz = {44100, 48000},
    .channelMode = {ChannelMode::STEREO},
    .bitsPerSample = {24},
};

// aptX Adaptive Default Sink Buffering and TTL
static const AptxSinkBuffering kDefaultAptxAdaptiveSinkBuffering = {
    .minLowLatency = {20},
    .maxLowLatency = {50},
    .minHighQuality = {20},
    .maxHighQuality = {50},
    .minTws = {20},
    .maxTws = {50}
};

static const AptxAdaptiveTimeToPlay  kDefaultAptxAdaptive_TTP = {
    .lowLowLatency = {69},
    .highLowLatency = {69},
    .lowHighQuality = {100},
    .highHighQuality = {100},
    .lowTws = {100},
    .highTws = {100}
};

// aptX Adaptive: mSampleRate:(44100|48000), mBitsPerSample:(24),
//          mChannelMode:(JOINT_STEREO)
static const AptxAdaptiveCapabilities kDefaultOffloadAptxAdaptiveCapability = {
    .sampleRateHz = {44100, 48000, 96000},
    .channelMode = {AptxAdaptiveChannelMode::DUAL_MONO, AptxAdaptiveChannelMode::JOINT_STEREO},
    .bitsPerSample = {24},
    .aptxMode = {AptxMode::HIGH_QUALITY},
    .sinkBufferingMs = kDefaultAptxAdaptiveSinkBuffering,
    .ttp = kDefaultAptxAdaptive_TTP
};

static const Lc3Capabilities kDefaultA2dpOffloadLc3Capability = {
    .samplingFrequencyHz = {44100, 48000},
    .frameDurationUs = {7500, 10000},
    .channelMode = {ChannelMode::MONO, ChannelMode::STEREO},
};

const std::vector<CodecCapabilities> kDefaultOffloadA2dpCodecCapabilities = {
    {.codecType = CodecType::SBC, .capabilities = {}},
    {.codecType = CodecType::AAC, .capabilities = {}},
    {.codecType = CodecType::LDAC, .capabilities = {}},
    {.codecType = CodecType::APTX, .capabilities = {}},
    {.codecType = CodecType::APTX_HD, .capabilities = {}},
    {.codecType = CodecType::APTX_ADAPTIVE, .capabilities = {}},
    {.codecType = CodecType::LC3, .capabilities = {}}};

std::vector<LeAudioCodecCapabilitiesSetting> kDefaultOffloadLeAudioCapabilities;

static const UnicastCapability kInvalidUnicastCapability = {
    .codecType = CodecType::UNKNOWN};

// Default Supported Codecs
// LC3 16_1: sample rate: 16 kHz, frame duration: 7.5 ms, octets per frame: 30
static const Lc3Capabilities kLc3Capability_16_1 = {
    .samplingFrequencyHz = {16000},
    .frameDurationUs = {7500},
    .octetsPerFrame = {30}};

// Default Supported Codecs
// LC3 16_2: sample rate: 16 kHz, frame duration: 10 ms, octets per frame: 40
static const Lc3Capabilities kLc3Capability_16_2 = {
    .samplingFrequencyHz = {16000},
    .frameDurationUs = {10000},
    .octetsPerFrame = {40}};

// LC3 24_1: sample rate: 24 kHz, frame duration: 7.5 ms, octets per frame: 45
static const Lc3Capabilities kLc3Capability_24_1 = {
    .samplingFrequencyHz = {24000},
    .frameDurationUs = {7500},
    .octetsPerFrame = {45}};

// LC3 24_2: sample rate: 24 kHz, frame duration: 10 ms, octets per frame: 60
static const Lc3Capabilities kLc3Capability_24_2 = {
    .samplingFrequencyHz = {24000},
    .frameDurationUs = {10000},
    .octetsPerFrame = {60}};

// LC3 32_1: sample rate: 32 kHz, frame duration: 7.5 ms, octets per frame: 60
static const Lc3Capabilities kLc3Capability_32_1 = {
    .samplingFrequencyHz = {32000},
    .frameDurationUs = {7500},
    .octetsPerFrame = {60}};

// LC3 32_2: sample rate: 32 kHz, frame duration: 10 ms, octets per frame: 80
static const Lc3Capabilities kLc3Capability_32_2 = {
    .samplingFrequencyHz = {32000},
    .frameDurationUs = {10000},
    .octetsPerFrame = {80}};

// LC3 48_1: sample rate: 48 kHz, frame duration: 7.5 ms, octets per frame: 75
static const Lc3Capabilities kLc3Capability_48_1 = {
    .samplingFrequencyHz = {48000},
    .frameDurationUs = {7500},
    .octetsPerFrame = {75}};

// LC3 48_2: sample rate: 48 kHz, frame duration: 10 ms, octets per frame: 100
static const Lc3Capabilities kLc3Capability_48_2 = {
    .samplingFrequencyHz = {48000},
    .frameDurationUs = {10000},
    .octetsPerFrame = {100}};

// LC3 48_3: sample rate: 48 kHz, frame duration: 7.5 ms, octets per frame: 90
static const Lc3Capabilities kLc3Capability_48_3 = {
    .samplingFrequencyHz = {48000},
    .frameDurationUs = {7500},
    .octetsPerFrame = {90}};

// Default Supported Codecs
// LC3 48_4: sample rate: 48 kHz, frame duration: 10 ms, octets per frame: 120
static const Lc3Capabilities kLc3Capability_48_4 = {
    .samplingFrequencyHz = {48000},
    .frameDurationUs = {10000},
    .octetsPerFrame = {120}};

// LC3 48_5: sample rate: 48 kHz, frame duration: 7.5 ms, octets per frame: 117
static const Lc3Capabilities kLc3Capability_48_5 = {
    .samplingFrequencyHz = {48000},
    .frameDurationUs = {7500},
    .octetsPerFrame = {117}};

// LC3 48_6: sample rate: 48 kHz, frame duration: 10 ms, octets per frame: 155
static const Lc3Capabilities kLc3Capability_48_6 = {
    .samplingFrequencyHz = {48000},
    .frameDurationUs = {10000},
    .octetsPerFrame = {155}};

static const std::vector<Lc3Capabilities> supportedLc3CapabilityList = {
    kLc3Capability_48_1, kLc3Capability_48_2, kLc3Capability_48_3,
    kLc3Capability_48_4, kLc3Capability_48_5, kLc3Capability_48_6,
    kLc3Capability_32_1, kLc3Capability_32_2, kLc3Capability_24_1,
    kLc3Capability_24_2, kLc3Capability_16_1, kLc3Capability_16_2, };

static const std::vector<Lc3Capabilities> supportedBcastLc3CapabilityList = {
    kLc3Capability_48_2, kLc3Capability_48_1, kLc3Capability_48_3,
    kLc3Capability_48_4, kLc3Capability_48_5, kLc3Capability_48_6,
    kLc3Capability_24_2, kLc3Capability_16_2};

static const AptxAdaptiveLeCapabilities
    kDefaultOffloadAptxAdaptiveLeCapability_48k = {
        .samplingFrequencyHz = {48000},
        .frameDurationUs = {10000},
        .octetsPerFrame = {816}};

static const AptxAdaptiveLeCapabilities
    kDefaultOffloadAptxAdaptiveLeCapability_96k = {
        .samplingFrequencyHz = {96000},
        .frameDurationUs = {10000},
        .octetsPerFrame = {816}};

static const AptxAdaptiveLeCapabilities
    kDefaultOffloadAptxAdaptiveLeXCapability_48k = {
        .samplingFrequencyHz = {48000},
        .frameDurationUs = {10000},
        .octetsPerFrame = {816}};

static const AptxAdaptiveLeCapabilities
    kDefaultOffloadAptxAdaptiveLeXCapability_96k = {
        .samplingFrequencyHz = {96000},
        .frameDurationUs = {10000},
        .octetsPerFrame = {816}};

static const BroadcastCapability kInvalidBroadcastCapability = {
    .codecType = CodecType::UNKNOWN};

static AudioLocation stereoAudio = static_cast<AudioLocation>(
    static_cast<uint8_t>(AudioLocation::FRONT_LEFT) |
    static_cast<uint8_t>(AudioLocation::FRONT_RIGHT));
static AudioLocation monoAudio = AudioLocation::UNKNOWN;

static const std::vector<AptxAdaptiveLeCapabilities>
    supportedAptxAdaptiveLeCapabilityList = {
        kDefaultOffloadAptxAdaptiveLeCapability_48k,
        kDefaultOffloadAptxAdaptiveLeCapability_96k,
        kDefaultOffloadAptxAdaptiveLeXCapability_48k,
        kDefaultOffloadAptxAdaptiveLeXCapability_96k};

// Stores the supported setting of audio location, connected device, and the
// channel count for each device
std::vector<std::tuple<AudioLocation, uint8_t, uint8_t>>
    supportedDeviceSetting = {
        // Stereo, two connected device, one for L one for R
        std::make_tuple(stereoAudio, 2, 1),
        // Stereo, one connected device for both L and R
        std::make_tuple(stereoAudio, 1, 2),
        // Mono
        std::make_tuple(monoAudio, 1, 1)};


template <class T>
bool BluetoothAudioCodecs::ContainedInVector(
    const std::vector<T>& vector, const typename identity<T>::type& target) {
  return std::find(vector.begin(), vector.end(), target) != vector.end();
}

bool BluetoothAudioCodecs::IsOffloadSbcConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() != CodecConfiguration::CodecSpecific::sbcConfig) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const SbcConfiguration sbc_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::sbcConfig>();

  if (ContainedInVector(kDefaultOffloadSbcCapability.sampleRateHz,
                        sbc_data.sampleRateHz) &&
      ContainedInVector(kDefaultOffloadSbcCapability.blockLength,
                        sbc_data.blockLength) &&
      ContainedInVector(kDefaultOffloadSbcCapability.numSubbands,
                        sbc_data.numSubbands) &&
      ContainedInVector(kDefaultOffloadSbcCapability.bitsPerSample,
                        sbc_data.bitsPerSample) &&
      ContainedInVector(kDefaultOffloadSbcCapability.channelMode,
                        sbc_data.channelMode) &&
      ContainedInVector(kDefaultOffloadSbcCapability.allocMethod,
                        sbc_data.allocMethod) &&
      sbc_data.minBitpool <= sbc_data.maxBitpool &&
      kDefaultOffloadSbcCapability.minBitpool <= sbc_data.minBitpool &&
      kDefaultOffloadSbcCapability.maxBitpool >= sbc_data.maxBitpool) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadAacConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() != CodecConfiguration::CodecSpecific::aacConfig) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const AacConfiguration aac_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::aacConfig>();

  if (ContainedInVector(kDefaultOffloadAacCapability.sampleRateHz,
                        aac_data.sampleRateHz) &&
      ContainedInVector(kDefaultOffloadAacCapability.bitsPerSample,
                        aac_data.bitsPerSample) &&
      ContainedInVector(kDefaultOffloadAacCapability.channelMode,
                        aac_data.channelMode) &&
      ContainedInVector(kDefaultOffloadAacCapability.objectType,
                        aac_data.objectType) &&
      (!aac_data.variableBitRateEnabled ||
       kDefaultOffloadAacCapability.variableBitRateSupported)) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadLdacConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() !=
      CodecConfiguration::CodecSpecific::ldacConfig) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const LdacConfiguration ldac_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::ldacConfig>();

  if (ContainedInVector(kDefaultOffloadLdacCapability.sampleRateHz,
                        ldac_data.sampleRateHz) &&
      ContainedInVector(kDefaultOffloadLdacCapability.bitsPerSample,
                        ldac_data.bitsPerSample) &&
      ContainedInVector(kDefaultOffloadLdacCapability.channelMode,
                        ldac_data.channelMode) &&
      ContainedInVector(kDefaultOffloadLdacCapability.qualityIndex,
                        ldac_data.qualityIndex)) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadAptxConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() !=
      CodecConfiguration::CodecSpecific::aptxConfig) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const AptxConfiguration aptx_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::aptxConfig>();

  if (ContainedInVector(kDefaultOffloadAptxCapability.sampleRateHz,
                        aptx_data.sampleRateHz) &&
      ContainedInVector(kDefaultOffloadAptxCapability.bitsPerSample,
                        aptx_data.bitsPerSample) &&
      ContainedInVector(kDefaultOffloadAptxCapability.channelMode,
                        aptx_data.channelMode)) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadAptxHdConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() !=
      CodecConfiguration::CodecSpecific::aptxConfig) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const AptxConfiguration aptx_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::aptxConfig>();

  if (ContainedInVector(kDefaultOffloadAptxHdCapability.sampleRateHz,
                        aptx_data.sampleRateHz) &&
      ContainedInVector(kDefaultOffloadAptxHdCapability.bitsPerSample,
                        aptx_data.bitsPerSample) &&
      ContainedInVector(kDefaultOffloadAptxHdCapability.channelMode,
                        aptx_data.channelMode)) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadAptxAdaptiveConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() !=
     CodecConfiguration::CodecSpecific::aptxAdaptiveConfig) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const AptxAdaptiveConfiguration aptxAdaptive_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::aptxAdaptiveConfig>();
  if (ContainedInVector(kDefaultOffloadAptxAdaptiveCapability.sampleRateHz,
                        aptxAdaptive_data.sampleRateHz) &&
      ContainedInVector(kDefaultOffloadAptxAdaptiveCapability.bitsPerSample,
                        aptxAdaptive_data.bitsPerSample) &&
      ContainedInVector(kDefaultOffloadAptxAdaptiveCapability.channelMode,
                        aptxAdaptive_data.channelMode)) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadLc3ConfigurationValid(
    const CodecConfiguration::CodecSpecific& codec_specific) {
  if (codec_specific.getTag() != CodecConfiguration::CodecSpecific::lc3Config) {
    LOG(WARNING) << __func__
                 << ": Invalid CodecSpecific=" << codec_specific.toString();
    return false;
  }
  const Lc3Configuration lc3_data =
      codec_specific.get<CodecConfiguration::CodecSpecific::lc3Config>();

  if (ContainedInVector(kDefaultA2dpOffloadLc3Capability.samplingFrequencyHz,
                        lc3_data.samplingFrequencyHz) &&
      ContainedInVector(kDefaultA2dpOffloadLc3Capability.frameDurationUs,
                        lc3_data.frameDurationUs) &&
      ContainedInVector(kDefaultA2dpOffloadLc3Capability.channelMode,
                        lc3_data.channelMode)) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << codec_specific.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadLeAudioBroadcastConfigurationValid(
    const SessionType& session_type, const LeAudioBroadcastConfiguration&) {
  if (session_type !=
          SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
    return false;
  }
  return true;
}

std::vector<PcmCapabilities>
BluetoothAudioCodecs::GetSoftwarePcmCapabilities() {
  return {kDefaultSoftwarePcmCapabilities};
}

std::vector<CodecCapabilities>
BluetoothAudioCodecs::GetA2dpOffloadCodecCapabilities(
    const SessionType& session_type) {
  if (session_type != SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH &&
      session_type != SessionType::A2DP_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
    return {};
  }
  std::vector<CodecCapabilities> offload_a2dp_codec_capabilities =
      kDefaultOffloadA2dpCodecCapabilities;
  for (auto& codec_capability : offload_a2dp_codec_capabilities) {
    switch (codec_capability.codecType) {
      case CodecType::SBC:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::sbcCapabilities>(
                kDefaultOffloadSbcCapability);
        break;
      case CodecType::AAC:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::aacCapabilities>(
                kDefaultOffloadAacCapability);
        break;
      case CodecType::LDAC:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::ldacCapabilities>(
                kDefaultOffloadLdacCapability);
        break;
      case CodecType::APTX:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::aptxCapabilities>(
                kDefaultOffloadAptxCapability);
        break;
      case CodecType::APTX_HD:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::aptxCapabilities>(
                kDefaultOffloadAptxHdCapability);
        break;
      case CodecType::APTX_ADAPTIVE:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::aptxAdaptiveCapabilities>(
                kDefaultOffloadAptxAdaptiveCapability);
        break;
      case CodecType::LC3:
        codec_capability.capabilities
            .set<CodecCapabilities::Capabilities::lc3Capabilities>(
                kDefaultA2dpOffloadLc3Capability);
        break;
      case CodecType::UNKNOWN:
      case CodecType::VENDOR:
      case CodecType::APTX_ADAPTIVE_LE:
      case CodecType::APTX_ADAPTIVE_LEX:
        break;
    }
  }
  return offload_a2dp_codec_capabilities;
}

bool BluetoothAudioCodecs::IsSoftwarePcmConfigurationValid(
    const PcmConfiguration& pcm_config) {
  if (ContainedInVector(kDefaultSoftwarePcmCapabilities.sampleRateHz,
                        pcm_config.sampleRateHz) &&
      ContainedInVector(kDefaultSoftwarePcmCapabilities.bitsPerSample,
                        pcm_config.bitsPerSample) &&
      ContainedInVector(kDefaultSoftwarePcmCapabilities.channelMode,
                        pcm_config.channelMode)
      // data interval is not checked for now
      // && pcm_config.dataIntervalUs != 0
  ) {
    return true;
  }
  LOG(WARNING) << __func__
               << ": Unsupported CodecSpecific=" << pcm_config.toString();
  return false;
}

bool BluetoothAudioCodecs::IsOffloadCodecConfigurationValid(
    const SessionType& session_type, const CodecConfiguration& codec_config) {
  if (session_type != SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH &&
      session_type != SessionType::A2DP_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
    LOG(ERROR) << __func__
               << ": Invalid SessionType=" << toString(session_type);
    return false;
  }
  const CodecConfiguration::CodecSpecific& codec_specific = codec_config.config;
  switch (codec_config.codecType) {
    case CodecType::SBC:
      if (IsOffloadSbcConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::AAC:
      if (IsOffloadAacConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::LDAC:
      if (IsOffloadLdacConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::APTX:
      if (IsOffloadAptxConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::APTX_HD:
      if (IsOffloadAptxHdConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::APTX_ADAPTIVE:
      if (IsOffloadAptxAdaptiveConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::LC3:
      if (IsOffloadLc3ConfigurationValid(codec_specific)) {
        return true;
      }
      break;
    case CodecType::UNKNOWN:
    case CodecType::VENDOR:
      break;
  }
  return false;
}

UnicastCapability composeUnicastLc3Capability(
    AudioLocation audioLocation, uint8_t deviceCnt, uint8_t channelCount,
    const Lc3Capabilities& capability) {
  return {
      .codecType = CodecType::LC3,
      .supportedChannel = audioLocation,
      .deviceCount = deviceCnt,
      .channelCountPerDevice = channelCount,
      .leAudioCodecCapabilities =
          UnicastCapability::LeAudioCodecCapabilities(capability),
  };
}

template <class T>
BroadcastCapability composeBroadcastLc3Capability(
    AudioLocation& audio_location, uint8_t channel_count,
    const std::vector<T>& capability) {
  return {
      .codecType = CodecType::LC3,
      .supportedChannel = audio_location,
      .channelCountPerStream = channel_count,
      .leAudioCodecCapabilities = std::optional(capability)};
}

std::vector<LeAudioCodecCapabilitiesSetting>
BluetoothAudioCodecs::GetLeAudioOffloadCodecCapabilities(
    const SessionType& session_type) {
  if (session_type !=
          SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH &&
      session_type !=
          SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH &&
      session_type !=
          SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
    return std::vector<LeAudioCodecCapabilitiesSetting>(0);
  }

  if (kDefaultOffloadLeAudioCapabilities.empty()) {
    /*auto le_audio_offload_setting =
           BluetoothLeAudioCodecsProvider::ParseFromLeAudioOffloadSettingFile();
    kDefaultOffloadLeAudioCapabilities =
             BluetoothLeAudioCodecsProvider::GetLeAudioCodecCapabilities(
                 le_audio_offload_setting);
  }

  for (auto [audioLocation, deviceCnt, channelCount] :
         supportedDeviceSetting) {
      for (auto capability : supportedAptxAdaptiveLeCapabilityList) {
        for (auto codec_type :
             {CodecType::APTX_ADAPTIVE_LE, CodecType::APTX_ADAPTIVE_LEX}) {
          if (session_type ==
              SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            UnicastCapability aptx_adaptive_le_cap = {
                .codecType = codec_type,
                .supportedChannel = audioLocation,
                .deviceCount = deviceCnt,
                .channelCountPerDevice = channelCount,
                .leAudioCodecCapabilities =
                    UnicastCapability::LeAudioCodecCapabilities(capability),
            };

            // Adds the capability for encode only
            kDefaultOffloadLeAudioCapabilities.push_back(
                {.unicastEncodeCapability = aptx_adaptive_le_cap,
                 .unicastDecodeCapability = kInvalidUnicastCapability,
                 .broadcastCapability = kInvalidBroadcastCapability});
          }
        }
      }
    }*/
    for (auto [audioLocation, deviceCnt, channelCount] :
         supportedDeviceSetting) {
      for (auto capability : supportedLc3CapabilityList) {
        UnicastCapability lc3Capability = composeUnicastLc3Capability(
            audioLocation, deviceCnt, channelCount, capability);
        UnicastCapability lc3MonoDecodeCapability =
            composeUnicastLc3Capability(monoAudio, 1, 1, capability);

        // Adds the capability for encode only
        kDefaultOffloadLeAudioCapabilities.push_back(
            {.unicastEncodeCapability = lc3Capability,
             .unicastDecodeCapability = kInvalidUnicastCapability,
             .broadcastCapability = kInvalidBroadcastCapability});

        // Adds the capability for decode only
        kDefaultOffloadLeAudioCapabilities.push_back(
            {.unicastEncodeCapability = kInvalidUnicastCapability,
             .unicastDecodeCapability = lc3Capability,
             .broadcastCapability = kInvalidBroadcastCapability});

        // Adds the capability for the case that encode and decode exist at the
        // same time
        kDefaultOffloadLeAudioCapabilities.push_back(
            {.unicastEncodeCapability = lc3Capability,
             .unicastDecodeCapability = lc3Capability,
             .broadcastCapability = kInvalidBroadcastCapability});
      }
    }

    for (auto capability : supportedBcastLc3CapabilityList) {
      std::vector<std::optional<Lc3Capabilities>> bcastLc3Cap(
          1, std::optional(capability));
      BroadcastCapability lc3BroadcastStereoCapability =
          composeBroadcastLc3Capability(stereoAudio, 2, bcastLc3Cap);
      BroadcastCapability lc3BroadcastMonoCapability =
          composeBroadcastLc3Capability(monoAudio, 1, bcastLc3Cap);

      // Adds the capability for broadcast stereo audio
      kDefaultOffloadLeAudioCapabilities.push_back(
          {.unicastEncodeCapability = kInvalidUnicastCapability,
           .unicastDecodeCapability = kInvalidUnicastCapability,
           .broadcastCapability = lc3BroadcastStereoCapability});

      // Adds the capability for broadcast mono audio
      kDefaultOffloadLeAudioCapabilities.push_back(
          {.unicastEncodeCapability = kInvalidUnicastCapability,
           .unicastDecodeCapability = kInvalidUnicastCapability,
           .broadcastCapability = lc3BroadcastMonoCapability});
    }
  }

  return kDefaultOffloadLeAudioCapabilities;
}

}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
}  // namespace aidl
