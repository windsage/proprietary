/*
 * Copyright (c) 2019, 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "UnSolMessages/DDSSwitchIPCMessage.h"

using namespace rildata;

DDSSwitchIPCMessage::DDSSwitchIPCMessage(DDSSwitchIPCMessageState setState,
                      int setSubId, dsd_dds_switch_type_enum_v01 switch_type, bool tempdds, int permanentSub, bool allow):
  IPCMessage(get_class_message_id()), state(setState), subId(setSubId),
  isTempDdsSwitched(tempdds), permanentDDSSub(permanentSub), switchType(switch_type), isDataAllowed(allow)
{}

DDSSwitchIPCMessage::~DDSSwitchIPCMessage() {}

void DDSSwitchIPCMessage::serialize(IPCOStream &os)
{
  os << static_cast<int>(state) << "," << subId << "," << static_cast<int>(switchType) << "," << isTempDdsSwitched << "," << permanentDDSSub << ","<< isDataAllowed;
}

void DDSSwitchIPCMessage::deserialize(IPCIStream &is)
{
  std::stringstream &iss = static_cast<std::stringstream&>(is);
  Log::getInstance().d("DDSSwitchIPCMessage deserialize iss=[" + iss.str() + "]");
  int inState;
  int switch_val;
  iss >> inState;
  iss.ignore(1, ',');
  iss >> subId;
  iss.ignore(1,',');
  iss >> switch_val;
  iss.ignore(1,',');
  iss >> isTempDdsSwitched;
  iss.ignore(1, ',');
  iss >> permanentDDSSub;
  iss.ignore(1,',');
  iss >> isDataAllowed;
  state = static_cast<DDSSwitchIPCMessageState>(inState);
  switchType = static_cast<dsd_dds_switch_type_enum_v01>(switch_val);
}

DDSSwitchIPCMessageState DDSSwitchIPCMessage::getState()
{
  return state;
}

int DDSSwitchIPCMessage::getSubId()
{
  return subId;
}

dsd_dds_switch_type_enum_v01 DDSSwitchIPCMessage::getSwitchType()
{
  return switchType;
}

bool DDSSwitchIPCMessage::getIsTempDDSSwitched()
{
  return isTempDdsSwitched;
}

int DDSSwitchIPCMessage::getPermanentDDSSub()
{
  return permanentDDSSub;
}

bool DDSSwitchIPCMessage::getIsDataAllowed()
{
  return isDataAllowed;
}

std::shared_ptr<UnSolicitedMessage> DDSSwitchIPCMessage::clone()
{
  auto copy = std::make_shared<DDSSwitchIPCMessage>(state, subId, switchType, isTempDdsSwitched, permanentDDSSub, isDataAllowed);
  copy->setIsRemote(getIsRemote());
  return copy;
}

string DDSSwitchIPCMessage::dump()
{
  std::string tempDDS = (isTempDdsSwitched)?("TRUE"):("FALSE");
  std::string allow = (isDataAllowed)?("TRUE"):("FALSE");
  std::string switch_val = (switchType == DSD_DDS_SWITCH_PERMANENT_V01)?"Permanent":"Temporary";
  return std::string(MESSAGE_NAME) + " isRemote: " +
         std::to_string(getIsRemote()) + " subId: " + std::to_string(subId) + "switchType :" + switch_val +
         " isTempDDS: " + tempDDS + " permanentDDSSub: " + std::to_string(permanentDDSSub) + " isDataAllowedOnNDDS: " + allow;
}
