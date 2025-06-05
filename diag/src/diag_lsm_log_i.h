#ifndef DIAG_LSM_LOG_I_H
#define DIAG_LSM_LOG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

EXTENDED DIAGNOSTIC LOG LEGACY SERVICE MAPPING HEADER FILE
(INTERNAL ONLY)

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support the reporting
  of messages.  This includes support for the
  extended capabilities as well as the legacy messaging scheme.

Copyright (c) 2007-2011,2014, 2018, 2020, 2022-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/07   JV      Created File
===========================================================================*/

#include "diaglogi.h"

typedef PACK(struct) {
	uint8 equip_id;
	unsigned int num_items;
} diag_log_mask_update_t;

#define LOG_ITEMS_TO_SIZE(num_items)	((num_items + 7) / 8)

#define SECURE_LOG_PKT_VERSION1	1

/**
 * Secure log packet with diag-id support. Encryption is not supported now,
 * so, not defined encryption header here. Refer ICD for more details.
 */
typedef PACK(struct
{
	uint8 cmd_code;
	uint8 version;
	uint8 diag_id : 5;
	uint8 reserved1 : 1;
	uint8 extnd_id_flag : 1;
	uint8 encryption_flag : 1;
	uint8 timestamp_type : 3;
	uint8 reserved2 : 5;
	uint32 extnd_id;
})
secure_log_pkt_type;

extern diag_log_mask_t enabled_log_mask_arr[MAX_EQUIP_ID];

/* Initializes legacy service mapping for Diag log service */
boolean Diag_LSM_Log_Init(void);

/* Releases all resources related to Diag Log Service */
void Diag_LSM_Log_DeInit(void);

/* updates the copy of log_mask */
void log_update_mask(unsigned char *, int len);

/* updates the copy of the dci log_mask */
void log_update_dci_mask(unsigned char*, int len);

/* clear the local copy of the log mask */
void log_clear_mask(void);

/* clear the local copy of the dci log mask */
void log_clear_dci_mask(void);

#endif /* DIAG_LSM_LOG_I_H */
