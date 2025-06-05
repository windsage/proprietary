/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILQosSession.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <iostream>
#include <cutils/memory.h>
using namespace std;

TEST(Marshalling, RIL_QosSession_marshal_qosInvalid) {
    RIL_Qos request = {};
    Marshal marshal;
    std::string s =
        "00000000" // qosType none
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, RIL_QosSession_marshal_unmarshal_qosNr) {
    RIL_Qos request = {
        .qosType = QOS_TYPE_NR,
        .nr = {
            .fiveQi = 4,
            .downlink = {
                .maxBitrateKbps = 4000,
                .guaranteedBitrateKbps = 2000,
            },
            .uplink = {
                .maxBitrateKbps = 2000,
                .guaranteedBitrateKbps = 1000,
            },
            .qfi = 2,
            .averagingWindowMs = 40
        },
    };
    Marshal marshal;
    std::string s =
        "00000002" // qosType NR
        "0004" // uint16 fiveQi
        "00000fa0" // dlMaxBitrateKbps
        "000007d0" // dlGuaranteedBitrateKbps
        "000007d0" // ulMaxBitrateKbps
        "000003e8" // ulGuaranteedBitrateKbps
        "02" // uint8 qfi
        "0028" // uint16 averagingWindowMs
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_Qos result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.qosType, result.qosType);
    ASSERT_EQ(request.nr.fiveQi, result.nr.fiveQi);
    ASSERT_EQ(request.nr.downlink.maxBitrateKbps, result.nr.downlink.maxBitrateKbps);
    ASSERT_EQ(request.nr.downlink.guaranteedBitrateKbps, result.nr.downlink.guaranteedBitrateKbps);
    ASSERT_EQ(request.nr.downlink.maxBitrateKbps, result.nr.downlink.maxBitrateKbps);
    ASSERT_EQ(request.nr.downlink.guaranteedBitrateKbps, result.nr.downlink.guaranteedBitrateKbps);
    ASSERT_EQ(request.nr.qfi, result.nr.qfi);
    ASSERT_EQ(request.nr.averagingWindowMs, result.nr.averagingWindowMs);
}

TEST(Marshalling, RIL_QosSession_marshal_qosEps) {
    RIL_Qos request = {
        .qosType = QOS_TYPE_EPS,
        .nr = {
            .fiveQi = 4,
            .downlink = {
                .maxBitrateKbps = 4000,
                .guaranteedBitrateKbps = 2000,
            },
            .uplink = {
                .maxBitrateKbps = 2000,
                .guaranteedBitrateKbps = 1000,
            },
        },
    };
    Marshal marshal;
    std::string s =
        "00000001" // qosType EPS
        "0004" // uint16 fiveQi
        "00000fa0" // dlMaxBitrateKbps
        "000007d0" // dlGuaranteedBitrateKbps
        "000007d0" // ulMaxBitrateKbps
        "000003e8" // ulGuaranteedBitrateKbps
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_Qos result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.qosType, result.qosType);
}

TEST(Marshalling, RIL_QosSession_marshal_qosFilter) {
    RIL_QosFilter request = {
        .localAddressesLength = 0,
        .remoteAddressesLength = 0,
        .localPortValid = 0,
        .remotePortValid = 0,
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 0,
        .flowLabelValid = 0,
        .spiValid = 0,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };
    Marshal marshal;
    std::string s =
        "00000000" // localAddressesLength 0
        "00000000" // remoteAddressesLength 0
        "00"       // localPortValid 0
        "00"       // remotePortValid 0
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "00"       // tosValid 0
        "00"       // flowLabelValid 0
        "00"       // spiValid 0
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, RIL_QosSession_marshal_qosFilterSingleAddress) {
    RIL_QosFilter request = {
        .localAddressesLength = 1,
        .localAddresses = {
            "192.0.1.11",
        },
        .remoteAddressesLength = 1,
        .remoteAddresses = {
            "192.0.1.255",
        },
        .localPortValid = 0,
        .remotePortValid = 0,
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 0,
        .flowLabelValid = 0,
        .spiValid = 0,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };

    Marshal marshal;
    std::string s =
        "00000001" // localAddressesLength 1
        "0000000a" // Length of type 10 bytes
        "3139322e302e312e3131"
        "00000001" // remoteAddressesLength 1
        "0000000b" // Length of type 11 bytes
        "3139322e302e312e323535"
        "00"       // localPortValid 0
        "00"       // remotePortValid 0
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "00"       // tosValid 0
        "00"       // flowLabelValid 0
        "00"       // spiValid 0
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_QosFilter result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.localAddressesLength, result.localAddressesLength);
    ASSERT_STREQ(request.localAddresses[0], result.localAddresses[0]);
    ASSERT_EQ(request.remoteAddressesLength, result.remoteAddressesLength);
    ASSERT_STREQ(request.remoteAddresses[0], result.remoteAddresses[0]);
    ASSERT_EQ(request.localPortValid, result.localPortValid);
    ASSERT_EQ(request.remotePortValid, result.remotePortValid);
    ASSERT_EQ(request.protocol, result.protocol);
    ASSERT_EQ(request.tosValid, result.tosValid);
    ASSERT_EQ(request.flowLabelValid, result.flowLabelValid);
    ASSERT_EQ(request.spiValid, result.spiValid);
    ASSERT_EQ(request.direction, result.direction);
    ASSERT_EQ(request.precedence, result.precedence);
}

TEST(Marshalling, RIL_QosSession_marshal_unmarshal_qosFilterMultipleAddress) {
    RIL_QosFilter request = {
        .localAddressesLength = 2,
        .localAddresses = {
            "192.0.1.11",
            "192.0.1.12",
        },
        .remoteAddressesLength = 2,
        .remoteAddresses = {
            "192.0.1.254",
            "192.0.1.255",
        },
        .localPortValid = 0,
        .remotePortValid = 0,
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 0,
        .flowLabelValid = 0,
        .spiValid = 0,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };
    Marshal marshal;
    std::string s =
        "00000002" // localAddressesLength 2
        "0000000a" // Length of type 10 bytes
        "3139322e302e312e3131"
        "0000000a" // Length of type 10 bytes
        "3139322e302e312e3132"
        "00000002" // remoteAddressesLength 2
        "0000000b" // Length of type 11 bytes
        "3139322e302e312e323534"
        "0000000b" // Length of type 11 bytes
        "3139322e302e312e323535"
        "00"       // localPortValid 0
        "00"       // remotePortValid 0
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "00"       // tosValid 0
        "00"       // flowLabelValid 0
        "00"       // spiValid 0
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_QosFilter result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.localAddressesLength, result.localAddressesLength);
    ASSERT_STREQ(request.localAddresses[0], result.localAddresses[0]);
    ASSERT_STREQ(request.localAddresses[1], result.localAddresses[1]);
    ASSERT_EQ(request.remoteAddressesLength, result.remoteAddressesLength);
    ASSERT_STREQ(request.remoteAddresses[0], result.remoteAddresses[0]);
    ASSERT_STREQ(request.remoteAddresses[1], result.remoteAddresses[1]);
    ASSERT_EQ(request.localPortValid, result.localPortValid);
    ASSERT_EQ(request.remotePortValid, result.remotePortValid);
    ASSERT_EQ(request.protocol, result.protocol);
    ASSERT_EQ(request.tosValid, result.tosValid);
    ASSERT_EQ(request.flowLabelValid, result.flowLabelValid);
    ASSERT_EQ(request.spiValid, result.spiValid);
    ASSERT_EQ(request.direction, result.direction);
    ASSERT_EQ(request.precedence, result.precedence);
}

TEST(Marshalling, RIL_QosSession_marshal_qosFilterPorts) {
    RIL_QosFilter request = {
        .localAddressesLength = 0,
        .remoteAddressesLength = 0,
        .localPortValid = 1,
        .localPort = {
            .start = 55,
            .end = 3333,
        },
        .remotePortValid = 1,
        .remotePort = {
            .start = 99,
            .end = 555,
        },
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 0,
        .flowLabelValid = 0,
        .spiValid = 0,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };
    Marshal marshal;
    std::string s =
        "00000000" // localAddressesLength 0
        "00000000" // remoteAddressesLength 0
        "01"       // localPortValid 1
        "00000037" // start 55
        "00000d05" // end 3333
        "01"       // remotePortValid 1
        "00000063" // start 99
        "0000022b" // end 555
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "00"       // tosValid 0
        "00"       // flowLabelValid 0
        "00"       // spiValid 0
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, RIL_QosSession_marshal_qosFilterTos) {
    RIL_QosFilter request = {
        .localAddressesLength = 0,
        .remoteAddressesLength = 0,
        .localPortValid = 0,
        .remotePortValid = 0,
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 1,
        .tos = 3,
        .flowLabelValid = 0,
        .spiValid = 0,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };
    Marshal marshal;
    std::string s =
        "00000000" // localAddressesLength 0
        "00000000" // remoteAddressesLength 0
        "00"       // localPortValid 0
        "00"       // remotePortValid 0
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "01"       // tosValid 1
        "03"       // tos 3
        "00"       // flowLabelValid 0
        "00"       // spiValid 0
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, RIL_QosSession_marshal_qosFilterFlowLabel) {
    RIL_QosFilter request = {
        .localAddressesLength = 0,
        .remoteAddressesLength = 0,
        .localPortValid = 0,
        .remotePortValid = 0,
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 0,
        .flowLabelValid = 1,
        .flowLabel = 55,
        .spiValid = 0,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };
    Marshal marshal;
    std::string s =
        "00000000" // localAddressesLength 0
        "00000000" // remoteAddressesLength 0
        "00"       // localPortValid 0
        "00"       // remotePortValid 0
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "00"       // tosValid 0
        "01"       // flowLabelValid 1
        "00000037" // flowLabel 55
        "00"       // spiValid 0
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, RIL_QosSession_marshal_qosFilterSpi) {
    RIL_QosFilter request = {
        .localAddressesLength = 0,
        .remoteAddressesLength = 0,
        .localPortValid = 0,
        .remotePortValid = 0,
        .protocol = RIL_QOS_PROTOCOL_TCP,
        .tosValid = 0,
        .flowLabelValid = 0,
        .spiValid = 1,
        .spi = 55,
        .direction = RIL_QOS_FILTER_DIRECTION_UPLINK,
        .precedence = 1,
    };
    Marshal marshal;
    std::string s =
        "00000000" // localAddressesLength 0
        "00000000" // remoteAddressesLength 0
        "00"       // localPortValid 0
        "00"       // remotePortValid 0
        "00000006" // RIL_QOS_PROTOCOL_TCP
        "00"       // tosValid 0
        "00"       // flowLabelValid 0
        "01"       // spiValid 1
        "00000037" // spi 55
        "00000001" // RIL_QOS_FILTER_DIRECTION_UPLINK
        "00000001" // precedence 1
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}
