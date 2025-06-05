/*
 * Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/IMSAutoCallRejectionInfo.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, IMSAutoCallRejectionInfo_marshall_001) {
  Marshal marshal;
  RIL_IMS_CallComposerInfo ccInfo = { .priority = RIL_IMS_CALL_COMPOSER_PRIORITY_URGENT, // = 1,
                                           .subject = "hello", // = 0x68656C6C6F,
                                           .organization = "hello", // = 0x68656C6C6F,
                                           .location = {
                                             .radius = 20, // = 0x14,
                                             .latitude = 7893242, // = 0x7870FA,
                                             .longitude = 7893242, // = 0x39447,
                                           },
                                           .imageUrl = "https://google.com", // = 0x68747470733A2F2F676F6F676C652E636F6D,
  };

  RIL_IMS_EcnamInfo ecnamInfo = {
    .name = "hello",                  // = 0x68656C6C6F,
    .iconUrl = "https://google.com",  // = 0x68747470733A2F2F676F6F676C652E636F6D,
    .infoUrl = "https://google.com",  // = 0x68747470733A2F2F676F6F676C652E636F6D,
    .cardUrl = "https://google.com",  // = 0x68747470733A2F2F676F6F676C652E636F6D,
  };

  RIL_IMS_AutoCallRejectionInfo request{
    .callType = RIL_IMS_CALL_TYPE_VT,                         // = 4,
    .cause = RIL_IMS_FAILCAUSE_SIP_REQUEST_ENTITY_TOO_LARGE,  // = 0x440
    .sipErrorCode = 404,
    .number = "123456",
    .verificationStatus = RIL_IMS_VERSTAT_VERIFICATION_STATUS_PASS,  // = 1,
    .ccInfo = &ccInfo,
    .ecnamInfo = &ecnamInfo,
  };

  std::string s =
      "00000004"      // .callType = RIL_IMS_CALL_TYPE_VT, // = 4,
      "00000440"      // .cause = RIL_IMS_FAILCAUSE_SIP_REQUEST_ENTITY_TOO_LARGE, // = 0x440
      "0194"          // .sipErrorCode = 404,
      "00000006"      // .number length = 6,
      "313233343536"  // .number ("123456")
      "00000001"      // .verificationStatus = RIL_IMS_VERSTAT_VERIFICATION_STATUS_PASS, // = 1,
      // RIL_IMS_CallComposerInfo {
      "00000001"    // ccInfo
      "00000001"    // .priority = RIL_IMS_CALL_COMPOSER_PRIORITY_URGENT, // = 1,
      "00000005"    // .subject length
      "68656C6C6F"  // .subject = "hello", // = 0x68656C6C6F,
      "00000005"    // .organization length
      "68656C6C6F"  // .organization = "hello", // = 0x68656C6C6F,
      // Double represntation may be different.
      "0000000000000014"                      // .location.radius = 20, // = 0x14,
      "00000000007870FA"                      // .location.latitude = 7893242, // = 0x7870FA,
      "0000000000039447"                      // .location.longitude = 234567, // = 0x39447,
      "00000012"                              // .imageUrl length
      "68747470733A2F2F676F6F676C652E636F6D"  //.imageUrl = "https://google.com", // =
                                              //0x68747470733A2F2F676F6F676C652E636F6D,
      // } RIL_IMS_CallComposerInfo
      // RIL_IMS_EcnamInfo {
      "00000001"                              // ecnamInfo
      "00000005"                              // .name length
      "68656C6C6F"                            // .name = "hello", // = 0x68656C6C6F,
      "00000012"                              // .iconUrl length
      "68747470733A2F2F676F6F676C652E636F6D"  // .iconUrl = "https://google.com", // =
                                              // 0x68747470733A2F2F676F6F676C652E636F6D,
      "00000012"                              // .infoUrl length
      "68747470733A2F2F676F6F676C652E636F6D"  // .infoUrl = "https://google.com", // =
                                              // 0x68747470733A2F2F676F6F676C652E636F6D,
      "00000012"                              // .cardUrl length
      "68747470733A2F2F676F6F676C652E636F6D"  // .cardUrl = "https://google.com", // =
                                              // 0x68747470733A2F2F676F6F676C652E636F6D,
      // } RIL_IMS_EcnamInfo
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  // Because of double reprentation not able to exactly match the marshal.
  // ASSERT_EQ(expected, (std::string)marshal);

  RIL_IMS_AutoCallRejectionInfo request_r = {};
  marshal.setDataPosition(0);
  ASSERT_NE(marshal.read(request_r), Marshal::Result::FAILURE);
  ASSERT_EQ(request.callType, request_r.callType);
  ASSERT_EQ(request.cause, request_r.cause);
  ASSERT_EQ(request.sipErrorCode, request_r.sipErrorCode);
  ASSERT_STREQ(request.number, request_r.number);
  ASSERT_EQ(request.verificationStatus, request_r.verificationStatus);

  ASSERT_NE(request_r.ccInfo, nullptr);
  RIL_IMS_CallComposerInfo r_ccInfo = *request_r.ccInfo;
  ASSERT_EQ(ccInfo.priority, r_ccInfo.priority);
  ASSERT_STREQ(ccInfo.subject, r_ccInfo.subject);
  ASSERT_STREQ(ccInfo.organization, r_ccInfo.organization);
  /* TODO not able to compare double
  ASSERT_EQ(ccInfo.location.radius, r_ccInfo.location.radius);
  ASSERT_EQ(ccInfo.location.latitude, r_ccInfo.location.latitude);
  ASSERT_EQ(ccInfo.location.longitude, r_ccInfo.location.longitude);*/
  ASSERT_STREQ(ccInfo.imageUrl, r_ccInfo.imageUrl);

  ASSERT_NE(request_r.ecnamInfo, nullptr);
  RIL_IMS_EcnamInfo r_ecnamInfo = *request_r.ecnamInfo;
  ASSERT_STREQ(ecnamInfo.name, r_ecnamInfo.name);
  ASSERT_STREQ(ecnamInfo.iconUrl, r_ecnamInfo.iconUrl);
  ASSERT_STREQ(ecnamInfo.infoUrl, r_ecnamInfo.infoUrl);
  ASSERT_STREQ(ecnamInfo.cardUrl, r_ecnamInfo.cardUrl);
}
