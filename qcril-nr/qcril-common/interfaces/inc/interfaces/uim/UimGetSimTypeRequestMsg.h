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
    Retrieves the SimTypes in the device on both slots.

*/
/*==========================================================================*/
class UimGetSimTypeRequestMsg : public SolicitedMessage<RIL_UIM_GetSimTypeResp>,
                                   public add_message_id<UimGetSimTypeRequestMsg>
{

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_sim_type_request";
    ~UimGetSimTypeRequestMsg() {}

    UimGetSimTypeRequestMsg() :
                               SolicitedMessage<RIL_UIM_GetSimTypeResp>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }


    inline string dump()
    {
      return mName ;
    }
}; /* UimGetSimTypeRequestMsg */
