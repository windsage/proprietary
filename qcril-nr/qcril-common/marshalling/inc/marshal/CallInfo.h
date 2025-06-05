/**
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_CALL_INFO_H_
#define __QTI_MARSHALLING_CALL_INFO_H_

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallFailCauseResponse>(const RIL_CallFailCauseResponse& arg);
template <>
Marshal::Result Marshal::read<RIL_CallFailCauseResponse>(RIL_CallFailCauseResponse& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallFailCauseResponse>(RIL_CallFailCauseResponse& arg);

template <>
Marshal::Result Marshal::write<RIL_VerstatInfo>(const RIL_VerstatInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_VerstatInfo>(RIL_VerstatInfo& arg) const;

template <>
Marshal::Result Marshal::write<RIL_CallProgressInfo>(const RIL_CallProgressInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CallProgressInfo>(RIL_CallProgressInfo& arg) const;

template <>
Marshal::Result Marshal::write<RIL_MsimAdditionalCallInfo>(const RIL_MsimAdditionalCallInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_MsimAdditionalCallInfo>(RIL_MsimAdditionalCallInfo& arg) const;

template <>
Marshal::Result Marshal::write<RIL_AudioQuality>(const RIL_AudioQuality& arg);
template <>
Marshal::Result Marshal::read<RIL_AudioQuality>(RIL_AudioQuality& arg) const;
template <>
Marshal::Result Marshal::release<RIL_AudioQuality>(RIL_AudioQuality& arg);

template <>
Marshal::Result Marshal::write<RIL_CallInfo>(const RIL_CallInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CallInfo>(RIL_CallInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallInfo>(RIL_CallInfo& arg);

#endif  // __QTI_MARSHALLING_CALL_INFO_H_
