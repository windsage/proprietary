/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_CALL_MODIFY_INFO
#define _QTI_MARSHALLING_CALL_MODIFY_INFO

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallModifyInfo>(const RIL_CallModifyInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CallModifyInfo>(RIL_CallModifyInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallModifyInfo>(RIL_CallModifyInfo& arg);

#endif
