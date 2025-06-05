/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/ImsSrtpEncryptionInfo.h>
#include <string>
#include <telephony/ril_ims.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Srtp_marshall_basic)
{
  Marshal marshal;
  RIL_IMS_SrtpEncryptionStatus srtpStatus = {
    .callId = 1,
    .categories = 7
  };

  std::string s =
      "00000001"        // .callId = 1
      "00000007"  // .categories = 7
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(srtpStatus), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_SrtpEncryptionStatus srtpStatus_r{};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(srtpStatus_r), Marshal::Result::FAILURE);
  ASSERT_EQ(srtpStatus.callId, srtpStatus_r.callId);
  ASSERT_EQ(srtpStatus.categories, srtpStatus_r.categories);

  ASSERT_NE(marshal.release(srtpStatus), Marshal::Result::FAILURE);
}
