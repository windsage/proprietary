/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_ANSWER
#define _QTI_MARSHALLING_ANSWER

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_Answer>(const RIL_Answer& arg);
template <>
Marshal::Result Marshal::read<RIL_Answer>(RIL_Answer& arg) const;
template <>
Marshal::Result Marshal::release<RIL_Answer>(RIL_Answer& arg);

#endif
