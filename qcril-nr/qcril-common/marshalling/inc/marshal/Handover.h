/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_HANDOVER
#define _QTI_MARSHALLING_HANDOVER

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_HandoverInfo>(const RIL_HandoverInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_HandoverInfo>(RIL_HandoverInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_HandoverInfo>(RIL_HandoverInfo& arg);

#endif  // _QTI_MARSHALLING_HANDOVER
