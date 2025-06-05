/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/Answer.h>
#include <string>
#include <telephony/ril_call.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Answer_marshall_basic)
{
  Marshal marshal;
  RIL_Answer answer = {
    .isValid = 1,
    .callType = RIL_CALL_TYPE_VT,                     // = 4
    .presentation = RIL_IP_PRESENTATION_NUM_DEFAULT,  // = 2
    .rttMode = RIL_RTT_FULL,                          // = 1
  };

  std::string s =
      "01"        // .isValid = 1
      "00000004"  // .callType = RIL_CALL_TYPE_VT, // = 4
      "00000002"  // .presentation = RIL_IP_PRESENTATION_NUM_DEFAULT, // = 2
      "00000001"  // .rttMode = RIL_RTT_FULL, // = 1
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(answer), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_Answer answer_r{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(answer_r), Marshal::Result::FAILURE);
  ASSERT_EQ(answer.isValid, answer_r.isValid);
  ASSERT_EQ(answer.callType, answer_r.callType);
  ASSERT_EQ(answer.presentation, answer_r.presentation);
  ASSERT_EQ(answer.rttMode, answer_r.rttMode);

  ASSERT_NE(marshal.release(answer_r), Marshal::Result::FAILURE);
}
