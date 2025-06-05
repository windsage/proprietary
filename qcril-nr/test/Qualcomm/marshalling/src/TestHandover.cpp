/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/Handover.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Handover_marshall_001)
{
  Marshal marshal;

  RIL_HoExtra hoExtra{
    .type = RIL_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL,  // = 0
    .extraInfo = "1234",
  };

  RIL_HandoverInfo request{
    .type = RIL_HO_COMPLETE_SUCCESS,  // = 1
    .srcTech = RADIO_TECH_LTE,        // = 14
    .targetTech = RADIO_TECH_IWLAN,   // = 18
    .hoExtra = &hoExtra,
    .errorCode = "12345",
    .errorMessage = "12345",
  };

  std::string s =
      "00000001"    // .type = RIL_HO_COMPLETE_SUCCESS // = 1
      "0000000E"    // .srcTech = RADIO_TECH_LTE // = 14
      "00000012"    // .targetTech = RADIO_TECH_IWLAN // = 18
      "00000001"    // hoExtra length = 1
      "00000000"    // .type = RIL_HO_XT_TYPE_LTE_TO_IWLAN_HO_FAIL // = 0
      "00000004"    // extraInfo length = 4
      "31323334"    // .extraInfo = "1234"
      "00000005"    // errorCode length = 5
      "3132333435"  // .errorCode = "12345"
      "00000005"    // errorMessage length = 5
      "3132333435"  // .errorMessage = "12345"
      ;
  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_HandoverInfo request_r{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.type, request_r.type);
  ASSERT_EQ(request.srcTech, request_r.srcTech);
  ASSERT_EQ(request.targetTech, request_r.targetTech);

  RIL_HoExtra r_hoExtra = *request_r.hoExtra;
  ASSERT_EQ(hoExtra.type, r_hoExtra.type);
  ASSERT_STREQ(hoExtra.extraInfo, r_hoExtra.extraInfo);

  ASSERT_STREQ(request.errorCode, request_r.errorCode);
  ASSERT_STREQ(request.errorMessage, request_r.errorMessage);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
