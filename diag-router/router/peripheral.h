/* Copyright (c) 2021, 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Copyright (c) 2008-2020, The Linux Foundation. All rights reserved.

 * Copyright (c) 2016, Linaro Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#define PERIPHERAL_APPS		0
#define PERIPHERAL_MODEM	1
#define PERIPHERAL_LPASS	2
#define PERIPHERAL_WCNSS	3
#define PERIPHERAL_SENSORS	4
#define PERIPHERAL_WDSP		5
#define PERIPHERAL_CDSP		6
#define PERIPHERAL_NPU		7
#define PERIPHERAL_NSP1		8
#define PERIPHERAL_GPDSP0	9
#define PERIPHERAL_GPDSP1	10
#define PERIPHERAL_HELIOS_M55	11
#define PERIPHERAL_SLATE_APPS   12
#define PERIPHERAL_SLATE_ADSP   13
#define PERIPHERAL_TELE_GVM	14
#define PERIPHERAL_FOTA_GVM	15
#define PERIPHERAL_SOCCP	16
#define NUM_PERIPHERALS		17

#define UPD_WLAN		NUM_PERIPHERALS
#define UPD_AUDIO		(UPD_WLAN + 1)
#define UPD_SENSORS		(UPD_AUDIO + 1)
#define UPD_CHARGER		(UPD_SENSORS + 1)
#define UPD_OEM			(UPD_CHARGER + 1)
#define UPD_OIS			(UPD_OEM + 1)
#define NUM_UPD			6
#define PD_UNKNOWN		255
#define MAX_PERIPHERAL_UPD	4

/*---------------------------------------------------------------------------
 * 1st,2nd,3rd nibble : is used for peripheral bit mask
 * 4th nibble : is used for peripheral bit mask and user PD's
 * 5th nibble : is used for peripheral up/down status indication (DIAG_STATUS_OPEN/DIAG_STATUS_CLOSED)
 * 		This is used across user space & kernel space
 * 6th nibble : is used for MSM_MDM mask to handle signal (DIAG_MD_MSM_MASK/DIAG_MD_MDM_MASK/DIAG_MD_MDM2_MASK)
 *		Right now only being used on ODL
 * 7th & 8th nibble : Reserved, can be used for new peripheral bit mask
 *---------------------------------------------------------------------------
 */
/* bit masks for peripherals */
#define DIAG_CON_APSS		(0x0001)	/* Bit mask for APSS */
#define DIAG_CON_MPSS		(0x0002)	/* Bit mask for MPSS */
#define DIAG_CON_LPASS		(0x0004)	/* Bit mask for LPASS */
#define DIAG_CON_WCNSS		(0x0008)	/* Bit mask for WCNSS */
#define DIAG_CON_SENSORS	(0x0010)	/* Bit mask for Sensors */
#define DIAG_CON_WDSP		(0x0020)	/* Bit mask for WDSP */
#define DIAG_CON_CDSP		(0x0040)	/* Bit mask for CDSP */
#define DIAG_CON_NPU		(0x0080)	/* Bit mask for NPU */
#define DIAG_CON_NSP1		(0x0100)	/* Bit mask for NSP1 */
#define DIAG_CON_GPDSP0		(0x0200)	/* Bit mask for GPDSP0 */
#define DIAG_CON_GPDSP1		(0x0400)	/* Bit mask for GPDSP1 */
#define DIAG_CON_HELIOS_M55	(0x0800)	/* Bit mask for RESERVED */
#define DIAG_CON_SLATE_APPS     (0x1000)        /* Bit mask for Slate APPS */
#define DIAG_CON_SLATE_ADSP     (0x2000)        /* Bit mask for Slate ADSP */
#define DIAG_CON_TELE_GVM	(0x4000)	/* Bit mask for Tele-gvm */
#define DIAG_CON_FOTA_GVM	(0x8000)	/* Bit mask for fota-gvm */
#define DIAG_CON_SOCCP		(0x10000)	/* Bit mask for SOCCP */

/* bit masks for user PD's */
#define DIAG_CON_UPD_WLAN	(0x1000)	/*Bit mask for WLAN PD*/
#define DIAG_CON_UPD_AUDIO	(0x2000)	/*Bit mask for AUDIO PD*/
#define DIAG_CON_UPD_SENSORS	(0x4000)	/*Bit mask for SENSORS PD*/
#define DIAG_CON_UPD_CHARGER	(0x8000)	/* Bit mask for CHARGER PD */
#define DIAG_CON_UPD_OEM	(0x10000)	/* Bit mask for OEM PD */
#define DIAG_CON_UPD_OIS	(0x20000)	/* Bit mask for OIS PD*/
#define DIAG_CON_ALL		(DIAG_CON_APSS | DIAG_CON_MPSS \
				| DIAG_CON_LPASS | DIAG_CON_WCNSS \
				| DIAG_CON_SENSORS | DIAG_CON_WDSP \
				| DIAG_CON_CDSP | DIAG_CON_NPU \
				| DIAG_CON_NSP1 | DIAG_CON_GPDSP0 \
				| DIAG_CON_GPDSP1 | DIAG_CON_HELIOS_M55 \
				| DIAG_CON_SLATE_APPS | DIAG_CON_SLATE_ADSP \
				| DIAG_CON_TELE_GVM | DIAG_CON_FOTA_GVM | DIAG_CON_SOCCP)
#define DIAG_CON_NONE		(0x0000)	/* Bit mask for No SS*/
#define DIAG_CON_UPD_ALL	(DIAG_CON_UPD_WLAN \
 				| DIAG_CON_UPD_AUDIO \
 				| DIAG_CON_UPD_SENSORS \
 				| DIAG_CON_UPD_CHARGER \
				| DIAG_CON_UPD_OEM \
				| DIAG_CON_UPD_OIS)

#define DIAG_CON_UPD_LAST	DIAG_CON_UPD_OIS

#define PERIPHERAL_MASK(x)					\
	((x == PERIPHERAL_APPS) ? DIAG_CON_APSS :		\
	((x == PERIPHERAL_MODEM) ? DIAG_CON_MPSS :		\
	((x == PERIPHERAL_LPASS) ? DIAG_CON_LPASS :		\
	((x == PERIPHERAL_WCNSS) ? DIAG_CON_WCNSS :		\
	((x == PERIPHERAL_SENSORS) ? DIAG_CON_SENSORS : \
	((x == PERIPHERAL_WDSP) ? DIAG_CON_WDSP : \
	((x == PERIPHERAL_CDSP) ? DIAG_CON_CDSP :	\
	((x == PERIPHERAL_NPU) ? DIAG_CON_NPU :	\
	((x == PERIPHERAL_NSP1) ? DIAG_CON_NSP1 : \
	((x == PERIPHERAL_GPDSP0) ? DIAG_CON_GPDSP0 : \
	((x == PERIPHERAL_GPDSP1) ? DIAG_CON_GPDSP1 : \
	((x == PERIPHERAL_HELIOS_M55) ? DIAG_CON_HELIOS_M55 : \
	((x == PERIPHERAL_SLATE_APPS) ? DIAG_CON_SLATE_APPS : \
	((x == PERIPHERAL_SLATE_ADSP) ? DIAG_CON_SLATE_ADSP : \
	((x == PERIPHERAL_SOCCP) ? DIAG_CON_SOCCP : 0))))))))))))))) \

#define TYPE_DATA		0
#define TYPE_CNTL		1
#define TYPE_DCI		2
#define TYPE_CMD		3
#define TYPE_DCI_CMD		4
#define NUM_TYPES		5

struct diag_buffering_mode_t {
	uint8_t peripheral;
	uint8_t mode;
	uint8_t high_wm_val;
	uint8_t low_wm_val;
} __packed;

struct pd_info {
	int pd_val;
	int diag_id;
	char *buf;
};

struct qrtr_pkt {
	uint32_t node;
	uint32_t port;
	unsigned int service;
	unsigned int instance;
};

struct peripheral {
	struct list_head  node;

	char *name;
	char *self_name;

	unsigned long features;

	struct list_head cmdq;
	struct list_head cntlq;
	struct list_head dataq;
	struct list_head dci_dataq;
	struct list_head dci_cmdq;
	int cntl_fd;
	bool cntl_open;
	int data_fd;
	bool data_open;
	bool dci_data_open;
	int cmd_fd;
	bool cmd_open;
	int dci_data_fd;
	int dci_cmd_fd;
	struct watch_flow *flow;
	struct watch_flow *dci_flow;
	int periph_id;
	int base_instance_id;

	struct qrtr_pkt cmd_ch;
	struct qrtr_pkt cmd_ch_remote;
	struct qrtr_pkt cntl_ch;
	struct qrtr_pkt cntl_ch_remote;
	struct qrtr_pkt data_ch;
	struct qrtr_pkt data_ch_remote;
	struct qrtr_pkt dci_cmd_ch;
	struct qrtr_pkt dci_data_ch;
	bool sockets;
	bool glinkpkt;
	unsigned int socket_node;
	struct circ_buf recv_buf;
	struct hdlc_decoder recv_decoder;
	struct diag_buffering_mode_t buf_params;
	int sent_feature_mask;
	int received_feature_mask;
	struct pd_info upd_info[MAX_PERIPHERAL_UPD];
	struct pd_info root_pd;
	int (*send)(struct peripheral *perif, const void *ptr, size_t len);
	void (*close)(struct peripheral *perif);
};

extern struct list_head peripherals;
struct diag_ssid_range_t;

int peripheral_init(void);
void peripheral_close(struct peripheral *peripheral);

void peripheral_broadcast_event_mask(uint32_t sub_id,
	uint8_t preset_id, int pid);

void peripheral_broadcast_log_mask(unsigned int equip_id,
	uint32_t sub_id,
	int pid,
	uint8_t preset_id);

void peripheral_broadcast_msg_mask(
	struct diag_ssid_range_t *range,
	uint32_t sub_id,
	int pid,
	int preset);

void peripheral_broadcast_time_switch(uint8_t time_api);
int peripheral_send(struct peripheral *peripheral, const void *ptr, size_t len);
void perif_flow_reset(int p_mask);
int peripheral_is_gvm(struct peripheral *perif);
void peripheral_broadcast_pkt_selection_req(uint8_t request);
struct peripheral * peripheral_get_vm(void);
#endif
