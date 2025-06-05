/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Legacy Service Mapping layer implementation for Events

GENERAL DESCRIPTION
  Contains main implementation of Legacy Service Mapping layer for Diagnostic
  Event Services.

EXTERNALIZED FUNCTIONS
  event_report
  event_report_payload

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2011, 2014-2015, 2020-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who    what, where, why
--------   ---    ----------------------------------------------------------
10/01/08   SJ     Changes for CBSP2.0
05/01/08   JV     Added support to update the copy of event_mask in this process
                  during initialization and also on mask change
11/12/07   mad    Created

===========================================================================*/


/* ==========================================================================
   Include Files
========================================================================== */
#include "event.h"
#include "diag_lsm.h"
#include "diag_lsmi.h" /* for declaration of diag Handle */
#include "diagsvc_malloc.h"
#include "event_defs.h"
#include "diagdiag.h"
#include "eventi.h"
#include "diag_lsm_event_i.h"
#include "diag_shared_i.h" /* for definition of diag_data struct. */
#include "diag_lsm_comm.h"
#include "ts_linux.h"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "errno.h"
#include "stdio.h"
#include <memory.h>
#include <sys/time.h>
//#include <time.h>

#define EVENT_WRITE_WAIT_US	1
#define EVENT_WRITE_TIMEOUT_US	1000

/*Local Function declarations*/
static byte *event_alloc (event_id_enum_type id, uint8 payload_length, int* pAlloc_Len);

/*this keeps track of number of failures to IDiagPkt_Send().
This will currently be used only internally.*/
static unsigned int gEvent_commit_to_cs_fail = 0;
static unsigned char event_mask[EVENT_MASK_SIZE];

int gnDiag_LSM_Event_Initialized = 0;

#define DCI_EVENT_MASK_SIZE		512
unsigned char dci_cumulative_event_mask[DCI_EVENT_MASK_SIZE];
int num_dci_clients_event;

unsigned char enabled_event_mask_arr[EVENT_MASK_SIZE];

static int event_write(const void *buf, unsigned int length)
{
	int ret = 0;
	struct timeval start_tv;
	struct timeval current_tv;
	struct timeval wait_tv;
	unsigned long long start_time_us;
	unsigned long long current_time_us;
	unsigned long long elapsed_time_us = 0;

	gettimeofday(&start_tv, NULL);
	start_time_us = DIAG_SEC_TO_USEC(start_tv.tv_sec) + start_tv.tv_usec;
	while (elapsed_time_us < EVENT_WRITE_TIMEOUT_US) {
		wait_tv.tv_sec = 0;
		wait_tv.tv_usec = EVENT_WRITE_WAIT_US;
		ret = diag_lsm_comm_write(diag_fd, (unsigned char *)buf, length, MSG_DONTWAIT);
		if (!ret || (errno != EWOULDBLOCK))
			break;
		select(0, NULL, NULL, NULL, &wait_tv);
		gettimeofday(&current_tv, NULL);
		current_time_us = DIAG_SEC_TO_USEC(current_tv.tv_sec) + current_tv.tv_usec;

		/* In the case where time has wrapped around, skip any further commit attempt */
		if (current_time_us < start_time_us)
			break;
		elapsed_time_us = current_time_us - start_time_us;
	}

	return ret;
}

/*===========================================================================

FUNCTION event_update_mask

DESCRIPTION
  Update event mask structure as per data from tool

DEPENDENCIES
   None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void event_update_mask(unsigned char* ptr, int len)
{
	if (!ptr || len <= 0 || !gnDiag_LSM_Event_Initialized)
		return;

	if (len > EVENT_MASK_SIZE)
		len = EVENT_MASK_SIZE;

	memcpy(event_mask, ptr, len);

	memcpy(enabled_event_mask_arr, ptr, len);
}

/*===========================================================================

FUNCTION event_update_dci_mask

DESCRIPTION
  Update cumulative dci event mask as per data passed from dci in diag kernel

DEPENDENCIES
   None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void event_update_dci_mask(unsigned char* ptr, int len)
{
	if (!ptr || !gnDiag_LSM_Event_Initialized || len < (int)sizeof(int))
		return;

	num_dci_clients_event = *(int *)ptr;
	ptr += sizeof(int);
	len -= sizeof(int);
	if ( len > DCI_EVENT_MASK_SIZE)
		len = DCI_EVENT_MASK_SIZE;

	/* Populating dci event mask table */
	memcpy(dci_cumulative_event_mask, ptr, len);
}

/* Externalized functions */
/*===========================================================================

FUNCTION EVENT_REPORT

DESCRIPTION
  Report an event. Published Diag API.

DEPENDENCIES
   Diag Event service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void event_report (event_id_enum_type Event_Id)
{
   if(diag_fd != -1)
   {
      byte *pEvent;
      int Alloc_Len = 0;
      pEvent = event_alloc (Event_Id, 0, &Alloc_Len);
      if(pEvent)
      {
	     int NumberOfBytesWritten = 0;
		 NumberOfBytesWritten = event_write((const void *)pEvent, Alloc_Len);
		 if(NumberOfBytesWritten != 0)
	     {
			if (errno != EWOULDBLOCK)
				DIAG_LOGE("Diag_LSM_Event: Write failed in %s, bytes written: %d, error: %d\n",
					   __func__, NumberOfBytesWritten, errno);
            gEvent_commit_to_cs_fail++;
         }

         DiagSvc_Free(pEvent, GEN_SVC_ID);
      }

   }
   return;
}/* event_report */

/*===========================================================================

FUNCTION EVENT_REPORT_PAYLOAD

DESCRIPTION
  Report an event with payload data.

DEPENDENCIES
  Diag Event service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
event_report_payload (event_id_enum_type Event_Id, uint8 Length, void *pPayload)
{
	if (diag_fd != -1) {
		byte *pEvent = NULL;
		int Alloc_Len = 0;

		if (Length > 0 && pPayload) {
			pEvent = event_alloc (Event_Id, Length, &Alloc_Len);
			if (pEvent) {
				struct event_store_type* temp = (struct event_store_type*) (pEvent + FPOS(diag_data, rest_of_data));

				if (is_diagid_logging_format_selected()) {
					event_pkt_type *event_pkt = (event_pkt_type *) (pEvent +
							FPOS(diag_data, rest_of_data) + sizeof(event_diagid_head_type));

					if (Length <= 2)
						memcpy (&event_pkt->payload, pPayload, Length);
					else
						memcpy (EVENT_LARGE_PAYLOAD(event_pkt->payload.payload), pPayload, Length);
				} else {
					if (Length <= 2)
						memcpy (&temp->payload, pPayload, Length);// Dont need the length field if payload <= 2 bytes
					else
						memcpy (EVENT_LARGE_PAYLOAD(temp->payload.payload), pPayload, Length);
				}

				int NumberOfBytesWritten = 0;
				NumberOfBytesWritten = event_write((const void *)pEvent, Alloc_Len);
				if (NumberOfBytesWritten != 0) {
					if (errno != EWOULDBLOCK)
						DIAG_LOGE("Diag_LSM_Event: Write failed in %s, bytes written:%d, error:%d\n",
							__func__, NumberOfBytesWritten, errno);
					gEvent_commit_to_cs_fail++;
				}
				DiagSvc_Free(pEvent, GEN_SVC_ID);
			}
		} else {
			event_report (Event_Id);
		}
	}
	return;
}/* event_report_payload */


/* allocate diag-id based event packet */
static byte * alloc_diagid_based_event(event_id_enum_type id,
		uint8 payload_length, int* palloc_len, uint8 mask)
{
	event_diagid_head_type *new_event;
	event_pkt_type *event_pkt = NULL;
	diag_data* pdiag_data = NULL;
	uint32 alloc_len = 0;
	uint32 ts_lo, ts_hi;
	byte *pEvent = NULL;

	alloc_len = FPOS(diag_data, rest_of_data) + sizeof(event_diagid_head_type) +
			FPOS(event_pkt_type, payload.payload) + payload_length;

	pEvent = (byte *) DiagSvc_Malloc(alloc_len, GEN_SVC_ID);
	if (pEvent) {
		pdiag_data = (diag_data*) pEvent;

		pdiag_data->diag_data_type = 0;
		if (mask & EVENT_MASK)
			pdiag_data->diag_data_type |= DIAG_DATA_TYPE_EVENT;
		if (mask & DCI_EVENT_MASK)
			pdiag_data->diag_data_type |= DIAG_DATA_TYPE_DCI_EVENT;

		new_event = (event_diagid_head_type*) (pEvent + FPOS(diag_data, rest_of_data));

		memset((void*)new_event, 0, sizeof(event_diagid_head_type));

		new_event->cmd_code = 0x9F;
		new_event->version = NEW_EVENT_PKT_VERSION1;
		new_event->diag_id = diag_id;

		event_pkt = (event_pkt_type*) ((char*)new_event + sizeof(event_diagid_head_type));

		ts_get_lohi(&ts_lo, &ts_hi);
		event_pkt->ts_lo = ts_lo;
		event_pkt->ts_hi = ts_hi;
		event_pkt->event_id.id = id;
		event_pkt->event_id.time_trunc_flag = 0;

		if (payload_length <= 2) {
			/* ignore length field if payload size <= 2 */
			alloc_len--;
			new_event->length = FPOS(event_pkt_type, payload) + payload_length;
		} else {
			new_event->length = FPOS(event_pkt_type, payload.payload) + payload_length;
			event_pkt->payload.length = payload_length;
		}

		if (payload_length > 0x3)
			payload_length = 0x3;

		event_pkt->event_id.payload_len = payload_length;

		if (palloc_len)
			*palloc_len = alloc_len;
		else
			DIAG_LOGE("diag: %s: Error, null pointer\n", __func__);
	}

	return pEvent;
}

/*==========================================================================

FUNCTION event_alloc

DESCRIPTION
  This routine allocates an event item from the process heap and fills in
  the following information:

  Event ID
  Time stamp
  Payload length field

  //TODO :This routine also detects dropped events and handles the reporting of
  //dropped events.

RETURN VALUE
  A pointer to the allocated  event is returned.
  NULL if the event cannot be allocated.
  The memory should be freed by the calling function, using DiagSvc_Free().
  pAlloc_Len is an output value, indicating the number of bytes allocated.

===========================================================================*/
static byte *
event_alloc (event_id_enum_type id, uint8 payload_length, int* pAlloc_Len)
{
	byte *pEvent = NULL;
	int alloc_len = 0, header_length;
	boolean mask_set = 0;
	boolean dci_mask_set = 0;
	uint32 ts_lo, ts_hi;
	uint8 mask = 0;

	if (!gnDiag_LSM_Event_Initialized)
		return NULL;

	/* Verify that the event id is in the right range and that the
	 * corresponding bit is turned on in the event mask. */
	if (id <= EVENT_LAST_ID) {
		mask_set = EVENT_MASK_BIT_SET (id);
		if (num_dci_clients_event > 0) {
			dci_mask_set  = (dci_cumulative_event_mask[(id)/8] &
							(1 << ((id) & 0x07)));
		}
	}
	if (!mask_set && !dci_mask_set) {
		mask_set = 0;
		return NULL;
	}

	/* if diag id based logging enabled, use new event packet format */
	if (is_diagid_logging_format_selected()) {
		mask |= mask_set ? EVENT_MASK : 0x0;
		mask |= dci_mask_set ? DCI_EVENT_MASK : 0x0;
		return alloc_diagid_based_event(id, payload_length, pAlloc_Len, mask);
	}

	// Prototyping Diag 1.5 WM7: Adding a uint32 so the diag driver can identify this as an event.
	alloc_len =  FPOS(diag_data, rest_of_data) + FPOS (struct event_store_type, payload.payload) + payload_length ;
	pEvent = (byte *) DiagSvc_Malloc(alloc_len, GEN_SVC_ID);
	if (pEvent) {
		struct event_store_type* temp = NULL;
		diag_data* pdiag_data = (diag_data*) pEvent;

		//Prototyping Diag 1.5 WM7:Fill in the fact that this is an event.
		pdiag_data->diag_data_type = 0;
		if (mask_set)
			pdiag_data->diag_data_type |= DIAG_DATA_TYPE_EVENT;
		if (dci_mask_set)
			pdiag_data->diag_data_type |= DIAG_DATA_TYPE_DCI_EVENT;

		//Prototyping Diag 1.5 WM7:Advance the pointer to point to the event_store_type part
		temp = (struct event_store_type*) (pEvent + FPOS(diag_data, rest_of_data));

		if (pAlloc_Len)
			*pAlloc_Len = alloc_len;

		/**
		 * Taking the address of packed structure member may result in an unaligned
		 * pointer value, hence use different variable to get the time hi & lo then
		 * populate structure memeber.
		 */
		ts_get_lohi(&ts_lo, &ts_hi);
		temp->ts_lo = ts_lo;
		temp->ts_hi = ts_hi;
		temp->cmd_code = 96;
		temp->event_id.event_id_field.id = id;
		temp->event_id.event_id_field.time_trunc_flag = 0;
		header_length = sizeof(temp->event_id) + sizeof(temp->ts_lo) + sizeof(temp->ts_hi);

		if (payload_length <= 2) {
			alloc_len--;
			if (pAlloc_Len) {
				*pAlloc_Len = alloc_len;
			} else {
				DIAG_LOGE("event_alloc: Error, null pointer "
				"encountered for returning allocation "
				"length\n");
			}
			temp->length = header_length + payload_length;
		} else {
			// Add the payload length field only if payload more than 2 bytes
			temp->payload.length = payload_length;
			//adding the payload length field
			temp->length = header_length + sizeof(temp->payload.length) + payload_length;
		}

		if (payload_length > 0x3)
			payload_length = 0x3;

		temp->event_id.event_id_field.payload_len = payload_length;
	}

	return pEvent;
} /* event_alloc */

/*===========================================================================
  FUNCTION event_clear_mask

  DESCRIPTION
    Clears the local copy of the event masks

===========================================================================*/
void event_clear_mask(void)
{
	/* Only works while event_mask is statically allocated and not malloced */
	memset(event_mask, 0, sizeof(event_mask));
}

/*===========================================================================
  FUNCTION event_clear_dci_mask

  DESCRIPTION
    Clears the local copy of the dci event masks

===========================================================================*/
void event_clear_dci_mask(void)
{
	num_dci_clients_event = 0;
	memset(dci_cumulative_event_mask, 0, DCI_EVENT_MASK_SIZE);
}

 /*===========================================================================

FUNCTION Diag_LSM_Event_Init

DESCRIPTION
  Initializes the event service

RETURN VALUE
  boolean indicating success

SIDE EFFECTS
  None.
===========================================================================*/
boolean Diag_LSM_Event_Init(void)
{
	boolean status = TRUE;
	if(!gnDiag_LSM_Event_Initialized)
	{
		num_dci_clients_event = 0;
		memset(dci_cumulative_event_mask, 0, DCI_EVENT_MASK_SIZE);
		gnDiag_LSM_Event_Initialized = TRUE;
	}
	return status;

} /* Diag_LSM_Event_Init */

/*===========================================================================

FUNCTION Diag_LSM_Event_DeInit

DESCRIPTION
  Deinitializes the event service


SIDE EFFECTS
  None.
===========================================================================*/
void Diag_LSM_Event_DeInit(void)
{
	gnDiag_LSM_Event_Initialized = FALSE;

} /* Diag_LSM_Event_Init */

