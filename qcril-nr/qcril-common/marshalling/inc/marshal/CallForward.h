/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_CALL_FORWARD_H_
#define __QTI_MARSHALLING_CALL_FORWARD_H_

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallFwdTimerInfo>(const RIL_CallFwdTimerInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CallFwdTimerInfo>(RIL_CallFwdTimerInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallFwdTimerInfo>(RIL_CallFwdTimerInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_CallForwardParams>(const RIL_CallForwardParams& arg);
template <>
Marshal::Result Marshal::read<RIL_CallForwardParams>(RIL_CallForwardParams& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallForwardParams>(RIL_CallForwardParams& arg);

template <>
Marshal::Result Marshal::write<RIL_QueryCallForwardStatusInfo>(
    const RIL_QueryCallForwardStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_QueryCallForwardStatusInfo>(
    RIL_QueryCallForwardStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_QueryCallForwardStatusInfo>(RIL_QueryCallForwardStatusInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_SetCallForwardStatusInfo>(const RIL_SetCallForwardStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_SetCallForwardStatusInfo>(RIL_SetCallForwardStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SetCallForwardStatusInfo>(RIL_SetCallForwardStatusInfo& arg);

#endif
