/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RefreshConferenceInfo.h>
#include <string>
#include <telephony/ril_call.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, RefreshConferenceInfo_marshall_001) {
  Marshal marshal;
  /*uint8_t p = 8;
  uint8_t *ptr = &p; */
  uint8_t uri[] = {55, 50};
  RIL_RefreshConferenceInfo request{
    .conferenceCallState = RIL_CONF_CALL_STATE_FOREGROUND, // 1
    .confInfoUriLength = 2, //1
    .confInfoUri = uri,
  };

  std::string s =
      "00000001"    // .conferenceCallState = RIL_CONF_CALL_STATE_FOREGROUND, // 1
      "00000002"    // .confInfoUriLength = 1, //1
      "3732"; // .confInfoUri = uri

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_RefreshConferenceInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.conferenceCallState, request_r.conferenceCallState);
  ASSERT_EQ(request.confInfoUriLength, request_r.confInfoUriLength);
  //ASSERT_EQ(request.confInfoUri, request_r.confInfoUri);
  (void)memcmp(request.confInfoUri, request_r.confInfoUri, request.confInfoUriLength);
  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
