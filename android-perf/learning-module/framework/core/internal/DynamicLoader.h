/******************************************************************************
 *   @file    DynamicLoader.h
 *   @brief   Loads the Libs from perf-core dynamically
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef DynamicLoader_H
#define DynamicLoader_H

#include "DebugLog.h"
#include "VendorIBoostConfig.h"

#define LOG_TAG_DL "DYNAMIC-LOADER"

class DynamicLoader {
private:
    DynamicLoader();
    DynamicLoader(DynamicLoader const&);
    DynamicLoader& operator=(DynamicLoader const&);
    static DynamicLoader dlInstance;

public:

    ~DynamicLoader() {
        DEBUGV(LOG_TAG_DL,"Dynamic Loader Destructor");
    };

    static DynamicLoader& getInstance();

    VendorIPerfDataStore* mStore;
    VendorIPerf* mCurrVendor;
    int (*mPerfLockAcq)(int, int, int[], int);
    int (*mPerfLockRel)(int);
    int (*mPerfGetPropExtn)(const char *, char *, size_t, const char *);
    int (*mPerfHint)(int, const char *, int, int);
    int (*mPerfLockAcqRel)(int, int, int[], int, int);
    int (*mPerfHintAcqRel)(int, int, const char *, int, int, int, int[]);
    const char * (*mPerfSyncReq)(int);
    void DynamicLoading();
};

#endif /* DynamicLoader_H */
