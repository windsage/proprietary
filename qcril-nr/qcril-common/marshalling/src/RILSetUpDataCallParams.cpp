/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILSetUpDataCallParams.h>

template <>
Marshal::Result Marshal::write<RIL_SetUpDataCallParams>(const RIL_SetUpDataCallParams &arg) {
    WRITE_AND_CHECK(static_cast<int>(arg.accessNetwork));
    WRITE_AND_CHECK(arg.profileInfo);
    WRITE_AND_CHECK(arg.roamingAllowed);
    WRITE_AND_CHECK(static_cast<int>(arg.reason));
    WRITE_AND_CHECK(arg.addresses);
    WRITE_AND_CHECK(arg.dnses);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SetUpDataCallParams>(RIL_SetUpDataCallParams &arg) const {
    READ_AND_CHECK(arg.accessNetwork);
    READ_AND_CHECK(arg.profileInfo);
    READ_AND_CHECK(arg.roamingAllowed);
    READ_AND_CHECK(arg.reason);
    READ_AND_CHECK(arg.addresses);
    READ_AND_CHECK(arg.dnses);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SetUpDataCallParams>(RIL_SetUpDataCallParams &arg)
{
    release(arg.accessNetwork);
    release(arg.profileInfo);
    release(arg.roamingAllowed);
    release(arg.reason);
    release(arg.addresses);
    release(arg.dnses);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SetUpDataCallParams_Ursp>(const RIL_SetUpDataCallParams_Ursp &arg) {
    WRITE_AND_CHECK(static_cast<int>(arg.accessNetwork));
    WRITE_AND_CHECK(arg.profileInfo);
    WRITE_AND_CHECK(arg.roamingAllowed);
    WRITE_AND_CHECK(static_cast<int>(arg.reason));
    WRITE_AND_CHECK(arg.addresses);
    WRITE_AND_CHECK(arg.dnses);
    WRITE_AND_CHECK_OPTIONAL(arg.sliceInfoValid, arg.sliceInfo);
    WRITE_AND_CHECK(arg.matchAllRuleAllowed);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SetUpDataCallParams_Ursp>(RIL_SetUpDataCallParams_Ursp &arg) const {
    READ_AND_CHECK(arg.accessNetwork);
    READ_AND_CHECK(arg.profileInfo);
    READ_AND_CHECK(arg.roamingAllowed);
    READ_AND_CHECK(arg.reason);
    READ_AND_CHECK(arg.addresses);
    READ_AND_CHECK(arg.dnses);
    READ_AND_CHECK_OPTIONAL(arg.sliceInfoValid, arg.sliceInfo);
    READ_AND_CHECK(arg.matchAllRuleAllowed);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SetUpDataCallParams_Ursp>(RIL_SetUpDataCallParams_Ursp &arg)
{
    release(arg.accessNetwork);
    release(arg.profileInfo);
    release(arg.roamingAllowed);
    release(arg.reason);
    release(arg.addresses);
    release(arg.dnses);
    release(arg.sliceInfoValid);
    release(arg.sliceInfo);
    release(arg.matchAllRuleAllowed);
    return Result::SUCCESS;
}
