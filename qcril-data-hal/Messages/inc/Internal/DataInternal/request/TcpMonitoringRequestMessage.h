/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef TCP_MONITORING_REQ_MESSAGE
#define TCP_MONITORING_REQ_MESSAGE

#include <string>
#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include "MessageCommon.h"

namespace rildata {


struct StartTcpMonitoringResp_t {
  rildata::TcpKaResponseError_t error;
  uint32_t handle;
  IpTuple tuple;
};

class TcpMonitoringRequestMessage: public SolicitedMessage<StartTcpMonitoringResp_t>,
                           public add_message_id<TcpMonitoringRequestMessage>
{
private:
  bool mIsStart;
  IpTuple mIpTuple;
  uint32_t mConfig_handle;
  uint32_t mClientId;
public:
  static constexpr const char *MESSAGE_NAME = "TcpMonitoringRequestMessage";

  TcpMonitoringRequestMessage(uint32_t config_handle):
          SolicitedMessage<StartTcpMonitoringResp_t>(get_class_message_id()) {
    mIsStart = false;
    mConfig_handle = config_handle;
  }
  TcpMonitoringRequestMessage(IpTuple& tuple, uint32_t clientId):
          SolicitedMessage<StartTcpMonitoringResp_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mIsStart = true;
    mIpTuple.sourceAddress = tuple.sourceAddress;
    mIpTuple.destinationAddress = tuple.destinationAddress;
    mIpTuple.source_port = tuple.source_port;
    mIpTuple.destination_port = tuple.destination_port;
    mConfig_handle = 0;
    mClientId = clientId;
  }
  ~TcpMonitoringRequestMessage() {};

  uint32_t isStartRequest() {
    return mIsStart;
  }
  void getIpTuple(IpTuple& in_tuple) {
    in_tuple.sourceAddress = mIpTuple.sourceAddress;
    in_tuple.destinationAddress = mIpTuple.destinationAddress;
    in_tuple.source_port = mIpTuple.source_port;
    in_tuple.destination_port = mIpTuple.destination_port;
  }
  uint32_t getConfigHandle() {
    return mConfig_handle;
  }
  uint32_t getClientId() {
    return mClientId;
  }
  string dump() {
    std::ostringstream os;
    os << MESSAGE_NAME;
    os << ":isStart :" << mIsStart;
    return os.str();
  }
};

} //namespace rildata

#endif //TCP_MONITORING_REQ_MESSAGE