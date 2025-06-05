/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/DeflectCallInfo.h>
#include <string>
#include <telephony/ril_call.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, DeflectCallInfo_marshall_001)
{
  Marshal marshal;
  RIL_DeflectCallInfo request{
    .connIndex = 2,
    .number = "12345",
  };
  std::string s =
      "00000002"    // .connIndex = 1
      "00000005"    // .number length
      "3132333435"  // .number ("12345")
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_DeflectCallInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.connIndex, request_r.connIndex);
  ASSERT_STREQ(request.number, request_r.number);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}

TEST(Marshalling, DeflectCallInfo_marshall_002)
{
  Marshal marshal;
  RIL_DeflectCallInfo request{
    .connIndex = 2,
    .number = nullptr,
  };
  std::string s =
      "00000002"    // .connIndex = 1
      "FFFFFFFF"    // .number length
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_DeflectCallInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.connIndex, request_r.connIndex);
  ASSERT_EQ(request.number, request_r.number);  // Expected nullptr

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
