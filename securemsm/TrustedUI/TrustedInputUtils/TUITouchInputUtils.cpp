/*
 * Copyright (c) 2021 - 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cstdint>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <stdio.h>
#include <unistd.h>

#include "TUITouchInputUtils.h"

#define LOG_TAG "TUI-Listener:Touch"

#define MAX_RETRY_ATTEMPTS 5

/******************************************************************************
 *                        Private Class Function Definitions
 *
 * *****************************************************************************/

/* Description :  This API searches for the control filename.
 *
 * Return :  -ENOENT,
 *           -EINVAL
 *
 */

int32_t TouchInput::touchSearchDevice() {
  int32_t ret = -ENOENT;
  std::string path = mQTSFileLocation + mDisplayType;
  DIR *dirp = NULL;
  struct dirent *dirInfo = NULL;
  string devicePath;
  TUI_CHECK_ERR(!path.empty(), -EINVAL);

  dirp = opendir(&path[0]);
  if (dirp == nullptr)
    return ret;
  devicePath = path + '/';

  while ((dirInfo = readdir(dirp)) != NULL) {
    if ((!strncmp(dirInfo->d_name, ".", 10)) ||
        (!strncmp(dirInfo->d_name, "..", 10)))
      continue;
    LOGD("%s::%d - d_name - %s", __func__, __LINE__, dirInfo->d_name);
    if ((dirInfo->d_type == DT_REG) &&
       (strcmp(dirInfo->d_name, controlFileName[1].c_str()) == 0)) {
      mTouchTypeFile = devicePath + controlFileName[0];
      mControlFile = devicePath + controlFileName[1];
      LOGD("%s::%d : Trusted touch enable file found %s", __func__, __LINE__,
           mControlFile.c_str());
      ret = 0;
      break;
    }
  }
  closedir(dirp);

end:
  return ret;
}

/* Description :  This API returns the controller id.
 *
 * In : str : Control FileName.
 *
 * Out : mTouchControllerId : controller id.
 *
 */

void TouchInput::getControllerId(char *str, int32_t &mTouchControllerId) {
  char *token, *stringPtr;

  token = strtok_r(str, "-", &stringPtr);
  if (token != NULL)
    token = strtok_r(NULL, "-", &stringPtr);
  if (token != NULL)
    token = strtok_r(NULL, "/", &stringPtr);
  if (token != NULL)
    mTouchControllerId = strtol(token, NULL, 16);
}

/* Description :  This API assigns touch control to TVM
 *
 * Return :  -errno,
 *           -1
 *
 */
int32_t TouchInput::assignTouchControl() {
  int32_t ret = 0, retry = MAX_RETRY_ATTEMPTS;
  ssize_t writtenBytes = 0;
  ssize_t readBytes = 0;
  char c;
  string str;

  /* Get the controller ID, so that the info is shared with hidl.  */
  str = mControlFile;
  getControllerId(&str[0], mTouchControllerId);
  LOGD("%s, %d : Opening control file: %s, mTouchControllerId:0x%x", __func__,
       __LINE__, mControlFile.c_str(), mTouchControllerId);

  mControlFd = open(mControlFile.c_str(), O_RDWR);
  if (mControlFd < 0) {
    LOGE("%s, %d : Failed to get controlFd", __func__, __LINE__);
    ret = -errno;
    TUI_CHECK_ERR(ret == 0, ret);
  }
  /* Donate touch device to VM */
  writtenBytes = pwrite(mControlFd, "1", 1, 0);
  if (writtenBytes <= 0) {
    LOGE("%s, %d : Failed to write to control FD", __func__, __LINE__);
    ret = -errno;
    TUI_CHECK_ERR(ret == 0, ret);
  }
  /* Read back to confirm if touch is now donated to VM, retry is needed
   * in case we read back too early */
  do {
    ret = 0;
    readBytes = pread(mControlFd, &c, 1, 0);
    TUI_CHECK_ERR(readBytes > 0, -errno);
    if (c - '0' != 1) {
      LOGE("%s,%d: Touch is not yet assigned to VM", __func__, __LINE__);
      ret = -1;
    }
    retry--;
    usleep(50);
  } while (retry && (ret != 0));

end:
  close(mControlFd);
  mControlFd = -1;
  mControlFile.clear();
  mTouchTypeFile.clear();
  return ret;
}

/* Description :  This API terminates the TouchInput session in Android
*
 * Return :  0,
 *          -1
*
*/

int32_t TouchInput::unassignTouchControl() {
  int32_t ret = -1, retry = MAX_RETRY_ATTEMPTS;
  ssize_t writtenBytes = 0;
  ssize_t readBytes = 0;
  char c;
  string str;

  /* Get the controller ID, so that the info is shared with hidl.  */
  str = mControlFile;
  getControllerId(&str[0], mTouchControllerId);
  LOGD("%s, %d : Opening control file: %s, mTouchControllerId:0x%x", __func__,
       __LINE__, mControlFile.c_str(), mTouchControllerId);

  mControlFd = open(mControlFile.c_str(), O_RDWR);
  if (mControlFd < 0) {
    LOGE("%s, %d : Failed to get controlFd", __func__, __LINE__);
    ret = -errno;
    TUI_CHECK_ERR(ret == 0, ret);
  }
  /* Write 0 to trusted_touch_enable to reclaim TrustedVM to access the
   * touchdata fd */
  writtenBytes = pwrite(mControlFd, "0", 1, 0);
  TUI_CHECK_ERR(writtenBytes > 0, -errno);
  LOGD("%s, %d : write(fd=%d) writtenBytes %zd", __func__, __LINE__, mControlFd,
       writtenBytes);

  do {
    ret = 0;
    /* Read the trusted_touch_enable node to verify access is reclaimed to
     * Android */
    readBytes = pread(mControlFd, &c, 1, 0);
    TUI_CHECK_ERR(readBytes > 0, -errno);
    if (c - '0' != 0) {
      LOGE("%s,%d: Touch is not yet reclaimed to Android", __func__, __LINE__);
      ret = -1;
    }
    retry--;
    usleep(50);
  } while (retry && (ret != 0));

end:
  close(mControlFd);
  mControlFd = -1;
  mControlFile.clear();
  mTouchTypeFile.clear();
  return ret;
}

int32_t TouchInput::requestResource(uint32_t touchId, uint32_t displayId) {

  int32_t ret = 0;

  LOGE("%s: Enter tID[%d] dId[%d]", __func__, touchId, displayId);
  if (displayId == 1)
    mDisplayType = "primary";
  else if (displayId == 2)
    mDisplayType = "secondary";

  /* Search for touch type and touch enable filename  */
  ret = touchSearchDevice();
  TUI_CHECK_ERR(ret == 0 && !mControlFile.empty() && !mTouchTypeFile.empty(),
                ret);

  ret = assignTouchControl();
  TUI_CHECK_COND(ret == 0, ret);

  LOGE("%s: Touch is now disabled in Android ret[%d]", __func__, ret);
end:
  return ret;
}

int32_t TouchInput::retrieveResource(uint32_t touchId, uint32_t displayId) {
  int32_t ret = 0;

  LOGE("%s: Enter tID[%d] dId[%d]", __func__, touchId, displayId);
  if (displayId == 1)
    mDisplayType = "primary";
  else if (displayId == 2)
    mDisplayType = "secondary";

  /* Search for touch type and touch enable filename  */
  ret = touchSearchDevice();
  TUI_CHECK_ERR(ret == 0 && !mControlFile.empty() && !mTouchTypeFile.empty(),
                ret);

  ret = unassignTouchControl();
  TUI_CHECK_COND(ret == 0, ret);

  LOGE("%s,%d: Touch is reclaimed to Android", __func__, __LINE__);
end:
  return ret;
}
