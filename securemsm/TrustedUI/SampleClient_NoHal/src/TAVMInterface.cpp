/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "TAVMInterface.h"
#include <utils/Log.h>
#include <TUIMINKInvoke.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TUI_TA_INTF"

//=========================================================================
TAVMInterface* TAVMInterface::getInterface(uint8_t intfType)
{
    TAVMInterface* intf = nullptr;

    switch (intfType) {
        case TUI_INTF_MINKINVOKE:
            intf = new TUIMINKInvoke();
            break;
        default:
            ALOGE("FATAL: invalid intfType received: %d", intfType);
    }

    if (intf == nullptr) {
        ALOGE("FATAL: failed to create TAVMInterface");
    }
    return intf;
}
