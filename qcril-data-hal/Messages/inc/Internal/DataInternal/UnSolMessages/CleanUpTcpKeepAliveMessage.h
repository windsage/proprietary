/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
******************************************************************************/
#ifndef CLEANUP_TCP_KEEP_ALIVE_MESSAGE
#define CLEANUP_TCP_KEEP_ALIVE_MESSAGE

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

namespace rildata {

/**
  * DataInitMessage: Sent by the DATA module.
  * Indicates that DATA module is ready to receive requests.
  *
  */

class CleanUpTcpKeepAliveMessage: public UnSolicitedMessage, public add_message_id<CleanUpTcpKeepAliveMessage>
{

public:
  static constexpr const char *MESSAGE_NAME = "CleanUpTcpKeepAliveMessage";
  CleanUpTcpKeepAliveMessage(uint32_t clientId):
          UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mClientId = clientId;
  }
  virtual ~CleanUpTcpKeepAliveMessage() {};

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<CleanUpTcpKeepAliveMessage>(
        getClientId());
    return msg;
  }
  string dump() {
    return std::string(MESSAGE_NAME);
  }
  uint32_t getClientId() {
    return mClientId;
  }

private:
  uint32_t mClientId;
};

} //namespace rildata

#endif //CLEANUP_TCP_KEEP_ALIVE_MESSAGE