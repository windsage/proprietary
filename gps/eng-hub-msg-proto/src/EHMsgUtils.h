/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 Engine Hub Message Converter Utils

 This file contains the implementation of helper class for engine hub message
 conversion routines.

 Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#ifndef EHUB_MSG_UTILS_H
#define EHUB_MSG_UTILS_H

// Protobuff message headers
#include "EHMsg.pb.h"
#include "LocationDataTypes.pb.h"

class EHMsgUtils {
public:
    EHMsgUtils() = default;
    virtual ~EHMsgUtils() = default;

    static void freeUpEHMessageReportSvMeas(EHMessageReportSvMeas& ehMsgReportSvMeas);
    static void freeUpEHMessageReportSv(EHMessageReportSv& ehMsgReportSv);
    static void freeUpEHMessageReportSvPolynomial(EHMessageReportSvPolynomial& ehMsgReportSvPoly);
    static void freeUpEHMessageReportSvEphemeris(EHMessageReportSvEphemeris& ehMsgReportSvEph);
    static void freeUpEHMessageReportPosition(EHMessageReportPosition& ehMsgReportPos);
    static void freeUpEHMessageProtocolRoutingTableUpdate(
            EHMessageProtocolRoutingTableUpdate& ehMsgProtoRoutTblUpdt);
    static void freeUpEHRoutingTableEntry(EHRoutingTableEntry& ehRoutingTblEntry);
    static void freeUpEHMessageReportCdfwQDgnssObservation(
        EHMessageReportCdfwQDgnssObservation& ehMsgReportCdfwQdgnssObs);

    static inline void freeUpEHMessageReportKlobucharIonoModel(
            EHMessageReportKlobucharIonoModel& ehMsgPreportKloIonoModel) {
        // PBGnssKlobucharIonoModel -> PBGnssSystemTimeStructType systemTime = 2;
        PBGnssKlobucharIonoModel klobIonoModel = ehMsgPreportKloIonoModel.klobucharionomodel();
        klobIonoModel.clear_systemtime();

        // PBGnssKlobucharIonoModel klobucharIonoModel = 2;
        ehMsgPreportKloIonoModel.clear_klobucharionomodel();
        return;
    }

    static inline void freeUpEHMessageReportGlonassAdditionalParams(
            EHMessageReportGlonassAdditionalParams& ehMsgGloAddlParams) {
        // PBGnssAdditionalSystemInfo additionalSystemInfo = 2;
        ehMsgGloAddlParams.clear_additionalsysteminfo();
        return;
    }

    static inline void freeUpEHMessageStatusLeverArmConfig(
            EHMessageStatusLeverArmConfig& ehMsgStatLeverArmCfg) {
        // PBLeverArmConfigInfo -> PBLeverArmParams   gnssToVRP = 2;
        // PBLeverArmConfigInfo -> PBLeverArmParams   drImuToGnss = 3;
        // PBLeverArmConfigInfo -> PBLeverArmParams   veppImuToGnss = 4;
        PBLeverArmConfigInfo leverArmInfo = ehMsgStatLeverArmCfg.leverarminfo();
        leverArmInfo.clear_gnsstovrp();
        leverArmInfo.clear_drimutognss();
        leverArmInfo.clear_veppimutognss();

        // PBLeverArmConfigInfo leverArmInfo = 1;
        ehMsgStatLeverArmCfg.clear_leverarminfo();
        return;
    }

    static inline void freeUpEHMessageStatusB2sMountParams(
            EHMessageStatusB2sMountParams& ehMsgStatB2sMntParams) {
        // PBBodyToSensorMountParams b2sMountParams = 1;
        ehMsgStatB2sMntParams.clear_b2smountparams();
        return;
    }

    static inline void freeUpEHMessageSessionDeleteAidingData(
            EHMessageSessionDeleteAidingData& ehMsgSessDelAidData) {
        // PBGnssAidingData -> PBGnssAidingDataSv sv = 2;
        // PBGnssAidingData -> PBGnssAidingDataCommon common = 3;
        PBGnssAidingData gpsAidingData = ehMsgSessDelAidData.gpsaidingdata();
        gpsAidingData.clear_sv();
        gpsAidingData.clear_common();

        // PBGnssAidingData gpsAidingData = 1;
        ehMsgSessDelAidData.clear_gpsaidingdata();
        return;
    }

    static inline void freeUpEHMessageSessionSetFixMode(
            EHMessageSessionSetFixMode& ehMsgSessSetFixMode) {
        // PBLocPosMode -> string credentials = 7;
        // PBLocPosMode -> string provider = 8;
        PBLocPosMode pbLocPostMode = ehMsgSessSetFixMode.fixmode();
        pbLocPostMode.clear_credentials();
        pbLocPostMode.clear_provider();

        // PBLocPosMode fixMode = 1;
        ehMsgSessSetFixMode.clear_fixmode();
        return;
    }

    static inline void freeUpEHMessageReportCdfwQDgnssReferenceStation(
            EHMessageReportCdfwQDgnssReferenceStation& ehMsgReportCdfwQdgnssRefStat) {
        // PBReferenceStation - PBStationStatusIndicatorMask  statusIndicator = 2;
        PBReferenceStation qDgnssRefStation = ehMsgReportCdfwQdgnssRefStat.qdgnssrefstation();
        qDgnssRefStation.clear_statusindicator();

        // PBReferenceStation qDgnssRefStation = 2;
        ehMsgReportCdfwQdgnssRefStat.clear_qdgnssrefstation();
        return;
    }

private:
    static void freeUpPBGnssSvMeasurementHeader(PBGnssSvMeasurementHeader& svMeasSetHeader);
    static void freeUpPBGpsLocationExtended(PBGpsLocationExtended& gpsLocExt);
    static void freeUpPBGnssSvEphemerisReport(PBGnssSvEphemerisReport& gnssSvEphReport);

    static inline void freeUpEHSubModifiers(EHSubModifiers& ehSubMod) {
        // EHSubscriptionModifier  svMeasSubModifier = 1;
        ehSubMod.clear_svmeassubmodifier();
        // EHSubscriptionModifier  nHzSvMeasSubModifier = 2;
        ehSubMod.clear_nhzsvmeassubmodifier();
        // EHSubscriptionModifier  svPolySubModifier = 3;
        ehSubMod.clear_svpolysubmodifier();
        // EHSubscriptionModifier  svEphSubModifier = 4;
        ehSubMod.clear_svephsubmodifier();
        // EHSubscriptionModifier  ionoModelSubModifier = 5;
        ehSubMod.clear_ionomodelsubmodifier();
        return;
    }

};

#endif // #ifndef EHUB_MSG_UTILS_H
