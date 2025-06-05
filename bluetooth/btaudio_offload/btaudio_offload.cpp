/****************************************************************************
 * Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 ****************************************************************************/
/*****************************************************************************
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
/*     btaudio_offload.c
 *
 *  Description:   Implements IPC interface between HAL and BT host
 *
 *****************************************************************************/
#include <time.h>
#include <unistd.h>

#include "ldac_level_bit_rate_lookup.h"
#include "btaudio_offload.h"
#include "btaudio_offload_qti.h"
#include "btaudio_offload_qti_2_1.h"
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <stdint.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <system/audio.h>
#include <utils/Log.h>
#include <cutils/properties.h>

#include "aidl_session/BluetoothAudioSessionControl.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <utils/Log.h>
#include <dlfcn.h>
#include "BluetoothAudioProviderFactory.h"
#ifdef AIDL_EXTENSION
#include "BluetoothAudioProviderExt.h"
#endif /* AIDL_EXTENSION */

#include <android-base/logging.h>

#include <aidl/vendor/qti/hardware/bluetooth/audio/LeAudioVendorConfiguration.h>
#include <aidl/vendor/qti/hardware/bluetooth/audio/VendorCodecType.h>
#include <aidl/android/hardware/bluetooth/audio/LatencyMode.h>

using ::aidl::android::hardware::bluetooth::audio::BluetoothAudioProviderFactory;

#ifdef AIDL_EXTENSION
using ::aidl::android::hardware::bluetooth::audio::extension::BluetoothAudioProviderExt;
#endif /* AIDL_EXTENSION */

using ::aidl::android::hardware::bluetooth::audio::CodecType;
using ::aidl::vendor::qti::hardware::bluetooth::audio::LeAudioVendorConfiguration;
using VendorCodecType = ::aidl::vendor::qti::hardware::bluetooth::audio::VendorCodecType;
using VendorConfiguration =
    ::aidl::android::hardware::bluetooth::audio::LeAudioCodecConfiguration::VendorConfiguration;
using LatencyMode = ::aidl::android::hardware::bluetooth::audio::LatencyMode;


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "btaudio_offload_aidl"

#define ENC_CODEC_TYPE_APTX_ADAPTIVE 620756992u // 0x25000000UL

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
/* Below two values adds up to 8 sec retry to address IOT issues*/
#define STREAM_START_MAX_RETRY_COUNT 10
#define STREAM_START_MAX_RETRY_LOOPER 8
#define CTRL_CHAN_RETRY_COUNT 3
/* 2DH5 Max (679) = 663 + 12 (AVDTP header) + 4 (L2CAP header) */
#define MAX_2MBPS_A2DP_STREAM_MTU 663
/* 3DH5 Max (1021) = 1005 + 12 (AVDTP header) + 4 (L2CAP header) */
#define MAX_3MBPS_A2DP_STREAM_MTU 1005
#define STREAM_OPEN_RETRIAL_COUNT  10
#define IN_CALL_TRIAL_COUNT 5
#define FAIL_TRIAL_COUNT 10
#define AAC_SAMPLE_SIZE  1024
#define AAC_LATM_HEADER  12

#define CASE_RETURN_STR(const) case const: return #const;

#define FNLOG()         ALOGW(LOG_TAG, "%s", __FUNCTION__);
#define DEBUG(fmt, ...) ALOGD(LOG_TAG, "%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define INFO(fmt, ...)  ALOGI(LOG_TAG, "%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define WARN(fmt, ...)  ALOGW(LOG_TAG, "%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define ERROR(fmt, ...) ALOGE(LOG_TAG, "%s: " fmt,__FUNCTION__, ## __VA_ARGS__)

#define ASSERTC(cond, msg, val) \
    if (!(cond)) { \
      ERROR("### ASSERT : %s line %d %s (%d) ###", __FILE__, __LINE__, msg, val); \
    }

/*****************************************************************************
**  Local type definitions
******************************************************************************/

static struct a2dp_stream_common audio_stream[MAX_SESSION_TYPE];
static volatile unsigned char ack_recvd = 0;
pthread_cond_t ack_cond = PTHREAD_COND_INITIALIZER;
int  max_waittime = 2000; // in ms
int wait_for_stack_response(int time_to_wait, tSESSION_TYPE session_type);
static audio_format_t codecType_;
bool qti_audio_hal = false;
bool aidl_client_enabled = false;
bool qti_audio_hal_2_1 = false;
static pthread_t thread_config_changed;
static bool config_thread_created = false;
static std::mutex mtx_config_changed;
static std::condition_variable cv_config_changed;
static int (*reconfig_to_audio_cb)(tSESSION_TYPE session_type, int status) = NULL;
static std::mutex api_lock;
using BluetoothAudioSessionControl =
         aidl::android::hardware::bluetooth::audio::BluetoothAudioSessionControl;

static bool is_qti_system_supported();
static bool qti_system_supported = false;

static void *vndr_fwk_lib_handle = NULL;
typedef int (*vndr_fwk_enhanced_info_t)();
static vndr_fwk_enhanced_info_t vndr_fwk_enhanced_info;

typedef struct {
  tSESSION_TYPE session;
  uint8_t status;
} session_reconfig_t;
std::vector<session_reconfig_t> reconfig_queue;

#define VNDR_FWK_DETECT_LIB "libqti_vndfwk_detect_vendor.so"

/*****************************************************************************
**  Static functions
******************************************************************************/

audio_sbc_encoder_config_t sbc_codec;
audio_aptx_encoder_config_t aptx_codec;
audio_aac_encoder_config_t aac_codec;
audio_ldac_encoder_config_t ldac_codec;
audio_lc3_codec_config_t lc3_enc_codec;
audio_lc3_codec_config_t lc3_dec_codec;
audio_lc3_codec_config_t lc3_bca_codec;
audio_aptx_adaptive_encoder_config_t aptx_adaptive_codec;

/*****************************************************************************
**  Functions
******************************************************************************/
void a2dp_open_ctrl_path(struct a2dp_stream_common *common);
void ldac_codec_parser(AudioConfiguration *codec_cfg);
static void btapoffload_port_deinit(tSESSION_TYPE session_type, bool unreg);
extern binder_status_t createIBluetoothAudioProviderFactory();
/*****************************************************************************
**   Miscellaneous helper functions
******************************************************************************/
static const char* dump_ctrl_ack(tCTRL_ACK ack)
{
    switch (ack) {
        CASE_RETURN_STR(CTRL_ACK_SUCCESS)
        CASE_RETURN_STR(CTRL_ACK_FAILURE)
        CASE_RETURN_STR(CTRL_ACK_INCALL_FAILURE)
        CASE_RETURN_STR(CTRL_ACK_PENDING)
        CASE_RETURN_STR(CTRL_ACK_DISCONNECT_IN_PROGRESS)
        CASE_RETURN_STR(CTRL_SKT_DISCONNECTED)
        CASE_RETURN_STR(CTRL_ACK_UNSUPPORTED)
        CASE_RETURN_STR(CTRL_ACK_UNKNOWN)
        CASE_RETURN_STR(CTRL_ACK_RECONFIGURATION)
    }
    return "UNKNOWN A2DP_CTRL_ACK";
}

static const char* dump_a2dp_hal_state(tAUDIO_STATE state)
{
    switch (state) {
        CASE_RETURN_STR(AUDIO_STATE_STARTING)
        CASE_RETURN_STR(AUDIO_STATE_STARTED)
        CASE_RETURN_STR(AUDIO_STATE_STOPPING)
        CASE_RETURN_STR(AUDIO_STATE_STOPPED)
        CASE_RETURN_STR(AUDIO_STATE_SUSPENDED)
        CASE_RETURN_STR(AUDIO_STATE_STANDBY)
    }
    return "UNKNOWN A2DP_STATE";
}

tCTRL_ACK aidl_to_audio_status(BluetoothAudioStatus status) {
    tCTRL_ACK ret = CTRL_ACK_UNKNOWN;
    switch (status) {
        case BluetoothAudioStatus::UNKNOWN: ret = CTRL_ACK_UNKNOWN;
            break;
        case BluetoothAudioStatus::SUCCESS: ret = CTRL_ACK_SUCCESS;
            break;
        case BluetoothAudioStatus::UNSUPPORTED_CODEC_CONFIGURATION: ret = CTRL_ACK_UNSUPPORTED;
            break;
        case BluetoothAudioStatus::FAILURE: ret = CTRL_ACK_FAILURE;
            break;
        case BluetoothAudioStatus::RECONFIGURATION: ret = CTRL_ACK_RECONFIGURATION;
          break;
        default: ret = CTRL_ACK_UNKNOWN;
            break;
    }
    return ret;
}

static bool is_qti_system_supported() {
    int vendor_info = -1;
    if(vndr_fwk_lib_handle == NULL) {
        ALOGE("%s: openning the lib %s", __func__, VNDR_FWK_DETECT_LIB);
        vndr_fwk_lib_handle = dlopen(VNDR_FWK_DETECT_LIB, RTLD_NOW | RTLD_LOCAL);
        if (vndr_fwk_lib_handle == NULL) {
            ALOGE("%s: vndr_fwk_lib_handle is null", __func__);
            return false;
        }
    } else {
      ALOGD("%s: Build is running with QC Value-adds for system ", __func__);
      return qti_system_supported;
    }
    vndr_fwk_enhanced_info = (vndr_fwk_enhanced_info_t)
                dlsym(vndr_fwk_lib_handle, "getVendorEnhancedInfo");
    if (vndr_fwk_enhanced_info == NULL) {
        ALOGE("%s: vndr_fwk_enhanced_info symbol not found", __func__);
        return false;
    }

    vendor_info = vndr_fwk_enhanced_info();
    // 0: Pure AOSP for both system and odm;
    // 1: Pure AOSP for system and QC Value-adds for odm
    // 2: QC value-adds for system and Pure AOSP for odm
    // 3: QC value-adds for both system and odm.
    ALOGD("%s: vendor_info = %d", __func__, vendor_info);

    if(vendor_info != 2 && vendor_info != 3) {
        ALOGE("%s: Build is not running with QC Value-adds for system ", __func__);
        return false;
    } else {
        ALOGD("%s: Build is running with QC Value-adds for system ", __func__);
        qti_system_supported = true;
        return true;
    }
}

static void* le_audio_codec_parser(a2dp_stream_common*a2dp_stream,
                               tSESSION_TYPE session_type,
                               audio_format_t *codec_type,
                               uint32_t *sample_freq) {
    const LeAudioConfiguration * leAudioConfigEnc = NULL;
    const LeAudioConfiguration * leAudioConfigDec = NULL;
    const LeAudioBroadcastConfiguration * leAudioBroadcastConfig = NULL;
    VendorCodecType vendor_codec = VendorCodecType::LC3Q;
    audio_lc3_codec_config_t lc3_codec;
    memset(&lc3_codec, 0, sizeof(audio_lc3_codec_config_t));
    lc3_codec.is_enc_config_set = false;
    lc3_codec.is_dec_config_set = false;

    pthread_mutex_lock(&audio_stream[session_type].ack_lock);
    if(session_type == LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
        ALOGD("%s: encode session",__func__);
        leAudioConfigEnc = &a2dp_stream->codec_cfg.get<AudioConfiguration::leAudioConfig>();
        lc3_codec.is_enc_config_set = true;
    } else if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
        ALOGD("%s: decode session",__func__);
        leAudioConfigEnc = &audio_stream[LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH].codec_cfg.get<AudioConfiguration::leAudioConfig>();
        leAudioConfigDec = &a2dp_stream->dec_codec_cfg.get<AudioConfiguration::leAudioConfig>();
        lc3_codec.is_enc_config_set = true;
        lc3_codec.is_dec_config_set = true;
        if(leAudioConfigEnc->codecType != CodecType::VENDOR) {
            ALOGD("%s: codec is not vendor type",__func__);
            if(leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().samplingFrequencyHz == 0) {
                ALOGE("%s: Invalid encoder session, set is_enc_config_set to false",__func__);
                lc3_codec.is_enc_config_set = false;
            }
        }
        if (leAudioConfigEnc->codecType != leAudioConfigDec->codecType) {
            ALOGD("%s: encoding and decoding codec types are different",__func__);
            lc3_codec.is_enc_config_set = false;
        } else if (leAudioConfigEnc->codecType == CodecType::VENDOR) {
            ALOGD("%s: encoding codec is vendor type",__func__);
            std::optional<LeAudioVendorConfiguration> le_vendor_enc_config;
            std::optional<LeAudioVendorConfiguration> le_vendor_dec_config;
            leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::vendorConfig>().extension.getParcelable(&le_vendor_enc_config);
            leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::vendorConfig>().extension.getParcelable(&le_vendor_dec_config);
            if (le_vendor_enc_config->vendorCodecType != le_vendor_dec_config->vendorCodecType) {
                lc3_codec.is_enc_config_set = false;
            }
        }
    } else if (session_type == LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH){
        ALOGD("%s: broadcast session",__func__);
        leAudioBroadcastConfig = &a2dp_stream->codec_cfg.get<AudioConfiguration::leAudioBroadcastConfig>();
        lc3_codec.is_enc_config_set = true;
    } else {
        ALOGE("%s: invalid session type",__func__);
        pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
        return NULL;
    }
    ALOGD("%s: lc3_codec.is_enc_config_set: %d",__func__, lc3_codec.is_enc_config_set);
    ALOGD("%s: lc3_codec.is_dec_config_set: %d",__func__, lc3_codec.is_dec_config_set);
    switch (session_type) {
        case LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH: /* fall through */
        case LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH:
             // Fill encoder for both enc and dec session type
                if(leAudioConfigEnc->codecType == CodecType::VENDOR) {
                  ALOGE("%s: LE vendor codec found ",__func__);
                  std::optional<LeAudioVendorConfiguration> le_vendor_config;
                  leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::vendorConfig>().extension.getParcelable(&le_vendor_config);

                  vendor_codec = le_vendor_config->vendorCodecType;
                  ALOGE("%s: vendorCodecType %d ",__func__, le_vendor_config->vendorCodecType);
                  if(vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4) {
                    ALOGE("%s: R4 vendor codec found ",__func__);
                    lc3_codec.enc_config.toAirConfig.sampling_freq = le_vendor_config->samplingFrequencyHz;
                    lc3_codec.enc_config.toAirConfig.api_version = 0x00;
                    lc3_codec.enc_config.toAirConfig.max_octets_per_frame =
                                     0x05DC0000 | (le_vendor_config->octetsPerFrame);
                    lc3_codec.enc_config.toAirConfig.frame_duration = le_vendor_config->frameDurationUs;
                    lc3_codec.enc_config.toAirConfig.bit_depth = (uint32_t)le_vendor_config->pcmBitDepth;
                    lc3_codec.enc_config.toAirConfig.num_blocks = 0x00;
                    lc3_codec.enc_config.toAirConfig.mode = 0;
                    for (int i = 0; i < 16; i++) {
                      lc3_codec.enc_config.toAirConfig.vendor_specific[i] =
                                      le_vendor_config->codecSpecificData[i];
                      ALOGE("%s: R4 vendor codec metadata for encoding session [%d] : %x ",__func__,
                             i, lc3_codec.enc_config.toAirConfig.vendor_specific[i]);
                    }
                    for (int i = 16; i < 20; i++) {
                      lc3_codec.enc_config.toAirConfig.mode =
                        (lc3_codec.enc_config.toAirConfig.mode << 8) | le_vendor_config->codecSpecificData[i];
                    }
                    lc3_codec.enc_config.toAirConfig.default_q_level = 0x00;
                    ALOGE("%s: R4 vendor codec mode %d",__func__, lc3_codec.enc_config.toAirConfig.mode);
                  } else { // not (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4)
                    lc3_codec.enc_config.toAirConfig.mode = 1;
                    lc3_codec.enc_config.toAirConfig.sampling_freq = le_vendor_config->samplingFrequencyHz;
                    lc3_codec.enc_config.toAirConfig.api_version = 0x00000021;
                    lc3_codec.enc_config.toAirConfig.max_octets_per_frame =
                                      le_vendor_config->octetsPerFrame;
                    lc3_codec.enc_config.toAirConfig.frame_duration = le_vendor_config->frameDurationUs;
                    lc3_codec.enc_config.toAirConfig.bit_depth =
                                      (uint32_t)le_vendor_config->pcmBitDepth;
                    lc3_codec.enc_config.toAirConfig.num_blocks =
                                      le_vendor_config->blocksPerSdu;
                    /* setting all vendor specific info as zero */
                    for (int i = 0; i < 16; i++) {
                      lc3_codec.enc_config.toAirConfig.vendor_specific[i] =
                                     le_vendor_config->codecSpecificData[i];
                    }
                    lc3_codec.enc_config.toAirConfig.default_q_level = 0x00;
                  } // not (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4)
                } else { // not (leAudioConfigEnc->codecType == CodecType::VENDOR)
                  lc3_codec.enc_config.toAirConfig.mode = 1;
                  lc3_codec.enc_config.toAirConfig.sampling_freq = leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().samplingFrequencyHz;
                  lc3_codec.enc_config.toAirConfig.api_version = 0x00000021;
                  lc3_codec.enc_config.toAirConfig.max_octets_per_frame =
                                    leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().octetsPerFrame;
                  lc3_codec.enc_config.toAirConfig.frame_duration = leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().frameDurationUs;
                  lc3_codec.enc_config.toAirConfig.bit_depth =
                                    (uint32_t)leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().pcmBitDepth;
                  lc3_codec.enc_config.toAirConfig.num_blocks =
                                    leAudioConfigEnc->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().blocksPerSdu;
                  /* setting all vendor specific info as zero */
                  for (int i = 0; i < 16; i++) {
                      lc3_codec.enc_config.toAirConfig.vendor_specific[i] = 0;
                  }
                  lc3_codec.enc_config.toAirConfig.default_q_level = 0x00;
                }
                for (int i = 0; i < 16; i++) {
                  ALOGE("%s: LE vendor codec metadata for encoding session [%d] : %x ",__func__,
                         i, lc3_codec.enc_config.toAirConfig.vendor_specific[i]);
                }
            if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {

                if(leAudioConfigDec->codecType == CodecType::VENDOR) {
                  ALOGE("%s: LE vendor codec found ",__func__);
                  std::optional<LeAudioVendorConfiguration> le_vendor_config;
                  leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::vendorConfig>().extension.getParcelable(&le_vendor_config);

                  vendor_codec = le_vendor_config->vendorCodecType;

                  ALOGE("%s: vendorCodecType %d ",__func__, le_vendor_config->vendorCodecType);
                  if(vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4) {
                    ALOGE("%s: R4 vendor codec found ",__func__);
                    lc3_codec.dec_config.fromAirConfig.sampling_freq = le_vendor_config->samplingFrequencyHz;
                    lc3_codec.dec_config.fromAirConfig.api_version = 0x00;
                    lc3_codec.dec_config.fromAirConfig.max_octets_per_frame =
                                     0x05DC0000|(le_vendor_config->octetsPerFrame);
                    lc3_codec.dec_config.fromAirConfig.frame_duration = le_vendor_config->frameDurationUs;
                    lc3_codec.dec_config.fromAirConfig.bit_depth = (uint32_t)le_vendor_config->pcmBitDepth;
                    lc3_codec.dec_config.fromAirConfig.num_blocks = 0x00;
                    lc3_codec.dec_config.fromAirConfig.mode = 0;
                    for (int i = 0; i < 16; i++) {
                      lc3_codec.dec_config.fromAirConfig.vendor_specific[i] =
                                     le_vendor_config->codecSpecificData[i];
                      ALOGE("%s: R4 vendor codec metadata for decoding session [%d] : %x ",__func__,
                             i, lc3_codec.dec_config.fromAirConfig.vendor_specific[i]);
                    }
                    for (int i = 16; i < 20; i++) {
                      lc3_codec.dec_config.fromAirConfig.mode =
                        (lc3_codec.dec_config.fromAirConfig.mode << 8) | le_vendor_config->codecSpecificData[i];
                    }
                    lc3_codec.dec_config.fromAirConfig.default_q_level = 0x00;
                    ALOGE("%s: R4 vendor codec mode %d",__func__,
                             lc3_codec.dec_config.fromAirConfig.mode);
                  } else { // not (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4)
                    lc3_codec.dec_config.fromAirConfig.mode = 0;
                    lc3_codec.dec_config.fromAirConfig.sampling_freq = le_vendor_config->samplingFrequencyHz;
                    lc3_codec.dec_config.fromAirConfig.api_version = 0x00000021;
                    lc3_codec.dec_config.fromAirConfig.max_octets_per_frame =
                                      le_vendor_config->octetsPerFrame;
                    lc3_codec.dec_config.fromAirConfig.frame_duration =
                                    le_vendor_config->frameDurationUs;
                    lc3_codec.dec_config.fromAirConfig.bit_depth =
                                      (uint32_t)le_vendor_config->pcmBitDepth;
                    lc3_codec.dec_config.fromAirConfig.num_blocks =
                                      le_vendor_config->blocksPerSdu;
                    /* setting all vendor specific info as zero */
                    for (int i = 0; i < 16; i++) {
                        lc3_codec.dec_config.fromAirConfig.vendor_specific[i] =
                                        le_vendor_config->codecSpecificData[i];;
                    }
                    lc3_codec.dec_config.fromAirConfig.default_q_level = 0x01;
                  } // not (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4)
                } else { // not (leAudioConfigDec->codecType == CodecType::VENDOR)
                  lc3_codec.dec_config.fromAirConfig.mode = 0;
                  lc3_codec.dec_config.fromAirConfig.sampling_freq = leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().samplingFrequencyHz;
                  lc3_codec.dec_config.fromAirConfig.api_version = 0x00000021;
                  lc3_codec.dec_config.fromAirConfig.max_octets_per_frame =
                                    leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().octetsPerFrame;
                  lc3_codec.dec_config.fromAirConfig.frame_duration =
                                  leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().frameDurationUs;
                  lc3_codec.dec_config.fromAirConfig.bit_depth =
                                    (uint32_t)leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().pcmBitDepth;
                  lc3_codec.dec_config.fromAirConfig.num_blocks =
                                    leAudioConfigDec->leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().blocksPerSdu;
                  /* setting all vendor specific info as zero */
                  for (int i = 0; i < 16; i++) {
                      lc3_codec.dec_config.fromAirConfig.vendor_specific[i] = 0;
                  }
                  lc3_codec.dec_config.fromAirConfig.default_q_level = 0x01;
                } // not (leAudioConfigDec->codecType == CodecType::VENDOR)
                for (int i = 0; i < 16; i++) {
                  ALOGE("%s: LE vendor codec metadata for decoding session [%d] : %x ",__func__,
                         i, lc3_codec.dec_config.fromAirConfig.vendor_specific[i]);
                }
            } // (session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH)

            lc3_codec.enc_config.stream_map_size =  leAudioConfigEnc->streamMap.size();
            if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {//find number of out and in stream map
                    lc3_codec.dec_config.stream_map_size =  leAudioConfigDec->streamMap.size();
                    lc3_codec.dec_config.decoder_output_channel = 2;
            }

            lc3_codec.enc_config.streamMapOut =
                  (lc3_stream_map_t*)malloc(lc3_codec.enc_config.stream_map_size*sizeof(lc3_stream_map_t));
            {
                lc3_stream_map_t *p_out = lc3_codec.enc_config.streamMapOut;

                for (int i = 0; i < leAudioConfigEnc->streamMap.size(); i++) {
                    p_out->audio_location = leAudioConfigEnc->streamMap[i].audioChannelAllocation;
                    p_out->stream_id = i; // (uint8_t)leAudioConfigEnc->streamMap[i].streamHandle;
                    p_out->direction = 0x00;
                    ALOGD("%s: stream_map p_out: stream_id = %d, audio location = %d,"
                               " direction = %d", __func__, p_out->stream_id,
                               p_out->audio_location, p_out->direction);
                    p_out++;
                }
                if ((session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) &&
                     (lc3_codec.dec_config.stream_map_size > 0)) {
                    lc3_codec.dec_config.streamMapIn =
                      (lc3_stream_map_t*)malloc(lc3_codec.dec_config.stream_map_size*sizeof(lc3_stream_map_t));
                    lc3_stream_map_t *p_in = lc3_codec.dec_config.streamMapIn;
                    for (int i = 0; i < leAudioConfigDec->streamMap.size(); i++) {
                        p_in->audio_location = leAudioConfigDec->streamMap[i].audioChannelAllocation;
                        p_in->stream_id = i; //(uint8_t)leAudioConfigDec->streamMap[i].streamHandle;
                        p_in->direction = 0x01;
                        ALOGD("%s: stream_map p_in: stream_id = %d, audio location = %d,"
                                   " direction = %d", __func__, p_in->stream_id,
                                   p_in->audio_location, p_in->direction);
                        p_in++;
                    }
                }
            }

            ALOGD("%s:Unicast LE Enc: sampling_freq = %d api_version = %d, max_octets_per_frame = %d, frame_duration = %d, bit_depth = %d, num_blocks = %d, mode = %d",
              __func__, lc3_codec.enc_config.toAirConfig.sampling_freq,
            lc3_codec.enc_config.toAirConfig.api_version,
            lc3_codec.enc_config.toAirConfig.max_octets_per_frame,
            lc3_codec.enc_config.toAirConfig.frame_duration,
            lc3_codec.enc_config.toAirConfig.bit_depth,
            lc3_codec.enc_config.toAirConfig.num_blocks,
            lc3_codec.enc_config.toAirConfig.mode);
            if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
                ALOGD("%s:Unicast LE Dec: sampling_freq = %d api_version = %d, max_octets_per_frame = %d, frame_duration = %d, bit_depth = %d, num_blocks = %d, mode = %d",
                  __func__, lc3_codec.dec_config.fromAirConfig.sampling_freq,
                  lc3_codec.dec_config.fromAirConfig.api_version,
                  lc3_codec.dec_config.fromAirConfig.max_octets_per_frame,
                  lc3_codec.dec_config.fromAirConfig.frame_duration,
                  lc3_codec.dec_config.fromAirConfig.bit_depth,
                  lc3_codec.dec_config.fromAirConfig.num_blocks,
                  lc3_codec.dec_config.fromAirConfig.mode);
            }

        if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
              ALOGD("%s: session_type is LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH",__func__);
              if (leAudioConfigEnc->codecType == CodecType::VENDOR) {
                  if (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R3) {
                      ALOGW("%s: codec_type APTX ADAPTIVE LE update to MM Audio", __func__);
                      *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_APTX_ADAPTIVE_LE);
                  } else if (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4) {
                      ALOGW("%s: codec_type APTX ADAPTIVE R4 update to MM Audio", __func__);
                      *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_APTX_ADAPTIVE_R4);
                  } else {
                      ALOGW("%s: codec_type LC3Q update to MM Audio", __func__);
                      *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_LC3);
                  }
              } else {
                  ALOGW("%s: codec_type LC3 update to MM Audio", __func__);
                  *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_LC3);
              }
        } else {
              if (leAudioConfigDec->codecType == CodecType::VENDOR) {
                  if (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R3) {
                      ALOGW("%s: codec_type APTX ADAPTIVE LE update to MM Audio", __func__);
                      *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_APTX_ADAPTIVE_LE);
                  } else if (vendor_codec == VendorCodecType::APTX_ADAPTIVE_R4) {
                      ALOGW("%s: codec_type APTX ADAPTIVE R4 update to MM Audio", __func__);
                      *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_APTX_ADAPTIVE_R4);
                  } else {
                      ALOGW("%s: codec_type LC3Q update to MM Audio", __func__);
                      *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_LC3);
                  }
              } else {
                  ALOGW("%s: codec_type LC3 update to MM Audio", __func__);
                  *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_LC3);
              }
        }
            codecType_ = (*codec_type);

            if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
                if (lc3_enc_codec.enc_config.streamMapOut != NULL) {
                    ALOGD("%s: Free allocated streamMapOut memory",__func__);
                    free(lc3_enc_codec.enc_config.streamMapOut);
                    lc3_enc_codec.enc_config.streamMapOut = NULL;
                }
                memset(&lc3_enc_codec, 0, sizeof(audio_lc3_codec_config_t));
                //shallow-copy only
                lc3_enc_codec = lc3_codec;
                memset(&lc3_codec, 0, sizeof(audio_lc3_codec_config_t));
                pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
                return ((void*)&lc3_enc_codec);
            } else { // not (session_type == LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH)
                if (lc3_dec_codec.enc_config.streamMapOut != NULL) {
                    ALOGD("%s: Free allocated streamMapOut memory",__func__);
                    free(lc3_dec_codec.enc_config.streamMapOut);
                    lc3_dec_codec.enc_config.streamMapOut = NULL;
                }
                if (lc3_dec_codec.dec_config.streamMapIn != NULL) {
                    ALOGD("%s: Free allocated streamMapIn memory",__func__);
                    free(lc3_dec_codec.dec_config.streamMapIn);
                    lc3_dec_codec.dec_config.streamMapIn = NULL;
                }
                memset(&lc3_dec_codec, 0, sizeof(audio_lc3_codec_config_t));
                //shallow-copy only
                lc3_dec_codec = lc3_codec;
                memset(&lc3_codec, 0, sizeof(audio_lc3_codec_config_t));
                pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
                return ((void*)&lc3_dec_codec);
            } // not (session_type == LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH)
            break;

        case LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH:
            lc3_codec.enc_config.toAirConfig.sampling_freq = leAudioBroadcastConfig->streamMap[0].leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().samplingFrequencyHz;
            lc3_codec.enc_config.toAirConfig.api_version = 0x00000021;
            lc3_codec.enc_config.toAirConfig.max_octets_per_frame =
                              leAudioBroadcastConfig->streamMap[0].leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().octetsPerFrame;
            lc3_codec.enc_config.toAirConfig.frame_duration = leAudioBroadcastConfig->streamMap[0].leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().frameDurationUs;
            lc3_codec.enc_config.toAirConfig.bit_depth =
                              (uint32_t)leAudioBroadcastConfig->streamMap[0].leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().pcmBitDepth;
            lc3_codec.enc_config.toAirConfig.num_blocks =
                              leAudioBroadcastConfig->streamMap[0].leAudioCodecConfig.get<LeAudioCodecConfiguration::lc3Config>().blocksPerSdu;
            /* setting all vendor specific info as zero */
            for (int i = 0; i < 16; i++) {
                lc3_codec.enc_config.toAirConfig.vendor_specific[i] = 0;
            }
            lc3_codec.enc_config.toAirConfig.mode = 2;
            lc3_codec.enc_config.toAirConfig.default_q_level = 0x03;
            lc3_codec.enc_config.stream_map_size =  leAudioBroadcastConfig->streamMap.size();
            lc3_codec.enc_config.streamMapOut =
                  (lc3_stream_map_t*)malloc(lc3_codec.enc_config.stream_map_size*sizeof(lc3_stream_map_t));
            {
                lc3_stream_map_t *p_out = lc3_codec.enc_config.streamMapOut;

                for (int i = 0; i < leAudioBroadcastConfig->streamMap.size(); i++) {
                    p_out->audio_location = leAudioBroadcastConfig->streamMap[i].audioChannelAllocation;
                    p_out->stream_id = i; //(uint8_t)leAudioBroadcastConfig->streamMap[i].streamHandle;
                    p_out->direction = 0x00;
                    ALOGD("%s: stream_map p_out: stream_id = %d, audio location = %d,"
                               " direction = %d", __func__, p_out->stream_id,
                               p_out->audio_location, p_out->direction);
                    p_out++;
                }
            }
            ALOGD("%s:Broadcat LE Encr: SR = %d octets = %d, input_frame = %d, num_blocks = %d",
                      __func__, lc3_codec.enc_config.toAirConfig.sampling_freq,
            lc3_codec.enc_config.toAirConfig.max_octets_per_frame,
            lc3_codec.enc_config.toAirConfig.frame_duration,
            lc3_codec.enc_config.toAirConfig.num_blocks);
            *codec_type = static_cast<audio_format_t>(AUDIO_CODEC_TYPE_LC3);

            if (lc3_bca_codec.enc_config.streamMapOut != NULL) {
                ALOGD("%s: Free allocated streamMap memory",__func__);
                free(lc3_bca_codec.enc_config.streamMapOut);
                lc3_bca_codec.enc_config.streamMapOut = NULL;
            }
            memset(&lc3_bca_codec, 0, sizeof(audio_lc3_codec_config_t));
            //shallow-copy only
            lc3_bca_codec = lc3_codec;
            memset(&lc3_codec, 0, sizeof(audio_lc3_codec_config_t));
            pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
            return ((void*)&lc3_bca_codec);
            break;

        default:
            break;
    }

    pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
    return NULL;
}

static void* a2dp_codec_parser(AudioConfiguration *codec_cfg,
                               audio_format_t *codec_type,
                               uint32_t *sample_freq)
{
    CodecConfiguration *codecConfig =
          &codec_cfg->get<AudioConfiguration::a2dpConfig>();
    ALOGI("%s: codec_type = %x", __func__, codecConfig->codecType);
    if (codecConfig->codecType == CodecType::SBC) {
        memset(&sbc_codec, 0, sizeof(audio_sbc_encoder_config_t));
        SbcConfiguration *sbcConfig =
           &codecConfig->config.get<CodecConfiguration::CodecSpecific::sbcConfig>();
        switch (sbcConfig->sampleRateHz) {
            case 48000:
                 sbc_codec.sampling_rate = 48000;
                 break;
            case 44100:
                 sbc_codec.sampling_rate = 44100;
                 break;
            default:
                 ALOGE("%s: SBC: unknown sampling rate:%d", __func__,
                       sbcConfig->sampleRateHz);
                 break;
        }

        switch (sbcConfig->channelMode) {
            case SbcChannelMode::STEREO:
                 sbc_codec.channels = A2D_SBC_CHANNEL_STEREO;
                 break;
            case SbcChannelMode::JOINT_STEREO:
                 sbc_codec.channels = A2D_SBC_CHANNEL_JOINT_STEREO;
                 break;
            case SbcChannelMode::DUAL:
                 sbc_codec.channels = A2D_SBC_CHANNEL_DUAL_MONO;
                 break;
            case SbcChannelMode::MONO:
                 sbc_codec.channels = A2D_SBC_CHANNEL_MONO;
                 break;
            default:
                 ALOGE("%s: SBC: unknown channel mode:%u", __func__,
                       (unsigned)sbcConfig->channelMode);
                 break;
        }

        switch (sbcConfig->blockLength) {
            case 16:
                sbc_codec.blk_len = 16;
                break;
            case 12:
                sbc_codec.blk_len = 12;
                break;
            case 8:
                sbc_codec.blk_len = 8;
                break;
            case 4:
                sbc_codec.blk_len = 4;
                break;
            default:
                ALOGE("%s: SBC: unknown block length:%u", __func__,
                      (unsigned int)sbcConfig->blockLength);
                break;
        }

        switch (sbcConfig->numSubbands) {
            case 8:
                sbc_codec.subband = 8;
                break;
            case 4:
                sbc_codec.subband = 4;
                break;
            default:
                ALOGE("%s: SBC: unknown subband:%u", __func__,
                      (unsigned int)sbcConfig->numSubbands);
                break;
        }
        switch (sbcConfig->allocMethod) {
            case SbcAllocMethod::ALLOC_MD_L:
                sbc_codec.alloc = 1;
                break;
            case SbcAllocMethod::ALLOC_MD_S:
                sbc_codec.alloc = 2;
                break;
            default:
                ALOGE("%s: SBC: unknown alloc method:%u", __func__,
                      (unsigned int)sbcConfig->allocMethod);
                break;
        }
        switch (sbcConfig->bitsPerSample) {
            case 16:
                sbc_codec.bits_per_sample = 16;
                break;
            case 24:
                sbc_codec.bits_per_sample = 24;
                break;
            case 32:
                sbc_codec.bits_per_sample = 32;
                break;
            default:
                ALOGE("%s: SBC: unknown bits per sample", __func__);
                break;
        }

        sbc_codec.min_bitpool = sbcConfig->minBitpool;
        sbc_codec.max_bitpool = sbcConfig->maxBitpool;
        ALOGD("%s: SBC: copied codec config: min_bitpool:%d max_bitpool:%d",
              __func__, sbc_codec.min_bitpool, sbc_codec.max_bitpool);

        if (codecConfig->encodedAudioBitrate == 0) {
            ALOGW("%s: SBC: bitrate is zero", __func__);
            sbc_codec.bitrate = 328000;
        } else if (codecConfig->encodedAudioBitrate >= 0x00000001 &&
                   codecConfig->encodedAudioBitrate <= 0x00FFFFFF) {
            sbc_codec.bitrate = codecConfig->encodedAudioBitrate;
        }
        ALOGI("%s: SBC: bitrate:%d i/p bitrate:%d", __func__, sbc_codec.bitrate,
              codecConfig->encodedAudioBitrate);
        *codec_type = AUDIO_FORMAT_SBC;

        if (sample_freq) *sample_freq = sbc_codec.sampling_rate;

        ALOGI("%s: SBC: done copying full codec config", __func__);
        return ((void *)(&sbc_codec));

    }else if (codecConfig->codecType == CodecType::AAC) {
        if (aac_codec.frame_ptr_ctl != NULL) {
          free(aac_codec.frame_ptr_ctl);
          aac_codec.frame_ptr_ctl = NULL;
        }
        if (aac_codec.abr_ptr_ctl != NULL) {
          ALOGE("%s: Free allocated memory: abr ctl", __func__);
          free(aac_codec.abr_ptr_ctl);
          aac_codec.abr_ptr_ctl = NULL;
        }
        AacConfiguration *aacConfig =
         &codecConfig->config.get<CodecConfiguration::CodecSpecific::aacConfig>();
        memset(&aac_codec, 0, sizeof(audio_aac_encoder_config_t));
        //USE 0 (AAC_LC) as hardcoded value till Audio
        //define constants
        aac_codec.enc_mode = 0;

        //USE LOAS(1) or LATM(4) hardcoded values till
        //Audio define proper constants
        aac_codec.format_flag = 4;
        switch (aacConfig->sampleRateHz) {
            case 44100:
                aac_codec.sampling_rate = 44100;
                break;
            case 48000:
                aac_codec.sampling_rate = 48000;
                break;
            case 88200:
                aac_codec.sampling_rate = 88200;
                break;
            case 96000:
                aac_codec.sampling_rate = 96000;
                break;
            default:
                ALOGE("%s: AAC: invalid sample rate:%d", __func__,
                      aacConfig->sampleRateHz);
                break;
        }
        switch (aacConfig->channelMode) {
            case ChannelMode::MONO:
                 aac_codec.channels = 1;
                 break;
            case ChannelMode::STEREO:
                 aac_codec.channels = 2;
                 break;
            default:
                 ALOGE("%s: AAC: unknown channel mode:%u", __func__,
                       (unsigned)aacConfig->channelMode);
                 break;
        }

        if(aacConfig->variableBitRateEnabled) {
            ALOGE("%s: AAC: Variable bit rate enabled", __func__);
            aac_codec.size_control_struct = A2D_AAC_VBR_SIZE_CTL_STRUCT;
            aac_codec.frame_ptr_ctl = (struct aac_frame_size_control_t*)malloc(aac_codec.size_control_struct*sizeof(struct aac_frame_size_control_t));
            if (aac_codec.frame_ptr_ctl != nullptr) {
              aac_codec.frame_ptr_ctl->ctl_type = A2D_AAC_VBR_SUPPORT;
              aac_codec.frame_ptr_ctl->ctl_value = A2D_AAC_VBR_ENABLE;
              ALOGI("%s: AAC: VBR ctl_type:%d VBR ctl_value:%d",
                     __func__, aac_codec.frame_ptr_ctl->ctl_type, aac_codec.frame_ptr_ctl->ctl_value);
            } else {
              ALOGE("%s: Memory allocation failed", __func__);
            }
        } else {
            ALOGE("%s: AAC: Variable bit rate disabled", __func__);
            aac_codec.size_control_struct = A2D_AAC_VBR_SIZE_CTL_STRUCT;
            aac_codec.frame_ptr_ctl = (struct aac_frame_size_control_t*)malloc(aac_codec.size_control_struct*sizeof(struct aac_frame_size_control_t));
            if (aac_codec.frame_ptr_ctl != nullptr) {
              aac_codec.frame_ptr_ctl->ctl_type = A2D_AAC_VBR_SUPPORT;
              aac_codec.frame_ptr_ctl->ctl_value = A2D_AAC_VBR_DISABLE;
              ALOGI("%s: AAC: VBR ctl_type:%d VBR ctl_value:%d",
                     __func__, aac_codec.frame_ptr_ctl->ctl_type, aac_codec.frame_ptr_ctl->ctl_value);
            } else {
              ALOGE("%s: Memory allocation failed", __func__);
            }
        }

        switch (aacConfig->bitsPerSample) {
            case 16:
                aac_codec.bits_per_sample = 16;
                break;
            case 24:
                aac_codec.bits_per_sample = 24;
                break;
            case 32:
                aac_codec.bits_per_sample = 32;
                break;
            default:
                ALOGE("%s: AAC: unknown bits per sample", __func__);
                break;
        }

        uint32_t bitrate_hal = 0;
        if (codecConfig->encodedAudioBitrate == 0) {
            ALOGW("%s: AAC: bitrate is zero", __func__);
        } else if (codecConfig->encodedAudioBitrate >= 0x00000001 &&
                   codecConfig->encodedAudioBitrate <= 0x00FFFFFF) {
            bitrate_hal = codecConfig->encodedAudioBitrate;
        }

        *codec_type = AUDIO_FORMAT_AAC;
        ALOGI("%s: AAC HAL: bitrate:%" PRIu32 " peermtu:%" PRIu16, __func__,
              bitrate_hal, codecConfig->peerMtu);
        uint16_t mtu = codecConfig->peerMtu;
        if ((mtu == 0) || (mtu > MAX_3MBPS_A2DP_STREAM_MTU))
            mtu = MAX_3MBPS_A2DP_STREAM_MTU;

        if(is_qti_system_supported()) {
          aac_codec.bitrate = bitrate_hal;
        } else {
          aac_codec.bitrate = (mtu - AAC_LATM_HEADER) *
                              (8 * aac_codec.sampling_rate/AAC_SAMPLE_SIZE);
          // Make sure bitrate is within the limit negotiated by stack
          if (aac_codec.bitrate > bitrate_hal)
              aac_codec.bitrate = bitrate_hal;
        }
        // Configure DSP for peak MTU
        aac_codec.frame_ctl.ctl_type = A2D_AAC_FRAME_PEAK_MTU;
        aac_codec.frame_ctl.ctl_value = mtu;
        ALOGI("%s: AAC HW: sampling_rate:%" PRIu32 " bitrate:%" PRIu32 " mtu:%d ctl_type:%d",
              __func__, aac_codec.sampling_rate, aac_codec.bitrate, mtu,
              aac_codec.frame_ctl.ctl_type);
        if (sample_freq) *sample_freq = aac_codec.sampling_rate;
        char prop_value[PROPERTY_VALUE_MAX] = "false";
        aac_codec.abr_size_control_struct = A2D_AAC_VBR_SIZE_CTL_STRUCT;
        aac_codec.abr_ptr_ctl = (struct aac_abr_control_t*)malloc(aac_codec.abr_size_control_struct*sizeof(struct aac_abr_control_t));
        if (aac_codec.abr_ptr_ctl != nullptr) {
           aac_codec.abr_ptr_ctl->is_abr_enabled = false;
           property_get("persist.vendor.qcom.bluetooth.aac_abr_support", prop_value, "false");
           ALOGE("%s: AAC ABR support : %s", __func__, prop_value);
           if (!strcmp(prop_value, "true")) {
              aac_codec.abr_ptr_ctl->is_abr_enabled = true;
           }
           aac_codec.abr_ptr_ctl->level_to_bitrate_map.num_levels = MAX_LEVELS;
           for (int i = 0; i < MAX_LEVELS; i++) {
              aac_codec.abr_ptr_ctl->level_to_bitrate_map.bit_rate_level_map[i].link_quality_level = i+1;
              uint32_t bit_rate = 2*(i+1)*bitrate_hal/10;
              if (bit_rate < A2D_AAC_MIN_BITRATE) {
                aac_codec.abr_ptr_ctl->level_to_bitrate_map.bit_rate_level_map[i].bitrate = A2D_AAC_MIN_BITRATE;
              } else if (bit_rate > A2D_AAC_MAX_BITRATE) {
                aac_codec.abr_ptr_ctl->level_to_bitrate_map.bit_rate_level_map[i].bitrate = A2D_AAC_MAX_BITRATE;
              } else {
                aac_codec.abr_ptr_ctl->level_to_bitrate_map.bit_rate_level_map[i].bitrate = bit_rate;
              }
              ALOGI("%s: AAC: level:%d bitrate:%d", __func__,
                 aac_codec.abr_ptr_ctl->level_to_bitrate_map.bit_rate_level_map[i].link_quality_level,
                 aac_codec.abr_ptr_ctl->level_to_bitrate_map.bit_rate_level_map[i].bitrate);
           }
        } else {
            ALOGE("%s: Memory allocation failed", __func__);
        }

        ALOGI("%s: AAC: done copying full codec config", __func__);
        return ((void *)(&aac_codec));

    } else if (codecConfig->codecType == CodecType::APTX ||
               codecConfig->codecType == CodecType::APTX_HD) {
        memset(&aptx_codec, 0, sizeof(audio_aptx_encoder_config_t));
        AptxConfiguration *aptxConfig =
           &codecConfig->config.get<CodecConfiguration::CodecSpecific::aptxConfig>();
        switch (aptxConfig->sampleRateHz) {
            case 48000:
                 aptx_codec.sampling_rate = 48000;
                 break;
            case 44100:
                 aptx_codec.sampling_rate = 44100;
                 break;
            default:
                 ALOGE("%s: APTX: unknown sampling rate:%d", __func__,
                       aptxConfig->sampleRateHz);
                 break;
        }
        switch (aptxConfig->channelMode) {
            case ChannelMode::STEREO:
                 aptx_codec.channels = 2;
                 break;
            case ChannelMode::MONO:
                 aptx_codec.channels = 1;
                 break;
            default:
                 ALOGE("%s: APTX: unknown channel mode:%u", __func__,
                       (unsigned)aptxConfig->channelMode);
                 break;
        }
        switch (aptxConfig->bitsPerSample) {
            case 16:
                aptx_codec.bits_per_sample = 16;
                break;
            case 24:
                aptx_codec.bits_per_sample = 24;
                break;
            case 32:
                aptx_codec.bits_per_sample = 32;
                break;
            default:
                ALOGE("%s: APTX: unknown bits per sample", __func__);
                break;
        }
        ALOGD("%s: APTX: codec config copied", __func__);
        if (codecConfig->encodedAudioBitrate == 0) {
            ALOGW("%s: APTX: bitrate is zero", __func__);
        } else if (codecConfig->encodedAudioBitrate >= 0x00000001 &&
                   codecConfig->encodedAudioBitrate <= 0x00FFFFFF) {
            aptx_codec.bitrate = codecConfig->encodedAudioBitrate;
        }

        if (codecConfig->codecType == CodecType::APTX) {
           *codec_type = AUDIO_FORMAT_APTX;
           ALOGI("%s: APTX: done copying full codec config", __func__);
        } else if (codecConfig->codecType == CodecType::APTX_HD) {
           *codec_type = AUDIO_FORMAT_APTX_HD;
           ALOGI("%s: APTXHD: done copying full codec config", __func__);
        }
        if (sample_freq) *sample_freq = aptx_codec.sampling_rate;
        return ((void *)&aptx_codec);

    } else if (codecConfig->codecType == CodecType::LDAC) {
        *codec_type = AUDIO_FORMAT_LDAC;
        ldac_codec_parser(codec_cfg);
        return ((void *)&ldac_codec);
    } else if (codecConfig->codecType == CodecType::APTX_ADAPTIVE) {
        *codec_type = static_cast<audio_format_t>(ENC_CODEC_TYPE_APTX_ADAPTIVE);
        memset(&aptx_adaptive_codec, 0, sizeof(audio_aptx_adaptive_encoder_config_t));
        AptxAdaptiveConfiguration *aptxAdaptiveConfig =
           &codecConfig->config.get<CodecConfiguration::CodecSpecific::aptxAdaptiveConfig>();

        switch (aptxAdaptiveConfig->sampleRateHz) {
            case 96000:
                 aptx_adaptive_codec.sampling_rate = 0x03;
                 break;
            case 48000:
                 aptx_adaptive_codec.sampling_rate = 0x01;
                 break;
            case 44100:
                 aptx_adaptive_codec.sampling_rate = 0x02;
                 break;
            default:
                 ALOGE("%s: APTX: unknown sampling rate:%d", __func__,
                       aptxAdaptiveConfig->sampleRateHz);
                 break;
        }

        switch (aptxAdaptiveConfig->channelMode) {
            case AptxAdaptiveChannelMode::MONO:
                 aptx_adaptive_codec.channel_mode = (int32_t)AptxAdaptiveChannelMode::MONO;
                 break;
            case AptxAdaptiveChannelMode::TWS_STEREO:
                 aptx_adaptive_codec.channel_mode = (int32_t)AptxAdaptiveChannelMode::TWS_STEREO;
                 break;
            case AptxAdaptiveChannelMode::JOINT_STEREO:
                 aptx_adaptive_codec.channel_mode = (int32_t)AptxAdaptiveChannelMode::JOINT_STEREO;
                 break;
            case AptxAdaptiveChannelMode::DUAL_MONO:
                 aptx_adaptive_codec.channel_mode = (int32_t)AptxAdaptiveChannelMode::DUAL_MONO;
                 break;
            default:
                 ALOGE("%s: APTX: unknown channel mode:%u", __func__,
                  aptxAdaptiveConfig->channelMode);
                 break;
        }

        switch (aptxAdaptiveConfig->bitsPerSample) {
            case 16:
                aptx_adaptive_codec.bits_per_sample = 16;
                break;
            case 24:
                aptx_adaptive_codec.bits_per_sample = 24;
                break;
            case 32:
                aptx_adaptive_codec.bits_per_sample = 32;
                break;
            default:
                ALOGE("%s: APTX: unknown bits per sample", __func__);
                break;
        }

        switch (aptxAdaptiveConfig->aptxMode) {
            case AptxMode::HIGH_QUALITY:
                aptx_adaptive_codec.aptx_mode = 0x1000;
                break;
            case AptxMode::LOW_LATENCY:
                aptx_adaptive_codec.aptx_mode = 0x2000;
                break;
            case AptxMode::ULTRA_LOW_LATENCY:
                aptx_adaptive_codec.aptx_mode = 0x4000;
                break;
            default:
                ALOGE("%s: APTX-Adaptive: unknown Aptx-AD mode", __func__);
                break;
        }

        /*Set default Input Mode*/
        aptx_adaptive_codec.input_mode = (uint32_t)AptxAdaptiveInputMode::STEREO;

        aptx_adaptive_codec.min_sink_buffering_LL = aptxAdaptiveConfig->sinkBufferingMs.minLowLatency;
        aptx_adaptive_codec.max_sink_buffering_LL = aptxAdaptiveConfig->sinkBufferingMs.maxLowLatency;
        aptx_adaptive_codec.min_sink_buffering_HQ = aptxAdaptiveConfig->sinkBufferingMs.minHighQuality;
        aptx_adaptive_codec.max_sink_buffering_HQ = aptxAdaptiveConfig->sinkBufferingMs.maxHighQuality;
        aptx_adaptive_codec.min_sink_buffering_TWS = aptxAdaptiveConfig->sinkBufferingMs.minTws;
        aptx_adaptive_codec.max_sink_buffering_TWS = aptxAdaptiveConfig->sinkBufferingMs.maxTws;

        char adaptive_prop_value[PROPERTY_VALUE_MAX] = "false";
        property_get("persist.vendor.qcom.bluetooth.aptxadaptiver2_1_support", adaptive_prop_value, "false");
        ALOGE("%s: APTX-Adaptive: R2.1 supported: %s", __func__, adaptive_prop_value);
        if (!strcmp(adaptive_prop_value, "true")) {
          aptx_adaptive_codec.max_sink_buffering_LL = 2*aptxAdaptiveConfig->sinkBufferingMs.maxLowLatency;
          aptx_adaptive_codec.max_sink_buffering_HQ = 2*aptxAdaptiveConfig->sinkBufferingMs.maxHighQuality;
          aptx_adaptive_codec.max_sink_buffering_TWS = 2*aptxAdaptiveConfig->sinkBufferingMs.maxTws;
        }
        uint16_t mtu = codecConfig->peerMtu;
        aptx_adaptive_codec.mtu = mtu;

        aptx_adaptive_codec.TTP_LL_low = aptxAdaptiveConfig->ttp.lowLowLatency + 128;
        aptx_adaptive_codec.TTP_LL_high = aptxAdaptiveConfig->ttp.highLowLatency + 128;
        aptx_adaptive_codec.TTP_HQ_low = aptxAdaptiveConfig->ttp.lowHighQuality + 128;
        aptx_adaptive_codec.TTP_HQ_high = aptxAdaptiveConfig->ttp.highHighQuality + 128;
        aptx_adaptive_codec.TTP_TWS_low = aptxAdaptiveConfig->ttp.lowTws + 128;
        aptx_adaptive_codec.TTP_TWS_high = aptxAdaptiveConfig->ttp.highTws + 128;
        aptx_adaptive_codec.input_fade_duration =
                               aptxAdaptiveConfig->inputFadeDurationMs;
        for (int i = 0; i < sizeof(aptx_adaptive_codec.sink_capabilities); i ++) {
          aptx_adaptive_codec.sink_capabilities[i] =
                               aptxAdaptiveConfig->aptxAdaptiveConfigStream[i];
          if (!strcmp(adaptive_prop_value, "true")) {
            if (i == 0) {
              aptx_adaptive_codec.aptx_mode = aptx_adaptive_codec.aptx_mode | (aptx_adaptive_codec.sink_capabilities[i] << 8);
            } else if (i == 1) {
              aptx_adaptive_codec.aptx_mode = aptx_adaptive_codec.aptx_mode | aptx_adaptive_codec.sink_capabilities[i];
            }
          }
          ALOGW("%s: ## aptXAdaptive ## SinkCaps[%d] =  %d ",
                               __func__, i, aptx_adaptive_codec.sink_capabilities[i]);
        }

        ALOGW("%s: ## aptXAdaptive ## sampleRate 0x%x", __func__, aptx_adaptive_codec.sampling_rate);
        ALOGW("%s: ## aptXAdaptive ## channelMode 0x%x", __func__, aptx_adaptive_codec.channel_mode);
        ALOGW("%s: ## aptXAdaptive ## min_sink_buffering_LL 0x%x", __func__, aptx_adaptive_codec.min_sink_buffering_LL);
        ALOGW("%s: ## aptXAdaptive ## max_sink_buffering_LL 0x%x", __func__, aptx_adaptive_codec.max_sink_buffering_LL);
        ALOGW("%s: ## aptXAdaptive ## min_sink_buffering_HQ 0x%x", __func__, aptx_adaptive_codec.min_sink_buffering_HQ);
        ALOGW("%s: ## aptXAdaptive ## max_sink_buffering_HQ 0x%x", __func__, aptx_adaptive_codec.max_sink_buffering_HQ);
        ALOGW("%s: ## aptXAdaptive ## min_sink_buffering_TWS 0x%x", __func__, aptx_adaptive_codec.min_sink_buffering_TWS);
        ALOGW("%s: ## aptXAdaptive ## max_sink_buffering_TWS 0x%x", __func__, aptx_adaptive_codec.max_sink_buffering_TWS);
        ALOGW("%s: ## aptXAdaptive ## ttp_ll_0 0x%x", __func__, aptx_adaptive_codec.TTP_LL_low);
        ALOGW("%s: ## aptXAdaptive ## ttp_ll_1 0x%x", __func__, aptx_adaptive_codec.TTP_LL_high);
        ALOGW("%s: ## aptXAdaptive ## ttp_hq_0 0x%x", __func__, aptx_adaptive_codec.TTP_HQ_low);
        ALOGW("%s: ## aptXAdaptive ## ttp_hq_1 0x%x", __func__, aptx_adaptive_codec.TTP_HQ_high);
        ALOGW("%s: ## aptXAdaptive ## ttp_tws_0 0x%x", __func__, aptx_adaptive_codec.TTP_TWS_low);
        ALOGW("%s: ## aptXAdaptive ## ttp_tws_1 0x%x", __func__, aptx_adaptive_codec.TTP_TWS_high);
        ALOGW("%s: ## aptXAdaptive ## MTU =  %d", __func__, aptx_adaptive_codec.mtu);
        ALOGW("%s: ## aptXAdaptive ## Bits Per Sample =  %d", __func__, aptx_adaptive_codec.bits_per_sample);
        ALOGW("%s: ## aptXAdaptive ## Mode =  %d", __func__, aptx_adaptive_codec.aptx_mode);
        ALOGW("%s: ## aptXAdaptive ## Input mode =  %d", __func__, aptx_adaptive_codec.input_mode);
        ALOGW("%s: ## aptXAdaptive ## input_fade_duriation =  %d",
                                                __func__, aptx_adaptive_codec.input_fade_duration);

        return ((void *)&aptx_adaptive_codec);
    }
    return NULL;
}

/*****************************************************************************
**
** AUDIO DATA PATH
**
*****************************************************************************/

tSESSION_TYPE get_session_type(const uint16_t& cntrl_key) {
    for (int i = 0; i < MAX_SESSION_TYPE; i++) {
        if (audio_stream[i].ctrl_key == cntrl_key)
            return (tSESSION_TYPE)i;
    }
    return UNKNOWN;
}

void register_reconfig_cb(int  (*reconfig_cb)(tSESSION_TYPE session_type, int status)) {
    ALOGI("%s:", __func__);
    reconfig_to_audio_cb = reconfig_cb;
}
void unregister_reconfig_cb(int (*reconfig_cb)(tSESSION_TYPE session_type, int status)) {
    ALOGI("%s:", __func__);
    reconfig_to_audio_cb = NULL;
}

void stack_resp_cb (const uint16_t& cntrl_key, const bool& start_resp,
                     const BluetoothAudioStatus&  status) {

    ALOGI("%s: status = %hhu", __func__, status);
    tSESSION_TYPE session_type = get_session_type(cntrl_key);
    if (session_type == UNKNOWN) {
        ALOGE("%s: invalid cntrl key = %d", __func__, cntrl_key);
        return;
    }
    pthread_mutex_lock(&audio_stream[session_type].ack_lock);
    audio_stream[session_type].ack_status = aidl_to_audio_status(status);
    if (!audio_stream[session_type].ack_recvd) {
        audio_stream[session_type].ack_recvd = 1;
        pthread_cond_signal(&audio_stream[session_type].ack_cond);
    }
    if ((audio_stream[session_type].reconfig_pending == true) &&
        (audio_stream[session_type].ack_status == CTRL_ACK_SUCCESS)) {
        // streamSuspended(SUCCESS) can also trigger resume
        audio_stream[session_type].reconfig_pending = false;
        session_reconfig_t q = {.session = session_type, .status = 1};
        std::unique_lock<std::mutex> lk(mtx_config_changed);
        reconfig_queue.push_back(q);
        ALOGI("%s: calling reconfig_to_audio_cb = 1", __func__);
        // Invoke callback
        cv_config_changed.notify_all();
    } else if (audio_stream[session_type].ack_status == CTRL_ACK_RECONFIGURATION) {
        audio_stream[session_type].reconfig_pending = true;
        audio_stream[session_type].state = AUDIO_STATE_SUSPENDED;
        std::unique_lock<std::mutex> lk(mtx_config_changed);
        session_reconfig_t q = {.session = session_type, .status = 0};
        reconfig_queue.push_back(q);
        ALOGI("%s: calling reconfig_to_audio_cb = 0", __func__);
        // Invoke callback
        cv_config_changed.notify_all();
    }
    pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
    ALOGI("%s: exit = %hhu", __func__, status);
}

void session_resp_cb (const uint16_t& cntrl_key)
{
    ALOGI("%s", __func__);
    tSESSION_TYPE session_type = get_session_type(cntrl_key);
    if (session_type == UNKNOWN) {
        ALOGE("%s: invalid cntrl key = %d", __func__, cntrl_key);
        return;
    }
    pthread_mutex_lock(&audio_stream[session_type].ack_lock);
    ALOGI("%s: session ready  = %d", __func__, audio_stream[session_type].session_ready);
    if (audio_stream[session_type].session_ready ||
        audio_stream[session_type].state == AUDIO_STATE_STARTED ||
        audio_stream[session_type].state == AUDIO_STATE_STARTING) {
        audio_stream[session_type].state = AUDIO_STATE_STANDBY;
        audio_stream[session_type].session_ready = 0;
        audio_stream[session_type].ack_status = CTRL_ACK_UNKNOWN;
        audio_stream[session_type].reconfig_pending = false;
        ALOGI("%s:  end session session ready  = %d", __func__, audio_stream[session_type].session_ready);
        btapoffload_port_deinit(session_type, false);
    } else if ((!audio_stream[session_type].session_ready)) {
        audio_stream[session_type].session_ready = 1;
        audio_stream[session_type].state = AUDIO_STATE_STANDBY;
        //memset(&audio_stream[session_type].codec_cfg, 0, sizeof(AudioConfiguration));
        SessionType session = audio_stream[session_type].sessionType;
        if (session ==
            SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::a2dpConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::a2dpConfig>());
        } else if(session ==
                  SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioConfig>());
        }  else if(session ==
                   SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
            audio_stream[session_type].dec_codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioConfig>());
        }  else if(session ==
                   SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioBroadcastConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioBroadcastConfig>());
        }
    }
    ALOGD("%s: state = %s", __func__, dump_a2dp_hal_state(audio_stream[session_type].state));
    if (!audio_stream[session_type].ack_recvd) {
       audio_stream[session_type].ack_recvd = 1;
       pthread_cond_signal(&audio_stream[session_type].ack_cond);
    }
    pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
}

void audio_configuration_changed_cb (const uint16_t& cntrl_key, const AudioConfiguration& audio_config)
{
    ALOGI("%s", __func__);
    tSESSION_TYPE session_type = get_session_type(cntrl_key);
    if (session_type == UNKNOWN) {
        ALOGE("%s: invalid cntrl key = %d", __func__, cntrl_key);
        return;
    }
    pthread_mutex_lock(&audio_stream[session_type].ack_lock);
    if (audio_stream[session_type].reconfig_pending == false) {
        // Stream has not started yet, copy audio config and wait for start stream from audio hal
        //memset(&audio_stream[session_type].codec_cfg, 0, sizeof(AudioConfiguration));
        SessionType session = audio_stream[session_type].sessionType;
        if (session ==
            SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::a2dpConfig>
            (audio_config.get<AudioConfiguration::a2dpConfig>());
        } else if(session ==
                  SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (audio_config.get<AudioConfiguration::leAudioConfig>());
            uint8_t channel_count = 0;
            auto leaudio_config = audio_config.get<AudioConfiguration::leAudioConfig>();
            std::unique_lock<std::mutex> lk(mtx_config_changed);
            for (const auto& map : leaudio_config.streamMap) {
              if (map.isStreamActive)
                channel_count++;
            }
            ALOGI("%s: channel count = %d", __func__, channel_count);
            if (channel_count == 1) {
              session_reconfig_t q = {.session = session_type, .status = 2};
              reconfig_queue.push_back(q);
              cv_config_changed.notify_all();
            } else if (channel_count > 1) {
              session_reconfig_t q = {.session = session_type, .status = 3};
              reconfig_queue.push_back(q);
              cv_config_changed.notify_all();
            }

        }  else if(session ==
                   SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
            audio_stream[session_type].dec_codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (audio_config.get<AudioConfiguration::leAudioConfig>());
        }  else if(session ==
                   SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioBroadcastConfig>
            (audio_config.get<AudioConfiguration::leAudioBroadcastConfig>());
        }
    } else {
        // Active stream getting suspended for reconfiguration. Process it in different thread context
        {
            std::unique_lock<std::mutex> lk(mtx_config_changed);
            ALOGI("%s: session type = %d", __func__, session_type);
            session_reconfig_t q = {.session = session_type, .status = 1};
            reconfig_queue.push_back(q);
            audio_stream[session_type].reconfig_pending = false;
            if (session_type == LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
              audio_stream[session_type].state = AUDIO_STATE_STANDBY;
            }
            cv_config_changed.notify_all();
        }
    }
    pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
}

static void btapoffload_port_init(tSESSION_TYPE session_type)
{
    ALOGI("%s: session type = %d", __func__, audio_stream[session_type].sessionType);
    if (BluetoothAudioSessionControl::IsSessionReady(audio_stream[session_type].sessionType)) {
        audio_stream[session_type].state = AUDIO_STATE_STANDBY;
        audio_stream[session_type].session_ready = 1;
        //memset(&audio_stream[session_type].codec_cfg, 0, sizeof(AudioConfiguration));
        SessionType session = audio_stream[session_type].sessionType;
        if (session ==
            SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::a2dpConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::a2dpConfig>());
        } else if(session ==
                  SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioConfig>());
        }  else if(session ==
                   SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
            audio_stream[session_type].dec_codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioConfig>());
        }  else if(session ==
                   SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioBroadcastConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioBroadcastConfig>());
        }
    } else
        ALOGE("%s, bluetooth provider session is not avail", __func__);

    audio_stream[session_type].control_result_cb = stack_resp_cb;
    audio_stream[session_type].session_changed_cb = session_resp_cb;
    audio_stream[session_type].audio_configuration_changed_cb = audio_configuration_changed_cb;

    ::aidl::android::hardware::bluetooth::audio::PortStatusCallbacks cbacks = {
      .control_result_cb_ = audio_stream[session_type].control_result_cb,
      .session_changed_cb_ = audio_stream[session_type].session_changed_cb,
      .audio_configuration_changed_cb_ = audio_stream[session_type].audio_configuration_changed_cb};
      audio_stream[session_type].ctrl_key = BluetoothAudioSessionControl::RegisterControlResultCback(
         audio_stream[session_type].sessionType, cbacks);
    ALOGI("%s: ctrl_key = %d", __func__, audio_stream[session_type].ctrl_key);
}

static void btapoffload_port_deinit(tSESSION_TYPE session_type, bool unreg) {
   ALOGI("%s session_type = %d, ctrl_key = %d", __func__, audio_stream[session_type].sessionType, audio_stream[session_type].ctrl_key);
   if (unreg)
     BluetoothAudioSessionControl::UnregisterControlResultCback(audio_stream[session_type].sessionType,audio_stream[session_type].ctrl_key);
  audio_stream[session_type].ctrl_key = 0;
}
void a2dp_stream_common_init(struct a2dp_stream_common *common, tSESSION_TYPE session_type)
{
    ALOGI("%s: session_type=%d", __func__, session_type);


    common->state = AUDIO_STATE_STANDBY;
    common->ack_status = CTRL_ACK_UNKNOWN;
    common->ack_cond = PTHREAD_COND_INITIALIZER;
    common->reconfig_pending = false;
    common->multicast = 0;
    common->num_conn_dev = 0;
    common->codec_cfg.set<AudioConfiguration::pcmConfig>(PcmConfiguration());
    common->dec_codec_cfg.set<AudioConfiguration::pcmConfig>(PcmConfiguration());
    common->session_ready = 0;
    common->sink_latency = A2DP_DEFAULT_SINK_LATENCY;
    common->ctrl_key = 0;
    common->sessionType = (SessionType)session_type;
    common->control_result_cb = NULL;
    common->session_changed_cb = NULL;
    common->audio_configuration_changed_cb = NULL;
    btapoffload_port_init(session_type);

}

int wait_for_stack_response(int time_to_wait, tSESSION_TYPE session_type)
{
    ALOGD("%s", __func__);
    struct timespec now;
    int ret = 0;
    pthread_mutex_lock(&audio_stream[session_type].ack_lock);
    if (audio_stream[session_type].session_ready== 0) {
        ALOGE("%s: stack deinitialized", __func__);
        pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
        return ret;
    }
    ALOGW("%s: entering conditional wait: ack_recvd = %d",
              __func__,audio_stream[session_type].ack_recvd);
    clock_gettime(CLOCK_REALTIME, &now);
    now.tv_sec += (time_to_wait / 1000);
    if (!audio_stream[session_type].ack_recvd) {
       pthread_cond_timedwait(&audio_stream[session_type].ack_cond,
                              &audio_stream[session_type].ack_lock, &now);
    }
    if (audio_stream[session_type].ack_recvd) {
        ALOGV("%s: ack received", __func__);
    }
    else {
        clock_gettime(CLOCK_REALTIME, &now);
        now.tv_nsec += ((time_to_wait % 1000) * 1000000);
        //ALOGW("%s: entering conditional wait for ms = %l",
          //      __func__,now.tv_nsec);
        pthread_cond_timedwait(&audio_stream[session_type].ack_cond,
                               &audio_stream[session_type].ack_lock, &now);
    }
    pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
    return ret;
}

int audio_start_stream()
{
  return audio_stream_start();
}

int audio_start_stream_api(tSESSION_TYPE session_type)
{
  return audio_stream_start(session_type);
}

int audio_stream_start() {
    return audio_stream_start(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}

int audio_stream_start(tSESSION_TYPE session_type)
{
    int retry_count = 0;
    int wait_time = 0;

    if(qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_stream_start_qti();
    if(qti_audio_hal) return vendor::qti::btoffload::audio_stream_start_qti();
    int ack_ret;
    tCTRL_ACK status = CTRL_ACK_SUCCESS;
    ALOGI("%s: session type: %d", __func__, session_type);
    std::unique_lock<std::mutex> lock(api_lock);
    if (!audio_stream[session_type].session_ready &&
      BluetoothAudioSessionControl::IsSessionReady(audio_stream[session_type].sessionType)) {
      ALOGD("%s: session state not updated, calling port init",__func__);
      audio_stream_open_api(session_type);
    }
    if (audio_stream[session_type].session_ready) {
       ALOGD("%s: state = %s", __func__,
              dump_a2dp_hal_state(audio_stream[session_type].state));
       if (audio_stream[session_type].state == AUDIO_STATE_SUSPENDED) {
           ALOGW("%s: stream suspended", __func__);
           lock.unlock();
           return -1;
       } else if(audio_stream[session_type].state == AUDIO_STATE_STARTED) {
           ALOGW("%s: stream already started", __func__);
           lock.unlock();
           return CTRL_ACK_SUCCESS;
       }
       audio_stream[session_type].ack_status = CTRL_ACK_UNKNOWN;
       ALOGE("%s: issues Start stream to stack",__func__);
       audio_stream[session_type].ack_recvd = 0;
       auto ret = BluetoothAudioSessionControl::StartStream(audio_stream[session_type].sessionType);
       if (!ret) {
          ALOGE("%s: client has died",__func__);
          goto end;
       }
       audio_stream[session_type].state = AUDIO_STATE_STARTING;
       pthread_mutex_lock(&audio_stream[session_type].ack_lock);
       status = audio_stream[session_type].ack_status;
       pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
       if (status == CTRL_ACK_UNKNOWN) {
          ack_ret = wait_for_stack_response(max_waittime, session_type);
          pthread_mutex_lock(&audio_stream[session_type].ack_lock);
          status = audio_stream[session_type].ack_status;
          pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
          ALOGD("%s: status = %s", __func__, dump_ctrl_ack(status));
       }
       if (status == CTRL_ACK_SUCCESS) {
          ALOGI("%s: a2dp stream started successfully", __func__);
          audio_stream[session_type].state = AUDIO_STATE_STARTED;
          goto end;
       } else if(status == CTRL_ACK_FAILURE) {
          ALOGW("%s: a2dp stream start failed: status = %s", __func__,
                  dump_ctrl_ack(status));
          audio_stream[session_type].state = AUDIO_STATE_STANDBY;
          retry_count = FAIL_TRIAL_COUNT;
          wait_time = 200;
       } else if(status == CTRL_ACK_INCALL_FAILURE) {
          ALOGW("%s: a2dp stream start failed: status = %s", __func__,
                  dump_ctrl_ack(status));
          audio_stream[session_type].state = AUDIO_STATE_STANDBY;
          retry_count = IN_CALL_TRIAL_COUNT;
          wait_time = 200;
       } else if(status == CTRL_ACK_UNSUPPORTED ||
             status == CTRL_ACK_DISCONNECT_IN_PROGRESS ||
             status == CTRL_ACK_UNKNOWN) {
          ALOGW("%s: a2dp stream start failed: status = %s", __func__,
                  dump_ctrl_ack(status));
          audio_stream[session_type].state = AUDIO_STATE_STANDBY;
          goto end;
       } else if(status == CTRL_ACK_RECONFIGURATION) {
          ALOGW("%s: a2dp stream start failed: status = %s", __func__,
                  dump_ctrl_ack(status));
          audio_stream[session_type].state = AUDIO_STATE_SUSPENDED;
          goto end;
       }

       if (session_type == A2DP_SOFTWARE_ENCODING_DATAPATH
            || session_type == A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH)
       {
         for (int i = 0; i < retry_count; i++) {
           ALOGI("%s: wait for %dmsec", __func__, wait_time);
           usleep(wait_time *1000);
           audio_stream[session_type].ack_status = CTRL_ACK_UNKNOWN;
           ALOGE("%s: Retry Start stream",__func__);
           audio_stream[session_type].ack_recvd = 0;
           auto ret = BluetoothAudioSessionControl::StartStream(audio_stream[session_type].sessionType);
           if (!ret) {
             ALOGE("%s: client has died",__func__);
             goto end;
           }
           audio_stream[session_type].state = AUDIO_STATE_STARTING;
           pthread_mutex_lock(&audio_stream[session_type].ack_lock);
           status = audio_stream[session_type].ack_status;
           pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
           if (status == CTRL_ACK_UNKNOWN) {
             ack_ret = wait_for_stack_response(max_waittime, session_type);
             pthread_mutex_lock(&audio_stream[session_type].ack_lock);
             status = audio_stream[session_type].ack_status;
             pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
             ALOGD("%s: status = %s", __func__, dump_ctrl_ack(status));
           }
           if (status == CTRL_ACK_SUCCESS) {
             ALOGI("%s: a2dp stream started successfully", __func__);
             audio_stream[session_type].state = AUDIO_STATE_STARTED;
             goto end;
           } else if(status == CTRL_ACK_FAILURE ||
                     status == CTRL_ACK_INCALL_FAILURE) {
             //wait for 200msec before retry for A2DP_CTRL_ACK_FAILURE
             //and A2DP_CTRL_ACK_INCALL_FAILURE
             ALOGI("%s:status = %s", __func__, dump_ctrl_ack(status));
           } else if(status == CTRL_ACK_UNSUPPORTED ||
                status == CTRL_ACK_DISCONNECT_IN_PROGRESS ||
                status == CTRL_ACK_UNKNOWN) {
             ALOGW("%s: a2dp stream start failed: status = %s", __func__,
                     dump_ctrl_ack(status));
             audio_stream[session_type].state = AUDIO_STATE_STANDBY;
             goto end;
           } else if(status == CTRL_ACK_RECONFIGURATION) {
             ALOGW("%s: a2dp stream start failed: status = %s", __func__,
                     dump_ctrl_ack(status));
             audio_stream[session_type].state = AUDIO_STATE_SUSPENDED;
             goto end;
           }
         }
       }
    } else {
        ALOGW("%s: session is not active", __func__);
        lock.unlock();
        return -1;
    }
end:
    lock.unlock();
    return status;
}

int audio_stream_open_api(tSESSION_TYPE session_type) {
    ALOGI("%s: session type: %d", __func__, session_type);
    if (audio_stream[session_type].ctrl_key != 0 &&
      audio_stream[session_type].session_ready) {
      ALOGW("%s: session is opened already", __func__);
      return 0;
    }
    return audio_stream_open(session_type);
}

int audio_stream_open() {
    return audio_stream_open(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}

int audio_stream_open(tSESSION_TYPE session_type)
{
    ALOGI("%s trying for AIDL sessions first: session %d",
                                 __func__, session_type);
    a2dp_stream_common_init(&audio_stream[session_type], session_type);
    for (int i = 0; i < STREAM_OPEN_RETRIAL_COUNT; i++) {
      audio_stream[session_type].session_ready =
          BluetoothAudioSessionControl::IsSessionReady(audio_stream[session_type].sessionType);
      if (audio_stream[session_type].session_ready) {
        ALOGI("%s: success", __func__);
        aidl_client_enabled = true;
        qti_audio_hal = false;
        return 0;
      } else {
        ALOGI("%s: wait for 100msec, and retry",__func__);
        usleep(100 *1000);
      }
    }
    if(aidl_client_enabled == false) {
      if (session_type != LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
        ALOGI("%s trying for QC sessions now", __func__);
        int vnd_session = vendor::qti::btoffload::V2_1::audio_stream_open_qti();
        if (vnd_session == 1) {
            qti_audio_hal_2_1 = true;
            return 0;
        }
        if (vnd_session != -1) {//-1 if software path is active
            vnd_session = vendor::qti::btoffload::audio_stream_open_qti();
            if(vnd_session == 1) {
               qti_audio_hal = true;
               return 0;
            }
        }
      } else {
        ALOGI("%s ignore trying for QC sessions if it is BLE OFFLOAD decoding session", __func__);
        return 0;
      }
    }
    btapoffload_port_deinit(session_type, true);
    ALOGE("%s: failed", __func__);
    return -1;
}

int audio_stream_close_api(tSESSION_TYPE session_type) {
    return audio_stream_close(session_type);
}

int audio_stream_close() {
    return audio_stream_close(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}

int audio_stream_close(tSESSION_TYPE session_type)
{
    if(qti_audio_hal_2_1) {
      if (session_type != LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
        ALOGI("%s close for QC sessions now", __func__);
        return vendor::qti::btoffload::V2_1::audio_stream_close_qti();
      } else {
        ALOGI("%s ignore close QC sessions if it is BLE OFFLOAD decoding session", __func__);
        return 1;
      }
    }
    if(qti_audio_hal) {
      if (session_type != LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
        ALOGI("%s close for QC sessions now", __func__);
        return vendor::qti::btoffload::audio_stream_close_qti();;
      } else {
        ALOGI("%s ignore close QC sessions if it is BLE OFFLOAD decoding session", __func__);
        return 1;
      }
    }
    ALOGI("%s: session type: %d", __func__, session_type);
    tCTRL_ACK status = CTRL_ACK_UNKNOWN;
    std::unique_lock<std::mutex> lock(api_lock);
    free(aac_codec.frame_ptr_ctl);
    aac_codec.frame_ptr_ctl = NULL;
    if (aac_codec.abr_ptr_ctl != NULL) {
       free(aac_codec.abr_ptr_ctl);
       aac_codec.abr_ptr_ctl = NULL;
    }

    if (audio_stream[session_type].state == AUDIO_STATE_STARTED ||
        audio_stream[session_type].state == AUDIO_STATE_STOPPING) {
        audio_stream[session_type].state = AUDIO_STATE_STOPPED;
        if (audio_stream[session_type].session_ready) {
            audio_stream[session_type].ack_status = CTRL_ACK_UNKNOWN;
            audio_stream[session_type].ack_recvd = 0;
            ALOGW("%s: suspending audio stream", __func__);
            auto ret = BluetoothAudioSessionControl::SuspendStream(audio_stream[session_type].sessionType);
            if (ret == false) {
                ALOGE("%s: client has died",__func__);
                lock.unlock();
                return -1;
            }
            pthread_mutex_lock(&audio_stream[session_type].ack_lock);
            status = audio_stream[session_type].ack_status;
            pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
            ALOGI("%s: ack status = %s", __func__, dump_ctrl_ack(status));
            if (status == CTRL_ACK_UNKNOWN) {
                wait_for_stack_response(max_waittime, session_type);
                pthread_mutex_lock(&audio_stream[session_type].ack_lock);
                status = audio_stream[session_type].ack_status;
                pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
            }
        }
    }
    btapoffload_port_deinit(session_type, true);
    lock.unlock();
    if (status == CTRL_ACK_UNKNOWN) {
        ALOGE("%s: failed to get ack from stack", __func__);
        return -1;
    } else {
      ALOGI("%s: return success", __func__);
      return 1;
    }
}

int audio_stop_stream()
{
  return audio_stream_stop();
}

int audio_stop_stream_api(tSESSION_TYPE session_type) {
    return audio_stream_stop(session_type);
}

int audio_stream_stop() {
    return audio_stream_stop(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}

int audio_stream_stop(tSESSION_TYPE session_type)
{
    if (qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_stream_stop_qti();
    if(qti_audio_hal) return vendor::qti::btoffload::audio_stream_stop_qti();
    ALOGI("%s: session type: %d", __func__, session_type);
    int ret = 0;
    tCTRL_ACK status;

    if (audio_stream[session_type].state == AUDIO_STATE_SUSPENDED ||
        audio_stream[session_type].state == AUDIO_STATE_STANDBY ||
        audio_stream[session_type].state == AUDIO_STATE_STOPPED) {
        ALOGD("%s: stream in standby/suspended already",__func__);
        return 0;
    }
    std::unique_lock<std::mutex> lock(api_lock);
    free(aac_codec.frame_ptr_ctl);
    aac_codec.frame_ptr_ctl = NULL;
    if (aac_codec.abr_ptr_ctl != NULL) {
      free(aac_codec.abr_ptr_ctl);
      aac_codec.abr_ptr_ctl = NULL;
    }

    //TODO: Reset sesion type
/*
    if (audio_stream.sessionType != get_session_type()) {
        ALOGI("%s: Reset session registeration on SHO", __func__);
        btapoffload_port_deinit();
        audio_stream.sessionType = get_session_type();
        btapoffload_port_init();
    }
*/

    if (audio_stream[session_type].session_ready) {
        ALOGD("%s: state = %s", __func__,
              dump_a2dp_hal_state(audio_stream[session_type].state));
        if (audio_stream[session_type].state != AUDIO_STATE_SUSPENDED &&
            audio_stream[session_type].state != AUDIO_STATE_STANDBY) {
            audio_stream[session_type].ack_recvd = 0;
            audio_stream[session_type].ack_status = CTRL_ACK_UNKNOWN;
            auto ret = BluetoothAudioSessionControl::SuspendStream(audio_stream[session_type].sessionType);
            if (ret == false) {
                ALOGE("%s: client has died",__func__);
                lock.unlock();
                return -1;
            }
            pthread_mutex_lock(&audio_stream[session_type].ack_lock);
            status = audio_stream[session_type].ack_status;
            pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
            ALOGI("%s: ack status = %s", __func__, dump_ctrl_ack(status));
            if (status == CTRL_ACK_UNKNOWN) {
                audio_stream[session_type].state = AUDIO_STATE_STOPPING;
                wait_for_stack_response(max_waittime, session_type);
                pthread_mutex_lock(&audio_stream[session_type].ack_lock);
                status = audio_stream[session_type].ack_status;
                pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
            }
            if (status == CTRL_ACK_SUCCESS) {
                ALOGD("%s: success", __func__);
                audio_stream[session_type].state = AUDIO_STATE_STANDBY;
                lock.unlock();
                return 0;
            } else {
                ALOGW("%s: failed", __func__);
                audio_stream[session_type].state = AUDIO_STATE_STOPPED;
                lock.unlock();
                return -1;
            }
        } else {
          ALOGD("%s: state is already stopped or in standby",__func__);
          lock.unlock();
          return 0;
        }
    } else {
       ret = -1;
       ALOGW("%s: session is not ready", __func__);
    }
    audio_stream[session_type].state = AUDIO_STATE_STOPPED;
    lock.unlock();
    return ret;
}

int audio_suspend_stream()
{
  return audio_stream_suspend();
}

int audio_suspend_stream_api(tSESSION_TYPE session_type) {
  return audio_stream_suspend(session_type);
}

int audio_stream_suspend() {
    return audio_stream_suspend(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}

int audio_stream_suspend(tSESSION_TYPE session_type)
{
    if(qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_stream_suspend_qti();
    if(qti_audio_hal) return vendor::qti::btoffload::audio_stream_suspend_qti();
    ALOGI("%s: session type: %d", __func__, session_type);
    tCTRL_ACK status;

    ALOGD("%s: state = %s", __func__,
          dump_a2dp_hal_state(audio_stream[session_type].state));
    if (audio_stream[session_type].state == AUDIO_STATE_SUSPENDED) {
      return 0;
    }
    if ((audio_stream[session_type].state == AUDIO_STATE_STOPPED ||
        audio_stream[session_type].state == AUDIO_STATE_STANDBY)
        && session_type != A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
      return 0;
    }

    std::unique_lock<std::mutex> lock(api_lock);
    if (audio_stream[session_type].session_ready) {
        bool need_suspend = true;
        if (audio_stream[session_type].state == AUDIO_STATE_SUSPENDED) {
          need_suspend = false;
        }
        if ((audio_stream[session_type].state == AUDIO_STATE_STOPPED ||
            audio_stream[session_type].state == AUDIO_STATE_STANDBY)
            && session_type != A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
          need_suspend = false;
        }
        if (!need_suspend) {
          ALOGD("%s: state = %s, not need suspend", __func__,
              dump_a2dp_hal_state(audio_stream[session_type].state));
          return 0;
        }

        audio_stream[session_type].ack_recvd = 0;
        audio_stream[session_type].ack_status = CTRL_ACK_UNKNOWN;
        auto ret = BluetoothAudioSessionControl::SuspendStream(audio_stream[session_type].sessionType);
        if (ret == false) {
            ALOGE("%s: client has died",__func__);
            lock.unlock();
            return -1;
        }
        pthread_mutex_lock(&audio_stream[session_type].ack_lock);
        status = audio_stream[session_type].ack_status;
        pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
        ALOGW("%s: ack status = %s", __func__, dump_ctrl_ack(status));
        if (status == CTRL_ACK_UNKNOWN) {
            wait_for_stack_response(max_waittime, session_type);
            pthread_mutex_lock(&audio_stream[session_type].ack_lock);
            status = audio_stream[session_type].ack_status;
            pthread_mutex_unlock(&audio_stream[session_type].ack_lock);
        }
        if (status == CTRL_ACK_SUCCESS) {
            ALOGD("%s: success", __func__);
            audio_stream[session_type].state = AUDIO_STATE_SUSPENDED;
            lock.unlock();
            return 0;
        } else {
            ALOGW("%s: failed", __func__);
            lock.unlock();
            return -1;
        }
    } else {
        ALOGW("%s: session is not ready", __func__);
    }
    lock.unlock();
    return -1;
}

void audio_handoff_triggered()
{
    if (qti_audio_hal_2_1) {
        vendor::qti::btoffload::V2_1::audio_handoff_triggered_qti();
        return;
    }
    if(qti_audio_hal) {
        vendor::qti::btoffload::audio_handoff_triggered_qti();
        return;
    }
    ALOGI("%s: state = %s", __func__, dump_a2dp_hal_state(audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state));
    std::unique_lock<std::mutex> lock(api_lock);
    if (audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state != AUDIO_STATE_STOPPED ||
        audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state != AUDIO_STATE_STOPPING) {
        audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state = AUDIO_STATE_STOPPED;
    }
    lock.unlock();
}

void clear_a2dpsuspend_flag()
{
    if(qti_audio_hal_2_1) {
        vendor::qti::btoffload::V2_1::clear_a2dp_suspend_flag_qti();
        return;
    }
    if(qti_audio_hal) {
        vendor::qti::btoffload::clear_a2dp_suspend_flag_qti();
        return;
    }
    ALOGI("%s: state = %s", __func__, dump_a2dp_hal_state(audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state));
    std::unique_lock<std::mutex> lock(api_lock);
    if (audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state == AUDIO_STATE_SUSPENDED) {
        audio_stream[A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state = AUDIO_STATE_STOPPED;
    }
    if (audio_stream[LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state == AUDIO_STATE_SUSPENDED) {
        audio_stream[LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH].state = AUDIO_STATE_STOPPED;
    }
    if (audio_stream[LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH].state == AUDIO_STATE_SUSPENDED) {
        audio_stream[LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH].state = AUDIO_STATE_STOPPED;
    }
    lock.unlock();
}

void * audio_get_codec_config_api(tSESSION_TYPE session_type, uint8_t* multicast_status, uint8_t* num_dev,
                              audio_format_t *codec_type) {
    return audio_get_codec_config(session_type, multicast_status, num_dev, codec_type);
}

void * audio_get_codec_config(uint8_t* multicast_status, uint8_t* num_dev,
                              audio_format_t *codec_type) {
    return audio_get_codec_config(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH, multicast_status, num_dev, codec_type);
}

void * audio_get_codec_config(tSESSION_TYPE session_type, uint8_t* multicast_status, uint8_t* num_dev,
                              audio_format_t *codec_type)
{
    ALOGI("%s: session_type = %d", __func__, session_type);
    if(qti_audio_hal_2_1) {
        return vendor::qti::btoffload::V2_1::audio_get_codec_config_qti(multicast_status, num_dev, codec_type);
    }
    if(qti_audio_hal) {
        return vendor::qti::btoffload::audio_get_codec_config_qti(multicast_status, num_dev, codec_type);
    }
    ALOGI("%s: state = %s", __func__, dump_a2dp_hal_state(audio_stream[session_type].state));
    if (audio_stream[session_type].session_ready) {
      if (audio_stream[session_type].sessionType == SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
          return (a2dp_codec_parser(&audio_stream[session_type].codec_cfg, codec_type, NULL));
      } else {
          return (le_audio_codec_parser(&audio_stream[session_type], session_type, codec_type, NULL));
      }
    }
    return NULL;
}

int audio_check_a2dp_ready()
{
   ALOGI("%s:", __func__);
   return audio_check_a2dp_ready_api(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}

int audio_check_a2dp_ready_api(tSESSION_TYPE session_type)
{
    if(qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_check_a2dp_ready_qti();
    if(qti_audio_hal) return vendor::qti::btoffload::audio_check_a2dp_ready_qti();
    ALOGI("%s: state = %s", __func__, dump_a2dp_hal_state(audio_stream[session_type].state));
    int status;
    std::unique_lock<std::mutex> lock(api_lock);
    ALOGI("%s: session ready  = %d", __func__, audio_stream[session_type].session_ready);
    if (session_type != UNKNOWN && audio_stream[session_type].session_ready == 0) {
        ALOGI("%s: session restarted, do port init", __func__);
        audio_stream_open_api(session_type);
    }
    if (audio_stream[session_type].session_ready) {
      status = 1;
    } else {
      status = 0;
    }
    lock.unlock();
    return status;
}

uint16_t audio_get_a2dp_sink_latency() {
    ALOGI("%s", __func__);
    return audio_get_a2dp_sink_latency(A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH);
}
uint16_t audio_get_a2dp_sink_latency(tSESSION_TYPE session_type) {
     ALOGI("%s: session_type is %d", __func__, session_type);

    if(qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_get_a2dp_sink_latency_qti();
    if(qti_audio_hal) return vendor::qti::btoffload::audio_get_a2dp_sink_latency_qti();
    struct timespec* data_position = { };
    PresentationPosition remote_delay;
    uint64_t remote_delay_ns = 0;
    std::unique_lock<std::mutex> lock(api_lock);
    if (audio_stream[session_type].session_ready) {
      if (audio_stream[session_type].sessionType ==
          SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
        audio_stream[session_type].sink_latency =
          audio_stream[session_type].codec_cfg.get<AudioConfiguration::leAudioConfig>().peerDelayUs;
      } else if (audio_stream[session_type].sessionType ==
          SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
        audio_stream[session_type].sink_latency =
          audio_stream[session_type].dec_codec_cfg.get<AudioConfiguration::leAudioConfig>().peerDelayUs;
      } else {
        BluetoothAudioSessionControl::GetPresentationPosition
          (audio_stream[session_type].sessionType, remote_delay);
        remote_delay_ns = remote_delay.remoteDeviceAudioDelayNanos;
        audio_stream[session_type].sink_latency = (remote_delay_ns/1000000);
      }
    } else {
        audio_stream[session_type].sink_latency = A2DP_DEFAULT_SINK_LATENCY;
    }
    lock.unlock();
    ALOGI("%s: sink_latency: %d", __func__, audio_stream[session_type].sink_latency);
    return audio_stream[session_type].sink_latency;
}

uint16_t audio_sink_get_a2dp_latency() {
    ALOGI("%s: ", __func__);
    return  audio_get_a2dp_sink_latency();
}

uint16_t audio_sink_get_a2dp_latency_api(tSESSION_TYPE session_type) {
    ALOGI("%s: session type is %d", __func__, session_type);
    return  audio_get_a2dp_sink_latency(session_type);
}

int audio_stream_get_supported_latency_modes_api(tSESSION_TYPE session_type,
                                                 size_t *num_modes, size_t max_latency_modes,
                                                 uint32_t *modes) {
  ALOGI("%s: session type is %d", __func__, session_type);
  size_t num_of_modes = 0;
  if (!audio_stream[session_type].session_ready ||
      (audio_stream[session_type].sessionType !=
       SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH)) {
    ALOGE("%s: session is not ready or session type is non-A2DP", __func__);
    return -1;
  }
  std::vector<LatencyMode> latency_modes =
      BluetoothAudioSessionControl::GetSupportedLatencyModes(audio_stream[session_type].sessionType);

  if (latency_modes.size() > max_latency_modes) {
    ALOGE("%s: max latency modes limit reached ", __func__);
    return -1;
  }
  for (int i=0; i < latency_modes.size(); i++) {
    switch (latency_modes[i]) {
      case LatencyMode::LOW_LATENCY:
        *modes = AUDIO_LATENCY_MODE_LOW;
        modes++;
        num_of_modes++;
        break;
      case LatencyMode::FREE:
        *modes = AUDIO_LATENCY_MODE_FREE;
        modes++;
        num_of_modes++;
        break;
      default:
        ALOGE("%s: Unknown mode:%d", __func__, latency_modes[i]);
        break;
    }
  }
  *num_modes = num_of_modes;
  return CTRL_ACK_SUCCESS;
}

int audio_stream_set_latency_mode_api(const tSESSION_TYPE session_type, uint32_t latency_mode) {
  ALOGI("%s: session type is %d", __func__, session_type);
  if (!audio_stream[session_type].session_ready ||
      (audio_stream[session_type].sessionType !=
       SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH)) {
    ALOGE("%s: session is not ready or session type is non-A2DP", __func__);
    return -1;
  }

  LatencyMode mode = LatencyMode::FREE;
  switch (latency_mode) {
    case AUDIO_LATENCY_MODE_LOW:
      mode = LatencyMode::LOW_LATENCY;
      break;
    case AUDIO_LATENCY_MODE_FREE:
      mode = LatencyMode::FREE;
      break;
    default:
      ALOGE("%s: Unknown latency mode:%d", __func__, latency_mode);
      break;
  }
  BluetoothAudioSessionControl::SetLatencyMode(audio_stream[session_type].sessionType, mode);
  return CTRL_ACK_SUCCESS;
}

void ldac_codec_parser(AudioConfiguration *codec_cfg)
{
    //memset(&ldac_codec, 0, sizeof(audio_ldac_encoder_config_t));
    CodecConfiguration *codecConfig =
          &codec_cfg->get<AudioConfiguration::a2dpConfig>();
    LdacConfiguration  *ldacConfig =
         &codecConfig->config.get<CodecConfiguration::CodecSpecific::ldacConfig>();

    switch (ldacConfig->sampleRateHz) {
        case 44100:
             ldac_codec.sampling_rate = 44100;
             break;
        case 48000:
             ldac_codec.sampling_rate = 48000;
             break;
        case 88200:
             ldac_codec.sampling_rate = 88200;
             break;
        case 96000:
             ldac_codec.sampling_rate = 96000;
             break;
        case 176400:
             ldac_codec.sampling_rate = 176400;
             break;
        case 192000:
             ldac_codec.sampling_rate = 192000;
             break;
        default:
             ALOGE("%s: LDAC: unknown sampling rate:%d", __func__,
                   ldacConfig->sampleRateHz);
             break;
    }

    switch (ldacConfig->channelMode) {
        case LdacChannelMode::DUAL:
            ldac_codec.channel_mode = A2D_LDAC_CHANNEL_DUAL;
            break;
        case LdacChannelMode::STEREO:
            ldac_codec.channel_mode = A2D_LDAC_CHANNEL_STEREO;
            break;
        case LdacChannelMode::MONO:
            ldac_codec.channel_mode = A2D_LDAC_CHANNEL_MONO;
            break;
        default:
             ALOGE("%s: LDAC: unknown channel mode:%u", __func__,
                   (unsigned)ldacConfig->channelMode);
             break;
    }
    ALOGD("%s: LDAC: channel mode: %d", __func__,ldac_codec.channel_mode);
    ALOGD("%s: LDAC: codec config copied", __func__);
    uint16_t mtu = codecConfig->peerMtu;
    if ((mtu == 0) || (mtu > MAX_2MBPS_A2DP_STREAM_MTU))
        mtu = MAX_2MBPS_A2DP_STREAM_MTU;
    ldac_codec.mtu = mtu;

    if (codecConfig->encodedAudioBitrate == 0) {
        ALOGW("%s: LDAC: bitrate is zero", __func__);
        // ldac_codec.bitrate = ?
    } else if (codecConfig->encodedAudioBitrate >= 0x00000001 &&
               codecConfig->encodedAudioBitrate <= 0x00FFFFFF) {
        ldac_codec.bitrate = codecConfig->encodedAudioBitrate;
    }

    switch (ldacConfig->bitsPerSample) {
        case 16:
            ldac_codec.bits_per_sample = 16;
            break;
        case 24:
            ldac_codec.bits_per_sample = 24;
            break;
        case 32:
            ldac_codec.bits_per_sample = 32;
            break;
        default:
            ALOGE("%s: LDAC: unknown bits per sample", __func__);
            break;
    }

    ALOGD("%s: LDAC: bits per sample:%d", __func__, ldac_codec.bits_per_sample);

    ldac_codec.is_abr_enabled = (ldacConfig->qualityIndex == LdacQualityIndex::ABR);

    ALOGD("%s: LDAC: create lookup for %d with ABR %d", __func__,
          ldacConfig->sampleRateHz, ldac_codec.is_abr_enabled);
    ALOGD("%s: LDAC: codec index value %u", __func__,
          (unsigned int)ldacConfig->qualityIndex);

    if (ldacConfig->sampleRateHz == 44100 ||
        ldacConfig->sampleRateHz == 88200) {
        int num_of_level_entries =
           sizeof(bit_rate_level_44_1k_88_2k_database) / sizeof(bit_rate_level_44_1k_88_2k_table_t);
        ldac_codec.level_to_bitrate_map.num_levels = num_of_level_entries;
        if (ldac_codec.is_abr_enabled) {
            ldac_codec.bitrate = bit_rate_level_44_1k_88_2k_database[0].bit_rate_value;
            ALOGI("%s: LDAC: send start highest bitrate value %d", __func__, ldac_codec.bitrate);
        }
        for (int i = 0; i < num_of_level_entries; i++) {
            ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].link_quality_level =
                bit_rate_level_44_1k_88_2k_database[i].level_value;
            ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].bitrate =
                bit_rate_level_44_1k_88_2k_database[i].bit_rate_value;
            ALOGI("%s: LDAC: level:%d bitrate:%d", __func__,
                  ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].link_quality_level,
                  ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].bitrate);
        }
    } else if (ldacConfig->sampleRateHz == 48000 ||
               ldacConfig->sampleRateHz == 96000) {
        int num_of_level_entries =
            sizeof(bit_rate_level_48k_96k_database) / sizeof(bit_rate_level_48k_96k_table_t);
        ldac_codec.level_to_bitrate_map.num_levels = num_of_level_entries;
        if (ldac_codec.is_abr_enabled) {
            ldac_codec.bitrate = bit_rate_level_48k_96k_database[0].bit_rate_value;
            ALOGI("%s: LDAC: send start highest bitrate value %d", __func__, ldac_codec.bitrate);
        }
        for (int i = 0; i < num_of_level_entries; i++) {
            ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].link_quality_level =
                bit_rate_level_48k_96k_database[i].level_value;
            ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].bitrate =
                bit_rate_level_48k_96k_database[i].bit_rate_value;
            ALOGI("%s: LDAC: level:%d bitrate:%d", __func__,
                  ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].link_quality_level,
                  ldac_codec.level_to_bitrate_map.bit_rate_level_map[i].bitrate);
        }
    } else {
        ALOGW("%s: LDAC: unsupported sample rate", __func__);
    }

    ALOGD("%s: LDAC: bitrate: %u", __func__, ldac_codec.bitrate);
    ALOGD("%s: LDAC: done copying full codec config", __func__);
}

bool audio_is_scrambling_enabled(void)
{
    char scram_val[PROPERTY_VALUE_MAX] = "false";
    if(qti_audio_hal_2_1) {
        return vendor::qti::btoffload::V2_1::audio_is_scrambling_enabled_qti();
    } else if(qti_audio_hal) {
        return vendor::qti::btoffload::audio_is_scrambling_enabled_qti();
    } else {
        tSESSION_TYPE session_type = A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH;
        if (BluetoothAudioSessionControl::IsSessionReady
                 (audio_stream[session_type].sessionType)) {
          AudioConfiguration *codec_cfg = &audio_stream[session_type].codec_cfg;
          if(codec_cfg->getTag() == AudioConfiguration::a2dpConfig) {
            CodecConfiguration *codecConfig = &codec_cfg->get<AudioConfiguration::a2dpConfig>();
            if(codecConfig->codecType == CodecType::APTX_ADAPTIVE) {
              ALOGD("%s: for APTX_ADAPTIVE, Scrambling is disabled", __func__);
              return false;
            }
          }
        }
        if (property_get("persist.vendor.qcom.bluetooth.scram.enabled", scram_val, "false") &&
            !strcmp(scram_val, "true")) {
          ALOGD("%s: Scrambling is enabled", __func__);
          return true;
        } else {
          ALOGD("%s: Scrambling is disabled", __func__);
          return false;
        }
    }
}

static void *audio_config_changed_handler(void * ptr) {
    ALOGD("%s: init", __func__);

    while(true) {
        tSESSION_TYPE session_type;
        {
            std::unique_lock<std::mutex> lk(mtx_config_changed);
            ALOGD("%s: waiting, reconfig_queue size=%d", __func__, reconfig_queue.size());
            if(reconfig_queue.size() == 0) {
              cv_config_changed.wait(lk);
            }
            ALOGD("%s: wait done: reconfig_cmd size = %d", __func__, reconfig_queue.size());
            auto itr = reconfig_queue.begin();
            if (itr != reconfig_queue.end()) {
              session_type = itr->session;
            } else {
              continue;
            }
        }
        // Fetch updated Audio Configuration
        //memset(&audio_stream[session_type].codec_cfg, 0, sizeof(AudioConfiguration));
        SessionType session = audio_stream[session_type].sessionType;
        ALOGD("%s: session type = %d", __func__, session);
        ALOGD("%s: reconfig_pending = %d", __func__, audio_stream[session_type].reconfig_pending);
        if (audio_stream[session_type].reconfig_pending == false) {
          if (session ==
            SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::a2dpConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::a2dpConfig>());
          } else if(session ==
                  SessionType::LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioConfig>());
          }  else if(session ==
                   SessionType::LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
            audio_stream[session_type].dec_codec_cfg.
            set<AudioConfiguration::leAudioConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioConfig>());
          }  else if(session ==
                   SessionType::LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH) {
            audio_stream[session_type].codec_cfg.
            set<AudioConfiguration::leAudioBroadcastConfig>
            (BluetoothAudioSessionControl::GetAudioConfig(session).
            get<AudioConfiguration::leAudioBroadcastConfig>());
          }
        }
        // Invoke callback
        if (reconfig_to_audio_cb != NULL) {
            bool reconfig = audio_stream[session_type].reconfig_pending;
            session_reconfig_t req = {.session = session_type, .status = !reconfig};
            std::unique_lock<std::mutex> lk(mtx_config_changed);
            if (!reconfig_queue.empty()) {
              auto it = reconfig_queue.begin();
              req = *it;
              reconfig_queue.erase(it);
              ALOGD("%s: reconfig_to_audio_cb called with session = %d, status = %d",__func__, req.session, req.status);
            }
            lk.unlock();
            reconfig_to_audio_cb(req.session, req.status);
            ALOGD("%s: reconfig_to_audio_cb call done",__func__);
        }
    }
    return NULL;
}

void bt_audio_pre_init() {
    ALOGD("%s: calling 2.0 preinit", __func__);
    vendor::qti::btoffload::bt_audio_pre_init_qti();
    ALOGD("%s: calling 2.1 preinit", __func__);
    vendor::qti::btoffload::V2_1::bt_audio_pre_init_qti();
    createIBluetoothAudioProviderFactory();
    if (!config_thread_created) {
        int ret = pthread_create(&thread_config_changed, NULL, audio_config_changed_handler, NULL);
        if (ret == 0) {
            config_thread_created = true;
        } else {
            ALOGE("%s: Thread create failed, %d", __func__, ret);
            config_thread_created = false;
        }
    } else {
        ALOGD("%s: config thread already exists", __func__);
    }
}

int audio_sink_start_stream() {
  return audio_sink_stream_start();
}

int audio_sink_stream_start() {
  //if(qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_sink_stream_start_qti();
  //else return -1;
  return -1;
}

int audio_sink_stop_stream() {
  return audio_sink_stream_stop();
}

int audio_sink_stream_stop() {
  //if (qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_sink_stream_stop_qti();
  //else return -1;
  return -1;
}

int audio_sink_suspend_stream() {
  return audio_sink_stream_suspend();
}

int audio_sink_stream_suspend() {
  //if(qti_audio_hal_2_1) return vendor::qti::btoffload::V2_1::audio_sink_stream_suspend_qti();
  //else return -1;
  return -1;
}


void update_metadata(const tSESSION_TYPE session_type , void * metadata) {
    ALOGD("%s: session_type =%d", __func__, session_type);
    std::unique_lock<std::mutex> lock(api_lock);
    if ((session_type == LE_AUDIO_HARDWARE_OFFLOAD_ENCODING_DATAPATH)
         || (session_type == LE_AUDIO_BROADCAST_HARDWARE_OFFLOAD_ENCODING_DATAPATH)
         || (session_type == A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH)) {
        ALOGD("%s: session_type =%d, source meta data", __func__, audio_stream[session_type].sessionType);
        ALOGD("%s: source meta data is session ready = %d", __func__, BluetoothAudioSessionControl::IsSessionReady(audio_stream[session_type].sessionType));
      BluetoothAudioSessionControl::UpdateSourceMetadata(audio_stream[session_type].sessionType, *((source_metadata*) metadata));
    } else if (session_type == LE_AUDIO_HARDWARE_OFFLOAD_DECODING_DATAPATH) {
        ALOGD("%s: session_type =%d, sink meta data", __func__, audio_stream[session_type].sessionType);
        ALOGD("%s: sink meta data is session ready = %d", __func__, BluetoothAudioSessionControl::IsSessionReady(audio_stream[session_type].sessionType));
      BluetoothAudioSessionControl::UpdateSinkMetadata(audio_stream[session_type].sessionType, *((sink_metadata*) metadata));
    }
    lock.unlock();
    ALOGD("%s: session_type exit", __func__, session_type);
}

#ifdef AIDL_EXTENSION
binder_status_t createIBluetoothAudioProviderFactory() {
  std::shared_ptr<BluetoothAudioProviderFactory>
   factory = ::ndk::SharedRefBase::make<BluetoothAudioProviderFactory>();

  //ABinderProcess_setThreadPoolMaxThreadCount(0);
  // making the extension service
  std::shared_ptr<BluetoothAudioProviderExt> provider_ext =
               ndk::SharedRefBase::make<BluetoothAudioProviderExt>();

  // need to attach the extension to the same binder we will be registering
  if(STATUS_OK == AIBinder_setExtension(factory->asBinder().get(),
                      provider_ext->asBinder().get())) {
    ALOGD("%s: Registering the AIDL 1.0 extension service ", __func__);
  }

  const std::string instance_name =
      std::string() + BluetoothAudioProviderFactory::descriptor + "/default";
  binder_status_t aidl_status = AServiceManager_addService(
      factory->asBinder().get(), instance_name.c_str());
  ALOGW_IF(aidl_status != STATUS_OK, "Could not register %s, status=%d",
           instance_name.c_str(), aidl_status);
  return aidl_status;
}
#endif /* AIDL_EXTENSION */
