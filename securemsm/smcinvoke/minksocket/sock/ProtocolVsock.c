/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/
#include <sys/socket.h>
#include "MinkTransportUtils.h"
#include "ProtocolVsock.h"

#if defined(VNDR_VSOCK)
int32_t ProtocolVsock_constructFd(void)
{
  int32_t fd = -1;

  fd = socket(AF_VSOCK, SOCK_DGRAM, 0);
  if (fd > 0) {
    LOG_MSG("construct sockfd %d of AF_VSOCK\n", fd);
    goto exit;
  }

  fd = socket(AF_QMSGQ, SOCK_DGRAM, 0);
  if (fd > 0) {
    LOG_MSG("construct sockfd of AF_VSOCK failed, \
            construct sockfd %d of AF_QMSGS instead\n", fd);
    goto exit;
  }

  LOG_ERR("fail to construct sockfd of AF_VSOCK or AF_QMSGQ\n");

exit:
  return fd;
}
#endif
