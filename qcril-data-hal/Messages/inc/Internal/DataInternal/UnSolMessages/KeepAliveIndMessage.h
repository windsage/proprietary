/******************************************************************************
#  Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef KEEPALIVEINDMESSAGE
#define KEEPALIVEINDMESSAGE

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "MessageCommon.h"

namespace rildata {

enum class KeepAliveResult_t : int{
  KEEP_ALIVE_RESULT_OK = 0,
  KEEP_ALIVE_RESULT_ERROR = 1,
  KEEP_ALIVE_NETWORK_DOWN_ERROR = 2,
  KEEP_ALIVE_TCP_STATUS,
};

struct KeepAliveInfo_t {
  KeepAliveResult_t status;
  uint32_t handle;
};

/********************** Class Definitions *************************/
class KeepAliveIndMessage: public UnSolicitedMessage,
                           public add_message_id<KeepAliveIndMessage> {
private:
  KeepAliveInfo_t mKeepalive;
  std::shared_ptr<TcpKeepAliveIndInfo_t> mtcpKaInfo;
  bool mIsTcpKeepAliveInd;

public:
  static constexpr const char *MESSAGE_NAME = "KeepAliveIndMessage";
  KeepAliveIndMessage() = delete;
  KeepAliveIndMessage(KeepAliveInfo_t keepalive);
  KeepAliveIndMessage(std::shared_ptr<TcpKeepAliveIndInfo_t> keepalive);
  virtual ~KeepAliveIndMessage();

  std::shared_ptr<UnSolicitedMessage> clone();
  string dump();

  KeepAliveResult_t get_status();
  uint32_t get_handle();
  bool isTcpKeepAliveInd() { return mIsTcpKeepAliveInd; }
  std::shared_ptr<TcpKeepAliveIndInfo_t> getTcpIndInfo();
};
}

#endif