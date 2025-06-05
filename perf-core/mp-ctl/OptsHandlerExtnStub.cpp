/******************************************************************************
  @file    OptsHandlerExtnStub.cpp
  @brief   Implementation of performance server module extn empty stubs

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017,2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "OptsHandlerExtn.h"

int32_t init_pasr() {
    return FAILED;
}

int32_t pasr_entry_func(Resource &r, OptsData &d) {
    return SUCCESS;
}

int32_t pasr_exit_func(Resource &r, OptsData &d) {
    return SUCCESS;
}

int32_t init_nr_thread() {
    return FAILED;
}

float get_display_fps() {
    return 60.0f;
}


bool set_fps_file(float fps) {
    return false;
}

float get_fps_file() {
    return 60.0f;
}

bool is_no_root_alive() {
    return false;
}

EventQueue *get_nr_queue() {
    return NULL;
}

int32_t post_boot_init_completed() {
    return SUCCESS;
}
