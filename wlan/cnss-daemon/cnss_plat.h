/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CNSS_PLAT_H__
#define __CNSS_PLAT_H__

#include <stdbool.h>
#include "wlan_firmware_service_v01.h"

#define CNSS_MAX_FILE_PATH 100
#define CNSS_FW_DATA_FILE_NAME_MAX 50
#define CNSS_ANDROID_PROPERTY_MAX 128
#define CNSS_CAL_FILE		"/data/vendor/wifi/wlfw_cal_"
#define CNSS_DEFAULT_QDSS_TRACE_FILE "/data/vendor/wifi/qdss_trace.bin"
#define CNSS_QDSS_TRACE_CONFIG_FILE_OLD "/data/vendor/wifi/qdss_trace_config.bin"
#define CNSS_QDSS_TRACE_CONFIG_FILE_NEW \
			"/vendor/firmware_mnt/image/qdss_trace_config.cfg"
#define CNSS_MAX_FILE_LEN (8 * 1024 * 1024)
#define MAX_DEVICE_NAME_LEN 20
#define MAX_NUM_RADIOS 5

enum wlfw_instance_id {
	ADRASTEA_ID = 0x0,
	HASTINGS_ID = 0x1,
	HAWKEYE_ID = 0x2,
	MOSELLE_ID = 0x3,
	PINE_1_ID = 0x27,
	PINE_2_ID = 0x28,
	PINE_3_ID = 0x29,
	PINE_4_ID = 0x2a,
	WAIKIKI_1_ID = 0x37,
	WAIKIKI_2_ID = 0x38,
	WAIKIKI_3_ID = 0x39,
	WAIKIKI_4_ID = 0x3a,
	SPRUCE_1_ID = 0x41,
	SPRUCE_2_ID = 0x42,
	YORK_1_ID = 0x51,
	YORK_2_ID = 0x52,
	YORK_3_ID = 0x53,
	PEBBLE_1_ID = 0x61,
	PEBBLE_2_ID = 0x62,
};

extern uint16_t g_instance_id_array[MAX_NUM_RADIOS];

struct cnss_evt {
	uint32_t msg_id;
	void *data;
	uint32_t data_len;
	struct cnss_evt *next;
};

struct cnss_evt_queue {
	pthread_mutex_t mutex;
	struct cnss_evt *head, *tail;
};

struct cnss_evt *cnss_evt_alloc(uint32_t msg_id, void *data, uint32_t data_len);
void cnss_evt_enqueue(struct cnss_evt_queue *evt_q, struct cnss_evt *evt);
struct cnss_evt *cnss_evt_dequeue(struct cnss_evt_queue *evt_q);
void cnss_evt_free_queue(struct cnss_evt_queue *evt_q);
void cnss_evt_free(struct cnss_evt *evt);

bool cnss_plat_get_dms_mac_addr_prov_support(void);
int cnss_plat_get_qdss_cfg_hw_trc_override(void);
int cnss_plat_save_file(const char *filename, unsigned char *data, uint32_t len,
			bool append, uint32_t max_file_len);
int cnss_plat_read_file(char *filename, unsigned char **file_buf);
const char *get_device_name_by_instance_id(uint32_t instance_id);
const char *get_file_suffix_by_instance_id(uint32_t instance_id);
int get_instance_id_by_device_name(char *device_name,
                                   uint32_t *instance_id);
#endif
