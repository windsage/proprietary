/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <marshal/DialParams.h>
#include <string>
#include <telephony/ril_call.h>
#include <QtiRilStringUtils.h>

TEST(Marshalling, DialParams_marshall_basic) {
  Marshal marshal;
  char extra1[] = "DisplayText=hello";       // 0x446973706C6179546578743D68656C6C6F
  char extra2[] = "RetryCallFailReason=21";  // 0x526574727943616C6C4661696C526561736F6E3D3231
  char extra3[] =
      "RetryCallFailRadioTech=20";  // 0x526574727943616C6C4661696C526164696F546563683D3230
  char* extraList[] = { extra1, extra2, extra3 };
  RIL_UUS_Info info{
    .uusType = RIL_UUS_TYPE2_REQUIRED,
    .uusDcs = RIL_UUS_DCS_X244,
    .uusLength = 5,
    .uusData = "hello",  // 0x68656C6C6F
  };

  RIL_DialParams dial {
    .address = "5551230000",
    .clirMode = 1,
    .uusInfo = &info,
    .presentation = RIL_IP_PRESENTATION_NUM_DEFAULT,
    .hasCallDetails = true,
    .callDetails = {
      .callType = RIL_CALL_TYPE_VOICE,
      .callDomain = RIL_CALLDOMAIN_AUTOMATIC,
      .extrasLength = 3,
      .extras = extraList,
      .rttMode = RIL_RTT_FULL,
    },
    .hasIsConferenceUri = true,
    .isConferenceUri = false,
    .hasIsCallPull = true,
    .isCallPull = true,
    .hasIsEncrypted = false,
    .isEncrypted = false,
    .multiLineInfo = {
      .msisdn = "55512300",
      .registrationStatus = RIL_STATUS_DISABLE,
      .lineType = RIL_LINE_TYPE_PRIMARY,
    },
  };

  std::string s =
      "0000000a"                            // Address length
      "35353531323330303030"                // .address = "5551230000"
      "00000001"                            // .clirMode = 1
      "00000001"                            // UUS info length
      "00000003"                            // .uusType = RIL_UUS_TYPE2_REQUIRED
      "00000002"                            // .uusDcs = RIL_UUS_DCS_X244
      "00000005"                            // .uusLength = 5
      "68656C6C6F"                          // .uusData
      "00000002"                            // .presentation = RIL_IP_PRESENTATION_NUM_DEFAULT
      "01"                                  //  .hasCallDetails = true
      "00000001"                            // callDetails.callType
      "00000003"                            // callDetails.callDomain
      "00000003"                            // callDetails.extrasLength
      "00000011"                            // callDetails.extras[0] len
      "446973706C6179546578743D68656C6C6F"  // callDetails.extras[0]
      "00000016"                            // callDetails.extras[1] len
      "526574727943616C6C4661696C526561736F6E3D3231"        // callDetails.extras[1]
      "00000019"                                            // callDetails.extras[2] len
      "526574727943616C6C4661696C526164696F546563683D3230"  // callDetails.extras[2]
      "00000001"                                            // callDetails.rttMode
      "01"                                                  // hasIsConferenceUri
      "00"                                                  // isConferenceUri
      "01"                                                  // hasIsCallPull
      "01"                                                  // isCallPull
      "00"                                                  // hasIsEncrypted
      "00000008"                                            // multiLineInfo.msisdn len
      "3535353132333030"                                    // multiLineInfo.msisdn
      "00000001"                                            // multiLineInfo.registrationStatus
      "00000001"                                            // multiLineInfo.lineType
      ;

  std::string expected = QtiRilStringUtils::fromHex(s);
  ASSERT_EQ(marshal.write(dial), Marshal::Result::SUCCESS);
  ASSERT_EQ(expected.size(), marshal.dataSize());
  ASSERT_EQ(expected, (std::string)marshal);

  marshal.setDataPosition(0);
  RIL_DialParams dial_r{};
  ASSERT_NE(marshal.read(dial_r), Marshal::Result::FAILURE);
  ASSERT_STREQ(dial.address, dial_r.address);
  ASSERT_EQ(dial.clirMode, dial_r.clirMode);

  ASSERT_NE(dial_r.uusInfo, nullptr);
  RIL_UUS_Info uusInfo_r = *dial_r.uusInfo;
  ASSERT_EQ(info.uusType, uusInfo_r.uusType);
  ASSERT_EQ(info.uusDcs, uusInfo_r.uusDcs);
  ASSERT_EQ(info.uusLength, uusInfo_r.uusLength);
  ASSERT_STREQ(info.uusData, uusInfo_r.uusData);

  ASSERT_EQ(dial.presentation, dial_r.presentation);
  ASSERT_EQ(dial.hasCallDetails, dial_r.hasCallDetails);

  RIL_CallDetails callDetails_r = dial_r.callDetails;
  ASSERT_EQ(dial.callDetails.callType, callDetails_r.callType);
  ASSERT_EQ(dial.callDetails.callDomain, callDetails_r.callDomain);
  ASSERT_EQ(dial.callDetails.extrasLength, callDetails_r.extrasLength);
  char** extraList_r = callDetails_r.extras;
  ASSERT_STREQ(extraList[0], extraList_r[0]);
  ASSERT_STREQ(extraList[1], extraList_r[1]);
  ASSERT_STREQ(extraList[2], extraList_r[2]);
  ASSERT_EQ(dial.callDetails.rttMode, callDetails_r.rttMode);

  ASSERT_EQ(dial.hasIsConferenceUri, dial_r.hasIsConferenceUri);
  ASSERT_EQ(dial.isConferenceUri, dial_r.isConferenceUri);
  ASSERT_EQ(dial.hasIsCallPull, dial_r.hasIsCallPull);
  ASSERT_EQ(dial.isCallPull, dial_r.isCallPull);
  ASSERT_EQ(dial.hasIsEncrypted, dial_r.hasIsEncrypted);
  ASSERT_EQ(dial.isEncrypted, dial_r.isEncrypted);

  RIL_MultiIdentityLineInfo multiLine_r = dial_r.multiLineInfo;
  ASSERT_STREQ(dial.multiLineInfo.msisdn, multiLine_r.msisdn);
  ASSERT_EQ(dial.multiLineInfo.registrationStatus, multiLine_r.registrationStatus);
  ASSERT_EQ(dial.multiLineInfo.lineType, multiLine_r.lineType);

  ASSERT_NE(marshal.release(dial_r), Marshal::Result::FAILURE);
}

/* DialParams_marshall_large_extras_length
 * Description: Create an invalid parcel with a large
 *              callDetails.extrasLength that doesn't
 *              reflect the actual number of extras.
 * Expectation: read should return Marshal::Result::NOT_ENOUGH_DATA.
 */

TEST(Marshalling, DialParams_marshall_large_extras_length) {
  Marshal marshal;
  std::string s =
      "0000000a"                            // Address length
      "35353531323330303030"                // .address = "5551230000"
      "00000001"                            // .clirMode = 1
      "00000001"                            // UUS info length
      "00000003"                            // .uusType = RIL_UUS_TYPE2_REQUIRED
      "00000002"                            // .uusDcs = RIL_UUS_DCS_X244
      "00000005"                            // .uusLength = 5
      "68656C6C6F"                          // .uusData
      "00000002"                            // .presentation = RIL_IP_PRESENTATION_NUM_DEFAULT
      "01"                                  //  .hasCallDetails = true
      "00000001"                            // callDetails.callType
      "00000003"                            // callDetails.callDomain
      "100000"                              // callDetails.extrasLength - 1,048,576 in base 10
      "00000011"                            // callDetails.extras[0] len
      "446973706C6179546578743D68656C6C6F"  // callDetails.extras[0]
      "00000016"                            // callDetails.extras[1] len
      "526574727943616C6C4661696C526561736F6E3D3231"        // callDetails.extras[1]
      "00000019"                                            // callDetails.extras[2] len
      "526574727943616C6C4661696C526164696F546563683D3230"  // callDetails.extras[2]
      "00000001"                                            // callDetails.rttMode
      "01"                                                  // hasIsConferenceUri
      "00"                                                  // isConferenceUri
      "01"                                                  // hasIsCallPull
      "01"                                                  // isCallPull
      "00"                                                  // hasIsEncrypted
      "00000008"                                            // multiLineInfo.msisdn len
      "3535353132333030"                                    // multiLineInfo.msisdn
      "00000001"                                            // multiLineInfo.registrationStatus
      "00000001"                                            // multiLineInfo.lineType
      ;

  std::string invalid_input = QtiRilStringUtils::fromHex(s);
  marshal.setData(invalid_input);
  RIL_DialParams dial_r{};
  ASSERT_EQ(marshal.read(dial_r), Marshal::Result::NOT_ENOUGH_DATA);
  ASSERT_NE(marshal.release(dial_r), Marshal::Result::FAILURE);
}
