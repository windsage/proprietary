
#ifndef DIAG_LSM_EVENT_I_H
#define DIAG_LSM_EVENT_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Internal Header File for Event Legacy Service Mapping

GENERAL DESCRIPTION

Copyright (c) 2007-2011, 2014-2015, 2020, 2022-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/26/07   mad     Created File
===========================================================================*/

#include <stdbool.h>
#include "eventi.h"

#define NEW_EVENT_PKT_VERSION1	1
#define EVENT_MASK		0x01
#define DCI_EVENT_MASK		0x02

/* Event packet with diag-id support */
typedef PACK(struct
{
	uint8 cmd_code;
	uint8 version;
	uint8 diag_id : 5;
	uint8 reserved1 : 1;
	uint8 extnd_id_flag : 1;
	uint8 reserved2  : 1;
	uint8 timestamp_type : 3;
	uint8 reserved3 : 5;
	uint32 extnd_id;
	uint16 length;
})
event_diagid_head_type;

typedef PACK(struct
{
	event_id_type event_id;
	uint32 ts_lo;
	uint32 ts_hi;
	event_payload_type payload;
})
event_pkt_type;

extern unsigned char enabled_event_mask_arr[EVENT_MASK_SIZE];

/* Initializes legacy service mapping for Diag event service */
boolean Diag_LSM_Event_Init(void);

/* Deinitializes legacy service mapping for Diag event service */
void Diag_LSM_Event_DeInit(void);

/* updates the copy of event_mask */
void event_update_mask(unsigned char*, int len);

/* updates the copy of dci event_mask */
void event_update_dci_mask(unsigned char*, int len);

/* clears the local copy of the event_mask */
void event_clear_mask(void);

/* clears the local copy of the dci event_mask */
void event_clear_dci_mask(void);

#endif /* DIAG_LSM_EVENT_I_H */
