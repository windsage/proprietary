/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_PARTICIPANT_STATUS_INFO_H_
#define __QTI_MARSHALLING_PARTICIPANT_STATUS_INFO_H_

#include <Marshal.h>
#include <telephony/ril.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_ParticipantStatusInfo>(const RIL_ParticipantStatusInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_ParticipantStatusInfo>(RIL_ParticipantStatusInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_ParticipantStatusInfo>(RIL_ParticipantStatusInfo& arg);

#endif
