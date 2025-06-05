/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/ClipInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, ClipInfo_marshall_001)
{
  Marshal marshal;

  RIL_IMS_SipErrorInfo errorDetails{
    .errorCode = 404,
    .errorString = "12345",
  };

  RIL_ClipInfo request{
    .clipStatus = RIL_CLIP_PROVISIONED,  // = 1
    .errorDetails = &errorDetails,
  };

  std::string s =
      "00000001"    // .clipStatus = RIL_CLIP_PROVISIONED,  // = 1
      "00000001"    // errorDetails length = 1
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3132333435"  // errorString ("12345")
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_ClipInfo request_r{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.clipStatus, request_r.clipStatus);

  RIL_IMS_SipErrorInfo r_errorDetails = *request_r.errorDetails;
  ASSERT_EQ(errorDetails.errorCode, r_errorDetails.errorCode);
  ASSERT_STREQ(errorDetails.errorString, r_errorDetails.errorString);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
