/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/CallWaiting.h>
#include <string>
#include <telephony/ril_call.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Hangup_marshall_001)
{
  Marshal marshal;
  RIL_Hangup request{
    .connIndex = 2,
    .connUri = "12345",
    .hasRejectCause = 1,
    .rejectCause = RIL_IMS_FAILCAUSE_NORMAL,  // 16
  };
  std::string s =
      "00000002"    // .connIndex = 1
      "00000005"    // .connUri length
      "3132333435"  // .connUri ("12345")
      "01"          // .hasRejectCause = 1
      "00000010"    // .rejectCause = RIL_IMS_FAILCAUSE_NORMAL;  // 16
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_Hangup request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.connIndex, request_r.connIndex);
  ASSERT_STREQ(request.connUri, request_r.connUri);
  ASSERT_EQ(request.hasRejectCause, request_r.hasRejectCause);
  ASSERT_EQ(request.rejectCause, request_r.rejectCause);
}

TEST(Marshalling, Hangup_marshall_002)
{
  Marshal marshal;
  RIL_Hangup request{
    .connIndex = 2,
    .connUri = nullptr,
    .hasRejectCause = 0,
    .rejectCause = RIL_IMS_FAILCAUSE_UNKNOWN, // 0,
  };
  std::string s =
      "00000002"    // .connIndex = 1
      "FFFFFFFF"    // .connUri length
      "00"          // .hasRejectCause = 1
      "00000000"    // .rejectCause = 0
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_Hangup request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.connIndex, request_r.connIndex);
  ASSERT_EQ(request.connUri, request_r.connUri);  // Expected nullptr
  ASSERT_EQ(request.hasRejectCause, request_r.hasRejectCause);
  ASSERT_EQ(request.rejectCause, request_r.rejectCause);
}
