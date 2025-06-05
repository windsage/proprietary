/******************************************************************************
#  Copyright (c) 2021 - 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <optional>

class RilRequestSetVonrCapRequest: public QcRilRequestMessage,
                                      public add_message_id<RilRequestSetVonrCapRequest> {
  private:
    std::optional<bool> mVonrEnabled;
  public:
    static constexpr const char *MESSAGE_NAME = "RilRequestSetVonrCapRequest";

    RilRequestSetVonrCapRequest() = delete;

    ~RilRequestSetVonrCapRequest() {};

    inline RilRequestSetVonrCapRequest(std::shared_ptr<MessageContext> context,
            std::optional<bool> enabled)
        : QcRilRequestMessage(get_class_message_id(), context) {
      mName = MESSAGE_NAME;
      mVonrEnabled = enabled;
    }

    bool getVonrEnabled() {
        return *mVonrEnabled;
    }

    string dump() {
      return mName + "{setVonrEnabled=" +
          (mVonrEnabled ? (*mVonrEnabled ? "true" : "false") : "<invalid>")+ "}";
    }
};
