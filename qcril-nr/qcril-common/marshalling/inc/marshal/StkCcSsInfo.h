/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_STK_CC_SS_INFO
#define _QTI_MARSHALLING_STK_CC_SS_INFO

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallForwardData>(const RIL_CallForwardData& arg);
template <>
Marshal::Result Marshal::read<RIL_CallForwardData>(RIL_CallForwardData& arg) const;

template <>
Marshal::Result Marshal::write<RIL_StkCcSsInfo>(const RIL_StkCcSsInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_StkCcSsInfo>(RIL_StkCcSsInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_StkCcSsInfo>(RIL_StkCcSsInfo& arg);

#endif
