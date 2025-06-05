/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSSubConfigInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

#define STACK_ENABLED_LEN 3

TEST(Marshalling, IMSSubConfigInfo_marshall_basic) {
    Marshal marshal;
    uint8_t stackEnabled[STACK_ENABLED_LEN] = {0, 1, 0};
    RIL_IMS_SubConfigInfo subConfigInfo {
        .simultStackCount = 1,
        .imsStackEnabledLen = STACK_ENABLED_LEN,
        .imsStackEnabled = stackEnabled,
    };

    std::string s = "00000001" // simultStackCount
                    "00000003" // imsStackEnabledLen
                    "00"       // imsStackEnabled[0]
                    "01"       // imsStackEnabled[1]
                    "00"       // imsStackEnabled[2]
                    ;

    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_EQ(marshal.write(subConfigInfo), Marshal::Result::SUCCESS);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_IMS_SubConfigInfo subConfigInfo_r{};
    ASSERT_EQ(marshal.read(subConfigInfo_r), Marshal::Result::SUCCESS);
    ASSERT_EQ(subConfigInfo_r.simultStackCount,
              subConfigInfo.simultStackCount);
    ASSERT_EQ(subConfigInfo_r.imsStackEnabledLen,
              subConfigInfo.imsStackEnabledLen);

    for (int i = 0; i < subConfigInfo.imsStackEnabledLen; i++) {
        ASSERT_EQ(subConfigInfo_r.imsStackEnabled[i],
                  subConfigInfo.imsStackEnabled[i]);
    }

    ASSERT_EQ(marshal.release(subConfigInfo_r), Marshal::Result::SUCCESS);
}

TEST(Marshalling, IMSSubConfigInfo_marshall_invalid_imsStackEnabledLen) {
    Marshal marshal;

    std::string s = "00000001" // simultStackCount
                    "10000000" // imsStackEnabledLen
                    "00"       // imsStackEnabled[0]
                    "01"       // imsStackEnabled[1]
                    "00"       // imsStackEnabled[2]
                    ;
    std::string invalid_input = QtiRilStringUtils::fromHex(s);
    marshal.setData(invalid_input);
    RIL_IMS_SubConfigInfo subConfigInfo_r{};
    ASSERT_EQ(marshal.read(subConfigInfo_r), Marshal::Result::NOT_ENOUGH_DATA);
    ASSERT_EQ(marshal.release(subConfigInfo_r), Marshal::Result::SUCCESS);
}
