/******************************************************************************
  @file    ril_utf_i_ims_aidl_radio_sim.h
  @brief   declare all attributes and attribute classes used in RIL UTF

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef _RIL_UTF_I_IMS_AIDL_RADIO_SIM_H_
#define _RIL_UTF_I_IMS_AIDL_RADIO_SIM_H_

void connect_to_i_ims_aidl_radio();
void reset_i_ims_aidl_radio();
void send_i_ims_aidl_radio_request(ims_MsgType type, ims_MsgId msgId, uint32_t token, void *msg);

#endif
