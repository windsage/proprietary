/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_COLRINFO
#define _QTI_MARSHALLING_COLRINFO

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_ColrInfo>(const RIL_ColrInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_ColrInfo>(RIL_ColrInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_ColrInfo>(RIL_ColrInfo& arg);

#endif
