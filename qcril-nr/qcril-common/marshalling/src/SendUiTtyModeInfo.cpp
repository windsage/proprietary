/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SendUiTtyModeInfo.h>

template <>
Marshal::Result Marshal::write<RIL_TtyNotifyInfo>(const RIL_TtyNotifyInfo& arg) {
    WRITE_AND_CHECK(arg.mode);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_TtyNotifyInfo>(RIL_TtyNotifyInfo& arg) const {
    READ_AND_CHECK(arg.mode);
    return Result::SUCCESS;
}

