/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CALL
#define _QTI_MARSHALLING_CALL

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_Call>(const RIL_Call& arg);
template <>
Marshal::Result Marshal::read<RIL_Call>(RIL_Call& arg) const;
template <>
Marshal::Result Marshal::release<RIL_Call>(RIL_Call& arg);
#endif
