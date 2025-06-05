/*===================================================================================
  Copyright (c) 2019, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

#ifndef STDEFS_H
#define STDEFS_H

#include <string>

using namespace std;

// TODO Replace this with some dynamic logging approach
#define LOGV(...) do { } while (0)  // Comment this line to enable LOGV
//#define LOGD(...) do { } while (0)  // Comment this line to enable LOGD

#define TUI_ERROR(error_code)                                  \
    ret = (error_code);                                        \
    LOGE("%s::%d err=%x (%s)", __func__, __LINE__, error_code, \
         strerror(error_code));                                \
    goto end;

#define TUI_CHECK_ERR(cond, error_code) \
    if (!(cond)) {                      \
        TUI_ERROR(error_code)           \
    }

#define TUI_CHECK(cond)                     \
    if (!(cond)) {                          \
        LOGE("%s::%d", __func__, __LINE__); \
        goto end;                           \
    }

static std::string locations[] = {
#if defined(ST_TARGET_TS_LOOPBACK)
    "/data/vendor/ts_loopback",
#endif
};

#endif  // STDEFS_H
