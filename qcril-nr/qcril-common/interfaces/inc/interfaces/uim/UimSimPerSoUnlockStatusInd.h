/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "qcril_uim_types.h"

/*===========================================================================*/
/*!
    @brief
    UIM Refresh indication message.

    @params
    mRefreshInd  - Refresh indication data

    @return
    None.
*/
/*=========================================================================*/
class UimSimPersoLockStatusInd
    : public UnSolicitedMessage,
      public add_message_id<UimSimPersoLockStatusInd> {
private:
  RIL_UIM_Perso_lock_Status mPersoLockStatus;

public:
  static constexpr const char *MESSAGE_NAME =
      "com.qualcomm.qti.qcril.uim.sim_perso_lock_status_indication";
  UimSimPersoLockStatusInd() = delete;
  ~UimSimPersoLockStatusInd() = default;

  inline UimSimPersoLockStatusInd(
      const RIL_UIM_Perso_lock_Status &perso_lock_status)
      : UnSolicitedMessage(get_class_message_id()) {
    mPersoLockStatus = perso_lock_status;
    mName = MESSAGE_NAME;
  }

  inline string dump() { return mName; }

  inline std::shared_ptr<UnSolicitedMessage> clone() {
    return std::static_pointer_cast<UnSolicitedMessage>(
        std::make_shared<UimSimPersoLockStatusInd>(mPersoLockStatus));
  }

  inline RIL_UIM_Perso_lock_Status &get_persolock_ind(void) {
    return mPersoLockStatus;
  }
}; /* UimSimRefreshIndication */
