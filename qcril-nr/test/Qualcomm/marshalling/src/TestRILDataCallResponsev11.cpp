/*
 * Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILDataCallResponsev11.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <iostream>
using namespace std;

 TEST(Marshalling, RIL_Data_Call_Response_v11_marshal) {
    RIL_Data_Call_Response_v11 request = {};
    Marshal marshal;
    request.status = 1;
    request.suggestedRetryTime = 2;
    request.cid = 1;
    request.active = 2; // physcial link up
    request.type = "IPV4V6";
    request.ifname = "dummy_rmnet_data0";
    request.dnses =     "192.0.1.11 2001:db8::1";
    request.addresses = "192.0.1.11 2001:db8::1";
    request.gateways =  "192.0.2.155 2001:db8::1";
    request.pcscf = "2001:c28::1";
    request.mtu = 1400;

    std::string s =
        "00000001"     //4 bytes or 8 bytes FIXME ????
        "0000000000000002"     //4 bytes or 8 bytes FIXME ??
        "00000001"
        "00000002"
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000011"     // Length of type 17 bytes
        "64756d6d795f726d6e65745f6461746130" // dummy_rmnet_data0
        "00000000"     // Length of 0
        "00000016"     // Length of type 22 bytes
        "3139322e302e312e313120323030313a6462383a3a31" // 192.0.1.11 2001:db8::1
        "00000016"     // Length of type 22 bytes
        "3139322e302e312e313120323030313a6462383a3a31" // 192.0.1.11 2001:db8::1
        "00000017"     // Length of type 23 bytes
        "3139322e302e322e31353520323030313a6462383a3a31" // "192.0.2.155 2001:db8::1"
        "0000000b"
        "323030313a6332383a3a31" //"2001:c28::1";
        "00000578" // 1400 in decimal equivalent to hexa 578
        "00000000" // 0 mtuv4
        "00000000" // 0 mtuV6
        "00000000" // no op values for 1.6 parameters
        "00000000"
        "00000000"
        "00000000"
        "00"
        ;
     std::string expected = QtiRilStringUtils::fromHex(s);
     ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
     ASSERT_EQ(expected.size(), marshal.dataSize());
     ASSERT_EQ(expected, (std::string)marshal);
 }

TEST(Marshalling, RIL_Data_Call_Response_v11_marshal_1_6_params) {
    RIL_Data_Call_Response_v11 request = {};
    Marshal marshal;
    request.status = 1;
    request.suggestedRetryTime = 2;
    request.cid = 1;
    request.active = 2; // physcial link up
    request.type = "IPV4V6";
    request.ifname = "dummy_rmnet_data0";
    request.linkAddressesLength = 2;
    request.linkAddresses[0].address = "192.0.1.11";
    request.linkAddresses[0].properties = 0;
    request.linkAddresses[0].deprecationTime = 0;
    request.linkAddresses[0].expirationTime = 0;
    request.linkAddresses[1].address = "2001:db8::1";
    request.linkAddresses[1].properties = 0;
    request.linkAddresses[1].deprecationTime = 0;
    request.linkAddresses[1].expirationTime = 0;
    request.dnses = "192.0.1.11 2001:db8::1";
    request.gateways = "192.0.2.155 2001:db8::1";
    request.pcscf = "2001:c28::1";
    request.mtu = 1000;
    request.mtuV4 = 1400;
    request.mtuV6 = 1200;
    request.defaultQos = {};
    request.qosSessionsLength = 0;
    request.handoverFailureMode = RIL_HANDOVER_FAILURE_DO_FALLBACK;
    request.sliceInfoValid = 0;
    request.trafficDescriptorsLength = 0;
    std::string s =
        "00000001"
        "0000000000000002" // uint64_t suggestedRetryTime
        "00000001"
        "00000002"
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000011"     // Length of type 17 bytes
        "64756d6d795f726d6e65745f6461746130" // dummy_rmnet_data0
        "00000002"     // addressesLength 2
        "0000000a"     // Length of type 10 bytes
        "3139322e302e312e3131" // 192.0.1.11
        "00000000"     // properties 0
        "0000000000000000"     // deprecationTime 0
        "0000000000000000"     // expirationTime 0
        "0000000b"     // Length of type 11 bytes
        "323030313a6462383a3a31" // 2001:db8::1
        "00000000"     // properties 0
        "0000000000000000"     // deprecationTime 0
        "0000000000000000"     // expirationTime 0
        "00000016"     // Length of type 22 bytes
        "3139322e302e312e313120323030313a6462383a3a31" // 192.0.1.11 2001:db8::1
        "00000017"     // Length of type 23 bytes
        "3139322e302e322e31353520323030313a6462383a3a31" // "192.0.2.155 2001:db8::1"
        "0000000b"
        "323030313a6332383a3a31" //"2001:c28::1";
        "000003e8" // 1000 in decimal equivalent to hexa 3e8
        "00000578" // 1400 in decimal equivalent to hexa 578
        "000004b0" // 1200 in decimal equivalent to hexa 4b0
        "00000000" // qosType none
        "00000000" // qosSessions 0
        "00000001" // handoverFailureMode 1
        "00" // sliceInfoValid false
        "00000000" // trafficDescriptorsLength 0
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_Data_Call_Response_v11 result = {};
    ASSERT_NE(marshal.read(result), Marshal::Result::FAILURE);

    ASSERT_STREQ(request.linkAddresses[0].address, result.linkAddresses[0].address);
    ASSERT_STREQ(request.linkAddresses[1].address, result.linkAddresses[1].address);
}

TEST(Marshalling, RIL_Data_Call_Response_v11_marshal_qosNrSession) {
    RIL_Qos defaultQos = {
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
    RIL_QosSession qosSessions = {
        .qosSessionId = 1,
        .qos = defaultQos,
        .qosFiltersLength = 0,
    };
    RIL_Data_Call_Response_v11 request = {};
    Marshal marshal;
    request.status = 1;
    request.suggestedRetryTime = 2;
    request.cid = 1;
    request.active = 2; // physcial link up
    request.type = "IPV4V6";
    request.ifname = "dummy_rmnet_data0";
    request.linkAddressesLength = 2;
    request.linkAddresses[0].address = "192.0.1.11";
    request.linkAddresses[0].properties = 0;
    request.linkAddresses[0].deprecationTime = 0;
    request.linkAddresses[0].expirationTime = 0;
    request.linkAddresses[1].address = "2001:db8::1";
    request.linkAddresses[1].properties = 0;
    request.linkAddresses[1].deprecationTime = 0;
    request.linkAddresses[1].expirationTime = 0;
    request.dnses = "192.0.1.11 2001:db8::1";
    request.gateways = "192.0.2.155 2001:db8::1";
    request.pcscf = "2001:c28::1";
    request.mtu = 1000;
    request.mtuV4 = 1400;
    request.mtuV6 = 1200;
    request.defaultQos = defaultQos;
    request.qosSessionsLength = 1;
    request.qosSessions[0] = qosSessions;
    request.handoverFailureMode = RIL_HANDOVER_FAILURE_DO_FALLBACK;
    request.sliceInfoValid = 0;
    request.trafficDescriptorsLength = 0;
    std::string s =
        "00000001"
        "0000000000000002" // uint64_t suggestedRetryTime
        "00000001"
        "00000002"
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000011"     // Length of type 17 bytes
        "64756d6d795f726d6e65745f6461746130" // dummy_rmnet_data0
        "00000002"     // addressesLength 2
        "0000000a"     // Length of type 10 bytes
        "3139322e302e312e3131" // 192.0.1.11
        "00000000"     // properties 0
        "0000000000000000"     // deprecationTime 0
        "0000000000000000"     // expirationTime 0
        "0000000b"     // Length of type 11 bytes
        "323030313a6462383a3a31" // 2001:db8::1
        "00000000"     // properties 0
        "0000000000000000"     // deprecationTime 0
        "0000000000000000"     // expirationTime 0
        "00000016"     // Length of type 22 bytes
        "3139322e302e312e313120323030313a6462383a3a31" // 192.0.1.11 2001:db8::1
        "00000017"     // Length of type 23 bytes
        "3139322e302e322e31353520323030313a6462383a3a31" // "192.0.2.155 2001:db8::1"
        "0000000b"
        "323030313a6332383a3a31" //"2001:c28::1";
        "000003e8" // 1000 in decimal equivalent to hexa 3e8
        "00000578" // 1400 in decimal equivalent to hexa 578
        "000004b0" // 1200 in decimal equivalent to hexa 4b0
        "00000002" // qosType NR
        "0004" // uint16 fiveQi
        "00000fa0" // dlMaxBitrateKbps
        "000007d0" // dlGuaranteedBitrateKbps
        "000007d0" // ulMaxBitrateKbps
        "000003e8" // ulGuaranteedBitrateKbps
        "02" // uint8 qfi
        "0028" // uint16 averagingWindowMs
        "00000001" // qosSessions 1
        "00000001" // qosSessionId 1
        "00000002" // qosType NR
        "0004" // uint16 fiveQi
        "00000fa0" // dlMaxBitrateKbps
        "000007d0" // dlGuaranteedBitrateKbps
        "000007d0" // ulMaxBitrateKbps
        "000003e8" // ulGuaranteedBitrateKbps
        "02" // uint8 qfi
        "0028" // uint16 averagingWindowMs
        "00000000" // qosFiltersLength 0
        "00000001" // handoverFailureMode 1
        "00" // sliceInfoValid false
        "00000000" // trafficDescriptorsLength 0
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);
}
