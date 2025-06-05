/******************************************************************************
  @file    OptsHandlerExtn.h
  @brief   Implementation of performance server module extn

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017,2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERFCONTROLLER_EXTN_H__
#define __PERFCONTROLLER_EXTN_H__
#include "ResourceInfo.h"
#include "OptsData.h"
#include "EventQueue.h"

#define PASR_SERVICE_NAME "pasrhal"
#define NSEC_TO_SEC (1000000000l)
#define MEM_OFFLINE_NODE "/sys/kernel/mem-offline"
#define MPCTL_NO_ROOT_NAME "PERFD-EW-DISP"



typedef struct noroot_args {
    int32_t val;
    int32_t retval;
} noroot_args;

int32_t init_pasr();
int32_t pasr_entry_func(Resource &r, OptsData &d);
int32_t pasr_exit_func(Resource &r, OptsData &d);
int32_t init_nr_thread();
float get_display_fps();
bool set_fps_file(float fps);
float get_fps_file();
bool is_no_root_alive();
EventQueue *get_nr_queue();
int32_t post_boot_init_completed();
#endif //__PERFCONTROLLER_EXTN_H__
