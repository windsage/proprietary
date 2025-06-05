/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RIL_LinkAddress
#define _QTI_MARSHALLING_RIL_LinkAddress

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_LinkAddress>(const RIL_LinkAddress &arg);
template <>
Marshal::Result Marshal::read<RIL_LinkAddress>(RIL_LinkAddress &arg) const;
template <>
Marshal::Result Marshal::release<RIL_LinkAddress>(RIL_LinkAddress &arg);
#endif

