/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RADIO_POWER
#define _QTI_MARSHALLING_RADIO_POWER

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_RadioPowerReq>(const RIL_RadioPowerReq &arg);
template <>
Marshal::Result Marshal::read<RIL_RadioPowerReq>(RIL_RadioPowerReq &arg) const;

#endif // _QTI_MARSHALLING_RADIO_POWER
