/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#ifndef _SERVICE_MANAGER_UTILS_H
#define _SERVICE_MANAGER_UTILS_H

#ifdef __ANDROID__
#include <binder/IServiceManager.h>
#else
#include "fakeservicemanager/FakeServiceManager.h"
#endif

::android::sp<android::IServiceManager> getServiceManager();

#endif
