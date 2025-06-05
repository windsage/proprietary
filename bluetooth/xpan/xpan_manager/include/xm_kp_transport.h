/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <mutex>

#define BT_KP_TRANSPORT_DEVICE     "/dev/btfmcodec_dev"


namespace xpan {
namespace implementation {

class KernelProxyTransport
{
  public:
   KernelProxyTransport();
   ~KernelProxyTransport();
   int OpenKpTransport(void);
   void CloseKpTransport(int);
   int WritetoKpTransport(const uint8_t *, int);
   static KernelProxyTransport *Get(void);
  private:
   int kp_fd;
   std::mutex internal_mutex_;
   static KernelProxyTransport *instance_;
};

}
}
