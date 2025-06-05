/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_CALLWAITING_H_
#define __QTI_MARSHALLING_CALLWAITING_H_

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallWaitingSettings>(const RIL_CallWaitingSettings& arg);

template <>
Marshal::Result Marshal::read<RIL_CallWaitingSettings>(RIL_CallWaitingSettings& arg) const;

template <>
Marshal::Result Marshal::release<RIL_CallWaitingSettings>(RIL_CallWaitingSettings& arg);

template <>
Marshal::Result Marshal::write<RIL_QueryCallWaitingResponse>(const RIL_QueryCallWaitingResponse& arg);

template <>
Marshal::Result Marshal::read<RIL_QueryCallWaitingResponse>(RIL_QueryCallWaitingResponse& arg) const;

template <>
Marshal::Result Marshal::release<RIL_QueryCallWaitingResponse>(RIL_QueryCallWaitingResponse& arg);

#endif
