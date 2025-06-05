/*
 * Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/RILImeiInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, Imei_Info)
{
  Marshal marshal;

  RIL_ImeiInfo req = {
    .imeiType = IMEI_TYPE_PRIMARY,
    .imei = "999",
    .imeiLen = 3,
  };

  std::string s =
      "01"                // .imeiType = IMEI_TYPE_PRIMARY
      "00000003393939"    // .imei = "999"
      "03";               // .imeiLen = 3

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(req), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  RIL_ImeiInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_STREQ(req.imei, request_r.imei);
  ASSERT_EQ(req.imeiLen, request_r.imeiLen);
  ASSERT_EQ(req.imeiType, request_r.imeiType);

  ASSERT_NE(marshal.release(request_r), Marshal::Result::FAILURE);
}
