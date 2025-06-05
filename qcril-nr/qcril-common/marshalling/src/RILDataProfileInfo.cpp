/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILDataProfileInfo.h>

template <>
Marshal::Result Marshal::write<RIL_DataProfileInfo>(const RIL_DataProfileInfo &arg) {
    WRITE_AND_CHECK(arg.profileId);
    WRITE_AND_CHECK(arg.apn);
    WRITE_AND_CHECK(arg.protocol);
    WRITE_AND_CHECK(arg.roamingProtocol);
    WRITE_AND_CHECK(arg.authType);
    WRITE_AND_CHECK(arg.user);
    WRITE_AND_CHECK(arg.password);
    WRITE_AND_CHECK(arg.type);
    WRITE_AND_CHECK(arg.maxConnsTime);
    WRITE_AND_CHECK(arg.maxConns);
    WRITE_AND_CHECK(arg.waitTime);
    WRITE_AND_CHECK(arg.enabled);
    WRITE_AND_CHECK(arg.supportedTypesBitmask);
    WRITE_AND_CHECK(arg.bearerBitmask);
    WRITE_AND_CHECK(arg.mtu);
    WRITE_AND_CHECK(arg.mtuV4);
    WRITE_AND_CHECK(arg.mtuV6);
    WRITE_AND_CHECK(arg.preferred);
    WRITE_AND_CHECK(arg.persistent);
    WRITE_AND_CHECK_OPTIONAL(arg.trafficDescriptorValid, arg.trafficDescriptor);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_DataProfileInfo>(RIL_DataProfileInfo &arg) const {
    READ_AND_CHECK(arg.profileId);
    READ_AND_CHECK(arg.apn);
    READ_AND_CHECK(arg.protocol);
    READ_AND_CHECK(arg.roamingProtocol);
    READ_AND_CHECK(arg.authType);
    READ_AND_CHECK(arg.user);
    READ_AND_CHECK(arg.password);
    READ_AND_CHECK(arg.type);
    READ_AND_CHECK(arg.maxConnsTime);
    READ_AND_CHECK(arg.maxConns);
    READ_AND_CHECK(arg.waitTime);
    READ_AND_CHECK(arg.enabled);
    READ_AND_CHECK(arg.supportedTypesBitmask);
    READ_AND_CHECK(arg.bearerBitmask);
    READ_AND_CHECK(arg.mtu);
    READ_AND_CHECK(arg.mtuV4);
    READ_AND_CHECK(arg.mtuV6);
    READ_AND_CHECK(arg.preferred);
    READ_AND_CHECK(arg.persistent);
    READ_AND_CHECK_OPTIONAL(arg.trafficDescriptorValid, arg.trafficDescriptor);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_DataProfileInfo>(RIL_DataProfileInfo &arg)
{
    release(arg.profileId);
    release(arg.apn);
    release(arg.protocol);
    release(arg.roamingProtocol);
    release(arg.authType);
    release(arg.user);
    release(arg.password);
    release(arg.type);
    release(arg.maxConnsTime);
    release(arg.maxConns);
    release(arg.waitTime);
    release(arg.enabled);
    release(arg.supportedTypesBitmask);
    release(arg.bearerBitmask);
    release(arg.mtu);
    release(arg.mtuV4);
    release(arg.mtuV6);
    release(arg.preferred);
    release(arg.persistent);
    release(arg.trafficDescriptorValid);
    release(arg.trafficDescriptor);
    return Result::SUCCESS;
}
