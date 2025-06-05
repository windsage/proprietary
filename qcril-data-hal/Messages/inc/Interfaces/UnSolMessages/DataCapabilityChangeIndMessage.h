/**
* Copyright (c) 2021,2023 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef DATACAPABILITYCHANGEINDMESSAGE
#define DATACAPABILITYCHANGEINDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

struct DataCapability_t {
  bool CIWlanSupported;
  // add new flag below
};

/********************** Class Definitions *************************/
class DataCapabilityChangeIndMessage : public UnSolicitedMessage,
                           public add_message_id<DataCapabilityChangeIndMessage> {

private:
  DataCapability_t mDataCapability;
  CIWlanCapabilityType ciwlanCapType;

public:
  static constexpr const char *MESSAGE_NAME = "DataCapabilityChangeIndMessage";

  ~DataCapabilityChangeIndMessage() = default;
  DataCapabilityChangeIndMessage():
  UnSolicitedMessage(get_class_message_id())
  {
    mName = MESSAGE_NAME;
    mDataCapability.CIWlanSupported = false;
    ciwlanCapType = None;
  }
  DataCapabilityChangeIndMessage(DataCapability_t capability, CIWlanCapabilityType capType ):
  UnSolicitedMessage(get_class_message_id()), mDataCapability(capability), ciwlanCapType(capType)
  {
    mName = MESSAGE_NAME;
  }

  void setCIWlanSupported(bool cap, CIWlanCapabilityType type = None ) {mDataCapability.CIWlanSupported=cap; ciwlanCapType = type;}
  bool getCIWlanSupported() {return mDataCapability.CIWlanSupported;}
  CIWlanCapabilityType getCIWlanCapabilityType() { return ciwlanCapType;}

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    return std::make_shared<DataCapabilityChangeIndMessage>(mDataCapability, ciwlanCapType);
  }

  string dump()
  {
    std::stringstream ss;
    ss << MESSAGE_NAME;
    ss << " CIWlanSupported=" << (int)getCIWlanSupported();
    ss << "CIWlanCapType=" << (int)getCIWlanCapabilityType();
    return ss.str();
  }
};

} //namespace

#endif