/*==========================================================================

                     FTM UWB Task Header File

Description
  Global Data declarations of the ftm uwb component.

# Copyright (c) 2010-2011, 2013-2014, 2023 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifdef  CONFIG_FTM_UWB

#include "log.h"
#include "diagpkt.h"
#include <sys/types.h>



/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

#define FTM_UWB_CMD_CODE            6            /* UWB FTM Command code */

/* FTM (UWB) PKT Header */
typedef PACKED struct
{
  word cmd_id;            /* command id (required) */
  word cmd_data_len;      /* request pkt data length, excluding the diag and ftm headers
                             (optional, set to 0 if not used)*/
  word cmd_rsp_pkt_size;  /* rsp pkt size, size of response pkt if different then req pkt
                             (optional, set to 0 if not used)*/
} ftm_uwb_cmd_header_type;

/* uwb FTM packet */
typedef PACKED struct
{
  diagpkt_subsys_header_type diag_hdr;
  ftm_uwb_cmd_header_type    ftm_hdr;
  byte                       data[1];
} ftm_uwb_pkt_type;


/*=======================================================================================
FUNCTION   uwb_dequeue_send

DESCRIPTION
  This function extracts command to be sent from uwb cmd queue and invokes
  ftm_uwb_dispatch() to send the command further to uwb chip.
DEPENDENCIES
  NIL

RETURN VALUE
  RETURN NIL

SIDE EFFECTS
  decrements the number of command queued

=========================================================================================*/
void uwb_dequeue_send(void);

/*========================================================================================
FUNCTION   uwb_qinsert_cmd

DESCRIPTION
  Uwb command queue insert routine. Adds uwb cmd packet in the uwb cmd queue.

DEPENDENCIES
  NIL

RETURN VALUE
  RETURNS FALSE if fails to allocate a new Queue item else returns TRUE.

SIDE EFFECTS
  increments the number of commands queued.

==========================================================================================*/
boolean uwb_qinsert_cmd(ftm_uwb_pkt_type *ftm_uwb_pkt);

/*===========================================================================================
FUNCTION   ftm_uwb_err_timedout

DESCRIPTION
  This function will be triggered when there is no response of the command sent to uwb chip.
  It will deinit the uwb transport and will close the chip. it will also inform the failure
  to diag.

DEPENDENCIES
  NIL

RETURN VALUE
  RETURN NIL

SIDE EFFECTS
  NONE

===============================================================================================*/
void ftm_uwb_err_timedout();
#endif /* CONFIG_FTM_UWB */
