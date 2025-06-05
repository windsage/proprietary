/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef TCPKEEPALIVEINDMESSAGE
#define TCPKEEPALIVEINDMESSAGE

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "MessageCommon.h"
namespace rildata {



/********************** Class Definitions *************************/
class TcpKeepAliveIndMessage: public UnSolicitedMessage,
                           public add_message_id<TcpKeepAliveIndMessage> {
private:
  std::shared_ptr<TcpKeepAliveIndInfo_t> mKeepalive;
  IpTuple mTuple;
public:
  static constexpr const char *MESSAGE_NAME = "TcpKeepAliveIndMessage";
  TcpKeepAliveIndMessage() = delete;
  TcpKeepAliveIndMessage(std::shared_ptr<TcpKeepAliveIndInfo_t> keepalive,
      IpTuple tuple) :
           UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mKeepalive = keepalive;
    mTuple = tuple;
  }
  virtual ~TcpKeepAliveIndMessage() {}

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<TcpKeepAliveIndMessage>(mKeepalive, mTuple);
  }
  string dump() {
     return MESSAGE_NAME;
  }
  std::shared_ptr<TcpKeepAliveIndInfo_t> getKaInfo() {return mKeepalive; }
  IpTuple& getIpTuple() { return mTuple;  }
};
} //namespace rildata

#endif //TCPKEEPALIVEINDMESSAGE
