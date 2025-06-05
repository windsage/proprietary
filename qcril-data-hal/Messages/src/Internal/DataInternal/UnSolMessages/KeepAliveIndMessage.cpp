/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "UnSolMessages/KeepAliveIndMessage.h"

using namespace rildata;

KeepAliveIndMessage::KeepAliveIndMessage(KeepAliveInfo_t keepalive)
    :UnSolicitedMessage(get_class_message_id())
{
  mKeepalive = keepalive;
  mIsTcpKeepAliveInd = false;
}

KeepAliveIndMessage::KeepAliveIndMessage(std::shared_ptr<TcpKeepAliveIndInfo_t> keepalive)
    : UnSolicitedMessage(get_class_message_id())
{
  mtcpKaInfo = keepalive;
  mIsTcpKeepAliveInd = true;
}
KeepAliveIndMessage::~KeepAliveIndMessage(){
  mtcpKaInfo = nullptr;
}

std::shared_ptr<UnSolicitedMessage> KeepAliveIndMessage::clone()
{
  if(mtcpKaInfo) {
    return std::make_shared<KeepAliveIndMessage>(mtcpKaInfo);
  }
  return std::make_shared<KeepAliveIndMessage>(mKeepalive);
}

string KeepAliveIndMessage::dump()
{
  return MESSAGE_NAME;
}

KeepAliveResult_t KeepAliveIndMessage::get_status()
{
  return mKeepalive.status;
}
std::shared_ptr<TcpKeepAliveIndInfo_t> KeepAliveIndMessage::getTcpIndInfo() {
  return mtcpKaInfo;
}

uint32_t KeepAliveIndMessage::get_handle()
{
  return mKeepalive.handle;
}
