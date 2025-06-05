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


