/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_SNPN_INFO
#define _QTI_MARSHALLING_SNPN_INFO

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SnpnInfo>(const RIL_SnpnInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_SnpnInfo>(RIL_SnpnInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SnpnInfo>(RIL_SnpnInfo& arg);

#endif
