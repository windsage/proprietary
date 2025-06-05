/******************************************************************************
 *   @file    DynamicLoader.cpp
 *   @brief   Implementation of DynamicLoader
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2022 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "DynamicLoader.h"
#include <dlfcn.h>
#include <client.h>

DynamicLoader DynamicLoader::dlInstance;

DynamicLoader::DynamicLoader() {
    mStore = NULL;
    mCurrVendor = NULL;
    mPerfLockAcq = NULL;
    mPerfLockRel = NULL;
    mPerfGetPropExtn = NULL;
    mPerfHint = NULL;
    mPerfLockAcqRel = NULL;
    mPerfHintAcqRel = NULL;
    mPerfSyncReq = NULL;
}

DynamicLoader& DynamicLoader::getInstance() {
    DEBUGV(LOG_TAG_DL,"Dynamic Loader getInstance");
    return dlInstance;
}

void DynamicLoader::DynamicLoading() {

    void* dlhandle = dlopen("libqti-perfd.so", RTLD_NOW);
    if(dlhandle == NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to (dl)open libqti-perfd.so %s\n", __func__, dlerror());
        return;
    }

    auto getVendorIPerf = (VendorIPerf* (*)()) dlsym(dlhandle, "getVendorIPerf");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get getVendorIPerf\n", __func__);
        dlclose(dlhandle);
        return;
    }
    if (getVendorIPerf == NULL) {
        QLOGE(LOG_TAG_DL, "%s getVendorIPerf is null\n", __func__);
        dlclose(dlhandle);
        return;
    }
    mCurrVendor = (*getVendorIPerf)();

    auto getVendorIPerfDataStore = (VendorIPerfDataStore* (*)()) dlsym(dlhandle, "getVendorIPerfDataStore");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get getVendorIPerfDataStore\n", __func__);
        dlclose(dlhandle);
        return;
    }
    if (getVendorIPerfDataStore == NULL) {
        QLOGE(LOG_TAG_DL, "%s getVendorIPerfDataStore is null\n", __func__);
        dlclose(dlhandle);
        return;
    }
    mStore = (*getVendorIPerfDataStore)();
    dlclose(dlhandle);

    dlhandle = dlopen("libqti-perfd-client.so", RTLD_LAZY);

    if(dlhandle == NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to (dl)open libqti-perfd-client.so %s\n", __func__, dlerror());
        return;
    }

    mPerfLockAcq = (int (*)(int, int, int[], int)) dlsym(dlhandle, "perf_lock_acq");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_lock_acq\n", __func__);
        dlclose(dlhandle);
        return;
    }

    mPerfLockRel = (int (*)(int)) dlsym(dlhandle, "perf_lock_rel");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_lock_rel\n", __func__);
        dlclose(dlhandle);
        return;
    }

    mPerfGetPropExtn = (int (*)(const char *, char *, size_t, const char *)) dlsym(dlhandle, "perf_get_prop_extn");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_get_prop_extn\n", __func__);
        dlclose(dlhandle);
        return;
    }

    mPerfHint = (int (*)(int, const char *, int, int)) dlsym(dlhandle, "perf_hint");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_hint\n", __func__);
        dlclose(dlhandle);
        return;
    }

    mPerfLockAcqRel = (int (*) (int, int, int[], int, int)) dlsym(dlhandle, "perf_lock_acq_rel");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_lock_acq_rel\n", __func__);
        dlclose(dlhandle);
        return;
    }

    mPerfHintAcqRel = (int (*) (int, int, const char *, int, int, int, int[])) dlsym(dlhandle, "perf_hint_acq_rel");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_hint_acq_rel\n", __func__);
        dlclose(dlhandle);
        return;
    }

    mPerfSyncReq = (const char * (*) (int)) dlsym(dlhandle, "perf_sync_req");
    if (dlerror() != NULL) {
        QLOGE(LOG_TAG_DL, "%s Failed to get perf_sync_req\n", __func__);
        dlclose(dlhandle);
        return;
    }
    dlclose(dlhandle);
}

