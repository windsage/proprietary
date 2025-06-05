/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/SwitchWaitingOrHoldingAndActive.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, RIL_SwitchWaitingOrHoldingAndActive_marshall_001)
{
  Marshal marshal;
  RIL_SwitchWaitingOrHoldingAndActive request{
    .hasCallType = 1,
    .callType = RIL_CALL_TYPE_VT,  // 4
  };

  std::string s =
      "01"        // .hasCallType = 1
      "00000004"  // .callType = RIL_IMS_CALL_TYPE_VT
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_SwitchWaitingOrHoldingAndActive request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.hasCallType, request_r.hasCallType);
  ASSERT_EQ(request.callType, request_r.callType);
}
