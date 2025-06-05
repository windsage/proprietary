/*==========================================================================
Description
  It has implementation for IPC logging mechanism.

# Copyright (c) 2017,2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <utils/Log.h>
#include "logger.h"
#include <sys/types.h>
#include <dirent.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifdef BT_VER_1_1
#define LOG_TAG "vendor.qti.bluetooth@1.1-uart_logs"
#else
#define LOG_TAG "vendor.qti.bluetooth@1.0-uart_logs"
#endif

#define UART_LOG_COLLECTION_DIR  "/sys/kernel/tracing/instances/hsuart"
#define UART_LOG_FILE_NAME    "/trace"

#define BT_SOC_CHEROKEE 1

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

void UartLogs::DumpLogs() {
  int fd;
  bool ret;
  char dpath[UART_LOG_PATH_BUF_SIZE];
  char spath[UART_LOG_PATH_BUF_SIZE];
  struct DIR* p_dir;

  ALOGD("%s: -->", __func__);

  logger_ = static_cast<void*>(Logger::Get());

  static_cast<Logger*>(logger_)->GetUartLogFilename(dpath);

  strlcpy(spath, UART_LOG_COLLECTION_DIR, sizeof(spath));

  /* Finding the UART IPC log source location */
  p_dir = opendir(UART_LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("%s: Unable to open the Dir %s err: %s (%d)", __func__, UART_LOG_COLLECTION_DIR,
      strerror(errno), errno);
    return;
  }

  closedir(p_dir);
  ALOGI("%s: found UART source log location: %s", __func__, spath);
  strlcat(spath, UART_LOG_FILE_NAME, sizeof(spath));

  fd = open(spath, O_RDONLY | O_NONBLOCK);

  if (fd >= 0) {
    close(fd);
    ret = DumpUartLogs(spath, dpath, UART_LOG_MAX_SIZE);
    ALOGD("%s: UART logs written at %s", __func__, dpath);
  } else {
    ALOGE("%s: Error opening source file (%s) error (%s)", __func__, spath, strerror(errno) );
  }
}

bool UartLogs :: DumpUartLogs(const char *spath, const char *dpath, long log_limit)
{
  int src_fd = -1;
  int dest_fd = -1;
  long logs_written = 0;
  long ret;
  char *buff;

  ALOGD("%s: dump to %s for max size %ld bytes", __func__, dpath, log_limit);

  buff = new (std::nothrow)char[UART_LOG_MAX_READ_PER_ITERATION];
  if (buff == NULL) {
    ALOGE("%s: Error allocating memory %ld bytes for log buffer", __func__,
             UART_LOG_MAX_READ_PER_ITERATION);
    return false;
  }

  src_fd = open(spath, O_RDONLY);
  if (src_fd < 0) {
    ALOGE("%s: Error opening source file (%s) error (%s)", __func__, spath, strerror(errno) );
    delete[] buff;
    return false;
  }

  dest_fd = open(dpath, O_CREAT | O_SYNC | O_WRONLY,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (dest_fd < 0) {
    ALOGE("%s: Error opening destination file: %s (%s)", __func__, dpath, strerror(errno) );
    delete[] buff;
    close(src_fd);
    return false;
  }

  do {
    ret = read(src_fd, buff, UART_LOG_MAX_READ_PER_ITERATION);
    if (ret <= 0) {
      ALOGE("%s: Finish reading src file: %ld (%s)", __func__, ret, strerror(errno) );
      break;
    }

    ret = write(dest_fd, buff, ret);
    if (ret <= 0) {
      ALOGE("%s: Error writing to dest file: %ld (%s)", __func__, ret, strerror(errno) );
      break;
    }


    logs_written += ret;
    if ((log_limit != COMPLETE_UART_LOGS) && (logs_written >= log_limit)) {
      ALOGE("%s: Have pulled enough UART IPC logs", __func__);
      break;
    }
  } while (1);

  ALOGI("%s: Total Read/Written size = %ld", __func__, logs_written);

  delete[] buff;
  close(src_fd);

  if (fsync(dest_fd) == -1) {
    ALOGE("%s: Error while synchronization of logs in :%s error code:%s", __func__,
          spath, strerror(errno));
  }

  close(dest_fd);

  return true;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
