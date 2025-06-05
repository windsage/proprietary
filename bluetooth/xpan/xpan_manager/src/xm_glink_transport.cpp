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
#include "xm_glink_transport.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "vendor.qti.xpan@1.0-glink_transport"

// change below namespace while integrations
namespace xpan {
namespace implementation {

GlinkTransport * GlinkTransport :: instance_ = NULL;
GlinkTransport * GlinkTransport :: Get()
{
  if (!instance_) {
    instance_ = new GlinkTransport();
  }

  return instance_;
}

GlinkTransport :: GlinkTransport()
{
  glink_cc_fd = -1;
}


GlinkTransport ::~GlinkTransport()
{
  glink_cc_fd = -1;
  instance_ = nullptr;
}

/******************************************************************
 *
 * Function         OpenGlinkChannel
 *
 * Description      Open device node based on channel type.
 *
 * Parameters:      type    - Name of the channel to be opened
 *
 * Returns          File descriptor if open is successful.
 *
 ******************************************************************/
int GlinkTransport::OpenGlinkChannel(glink_channel type)
{
  ALOGI("%s: of type :%u", __func__, type);

  if (type == TYPE_GLINK_CC)
    glink_cc_fd = open(GLINK_CONTROL_CHANNEL, O_RDWR | O_NOCTTY);

  if (glink_cc_fd <= 0) {
    ALOGE("%s: unable to open %s: %s(%d)", __func__, GLINK_CONTROL_CHANNEL,
          strerror(errno), errno);
  } else {
    ALOGI("%s: successfully opened %s with fd %d", __func__,
          GLINK_CONTROL_CHANNEL, glink_cc_fd);
  }

  return glink_cc_fd;
}

/******************************************************************
 *
 * Function         CloseGlinkChannel
 *
 * Description      Close device node based on file descriptor.
 *
 * Parameters:      fd     - File descriptor number
 *
 ******************************************************************/
void GlinkTransport::CloseGlinkChannel(int fd)
{
  ALOGI("%s: of fd :%u", __func__, fd);
  /* Reset correct channel based on the fd */
  glink_cc_fd = glink_cc_fd == fd ? -1 : glink_cc_fd;
  close(fd);
}

/******************************************************************
 *
 * Function         WritetoGlinkCC
 *
 * Description      Write to glink control channel
 *
 * Parameters:      data    - Data to be written to channel
 *                  len     - Number of bytes to be written.
 *
 * Returns          Number of bytes written
 *
 ******************************************************************/
int GlinkTransport::WritetoGlinkCC(const uint8_t *data, int len)
{
  std::unique_lock<std::mutex> guard(internal_mutex_);
  int write_len = 0;
  while (len > 0) {
    ssize_t ret =
      TEMP_FAILURE_RETRY(write(glink_cc_fd, data + write_len, len));

    if (ret == -1) {
      if (errno == EAGAIN) continue;
      ALOGE("%s error writing to GLINK CC (%s)", __func__, strerror(errno));
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

int GlinkTransport::GetGlinkFd(void)
{
  return glink_cc_fd;
}

}
}
