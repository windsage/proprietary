/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"
#include "qcril_uim_types.h"

/*===========================================================================*/
/*!
    @brief
    Provides Card status of requested slot.

    @params
    mSlot            - In Slot on which Card status needed
*/
/*==========================================================================*/
class UimGetPersoLockStatusRequestMsg
    : public SolicitedSyncMessage<RIL_UIM_Perso_lock_Status>,
      public add_message_id<UimGetPersoLockStatusRequestMsg> {
private:
  uint8_t mSlot;

public:
  static constexpr const char *MESSAGE_NAME =
      "com.qualcomm.qti.qcril.uim.get_perso_lock_status_sync_request";
  UimGetPersoLockStatusRequestMsg() = delete;
  ~UimGetPersoLockStatusRequestMsg() = default;

  UimGetPersoLockStatusRequestMsg(uint8_t slot)
      : SolicitedSyncMessage<RIL_UIM_Perso_lock_Status>(
            get_class_message_id()) {
    mSlot = slot;
    mName = MESSAGE_NAME;
  }

  inline uint8_t get_slot(void) { return mSlot; }

  inline string dump() {
    return mName + "{ slot :" + std::to_string(mSlot) + "}";
  }
}; /* UimGetCardStatusRequestMsg */
