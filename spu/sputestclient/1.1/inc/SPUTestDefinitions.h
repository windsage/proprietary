/*!
 *
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#pragma once

/*
 * Return values enum
 */
enum TestErrorCodes : int32_t {
    SUCCESS             =  0,
    GENERAL_ERROR       = -1,
    INVALID_PARAM       = -2,
    FRAMEWORK_ERROR     = -3,
    RESOURCE_BUSY       = -4,
    LOAD_APP_ERROR      = -5,
    ALREADY_LOADED      = -6,
    MESSAGING_ERROR     = -7,
    RESPONSE_ERROR      = -8,
    SSR_ERROR           = -9,
    UNEXPECTED_ERROR    = -10,
    SPCOM_MEMORY_ERROR  = -11,
    HIDL_MEMORY_ERROR   = -12,
    CHANNEL_ERROR       = -13,
    INPUT_ERROR         = -14,
    CLIENT_TEST_ERROR   = -15,
    SERVER_TEST_ERROR   = -16,
    SPU_INFO_TEST_ERROR = -17,
    REQUEST_ERROR       = -18,
    SHARED_BUFFER_ERROR = -19,
};

enum SSRHandlingState : uint32_t {
    SSR_STATE_OFF      = 1,
    SSR_STATE_STARTED  = 2,
    SSR_STATE_READY    = 3,
    SSR_STATE_HANDLE   = 4,
    SSR_STATE_SHUTDOWN = 5,
    SSR_STATE_ERROR    = 6,
};

struct appDescriptor {
    const char* appName;
    const uint32_t uuid;
};

static const uint32_t sAppVersionMajor = 1;
static const uint32_t sAppVersionMinor = 1;

static const char* sSpcomClientAppChannel = "iuicc1";
static const char* sSpcomClientAppFullPath =
    "/vendor/firmware_mnt/image/sphalcli1t.sig";

static const char* sSpcomServerAppChannel = "iuicc0";
static const char* sSpcomServerAppFullPath =
    "/vendor/firmware_mnt/image/sphalsrv1t.sig";

static const char* sSpcomServerHLOSChannelName = "iuiccserver";

static const uint32_t sPgSz = 4096;
static const uint32_t sDefaultTimeotMs = 5000;
static const uint32_t sConnectionRetries = 15;
static const uint32_t sWaitForSpuReadyTimeoutSec = 30;
static const uint32_t sNumOfBootApps = 4;
static const uint32_t sMaxSwapSz = 256 * 1024;
static const uint32_t sMaxSpcomChannelNameLength = 32;
static const uint32_t sMaxSpcomMessageLength = 268;
static const uint32_t sWaitForSSRReadyRetries = 10;
static const uint32_t sSpcomStressTestCycles = 10;

static const appDescriptor sBootAppsDescriptors[] = {
    { "asym_cryptoapp",  0x3D86A887 },
    { "cryptoapp",       0xEF80345E },
    { "sp_keymaster",    0x24500E2F },
    { "macchiato",       0x09068E40 },
};
