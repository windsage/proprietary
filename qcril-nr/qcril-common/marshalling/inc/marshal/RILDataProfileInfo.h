/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RILDataProfileInfo
#define _QTI_MARSHALLING_RILDataProfileInfo

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_DataProfileInfo>(const RIL_DataProfileInfo &arg);
template <>
Marshal::Result Marshal::read<RIL_DataProfileInfo>(RIL_DataProfileInfo &arg) const;
template <>
Marshal::Result Marshal::release<RIL_DataProfileInfo>(RIL_DataProfileInfo &arg);
#endif

