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

TEST(Marshalling, CallWaitingSettings_marshall_001)
{
  Marshal marshal;
  RIL_CallWaitingSettings request{
    .enabled = 1,
    .serviceClass = 5,
  };
  std::string s =
      "01"          // enabled = 1
      "00000005"    // service class = 5
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_CallWaitingSettings request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.enabled, request_r.enabled);
  ASSERT_EQ(request.serviceClass, request_r.serviceClass);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}

TEST(Marshalling, QueryCallWaitingResponse_marshall_001) {
  Marshal marshal;
  RIL_QueryCallWaitingResponse request{
    .callWaitingSettings = {
      .enabled = 1,
      .serviceClass = 5,
    },
    .sipError = {
      .errorCode = 404,
      .errorString = "12345",
    },
  };

  std::string s =
      "01"          // enabled = 1
      "00000005"    // service class = 5
      "00000194"    // errorCode = 404
      "00000005"    // errorString length
      "3132333435"  // errorString ("12345")
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_QueryCallWaitingResponse readData{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(readData), Marshal::Result::FAILURE);

  ASSERT_EQ(request.callWaitingSettings.enabled, readData.callWaitingSettings.enabled);
  ASSERT_EQ(request.callWaitingSettings.serviceClass, readData.callWaitingSettings.serviceClass);
  ASSERT_EQ(request.sipError.errorCode, readData.sipError.errorCode);
  ASSERT_STREQ(request.sipError.errorString, request.sipError.errorString);

  ASSERT_NE(marshal.release(readData), Marshal::Result::FAILURE);
}
