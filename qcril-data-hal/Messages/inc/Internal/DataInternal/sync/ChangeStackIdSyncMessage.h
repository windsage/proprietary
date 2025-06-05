/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef CHANGESTACKIDSYNCMESSAGE
#define CHANGESTACKIDSYNCMESSAGE

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

class ChangeStackIdWdsSyncMessage : public SolicitedSyncMessage<int>,
                                     public add_message_id<ChangeStackIdWdsSyncMessage>
{
private:
  int stackId;
  int instanceId;
public:

  static constexpr const char *MESSAGE_NAME = "ChangeStackIdWdsSyncMessage";
  inline ChangeStackIdWdsSyncMessage(GenericCallback<int> *callback) :
    SolicitedSyncMessage<int>(get_class_message_id())
  {
    (void)callback;
    mName = MESSAGE_NAME;
  }

  ~ChangeStackIdWdsSyncMessage(){}

  int getStackId()
  {
    return stackId;
  }
  int getInstanceId()
  {
    return instanceId;
  }
  void setParams(int StackId, int InstanceId)
  {
    stackId = StackId;
    instanceId = InstanceId;
  }
  string dump()
  {
    return mName;
  }
};

class ChangeStackIdDsdSyncMessage : public SolicitedSyncMessage<int>,
                                     public add_message_id<ChangeStackIdDsdSyncMessage>
{
private:
  int stackId;
  int instanceId;
public:

  static constexpr const char *MESSAGE_NAME = "ChangeStackIdDsdSyncMessage";
  inline ChangeStackIdDsdSyncMessage(GenericCallback<int> *callback) :
    SolicitedSyncMessage<int>(get_class_message_id())
  {
    (void)callback;
    mName = MESSAGE_NAME;
  }

  ~ChangeStackIdDsdSyncMessage(){}

  int getStackId()
  {
    return stackId;
  }
  int getInstanceId()
  {
    return instanceId;
  }
  void setParams(int StackId, int InstanceId)
  {
    stackId = StackId;
    instanceId = InstanceId;
  }
  string dump()
  {
    return mName;
  }
};
#endif
