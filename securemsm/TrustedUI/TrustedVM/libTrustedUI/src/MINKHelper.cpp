/*========================================================================
Copyright (c) 2020, 2022-2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================*/
/******************************************************************************
 *                   Header Inclusions
*****************************************************************************/
#include <cstdint>

#include "impl_base.hpp"
#include "minkipc.h"
#include "object.h"
#include "proxy_base.hpp"
#include "TUILog.h"
#include "ITUICoreService.hpp"
#include "ITUISystemSession.hpp"
#include "CTUICoreService.hpp"
#include "IOpener.h"

extern "C" {
#include "moduleAPI.h"
}

#define LOG_TAG "libTrustedUI"

TUICoreService *getTUIService() {
    int32_t ret = 0;
    TUICoreService *tuiCoreSvc;
    Object TUICoreServiceObj;

    TUILOGD("%s:++", __func__);
    ret = IOpener_open(gTVMEnv, CTUICoreService_UID, &TUICoreServiceObj);
    if (ret != 0 || Object_isNull(TUICoreServiceObj)) {
        TUILOGE("%s: Failed to get TUI Service object", __func__);
        goto exit_null;
    }

    tuiCoreSvc = new TUICoreService(TUICoreServiceObj);
    if (tuiCoreSvc == nullptr) {
        TUILOGE("%s: failed to create TrustedUICoreSvc object", __func__);
        goto exit_null;
    }

    TUILOGE("Success in connecting to TUI service");
    return tuiCoreSvc;

exit_null:
    Object_ASSIGN_NULL(TUICoreServiceObj);
    return nullptr;
}

void releaseTUIService(TUICoreService *tuiCoreSvc) {

    TUILOGD("%s:++", __func__);
    delete tuiCoreSvc;
    TUILOGD("%s:--", __func__);
}
