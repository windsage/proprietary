/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/CarrierInfoForImsiEncryption.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <iostream>
using namespace std;

TEST(Marshalling, RIL_CarrierInfoForImsiEncryption_marshal) {
    RIL_CarrierInfoForImsiEncryption request = {
        .mcc = "310",
        .mnc = "10",
        .carrierKey = (uint8_t *)"MIIDsjCCAxugAwIBAgIJAPLf2gS0zYGUMA0GCSqGSIb3DQEBBQUAMIGYMQswCQYDVQQGEwJVUzET",
        .carrierKeyLength = 76,
        .keyIdentifier = "epdg_key_id_1",
        .expirationTime = 1592265600,
    };
    Marshal marshal;
    std::string s =
        "00000003" // mccLen 3
        "333130" // mcc 310
        "00000002" // mncLen 2
        "3130" // mccLen 3
        "0000004c" // carrierKeyLength 76
        "4d494944736a434341787567417749424167494a41504c66326753307a5947554d413047435371475349623344514542425155414d4947594d517377435159445651514745774a56557a4554"
        "0000000d"
        "657064675f6b65795f69645f31"
        "000000005ee80b80"
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_CarrierInfoForImsiEncryption result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_STREQ(request.mcc, result.mcc);
    ASSERT_STREQ(request.mnc, result.mnc);
    ASSERT_EQ(request.carrierKeyLength, result.carrierKeyLength);
    ASSERT_EQ(0, memcmp(request.carrierKey, result.carrierKey, request.carrierKeyLength));
    ASSERT_STREQ(request.keyIdentifier, result.keyIdentifier);
    ASSERT_EQ(request.expirationTime, result.expirationTime);
}
