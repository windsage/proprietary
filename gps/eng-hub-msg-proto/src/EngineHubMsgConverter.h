/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 Engine Hub Message Converter

 This file contains the implementation of helper class for engine hub message
 conversion routines.

 Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#ifndef ENGINE_HUB_MSG_CONV_H
#define ENGINE_HUB_MSG_CONV_H

#include <loc_pla.h>
#include <log_util.h>
#include <loc_misc_utils.h>

#include <LocIpc.h>
#include <EngineHubMsg.h>
#include <loc_misc_utils.h>

// Protobuff message headers
#include "EHMsg.pb.h"
#include "LocationDataTypes.pb.h"

using namespace loc_util;

class EngineHubMsgConverter {
public:
    EngineHubMsgConverter() {
        LOC_LOGi("EHubConverter: EH version  Major:%d, Minor:%d", EHMSG_VER_MAJOR,
                 EHMSG_VER_MINOR);
        LOC_LOGi("EHubConverter: EH LocationDataTypes version  Major:%d, Minor:%d",
                 PB_LOCDATATYPES_VER_MAJOR, PB_LOCDATATYPES_VER_MINOR);
    }
    virtual ~EngineHubMsgConverter() {}

    // RIGID TO PROTOBUF FORMAT
    // ************************
    // Convert rigid structures to protobuff msg format. Local structure is converted
    // to protobuff structure and serialized to string passed in the function. This
    // payload can be passed over IPC.

    // generic function to add the protobuf string buffer for structure in EHMsg with
    // message counter and generate the serialize string to be passed to EP.
    int pbPopulateEHMsgWithCounterToSend(const string &ehMsgPayLoad, EHMsgId msgId,
            uint32_t msgCounter, string &os);
    // Convert GnssSvMeasurementSet -> EHMessageReportSvMeas payload
    int pbPopulateEHMsgReportSvMeas(const GnssSvMeasurementSet &svMeasSet,
            uint64_t ptpTimeNs, string &os);
    // Convert GnssSvNotification -> PBGnssSvNotification payload
    int pbPopulateEHMsgReportSv(const GnssSvNotification &gnssSvNotif, string &os);
    // Convert GnssSvPolynomial -> PBGnssSvPolynomial payload
    int pbPopulateEHMsgSvPolynomial(const GnssSvPolynomial &svPoly, string &os);
    // Convert GnssSvEphemerisReport -> PBGnssSvEphemerisReport payload
    int pbPopulateEHMsgSvEphemeris(const GnssSvEphemerisReport &gnssEph, string &os);
    // Convert position report information to payload
    int pbPopulateEHMsgReportPosition(const UlpLocation &location,
            const GpsLocationExtended &locationExtended, enum loc_sess_status status, string &os);
    // Convert GnssKlobucharIonoModel ->  PBGnssKlobucharIonoModel payload
    int pbPopulateEHMsgKlobucharIonoModel(const GnssKlobucharIonoModel &klobIonoMdl, string &os);
    // Convert GnssAdditionalSystemInfo ->  PBGnssAdditionalSystemInfo payload
    int pbPopulateEHMsgGlonassAddnParams(const GnssAdditionalSystemInfo &gnssAddSysInfo,
            string &os);
    // Session Start EHMessage
    int pbPopulateEHMessageSessionStart(string &pbStr);
    // Session Stop EHMessage
    int pbPopulateEHMessageSessionStop(string &pbStr);
    // Protocol Engine Hub Up Message
    int pbPopulateEHMessageProtocolEngineHubUp(string &pbStr);
    // Connectivity Status EHMessage
    int pbPopulateEHMessageStatusConnectivity(string &pbStr, bool connected);
    // Convert LeverArmConfigInfo to PBLeverArmConfigInfo
    int pbPopulateEHMessageStatusLeverArmConfig(const LeverArmConfigInfo &leverArmConfgInfo,
            string &os);
    // Convert BodyToSensorMountParams to PBBodyToSensorMountParams
    int pbPopulateEHMessageStatusB2sMountParams(const BodyToSensorMountParams &b2sParams,
            string &os);
    // Convert GnssAidingData to PBGnssAidingData
    int pbPopulateEHMessageSessionDeleteAidingData(const GnssAidingData &gnssAidData, string &os);
    // Convert LocPosMode to PBLocPosMode
    int pbPopulateEHMessageSessionSetFixMode(const LocPosMode &locPosMode, string &os);
    // Convert EHubMessageProtocolRoutingTableUpdate to EHMessageProtocolRoutingTableUpdate
    int pbPopulateEHMessageProtocolRoutingTableUpdate(
            const EHubMessageProtocolRoutingTableUpdate &eHubMsgPrtclRoutngTblUpdt, string &os);
#ifdef FEATURE_CDFW
    // Conversion function for QDGnss information to protobuf
    // Convert ReferenceStation to PBReferenceStation
    int pbPopulateEHMessageQDgnssReferenceStation(const ReferenceStation &refStation, string &os);
    // Convert DGnssObservation to PBDGnssObservation
    int pbPopulateEHMessageQDgnssObservation(const DGnssObservation &dGnssObs, string &os);
#endif

    int pbPopulateEHMessageQWESFeatureStatusInfo(const ehFeatureStatus &status,
            const ehLicenseType &type, string &os);
    int pbPopulateEHMessageQWESInstallLicenseResp(const ehInstallLicenseStatus & status,
            string &os);
    int pbPopulateEHMessageQWESProvideAttestationStatement(const uint8_t *outBuff,
            uint32_t bufLen, const ehAttestationStatus &status, string &os);
    int pbPopulateEHMessageQWESForceSyncNotify(int32_t status, string &os);
    // PROTOBUF TO LOCAL STRUCT
    // ************************
    // Convert protobuff msg received to local struct. Protobuff msg received is deserialized
    // and converted into local structures.
    // Responsibility of caller to delete the memory returned by these decode* functions.
    EHubMessageProtocolRegMsg* decodeEHRoutingTableEntry(EHRoutingTableEntry &pbEhRoutingTable);
    EHubMessageReportPosition* decodeEHMessageReportPosition(
            EHMessageReportPosition &pbEhMsgRprtPos);
    EHubMessageReportSv* decodeEHMessageReportSv(EHMessageReportSv &pbEhMsgRprtSv);

    EHubMessageQWESInstallLicenseReq* decodeEHMessageInstallLicenseReq(
            EHMessageQWESInstallLicenseReq &pbQwesInstallLicenseReq);
    EHubMessageQWESReqAttestationStatement* decodeEHMessageAttestationReq(
            EHMessageQWESReqAttestationStatement &pbQwesAttestationReq);
    // Parse data from received protobuf serialised string.
    bool ParseFromString(::google::protobuf::MessageLite& engHubMsg, const string& data);
    bool ParseFromString(::google::protobuf::MessageLite& engHubMsg,
            const EngineHubMessage& eHubPayLoadMsg);
    bool ParseFromString(::google::protobuf::MessageLite& engHubMsg, uint32_t &msgCounter,
            string &senderName, const EngineHubMessageWithMsgCounter& eHubPayLoadMsg);

    // Convert EHubMessageReportSvMeas -> EHMessageReportSvMeas payload based on SubModifiers
    bool pbPopulateEHMsgReportSvMeasBasedOnSubModifiers(
            const EHubMessageReportSvMeas* inMsg, GnssConstellationTypeMask regConstellationMask,
            GnssSignalTypeMask regSignalTypeMask, uint8_t * &outMsg, size_t &outMsgSize,
            uint32_t msgCounter);
    // Convert EHubMessageReportSvEphemeris to EHMessageReportSvEphemeris
    bool pbPopulateEHMsgSvEphemerisBasedOnSubModifiers(
            const EHubMessageReportSvEphemeris* inMsg, GnssSignalTypeMask gnssSignalMask,
            uint8_t * &outMsg, size_t &outMsgSize, uint32_t msgCounter);

    // helper function
    // Check if SV is needed to be included based on SV system.
    bool svNeededBasedOnSvSystem(GnssConstellationTypeMask regConstellationMask,
                                 Gnss_LocSvSystemEnumType gnssSystem);

private:
    // RIGID TO PROTOBUF FORMAT
    // ************************
    // helper function for structure conversion to protobuf format
    int convertUlpLocationToPB(const UlpLocation &ulpLoc, PBUlpLocation *pbUlpLoc);
    int convertGpsLocExtToPB(const GpsLocationExtended &gpsLocExt,
            PBGpsLocationExtended *pbGpsLocExt);
    int convertLocGpsLocationToPB(const LocGpsLocation &locGpsLoc, PBLocGpsLocation *pbLocGpsLoc);
    int convertGnssSvMeasToPB(const GnssSvMeasurementHeader &gnssSvMeasHeader,
            PBGnssSvMeasurementHeader* pPbSvMeasHeader);
    int convertGnssInterSysBiasToPB (const Gnss_InterSystemBiasStructType &gnssIST,
            PBGnssInterSystemBias* pPBIST);
    int convertGnssSystemTimeStructToPB(const GnssSystemTimeStructType &gnssSystemTime,
            PBGnssSystemTimeStructType * pPBGnssSystemTimeInfo);
    int convertGnssLocGnssTimeExtInfoToPB(const Gnss_LocGnssTimeExtStructType &gnssSystemTimeExt,
            PBLocGnssTimeExtInfo * pPBGnssSystemTimeExtInfo);
    int convertGnssGloTimeInfoToPB(const GnssGloTimeStructType &gloSystemTime,
            PBGnssGloTimeStructType* pPBGnssGloTimeInfo);
    int convertGnssLocSVTimeSpeedToPB(const Gnss_LocSVTimeSpeedStructType &gnssSvTimeSpeed,
            PBSVTimeSpeedInfo* pPbSVTimeSpeed);
    int convertGpsEphRespToPB(const GpsEphemerisResponse &gpsEphRsp,
            PBGpsEphemerisResponse *pbGnssEphRsp);
    int convertGlonassEphRespToPB(const GlonassEphemerisResponse &gpsEphRsp,
            PBGlonassEphemerisResponse *pbGloEphRsp);
    int convertBdsEphRespToPB(const BdsEphemerisResponse &gpsEphRsp,
            PBBdsEphemerisResponse *pbBdsEphRsp);
    int convertGalileoEphRespToPB(const GalileoEphemerisResponse &gpsEphRsp,
            PBGalileoEphemerisResponse *pbGalEphRsp);
    int convertQzssEphRespToPB(const QzssEphemerisResponse &gpsEphRsp,
            PBQzssEphemerisResponse *pbQzssEph);
    int convertGnssEphCommonToPB(const GnssEphCommon &gpsEphRsp, PBGnssEphCommon *pbGnssEph);
    int convertGpsEphemerisToPB(const GpsEphemeris &gpsEphRsp, PBGpsEphemeris *pbGpsEph);
    int convertGlonassEphemerisToPB(const GlonassEphemeris &gloEph, PBGlonassEphemeris *pbGloEph);
    int convertBdsEphemerisToPB(const BdsEphemeris &bdsEph, PBBdsEphemeris *pbBdsEph);
    int convertGalEphemerisToPB(const GalileoEphemeris &galEph, PBGalileoEphemeris *pbGalEph);
    int convertGnssSvUsedInPositionToPB(const GnssSvUsedInPosition &gnssSvUsedInFix,
            PBGnssSvUsedInPosition *pbGnssSvUsedInFix);
    int convertGnssSvMbUsedInPositionToPB(const GnssSvMbUsedInPosition  &gnssSvMbUsedInFix,
            PBGnssSvMbUsedInPosition *pbGnssSvMbUsedInFix);
    int convertGnssLocationPositionDynamicsToPB(
            const GnssLocationPositionDynamics &gnssLocPosDynmcs,
            const GnssLocationPositionDynamicsExt &gnssLocPosDynmcsExt,
            PBGnssLocationPositionDynamics *pbGnssLocPosDynmcs);
    int convertGnssSystemTimeToPB(const GnssSystemTime &gnssSysTime,
            PBGnssSystemTime *pbGnssSysTime);
    int convertGnssSVMeasurementStructTypeToPB(const Gnss_SVMeasurementStructType &svMeas,
            PBSVMeasurementStructType* svInfo);
    int convertGnssAidingDataSvToPB(const GnssAidingDataSv &gnssAidingDataSv,
            PBGnssAidingDataSv* pbGnssAidingDataSv);
    int convertGnssAidingDataCommonToPB(const GnssAidingDataCommon &gnssAidingDataCommon,
            PBGnssAidingDataCommon* pbGnssAidingDataCommon);
    int convertLocPosModeToPB(const LocPosMode &locPosMode, PBLocPosMode* pbLocPosMode);
    int convertEHubRoutingTableEntryToPB(const EHubRoutingTableEntry &eHRoutTblEntry,
            EHRoutingTableEntry* pbEHRoutTblEntry);
    int convertEHubSubModifiersToPB(const eHubSubModifiers &eHSubModifiers,
            EHSubModifiers* pbEHSubModifiers);
    int convertEngineConnectionInfoToPB(const engineConnectionInfo &engConnInfo,
            EngineConnectionType engConnType, EHEngineConnectionInfo *pbEhEngConnInfo);
    int convertEHSubscriptionModifierToPB(const eHubSubscriptionModifier &ehSubscModifier,
            EHSubscriptionModifier *pbEhSubscModifier);
    int convertLeverArmConfigInfoToPB(const LeverArmConfigInfo &leverArmConfgInfo,
            PBLeverArmConfigInfo *pbLeverArmConfgInfo);
    int convertLeverArmParamsToPB(const LeverArmParams &leverArmParams,
            PBLeverArmParams *pbLeverArmParams);
    int convertB2sMountParamsToPB(const BodyToSensorMountParams &b2sMountParams,
            PBBodyToSensorMountParams *pbB2sMountParams);
    int convertGnssLocDgnssSVMeasurementToPB(const Gnss_LocDgnssSVMeasurement &locDgnssSvMeas,
            PBGnssLocDgnssSVMeasurement *pbLocDgnssSvMeas);
#ifdef FEATURE_CDFW
    // QDGnss structs conversion to protobuf
    int convertStationStatusIndicatorMaskToPB(const StationStatusIndicatorMask &statStatusIndiMask,
            PBStationStatusIndicatorMask *pbStatStatusIndiMask);
    int convertReferenceStationToPB(const ReferenceStation &refStation,
            PBReferenceStation *pbRefStation);
    int convertGnssEpochTimeToPB(const DGnssConstellationIdentifier &gnssConstId,
            const GnssEpochTime &gnssEpoch, PBGnssEpochTime *pbGnssEpoch);
    int convertDGnssObservationStatusFlagsToPB(const DGnssObservationStatusFlags &dGnssObsStatFlags,
            PBDGnssObservationStatusFlags *pbDGnssObsStatFlags);
    int convertDGnssMeasurementToPB(const DGnssConstellationIdentifier &gnssConstId,
            const DGnssMeasurement &dGnssMeas,
            PBDGnssMeasurement *pbDGnssMeas);
    int convertDGnssObservationToPB(const DGnssObservation &dGnssObs,
            PBDGnssObservation *pbDGnssObs);
#endif

    // helper function for mask conversion to protobuff masks
    // Gnss_LocSvInfoMaskT to PBSvInfoMask
    uint32_t getPBMaskForGnssLocSvInfoMaskT(const uint32_t &gnssSvInfoMask);
    // Gnss_LocSvMeasStatusMaskType to PBGnssLocSvMeasStatusMaskType
    uint64_t getPBMaskForGnssLocSvMeasStatus(const uint64_t &measStatusMask);
    // GnssAdditionalSystemInfoMask to PBGnssAdditionalSystemInfoMask
    uint32_t getPBMaskForGnssAdditionalSystemInfoMask(const uint32_t &gnssAddnlSysInfoMask);
    // GnssSvOptionsMask to PBGnssSvOptionsMask
    uint32_t getPBMaskForGnssSvOptionsMask(const uint16_t gnssSvOptionsMask);
    // GnssSignalTypeMask To PBGnssSignalInfoMask
    uint32_t getPBMaskForGnssSignalTypeMask(const uint32_t &gnssSignalTypeMask);
    // GnssSvPolyStatusMaskValidity To PBGnssSvPolyStatusMaskValidity
    uint32_t getPBMaskForGnssSvPolyStatusMaskValidity(const uint32_t &gnssSvPolyStatusMaskValidity);
    // GnssSvPolyStatusMask To PBGnssSvPolyStatusMask
    uint32_t getPBMaskForGnssSvPolyStatusMask(const uint32_t &gnssSvPolyStatusMask);
    // UlpGnssSvPolyValidFlags to PBUlpGnssSvPolyValidFlags
    uint32_t getPBMaskForUlpGnssSvPolyValidFlags(const uint32_t &ulpGnssSvPolyValidFlags);
    // LocPosTechMask to PBLocPosTechMask
    uint32_t getPBMaskForLocPosTechMask(const uint32_t &locPosTechMask);
    // LocGpsSpoofMask to PBLocGpsSpoofMask
    uint32_t getPBMaskForLocGpsSpoofMask(const uint32_t &locGpsSpoofMask);
    // LocGpsLocationFlags to PBLocGpsLocationFlags
    uint32_t getPBMaskForLocGpsLocationFlags(const uint32_t &locGpsLocFlags);
    // LocNavSolutionMask to PBLocNavSolutionMask
    uint32_t getPBMaskForLocNavSolutionMask(const uint32_t &locNavSolnMask);
    // DrCalibrationStatusMask to PBDrCalibrationStatusMask
    uint32_t getPBMaskForDrCalibrationStatusMask(const uint32_t &drCalibStatMask);
    // DrSolutionStatusMask to PBSolutionStatusMask
    uint32_t getPBMaskForSolutionStatusMask(const uint32_t &solStatusMask);
    // PositioningEngineMask to PBPositioningEngineMask
    uint32_t getPBMaskForPositioningEngineMask(const uint32_t &posEngMask);
    // GnssMeasUsageInfoValidityMask to PBGnssMeasUsageInfoValidityMask
    uint32_t getPBMaskForGnssMeasUsageInfoValidityMask(const uint32_t &gnssMeasUsageInfoValidMask);
    // GnssMeasUsageStatusBitMask to PBGnssMeasUsageStatusBitMask
    uint32_t getPBMaskForGnssMeasUsageStatusBitMask(const uint32_t &gnssMeasUsageStatusBitMask);
    // GnssLocationPosDataMask to PBGnssLocationPosDataMask
    uint32_t getPBMaskForGnssLocationPosDataMask(const uint32_t &gnssLocPosDataMask,
        const uint32_t &gnssLocPosDataMaskExt);
    // GpsLocationExtendedFlags to PBGpsLocationExtendedFlagsLower32/Upper32
    // See comment declaration of PBGpsLocationExtendedFlagsLower32 in the file
    // LocationDataTypes.proto.
    uint64_t getPBMaskForGpsLocationExtendedFlags(const uint64_t &gpsLocExtFlags);
    // GpsSvMeasHeaderFlags to PBGpsSvMeasHeaderFlags
    uint64_t getPBMaskForGpsSvMeasHeaderFlags(const uint64_t &gpsSvMeasHdrFlags);
    // Gnss_LocInterSystemBiasValidMaskType to PBLocInterSystemBiasValidMaskType
    uint32_t getPBMaskForGnssLocInterSystemBiasValidMaskType(
            const uint32_t &gnssLocInterSysBiasValid);
    // GnssAidingDataSvMask to PBGnssAidingDataSvMask
    uint32_t getPBMaskForGnssAidingDataSvMask(const uint32_t &gnssAidDataSvMask);
    // GnssAidingDataSvTypeMask to PBGnssAidingDataSvTypeMask
    uint32_t getPBMaskForGnssAidingDataSvTypeMask(const uint32_t &gnssAidDataSvTypeMask);
    // GnssAidingDataCommonMask to PBGnssAidingDataCommonMask
    uint32_t getPBMaskForGnssAidingDataCommonMask(const uint32_t &gnssAidDataCommonMask);
    // eHubRegReportMask to EHRegReportMask
    uint32_t getPBMaskForeHubRegReportMask(const uint32_t &ehRegReportMask);
    // eHubRegStatusMask to EHRegStatusMask
    uint32_t getPBMaskForeHubRegStatusMask(const uint32_t &ehRegStatusMask);
    // eHubRegReportCorrectionMask to EHRegReportCorrectionMask
    uint32_t getPBMaskForeHubRegReportCorrectionMask(const uint32_t &ehRegReportCrrnMask);
    // GnssConstellationTypeMask to PBGnssConstellationInfoMask
    uint32_t getPBMaskForGnssConstellationTypeMask(const uint32_t &gnssConstlMask);
    // GnssSystemTimeStructTypeFlags to PBGnssSystemTimeStructTypeFlags
    uint32_t getPBMaskForGnssSystemTimeStructTypeFlags(const uint32_t &gnssSysTimeStruct);
    // GnssGloTimeStructTypeFlags to PBGnssGloTimeInfoBits
    uint32_t getPBMaskForGnssGloTimeStructTypeFlags(const uint32_t &gloSysTimeStruct);
    // LeverArmTypeMask to PBLeverArmTypeMask
    uint32_t getPBMaskForLeverArmTypeMask(const uint32_t &leverArmTypeMask);
    // LocSvDgnssMeasStatusMask to PBLocSvDgnssMeasStatusMask
    uint32_t getPBMaskForLocSvDgnssMeasStatusMask(const uint32_t &svDgnssMeasStatMask);
#ifdef FEATURE_CDFW
    // QDGnss  mask conversion to protobuff masks
    // DGnssSignalStatusFlags to PBDGnssSignalStatusFlags
    uint32_t getPBMaskForDGnssSignalStatusFlags(const uint32_t &dGnssSignStatFlags);
    // DGnssConstellationBitMask to PBDGnssConstellationBitMask
    uint32_t getPBMaskForDGnssConstellationBitMask(const uint32_t &dGnssConstMask);
#endif

    // helper function for enum conversion to protobuff enums
    PBSvSearchStatus getPBEnumForSvSearchStatus(
            const Gnss_LocSvSearchStatusEnumT &gnssSearchStatus);
    PBGnss_LocSvSystemEnumType getPBEnumForGnssLocSvSystem(
            const Gnss_LocSvSystemEnumType &svMeasSet);
    PBLocSourceofFreqEnumType getPBEnumForGnssLocSourceofFreq(
            const Gnss_LocSourceofFreqEnumType &sourceOfFreq);
    PBGnssEphAction getPBEnumForGnssEphAction(const GnssEphAction &gnssEphAction);
    PBGalEphSignalSource getPBEnumForGalEphSignalSource(const GalEphSignalSource &galEphSigSrc);
    PBGnssSvType getPBEnumForGnssSvType(const GnssSvType &gnssSvType);
    PBLocSessionStatus getPBEnumForLocSessStatus(const loc_sess_status &locSessStatus);
    PBLocReliability getPBEnumForLocReliability(const LocReliability &locReliability);
    PBLocOutputEngineType getPBEnumForLocOutputEngineType(
            const LocOutputEngineType &locOutputEngType);
    PBLocSvInfoSource getPBEnumForLocSvInfoSource(const LocSvInfoSource &locSvSrcInfo);
    PBCarrierPhaseAmbiguityType getPBEnumForCarrierPhaseAmbiguityType(
            const CarrierPhaseAmbiguityType &carrierPhaseAmbigType);
    EHTechId getPBEnumForEHubTechId(const EHubTechId &engHubTechId);
    PBLocPositionMode getPBEnumForLocPositionMode(const LocPositionMode &locPosMode);
    PBLocGpsPositionRecurrence getPBEnumForLocGpsPositionRecurrence(
            const LocGpsPositionRecurrence &locGpsRecur);
    PBGnssPowerMode getPBEnumForGnssPowerMode(const GnssPowerMode &gnssPwrMode);
    // EngineConnectionType to EHConnectionType
    EHConnectionType getPBEnumForEngineConnectionType(const EngineConnectionType &engConnType);
    PBLocDgnssCorrectionSourceType getPBEnumForLocDgnssCorrectionSourceType(
            const LocDgnssCorrectionSourceType &locDgnssCorrnSrcType);
#ifdef FEATURE_CDFW
    // QDGnss  enum conversion to protobuff enums
    // #define QDGNSS_* in  QDgnssDataType.h
    PBQDgnssDataType getPBEnumForQDgnssDataType(const uint32_t &qDgnssDataType);
    PBExtendedSatelliteInformation getPBEnumForExtendedSatelliteInformation(
            const ExtendedSatelliteInformation &extSatInfo);
    PBDGnssSbasSignalIdentifier getPBEnumForDGnssSbasSignalIdentifier(
            const DGnssSbasSignalIdentifier &dGnssSbasSignId);
    PBDGnssBdsSignalIdentifier getPBEnumForDGnssBdsSignalIdentifier(
            const DGnssBdsSignalIdentifier &dGnssBdsSignId);
    PBDGnssQzssSignalIdentifier getPBEnumForDGnssQzssSignalIdentifier(
            const DGnssQzssSignalIdentifier &dGnssQzssSignId);
    PBDGnssGalileoSignalIdentifier getPBEnumForDGnssGalileoSignalIdentifier(
            const DGnssGalileoSignalIdentifier &dGnssGalSignId);
    PBDGnssGlonassSignalIdentifier getPBEnumForDGnssGlonassSignalIdentifier(
            const DGnssGlonassSignalIdentifier &dGnssGloSignId);
    PBDGnssGpsSignalIdentifier getPBEnumForDGnssGpsSignalIdentifier(
            const DGnssGpsSignalIdentifier &dGnssGpsSignId);
    PBDGnssConstellationIdentifier getPBEnumForDGnssConstellationIdentifier(
            const DGnssConstellationIdentifier &dGnssConstSignId);
#endif

    // PROTOBUF TO LOCAL STRUCT
    // ************************
    // helper function for mask conversion from protobuff masks to normal rigid values
    // EHRegReportMask to  eHubRegReportMask
    uint32_t getEhubRegReportMaskFromPBEHRegReportMask(const uint32_t &ehRegReportMask);
    // EHRegStatusMask to eHubRegStatusMask
    uint32_t getEhubRegStatusMaskFromPBEHRegStatusMask(const uint32_t &ehRegStatusMask);
    // EHRegReportCorrectionMask to eHubRegReportCorrectionMask
    uint32_t getEhubRegCorrectionMaskFromPBEHRegReportCorrectionMask(
            const uint32_t &ehRegReportCrrnMask);
    // PBGnssSignalInfoMask to GnssSignalTypeMask
    uint32_t getGnssSignalTypeMaskFromPBGnssSignalInfoMask(const uint32_t &pbGnssSignalInfoMask);
    // PBGnssConstellationInfoMask to GnssConstellationTypeMask
    uint32_t getGnssConstellationTypeMaskFromPBGnssConstellationInfoMask(
            const uint32_t &pbGnssConstellationInfoMask);
    // PBLocPosTechMask to LocPosTechMask
    uint32_t getLocPosTechMaskFromPBLocPosTechMask(const uint32_t &pbLocPosTechMask);
    // PBLocGpsSpoofMask to LocGpsSpoofMask
    uint32_t getLocGpsSpoofMaskFromPBLocGpsSpoofMask(const uint32_t &pbLocGpsSpoofMask);
    // PBLocGpsLocationFlags to LocGpsLocationFlags
    uint32_t getLocGpsLocationFlagsFromPBLocGpsLocationFlags(const uint32_t &pbLocGpsLocFlags);
    // PBGnssSvOptionsMask to GnssSvOptionsMask
    uint32_t getGnssSvOptionsMaskFromPBGnssSvOptionsMask(const uint16_t &pbGnssSvOptionsMask);
    // PBGnssSignalInfoMask to GnssSignalTypeMask
    uint32_t getGnssSignalTypeMaskFromPBGnssSignalTypeMask(const uint32_t &pbGnssSignalTypeMask);
    // PBLocNavSolutionMask to LocNavSolutionMask
    uint32_t getLocNavSolutionMaskFromPBLocNavSolutionMask(const uint32_t &pbLocNavSolnMask);
    // PBDrCalibrationStatusMask to DrCalibrationStatusMask
    uint32_t getDrCalibrationStatusMaskFromPBDrCalibrationStatusMask(
            const uint32_t &pbDrCalibStatMask);
    // PBPositioningEngineMask to PositioningEngineMask
    uint32_t getPositioningEngineMaskFromPBPositioningEngineMask(const uint32_t &pbPosEngMask);
    // PBGnssMeasUsageInfoValidityMask to GnssMeasUsageInfoValidityMask
    uint32_t getGnssMeasUsageInfoValidityMaskFromPB(const uint32_t &pbGnssMeasUsageInfoValidMask);
    // PBGnssMeasUsageStatusBitMask to GnssMeasUsageStatusBitMask
    uint32_t getGnssMeasUsageStatusBitMaskFromPB(const uint32_t &pbGnssMeasUsageStatusBitMask);
    // PBGnssSystemTimeStructTypeFlags to GnssSystemTimeStructTypeFlags
    uint32_t getGnssSystemTimeStructTypeFlagsFromPB(const uint32_t &pbGnssSysTimeStruct);
    // PBGnssGloTimeInfoBits to GnssGloTimeStructTypeFlags
    uint32_t getGnssGloTimeStructTypeFlagsFromPB(const uint32_t &pbGloSysTimeStruct);
    // PBGnssLocationPosDataMask to GnssLocationPosDataMask
    uint32_t getGnssLocationPosDataMaskFromPB(const uint32_t &pbGnssLocPosDataMask);
    // PBSolutionStatusMask to DrSolutionStatusMask
    uint32_t getDrSolutionStatusMaskFromPBSolutionStatusMask(
            const uint32_t &pbSolStatusMask);

    // helper function for enum conversion from protobuff enums to normal format.
    EngineConnectionType getEnumForPBEHConnectionType(const EHConnectionType &engConnectionType);
    EHubTechId getEnumForPBEHTechId(const EHTechId &pbEhTechId);
    loc_sess_status getEnumForPBLocSessionStatus(const PBLocSessionStatus &pbLocSessStatus);
    GnssSvType getEnumForPBGnssSvType(const PBGnssSvType &pbGnssSvType);
    LocReliability getEnumForPBLocReliability(const PBLocReliability &pbLocReliability);
    LocSvInfoSource getEnumForPBLocSvInfoSource(const PBLocSvInfoSource &pbLocSvSrcInfo);
    LocOutputEngineType getEnumForPBLocOutputEngineType(
            const PBLocOutputEngineType &pbLocOutputEngType);
    CarrierPhaseAmbiguityType getEnumForPBCarrierPhaseAmbiguityType(
            const PBCarrierPhaseAmbiguityType &pbCarrierPhaseAmbigType);
    Gnss_LocSvSystemEnumType getEnumForPBGnss_LocSvSystemEnumType(
            const PBGnss_LocSvSystemEnumType &pbGnssLocSvSystemEnum);
    LocDgnssCorrectionSourceType getEnumForPBLocDgnssCorrectionSourceType(
            const PBLocDgnssCorrectionSourceType &pbLocDgnssCorrnSrcType);

    // convert from protobuff struct to normal struct.
    // PBGnssSvNotification to GnssSvNotification
    int pbConvertToGnssSvNotification(const PBGnssSvNotification &pbGnssSvNotif,
            GnssSvNotification &gnssSvNotif);
    // PBUlpLocation to UlpLocation
    int pbConvertToUlpLocation(const PBUlpLocation &pbUlpLoc, UlpLocation &ulpLoc);
    // PBGpsLocationExtended to GpsLocationExtended
    int pbConvertToGpsLocationExtended(const PBGpsLocationExtended &pbGpsLocExt,
            GpsLocationExtended &gpsLocExt);
    // PBLocGpsLocation to LocGpsLocation
    int pbConvertToLocGpsLocation(const PBLocGpsLocation &pbLocGpsLoc, LocGpsLocation &locGpsLoc);
    // PBGnssSystemTime to GnssSystemTime
    int pbConvertToGnssSystemTime(const PBGnssSystemTime &pbGnssSysTime,
            GnssSystemTime &gnssSysTime);
    // PBGnssSystemTimeStructType to GnssSystemTimeStructType
    int pbConvertToGnssSystemTimeStructType(const PBGnssSystemTimeStructType &pbGnssSysTimeInfo,
            GnssSystemTimeStructType &gnssSystemTime);
    // PBGnssGloTimeStructType to GnssGloTimeStructType
    int pbConvertToGnssGloTimeStructType(const PBGnssGloTimeStructType &pBGnssGloTimeInfo,
            GnssGloTimeStructType &gloSystemTime);

    // EHSubModifiers to eHubSubModifiers
    eHubSubModifiers pbConvertToEHubSubModifiers(const EHSubModifiers &pbEHSubModifiers);
    // EHSubscriptionModifier to eHubSubscriptionModifier
    eHubSubscriptionModifier pbConvertToEHSubscriptionModifier(
            const EHSubscriptionModifier &pbEHSubscriptionModifier);

};

// Entry point to the library
typedef void* (getEngHubMsgConvIfaceFunc)();

struct EngHubMsgConvInterface {
    size_t size;
    int (*pbPopulateEHMsgWithCounterToSend)(const string &ehMsgPayLoad, EHMsgId msgId,
            uint32_t msgCounter, string &os);
    int (*pbPopulateEHMsgReportSvMeas)(const GnssSvMeasurementSet &svMeasSet,
            uint64_t ptpTimeNs, string &os);
    int (*pbPopulateEHMsgReportSv)(const GnssSvNotification &gnssSvNotif, string &os);
    int (*pbPopulateEHMsgSvPolynomial)(const GnssSvPolynomial &svPoly, string &os);
    int (*pbPopulateEHMsgSvEphemeris)(const GnssSvEphemerisReport &gnssEph, string &os);
    int (*pbPopulateEHMsgReportPosition)(const UlpLocation &location,
            const GpsLocationExtended &locationExtended, enum loc_sess_status status,
            string &os);
    int (*pbPopulateEHMsgKlobucharIonoModel)(const GnssKlobucharIonoModel &klobIonoMdl,
            string &os);
    int (*pbPopulateEHMsgGlonassAddnParams)(const GnssAdditionalSystemInfo &gnssAddSysInfo,
            string &os);
    int (*pbPopulateEHMessageSessionStart)(string &pbStr);
    int (*pbPopulateEHMessageSessionStop)(string &pbStr);
    int (*pbPopulateEHMessageProtocolEngineHubUp)(string &pbStr);
    int (*pbPopulateEHMessageStatusConnectivity)(string &pbStr, bool connected);
    int (*pbPopulateEHMessageStatusLeverArmConfig)(const LeverArmConfigInfo &leverArmConfgInfo,
            string &os);
    int (*pbPopulateEHMessageStatusB2sMountParams)(const BodyToSensorMountParams &b2sParams,
            string &os);
    int (*pbPopulateEHMessageSessionDeleteAidingData)(const GnssAidingData &gnssAidData,
            string &os);
    int (*pbPopulateEHMessageSessionSetFixMode)(const LocPosMode &locPosMode, string &os);
    int (*pbPopulateEHMessageProtocolRoutingTableUpdate)(
            const EHubMessageProtocolRoutingTableUpdate &eHubMsgPrtclRoutngTblUpdt, string &os);
#ifdef FEATURE_CDFW
    int (*pbPopulateEHMessageQDgnssReferenceStation)(const ReferenceStation &refStation,
            string &os);
    int (*pbPopulateEHMessageQDgnssObservation)(const DGnssObservation &dGnssObs, string &os);
#endif
    bool (*pbPopulateEHMsgReportSvMeasBasedOnSubModifiers)(
            const EHubMessageReportSvMeas* inMsg, GnssConstellationTypeMask regConstellationMask,
            GnssSignalTypeMask regSignalTypeMask, uint8_t * &outMsg, size_t &outMsgSize,
            uint32_t msgCounter);
    bool (*pbPopulateEHMsgSvEphemerisBasedOnSubModifiers)(
            const EHubMessageReportSvEphemeris* inMsg, GnssSignalTypeMask gnssSignalMask,
            uint8_t * &outMsg, size_t &outMsgSize, uint32_t msgCounter);
    int (*pbPopulateEHMessageQWESFeatureStatusInfo)(const ehFeatureStatus &status,
            const ehLicenseType &type, string &os);
    int (*pbPopulateEHMessageQWESInstallLicenseResp)(const ehInstallLicenseStatus & status,
            string &os);
    int (*pbPopulateEHMessageQWESProvideAttestationStatement)(const uint8_t *outBuff,
            uint32_t bufLen, const ehAttestationStatus &status, string &os);
    int (*pbPopulateEHMessageQWESForceSyncNotify)(int32_t status, string &os);
    // Decode routines
    bool (*decodeEHubWthMsgCounterPayLoadMsg)(const string& eHubWithCtrPayLoadMsg,
            string& engHubMsgPayload, uint32_t &msgId, uint32_t &msgCounter, string &senderName);
    EHubMessageProtocolRegMsg* (*decodeEHRoutingTableEntry)(const string& pbEhRoutingTablePayload);
    EHubMessageReportPosition* (*decodeEHMessageReportPosition)(
            const string& pbEhMsgRprtPosPayload);
    EHubMessageReportSv* (*decodeEHMessageReportSv)(const string& pbEhMsgRprtSvPayload);
    EHubMessageQWESInstallLicenseReq* (*decodeEHMessageInstallLicenseReq)(
        const string& pbEhMsgInstallLicensePayload);
    EHubMessageQWESReqAttestationStatement* (*decodeEHMessageAttestationReq)(
        const string& pbEhMsgAttestationPayload);
    // Util functions
};

static inline EngHubMsgConvInterface* loadEngHubMsgConvIface() {
    static EngHubMsgConvInterface *ehubMsgConvIface = nullptr;

    if (nullptr == ehubMsgConvIface) {
        void* libHandle = nullptr;
        getEngHubMsgConvIfaceFunc* getter = (getEngHubMsgConvIfaceFunc*)dlGetSymFromLib(
                libHandle, "libeng_hub_msg_proto.so", "getEngHubMsgConvIface");
        if (nullptr != getter) {
            ehubMsgConvIface = (EngHubMsgConvInterface*)(*getter)();
        }
    }
    return ehubMsgConvIface;
}

#endif // #ifndef ENGINE_HUB_MSG_CONV_H
