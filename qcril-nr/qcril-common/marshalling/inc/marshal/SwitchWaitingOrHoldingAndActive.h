/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_RIL_SWITCHWAITINGORHOLDINGANDACTIVE
#define _QTI_MARSHALLING_RIL_SWITCHWAITINGORHOLDINGANDACTIVE

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_SwitchWaitingOrHoldingAndActive>(
    const RIL_SwitchWaitingOrHoldingAndActive& arg);
template <>
Marshal::Result Marshal::read<RIL_SwitchWaitingOrHoldingAndActive>(
    RIL_SwitchWaitingOrHoldingAndActive& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SwitchWaitingOrHoldingAndActive>(
    RIL_SwitchWaitingOrHoldingAndActive& arg);

#endif  // _QTI_MARSHALLING_RIL_SWITCHWAITINGORHOLDINGANDACTIVE

