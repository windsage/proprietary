/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SysSelChannels.h>

template <>
Marshal::Result Marshal::write<RIL_SysSelChannels>(const RIL_SysSelChannels& arg) {
    WRITE_AND_CHECK(arg.specifiers_latest, static_cast<std::size_t>(arg.specifiers_length));
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SysSelChannels>(RIL_SysSelChannels& arg) const {
    std::size_t len = 0;
    RUN_AND_CHECK(readAndAlloc(arg.specifiers_latest, len));
    arg.specifiers_length = len;
    return Result::SUCCESS;
}


