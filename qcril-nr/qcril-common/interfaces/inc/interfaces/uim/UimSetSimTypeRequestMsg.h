/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
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
    Provides Sim type needs to set of requested slot.

    @params
    mSlotMap            - Vector of Sim Types to be Sent
*/
/*==========================================================================*/
class UimSetSimTypeRequestMsg : public SolicitedMessage<RIL_UIM_Errno>,
                                   public add_message_id<UimSetSimTypeRequestMsg>
{


  private:
     std::vector<uint32_t> mSlotMap = {};
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.set_sim_type_request";
     UimSetSimTypeRequestMsg() = delete;
    ~UimSetSimTypeRequestMsg() {}

    UimSetSimTypeRequestMsg(const std::vector<uint32_t> &set_sim_type_map) :
                               SolicitedMessage<RIL_UIM_Errno>(get_class_message_id()),mSlotMap(set_sim_type_map)
    {
      mName = MESSAGE_NAME;
    }

    inline const std::vector<uint32_t> &get_sim_type_mapping(void)
    {
       return mSlotMap;
    }

    inline string dump()
    {
      return mName ;
    }
}; /* UimSetSimTypeRequestMsg */
