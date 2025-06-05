/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_EXPLICITCALLTRANSFER
#define _QTI_MARSHALLING_EXPLICITCALLTRANSFER

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_ExplicitCallTransfer>(const RIL_ExplicitCallTransfer& arg);
template <>
Marshal::Result Marshal::read<RIL_ExplicitCallTransfer>(RIL_ExplicitCallTransfer& arg) const;
template <>
Marshal::Result Marshal::release<RIL_ExplicitCallTransfer>(RIL_ExplicitCallTransfer& arg);
#endif
