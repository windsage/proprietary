/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gtest/gtest.h>
#include <string>
#include <telephony/ril.h>
#include <QtiRilStringUtils.h>
#include <marshal/RILDataProfileInfo.h>
#include <Marshal.h>

 TEST(Marshalling, RIL_SetupDataCall_marshal) {
    RIL_DataProfileInfo dataProfile = {
        1,
        "IPV4V6",
        "IPV4V6",
        "IPV4V6",
         1,
         "IPV4V6",
         "IPV4V6",
         1,
         1,
         1,
         1,
         1,
         2,
         3,
         4,
         5,
         6,
         7,
         8,
         0,
         { 0, nullptr, 0, 0, {} }
    };
    RIL_RadioAccessNetworks accessNetwork = (RIL_RadioAccessNetworks)0x03;
    bool roamingAllowed = true;
    RIL_RadioDataRequestReasons reason = (RIL_RadioDataRequestReasons)0x01;
    Marshal marshal;
    uint16_t val;

    RIL_DataProfileInfo readdataProfile;
    RIL_RadioAccessNetworks readaccessNetwork ;
    bool readroamingAllowed ;
    RIL_RadioDataRequestReasons readreason ;

    std::string s =
        "0003"     //accessNetwork
        "00000001"     //4 bytes
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"     //
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000002"
        "00000003"
        "00000004"
        "00000005"
        "00000006"
        "00000007"
        "00000008"
        "00" //trafficDescriptorValid
        "01" //roamingAllowed
        "0001"   //reason
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);

    ASSERT_NE(marshal.write(static_cast<uint16_t>(accessNetwork)),Marshal::Result::FAILURE);
    ASSERT_NE(marshal.write(dataProfile),Marshal::Result::FAILURE);
    ASSERT_NE(marshal.writeBool(roamingAllowed),(int32_t)Marshal::Result::FAILURE);
    ASSERT_NE(marshal.write(static_cast<uint16_t>(reason)),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);

    ASSERT_NE(marshal.read(val), Marshal::Result::FAILURE);
    readaccessNetwork = (RIL_RadioAccessNetworks)val;
    ASSERT_NE(marshal.read(readdataProfile), Marshal::Result::FAILURE);
    readroamingAllowed = marshal.readBool();
    ASSERT_NE(marshal.read(val), Marshal::Result::FAILURE);
    readreason = (RIL_RadioDataRequestReasons)val;
    ASSERT_EQ(readaccessNetwork, accessNetwork);
    ASSERT_EQ(readreason, reason);
    ASSERT_STREQ(readdataProfile.apn, dataProfile.apn);
}

TEST(Marshalling, RIL_SetupDataCall_1_6_marshal) {
    RIL_SetUpDataCallParams_Ursp request = {
        .accessNetwork = (RIL_RadioAccessNetworks)0x03,
        .profileInfo = {
            1,
            "IPV4V6",
            "IPV4V6",
            "IPV4V6",
            1,
            "IPV4V6",
            "IPV4V6",
            1,
            1,
            1,
            1,
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8,
            0,
            { 0, nullptr, 0, 0, {} }
        },
        .roamingAllowed = true,
        .reason = (RIL_RadioDataRequestReasons)0x01,
        .addresses = "",
        .dnses = "",
        .sliceInfoValid = 0,
        .matchAllRuleAllowed = 0,
    };

    Marshal marshal;
    std::string s =
        "00000003"     //accessNetwork
        "00000001"     //4 bytes
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"     //
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000002"
        "00000003"
        "00000004"
        "00000005"
        "00000006"
        "00000007"
        "00000008"
        "00" //trafficDescriptorValid
        "01" //roamingAllowed
        "00000001" //reason
        "00000000" // addressLength
        "00000000" // dnsLength
        "00" //sliceInfoValid
        "00" //matchAllRule
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_SetUpDataCallParams result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.accessNetwork, result.accessNetwork);

    ASSERT_EQ(request.profileInfo.profileId, result.profileInfo.profileId);
    ASSERT_STREQ(request.profileInfo.apn, result.profileInfo.apn);
    ASSERT_STREQ(request.profileInfo.protocol, result.profileInfo.protocol);
    ASSERT_EQ(request.profileInfo.authType, result.profileInfo.authType);
    ASSERT_STREQ(request.profileInfo.user, result.profileInfo.user);
    ASSERT_STREQ(request.profileInfo.password, result.profileInfo.password);
    ASSERT_EQ(request.profileInfo.type, result.profileInfo.type);
    ASSERT_EQ(request.profileInfo.maxConnsTime, result.profileInfo.maxConnsTime);
    ASSERT_EQ(request.profileInfo.maxConns, result.profileInfo.maxConns);
    ASSERT_EQ(request.profileInfo.waitTime, result.profileInfo.waitTime);
    ASSERT_EQ(request.profileInfo.enabled, result.profileInfo.enabled);

    ASSERT_EQ(request.roamingAllowed, result.roamingAllowed);
    ASSERT_EQ(request.reason, result.reason);
}

TEST(Marshalling, RIL_SetupDataCall_1_6_marshal_SliceInfo) {
    RIL_SetUpDataCallParams_Ursp request = {
        .accessNetwork = (RIL_RadioAccessNetworks)0x03,
        .profileInfo = {
            1,
            "IPV4V6",
            "IPV4V6",
            "IPV4V6",
            1,
            "IPV4V6",
            "IPV4V6",
            1,
            1,
            1,
            1,
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8,
            0,
            { 0, nullptr, 0, 0, {} },
        },
        .roamingAllowed = true,
        .reason = (RIL_RadioDataRequestReasons)0x01,
        .addresses = "",
        .dnses = "",
        .sliceInfoValid = 1,
        .sliceInfo = {
            .sst = RIL_SLICE_SERVICE_TYPE_EMBB,
            .sliceDifferentiator = 4,
            .mappedHplmnSst = RIL_SLICE_SERVICE_TYPE_EMBB,
            .mappedHplmnSD = 5,
            .status = RIL_SLICE_STATUS_ALLOWED,
        },
        .matchAllRuleAllowed = 0,
    };

    Marshal marshal;
    std::string s =
        "00000003"     //accessNetwork
        "00000001"     //4 bytes
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"     //
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000006"     // Length of type 6 bytes
        "495056345636" // IPV4V6
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000001"
        "00000002"
        "00000003"
        "00000004"
        "00000005"
        "00000006"
        "00000007"
        "00000008"
        "00" //trafficDescriptorValid
        "01" //roamingAllowed
        "00000001" //reason
        "00000000" // addressLength
        "00000000" // dnsLength
        "01" //sliceInfoValid
        "00000001" // sst 1
        "00000004" // sliceDifferentiator 4
        "00000001" // mappedHplmnSst 1
        "00000005" // mappedHplmnSD 5
        "00000002" // status 2
        "00" //matchAllRule
        ;
    std::string expected = QtiRilStringUtils::fromHex(s);
    ASSERT_NE(marshal.write(request),Marshal::Result::FAILURE);
    ASSERT_EQ(expected.size(), marshal.dataSize());
    ASSERT_EQ(expected, (std::string)marshal);

    marshal.setDataPosition(0);
    RIL_SetUpDataCallParams_Ursp result = {};
    ASSERT_NE(marshal.read(result),Marshal::Result::FAILURE);
    ASSERT_EQ(request.sliceInfo.sst, result.sliceInfo.sst);
    ASSERT_EQ(request.sliceInfo.sliceDifferentiator, result.sliceInfo.sliceDifferentiator);
    ASSERT_EQ(request.sliceInfo.mappedHplmnSst, result.sliceInfo.mappedHplmnSst);
    ASSERT_EQ(request.sliceInfo.mappedHplmnSD, result.sliceInfo.mappedHplmnSD);
    ASSERT_EQ(request.sliceInfo.status, result.sliceInfo.status);
}
