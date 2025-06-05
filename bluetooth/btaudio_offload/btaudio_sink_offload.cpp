/****************************************************************************
 * Copyright (c) 2017-2019,2022 Qualcomm Technologies, Inc.
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
/*     btaudio_sink_offload.cpp
 *
 *  Description:   Implements IPC interface between HAL and BT host
 *
 *****************************************************************************/
#include <time.h>
#include <unistd.h>
#include "btaudio_offload.h"
#include "btaudio_offload_qti.h"
#include "btaudio_offload_qti_2_1.h"
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <system/audio.h>
#include <utils/Log.h>
#include <cutils/properties.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "btaudio_offload_sink"

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

/*****************************************************************************
**  Local type definitions
******************************************************************************/

bool qti_audio_hal_2_1 = false;

/*****************************************************************************
**  Static functions
******************************************************************************/

/*****************************************************************************
**
** AUDIO DATA PATH
**
*****************************************************************************/

void bt_audio_pre_init() {
  ALOGD("%s: calling 2.1 preinit", __func__);
  vendor::qti::btoffload::V2_1::bt_audio_pre_init_qti();
}

int audio_stream_open() {
  ALOGI("%s", __func__);
  int vnd_session = vendor::qti::btoffload::V2_1::audio_stream_open_qti();
  if (vnd_session == 1) {
      qti_audio_hal_2_1 = true;
      return 0;
  }
  ALOGE("%s: failed", __func__);
  return -1;
}

int audio_stream_close() {
  ALOGI("%s", __func__);
  if(qti_audio_hal_2_1) {
    return vendor::qti::btoffload::V2_1::audio_stream_close_qti();
  } else return -1;
}


void *audio_get_decoder_config(audio_format_t *codec_type) {
  ALOGI("%s", __func__);
  if(qti_audio_hal_2_1) {
    return vendor::qti::btoffload::V2_1::audio_get_decoder_config_qti(codec_type);
  }
  return NULL;
}

int audio_sink_check_a2dp_ready() {
  ALOGI("%s", __func__);
  if(qti_audio_hal_2_1) {
    return vendor::qti::btoffload::V2_1::audio_check_a2dp_ready_qti();
  } else return -1;
}

int audio_sink_start_capture() {
  ALOGI("%s", __func__);
  //audio_stream_open();
  if(qti_audio_hal_2_1) {
    return vendor::qti::btoffload::V2_1::audio_stream_start_qti();

  } else return -1;
}

int audio_sink_stop_capture() {
  ALOGI("%s", __func__);
  int status = 0;
  if (qti_audio_hal_2_1) {
    status = vendor::qti::btoffload::V2_1::audio_stream_stop_qti();
    //audio_stream_close();
    return status;
  } else return -1;
}

int audio_sink_session_setup_complete(uint64_t system_latency) {
  ALOGI("%s", __func__);
  //TODO to pass it on to BT stack
  if(qti_audio_hal_2_1) {
    return vendor::qti::btoffload::V2_1::audio_update_sink_latency_qti((uint16_t)system_latency);
  }
  return 0;
}