/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_SENDUITTYMODEINFO
#define _QTI_MARSHALLING_SENDUITTYMODEINFO

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_TtyNotifyInfo>(const RIL_TtyNotifyInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_TtyNotifyInfo>(RIL_TtyNotifyInfo& arg) const;

#endif
