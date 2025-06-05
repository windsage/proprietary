/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_SET_NETWORK_SELECTION_MANUAL_PARAM
#define _QTI_MARSHALLING_SET_NETWORK_SELECTION_MANUAL_PARAM

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SetNetworkSelectionManualParam>(
    const RIL_SetNetworkSelectionManualParam& arg);
template <>
Marshal::Result Marshal::read<RIL_SetNetworkSelectionManualParam>(
    RIL_SetNetworkSelectionManualParam& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SetNetworkSelectionManualParam>(
    RIL_SetNetworkSelectionManualParam& arg);

#endif
