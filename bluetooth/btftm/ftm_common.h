/*==========================================================================

                     FTM BT HCI PFAL Header File

Description
   Queue insert/delete routines and data structures

# Copyright (c) 2010-2011, 2014 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                         Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/10   rakeshk  Created
11/09/10   rakeshk  Added two APIs to perform read/write of BT Top level
                    I2C registers
===========================================================================*/

#define LOG_TAG "ftmdaemon"

#define PRI_INFO " I"
#define PRI_WARN " W"
#define PRI_ERROR " E"
#define PRI_DEBUG " D"
#define PRI_VERB " V"

#define ALOG(pri, tag, fmt, arg...) fprintf(stderr, tag pri ": " fmt"\n", ##arg)
#define ALOGV(fmt, arg...) ALOG(PRI_VERB, LOG_TAG, fmt, ##arg)
#define ALOGD(fmt, arg...) ALOG(PRI_DEBUG, LOG_TAG, fmt, ##arg)
#define ALOGI(fmt, arg...) ALOG(PRI_INFO, LOG_TAG, fmt, ##arg)
#define ALOGW(fmt, arg...) ALOG(PRI_WARN, LOG_TAG, fmt, ##arg)
#define ALOGE(fmt, arg...) ALOG(PRI_ERROR, LOG_TAG, fmt, ##arg)

#if defined(CONFIG_FTM_BT) || defined(CONFIG_FTM_FM)
#include <ftm_bt_common.h>
#include "ftm_bt.h"
#include <semaphore.h>
#include <pthread.h>
/* Semaphore shared by the Event handler and main thread */
extern sem_t semaphore_cmd_complete;
/* Structure used by the FTM BT/FM component to
 * queue the FTM packet contents
 */

pthread_mutex_t fm_event_lock;
pthread_cond_t fm_event_cond;
extern int fm_passthrough;

typedef struct cmdQ
{
  int command_id;/*Command id */
  void *data; /* Command data */
  boolean bt_command; /* whether BT or FM command */
  int cmd_len; /* Command length */
  struct cmdQ *next; /* pointer to next CmdQ item */
}cmdQ;

/* Callback declaration for BT FTM packet processing */
void *bt_ftm_diag_dispatch(void *req_pkt, uint16 pkt_len);

/*===========================================================================
FUNCTION   qinsert_cmd

DESCRIPTION
  Command Queue insert routine. Add the FTM BT packet to the Queue

DEPENDENCIES
  NIL

RETURN VALUE
  RETURNS FALSE without adding queue entry in failure
  to allocate a new Queue item
  else returns TRUE

SIDE EFFECTS
  increments the number of commands queued

===========================================================================*/
boolean qinsert_cmd(ftm_bt_pkt_type *ftm_bt_pkt);
/*===========================================================================
FUNCTION   dequeue_send

DESCRIPTION
  Command Queue delete and calls HCI send routine. Dequeues the HCI data from
  the queue and sends it to HCI HAL layer.

DEPENDENCIES
  NIL

RETURN VALUE
  RETURN NIL

SIDE EFFECTS
  decrements the number of command queued

===========================================================================*/
void dequeue_send();

/*===========================================================================
FUNCTION  i2c_write

DESCRIPTION
  Helper function to construct the I@C request to be sent to the FM I2C
  driver

DEPENDENCIES
  NIL

RETURN VALUE
  -1 in failure,positive or zero in success

SIDE EFFECTS
  None

===========================================================================*/
int i2c_write
(
int fd,
unsigned char offset,
const unsigned char* buf,
unsigned char len,
unsigned int slave_addr
);

/*===========================================================================
FUNCTION  i2c_read

DESCRIPTION
  Helper function to construct the I2C request to read data from the FM I2C
  driver

DEPENDENCIES
  NIL

RETURN VALUE
  -1 in failure,positive or zero in success

SIDE EFFECTS
  None

===========================================================================*/
int i2c_read
(
int fd,
unsigned char offset,
const unsigned char* buf,
unsigned char len,
unsigned int slave_addr
);
#endif
