/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag mux communication support

GENERAL DESCRIPTION

Implementation of diag mux layer to communicate with diag transport layer.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "diag.h"
#include "diag_cntl.h"
#include "diag_mux.h"
#include "peripheral.h"
#include "dm.h"
#include "diag_hidl_wrapper.h"

#define MAX_MUX_STRLEN		10

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif

struct diag_mux_state_t *diag_mux;
static struct diag_logger_t usb_logger;
static struct diag_logger_t md_logger;
static struct diag_logger_t socket_logger;
static struct diag_logger_t pcie_logger;
static struct diag_logger_t gvm_logger;

static struct diag_logger_ops usb_log_ops = {
	.write = diag_usb_write,
	.close_device = diag_usb_disconnect,
	.open_device = diag_usb_connect,
	.close = diag_usb_disconnect_all,
	.open = diag_usb_connect_all,
};

static struct diag_logger_ops pcie_log_ops = {
	.write = diag_pcie_write,
	.close_device = NULL,
	.open_device = NULL,
	.close = diag_pcie_close,
	.open = diag_pcie_open,
};

static struct diag_logger_ops md_log_ops = {
	.write = diag_md_write,
	.open = NULL,
	.close = NULL,
	.open_device = NULL,
	.close_device = NULL,
};

static struct diag_logger_ops gvm_log_ops = {
	.write = diag_vm_write,
	.open = NULL,
	.close = NULL,
	.open_device = NULL,
	.close_device = NULL,
};

static void diag_enable_hdlc_mode(void)
{
	int i;

	pthread_mutex_lock(&diagmem->hdlc_disable_mutex);
	if (diagmem->md_session_mode[DIAG_LOCAL_PROC] == DIAG_MD_NONE) {
		diagmem->hdlc_disabled = 0;
		/*
		 * HDLC encoding is re-enabled when
		 * there is logical/physical disconnection of diag
		 * to USB.
		 */
		for (i = 0; i < NUM_MD_SESSIONS; i++)
			diagmem->p_hdlc_disabled[i] =
				diagmem->hdlc_disabled;
	}
	pthread_mutex_unlock(&diagmem->hdlc_disable_mutex);

}

int diag_apps_rsp_send(int pid, unsigned char *resp, unsigned int write_len)
{
	struct diag_pkt_frame_t header;
	struct diag_md_session_t *info;
	unsigned char *rsp_ptr = NULL;
	uint8_t hdlc_disabled;
	int len = 0;
	int index = PERIPHERAL_APPS;
	int ret;
	int i;

	info = diag_md_session_get_pid(pid);
	pthread_mutex_lock(&diagmem->hdlc_disable_mutex);
	hdlc_disabled = (info) ? info->hdlc_disabled : diagmem->hdlc_disabled;
	pthread_mutex_unlock(&diagmem->hdlc_disable_mutex);
	if (hdlc_disabled) {
		memset(&header, 0, sizeof(header));
		header.start = CONTROL_CHAR;
		header.version = 1;
		header.length = write_len;

		rsp_ptr = malloc(sizeof(header) + write_len + 1);
		if (!rsp_ptr)
			return -EMSGSIZE;
		memcpy(rsp_ptr, &header, sizeof(header));
		len += sizeof(header);

		memcpy(rsp_ptr + len, resp, write_len);
		len += write_len;

		*(uint8_t *)(rsp_ptr + len) = CONTROL_CHAR;
		len += sizeof(uint8_t);

		if (info && info->peripheral_mask[DIAG_LOCAL_PROC]) {
			for (i = 0; i < NUM_MD_SESSIONS; i++) {
				if (info->peripheral_mask[DIAG_LOCAL_PROC] & MD_PERIPHERAL_MASK(i)) {
					index = i;
					break;
				}
			}
		}
		ret = diag_mux_write(NULL, 0, index, rsp_ptr, len, NULL, 1);
		free(rsp_ptr);
		return ret;
	} else {
		if (info && info->peripheral_mask[DIAG_LOCAL_PROC]) {
			for (i = 0; i < NUM_MD_SESSIONS; i++) {
				if (info->peripheral_mask[DIAG_LOCAL_PROC] & MD_PERIPHERAL_MASK(i)) {
					index = i;
					break;
				}
			}
		}
		return diag_mux_write(NULL, 0, index, resp, write_len, NULL, 1);
	}
}

static struct diag_logger_ops socket_log_ops = {
	.write = diag_socket_write,
	.open = NULL,
	.close = NULL,
	.open_device = NULL,
	.close_device = NULL,
};

int diag_md_get_peripheral(int id)
{
	int idx;

	if (id < 0 || id >= NUM_MD_SESSIONS)
		return -EINVAL;

	if (id <= NUM_PERIPHERALS)
		return id;

	/* id is a pd, check to see if any md session set that pd mask */
	idx = id - UPD_WLAN;
	if (diagmem->num_pd_session && diagmem->pd_logging_mode[idx])
		return id;

	/* No matching pd sessions, return peripheral that pd is on */
	return diag_query_peripheral_from_pd((uint8_t)id);
}

int diag_md_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag)
{
	int remote_proc, data_len = 0;
	int user_space_data_type = 32;
	size_t outlen = 0;
	void *outbuf = NULL;
	struct diag_md_session_t *session_info = NULL;
	unsigned char *ptr;
	int num_data_fields = 1, offset = 0;
	int local_peripheral = peripheral;

	if (proc == DIAG_LOCAL_PROC) {
		local_peripheral = diag_md_get_peripheral(peripheral);
		if (local_peripheral < 0)
			return -EINVAL;
	}
	session_info = diag_md_session_get_peripheral(proc, local_peripheral);
	if (!session_info)
		return -EINVAL;

	if (!buf)
		return -EINVAL;

	if (proc > DIAG_LOCAL_PROC) {
		data_len = (len + sizeof(int)*4);
		ptr = alloca(data_len);
		if (!ptr) {
			printf("diag: In %s failed to allocate memory\n", __func__);
			return -ENOMEM;
		}
		memcpy(ptr,&user_space_data_type, sizeof(user_space_data_type));
		offset = offset + sizeof(user_space_data_type);
		memcpy(ptr + offset, &num_data_fields, sizeof(num_data_fields));
		remote_proc = -proc;
		offset = offset + sizeof(num_data_fields);
		memcpy(ptr + offset, &remote_proc, sizeof(remote_proc));
		offset = offset + sizeof(remote_proc);
		memcpy(ptr + offset, &len, sizeof(len));
		offset = offset + sizeof(len);
		memcpy(ptr + offset, buf, len);
		if (session_info->dm) {
			dm_send_flow(dm, session_info->dm, ptr, data_len, flow, cmd_rsp_flag);
		} else {
#ifndef FEATURE_LE_DIAG
			diag_update_hidl_client(USER_SPACE_DATA_TYPE);
			diag_copy_for_hidl_client(ptr, data_len, session_info->pid);
#endif /* FEATURE_LE_DIAG */
		}
	} else {
		if (diagmem->p_hdlc_disabled[local_peripheral]) {
                        data_len = (len + sizeof(int) * 3);
		} else {
			if (dm && dm->hdlc_enc_done) {
				outbuf = buf;
				outlen = len;
			} else {
				outbuf = hdlc_encode(dm, buf, (size_t)len, &outlen);
				if (!outbuf) {
					printf("failed to allocate hdlc destination buffer\n");
					return -ENOMEM;
				}
			}
			data_len = (outlen + sizeof(int) * 3);
		}
		ptr = alloca(data_len);
		if (!ptr) {
			printf("diag: In %s failed to allocate memory\n", __func__);
			return -ENOMEM;
		}
		memcpy(ptr,&user_space_data_type, sizeof(user_space_data_type));
		offset = offset + sizeof(user_space_data_type);
		memcpy(ptr + offset, &num_data_fields, sizeof(num_data_fields));
		offset = offset + sizeof(num_data_fields);
		if (diagmem->p_hdlc_disabled[local_peripheral]) {
			memcpy(ptr + offset, &len, sizeof(int));
			offset += sizeof(int);
			memcpy(ptr + offset, buf, len);
		} else {
			memcpy(ptr + offset, &outlen, sizeof(int));
			offset += sizeof(int);
			memcpy(ptr + offset, outbuf, outlen);
		}
		if (session_info->dm) {
			dm_send_flow(dm, session_info->dm, ptr, data_len, flow, cmd_rsp_flag);
		} else {
#ifndef FEATURE_LE_DIAG
			diag_update_hidl_client(USER_SPACE_DATA_TYPE);
			diag_copy_for_hidl_client(ptr, data_len, session_info->pid);
#endif /* FEATURE_LE_DIAG */
		}
		if (!dm) {
			if (outbuf)
				free(outbuf);
			outbuf = NULL;
		}
	}
	return 0;
}

int diag_socket_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag)
{
	(void)dm;
	(void)proc;
	(void)peripheral;

	if (socket_dm)
		dm_send_flow(NULL, socket_dm, buf, len, flow, cmd_rsp_flag);
	return 0;
}

int diag_mux_init(void)
{
	int proc;
	diag_mux = malloc(sizeof(struct diag_mux_state_t));
	if (!diag_mux)
		return -ENOMEM;

	usb_logger.mode = DIAG_USB_MODE;
	usb_logger.log_ops = &usb_log_ops;

	md_logger.mode = DIAG_MEMORY_DEVICE_MODE;
	md_logger.log_ops = &md_log_ops;

	socket_logger.mode = DIAG_USB_MODE;
	socket_logger.log_ops = &socket_log_ops;

	pcie_logger.mode = DIAG_PCIE_MODE;
	pcie_logger.log_ops = &pcie_log_ops;

	gvm_logger.mode = DIAG_USB_MODE;
	gvm_logger.log_ops = &gvm_log_ops;

	/*
	 * Set USB logging as the default logger. This is the mode
	 * Diag should be in when it initializes.
	 */
	diag_mux->usb_ptr = &usb_logger;
	diag_mux->md_ptr = &md_logger;
	diag_mux->socket_ptr = &socket_logger;
	diag_mux->pcie_ptr = &pcie_logger;
	diag_mux->gvm_ptr = &gvm_logger;

	for (proc = 0; proc < NUM_MUX_PROC; proc++) {
		if (socket_dm) {
			diag_mux->logger[proc] = &socket_logger;
		} else {
			if (vm_enabled)
				diag_mux->logger[proc] = &gvm_logger;
			else
				diag_mux->logger[proc] = &usb_logger;
		}
		diag_mux->mux_mask[proc] = 0;
		diag_mux->mode[proc] = DIAG_USB_MODE;
	}
	return 0;
}

void diag_mux_exit(void)
{
	free(diag_mux);
	diag_mux = NULL;
}

int diag_mux_write(struct diag_client *dm, int proc, int peripheral, unsigned char * buf, int len, struct watch_flow *flow, int cmd_rsp_flag)
{
	struct diag_logger_t *logger = NULL;
	int local_peripheral = peripheral;
	int logger_mode = -1;
	unsigned char logger_ptr[MAX_MUX_STRLEN];

	if (proc == DIAG_LOCAL_PROC) {
		local_peripheral = diag_md_get_peripheral(peripheral);
		if (local_peripheral < 0)
			return -EINVAL;
	}
	if (MD_PERIPHERAL_MASK(local_peripheral) & diag_mux->mux_mask[proc]) {
		logger = diag_mux->md_ptr;
		logger_mode = MEMORY_DEVICE_MODE;
		strlcpy(logger_ptr, "md_ptr", 10);
	} else {
		if (socket_dm) {
			logger = diag_mux->socket_ptr;
			logger_mode = SOCKET_MODE;
			strlcpy(logger_ptr, "socket", 10);
		} else if (pcie_dm) {
			logger = diag_mux->pcie_ptr;
			logger_mode = PCIE_MODE;
			strlcpy(logger_ptr, "pcie", 10);
		} else if (vm_enabled) {
			logger = diag_mux->gvm_ptr;
			logger_mode = QRTR_SOCKET_MODE;
			strlcpy(logger_ptr, "gvm", 10);
		} else {
			logger = diag_mux->usb_ptr;
			logger_mode = USB_MODE;
			strlcpy(logger_ptr, "usb", 10);
		}
	}
	if (cmd_rsp_flag && (diag_debug_mask & DIAG_DBG_MASK_MUX)) {
		ALOGM(DIAG_DBG_MASK_MUX, "diag: %s: cmd rsp of len %d written to %s (%d)\n",
					__func__, len, logger_ptr, logger_mode);
		print_hex_dump(__func__, (void *)buf, MIN(len, PRINT_HEX_DUMP_LEN));
	}
	if (logger && logger->log_ops && logger->log_ops->write)
		return logger->log_ops->write(dm, proc, peripheral, buf, len, flow, cmd_rsp_flag);
	return 0;
}

int diag_mux_close_peripheral(int proc, uint8_t peripheral)
{
	struct diag_logger_t *logger = NULL;

	if (proc < 0 || proc >= NUM_MUX_PROC)
		return -EINVAL;
	/* Peripheral should account for Apps data as well */
	if (peripheral > NUM_PERIPHERALS) {
		if (!diagmem->num_pd_session)
			return -EINVAL;
		if (peripheral > NUM_MD_SESSIONS)
			return -EINVAL;
	}

	if (!diag_mux)
		return -EIO;

	if (MD_PERIPHERAL_MASK(peripheral) & diag_mux->mux_mask[proc])
		logger = diag_mux->md_ptr;
	else
		logger = diag_mux->logger[proc];

	return 0;
}

int diag_mux_switch_logging(struct diag_client *dm,int proc, int *req_mode, unsigned int *peripheral_mask)
{
	unsigned int new_mask = 0;
	(void)dm;

	if (!req_mode)
		return -EINVAL;


	switch (*req_mode) {
	case DIAG_PCIE_MODE:
	case DIAG_USB_MODE:
		new_mask = ~(*peripheral_mask) & diag_mux->mux_mask[proc];
		if (new_mask != DIAG_CON_NONE)
			*req_mode = DIAG_MULTI_MODE;
		if (new_mask == DIAG_CON_ALL)
			*req_mode = DIAG_MEMORY_DEVICE_MODE;
		break;
	case DIAG_MEMORY_DEVICE_MODE:
		new_mask = (*peripheral_mask) | diag_mux->mux_mask[proc];
		if (new_mask != DIAG_CON_ALL)
			*req_mode = DIAG_MULTI_MODE;
		break;
	default:
		ALOGE("diag: Invalid mode %d in %s\n", *req_mode, __func__);
		return -EINVAL;
	}

	switch (diag_mux->mode[proc]) {
	case DIAG_PCIE_MODE:
		if (*req_mode == DIAG_MEMORY_DEVICE_MODE) {
			diag_mux->pcie_ptr->log_ops->close();
			diag_enable_hdlc_mode();
			diag_mux->logger[proc] = diag_mux->md_ptr;
		} else if (*req_mode == DIAG_MULTI_MODE) {
			diag_mux->logger[proc] = NULL;
		} else if (*req_mode == DIAG_USB_MODE) {
			if (socket_dm) {
				diag_mux->logger[proc] = diag_mux->socket_ptr;
				dm_enable(socket_dm);
			} else {
				diag_mux->logger[proc] = diag_mux->usb_ptr;
				diag_mux->usb_ptr->log_ops->open_device(proc);
			}
		}
		break;
	case DIAG_USB_MODE:
		if (*req_mode == DIAG_MEMORY_DEVICE_MODE) {
			if (socket_dm) {
				dm_disable(socket_dm);
			} else {
				diag_mux->usb_ptr->log_ops->close_device(proc);
				diag_enable_hdlc_mode();
				diag_mux->logger[proc] = diag_mux->md_ptr;
			}
		} else if (*req_mode == DIAG_MULTI_MODE) {
			diag_mux->logger[proc] = NULL;
		} else if (*req_mode == DIAG_PCIE_MODE) {
			diag_mux->usb_ptr->log_ops->close_device(proc);
			diag_mux->logger[proc] = diag_mux->pcie_ptr;
			diag_mux->pcie_ptr->log_ops->open();
		}
		break;
	case DIAG_MEMORY_DEVICE_MODE:
		if (*req_mode == DIAG_USB_MODE) {
			if (socket_dm) {
				diag_mux->logger[proc] = diag_mux->socket_ptr;
				dm_enable(socket_dm);
			} else {
				diag_enable_hdlc_mode();
				diag_mux->logger[proc] = diag_mux->usb_ptr;
				diag_mux->usb_ptr->log_ops->open_device(proc);
			}
		} else if (*req_mode == DIAG_MULTI_MODE) {
			if (socket_dm) {
				dm_enable(socket_dm);
			} else {
				diag_mux->usb_ptr->log_ops->open_device(proc);
			}
			diag_mux->logger[proc] = NULL;
		} else if (*req_mode == PCIE_MODE) {
			diag_mux->logger[proc] = diag_mux->pcie_ptr;
			diag_mux->pcie_ptr->log_ops->open();
		}
		break;
	case DIAG_MULTI_MODE:
		if (*req_mode == DIAG_USB_MODE) {
			if (socket_dm) {
				diag_mux->logger[proc] = diag_mux->socket_ptr;
			} else {
				diag_enable_hdlc_mode();
				diag_mux->logger[proc] = diag_mux->usb_ptr;
			}
		} else if (*req_mode == DIAG_MEMORY_DEVICE_MODE) {
			diag_mux->usb_ptr->log_ops->close_device(proc);
			diag_enable_hdlc_mode();
			diag_mux->logger[proc] = diag_mux->md_ptr;
		} else if (*req_mode == DIAG_PCIE_MODE) {
			diag_mux->logger[proc] = diag_mux->pcie_ptr;
			diag_mux->pcie_ptr->log_ops->open();
		}
		break;
	}
	if (*req_mode)
		diag_usb_mux_switch_cleanup();
	perif_flow_reset(*peripheral_mask);
	diag_mux->mode[proc] = *req_mode;
	diag_mux->mux_mask[proc] = new_mask;
	*peripheral_mask = new_mask;
	return 0;
}

