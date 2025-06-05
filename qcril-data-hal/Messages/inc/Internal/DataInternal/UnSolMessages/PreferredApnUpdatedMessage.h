/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef PREFERREDAPNUPDATEDMESSAGE
#define PREFERREDAPNUPDATEDMESSAGE
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "data_system_determination_v01.h"

namespace rildata {

/********************** Class Definitions *************************/
class PreferredApnUpdatedMessage: public UnSolicitedMessage,
                           public add_message_id<PreferredApnUpdatedMessage> {

private:
  dsd_preferred_apn_updated_ind_msg_v01 mResult;

public:
  static constexpr const char *MESSAGE_NAME = "PreferredApnUpdatedMessage";

  PreferredApnUpdatedMessage(dsd_preferred_apn_updated_ind_msg_v01 const &p);
  ~PreferredApnUpdatedMessage();

  std::shared_ptr<UnSolicitedMessage> clone();

  const dsd_preferred_apn_updated_ind_msg_v01 & getParams();

  string dump();
};

} //namespace

#endif