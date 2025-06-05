/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __CNSS_PLAT_IPC_QMI_H__
#define __CNSS_PLAT_IPC_QMI_H__

#include "cnss_plat_ipc_service_v01.h"

#ifdef CNSS_PLAT_IPC_QMI
int cnss_plat_ipc_qmi_init(void);
void cnss_plat_ipc_qmi_deinit(void);
void cnss_plat_ipc_qmi_msg_process(void);
int cnss_plat_ipc_qmi_get_fd(void);
int cnss_plat_ipc_qmi_send_config_param_req(
				 cnss_plat_ipc_qmi_config_param_type_v01 type,
				 uint32_t instance_id, uint64_t value);
#else
static inline
int cnss_plat_ipc_qmi_init(void)
{
	return 0;
}

static inline
void cnss_plat_ipc_qmi_deinit(void) {}

static inline
void cnss_plat_ipc_qmi_msg_process(void) {}

static inline
int cnss_plat_ipc_qmi_get_fd(void)
{
	return -1;
}

static inline
int cnss_plat_ipc_qmi_send_config_param_req(
				 cnss_plat_ipc_qmi_config_param_type_v01 type,
				 uint32_t instance_id, uint64_t value)
{
	UNUSED(type);
	UNUSED(instance_id);
	UNUSED(value);
	return 0;
}
#endif

#endif
