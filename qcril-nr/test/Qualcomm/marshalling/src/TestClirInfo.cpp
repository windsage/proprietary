/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/ClirInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, ClirInfo_marshall_001)
{
  Marshal marshal;

  RIL_ClirInfo request{
    .action = 1,        // = CLIR invocation
    .presentation = 0,  // = CLIR not provisioned
  };

  std::string s =
      "00000001"  // .action = 1,  // = CLIR invocatio
      "00000000"  // .presentation = 1, // = CLIR not provisioned
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_ClirInfo request_r{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.action, request_r.action);
  ASSERT_EQ(request.presentation, request_r.presentation);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
