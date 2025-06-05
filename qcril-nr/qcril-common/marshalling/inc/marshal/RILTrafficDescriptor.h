/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RIL_TrafficDescriptor
#define _QTI_MARSHALLING_RIL_TrafficDescriptor

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_TrafficDescriptor>(const RIL_TrafficDescriptor &arg);
template <>
Marshal::Result Marshal::read<RIL_TrafficDescriptor>(RIL_TrafficDescriptor &arg) const;
template <>
Marshal::Result Marshal::release<RIL_TrafficDescriptor>(RIL_TrafficDescriptor &arg);

#endif

