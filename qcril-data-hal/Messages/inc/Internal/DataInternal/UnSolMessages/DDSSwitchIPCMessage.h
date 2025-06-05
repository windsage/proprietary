/*
 * Copyright (c) 2019-2020, 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DDSSWITCHIPCMESSAGE
#define DDSSWITCHIPCMESSAGE
#include "framework/Message.h"
#include "qtibus/IPCMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"
#include "data_system_determination_v01.h"

namespace rildata {

enum class DDSSwitchIPCMessageState : int {
    ApStarted = 0,
    MpStarted = 1,
    Completed = 2,
    SetDataAllowedOnNDDS = 3,
};

/********************** Class Definitions *************************/
class DDSSwitchIPCMessage: public IPCMessage,
                           public add_message_id<DDSSwitchIPCMessage> {

private:
  DDSSwitchIPCMessageState state;
  int subId;
  bool isTempDdsSwitched;
  int permanentDDSSub;
  dsd_dds_switch_type_enum_v01 switchType;
  bool isDataAllowed;

public:
  static constexpr const char *MESSAGE_NAME = "DDSSwitchIPCMessage";

  DDSSwitchIPCMessage(DDSSwitchIPCMessageState setState, int setSubId, dsd_dds_switch_type_enum_v01 switch_type, bool isTempDDS, int permanentSub, bool isDataAllowed);
  ~DDSSwitchIPCMessage();

  void serialize(IPCOStream &os) override;
  void deserialize(IPCIStream &is) override;

  DDSSwitchIPCMessageState getState();
  int getSubId();
  bool getIsTempDDSSwitched();
  int getPermanentDDSSub();
  dsd_dds_switch_type_enum_v01 getSwitchType();
  bool getIsDataAllowed();

  std::shared_ptr<UnSolicitedMessage> clone();
  string dump();
};

} //namespace

#endif