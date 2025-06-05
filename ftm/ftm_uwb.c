/*==========================================================================

                     FTM UWB Task Source File

Description
  FTM state machine and platform independent routines for UWB

# Copyright (c) 2010-2021, 2023 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifdef  CONFIG_FTM_UWB

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "ftm_dbg.h"
#ifndef ANDROID
#include <stdbool.h>
#endif
#include <string.h>
#include <ftm_uwb.h>
#include <uwb_ftm_client.h>


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define UNUSED(x) (void)(x)


/* uwb msg types */
#define UCI_MT_DATA  0x00
#define UCI_MT_CMD   0x20
#define UCI_MT_RSP   0x40
#define UCI_MT_NTF   0x60

#define UCI_MT_MASK   0xE0
#define UCI_MT_OFFSET 0x00

/* Reader thread handle */
pthread_t uwb_ftm_thread_handle;

typedef struct uwb_cmdq
{
  void *data; /* Command data */
  int cmd_len; /* Command length */
  struct uwb_cmdq *next; /* pointer to next uwb_cmdq item */
}uwb_cmdq;

typedef enum
{
  FTM_UWB_DRV_NO_ERR = 0,
  FTM_UWB_DRV_NO_SOC_RSP_TOUT,
  FTM_UWB_DRV_UNKNOWN_ERR
} ftm_uwb_drv_err_state_type;

/* FTM Log Packet - Used to send back response from uwb */
typedef PACKED struct
{
  log_hdr_type hdr;
  uint32_t version;
  byte     data[1];         /* Variable length payload, look at FTM log id for contents */
} ftm_uwb_log_pkt_type;

/* uwb_request_status - enum to encapuslate the status of a HAL request*/
typedef enum uwb_request_status
{
  UWB_STATUS_SUCCESS,
  UWB_STATUS_FAIL,
  UWB_STATUS_NO_RESOURCES,
  UWB_STATUS_SHORT_WRITE,
  UWB_STATUS_SHORT_READ
}uwb_request_status;

#define FTM_UWB_LOG_HEADER_SIZE (sizeof(ftm_uwb_log_pkt_type) - 1)



/* FTM status log size*/
const uint8 uwblogsize = 2;

/* Command Queue front pointer */
uwb_cmdq *uwbfront = NULL;
/* Command Queue rear pointer */
uwb_cmdq *uwbrear = NULL;
/* cmd count for unprocessed cmds in queue */
uint32 uwb_num_pending_cmds = 0;
static bool uwb_chip_opened = false;

/* Pipe file descriptors for cancelling read operation */
int uwbpipefd[2] = {0,0};
/* Transport file descriptor */
int uwb_fd_transport = 0;

/* UWB user Cmd timed out Log Packet */
const uint8 uwb_event_buf_user_cmd_timeout[2] = {0x0f,FTM_UWB_DRV_NO_SOC_RSP_TOUT};
/* UWB user Cmd Unknown error Log Packet */
const uint8 uwb_event_buf_user_cmd_unknown_err[2] = {0x0f,FTM_UWB_DRV_UNKNOWN_ERR};


void *ftm_uwb_readerthread(void *ptr);

extern sem_t semaphore_uwb_cmd_complete;

/*=================================================================================
FUNCTION   ftm_figure_out_the_log_code_of_the_packet_received

DESCRIPTION
  This function decides log code to be sent to diag based on reponse or notification
  from uwb chip.

DEPENDENCIES
  Response or notification coming from uwb

RETURN VALUE
  Correct log code to be sent to diag.

SIDE EFFECTS
  None

===================================================================================*/
int ftm_figure_out_the_log_code_of_the_packet_received(const uint8 *pEventBuf)
{
  int log_code = 0x00;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);

  if((pEventBuf[UCI_MT_OFFSET] & UCI_MT_MASK) == UCI_MT_RSP)
  {
    log_code = LOG_UWB_UCI_RX_CTRL_C;
  }
  else if((pEventBuf[UCI_MT_OFFSET] & UCI_MT_MASK) == UCI_MT_NTF)
  {
    log_code = LOG_UWB_UCI_RX_NTF_C;
  }
  else if((pEventBuf[UCI_MT_OFFSET] & UCI_MT_MASK) == UCI_MT_DATA)
  {
    log_code = LOG_UWB_UCI_RX_DATA_C;
  }
  else
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: Error in getting message type\n", __func__);
  }
  return log_code;
}

/*=================================================================================
FUNCTION   is_uwb_snoop_log_enabled

DESCRIPTION
  This function reads a persist property set by user to decide if snoop logging is
  enabled or not. If snoop logging is enabled then it means that all uwb responses
  as well as cmds are being registered to QXDM via diag.In this case , uwb responses
  or notifications will not be sent to diag from uwb ftm.For sending uwb responses to
  tools other than QXDM, this property must be set false.

DEPENDENCIES
  NIL

RETURN VALUE
 True if property is set true otherwise false.

SIDE EFFECTS
  None

===================================================================================*/
bool is_uwb_snoop_log_enabled (void)
{
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  property_get("persist.vendor.service.uwb.snooplog", value, "false");
  return (strcmp(value, "true") == 0);
}

/*====================================================================================
FUNCTION   ftm_send_uwb_rsp_back_to_diag

DESCRIPTION
  Processes uwb response/notification and sends it to the libdiag to be sent to
  the test tool further.

DEPENDENCIES
  Response buffer from uwb chip and its length.

RETURN VALUE
  NIL

SIDE EFFECTS
  None

======================================================================================*/
void ftm_send_uwb_rsp_back_to_diag(const uint8 *pEventBuf, int event_bytes)
{
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
  int uwb_log_code = 0;

  ftm_uwb_log_pkt_type*  ftm_uwb_log_pkt_ptr = NULL;
  if((pEventBuf == NULL) || (event_bytes == 0))
  {
    return;
  }

  DPRINTF(FTM_DBG_TRACE,"RCV : ");
  for( int i = 0; i<event_bytes; i++)
  {
    DPRINTF(FTM_DBG_TRACE,"%02X ", pEventBuf[i]);
  }
  DPRINTF(FTM_DBG_TRACE,"\n");

  uwb_log_code = ftm_figure_out_the_log_code_of_the_packet_received(pEventBuf);
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: diag_log_code:0x%x\n", __func__,uwb_log_code);
  if(log_status(uwb_log_code) && (!is_uwb_snoop_log_enabled()))
  {
    ftm_uwb_log_pkt_ptr = (ftm_uwb_log_pkt_type *)log_alloc(uwb_log_code , FTM_UWB_LOG_HEADER_SIZE+event_bytes );
  }
  else
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:UWB RX log code is not enabled for logging\n", __func__);
  }

  if(ftm_uwb_log_pkt_ptr != NULL)
  {
    ftm_uwb_log_pkt_ptr->version = 0;
    memcpy((void *)ftm_uwb_log_pkt_ptr->data,(void *)pEventBuf, event_bytes);
    log_commit( ftm_uwb_log_pkt_ptr );
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: packet sent to diag\n", __func__);
  }
  else
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:Dropping packet\n", __func__);
    return;
  }
}

/*===========================================================================
FUNCTION   ftm_uwb_init_transport

DESCRIPTION
  Routine to intialise the uwb client.

DEPENDENCIES
  NIL

RETURN VALUE
  UWB_STATUS_SUCCESS if SUCCESS, else other reasons.

SIDE EFFECTS
  None

===========================================================================*/
uwb_request_status ftm_uwb_init_transport(void)
{
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter uwb_fd_transport:%d\n", __func__,uwb_fd_transport);

  if (uwb_ftm_create_socketpair(&uwb_fd_transport) == false) {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: Socket pair creation failed\n", __func__);
      return UWB_STATUS_NO_RESOURCES;
  }
  else
  {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: Socket pair created with client fd:%d: \n", __func__, uwb_fd_transport);
      if(uwb_ftm_client_initialize())
      {
          DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: uwb chip is open\n", __func__);
      }
      else{
          DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: uwb chip failed\n", __func__);
          return UWB_STATUS_FAIL;
      }
  }

  if (pipe(uwbpipefd) == -1)
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: pipe create error \n");
    return UWB_STATUS_FAIL;
  }
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:creating reader thread\n", __func__);
  /* Creating uwb ftm read thread which listens for various read requests from uwb ftm client */
  pthread_create( &uwb_ftm_thread_handle, NULL, ftm_uwb_readerthread, NULL);

  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit uwb_fd_transport:%d\n", __func__,uwb_fd_transport);
  return UWB_STATUS_SUCCESS;
}

/*===============================================================================
FUNCTION   uwb_qinsert_cmd

DESCRIPTION
  Uwb command queue insert routine. Adds uwb cmd packet in the uwb cmd queue.

DEPENDENCIES
  NIL

RETURN VALUE
  RETURNS FALSE if fails to allocate a new Queue item else returns TRUE.

SIDE EFFECTS
  increments the number of commands queued.

================================================================================*/
boolean uwb_qinsert_cmd(ftm_uwb_pkt_type *ftm_uwb_pkt)
{
  uwb_cmdq *newitem;
  bool status = FALSE;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: qinsert_cmd > uwbrear = 0x%x uwbfront = 0x%x\n",
        (unsigned int)uwbrear,(unsigned int)uwbfront);
#endif

  newitem = (uwb_cmdq*)malloc(sizeof(uwb_cmdq));
  if(newitem == NULL)
  {
    ftm_send_uwb_rsp_back_to_diag(&uwb_event_buf_user_cmd_unknown_err[0],uwblogsize);
     return status;
  }
  newitem->next=NULL;
  newitem->data = (void *)malloc(ftm_uwb_pkt->ftm_hdr.cmd_data_len);
  if(newitem->data == NULL)
  {
    free(newitem);
    ftm_send_uwb_rsp_back_to_diag(&uwb_event_buf_user_cmd_unknown_err[0],uwblogsize);
    return status;
  }
  /* Copy the data into the queue buffer */
  memcpy(newitem->data,(void*)ftm_uwb_pkt->data, ftm_uwb_pkt->ftm_hdr.cmd_data_len);

  newitem->cmd_len = ftm_uwb_pkt->ftm_hdr.cmd_data_len;

  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: newitem->cmd_len=%d\n", __func__,newitem->cmd_len);
  if(newitem->cmd_len != 0)
  {
     DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: data being put in queue is\n", __func__);
     unsigned char *value = newitem->data;
     DPRINTF(FTM_DBG_TRACE,"STORE : ");
     for (int i = 0; i < newitem->cmd_len; i++)
     {
       DPRINTF(FTM_DBG_TRACE,"%02X ", value[i]);
     }
   }
    DPRINTF(FTM_DBG_TRACE,"\n");

  if(uwbfront==NULL && uwbrear==NULL)
  {
    uwbfront=newitem;
    uwbrear=newitem;
  }
  else
  {
    (uwbrear)->next=newitem;
    uwbrear=newitem;
  }
  uwb_num_pending_cmds++;
#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: qinsert_cmd < uwbrear = 0x%x uwbfront = 0x%x\n",
        (unsigned int)uwbrear,(unsigned int)uwbfront);
#endif
printf("%s:exit\n", __func__);
  status = TRUE;
  return status;
}

/*===========================================================================
FUNCTION   ftm_uwb_hal_nwrite

DESCRIPTION
  This function sends the uwb command to uwb ftm client which then will semd
  this command further to uwb chip over uwb aidl hal.

DEPENDENCIES
  NIL

RETURN VALUE
  FALSE if fails to send the command else TRUE.

SIDE EFFECTS
  None

===========================================================================*/
boolean ftm_uwb_hal_nwrite (uint8 *buf, int size)
{
  int tx_bytes = 0, nwrite,nwrite_len=0;
  int i = 0, buf_size = size;
  int buf_len = 0;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter  buf_size:%d\n", __func__,buf_size);

  if(uwb_fd_transport == -1)
  {
    return FALSE;
  }

  do
  {
    buf_len = size - tx_bytes;

    nwrite_len = write(uwb_fd_transport, &buf_len, sizeof(buf_len));
    nwrite = write(uwb_fd_transport, (buf + tx_bytes), (size - tx_bytes));
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: nwrite:%d\n", nwrite);
    DPRINTF(FTM_DBG_TRACE,"SEND : ");
    for (i = 0; i < buf_size; i++)
    {
      DPRINTF(FTM_DBG_TRACE,"%02X ", buf[i]);
    }
    DPRINTF(FTM_DBG_TRACE,"\n");

    if (nwrite < 0)
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: Error while writing \n");
      return UWB_STATUS_SHORT_WRITE;
    }
    if (nwrite == 0)
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: zero-length write \n");
      return UWB_STATUS_SHORT_WRITE;
    }

    tx_bytes += nwrite;
    size     -= nwrite;
  } while (tx_bytes < size);

  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
  return UWB_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION   ftm_uwb_send_cmd

DESCRIPTION
  This function initializes the uwb transport by opening the uwb chip if it is
  not open.If the uwb chip opens, then it invokes ftm_uwb_hal_nwrite() to send
  uwb command to the uwb chip.

DEPENDENCIES
  Command buffer to be sent and its length.

RETURN VALUE
  FALSE if fails to send the command else TRUE.

SIDE EFFECTS
  None

===========================================================================*/
boolean ftm_uwb_send_cmd(uint8 * cmd_buf, uint16 cmd_len)
{
  uwb_request_status ret = 0;
  boolean status = FALSE;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter cmd_len:%d\n", __func__,cmd_len);
  if(NULL == cmd_buf)
  {
    return status;
  }

  if(!uwb_chip_opened)
  {
    if(ftm_uwb_init_transport() == UWB_STATUS_SUCCESS)
    {
      uwb_chip_opened = true;
    }
    else
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:chip open failed\n", __func__);
    }
  }
  if(uwb_chip_opened)
  {
    ret = ftm_uwb_hal_nwrite((uint8 *) cmd_buf, cmd_len);
    if(ret == UWB_STATUS_SUCCESS)
      status = TRUE;
  }
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
  return status;
}

/*===========================================================================
FUNCTION   ftm_uwb_dispatch

DESCRIPTION
  This function gets the command from uwb command queue and sends that to
  ftm_uwb_send_cmd() to be processed further. It also sends the failure response
  back to diag in case command could not be sent.

DEPENDENCIES
  Command buffer and its length.

RETURN VALUE
  NIL in case uwb command has been sent sucessfully. Otherwise it sends the error
  message to diag to inform that command could not be sent.

SIDE EFFECTS
  None

===========================================================================*/
void ftm_uwb_dispatch(void *ftm_uwb_pkt ,int cmd_len )
{
  int ret;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: cmd_len=%d cmd is\n", __func__,cmd_len);

#ifdef FTM_DEBUG
  for (int i =0; i<cmd_len;i++)
  {
    DPRINTF(FTM_DBG_TRACE,"%X ",*((uint8*)ftm_uwb_pkt+i));
  }
#endif

  ret = ftm_uwb_send_cmd((uint8 *) ftm_uwb_pkt, cmd_len);
  if (ret != TRUE)
  {
    ftm_send_uwb_rsp_back_to_diag(&uwb_event_buf_user_cmd_unknown_err[0],uwblogsize);
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: Error->Send FTM command failed:: %d\n", ret);
  }

  sem_post(&semaphore_uwb_cmd_complete);
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
  return ;
}

/*===========================================================================
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

===========================================================================*/
void uwb_dequeue_send(void)
{
  uwb_cmdq *delitem;      /* Node to be deleted */
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: dequeue_send > uwbrear = 0x%x uwbfront = 0x%x\n",
    (unsigned int)uwbrear,(unsigned int)uwbfront);
#endif
  if((uwbfront==NULL) && (uwbrear==NULL))
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: Queue is empty to delete any element \n");
  }
  else
  {
    delitem=uwbfront;
    if(delitem)
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: delitem->cmd_len=%d\n", __func__,delitem->cmd_len);
      if(delitem->cmd_len != 0)
      {
        DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: data being data retreived from q is\n", __func__);
        unsigned char *value = delitem->data;
        DPRINTF(FTM_DBG_TRACE,"RETREIVE : ");
        for (int i = 0; i < delitem->cmd_len; i++)
        {
          DPRINTF(FTM_DBG_TRACE,"%02X ", value[i]);
        }
        DPRINTF(FTM_DBG_TRACE,"\n");
      }

      ftm_uwb_dispatch(delitem->data,delitem->cmd_len);
      uwbfront=uwbfront->next;
      if(uwbfront == NULL)
      {
        uwbrear = NULL;
        uwb_num_pending_cmds = 0;
      }
      free(delitem->data);
      free(delitem);
      if(uwb_num_pending_cmds > 0)
      {
        uwb_num_pending_cmds--;
      }
    }
  }
#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: dequeue_send < uwbrear = 0x%x uwbfront = 0x%x\n",
         (unsigned int)uwbrear,(unsigned int)uwbfront);
#endif
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
}

/*================================================================================
FUNCTION   cleanup_pending_uwb_cmd_queue

DESCRIPTION
  This function deletes uwb cmd queue entries and frees memory when uwb transport
  deinit happens.

DEPENDENCIES
  NIL

RETURN VALUE
  RETURN NIL

SIDE EFFECTS
  NONE

===================================================================================*/
void cleanup_pending_uwb_cmd_queue(void)
{
  uwb_cmdq *delitem;      /* Node to be deleted */
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: cleanup_pending_cmd_queue > uwbrear = 0x%x uwbfront = 0x%x\n",
         (unsigned int)uwbrear,(unsigned int)uwbfront);
#endif
  if((uwbfront==NULL) && (uwbrear==NULL))
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: \n Uwb queue is empty to delete any element");
  }
  else
  {
    while(uwbfront != NULL)
    {
      delitem=uwbfront;
      uwbfront=uwbfront->next;
      if(uwbfront == NULL)
        uwbrear = NULL;
      free(delitem->data);
      free(delitem);
    }
  }
#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: cleanup_pending_cmd_queue < uwbrear = 0x%x uwbfront = 0x%x\n",
        (unsigned int)uwbrear,(unsigned int)uwbfront);
#endif
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
}

/*======================================================================================
FUNCTION   ftm_uwb_hal_nread

DESCRIPTION
  This function reads given length of bytes from uwb client which is response of
  previous cmd sent to uwb chip.This function is called 2 times from ftm_uwb_read_event(),
  once to read the length of the response and next time to reads the actual response.

DEPENDENCIES
  NIL

RETURN VALUE
  UWB_STATUS_SUCCESS if SUCCESS, else other reasons.

SIDE EFFECTS
  None

=========================================================================================*/
uwb_request_status ftm_uwb_hal_nread(uint8 *uwb_event_read_buff, int size)
{
  int rx_bytes = 0, nread;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
  if(uwb_fd_transport < 0)
    return UWB_STATUS_NO_RESOURCES;

  nread = read(uwb_fd_transport, (uwb_event_read_buff + rx_bytes), (size - rx_bytes));
  if (nread < 0)
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: Error while reading \n");
    return UWB_STATUS_SHORT_READ;
  }

#ifdef FTM_DEBUG
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: rsp is\n", __func__);
  for (int i =0; i<size;i++)
  {
    DPRINTF(FTM_DBG_TRACE,"%02X ",uwb_event_read_buff[i]);
  }
#endif

  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
  return UWB_STATUS_SUCCESS;
}

/*=====================================================================================
FUNCTION   ftm_uwb_read_event

DESCRIPTION
  This function reads the response from uwb ftm client.It reads first the length of
  the response and then response itself.It is invoked from ftm_uwb_handle_read_event().

DEPENDENCIES
  NIL

RETURN VALUE
  FALSE if fails to read the response otherwise TRUE.

SIDE EFFECTS
  None

=======================================================================================*/
boolean ftm_uwb_read_event (void)
{
  boolean status = FALSE;
  uwb_request_status   rx_status;
  unsigned int size = 0;
  uint8 *uwb_event_read_buff = NULL;
  int nread = 0;

  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);

  nread = read(uwb_fd_transport, &size, sizeof(size));
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:size to read = %d\n", __func__, size);

  if (nread < 0)
  {
    DPRINTF(FTM_DBG_TRACE,"UWB FTM: Error while reading  %s (%d)\n", strerror(errno), errno);
    return UWB_STATUS_SHORT_READ;
  }

  if (size > 0)
  {
    uwb_event_read_buff = (uint8*)malloc(size);

    if(uwb_event_read_buff == NULL)
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: Mem allocation failed \n", __func__);
      return status;
    }

    rx_status = ftm_uwb_hal_nread(uwb_event_read_buff, size);
    if (rx_status == UWB_STATUS_SHORT_READ)
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: ftm_uwb_read_event: VERY SHORT READ! \n");
      free(uwb_event_read_buff);
      return status;
    }
    else
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s: Send back rsp to diag : %d\n", __func__, size);
      ftm_send_uwb_rsp_back_to_diag(uwb_event_read_buff, size);
      status = TRUE;
    }
    free(uwb_event_read_buff);
  }
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
  return status;
}

/*=====================================================================================
FUNCTION   ftm_uwb_read_event

DESCRIPTION
  This function will be invoked by the ftm_uwb_readerthread() to read the uwb response
  from uwb ftm client.

DEPENDENCIES
  NIL

RETURN VALUE
  FALSE if fails to read the response otherwise TRUE.

SIDE EFFECTS
  None

========================================================================================*/
boolean ftm_uwb_handle_read_event (void)
{
  boolean status = TRUE;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
  if(ftm_uwb_read_event() != TRUE)
  {
      return FALSE;
  }
  sem_post(&semaphore_uwb_cmd_complete);
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
  return status;
}

/*=====================================================================================
FUNCTION   ftm_uwb_readerthread

DESCRIPTION
  Thread Routine to perfom asynchrounous reading of responses coming from uwn ftm client
  descriptor. This descriptor to read becomes available when there is any response from
  uwb chip.It invokes ftm_uwb_handle_read_event() to read response/notification bytes.

DEPENDENCIES
  The LifeTime of ReaderThraad is dependent on the status returned by the call to
  ftm_uwb_handle_read_event(). other thans this, if chip cmd time out happens then
  also this thread will exit.

RETURN VALUE
  RETURN NIL

SIDE EFFECTS
  None

============================================================================================*/
void *ftm_uwb_readerthread(void *ptr)
{
  UNUSED(ptr);
  boolean status = FALSE;
  int retval;
  fd_set readfds;
  int buf;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter uwb_fd_transport:%d\n", __func__,uwb_fd_transport);

  do
  {
    FD_ZERO(&readfds);
    FD_SET(uwb_fd_transport, &readfds);
    FD_SET(uwbpipefd[0],&readfds);
    retval = select(uwb_fd_transport+1,&readfds, NULL, NULL, NULL);
    if(retval == -1)
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb select failed \n");
      break;
    }

    if(FD_ISSET(uwbpipefd[0],&readfds))
    {
       DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb pipe descriptor set \n");
       read(uwbpipefd[0],&buf,1);
       if(buf == 1)
         break;
    }
    if(FD_ISSET(uwb_fd_transport,&readfds))
    {
      DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb read descriptor set \n");
      status = ftm_uwb_handle_read_event();
      if(TRUE != status)
        break;
    }
  }
  while(1);
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb reader thread exited");
  return 0;
}

/*===========================================================================
FUNCTION   ftm_uwb_hal_deinit_transport

DESCRIPTION
  This function will deinitialise the uwb transport and will invoke chip close
  when there is no response from uwb chip.

DEPENDENCIES
  NIL

RETURN VALUE
  RETURN NIL

SIDE EFFECTS
  NONE

===========================================================================*/
void ftm_uwb_hal_deinit_transport(void)
{
  int buf = 1;
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
  write(uwbpipefd[1],&buf,1);
  uwb_ftm_client_close();
  uwb_fd_transport = -1;
  uwb_chip_opened = false;
}

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
void ftm_uwb_err_timedout(void)
{
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:enter\n", __func__);
  ftm_uwb_hal_deinit_transport();

  DPRINTF(FTM_DBG_TRACE,"UWB FTM: Uwb timed out \n");

  cleanup_pending_uwb_cmd_queue();
  ftm_send_uwb_rsp_back_to_diag(&uwb_event_buf_user_cmd_timeout[0],uwblogsize);
  DPRINTF(FTM_DBG_TRACE,"UWB FTM: %s:exit\n", __func__);
}
#endif /* CONFIG_FTM_UWB */
