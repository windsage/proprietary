/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_AIDL_vendor.qti.gnss@service"

#include <log_util.h>
#include <loc_cfg.h>
#include <loc_pla.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include "LocAidlGnss.h"

#if defined __LIBFUZZON__
#include <signal.h>
#include <stdio.h>

extern "C" void __gcov_dump(void);

void gcovHandler(int signal) {
    ALOGW("gcovHandler is called.\n");
    char *s1 = getenv("GCOV_PREFIX");
    if (s1 != NULL) {
        ALOGW("GCOV_PREFIX is %s", s1);
    }
    char *s2 = getenv("GCOV_PREFIX_STRIP");
    if (s2 != NULL) {
        ALOGW("GCOV_PREFIX_STRIP is %s", s2);
    }

    ALOGW("__gcov_dump prepare.\n");
    __gcov_dump();
    ALOGW("__gcov_dump has been called.\n");
}

void setGcovHandler() {
    ALOGI("start setting gcov handler for Loc AIDL.");
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = gcovHandler;
    sigaction(SIGTERM, &act, 0);
    sigaction(SIGKILL, &act, 0);
    sigaction(SIGSEGV, &act, 0);
    sigaction(SIGINT, &act, 0);
    sigaction(SIGILL, &act, 0);
    sigaction(SIGABRT, &act, 0);
    sigaction(SIGFPE, &act, 0);
    ALOGI("finish setting gcov handler for Loc AIDL.");
}
#endif

using aidl::vendor::qti::gnss::implementation::LocAidlGnss;

int main(int /* argc */, char* /* argv */ []) {
#if defined __LIBFUZZON__
    setGcovHandler();
#endif
    UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);
    std::shared_ptr<LocAidlGnss> gnssService = ndk::SharedRefBase::make<LocAidlGnss>();
    if (gnssService != nullptr) {
        ndk::SpAIBinder locAidlBinder = gnssService->asBinder();
        const std::string instance = std::string() + LocAidlGnss::descriptor + "/default";
        AServiceManager_addService(locAidlBinder.get(), instance.c_str());
    }
    return 0;
}
