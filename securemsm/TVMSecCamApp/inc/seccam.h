/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _SECCAM_H
#define _SECCAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "CSecureCamera2.h"
#include "ISecureCamera2.h"
#include "ISecureCamera2Notify_invoke.h"
#include "object.h"
#include "moduleAPI.h"
#include "TUtils.h"

// SECURE CAMERA ERROR CODES
typedef enum {
    SC_SUCCESS = 0x0,               /**< Operation was successful. */
    SC_FAILURE = 0x10,              /**< General failure. */
    SC_INVALID_PARAM = 0x11,        /**< Parameter passed in was invalid */
    SC_CAMERA_NOT_PROTECTED = 0x12, /**< Camera not protected */
    SC_MAP_FAILURE = 0x13,          /**< Buffer mappping failure */
    SC_KERNEL_FAILURE = 0x14,       /**< Kernel failure */
} seccam_etype;

typedef struct {
    void *buf;
    uint32_t buf_size;
    Object lock;
    size_t map_size;
} seccam_data_buf_t;

#pragma pack(push, seccam, 1)

// cam_id is defined as int32_t to be consistent with HIDL
// frame_number is defined as int64_t to be consistent with HIDL
typedef struct {
    int32_t cam_id;
    int64_t frame_number;
    int64_t time_stamp;
} seccam_frame_info_t;

typedef struct {
    uint32_t ret;
} seccam_cmd_rsp_t;

#pragma pack(pop, seccam)

int32_t register_callback();

int32_t check_camera_state();

int32_t seccam_reset_camera();

void seccam_shutdown();

int32_t process_frame_algo(seccam_data_buf_t *capture,
                           seccam_frame_info_t *info, seccam_cmd_rsp_t *rsp);

int32_t set_license(const uint8_t *license_cert, size_t license_size);

int32_t set_sensors(uint32_t num_sensors);

int32_t set_callback(Object cb);

int32_t open_sc_object();

#endif /* _SECCAM_H */

