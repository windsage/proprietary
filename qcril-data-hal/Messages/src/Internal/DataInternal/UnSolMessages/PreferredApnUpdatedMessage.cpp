/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "UnSolMessages/PreferredApnUpdatedMessage.h"

using namespace rildata;

PreferredApnUpdatedMessage::PreferredApnUpdatedMessage
(
  dsd_preferred_apn_updated_ind_msg_v01 const &p
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mResult = p;
}

PreferredApnUpdatedMessage::~PreferredApnUpdatedMessage()
{
}

std::shared_ptr<UnSolicitedMessage> PreferredApnUpdatedMessage::clone()
{
  return std::make_shared<PreferredApnUpdatedMessage>(mResult);
}

const dsd_preferred_apn_updated_ind_msg_v01& PreferredApnUpdatedMessage::getParams()
{
  return mResult;
}

string PreferredApnUpdatedMessage::dump()
{
  return PreferredApnUpdatedMessage::MESSAGE_NAME;
}
