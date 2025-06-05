/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CAG_INFO
#define _QTI_MARSHALLING_CAG_INFO

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_CagInfo>(const RIL_CagInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CagInfo>(RIL_CagInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CagInfo>(RIL_CagInfo& arg);

#endif
