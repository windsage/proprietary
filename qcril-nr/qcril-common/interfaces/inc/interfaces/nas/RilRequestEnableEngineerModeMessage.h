/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __NAS_RILREQUESTENABLEENGINEERMODEMESSAGE_H_
#define __NAS_RILREQUESTENABLEENGINEERMODEMESSAGE_H_
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nas/nas_types.h"

/**
 * Request to get preferred network band pref.
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 **/
class RilRequestEnableEngineerModeMessage
    : public QcRilRequestMessage,
      public add_message_id<RilRequestEnableEngineerModeMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestEnableEngineerModeMessage";

  RilRequestEnableEngineerModeMessage() = delete;

  explicit inline RilRequestEnableEngineerModeMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool isEnable() const {
    return mEnable;
  }

  oem_hook_ftm_subscription_source_e_type getSubsType() const {
    return mSubsType;
  }

  RIL_Errno setData(const uint8_t* in, uint32_t inLen) {
    if (!in || !inLen) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    uint32_t subsSrcType = 0;
    if (inLen < sizeof(subsSrcType)) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    memcpy(&subsSrcType, in, sizeof(subsSrcType));
    mSubsType = static_cast<oem_hook_ftm_subscription_source_e_type>(subsSrcType);
    inLen -= sizeof(subsSrcType);
    in += sizeof(subsSrcType);

    uint32_t enable = 0;
    if (inLen < sizeof(enable)) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    memcpy(&enable, in, sizeof(enable));
    mEnable = enable;
    return RIL_E_SUCCESS;
  }

  virtual string dump() {
    std::string os;
    os += QcRilRequestMessage::dump();
    os += "{";
    os += std::string(".mEnable=") + (mEnable ? "TRUE" : "FALSE");
    os += ".mSubsType=" + std::to_string(mSubsType);;
    os += "}";
    return os;
  }
 private:
  bool mEnable;
  oem_hook_ftm_subscription_source_e_type mSubsType;
};
#endif
