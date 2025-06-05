/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <Marshal.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_ImeiInfo>(const RIL_ImeiInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_ImeiInfo>(RIL_ImeiInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_ImeiInfo>(RIL_ImeiInfo& arg);