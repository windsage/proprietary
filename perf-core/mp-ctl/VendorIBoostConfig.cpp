/******************************************************************************
  @file    VendorIBoostConfig.cpp
  @brief   Implementation of VendorIBoostConfig Interface

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "VendorIBoostConfig.h"
#include "BoostConfigReader.h"
#include "OptsHandlerExtn.h"

VendorIPerfDataStore VendorIPerfDataStore::mCurrDs;

extern "C" VendorIPerfDataStore* getVendorIPerfDataStore() {
  return &VendorIPerfDataStore::getVendorPerfDataStore();
}

//rename later
VendorIPerfDataStore& VendorIPerfDataStore::getVendorPerfDataStore() {
  return mCurrDs;
}

//Delete Later
VendorIPerfDataStore* VendorIPerfDataStore::getVendorIPerfDataStore() {
  return &mCurrDs;
}

int32_t VendorIPerfDataStore::GetSysNode(const int32_t idx_value, char *node_path) {
    PerfDataStore *store = PerfDataStore::getPerfDataStore();
    if(store == NULL) {
      return -1;
    }
    return store->GetSysNode(idx_value, node_path);
}

float VendorIPerfDataStore::GetVendorFpsFile() {
    return get_fps_file();
}