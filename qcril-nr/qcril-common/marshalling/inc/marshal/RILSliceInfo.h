/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RIL_SliceInfo
#define _QTI_MARSHALLING_RIL_SliceInfo

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SliceInfo>(const RIL_SliceInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_SliceInfo>(RIL_SliceInfo &arg) const;
template <>
Marshal::Result Marshal::release<RIL_SliceInfo>(RIL_SliceInfo &arg);
#endif

