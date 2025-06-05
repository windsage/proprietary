/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <type_traits>
#include <telephony/ril.h>
#include <marshal/RILRadioPower.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_RadioPowerReq>(const RIL_RadioPowerReq &arg) {
    WRITE_AND_CHECK(arg.state);
    WRITE_AND_CHECK(arg.forEmergencyCall);
    WRITE_AND_CHECK(arg.preferredForEmergencyCall);
    return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::read<RIL_RadioPowerReq>(RIL_RadioPowerReq &arg) const {
    READ_AND_CHECK(arg.state);
    READ_AND_CHECK(arg.forEmergencyCall);
    READ_AND_CHECK(arg.preferredForEmergencyCall);
    return Result::SUCCESS;
}
