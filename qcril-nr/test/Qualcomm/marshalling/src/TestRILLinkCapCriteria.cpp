 /*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILLinkCapCriteria.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

#define DOWN_LEN 11
#define UP_LEN 9

TEST(Marshalling, RILLinkCapCriteria_marshall_basic) {
    Marshal marshal;
    int downLength = DOWN_LEN;
    int upLength = UP_LEN;
    int downlinkKbps[DOWN_LEN] = {
        100,500,1000,5000,10000,20000,
        50000,100000,200000,500000,1000000
    };
    int uplinkKbps[UP_LEN] = {
        100,500,1000,5000,10000,20000,50000,100000,200000
    };
    RIL_LinkCapCriteria criteria = {
        .hysteresisMs = 3000,
        .hysteresisDlKbps = 50,
        .hysteresisUlKbps = 50,
        .thresholdsDownLength = downLength,
        .thresholdsDownlinkKbps = downlinkKbps,
        .thresholdsUpLength = upLength,
        .thresholdsUplinkKbps = uplinkKbps,
        .ran = RIL_RAN::LTE_RAN,
    };

    std::string s = "00000BB8"          // hysteresisMs
                    "00000032"          // hysteresisDlKbps
                    "00000032"          // hysteresisUlKbps
                    "0000000B"          // thresholdsDownLength
                    "00000064"          // thresholdsDownlinkKbps[0]
                    "000001F4"          // thresholdsDownlinkKbps[1]
                    "000003E8"          // thresholdsDownlinkKbps[2]
                    "00001388"          // thresholdsDownlinkKbps[3]
                    "00002710"          // thresholdsDownlinkKbps[4]
                    "00004E20"          // thresholdsDownlinkKbps[5]
                    "0000C350"          // thresholdsDownlinkKbps[6]
                    "000186A0"          // thresholdsDownlinkKbps[7]
                    "00030D40"          // thresholdsDownlinkKbps[8]
                    "0007A120"          // thresholdsDownlinkKbps[9]
                    "000F4240"          // thresholdsDownlinkKbps[10]
                    "00000009"          // thresholdsUpLength
                    "00000064"          // thresholdsUplinkKbps[0]
                    "000001F4"          // thresholdsUplinkKbps[1]
                    "000003E8"          // thresholdsUplinkKbps[2]
                    "00001388"          // thresholdsUplinkKbps[3]
                    "00002710"          // thresholdsUplinkKbps[4]
                    "00004E20"          // thresholdsUplinkKbps[5]
                    "0000C350"          // thresholdsUplinkKbps[6]
                    "000186A0"          // thresholdsUplinkKbps[7]
                    "00030D40"          // thresholdsUplinkKbps[8]
                    "02"                // ran
                    ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_EQ(marshal.write(criteria), Marshal::Result::SUCCESS);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_LinkCapCriteria criteria_r{};
    ASSERT_EQ(marshal.read(criteria_r), Marshal::Result::SUCCESS);
    ASSERT_EQ(criteria_r.hysteresisMs, criteria.hysteresisMs);
    ASSERT_EQ(criteria_r.hysteresisDlKbps, criteria.hysteresisDlKbps);
    ASSERT_EQ(criteria_r.hysteresisUlKbps, criteria.hysteresisUlKbps);
    ASSERT_EQ(criteria_r.thresholdsDownLength, criteria.thresholdsDownLength);
    ASSERT_EQ(criteria_r.thresholdsUpLength, criteria.thresholdsUpLength);
    ASSERT_EQ(criteria_r.ran, criteria.ran);

    for (int i = 0; i < criteria.thresholdsDownLength; i++) {
        ASSERT_EQ(criteria_r.thresholdsDownlinkKbps[i],
                  criteria.thresholdsDownlinkKbps[i]);
    }

    for (int i = 0; i < criteria.thresholdsUpLength; i++) {
        ASSERT_EQ(criteria_r.thresholdsUplinkKbps[i],
                  criteria.thresholdsUplinkKbps[i]);
    }

    ASSERT_EQ(marshal.release(criteria_r), Marshal::Result::SUCCESS);
}

TEST(Marshalling, RILLinkCapCriteria_marshal_invalid_thresholdsDownLength) {
    Marshal marshal;
    std::string s = "00000BB8"          // hysteresisMs
                    "00000032"          // hysteresisDlKbps
                    "00000032"          // hysteresisUlKbps
                    "10000000"          // thresholdsDownLength
                    "00000064"          // thresholdsDownlinkKbps[0]
                    "000001F4"          // thresholdsDownlinkKbps[1]
                    "000003E8"          // thresholdsDownlinkKbps[2]
                    "00001388"          // thresholdsDownlinkKbps[3]
                    "00002710"          // thresholdsDownlinkKbps[4]
                    "00004E20"          // thresholdsDownlinkKbps[5]
                    "0000C350"          // thresholdsDownlinkKbps[6]
                    "000186A0"          // thresholdsDownlinkKbps[7]
                    "00030D40"          // thresholdsDownlinkKbps[8]
                    "0007A120"          // thresholdsDownlinkKbps[9]
                    "000F4240"          // thresholdsDownlinkKbps[10]
                    "00000009"          // thresholdsUpLength
                    "00000064"          // thresholdsUplinkKbps[0]
                    "000001F4"          // thresholdsUplinkKbps[1]
                    "000003E8"          // thresholdsUplinkKbps[2]
                    "00001388"          // thresholdsUplinkKbps[3]
                    "00002710"          // thresholdsUplinkKbps[4]
                    "00004E20"          // thresholdsUplinkKbps[5]
                    "0000C350"          // thresholdsUplinkKbps[6]
                    "000186A0"          // thresholdsUplinkKbps[7]
                    "00030D40"          // thresholdsUplinkKbps[8]
                    "02"                // ran
                    ;
    std::string invalid_input = QtiRilStringUtils::fromHex(s);
    marshal.setData(invalid_input);
    RIL_LinkCapCriteria criteria_r{};
    ASSERT_EQ(marshal.read(criteria_r), Marshal::Result::NOT_ENOUGH_DATA);
    ASSERT_NE(marshal.release(criteria_r), Marshal::Result::FAILURE);

}

TEST(Marshalling, RILLinkCapCriteria_marshal_invalid_thresholdsUpLength) {
    Marshal marshal;
    std::string s = "00000BB8"          // hysteresisMs
                    "00000032"          // hysteresisDlKbps
                    "00000032"          // hysteresisUlKbps
                    "0000000B"          // thresholdsDownLength
                    "00000064"          // thresholdsDownlinkKbps[0]
                    "000001F4"          // thresholdsDownlinkKbps[1]
                    "000003E8"          // thresholdsDownlinkKbps[2]
                    "00001388"          // thresholdsDownlinkKbps[3]
                    "00002710"          // thresholdsDownlinkKbps[4]
                    "00004E20"          // thresholdsDownlinkKbps[5]
                    "0000C350"          // thresholdsDownlinkKbps[6]
                    "000186A0"          // thresholdsDownlinkKbps[7]
                    "00030D40"          // thresholdsDownlinkKbps[8]
                    "0007A120"          // thresholdsDownlinkKbps[9]
                    "000F4240"          // thresholdsDownlinkKbps[10]
                    "10000000"          // thresholdsUpLength
                    "00000064"          // thresholdsUplinkKbps[0]
                    "000001F4"          // thresholdsUplinkKbps[1]
                    "000003E8"          // thresholdsUplinkKbps[2]
                    "00001388"          // thresholdsUplinkKbps[3]
                    "00002710"          // thresholdsUplinkKbps[4]
                    "00004E20"          // thresholdsUplinkKbps[5]
                    "0000C350"          // thresholdsUplinkKbps[6]
                    "000186A0"          // thresholdsUplinkKbps[7]
                    "00030D40"          // thresholdsUplinkKbps[8]
                    "02"                // ran
                    ;
    std::string invalid_input = QtiRilStringUtils::fromHex(s);
    marshal.setData(invalid_input);
    RIL_LinkCapCriteria criteria_r{};
    ASSERT_EQ(marshal.read(criteria_r), Marshal::Result::NOT_ENOUGH_DATA);
    ASSERT_NE(marshal.release(criteria_r), Marshal::Result::FAILURE);

}
