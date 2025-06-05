/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI__CALLBACK_H__
#define __TUI__CALLBACK_H__

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <semaphore.h>
#include <utils/Log.h>
#include "common_log.h"

using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct TrustedUICallback
{
    Return<void> onComplete();
    Return<void> onError();

};

#endif