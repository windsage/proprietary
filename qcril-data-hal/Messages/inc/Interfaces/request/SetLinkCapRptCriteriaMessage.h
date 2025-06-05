#ifndef SETLINKCAPRPTCRITERIAMESSAGE
#define SETLINKCAPRPTCRITERIAMESSAGE

/*===========================================================================

  Copyright (c) 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>
#include "telephony/ril.h"
#include "MessageCommon.h"
#include <vector>

namespace rildata {

/**
 * @brief      Class for ril request set link capability reporting criteria message.
 */
class SetLinkCapRptCriteriaMessage : public SolicitedMessage<LinkCapCriteriaResult_t>,
                                     public add_message_id<SetLinkCapRptCriteriaMessage> {
private:
  int32_t mSerial;
  std::shared_ptr<std::function<void(uint16_t)>> mAcknowlegeRequestCb;
  LinkCapCriteria_t params;

public:
  static constexpr const char* MESSAGE_NAME = "com.qualcomm.qti.qcril.data.SetLinkCapRptCriteriaMessage";
  ~SetLinkCapRptCriteriaMessage() {
    if (mCallback) {
      delete mCallback;
      mCallback = nullptr;
    }
  }
  SetLinkCapRptCriteriaMessage(
        const int32_t serial,
        const LinkCapCriteria_t &r,
        const std::shared_ptr<std::function<void(uint16_t)>> ackCb
  ):SolicitedMessage<LinkCapCriteriaResult_t>(get_class_message_id()),params(r) {
    mName = MESSAGE_NAME;
    mSerial = serial;
    mAcknowlegeRequestCb = ackCb;
  }
  inline SetLinkCapRptCriteriaMessage(
     const LinkCapCriteria_t &p):SolicitedMessage<LinkCapCriteriaResult_t>(get_class_message_id()), params(p) {
     mName = MESSAGE_NAME;
  }
  const LinkCapCriteria_t &getParams() {
    return params;
  }
  string dump() {
    return mName + "{ }";
  }
  int32_t getSerial() {
    return mSerial;
  }
  std::shared_ptr<std::function<void(uint16_t)>> getAcknowlegeRequestCb() {
    return mAcknowlegeRequestCb;
  }
};

}//namespace

#endif