#ifndef DIAG_LSM_MSG_I_H
#define DIAG_LSM_MSG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

EXTENDED DIAGNOSTIC MESSAGE SERVICE LEGACY MAPPING
INTERNAL HEADER FILE

GENERAL DESCRIPTION
Internal header file

Copyright (c) 2007-2011, 2013-2015, 2018, 2020, 2022-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/07   mad     Created File
===========================================================================*/

#include "msg.h"
#include "msgcfg.h"
#include "diag_lsm.h"

#define MAX_SSID_PER_RANGE	200

#define DIAG_ID_MSG_PKT_VERSION_1	1

typedef enum {
	MSG_TYPE_EXT_PKT,	/* msg type for DIAG_EXT_MSG_F (0x79) */
	MSG_TYPE_OPT_EXT_PKT,	/* msg type for DIAG_QSR_EXT_MSG_TERSE_F (0x92) */
	MSG_TYPE_OPT_EXT_PH2	/* msg type for DIAG_QSR4_EXT_MSG_TERSE_F (0x99) */
} msg_type;

typedef PACK(struct) {
	uint32 ssid_first;
	uint32 ssid_last;
	uint32 ptr[MAX_SSID_PER_RANGE];
} diag_msg_mask_t;

typedef PACK(struct) {
	uint32_t ssid_first;
	uint32_t ssid_last;
	uint32_t range;
} diag_msg_mask_update_t;

typedef PACK(struct {
	uint8 cmd_code;
	uint8 version;
	uint8 diag_id : 5;
	uint8 reserved1 : 1;
	uint8 extnd_id_flag : 1;
	uint8 reserved2 : 1;
	uint8 timestamp_type : 3;
	uint8 msg_type : 3;
	uint8 reserved3 : 2;
	uint32 extnd_id;
	uint8 arg_info;
	uint32 ts_lo;
	uint32 ts_hi;
}) msg_diagid_head_type;

typedef PACK(struct {
	msg_diagid_head_type hdr;
	msg_desc_type desc;
	uint32 args[1];
}) msg_diagid_type;

typedef PACK(struct {
	msg_diagid_head_type hdr;
	msg_desc_type desc;
	uint32 msg_hash;
	uint32 args[1];
}) msg_qsr_diagid_type;

typedef PACK(struct {
	msg_diagid_head_type hdr;
	const msg_const_type *const_data_ptr;
	uint32 args[1];
}) msg_diagid_ext_store_type;

/*
 * Each row contains First (uint32_t), Last (uint32_t) values along with the
 * range of SSIDs (MAX_SSID_PER_RANGE * uint32_t). And there are
 * MSG_MASK_TBL_CNT rows.
 */
#define MSG_MASK_SIZE		(sizeof(diag_msg_mask_t) * MSG_MASK_TBL_CNT)

extern diag_msg_mask_t enabled_msg_mask_arr[MSG_MASK_TBL_CNT];

/* Initializes Mapping layer for message service*/
boolean Diag_LSM_Msg_Init(void);

/* clean up before exiting legacy service mapping layer.
Does nothing as of now, just returns TRUE. */
void Diag_LSM_Msg_DeInit(void);

/* updates the copy of the run-time masks for messages */
void msg_update_mask(unsigned char *ptr, int len);

/* clears local copy of the run time masks for messages */
void msg_clear_mask(void);

#endif /* DIAG_LSM_MSG_I_H */
