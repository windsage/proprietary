/*
 *  Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved..
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <mutex>

#define GLINK_CONTROL_CHANNEL     "/dev/bt_cp_ctrl"

enum glink_channel {
  TYPE_GLINK_CC = 1,
};

namespace xpan {
namespace implementation {

class GlinkTransport
{
  public:
   GlinkTransport();
   ~GlinkTransport();
   int OpenGlinkChannel(glink_channel);
   void CloseGlinkChannel(int);
   int WritetoGlinkCC(const uint8_t *, int);
   static GlinkTransport *Get(void);
   int GetGlinkFd(void);
  private:
   int glink_cc_fd;
   std::mutex internal_mutex_;
   static GlinkTransport *instance_;
};

}
}
