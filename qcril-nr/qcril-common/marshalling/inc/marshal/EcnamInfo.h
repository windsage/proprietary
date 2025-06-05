/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_ECNAM_INFO_H_
#define __QTI_MARSHALLING_ECNAM_INFO_H_

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_EcnamInfo>(const RIL_EcnamInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_EcnamInfo>(RIL_EcnamInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_EcnamInfo>(RIL_EcnamInfo& arg);

#endif
