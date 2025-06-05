/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_DEFLECTCALLINFO_H_
#define __QTI_MARSHALLING_DEFLECTCALLINFO_H_

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_DeflectCallInfo>(const RIL_DeflectCallInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_DeflectCallInfo>(RIL_DeflectCallInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_DeflectCallInfo>(RIL_DeflectCallInfo& arg);

#endif
