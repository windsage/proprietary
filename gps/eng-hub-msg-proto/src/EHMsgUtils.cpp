/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Engine Hub Message Converter Utils

 This file contains the implementation of helper class for engine hub message
 conversion routines.

 Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#define LOG_TAG "Loc_EHMsgUtils"

#include <inttypes.h>
#include <dirent.h>

#include "EHMsgUtils.h"

using std::min;

void EHMsgUtils::freeUpEHMessageReportSvMeas(EHMessageReportSvMeas& ehMsgReportSvMeas) {
    // PBGnssSvMeasurementSet -> PBGnssSvMeasurementHeader svMeasSetHeader = 3;
    PBGnssSvMeasurementSet svMeasSet = ehMsgReportSvMeas.svmeasset();
    PBGnssSvMeasurementHeader svMeasSetHeader = svMeasSet.svmeassetheader();
    freeUpPBGnssSvMeasurementHeader(svMeasSetHeader);
    svMeasSet.clear_svmeassetheader();

    // PBGnssSvMeasurementSet -> repeated PBSVMeasurementStructType  svMeas = 5;
    uint32_t numSvMeas = svMeasSet.svmeas_size();
    for (uint32_t i=0; i < numSvMeas; i++) {
        // PBSVMeasurementStructType -> PBSVTimeSpeedInfo      svTimeSpeed = 14;
        // PBSVMeasurementStructType -> PBGnssLocDgnssSVMeasurement     dgnssSvMeas = 32;
        PBSVMeasurementStructType  svMeas = svMeasSet.svmeas(i);
        svMeas.clear_svtimespeed();
        svMeas.clear_dgnsssvmeas();
    }

    // PBGnssSvMeasurementSet svMeasSet = 2;
    ehMsgReportSvMeas.clear_svmeasset();
}

void EHMsgUtils::freeUpPBGnssSvMeasurementHeader(PBGnssSvMeasurementHeader& svMeasSetHeader) {
    // PBLeapSecondInfo                   leapSec = 2;
    svMeasSetHeader.clear_leapsec();
    // PBLocRcvrClockFreqInfo             clockFreq = 3;
    svMeasSetHeader.clear_clockfreq();
    // PBApTimeStampStructType          apBootTimeStamp = 4;
    // PBApTimeStampStructType -> PBTimespec      apTimeStamp = 1;
    PBApTimeStampStructType apBootTimeStamp = svMeasSetHeader.apboottimestamp();
    apBootTimeStamp.clear_aptimestamp();
    svMeasSetHeader.clear_apboottimestamp();
    // PBGnssInterSystemBias              gpsGloInterSystemBias = 5;
    svMeasSetHeader.clear_gpsglointersystembias();
    // PBGnssInterSystemBias              gpsBdsInterSystemBias = 6;
    svMeasSetHeader.clear_gpsbdsintersystembias();
    // PBGnssInterSystemBias              gpsGalInterSystemBias = 7;
    svMeasSetHeader.clear_gpsgalintersystembias();
    // PBGnssInterSystemBias              bdsGloInterSystemBias = 8;
    svMeasSetHeader.clear_bdsglointersystembias();
    // PBGnssInterSystemBias              galGloInterSystemBias = 9;
    svMeasSetHeader.clear_galglointersystembias();
    // PBGnssInterSystemBias              galBdsInterSystemBias = 10;
    svMeasSetHeader.clear_galbdsintersystembias();
    // PBGnssInterSystemBias              gpsL1L5TimeBias = 11;
    svMeasSetHeader.clear_gpsl1l5timebias();
    // PBGnssInterSystemBias              galE1E5aTimeBias = 12;
    svMeasSetHeader.clear_gale1e5atimebias();
    // PBGnssInterSystemBias              bdsb1ib2atimebias = 13;
    svMeasSetHeader.clear_bdsb1ib2atimebias();
    // PBGnssSystemTimeStructType       gpsSystemTime = 14;
    svMeasSetHeader.clear_gpssystemtime();
    // PBGnssSystemTimeStructType       galSystemTime = 15;
    svMeasSetHeader.clear_galsystemtime();
    // PBGnssSystemTimeStructType       bdsSystemTime = 16;
    svMeasSetHeader.clear_bdssystemtime();
    // PBGnssSystemTimeStructType       qzssSystemTime = 17;
    svMeasSetHeader.clear_qzsssystemtime();
    // PBGnssGloTimeStructType          gloSystemTime = 18;
    svMeasSetHeader.clear_glosystemtime();
    // PBLocGnssTimeExtInfo               gpsSystemTimeExt = 19;
    svMeasSetHeader.clear_gpssystemtimeext();
    // PBLocGnssTimeExtInfo               galSystemTimeExt = 20;
    svMeasSetHeader.clear_galsystemtimeext();
    // PBLocGnssTimeExtInfo               bdsSystemTimeExt = 21;
    svMeasSetHeader.clear_bdssystemtimeext();
    // PBLocGnssTimeExtInfo               qzssSystemTimeExt = 22;
    svMeasSetHeader.clear_qzsssystemtimeext();
    // PBLocGnssTimeExtInfo               gloSystemTimeExt = 23;
    svMeasSetHeader.clear_glosystemtimeext();
    // PBGnssInterSystemBias              gpsL1L2cTimeBias = 24;
    svMeasSetHeader.clear_gpsl1l2ctimebias();
    // PBGnssInterSystemBias              gloG1G2TimeBias = 25;
    svMeasSetHeader.clear_glog1g2timebias();
    // PBGnssInterSystemBias              bdsB1iB1ctimebias = 26;
    svMeasSetHeader.clear_bdsb1ib1ctimebias();
    // PBGnssInterSystemBias              galE1E5bTimeBias = 27;
    svMeasSetHeader.clear_gale1e5btimebias();
}

void EHMsgUtils::freeUpEHMessageReportSv(EHMessageReportSv& ehMsgReportSv) {
    PBGnssSvNotification svNotification = ehMsgReportSv.svnotification();
    // PBGnssSvNotification -> repeated PBGnssSv gnssSvs = 2;
    uint32_t numSvs = svNotification.gnsssvs_size();
    for (uint32_t i=0; i < numSvs; i++) {
        svNotification.gnsssvs(i);
    }

    // PBGnssSvNotification svNotification = 2;
    ehMsgReportSv.clear_svnotification();
}

void EHMsgUtils::freeUpEHMessageProtocolRoutingTableUpdate(
        EHMessageProtocolRoutingTableUpdate& ehMsgProtoRoutTblUpdt) {
    // repeated EHRoutingTableEntry eHubRoutingTable = 2;
    uint32_t numTables = ehMsgProtoRoutTblUpdt.ehubroutingtable_size();
    for (uint32_t i=0; i < numTables; i++) {
        EHRoutingTableEntry ehRoutTblEntry = ehMsgProtoRoutTblUpdt.ehubroutingtable(i);
        freeUpEHRoutingTableEntry(ehRoutTblEntry);
    }
    return;
}

void EHMsgUtils::freeUpEHRoutingTableEntry(EHRoutingTableEntry& ehRoutingTblEntry) {
    // EHEngineConnectionInfo engConnectInfo = 2;
    // oneof values {
    //     string socketName = 1;
    //     EHEngineConnectionIpAddrInfo pluginIpAddrInfo = 2;
    EHConnectionType engConnectionType = ehRoutingTblEntry.engconnectiontype();
    EHEngineConnectionInfo engConnectInfo = ehRoutingTblEntry.engconnectinfo();
    if (EH_ENGINE_CONNECT_TYPE_LOCAL == engConnectionType) {
        engConnectInfo.clear_socketname();
    } else {
        // EH_ENGINE_CONNECT_TYPE_INET
        // EHEngineConnectionIpAddrInfo -> string pluginIpAddress = 1;
        EHEngineConnectionIpAddrInfo pluginIpAddrInfo = engConnectInfo.pluginipaddrinfo();
        pluginIpAddrInfo.clear_pluginipaddress();
        engConnectInfo.clear_pluginipaddrinfo();
    }
    ehRoutingTblEntry.clear_engconnectinfo();

    // repeated uint32 regReportMask = 3;
    ehRoutingTblEntry.clear_regreportmask();
    // EHSubModifiers  subModifiers = 5;
    EHSubModifiers  subModifiers = ehRoutingTblEntry.submodifiers();
    freeUpEHSubModifiers(subModifiers);
    ehRoutingTblEntry.clear_submodifiers();
    return;
}

void EHMsgUtils::freeUpEHMessageReportSvPolynomial(EHMessageReportSvPolynomial& ehMsgReportSvPoly) {
    // PBGnssSvPolynomial -> repeated float      polyCoeffXYZ0 = 8;
    // PBGnssSvPolynomial -> repeated float      polyCoefXYZN = 9;
    // PBGnssSvPolynomial -> repeated float       polyCoefOther = 10;
    // PBGnssSvPolynomial -> repeated float      velCoef = 20;
    PBGnssSvPolynomial svPolynomial = ehMsgReportSvPoly.svpolynomial();
    svPolynomial.clear_polycoeffxyz0();
    svPolynomial.clear_polycoefxyzn();
    svPolynomial.clear_polycoefother();
    svPolynomial.clear_velcoef();

    // PBGnssSvPolynomial svPolynomial = 2;
    ehMsgReportSvPoly.clear_svpolynomial();
    return;
}

void EHMsgUtils::freeUpEHMessageReportSvEphemeris(EHMessageReportSvEphemeris& ehMsgReportSvEph) {
    // PBGnssSvEphemerisReport svEphemeris = 2;
    PBGnssSvEphemerisReport svEphemeris = ehMsgReportSvEph.svephemeris();
    freeUpPBGnssSvEphemerisReport(svEphemeris);
    ehMsgReportSvEph.clear_svephemeris();
    return;
}

void EHMsgUtils::freeUpPBGnssSvEphemerisReport(PBGnssSvEphemerisReport& gnssSvEphReport) {
    // PBGnssSystemTimeStructType systemTime = 3;
    gnssSvEphReport.clear_systemtime();

    // oneof ephInfo
    PBGnss_LocSvSystemEnumType gnssConstellation = gnssSvEphReport.gnssconstellation();
    switch (gnssConstellation) {
        case PB_GNSS_LOC_SV_SYSTEM_GPS:
        {
            // PBGpsEphemerisResponse gpsEphemeris = 4;
            PBGpsEphemerisResponse gpsEphemerisResp = gnssSvEphReport.gpsephemeris();

            // PBGpsEphemerisResponse -> repeated PBGpsEphemeris gpsEphemerisData = 2;
            uint32_t numData = gpsEphemerisResp.gpsephemerisdata_size();
            for (uint32_t i=0; i < numData; i++) {
                // PBGpsEphemeris -> PBGnssEphCommon commonEphemerisData = 1;
                PBGpsEphemeris gpsEphData = gpsEphemerisResp.gpsephemerisdata(i);
                gpsEphData.clear_commonephemerisdata();
            }
            gpsEphemerisResp.clear_gpsephemerisdata();

            gnssSvEphReport.clear_gpsephemeris();
        }
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GALILEO:
        {
            // PBGalileoEphemerisResponse galileoEphemeris = 7;
            PBGalileoEphemerisResponse galileoEphemerisResp = gnssSvEphReport.galileoephemeris();

            // PBGalileoEphemerisResponse -> repeated PBGalileoEphemeris galEphemerisData = 2;
            uint32_t numData = galileoEphemerisResp.galephemerisdata_size();
            for (uint32_t i=0; i < numData; i++) {
                // PBGalileoEphemeris -> PBGnssEphCommon commonEphemerisData = 1;
                PBGalileoEphemeris galEphData = galileoEphemerisResp.galephemerisdata(i);
                galEphData.clear_commonephemerisdata();
            }
            galileoEphemerisResp.clear_galephemerisdata();

            gnssSvEphReport.clear_galileoephemeris();
        }
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GLONASS:
        {
            // PBGlonassEphemerisResponse glonassEphemeris = 5;
            PBGlonassEphemerisResponse glonassEphemerisResp = gnssSvEphReport.glonassephemeris();

            // PBGlonassEphemerisResponse -> repeated PBGlonassEphemeris gloEphemerisData = 2;
            uint32_t numData = glonassEphemerisResp.gloephemerisdata_size();
            for (uint32_t i=0; i < numData; i++) {
                // PBGlonassEphemeris -> PBGnssEphCommon commonEphemerisData = 1;
                PBGlonassEphemeris gloEphData = glonassEphemerisResp.gloephemerisdata(i);
                // PBGlonassEphemeris -> repeated float position = 13;
                // PBGlonassEphemeris -> repeated float velocity = 14;
                // PBGlonassEphemeris -> repeated float acceleration = 15;
                gloEphData.clear_position();
                gloEphData.clear_velocity();
                gloEphData.clear_acceleration();
            }
            glonassEphemerisResp.clear_gloephemerisdata();

            gnssSvEphReport.clear_glonassephemeris();
        }
        break;
        case PB_GNSS_LOC_SV_SYSTEM_BDS:
        {
            // PBBdsEphemerisResponse bdsEphemeris = 6;
            PBBdsEphemerisResponse bdsEphemerisResp = gnssSvEphReport.bdsephemeris();

            // PBBdsEphemerisResponse -> repeated PBBdsEphemeris bdsEphemerisData = 2;
            uint32_t numData = bdsEphemerisResp.bdsephemerisdata_size();
            for (uint32_t i=0; i < numData; i++) {
                // PBBdsEphemeris -> PBGnssEphCommon commonEphemerisData = 1;
                PBBdsEphemeris bdsEphData = bdsEphemerisResp.bdsephemerisdata(i);
                bdsEphData.clear_commonephemerisdata();
            }
            bdsEphemerisResp.clear_bdsephemerisdata();

            gnssSvEphReport.clear_bdsephemeris();
        }
        break;
        case PB_GNSS_LOC_SV_SYSTEM_QZSS:
        {
            // PBQzssEphemerisResponse qzssEphemeris = 8;
            PBQzssEphemerisResponse qzssEphemerisResp = gnssSvEphReport.qzssephemeris();

            // PBQzssEphemerisResponse-> repeated PBGpsEphemeris qzssEphemerisData = 2;
            uint32_t numData = qzssEphemerisResp.qzssephemerisdata_size();
            for (uint32_t i=0; i < numData; i++) {
                // PBGpsEphemeris -> PBGnssEphCommon commonEphemerisData = 1;
                PBGpsEphemeris qzssEphData = qzssEphemerisResp.qzssephemerisdata(i);
                qzssEphData.clear_commonephemerisdata();
            }
            qzssEphemerisResp.clear_qzssephemerisdata();

            gnssSvEphReport.clear_qzssephemeris();
        }
        break;
        default:
        break;
    }

    return;
}

void EHMsgUtils::freeUpEHMessageReportPosition(EHMessageReportPosition& ehMsgReportPos) {
    // PBUlpLocation location = 2;
    // PBUlpLocation -> PBLocGpsLocation    gpsLocation = 1;
    PBUlpLocation location = ehMsgReportPos.location();
    location.clear_gpslocation();
    ehMsgReportPos.clear_location();

    // PBGpsLocationExtended locationExtended = 3;
    PBGpsLocationExtended locExt = ehMsgReportPos.locationextended();
    freeUpPBGpsLocationExtended(locExt);
    ehMsgReportPos.clear_locationextended();
    return;
}

void EHMsgUtils::freeUpPBGpsLocationExtended(PBGpsLocationExtended& gpsLocExt) {
    // PBApTimeStampStructType         timeStamp = 15;
    // PBApTimeStampStructType -> PBTimespec      apTimeStamp = 1;
    PBApTimeStampStructType aptimeStamp = gpsLocExt.timestamp();
    aptimeStamp.clear_aptimestamp();
    gpsLocExt.clear_timestamp();

    // PBGnssSvUsedInPosition gnss_sv_used_ids = 16;
    // PBGnssSvMbUsedInPosition gnss_mb_sv_used_ids = 17;
    // PBGnssLocationPositionDynamics bodyFrameData = 21;
    // PBGPSTimeStruct gpsTime = 22;
    // PBLocExtDOP extDOP = 24;
    // repeated uint32 referenceStation = 38;
    gpsLocExt.clear_gnss_sv_used_ids();
    gpsLocExt.clear_gnss_mb_sv_used_ids();
    gpsLocExt.clear_bodyframedata();
    gpsLocExt.clear_gpstime();
    gpsLocExt.clear_extdop();
    gpsLocExt.clear_referencestation();
    // repeated PBGpsMeasUsageInfo measUsageInfo = 40;
    gpsLocExt.clear_measusageinfo();

    // PBGnssSystemTime gnssSystemTime = 23;
    // PBGnssSystemTime  -> PBSystemTimeStructUnion u = 2;
    PBGnssSystemTime gnssSysTime = gpsLocExt.gnsssystemtime();
    PBGnss_LocSvSystemEnumType gnssSystemTimeSrc = gnssSysTime.gnsssystemtimesrc();
    PBSystemTimeStructUnion sysTimeStruct = gnssSysTime.u();
    switch (gnssSystemTimeSrc) {
        case PB_GNSS_LOC_SV_SYSTEM_GPS:
            // PBGnssSystemTimeStructType gpsSystemTime = 1;
            sysTimeStruct.clear_gpssystemtime();
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GALILEO:
            // PBGnssSystemTimeStructType galSystemTime = 2;
            sysTimeStruct.clear_galsystemtime();
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GLONASS:
            // PBGnssGloTimeStructType    gloSystemTime = 5;
            sysTimeStruct.clear_glosystemtime();
        break;
        case PB_GNSS_LOC_SV_SYSTEM_BDS:
            // PBGnssSystemTimeStructType bdsSystemTime = 3;
            sysTimeStruct.clear_bdssystemtime();
        break;
        case PB_GNSS_LOC_SV_SYSTEM_QZSS:
            // PBGnssSystemTimeStructType qzssSystemTime = 4;
            sysTimeStruct.clear_qzsssystemtime();
        break;
        // No variable to PB_GNSS_LOC_SV_SYSTEM_NAVIC
        default:
        break;
    }
    gnssSysTime.clear_u();
    gpsLocExt.clear_gnsssystemtime();
}

void EHMsgUtils::freeUpEHMessageReportCdfwQDgnssObservation(
        EHMessageReportCdfwQDgnssObservation& ehMsgReportCdfwQdgnssObs) {
    PBDGnssObservation qDgnssObserv = ehMsgReportCdfwQdgnssObs.qdgnssobserv();
    // PBDGnssObservation -> PBGnssEpochTime             epochTime = 2;
    // message PBGnssEpochTime {
    //  PBGnssEpochTime -> oneof gnssEpochTime {
    //           uint32  timeOfWeek = 1;
    //           glonassEpochTime gloEpochTime = 2;
    PBDGnssConstellationIdentifier  gnssConstId = qDgnssObserv.constellationidentifier();
    if (gnssConstId == PB_DGNSS_CONSTELLATION_IDENTIFIER_GLONASS) {
        PBGnssEpochTime epochTime = qDgnssObserv.epochtime();
        epochTime.clear_gloepochtime();
    }
    qDgnssObserv.clear_epochtime();

    // PBDGnssObservation -> PBDGnssObservationStatusFlags   statusFlags = 3;
    qDgnssObserv.clear_statusflags();

    // PBDGnssObservation -> repeated PBDGnssMeasurement dGnssMeasurement = 7;
    qDgnssObserv.clear_dgnssmeasurement();

    // PBDGnssObservation qDgnssObserv = 2;
    ehMsgReportCdfwQdgnssObs.clear_qdgnssobserv();
}
