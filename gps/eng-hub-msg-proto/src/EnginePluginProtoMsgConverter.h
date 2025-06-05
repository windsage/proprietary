/******************************************************************************
    Copyright (c) 2018, 2020-2023 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef EP_PROTO_MSG_CONVERTER_H
#define EP_PROTO_MSG_CONVERTER_H

#include <stdio.h>
#include <string>
#include <map>
#include "gps_extended_c.h"
#include "EngineHubMsg.h"
#include "EnginePluginAPI.h"
#include "EnginePluginImpl.h"
#include "LocationDataTypes.pb.h"
#include "EHMsg.pb.h"
#include <loc_misc_utils.h>

using namespace std;

namespace engine_plugin {
class EpProtoMsgConverter {
private:
    void pbPopulateGpsEphemeris
    (
        const PBGpsEphemerisResponse &gpsEphemeris,
        epGnssEphemerisReport &ePEphRpt
    );
    void pbPopulateGlonassEphemeris
    (
        const PBGlonassEphemerisResponse &gloEphemeris,
        epGnssEphemerisReport &ePEphRpt
    );
    void pbPopulateBdsEphemeris
    (
        const PBBdsEphemerisResponse &bdsEphemeris,
        epGnssEphemerisReport &ePEphRpt
    );
    void pbPopulateQzssEphemeris
    (
        const PBQzssEphemerisResponse &qzssEphemeris,
        epGnssEphemerisReport &ePEphRpt
    );
    void pbPopulateGalEphemeris
    (
        const PBGalileoEphemerisResponse &galEphemeris,
        epGnssEphemerisReport &ePEphRpt
    );
    void pbFillEpGnssSvPolyTgd
    (
        const EHMessageReportSvPolynomial *pEHubSvPolyIn,
        epGnssSvPolynomial &epSvPolyOut
    );
    eHubSubModifiers pbConvertToEHubSubModifiers
    (
        const EHSubModifiers &pbEHSubModifiers
    );
    eHubSubscriptionModifier pbConvertToEHSubscriptionModifier
    (
        const EHSubscriptionModifier &pbEHSubscriptionModifier
    );
#ifdef FEATURE_CDFW
    void fillQdgnssStatusIndicator
    (
        const PBStationStatusIndicatorMask pbStatusindicationMask,
        StationStatusIndicatorMask  &statusIndicator
    );
    void fillQdgnssConstellationBitMask
    (
        const uint32_t constellationBitMask,
        DGnssConstellationBitMask &qdGnssConstellationBitMask
    );
    void fillQDGnssConstellationIdentifier
    (
        const PBDGnssConstellationIdentifier &pbConstellationIdentifier,
        DGnssConstellationIdentifier &qdgnssConstellationIdentifier
    );
    void fillQDGnssEpochTime
    (
        const PBGnssEpochTime &pbEpochTime,
        const PBDGnssConstellationIdentifier &pbConstellationIdentifier,
        GnssEpochTime &qdgnssEpochTime
    );
    void fillQDGnssObservationStatusFlags
    (
        const PBDGnssObservationStatusFlags &pbStatusFlags,
        DGnssObservationStatusFlags &qdgnssStatusFlags
    );
    void fillQDgnssGpsSignalIdentifier
    (
        const PBDGnssGpsSignalIdentifier &pbGpsSignalIdentifier,
        DGnssGpsSignalIdentifier &qdgnssGpsSignalIdentifier
    );
    void fillQDgnssSbasSignalIdentifier
    (
        const PBDGnssSbasSignalIdentifier &pbSbasSignalIdentifier,
        DGnssSbasSignalIdentifier &qdgnssSbasSignalIdentifier
    );
    void fillQDgnssQzssSignalIdentifier
    (
        const PBDGnssQzssSignalIdentifier &pbQzssSignalIdentifier,
        DGnssQzssSignalIdentifier &qdgnssQzssSignalIdentifier
    );
    void fillQDgnssGalileoSignalIdentifier
    (
        const PBDGnssGalileoSignalIdentifier &pbGalileoSignalIdentifier,
        DGnssGalileoSignalIdentifier &qdgnssGalieloSignalIdentifier
    );
    void fillQDgnssGlonassSignalIdentifier
    (
        const PBDGnssGlonassSignalIdentifier &pbGlonassSignalIdentifier,
        DGnssGlonassSignalIdentifier &qdgnssGlonassSignalIdentifier
    );
    void fillQDgnssBdsSignalIdentifier
    (
        const PBDGnssBdsSignalIdentifier &pbBdsSignalIdentifier,
        DGnssBdsSignalIdentifier &qdgnssBdsSignalIdentifier
    );
    void fillQDGnssSignalObservationStatus
    (
        const uint32_t &pbSignalObservationStatus,
        DGnssSignalStatusFlags &qdgnssSignalObservationStatus
    );
    void fillQDGnssMeasurementInfo
    (
        const PBDGnssMeasurement &pbDGnssMeasurement,
        const PBDGnssConstellationIdentifier &gnssConstId,
        DGnssMeasurement &qDGnssMeasurement
    );
    void fillQdgnssExtendedSatelliteInformation
    (
        const PBExtendedSatelliteInformation &pbExtendedSatelliteInformation,
        ExtendedSatelliteInformation &qdgnssExtendedSatelliteInformation
    );
#endif //FEATURE_CDFW
    void pbFillEpLeverArmParams
    (
        const PBLeverArmParams &eHubLeverArmParams,
        epLeverArmParams &ePLeverArmParams
    );
    uint32_t pbGetGnssSubMask
    (
        const epReportSubscriptionMask &epSubInfoFlags
    );
    void pbFillSubscriptionMask
    (
        const epSubscriptionInfo &epSubInfo,
        EHSubModifiers *subMods
    );
    uint32_t pbGetEHubGnssSignalTypeMask
    (
       epGnssSignalTypeMask gnssSignalType
    );

public:
    // Parse data from received protobuf serialised string.
    bool ParseFromString(::google::protobuf::MessageLite& ePMsg, const string& data);
    bool ParseFromString(::google::protobuf::MessageLite* ePMsg,
            const EngineHubMessage& ePPayLoadMsg);
    bool ParseFromString(::google::protobuf::MessageLite& ePMsg, uint32_t &msgCounter,
            string &senderName, const EngineHubMessageWithMsgCounter& ePPayLoadMsg);
    uint32_t getMsgIdFromEHubMessage(const EngineHubMessage& ePMsg);

    bool pbRegisterWithEHub
    (
        const EHubTechId &techID,
        const epFeatureStatus &featureStatus,
        const epEngineProcessorType &engineProcessorType,
        const uint32_t &sendMsgCounter,
        const epSubscriptionInfo &epSubInfo,
        const string &sockName,
        const char *ePServerIpAddr,
        const char *ePServerPort,
        int featureId,
        string &protoStr
    );
    bool pbEpImplPvtReportConvertProto
    (
        const epImplPVTReport * pEpImplPvtReport,
        const Gnss_ApTimeStampStructType &bootTimeStamp,
        const string &epUniqueName,
        uint32_t sendMsgCounter,
        string &os
    );
    bool pbFillepGnssSvNotification
    (
        const EHMessageReportSv *svRpt,
        epGnssSvNotification &ePGnssSv,
        EHTechId &sourceTechId
    );
    bool pbFillepGnssSvPolynomial
    (
        const EHMessageReportSvPolynomial *pEhubSvPoly,
        epImplGnssSvPolynomial &epSvPoly,
        EHTechId &sourceTechId
    );
    bool pbFillepDeadReckoningEngineConfig
    (
        const EHMessageStatusB2sMountParams *pEHubB2sInfo,
        epDeadReckoningEngineConfig &epDREConfParams
    );
    bool pbFillepGloAdditionalParameter
    (
        const EHMessageReportGlonassAdditionalParams *pEHubAddParamsRpt,
        epGloAdditionalParameter &ePAdditionalData,
        EHTechId &sourceTechId
    );
    bool pbFillepKlobucharIonoModel
    (
        const EHMessageReportKlobucharIonoModel *pEHubIonoRpt,
        epKlobucharIonoModel &ePIonoRpt,
        EHTechId &sourceTechId
    );
    bool pbFillepGnssEphemerisReport
    (
        const EHMessageReportSvEphemeris *pEHubEphRpt,
        epGnssEphemerisReport &ePEphRpt,
        EHTechId &sourceTechId
    );
    bool pbFillepImplPVTReport
    (
        const EHMessageReportPosition *pEHubPosRpt,
        epImplPVTReport &ePImplPVTRpt,
        EHTechId &sourceTechId
    );
    bool pbFillepSetPositionModeCmdReq
    (
        const EHMessageSessionSetFixMode *pEhubFixMode,
        epCommandRequest &epCmd
    );
    bool pbFillepDeleteAidingDataCmdReq
    (
        const EHMessageSessionDeleteAidingData *pEhubDeleteData,
        epCommandRequest &epCmd
    );
    void getNhzInfoFromSvMeas
    (
        const EHMessageReportSvMeas *pEHubMsrRpt,
        EHTechId &sourceTechId,
        bool &isNhz
    );
    void getClkTickCountInfoFromSvMeas
    (
        const EHMessageReportSvMeas *pEHubMsrRpt,
        uint64_t &headerFlags,
        uint64_t &refCountTicks,
        uint64_t &ptpTimens
    );
    uint32_t getPosEngineMaskFromDeleteAidingData
    (
        const EHMessageSessionDeleteAidingData *pEhubDeleteData
    );
    bool getConnectionStatusFromEHMsgStatusConnectivity
    (
        const EHMessageStatusConnectivity &pBConStatus
    );
    void getInstallLicenseRespFromEHMsgQWESInstallLicenseResp
    (
        const EHMessageQWESInstallLicenseResp &pBQwesResp,
        ehInstallLicenseStatus &status
    );
    void getFeatureStatusFromEHMsgQWESFeatureStatusInfo
    (
        const EHMessageQWESFeatureStatusInfo &pBQwesStatus,
        ehFeatureStatus &featureStatus,
        ehLicenseType &licenseType
    );
    void getAttestationStatementFromEHMsgQwesAttestationStatement
    (
        const EHMessageQWESProvideAttestationStatement &pBAttestation,
        std::string &outBuf,
        uint32_t &bufLen,
        ehAttestationStatus &status
    );
    int32_t getQwesSyncStatusFromEHMsgQWESForceSyncNotify
    (
        const EHMessageQWESForceSyncNotify &pBNotify
    );
    void getDestinationSocketNameEHRoutingTblEntry
    (
        const EHRoutingTableEntry &routingTableEntry,
        EHubRoutingTableEntry &eHubRoutingTable
    );
    uint32_t getEhubMsgRoutingTblNumberOfEntries
    (
        const EHMessageProtocolRoutingTableUpdate *ptable
    );
    bool pbFillReportPositionEngineHubMsgWithCntr
    (
        const EHMessageReportPosition &posRpt,
        const string &epUniqueName,
        EngineHubMessageWithMsgCounter &ehMsgWithCtr
    );
    bool pbEhMsgWithCtrSerializeToString
    (
        EngineHubMessageWithMsgCounter &ehMsgWithCtr,
        uint32_t sendMsgCounter,
        string &os
    );
    bool checkIfSendReportToRoutingTblEntry
    (
        const EHMessageProtocolRoutingTableUpdate pbRoutingTable,
        const EHubTechId &techID,
        unsigned int index,
        EHRoutingTableEntry &eHubRoutingTable
    );
    void pbFillEpPVTReport
    (
        const EHMessageReportPosition *pEHubPosRptIn,
        epImplPVTReport &ePImplPVTRpt
    );
    bool pbFillEpGnssMeasReport
    (
        const EHMessageReportSvMeas *pEHubMsrRptIn,
        epGnssImplMeasurementReport &ePImplMsrRptOut,
        uint64_t &measurementStatus
    );
    void pbFillEpGnssSvPolynomial
    (
        const EHMessageReportSvPolynomial *pEHubSvPolyIn,
        epImplGnssSvPolynomial &epSvPolyOut
    );
    void pbFillEpSvReport
    (
        const EHMessageReportSv *pEhubSvRpt,
        epGnssSvNotification &ePSvReport
    );
    void pbFillEHubPosFlags
    (
        epPositionFlags epPosFlags,
        PBLocPosTechMask &techMask
    );
    void pbFillEHubPVTReport
    (
        const epImplPVTReport* pEPImplPVTRptIn,
        const Gnss_ApTimeStampStructType& apTimestamp,
        PBUlpLocation *pbLocation,
        PBGpsLocationExtended *pbLocationExtended
    );
    epGnssConstellationTypeMask pbGetEPGnssConstellationType
    (
        PBGnss_LocSvSystemEnumType system
    );
    epGnssConstellationTypeMask pbGetEPGnssConstellationType
    (
        uint32_t system
    );
    uint32_t pbGetEHGnssConstellationType
    (
        epGnssConstellationTypeMask eptype
    );
    PBGnss_LocSvSystemEnumType pbGetEHubGnssConstellationType
    (
       epGnssConstellationTypeMask  system
    );
    uint32_t pbGetMeasUsageStatusMask
    (
       epGnssMeasUsageStatusBitMask epMeasUsageStatusMask
    );
    void pbFillEpGloSystemTimeInfo
    (
        const PBGnssGloTimeStructType &gloTime,
        epGnssGloTimeStructType &gloSystemTime
    );
    void pbFillEHubGnssSystemTimeInfo
    (
        const epGnssSystemTimeStructType* pSystemTime,
        PBGnssSystemTimeStructType *gnssTime
    );
    void pbFillEpGnssSystemTimeInfo
    (
        const PBGnssSystemTimeStructType &gnssTime,
        epGnssSystemTimeStructType &epSystemTime
    );
    void pbFillEHubGloSystemTimeInfo
    (
        const epGnssGloTimeStructType* pSystemTime,
        PBGnssGloTimeStructType *gloTime
    );
    void pbFillEpGnssEphemeris
    (
        const EHMessageReportSvEphemeris *pEHubEphRpt,
        epGnssEphemerisReport &ePEphRpt
    );
    void pbFillEpInonData
    (
        const EHMessageReportKlobucharIonoModel *pEHubIonoRpt,
        epKlobucharIonoModel &ePIonoRpt
    );
    void pbFillEpGloAddParamsData
    (
        const EHMessageReportGlonassAdditionalParams *pEHubAddParamsRpt,
        epGloAdditionalParameter &ePAddParamsRpt
    );
    void pbFillEpIntraSystemTimeBiasInfo
    (
        const PBGnssInterSystemBias &istInput,
        epGnssSystemTimeStructType &ePISTOut,
        uint8_t whichOut
    );
    void pbFillEpGloIntraSystemTimeBiasInfo
    (
        const PBGnssInterSystemBias &istInput,
        epGnssGloTimeStructType &ePISTOut
    );
    void pbFillEHubNavSolutionMask
    (
        const epPositionFlags &ePPosFlags,
        uint32_t &pbNavSolutionMask
    );
    void pbFillEpDgnssSVMeasurement
    (
        const PBGnssLocDgnssSVMeasurement &dgnssSvMeas,
        epDgnssSVMeasurement &epDgnssSvMeas
    );
#ifdef FEATURE_CDFW
    void pbFillQDgnssReferenceStationData
    (
        const EHMessageReportCdfwQDgnssReferenceStation *pEHubQDgnssRefStationInfo,
        ReferenceStation &qDgnssRefStation
    );
    void pbFillQDgnssObservation
    (
        const EHMessageReportCdfwQDgnssObservation *pEHubQDgnssObserInfo,
        DGnssObservation &qDgnssObserv
    );
#endif //FEATURE_CDFW
    void pbFillEpLeverArmConfData
    (
        const EHMessageStatusLeverArmConfig *pEHubLeverArmConfRpt,
        epLeverArmConfigInfo &ePLeverArmConfigData
    );

    int pbPopulateQwesInstallLicenseReqWithCtr
    (
        const uint8_t *licenseBuf,
        uint32_t licenseLen,
        const string &epUniqueName,
        uint32_t sendMsgCounter,
        string &os
    );
    int pbPopulateAttestationStatementReqWithCtr
    (
        const uint8_t *nonce,
        uint32_t nonceLen,
        const uint8_t *data,
        uint32_t dataLen,
        const string &epUniqueName,
        uint32_t sendMsgCounter,
        string &os
    );
    EpProtoMsgConverter() {
        LOC_LOGi("EnginePlugin: EH version  Major:%d, Minor:%d", EHMSG_VER_MAJOR,
                 EHMSG_VER_MINOR);
        LOC_LOGi("EnginePlugin: EH LocationDataTypes version  Major:%d, Minor:%d",
                 PB_LOCDATATYPES_VER_MAJOR, PB_LOCDATATYPES_VER_MINOR);
    }
    ~EpProtoMsgConverter() {}
};


// Entry point to the library
typedef void* (getEngPluginMsgConvIfaceFunc)();

struct EngPluginMsgConvInterface {
    size_t size;
    // Encode routines
    bool (*pbRegisterWithEHub)
    (
        const EHubTechId &techID,
        const epFeatureStatus &featureStatus,
        const epEngineProcessorType &engineProcessorType,
        const uint32_t &sendMsgCounter,
        const epSubscriptionInfo &epSubInfo,
        const string &sockName,
        const char *ePServerIpAddr,
        const char *ePServerPort,
        int featureId,
        string &protoStr
    );
    bool (*encodeEHubMessageReportPosition)
    (
        const epImplPVTReport * pEpImplPvtReport,
        const Gnss_ApTimeStampStructType &bootTimeStamp,
        const string &epUniqueName,
        uint32_t sendMsgCounter,
        string &protoStr
    );
    // Decode routines
    bool (*decodeEHubWthMsgCounterPayLoadMsg)
    (
        const string& eHubWithCtrPayLoadMsg,
        string& engHubMsgPayload,
        uint32_t &msgId,
        uint32_t &msgCounter,
        string &senderName
    );
    // This Decode routine's returns allocated memory. Responsibility of caller to
    // free up the memory.
    EHubMessageProtocolRoutingTableUpdate* (*decodeEHMessageProtocolRoutTblUpdt)
    (
        const string& pbEhMsgProtoRoutTblUpdt
    );
    bool (*pbFillepSetPositionModeCmdReq)
    (
        const string& ehMsgPayload,
        epCommandRequest &epCmd
    );
    bool (*pbFillepDeleteAidingDataCmdReq)
    (
        const string& ehMsgPayload,
        epCommandRequest &epCmd
    );
    uint32_t (*getPosEngineMaskFromDeleteAidingData)
    (
        const string& ehMsgMsrDelAidDataPayload
    );
    bool (*pbFillepImplPVTReport)
    (
        const string& ehMsgPayload,
        epImplPVTReport &ePImplPVTRpt,
        EHTechId &sourceTechId
    );
    bool (*pbFillepGnssSvNotification)
    (
        const string& ehMsgPayload,
        epGnssSvNotification &ePGnssSv,
        EHTechId &sourceTechId
    );
    bool (*pbFillepGnssSvPolynomial)
    (
        const string& ehMsgPayload,
        epImplGnssSvPolynomial &epSvPoly,
        EHTechId &sourceTechId
    );
    void (*getNhzInfoFromSvMeas)
    (
        const string& ehMsgMsrRptPayload,
        EHTechId &sourceTechId,
        bool &isNhz
    );
    bool (*pbFillepGnssMeasReport)
    (
        const string& ehMsgPayload,
        epGnssImplMeasurementReport &ePImplMsrRptOut,
        uint64_t &measurementStatus
    );
    void (*getClkTickCountInfoFromSvMeas)
    (
        const string& ehMsgMsrRptPayload,
        uint64_t &headerFlags,
        uint64_t &refCountTicks,
        uint64_t &ptpTimens
    );
    bool (*getConnectionStatusFromEHMsgStatusConnectivity)
    (
        const string& ehMsgMsgStatConnectivity
    );
    void (*getInstallLicenseRespFromEHMsgQWESInstallLicenseResp)
    (
        const string &ehMsgQwesResp,
        ehInstallLicenseStatus &status
    );
    void (*getFeatureStatusFromEHMsgQWESFeatureStatusInfo)
    (
        const string &ehMsgQwesStatus,
        ehFeatureStatus &featureStatus,
        ehLicenseType &licenseType
    );
    void (*getAttestationStatementFromEHMsgQwesAttestationStatement)
    (
        const string &ehMsgAttestation,
        std::string &outBuf,
        uint32_t &bufLen,
        ehAttestationStatus &status
    );
    int32_t (*getQwesSyncStatusFromEHMsgQWESForceSyncNotify)
    (
        const string &ehMsgNotify
    );
    bool (*pbFillepGnssEphemerisReport)
    (
        const string& ehMsgPayload,
        epGnssEphemerisReport &ePEphRpt,
        EHTechId &sourceTechId
    );
    bool (*pbFillepKlobucharIonoModel)
    (
        const string& ehMsgPayload,
        epKlobucharIonoModel &ePIonoRpt,
        EHTechId &sourceTechId
    );
    bool (*pbFillepGloAdditionalParameter)
    (
        const string& ehMsgPayload,
        epGloAdditionalParameter &ePAdditionalData,
        EHTechId &sourceTechId
    );
#ifdef FEATURE_CDFW
    void (*pbFillQDgnssReferenceStationData)
    (
        const string& ehMsgPayload,
        ReferenceStation &qDgnssRefStation
    );
    void (*pbFillQDgnssObservation)
    (
        const string& ehMsgPayload,
        DGnssObservation &qDgnssObserv
    );
#endif
    void (*pbFillepLeverArmConfData)
    (
        const string& ehMsgPayload,
        epLeverArmConfigInfo &ePLeverArmConfigData
    );
    bool (*pbFillepDeadReckoningEngineConfig)
    (
        const string& ehMsgPayload,
        epDeadReckoningEngineConfig &epDREConfParams
    );
    int (*pbPopulateQwesInstallLicenseReqWithCtr)
    (
        const uint8_t *licenseBuf,
        uint32_t licenseLen,
        const string &epUniqueName,
        uint32_t sendMsgCounter,
        string &os
    );
    int (*pbPopulateAttestationStatementReqWithCtr)
    (
        const uint8_t *nonce,
        uint32_t nonceLen,
        const uint8_t *data,
        uint32_t dataLen,
        const string &epUniqueName,
        uint32_t sendMsgCounter,
        string &os
    );
};

static inline EngPluginMsgConvInterface* loadEngPluginMsgConvIface() {
    static EngPluginMsgConvInterface *engPluginMsgConvIface = nullptr;

    if (nullptr == engPluginMsgConvIface) {
        void* libHandle = nullptr;
        getEngPluginMsgConvIfaceFunc* getter = (getEngPluginMsgConvIfaceFunc*)dlGetSymFromLib(
                libHandle, "libeng_hub_msg_proto.so", "getEngPluginMsgConvIface");
        if (nullptr != getter) {
            engPluginMsgConvIface = (EngPluginMsgConvInterface*)(*getter)();
        }
    }
    return engPluginMsgConvIface;
}

}
#endif //EP_PROTO_MSG_CONVERTER_H
