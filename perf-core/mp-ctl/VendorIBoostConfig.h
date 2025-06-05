/******************************************************************************
  @file    VendorIBoostConfig.h
  @brief   Implementation of boostconfigreader Interface

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __VENDOR_I_BOOSTCONFIG__H_
#define __VENDOR_I_BOOSTCONFIG__H_

#include <inttypes.h>
#include <vector>
#include "VendorIPerf.h"

using namespace std;

class VendorIPerfDataStore : public VendorIPerf {

private:

  //ctor, copy ctor, assignment overloading
    VendorIPerfDataStore(VendorIPerfDataStore const& oh);
    VendorIPerfDataStore& operator=(VendorIPerfDataStore const& oh);
    VendorIPerfDataStore() {};
    static VendorIPerfDataStore mCurrDs;
public:

    //dtor
    ~VendorIPerfDataStore(){};

    static VendorIPerfDataStore& getVendorPerfDataStore();

    //delete later
    static VendorIPerfDataStore* getVendorIPerfDataStore();

    int32_t GetSysNode(const int32_t idx_value, char *node_path);
    float GetVendorFpsFile();
};

#endif /*__VENDOR_I_BOOSTCONFIG__H_*/
