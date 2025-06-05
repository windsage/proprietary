#ifndef DIAG_SHARED_I_H
#define DIAG_SHARED_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                GENERAL DESCRIPTION

  Diag-internal declarations  and definitions common to API layer
  (Diag_LSM) and diag driver (DCM).

Copyright (c) 2007-2015, 2017-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE
$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   mad     Created file.
===========================================================================*/

#include <stdint.h>
#include "diagi.h"
#include "diag_lsm_dci_i.h"
/*Prototyping Diag: Adding a uint32 to the beginning of the diag packet,
 so the diag driver can identify what this is. This will be stripped out in the
 WDG_Write() function, and only the rest of the data will
 be copied to diagbuf. */
typedef struct
{
  uint32 diag_data_type; /* This will be used to identify whether the data passed to DCM is an event, log, F3 or response.*/
  uint8 rest_of_data;
}
diag_data;
#define DIAG_REST_OF_DATA_POS (FPOS(diag_data, rest_of_data))

/* Reasoning for an extra structure:
diagpkt_delay_alloc allocates a certain length, to send out a delayed response.
On WM, diagpkt_delay_commit needs to know the length of the response, to call WriteFile(),
and the header doesn't have any length field.

(On AMSS, diagpkt_delay_alloc directly allocates to diagbuf, and length is stored in the
diagbuf header. When diagpkt_delay_commit() (that results in diagbuf_commit()) is called,
we know how much to commit.)

*/
typedef struct
{
   uint32 length; /* length of delayed response pkt */
   diag_data diagdata;
}diag_data_delayed_response;
#define DIAG_DEL_RESP_REST_OF_DATA_POS (FPOS(diag_data_delayed_response,diagdata.rest_of_data))

/* different values that go in for diag_data_type */
#define DIAG_DATA_TYPE_EVENT         0
#define DIAG_DATA_TYPE_F3            1
#define DIAG_DATA_TYPE_LOG           2
#define DIAG_DATA_TYPE_RESPONSE      3
#define DIAG_DATA_TYPE_DELAYED_RESPONSE   4
#define DIAG_DATA_TYPE_DCI_LOG		0x00000100
#define DIAG_DATA_TYPE_DCI_EVENT	0x00000200
#define DIAG_DATA_TYPE_DCI_PKT		0x00000400

#define DIAG_EVENTSVC_MASK_CHANGE   0
#define DIAG_LOGSVC_MASK_CHANGE     1
#define DIAG_MSGSVC_MASK_CHANGE     2

/*
 * Structure for setting diag buffering mode on the peripherals
 *
 * @peripheral: id of the peripheral interested
 *
 * @mode: buffering mode - STREAMING_MODE, THRESHOLD_BUFFERING_MODE,
 * CIRCULAR_BUFFERING_MODE
 *
 * @highwmvalue: High watermark value (in percentage of buffer size) 0 keeps
 * current value on the peripherals. > 100 is not a value value. This value
 * cannot be lower than or equal to lowwmvalue
 *
 * @lowwmvalue: Low watermark value (in percentage of buffer size) 0 keeps
 * current value on the peripherals. > 100 is not a value value. This value
 * cannot be higher than or equal to highwmvalue
 */
PACK(struct) diag_periph_buffering_tx_mode {
	uint8 peripheral;
	uint8 mode;
	uint8 high_wm_val;
	uint8 low_wm_val;
};

#define MAX_SYNC_OBJ_NAME_SIZE 32

typedef struct {
	uint16_t cmd_code;
	uint16_t subsys_id;
	uint16_t cmd_code_lo;
	uint16_t cmd_code_hi;
} diag_cmd_reg_entry_t;

/*
 * @sync_obj_name: name of the synchronization object associated with this proc
 * @count: number of entries in the bind
 * @entries: the actual packet registrations
 */
typedef struct {
        char sync_obj_name[MAX_SYNC_OBJ_NAME_SIZE];
        uint32 count;
        diag_cmd_reg_entry_t *entries;
} diag_cmd_reg_tbl_t;

/*
 * To vote for real time or non real time mode
 *
 * @client_id: The DCI client ID. For other processes it should be -1
 * @proc: which process is voting for RT/NRT mode (DIAG_PROC_DCI or DIAG_PROC_MEMORY_DEVICE)
 * @real_time_vote: The actual vote. 0 for NRT mode, 1 for RT mode
 */
struct real_time_vote_t {
	int client_id;
	uint16 proc;
	uint8 real_time_vote;
};

PACK(struct) real_time_query_t {
        int real_time;
        int proc;
};

PACK(struct) diag_callback_reg_t {
        int proc;
};

/* functions used by diag driver to write events into diag buffers */
void* event_q_alloc (void *in, unsigned int length);
void event_q_pending (void *in);

void diagpkt_tbl_reg_dcm (void* LSM_obj, const byte * tbl_ptr, unsigned int count);
boolean diagpkt_tbl_dereg_dcm(uint32 client_id);
boolean event_process_LSM_mask_req (unsigned char* mask, int maskLen, int * maskLenReq);
boolean log_process_LSM_mask_req (unsigned char* mask, int maskLen, int * maskLenReq);
boolean msg_process_LSM_mask_req (unsigned char* mask, int maskLen, int * maskLenReq);
boolean diagpkt_mask_tbl_reg (uint32 client_id, uint32 LSM_sync_obj);
int wait_for_dci_reg_client_id(void);
int wait_for_dci_support(struct diag_dci_peripheral_list_t *ptr);
int wait_for_dci_health_stats(struct diag_dci_health_stats_proc *ptr);
int wait_for_dci_log_status(struct diag_log_event_stats *ptr);
int wait_for_dci_event_status(struct diag_log_event_stats *ptr);
int wait_for_query_real_time(struct real_time_query_t *ptr);
#endif
