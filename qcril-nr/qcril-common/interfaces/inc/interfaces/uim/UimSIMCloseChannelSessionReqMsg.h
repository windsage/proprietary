/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "qcril_uim_types.h"

/*===========================================================================*/
/*!
    @brief
    Sends the request to close the SIM AID channel.

    @params
    mChId   - Channel ID of the channel that need to be closed
*/
/*==========================================================================*/
class UimSIMCloseChannelSessionReqMsg
    : public SolicitedMessage<RIL_UIM_Errno>,
      public add_message_id<UimSIMCloseChannelSessionReqMsg> {
private:
  int32_t mChId;
  bool mEs10;

public:
  static constexpr const char *MESSAGE_NAME =
      "com.qualcomm.qti.qcril.uim.close_channel_session_request";
  UimSIMCloseChannelSessionReqMsg() = delete;
  ~UimSIMCloseChannelSessionReqMsg() = default;

  UimSIMCloseChannelSessionReqMsg(int32_t ch_id, bool isEs10)
      : SolicitedMessage<RIL_UIM_Errno>(get_class_message_id()) {
    mChId = ch_id;
    mEs10 = isEs10;
    mName = MESSAGE_NAME;
  }

  inline int32_t get_channel_id(void) { return mChId; }

  inline bool get_Es10(void) { return mEs10; }
  inline string dump() { return mName; }
}; /* UimSIMCloseChannelRequestMsg */
