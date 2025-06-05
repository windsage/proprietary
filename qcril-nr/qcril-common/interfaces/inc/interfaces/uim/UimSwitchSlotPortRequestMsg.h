/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include <vector>

/*===========================================================================*/
/*!
    @brief
    Switch slot request message

    Retruns RIL_UIM_Errno shared pointer
*/
/*==========================================================================*/
class UimSwitchSlotPortRequestMsg
    : public SolicitedMessage<RIL_UIM_Errno>,
      public add_message_id<UimSwitchSlotPortRequestMsg> {
private:
  std::vector<RIL_UIM_SwitchSlot> mSlotMap;

public:
  static constexpr const char *MESSAGE_NAME =
      "com.qualcomm.qti.qcril.uim.switch_slot_port_request";
  UimSwitchSlotPortRequestMsg() = delete;
  ~UimSwitchSlotPortRequestMsg() = default;

  UimSwitchSlotPortRequestMsg(const std::vector<RIL_UIM_SwitchSlot> &slot_map)
      : SolicitedMessage<RIL_UIM_Errno>(get_class_message_id()),
        mSlotMap(slot_map) {
    mName = MESSAGE_NAME;
  }

  inline const std::vector<RIL_UIM_SwitchSlot> &get_slot_mapping(void) {
    return mSlotMap;
  }

  inline string dump() { return mName; }
};
