/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/ExplicitCallTransferInfo.h>
#include <string>
#include <telephony/ril_call.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, ExplicitCallTransferInfo_marshall_001)
{
  Marshal marshal;
  RIL_ExplicitCallTransfer request{
    .isValid = 1,
    .callId = 2,
    .ectType = RIL_ECT_TYPE_ASSURED_TRANSFER,  // = 2
    .targetAddress = "12345",
    .targetCallId = 1,
  };

  std::string s =
      "01"          // .isValid = 1
      "00000002"    // .callId = 2
      "00000002"    // .ectType = RIL_ECT_TYPE_ASSURED_TRANSFER, // = 2
      "00000005"    // .targetAddress length = 5
      "3132333435"  // .targetAddress = "12345"
      "00000001"    // .targetCallId = 1
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_ExplicitCallTransfer request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.isValid, request_r.isValid);
  ASSERT_EQ(request.callId, request_r.callId);
  ASSERT_EQ(request.ectType, request_r.ectType);
  ASSERT_STREQ(request.targetAddress, request_r.targetAddress);
  ASSERT_EQ(request.targetCallId, request_r.targetCallId);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
