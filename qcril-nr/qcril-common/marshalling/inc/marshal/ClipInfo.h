/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CLIPINFO
#define _QTI_MARSHALLING_CLIPINFO

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_ClipInfo>(const RIL_ClipInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_ClipInfo>(RIL_ClipInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_ClipInfo>(RIL_ClipInfo& arg);

#endif
