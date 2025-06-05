/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef TOGGLELQEINDICATIONSYNCMESSAGE
#define TOGGLELQEINDICATIONSYNCMESSAGE

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

class ToggleLqeIndicationSyncMessage : public SolicitedSyncMessage<int>,
                                     public add_message_id<ToggleLqeIndicationSyncMessage>
{
private:
   rildata::LqeIndicationSwitch mIndication;
   rildata::LqeDataDirection mDirection;
public:

  static constexpr const char *MESSAGE_NAME = "ToggleLqeIndicationSyncMessage";
  inline ToggleLqeIndicationSyncMessage(GenericCallback<int> *callback) :
    SolicitedSyncMessage<int>(get_class_message_id())
  {
    (void)callback;
    mName = MESSAGE_NAME;
  }

  ~ToggleLqeIndicationSyncMessage(){}

  rildata::LqeDataDirection getLqeDirection()
  {
    return mDirection;
  }
  void setParams(rildata::LqeDataDirection dir, rildata::LqeIndicationSwitch ind){
    mDirection = dir;
    mIndication = ind;
  }
  rildata::LqeIndicationSwitch getLqeIndication()
  {
    return mIndication;
  }
  string dump()
  {
    return mName;
  }
};
#endif
