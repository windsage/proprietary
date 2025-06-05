/******************************************************************************
  @file  VendorIGlue.cpp
  @brief  VendorIGlue glues modules to the framework which needs perf events

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "PerfGlueLayer.h"
#include "VendorIGlue.h"

VendorIGlue::VendorIGlue(const char *libname, int32_t *events, int32_t numevents) {
    PerfGlueLayer(libname, events, numevents);
}

VendorIGlue::~VendorIGlue() {
}