/*
 * Copyright (c) 2013-2014, 2017-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * image_version.h : Header file for image_version
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include <errno.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "msg.h"
#include "diagpkt.h"
#include "diagcmd.h"

/* Subsystem command codes for image version */

#define VERSION_DIAGPKT_PROCID            0x80              // VERSION_PROCID 128
#define VERSION_DIAGPKT_SUBSYS            0x63              // VERSION_SUBSYS 99
#define VERSION_DIAGPKT_PREFIX            0x00              // VERSION_PREFIX 0

#define SELECT_IMAGE_FILE		"/sys/devices/soc0/select_image"
#define IMAGE_VERSION_FILE		"/sys/devices/soc0/image_version"
#define IMAGE_VARIANT_FILE		"/sys/devices/soc0/image_variant"
#define IMAGE_CRM_VERSION_FILE		"/sys/devices/soc0/image_crm_version"


/* Size of version table stored in smem */
#define VERSION_TABLE_S 4096
#define IMAGE_VERSION_SINGLE_BLOCK_SIZE 128
#define IMAGE_VERSION_NAME_SIZE 75
#define IMAGE_VERSION_VARIANT_SIZE 20
#define IMAGE_VERSION_OEM_SIZE 32

