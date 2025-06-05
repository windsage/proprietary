/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_HANGUP_
#define __QTI_MARSHALLING_HANGUP_

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_Hangup>(const RIL_Hangup& arg);
template <>
Marshal::Result Marshal::read<RIL_Hangup>(RIL_Hangup& arg) const;
template <>
Marshal::Result Marshal::release<RIL_Hangup>(RIL_Hangup& arg);

#endif
