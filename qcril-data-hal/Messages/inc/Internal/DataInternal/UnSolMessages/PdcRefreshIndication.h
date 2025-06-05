/**
* Copyright (c) 2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef PDCREFRESHINDICATION
#define PDCREFRESHINDICATION
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

/********************** Class Definitions *************************/
class PdcRefreshIndication: public UnSolicitedMessage,
                           public add_message_id<PdcRefreshIndication> {
public:
  static constexpr const char *MESSAGE_NAME = "PdcRefreshIndication";

  PdcRefreshIndication(pdc_refresh_event_enum_v01 event)
  : UnSolicitedMessage(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mEvent = event;
  }
  ~PdcRefreshIndication(){};

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<PdcRefreshIndication>(mEvent);
  }

  string dump() {
    return MESSAGE_NAME;
  }

  pdc_refresh_event_enum_v01 getEvent() {
    return mEvent;
  }

private:
  pdc_refresh_event_enum_v01 mEvent;
};

} //namespace

#endif
