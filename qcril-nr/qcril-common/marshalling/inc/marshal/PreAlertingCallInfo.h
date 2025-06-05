/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_PRE_ALERTING_CALL_INFO_H_
#define __QTI_MARSHALLING_PRE_ALERTING_CALL_INFO_H_

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::read<RIL_PreAlertingCallInfo>(RIL_PreAlertingCallInfo& arg) const;
template <>
Marshal::Result Marshal::write<RIL_PreAlertingCallInfo>(const RIL_PreAlertingCallInfo& arg);
template <>
Marshal::Result Marshal::release<RIL_PreAlertingCallInfo>(RIL_PreAlertingCallInfo& arg);
#endif
