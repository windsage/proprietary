/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __RFRPE_RILREQUESTSETRFMSCENARIOMESSAGE_H_
#define __RFRPE_RILREQUESTSETRFMSCENARIOMESSAGE_H_
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "services/qtuner_v01.h"

/**
 * Request to set rfm secnarios
 * @Receiver: NasModule
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */

class RilRequestSetRfmScenarioMessage : public QcRilRequestMessage,
                                        public add_message_id<RilRequestSetRfmScenarioMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestSetRfmScenarioMessage";

  RilRequestSetRfmScenarioMessage() = delete;

  explicit inline RilRequestSetRfmScenarioMessage(std::shared_ptr<MessageContext> context,
                                                  Qtuner_set_scenario_req_v01& req)
      : QcRilRequestMessage(get_class_message_id(), context), mRequest(req) {
    mName = MESSAGE_NAME;
  }

  bool isSameMessage(std::shared_ptr<Message> msg) override {
    if (msg) {
      return (get_message_id() == msg->get_message_id());
    }
    return false;
  }

  const Qtuner_set_scenario_req_v01& getRequest() const {
    return mRequest;
  }

 private:
  Qtuner_set_scenario_req_v01 mRequest;
};
#endif
