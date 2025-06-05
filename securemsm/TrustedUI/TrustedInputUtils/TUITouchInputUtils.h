/*
 * Copyright (c) 2021 - 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __TUI_TOUCHINPUT_UTILS_H__
#define __TUI_TOUCHINPUT_UTILS_H__

#include <sstream>
#include <string.h>
#include <vector>
#include <android/log.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)                                                              \
  do {                                                                         \
  } while (0)
#endif

#define TUI_ERROR(error_code)                                                  \
  ret = (error_code);                                                          \
  LOGE("%s::%d err=%x (%s)", __func__, __LINE__, error_code,                   \
       strerror(error_code));                                                  \
  goto end;

#define TUI_CHECK_ERR(cond, error_code)                                        \
  if (!(cond)) {                                                               \
    TUI_ERROR(error_code)                                                      \
  }

#define TUI_CHECK(cond)                                                        \
  if (!(cond)) {                                                               \
    LOGE("%s::%d", __func__, __LINE__);                                        \
    goto end;                                                                  \
  }

#define TUI_COND(error_code)                                                   \
  ret = (error_code);                                                          \
  LOGE("%s::%d err=%d", __func__, __LINE__, error_code);                       \
  goto end;

#define TUI_CHECK_COND(cond, error_code)                                       \
  if (!(cond)) {                                                               \
    TUI_COND(error_code)                                                       \
  }

using namespace std;

class TouchInput {
private:
  int32_t mControlFd = -1;
  int32_t mTouchControllerId = -1;
  string mDisplayType = "primary";
  string mControlFile = "";
  string mTouchTypeFile = "";
  inline static const vector<string> controlFileName = {"trusted_touch_type",
                                                        "trusted_touch_enable"};
  inline static const string defaultDisplayType = "primary";
  inline static const string mQTSFileLocation = {"/sys/kernel/qts/"};

  /******************************************************************************
  *                        Private Class Function Definitions
  *****************************************************************************/

  /* Description :  This API returns the controller id.
   *
   * In : str : Control FileName.
   *
   * Out : mTouchControllerId : controller id.
   *
   */
  void getControllerId(char *str, int32_t &mTouchControllerId);

  /* Description :  This API searches for the control filename.
   *
   * Return :  -ENOENT,
   *           -EINVAL
   *
   */

  int32_t touchSearchDevice();

  /* Description :  This API assigns touch control to TVM
   *
   * Return :  -errno,
   *           -1
   *
   */

  int32_t assignTouchControl();

  /* Description :  This API un-assigns touch control to TVM
   *
   * Return :  -errno,
   *           -1
   *
   */

  int32_t unassignTouchControl();

public:
  int32_t requestResource(uint32_t touchId, uint32_t displayID);
  int32_t retrieveResource(uint32_t touchId, uint32_t displayID);
};

#endif
