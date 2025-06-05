/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_DIAL
#define _QTI_MARSHALLING_DIAL

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_MultiIdentityLineInfo>(const RIL_MultiIdentityLineInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_MultiIdentityLineInfo>(RIL_MultiIdentityLineInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_MultiIdentityLineInfo>(RIL_MultiIdentityLineInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_CallDetails>(const RIL_CallDetails& arg);
template <>
Marshal::Result Marshal::read<RIL_CallDetails>(RIL_CallDetails& arg) const;

template <>
Marshal::Result Marshal::write<RIL_DialParams>(const RIL_DialParams& arg);
template <>
Marshal::Result Marshal::read<RIL_DialParams>(RIL_DialParams& arg) const;
template <>
Marshal::Result Marshal::release<RIL_DialParams>(RIL_DialParams& arg);

#endif
