/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CLIRINFO
#define _QTI_MARSHALLING_CLIRINFO

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_ClirInfo>(const RIL_ClirInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_ClirInfo>(RIL_ClirInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_ClirInfo>(RIL_ClirInfo& arg);

#endif
