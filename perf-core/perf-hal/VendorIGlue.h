/******************************************************************************
  @file  VendorIGlue.h
  @brief  Vendor Perf Hal glue layer interface

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PERF_VENDORIGLUE_H__
#define __PERF_VENDORIGLUE_H__

class VendorIGlue {

public:
    VendorIGlue();

    //events need to be passed as an array, first two elements should contain range
    //of the events interested in (if not interested in range, -1, -1 should be specified),
    //next elements should contain individual events, max is limited by MAX_EVENTS
    //numevents should be first two elements (range) + number of next elements
    VendorIGlue(const char *libname, int32_t *events, int32_t numevents);

    ~VendorIGlue();

};

#endif