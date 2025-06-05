/********************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/
#ifndef __LISTENERMNGR_H
#define __LISTENERMNGR_H

#include <stdint.h>
#include "object.h"
#include <BufferAllocator/BufferAllocator.h>


/* List of services buffer length*/
#define FILE_SERVICE_BUF_LEN  20*1024
#define TIME_SERVICE_BUF_LEN  20*1024
#define RPMB_SERVICE_BUF_LEN  25*1024
#define SSD_SERVICE_BUF_LEN  20*1024
#define SECURE_UI_SERVICE_BUF_LEN 25*1024
#define GPFILE_SERVICE_BUF_LEN  (504*1024)
#define QIS_SERVICE_BUF_LEN  500   /*secure ui to be included here*/
#define SP_SERVICE_BUF_LEN  1024
#define OPS_SERVICE_BUF_LEN  25*1024
#define GPREQCANCEL_SERVICE_BUF_LEN  20*1024
#define GPT_SERVICE_BUF_LEN 256*1024
/* End of list */

/* List of services id's */
#define FILE_SERVICE  10
#define TIME_SERVICE  11
#define RPMB_SERVICE  0x2000
#define GPT_SERVICE 0x2001
#define SSD_SERVICE  0x3000
#define SECURE_UI_SERVICE  0x4000
#define ML_SERVICE  0x5000 //Reserving the number here to avoid future conflicts. The listener will be started from the ML daemon.
#define MDTP_SERVICE  0x6000
#define GPFILE_SERVICE  0x7000
#define TLOC_SERVICE  0x8000 //TLOC Daemon listener service ID
#define QIS_SERVICE  0x9000 // QSEE Interrupt Service ID
#define SECCAM_SERVICE  0xA000   // Secure camera listener is implemented as a stand alone daemon
#define SP_SERVICE  0xB000   // Secure Processor (SP) listener service ID
#define OPS_SERVICE_ID  0xC000 // OPS listener
#define GPREQCANCEL_SERVICE  0x1100 // GP Request Cancellation Service ID
/* End of list */

typedef int (*serviceInitialize)();

struct dmaBufManager {
  int32_t dmaFd;
  BufferAllocator *dmaBufAllocator;
};

struct listenerServices{
  char *serviceName; //name of the listener service
  int id; //id of the listener service
  int isRegistered; //status listener has been registered or not
  char *fileName; //file_name of the listener service
  char *smciDispatch; //smcinvoke based dispatch function of the listener
  char *fileInit; //file init of listener
  size_t listenerLen; //buffer length for listener service
  Object oListener; //store cbo of listener serice
  void *libHandle; //libHandle for the listener
  struct dmaBufManager dmaBufMgr; //store dmaFd and dmaBufAllocator object
};

#endif //__LISTENERMNGR_H
