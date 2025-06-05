/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"

/* Request to get Imei and Type information
   @Receiver: DmsModule
*/

class RilRequestGetImeiMessage : public QcRilRequestMessage,
    public add_message_id<RilRequestGetImeiMessage> {
 private:
  std::optional<uint8_t> mTxId;

public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_GET_IMEI_INFO";

    RilRequestGetImeiMessage() = delete;
    ~RilRequestGetImeiMessage() {}

    explicit inline RilRequestGetImeiMessage(std::shared_ptr<MessageContext> context)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
    }
    bool isSameMessage(std::shared_ptr<Message> msg) override {
      if (msg) {
        return (get_message_id() == msg->get_message_id());
      }
      return false;
    }

  void setTxId(uint8_t txId) { mTxId = txId; }

  uint8_t getTxId() { return *mTxId; }
  bool idTxIdValid() {  return mTxId ? true : false; }
};
