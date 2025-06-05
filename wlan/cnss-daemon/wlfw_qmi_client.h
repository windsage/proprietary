/*
 * Copyright (c) 2015-2019, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WLFW_QMI_CLIENT_H__
#define __WLFW_QMI_CLIENT_H__
#include <pthread.h>
#include "debug.h"

enum wlfw_svc_flag {
	SVC_START,
	SVC_RECONNECT,
	SVC_DISCONNECTED,
	SVC_EXIT,
};

#define MODEM_BASEBAND_PROPERTY   "ro.baseband"
#if defined(__BIONIC_FORTIFY)
#define MAX_PROPERTY_SIZE  PROP_VALUE_MAX
#else
#define MAX_PROPERTY_SIZE  10
#endif
#define MODEM_BASEBAND_VALUE_APQ  "apq"
#define MODEM_BASEBAND_VALUE_SDA  "sda"
#define MODEM_BASEBAND_VALUE_QCS  "qcs"

#ifdef ICNSS_QMI

int wlfw_start(enum wlfw_svc_flag flag, uint8_t index);
int wlfw_stop(enum wlfw_svc_flag flag, uint8_t index);
int wlfw_qdss_trace_start(uint32_t instance_id);
int wlfw_qdss_trace_stop(unsigned long long option, uint32_t instance_id);
int wlfw_send_qdss_trace_config_download_req(uint32_t instance_id);
void wlfw_set_qdss_max_file_len(uint32_t instance_id, uint32_t value);
void wlfw_set_qmi_timeout(uint32_t instance_id, uint32_t msec);
#else
static inline int wlfw_start(enum wlfw_svc_flag flag, uint8_t index)
{
	UNUSED(flag);
	UNUSED(index);
	return 0;
}
static inline int wlfw_stop(enum wlfw_svc_flag flag, uint8_t index)
{
	UNUSED(flag);
	UNUSED(index);
	return 0;
}
static inline int wlfw_qdss_trace_start(uint32_t instance_id)
{
	UNUSED(instance_id);
	return 0;
}
static inline int wlfw_qdss_trace_stop(unsigned int option,
				       uint32_t instance_id)
{
	UNUSED(option);
	UNUSED(instance_id);
	return 0;
}
static inline int wlfw_send_qdss_trace_config_download_req(uint32_t instance_id)
{
	UNUSED(instance_id);
	return 0;
}
static inline void wlfw_set_qdss_max_file_len(uint32_t instance_id,
					      uint32_t value)
{
	UNUSED(instance_id);
	UNUSED(value);
}
static inline void wlfw_set_qmi_timeout(uint32_t instance_id,
					uint32_t msec)
{
	UNUSED(instance_id);
	UNUSED(msec);
}
#endif /* ICNSS_QMI */


#endif /* __WLFW_QMI_CLIENT_H__ */
