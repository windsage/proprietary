/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <QtiRilStringUtils.h>
#include <gtest/gtest.h>
#include <marshal/SimIoRequest.h>
#include <telephony/ril.h>
#include <string>

TEST(Marshalling, SIM_IO_Request_marshall_write_001) {
  Marshal marshal;

  RIL_SIM_IO_v6 request;

  request.command = 4;
  request.fileid = 5;
  request.path = "12345";
  request.p1 = 1;
  request.p2 = 2;
  request.p3 = 3;
  request.data = "3456789";
  request.pin2 = "456789";
  request.aidPtr = "67890";

  std::string s =
		"00000004"     // command
		"00000005"     // fileid
        "00000005"     // path length
		"3132333435"           // path
		"00000001"     // p1
		"00000002"     // p2
		"00000003"     // p3
        "00000007"             // data length
		"33343536373839"       // data
        "00000006"             // pin2 length
		"343536373839"         // pin2
		"00000005"             // aidPtr length
		"3637383930"           // aidPtr
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

TEST(Marshalling, SIM_IO_Request_marshall_write_002) {
  Marshal marshal;

  RIL_SIM_IO_v6 request;

  request.command = 4;
  request.fileid = 5;
  request.path = nullptr;
  request.p1 = 1;
  request.p2 = 2;
  request.p3 = 3;
  request.data = nullptr;
  request.pin2 = nullptr;
  request.aidPtr = nullptr;

  std::string s =
		"00000004"     // command
		"00000005"     // fileid
		"ffffffff"     // path length
		"00000001"     // p1
		"00000002"     // p2
		"00000003"     // p3
		"ffffffff"     // data length
		"ffffffff"     // pin2 length
		"ffffffff"     // aidPtr length
        ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_NE(marshal.write(request), Marshal::Result::FAILURE);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
}

/* RIL_AdnRecordInfo_marshall_basic
 * Basic test of read, write, and release methods
*/
TEST(Marshalling, RIL_AdnRecordInfo_marshall_basic) {
  Marshal marshal;
  RIL_AdnRecordInfo recordInfo {
    .record_id = 0,
    .name = "abc",
    .number = "1234567890",
    .email_elements = 1,
    .email = {"abc@def.com"},
    .anr_elements = 2,
    .ad_number = {
      "0987654321",
      "1111"
    },
  };
  std::string s =
    "00000003"                // length of name
    "616263"                  // name
    "0000000a"                // length of number
    "31323334353637383930"    // number
    "00000001"                // email elements
    "0000000b"                // length of email[0]
    "616263406465662E636f6d"  // email[0]
    "00000002"                // anr elements
    "0000000a"                // length of ad_number[0]
    "30393837363534333231"    // ad_number[0]
    "00000004"                // length of ad_number[1]
    "31313131"                // ad_number[1]
  ;
  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_EQ(marshal.write(recordInfo), Marshal::Result::SUCCESS);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);
  marshal.setDataPosition(0);

  RIL_AdnRecordInfo recordInfo_r{};
  ASSERT_EQ(marshal.read(recordInfo_r), Marshal::Result::SUCCESS);
  ASSERT_EQ(std::string(recordInfo_r.name), std::string(recordInfo.name));
  ASSERT_EQ(std::string(recordInfo_r.number), std::string(recordInfo.number));
  ASSERT_EQ(recordInfo_r.email_elements, recordInfo.email_elements);
  ASSERT_EQ(recordInfo_r.anr_elements, recordInfo.anr_elements);

  for (int i = 0; i < recordInfo.email_elements; i++) {
    ASSERT_EQ(std::string(recordInfo_r.email[i]),
              std::string(recordInfo.email[i]));
  }

  for (int i = 0; i < recordInfo.anr_elements; i++) {
    ASSERT_EQ(std::string(recordInfo_r.ad_number[i]),
              std::string(recordInfo.ad_number[i]));
  }

  ASSERT_EQ(marshal.release(recordInfo_r), Marshal::Result::SUCCESS);

}

/* RIL_AdnRecordInfo_marshall_invalid_email_elements
 * Read should return Result::FAILURE when the number of
 * email elements exceeds RIL_MAX_NUM_EMAIL_COUNT.
*/
TEST(Marshalling, RIL_AdnRecordInfo_marshall_invalid_email_elements) {
  Marshal marshal;
  RIL_AdnRecordInfo recordInfo_r{};
  std::string s =
    "00000003"                // length of name
    "616263"                  // name
    "0000000a"                // length of number
    "31323334353637383930"    // number
    "00000005"                // email elements
    "0000000b"                // length of email[0]
    "616263406465662E636f6d"  // email[0]
    "0000000b"                // length of email[1]
    "616263406465662E636f6d"  // email[1]
    "0000000b"                // length of email[2]
    "616263406465662E636f6d"  // email[2]
    "0000000b"                // length of email[3]
    "616263406465662E636f6d"  // email[3]
    "0000000b"                // length of email[4]
    "616263406465662E636f6d"  // email[4]
    "00000002"                // anr elements
    "0000000a"                // length of ad_number[0]
    "30393837363534333231"    // ad_number[0]
    "00000004"                // length of ad_number[1]
    "31313131"                // ad_number[1]
  ;
  std::string invalid_input = QtiRilStringUtils::fromHex(s);
  marshal.setData(invalid_input);
  ASSERT_EQ(marshal.read(recordInfo_r), Marshal::Result::FAILURE);
  ASSERT_EQ(marshal.release(recordInfo_r), Marshal::Result::SUCCESS);
}

/* RIL_AdnRecordInfo_marshall_invalid_email_elements
 * Read should return Result::FAILURE when the number of
 * anr elements exceeds RIL_MAX_NUM_AD_COUNT.
*/
TEST(Marshalling, RIL_AdnRecordInfo_marshall_invalid_anr_elements) {
  Marshal marshal;
  RIL_AdnRecordInfo recordInfo_r{};
  std::string s =
    "00000003"                // length of name
    "616263"                  // name
    "0000000a"                // length of number
    "31323334353637383930"    // number
    "00000001"                // email elements
    "0000000b"                // length of email[0]
    "616263406465662E636f6d"  // email[0]
    "00000005"                // anr elements
    "0000000a"                // length of ad_number[0]
    "30393837363534333231"    // ad_number[0]
    "00000004"                // length of ad_number[1]
    "31313131"                // ad_number[1]
    "00000004"                // length of ad_number[2]
    "31313132"                // ad_number[2]
    "00000004"                // length of ad_number[3]
    "31313133"                // ad_number[3]
    "00000004"                // length of ad_number[4]
    "31313134"                // ad_number[4]
  ;
  std::string invalid_input = QtiRilStringUtils::fromHex(s);
  marshal.setData(invalid_input);
  ASSERT_EQ(marshal.read(recordInfo_r), Marshal::Result::FAILURE);
  ASSERT_EQ(marshal.release(recordInfo_r), Marshal::Result::SUCCESS);
}

/* RIL_AdnRecordInfo_marshall_large_email_elements
 * Read should return Result::NOT_ENOUGH_DATA when
 * user-specified number of email elements doesn't
 * match the actual array size.
*/
TEST(Marshalling, RIL_AdnRecordInfo_marshall_large_email_elements) {
  Marshal marshal;
  RIL_AdnRecordInfo recordInfo_r{};
  std::string s =
    "00000003"                // length of name
    "616263"                  // name
    "0000000a"                // length of number
    "31323334353637383930"    // number
    // RIL_MAX_NUM_EMAIL_COUNT  is 2.
    "f0000000"                // email elements
    "0000000b"                // length of email[0]
    "616263406465662E636f6d"  // email[0]
    "00000002"                // anr elements
    "0000000a"                // length of ad_number[0]
    "30393837363534333231"    // ad_number[0]
    "00000004"                // length of ad_number[1]
    "31313131"                // ad_number[1]
  ;
  std::string invalid_input = QtiRilStringUtils::fromHex(s);
  marshal.setData(invalid_input);
  ASSERT_NE(marshal.read(recordInfo_r), Marshal::Result::SUCCESS);
  ASSERT_EQ(marshal.release(recordInfo_r), Marshal::Result::SUCCESS);
}

/* RIL_AdnRecordInfo_marshall_large_anr_elements
 * Read should not succeed when
 * user-specified number of anr elements doesn't
 * match the actual array size.
*/
TEST(Marshalling, RIL_AdnRecordInfo_marshall_large_anr_elements) {
  Marshal marshal;
  RIL_AdnRecordInfo recordInfo_r{};
  std::string s =
    "00000003"                // length of name
    "616263"                  // name
    "0000000a"                // length of number
    "31323334353637383930"    // number
    "00000001"                // email elements
    "0000000b"                // length of email[0]
    "616263406465662E636f6d"  // email[0]
    // RIL_MAX_NUM_AD_COUNT is 4.
    "f0000000"                // anr elements
    "0000000a"                // length of ad_number[0]
    "30393837363534333231"    // ad_number[0]
    "00000004"                // length of ad_number[1]
    "31313131"                // ad_number[1]
  ;
  std::string invalid_input = QtiRilStringUtils::fromHex(s);
  marshal.setData(invalid_input);
  ASSERT_NE(marshal.read(recordInfo_r), Marshal::Result::SUCCESS);
  ASSERT_EQ(marshal.release(recordInfo_r), Marshal::Result::SUCCESS);
}
