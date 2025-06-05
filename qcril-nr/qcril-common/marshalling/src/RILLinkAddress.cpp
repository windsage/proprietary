/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILLinkAddress.h>

template <>
Marshal::Result Marshal::write<RIL_LinkAddress>(const RIL_LinkAddress &arg) {
    WRITE_AND_CHECK(arg.address);
    WRITE_AND_CHECK(arg.properties);
    WRITE_AND_CHECK(arg.deprecationTime);
    WRITE_AND_CHECK(arg.expirationTime);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_LinkAddress>(RIL_LinkAddress &arg) const {
    READ_AND_CHECK(arg.address);
    READ_AND_CHECK(arg.properties);
    READ_AND_CHECK(arg.deprecationTime);
    READ_AND_CHECK(arg.expirationTime);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_LinkAddress>(RIL_LinkAddress &arg) {
    release(arg.address);
    release(arg.properties);
    release(arg.deprecationTime);
    release(arg.expirationTime);
    return Result::SUCCESS;
}
