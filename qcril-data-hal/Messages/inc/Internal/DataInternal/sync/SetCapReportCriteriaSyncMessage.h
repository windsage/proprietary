/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef SETCAPREPORTCRIETERIASYNCMESSAGE
#define SETCAPREPORTCRIETERIASYNCMESSAGE

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

class SetCapReportCriteriaSyncMessage : public SolicitedSyncMessage<int>,
                                     public add_message_id<SetCapReportCriteriaSyncMessage>
{
private:
   rildata::LinkCapCriteria_t mCapacity;
public:

  static constexpr const char *MESSAGE_NAME = "SetCapReportCriteriaSyncMessage";
  inline SetCapReportCriteriaSyncMessage(GenericCallback<int> *callback) :
    SolicitedSyncMessage<int>(get_class_message_id())
  {
    (void)callback;
    mName = MESSAGE_NAME;
  }

  ~SetCapReportCriteriaSyncMessage(){}

  rildata::LinkCapCriteria_t getParams()
  {
    return mCapacity;
  }
  void setParams(rildata::LinkCapCriteria_t cap)
  {
    mCapacity = cap;
  }
  string dump()
  {
    return mName;
  }
};
#endif
