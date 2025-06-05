/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CALL_FORWARD_PARAMS
#define _QTI_MARSHALLING_CALL_FORWARD_PARAMS

#include <Marshal.h>
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

#endif
