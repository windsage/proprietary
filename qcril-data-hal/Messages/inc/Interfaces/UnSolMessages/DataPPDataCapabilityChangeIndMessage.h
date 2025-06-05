/**
* Copyright (c) 2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATAPPDATACAPABILITYCHANGEINDMESSAGE
#define DATAPPDATACAPABILITYCHANGEINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {


/********************** Class Definitions *************************/
class DataPPDataCapabilityChangeIndMessage : public UnSolicitedMessage,
                           public add_message_id<DataPPDataCapabilityChangeIndMessage> {

private:
  bool mDataPPDataCapability;

public:
  static constexpr const char *MESSAGE_NAME = "DataPPDataCapabilityChangeIndMessage";

  ~DataPPDataCapabilityChangeIndMessage() = default;
  DataPPDataCapabilityChangeIndMessage():
  UnSolicitedMessage(get_class_message_id())
  {
    mName = MESSAGE_NAME;
    mDataPPDataCapability = false;
  }
  DataPPDataCapabilityChangeIndMessage(bool capability):
  UnSolicitedMessage(get_class_message_id()), mDataPPDataCapability(capability)
  {
    mName = MESSAGE_NAME;
  }

  void setDataPPDataSupported(bool cap ) {mDataPPDataCapability=cap; }
  bool getDataPPDataSupported() {return mDataPPDataCapability;}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<DataPPDataCapabilityChangeIndMessage>(mDataPPDataCapability);
  }

  string dump()
  {
    std::stringstream ss;
    ss << MESSAGE_NAME;
    ss << " DataPPDataSupported=" << (int)getDataPPDataSupported();
    return ss.str();
  }
};

} //namespace

#endif