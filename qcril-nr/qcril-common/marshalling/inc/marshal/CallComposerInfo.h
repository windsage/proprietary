/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_CALL_COMPOSER_INFO_H_
#define __QTI_MARSHALLING_CALL_COMPOSER_INFO_H_

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallComposerInfo>(const RIL_CallComposerInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_CallComposerInfo>(RIL_CallComposerInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallComposerInfo>(RIL_CallComposerInfo& arg);

template <>
Marshal::Result Marshal::write<RIL_CallComposerLocation>(const RIL_CallComposerLocation& arg);
template <>
Marshal::Result Marshal::read<RIL_CallComposerLocation>(RIL_CallComposerLocation& arg) const;
template <>
Marshal::Result Marshal::release<RIL_CallComposerLocation>(RIL_CallComposerLocation& arg);

#endif
