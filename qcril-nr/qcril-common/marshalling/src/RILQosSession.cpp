/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>
#include <marshal/RILQosSession.h>

template <>
Marshal::Result Marshal::write<RIL_QosBandwidth>(const RIL_QosBandwidth &arg) {
    WRITE_AND_CHECK(arg.maxBitrateKbps);
    WRITE_AND_CHECK(arg.guaranteedBitrateKbps);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_QosBandwidth>(RIL_QosBandwidth &arg) const {
    READ_AND_CHECK(arg.maxBitrateKbps);
    READ_AND_CHECK(arg.guaranteedBitrateKbps);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_QosBandwidth>(RIL_QosBandwidth &arg) {
    release(arg.maxBitrateKbps);
    release(arg.guaranteedBitrateKbps);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_EpsQos>(const RIL_EpsQos &arg) {
    WRITE_AND_CHECK(arg.qci);
    WRITE_AND_CHECK(arg.downlink);
    WRITE_AND_CHECK(arg.uplink);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_EpsQos>(RIL_EpsQos &arg) const {
    READ_AND_CHECK(arg.qci);
    READ_AND_CHECK(arg.downlink);
    READ_AND_CHECK(arg.uplink);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_EpsQos>(RIL_EpsQos &arg) {
    release(arg.qci);
    release(arg.downlink);
    release(arg.uplink);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_NrQos>(const RIL_NrQos &arg) {
    WRITE_AND_CHECK(arg.fiveQi);
    WRITE_AND_CHECK(arg.downlink);
    WRITE_AND_CHECK(arg.uplink);
    WRITE_AND_CHECK(arg.qfi);
    WRITE_AND_CHECK(arg.averagingWindowMs);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_NrQos>(RIL_NrQos &arg) const {
    READ_AND_CHECK(arg.fiveQi);
    READ_AND_CHECK(arg.downlink);
    READ_AND_CHECK(arg.uplink);
    READ_AND_CHECK(arg.qfi);
    READ_AND_CHECK(arg.averagingWindowMs);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_NrQos>(RIL_NrQos &arg) {
    release(arg.fiveQi);
    release(arg.downlink);
    release(arg.uplink);
    release(arg.qfi);
    release(arg.averagingWindowMs);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_Qos>(const RIL_Qos &arg) {
    WRITE_AND_CHECK(arg.qosType);
    if (arg.qosType == QOS_TYPE_EPS) {
        WRITE_AND_CHECK(arg.eps);
    } else if (arg.qosType == QOS_TYPE_NR) {
        WRITE_AND_CHECK(arg.nr);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Qos>(RIL_Qos &arg) const {
    READ_AND_CHECK(arg.qosType);
    if (arg.qosType == QOS_TYPE_EPS) {
        READ_AND_CHECK(arg.eps);
    } else if (arg.qosType == QOS_TYPE_NR) {
        READ_AND_CHECK(arg.nr);
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Qos>(RIL_Qos &arg) {
    release(arg.qosType);
    release(arg.eps);
    release(arg.nr);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_PortRange>(const RIL_PortRange &arg) {
    WRITE_AND_CHECK(arg.start);
    WRITE_AND_CHECK(arg.end);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_PortRange>(RIL_PortRange &arg) const {
    READ_AND_CHECK(arg.start);
    READ_AND_CHECK(arg.end);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_PortRange>(RIL_PortRange &arg) {
    release(arg.start);
    release(arg.end);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_QosFilter>(const RIL_QosFilter &arg) {
    WRITE_AND_CHECK(arg.localAddresses, arg.localAddressesLength);
    WRITE_AND_CHECK(arg.remoteAddresses, arg.remoteAddressesLength);
    WRITE_AND_CHECK_OPTIONAL(arg.localPortValid, arg.localPort);
    WRITE_AND_CHECK_OPTIONAL(arg.remotePortValid, arg.remotePort);
    WRITE_AND_CHECK(arg.protocol);
    WRITE_AND_CHECK_OPTIONAL(arg.tosValid, arg.tos);
    WRITE_AND_CHECK_OPTIONAL(arg.flowLabelValid, arg.flowLabel);
    WRITE_AND_CHECK_OPTIONAL(arg.spiValid, arg.spi);
    WRITE_AND_CHECK(arg.direction);
    WRITE_AND_CHECK(arg.precedence);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_QosFilter>(RIL_QosFilter &arg) const {
    READ_AND_CHECK(arg.localAddresses, arg.localAddressesLength);
    READ_AND_CHECK(arg.remoteAddresses, arg.remoteAddressesLength);
    READ_AND_CHECK_OPTIONAL(arg.localPortValid, arg.localPort);
    READ_AND_CHECK_OPTIONAL(arg.remotePortValid, arg.remotePort);
    READ_AND_CHECK(arg.protocol);
    READ_AND_CHECK_OPTIONAL(arg.tosValid, arg.tos);
    READ_AND_CHECK_OPTIONAL(arg.flowLabelValid, arg.flowLabel);
    READ_AND_CHECK_OPTIONAL(arg.spiValid, arg.spi);
    READ_AND_CHECK(arg.direction);
    READ_AND_CHECK(arg.precedence);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_QosFilter>(RIL_QosFilter &arg) {
    release(arg.localAddresses, arg.localAddressesLength);
    release(arg.remoteAddresses, arg.remoteAddressesLength);
    release(arg.localPortValid);
    release(arg.localPort);
    release(arg.remotePortValid);
    release(arg.remotePort);
    release(arg.protocol);
    release(arg.tosValid);
    release(arg.tos);
    release(arg.flowLabelValid);
    release(arg.flowLabel);
    release(arg.spiValid);
    release(arg.spi);
    release(arg.direction);
    release(arg.precedence);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_QosSession>(const RIL_QosSession &arg) {
    WRITE_AND_CHECK(arg.qosSessionId);
    WRITE_AND_CHECK(arg.qos);
    WRITE_AND_CHECK(arg.qosFilters, arg.qosFiltersLength);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_QosSession>(RIL_QosSession &arg) const {
    READ_AND_CHECK(arg.qosSessionId);
    READ_AND_CHECK(arg.qos);
    READ_AND_CHECK(arg.qosFilters, arg.qosFiltersLength);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_QosSession>(RIL_QosSession &arg) {
    release(arg.qosSessionId);
    release(arg.qos);
    release(arg.qosFilters, arg.qosFiltersLength);
    return Result::SUCCESS;
}
