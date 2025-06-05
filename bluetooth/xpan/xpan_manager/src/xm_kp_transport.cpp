/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utils/Log.h>
#include <string.h>
#include <errno.h>
#include "xm_kp_transport.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-kp_transport"

namespace xpan {
namespace implementation {

KernelProxyTransport * KernelProxyTransport :: instance_ = NULL;
KernelProxyTransport * KernelProxyTransport :: Get()
{
  if (!instance_) {
    instance_ = new KernelProxyTransport();
  }

  return instance_;
}

KernelProxyTransport :: KernelProxyTransport()
{
  kp_fd = -1;
}

KernelProxyTransport :: ~KernelProxyTransport()
{
  kp_fd = -1;
  instance_ = nullptr;
}

/******************************************************************
 *
 * Function         OpenKpTransport
 *
 * Description      Open device node of KP transport.
 *
 * Returns          File descriptor if open is successful.
 *
 ******************************************************************/
int KernelProxyTransport::OpenKpTransport(void)
{
  kp_fd = open(BT_KP_TRANSPORT_DEVICE, O_RDWR | O_NOCTTY);
  if (kp_fd <= 0) {
    ALOGE("%s: unable to open %s: %s(%d)", __func__, BT_KP_TRANSPORT_DEVICE,
          strerror(errno), errno);
  } else {
    ALOGI("%s: successfully opened %s with fd %d", __func__,
          BT_KP_TRANSPORT_DEVICE, kp_fd);
  }

  return kp_fd;
}

/******************************************************************
 *
 * Function         CloseKpTransport
 *
 * Description      Close device node based on file descriptor.
 *
 * Parameters:      fd     - File descriptor number
 *
 ******************************************************************/
void KernelProxyTransport::CloseKpTransport(int fd)
{
  ALOGI("%s: of fd :%u", __func__, fd);
  close(fd);
}

/******************************************************************
 *
 * Function         WritetoKpTransport
 *
 * Description      Write to kernel proxy transport
 *
 * Parameters:      data    - Data to be written to channel
 *                  len     - Number of bytes to be written.
 *
 * Returns          Number of bytes written
 *
 ******************************************************************/
int KernelProxyTransport::WritetoKpTransport(const uint8_t *data, int len)
{
  std::unique_lock<std::mutex> guard(internal_mutex_);
  int write_len = 0;
  while (len > 0) {
    ssize_t ret =
      TEMP_FAILURE_RETRY(write(kp_fd, data + write_len, len));

    if (ret == -1) {
      if (errno == EAGAIN) continue;
      ALOGE("%s error writing to KP (%s)", __func__, strerror(errno));
      break;

    } else if (ret == 0) {
      ALOGE("%s zero bytes written - something went wrong...", __func__);
      break;
    }

    write_len += ret;
    len -= ret;
  }
  return write_len;
}

}
}
