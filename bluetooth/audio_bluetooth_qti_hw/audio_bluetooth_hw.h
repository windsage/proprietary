/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 */

#include <hardware/audio.h>
#pragma once

struct bluetooth_audio_device {
    struct audio_hw_device device;
    audio_stream_out *output;
};

// AIDL 1.0
int adev_set_parameters_aidl_1_0(struct audio_hw_device* dev,
                               const char* kvpairs);

char* adev_get_parameters_aidl_1_0(const struct audio_hw_device* dev,
                                 const char* keys);

int adev_init_check_aidl_1_0(const struct audio_hw_device* dev);


int adev_set_master_volume_aidl_1_0(struct audio_hw_device* dev, float volume);

int adev_get_master_volume_aidl_1_0(struct audio_hw_device* dev, float* volume);

int adev_set_master_mute_aidl_1_0(struct audio_hw_device* dev, bool muted);

int adev_get_master_mute_aidl_1_0(struct audio_hw_device* dev, bool* muted);

int adev_set_mode_aidl_1_0(struct audio_hw_device* dev, audio_mode_t mode);

int adev_set_mic_mute_aidl_1_0(struct audio_hw_device* dev, bool state);

int adev_get_mic_mute_aidl_1_0(const struct audio_hw_device* dev, bool* state);

int adev_dump_aidl_1_0(const audio_hw_device_t* device, int fd);

int adev_close_aidl_1_0(hw_device_t* device);

int adev_open_output_stream_aidl_1_0(struct audio_hw_device* dev,
                            audio_io_handle_t handle, audio_devices_t devices,
                            audio_output_flags_t flags,
                            struct audio_config* config,
                            struct audio_stream_out** stream_out,
                            const char* address __unused);

void adev_close_output_stream_aidl_1_0(struct audio_hw_device* dev,
                              struct audio_stream_out* stream);

size_t adev_get_input_buffer_size_aidl_1_0(const struct audio_hw_device* dev,
    const struct audio_config* config);

int adev_open_input_stream_aidl_1_0(struct audio_hw_device* dev,
                           audio_io_handle_t handle, audio_devices_t devices,
                           struct audio_config* config,
                           struct audio_stream_in** stream_in,
                           audio_input_flags_t flags __unused,
                           const char* address __unused,
                           audio_source_t source __unused);

void adev_close_input_stream_aidl_1_0(struct audio_hw_device* dev,
                             struct audio_stream_in* stream);