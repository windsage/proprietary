/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <marshal/RILDataCallResponsev11.h>
#include <iostream>

template <>
Marshal::Result Marshal::write<RIL_Data_Call_Response_v11>(const RIL_Data_Call_Response_v11 &arg) {
    WRITE_AND_CHECK(arg.status);
    WRITE_AND_CHECK(arg.suggestedRetryTime);
    WRITE_AND_CHECK(arg.cid);
    WRITE_AND_CHECK(arg.active);
    WRITE_AND_CHECK(arg.type);
    WRITE_AND_CHECK(arg.ifname);
    WRITE_AND_CHECK(arg.linkAddresses, arg.linkAddressesLength);
    if (arg.linkAddressesLength == 0) {
        WRITE_AND_CHECK(arg.addresses);
    }
    WRITE_AND_CHECK(arg.dnses);
    WRITE_AND_CHECK(arg.gateways);
    WRITE_AND_CHECK(arg.pcscf);
    WRITE_AND_CHECK(arg.mtu);
    WRITE_AND_CHECK(arg.mtuV4);
    WRITE_AND_CHECK(arg.mtuV6);
    WRITE_AND_CHECK(arg.defaultQos);
    WRITE_AND_CHECK(arg.qosSessions, arg.qosSessionsLength);
    WRITE_AND_CHECK(arg.handoverFailureMode);
    WRITE_AND_CHECK_OPTIONAL(arg.sliceInfoValid, arg.sliceInfo);
    WRITE_AND_CHECK(arg.trafficDescriptors, arg.trafficDescriptorsLength);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Data_Call_Response_v11>(RIL_Data_Call_Response_v11 &arg) const {
    READ_AND_CHECK(arg.status);
    READ_AND_CHECK(arg.suggestedRetryTime);
    READ_AND_CHECK(arg.cid);
    READ_AND_CHECK(arg.active);
    READ_AND_CHECK(arg.type);
    READ_AND_CHECK(arg.ifname);
    READ_AND_CHECK(arg.linkAddresses, arg.linkAddressesLength);
    if (arg.linkAddressesLength == 0) {
        READ_AND_CHECK(arg.addresses);
    }
    READ_AND_CHECK(arg.dnses);
    READ_AND_CHECK(arg.gateways);
    READ_AND_CHECK(arg.pcscf);
    READ_AND_CHECK(arg.mtu);
    READ_AND_CHECK(arg.mtuV4);
    READ_AND_CHECK(arg.mtuV6);
    READ_AND_CHECK(arg.defaultQos);
    READ_AND_CHECK(arg.qosSessions, arg.qosSessionsLength);
    READ_AND_CHECK(arg.handoverFailureMode);
    READ_AND_CHECK_OPTIONAL(arg.sliceInfoValid, arg.sliceInfo);
    READ_AND_CHECK(arg.trafficDescriptors, arg.trafficDescriptorsLength);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Data_Call_Response_v11>(RIL_Data_Call_Response_v11 &arg) {
    release(arg.status);
    release(arg.suggestedRetryTime);
    release(arg.cid);
    release(arg.active);
    release(arg.type);
    release(arg.ifname);
    release(arg.linkAddresses, arg.linkAddressesLength);
    release(arg.addresses);
    release(arg.dnses);
    release(arg.gateways);
    release(arg.pcscf);
    release(arg.mtu);
    release(arg.mtuV4);
    release(arg.mtuV6);
    release(arg.defaultQos);
    release(arg.qosSessions, arg.qosSessionsLength);
    release(arg.handoverFailureMode);
    release(arg.sliceInfoValid);
    release(arg.sliceInfo);
    release(arg.trafficDescriptors, arg.trafficDescriptorsLength);
    return Result::SUCCESS;
}
