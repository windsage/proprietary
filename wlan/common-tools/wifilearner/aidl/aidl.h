/*
 * Copyright (c) 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef WIFILEARNER_AIDL_H
#define WIFILEARNER_AIDL_H

#ifdef __cplusplus
extern "C"
{
#endif  // _cplusplus
#include "wifilearner.h"

int wifilearner_aidl_process(struct wifilearner_ctx *wlc);

#ifdef __cplusplus
}
#endif  // _cplusplus

#endif  // WIFILEARNER_AIDL_H
