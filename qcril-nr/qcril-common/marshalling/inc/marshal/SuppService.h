/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_SUPP_SVC_REQUEST
#define _QTI_MARSHALLING_SUPP_SVC_REQUEST

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_SuppSvcRequest>(const RIL_SuppSvcRequest& arg);
template <>
Marshal::Result Marshal::read<RIL_SuppSvcRequest>(RIL_SuppSvcRequest& arg) const;

template <>
Marshal::Result Marshal::write<RIL_SuppSvcResponse>(const RIL_SuppSvcResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_SuppSvcResponse>(RIL_SuppSvcResponse& arg) const;

template <>
Marshal::Result Marshal::release<RIL_SuppSvcResponse>(RIL_SuppSvcResponse& arg);

#endif
