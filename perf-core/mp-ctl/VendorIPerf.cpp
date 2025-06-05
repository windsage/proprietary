/******************************************************************************
  @file    VendorIPerf.cpp
  @brief   Implementation of vendor internal interface

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "VendorIPerf.h"
#include "TargetConfig.h"
#include "client.h"
#include "PerfLog.h"

VendorIPerf VendorIPerf::mCurrVendor;

extern "C" VendorIPerf* getVendorIPerf() {
  return &VendorIPerf::getVendorIPerf();
}

VendorIPerf& VendorIPerf::getVendorIPerf() {
    return mCurrVendor;
}

int32_t VendorIPerf::GetSysNode(const int32_t /*idx_value*/, char * /*node_path*/) {
    return 0;
}

float VendorIPerf::GetVendorFpsFile() {
    return 0;
}

VendorIPerf::VendorIPerf() {
  mMaxArgsPerReq = TargetConfig::getTargetConfig().getTargetMaxArgsPerReq();
  mTargetName = TargetConfig::getTargetConfig().getTargetName();
}

VendorIPerf::~VendorIPerf() {
}
