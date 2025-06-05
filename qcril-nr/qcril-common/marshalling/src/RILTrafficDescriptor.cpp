/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILTrafficDescriptor.h>

template <>
Marshal::Result Marshal::write<RIL_TrafficDescriptor>(const RIL_TrafficDescriptor &arg) {
    WRITE_AND_CHECK_OPTIONAL(arg.dnnValid, arg.dnn);
    WRITE_AND_CHECK(arg.osAppIdValid);
    if (arg.osAppIdValid) {
        WRITE_AND_CHECK(arg.osAppId, arg.osAppIdLength);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_TrafficDescriptor>(RIL_TrafficDescriptor &arg) const {
    READ_AND_CHECK_OPTIONAL(arg.dnnValid, arg.dnn);
    READ_AND_CHECK(arg.osAppIdValid);
    if (arg.osAppIdValid) {
        READ_AND_CHECK(arg.osAppId, arg.osAppIdLength);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_TrafficDescriptor>(RIL_TrafficDescriptor &arg) {
    release(arg.dnnValid);
    release(arg.dnn);
    release(arg.osAppIdValid);
    release(arg.osAppId, arg.osAppIdLength);
    return Result::SUCCESS;
}
