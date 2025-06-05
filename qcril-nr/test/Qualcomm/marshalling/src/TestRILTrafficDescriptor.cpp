/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILTrafficDescriptor.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <iostream>
using namespace std;

TEST(Marshalling, RIL_TrafficDescriptor_marshal_dnnInvalid) {
    RIL_TrafficDescriptor request = {};
    request.osAppIdValid = 1;
    std::vector<uint8_t> osAppId = {0x64, 0x75, 0x6d, 0x6d, 0x79, 0x5f, 0x61, 0x70, 0x70, 0x5f, 0x69, 0x64};
    memcpy(request.osAppId, osAppId.data(), osAppId.size());
    request.osAppIdLength = osAppId.size();
    Marshal marshal;
    std::string s =
        "00" // dnnValid false
        "01" // osAppId true
        "0000000c"
        "64756d6d795f6170705f6964"
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_TrafficDescriptor result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.dnnValid, result.dnnValid);
    ASSERT_STREQ(request.dnn, result.dnn);
    ASSERT_EQ(request.osAppIdValid, result.osAppIdValid);
    ASSERT_EQ(request.osAppIdLength, result.osAppIdLength);
}

TEST(Marshalling, RIL_TrafficDescriptor_marshal_osAppIdInvalid) {
    RIL_TrafficDescriptor request = {};
    request.dnnValid = 1;
    request.dnn = "dummy_dnn";
    Marshal marshal;
    std::string s =
        "01" // dnnValid false
        "00000009"
        "64756d6d795f646e6e"
        "00" // osAppIdValid false
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_TrafficDescriptor result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.dnnValid, result.dnnValid);
    ASSERT_STREQ(request.dnn, result.dnn);
    ASSERT_EQ(request.osAppIdValid, result.osAppIdValid);
    ASSERT_EQ(request.osAppIdLength, result.osAppIdLength);
}