/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_REFRESH_CONFERENCE
#define _QTI_MARSHALLING_REFRESH_CONFERENCE

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_RefreshConferenceInfo>(const RIL_RefreshConferenceInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_RefreshConferenceInfo>(RIL_RefreshConferenceInfo &arg) const;
template <>
Marshal::Result Marshal::release<RIL_RefreshConferenceInfo>(RIL_RefreshConferenceInfo &arg);

#endif
