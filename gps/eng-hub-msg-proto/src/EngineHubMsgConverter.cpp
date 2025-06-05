/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Engine Hub Message Converter

 This file contains the implementation of helper class for engine hub message
 conversion routines.

 Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#define LOG_TAG "Loc_EngHubMsgConv"

#include <inttypes.h>
#include <dirent.h>

#include <gps_extended.h>
#include <MsgTask.h>
#include <LocIpc.h>
#include <LocTimer.h>
#include <loc_cfg.h>

#include <EHMsgUtils.h>
#include <EngineHubMsgConverter.h>

#ifdef FEATURE_CDFW
// EHUB QDGnss interfacing
#include <IQDgnss.h>
#include <QDgnssApi.h>
#endif

using namespace loc_util;
using std::min;

PBLocSourceofFreqEnumType EngineHubMsgConverter::getPBEnumForGnssLocSourceofFreq(
        const Gnss_LocSourceofFreqEnumType &sourceOfFreq) {
    PBLocSourceofFreqEnumType pbSrcFreq = PB_GNSS_FREQ_SOURCE_INVALID;

    switch (sourceOfFreq) {
        case GNSS_LOC_FREQ_SOURCE_EXTERNAL:
            pbSrcFreq = PB_GNSS_FREQ_SOURCE_EXTERNAL;
        break;
        case GNSS_LOC_FREQ_SOURCE_PE_CLK_REPORT:
            pbSrcFreq = PB_GNSS_FREQ_SOURCE_PE_CLK_REPORT;
        break;
        case GNSS_LOC_FREQ_SOURCE_UNKNOWN:
            pbSrcFreq = PB_GNSS_FREQ_SOURCE_UNKNOWN;
        break;
        default:
        break;
    }
    return pbSrcFreq;
}

PBGnss_LocSvSystemEnumType EngineHubMsgConverter::getPBEnumForGnssLocSvSystem(
        const Gnss_LocSvSystemEnumType &locSvSystem) {
    PBGnss_LocSvSystemEnumType pbSystemType = PB_GNSS_LOC_SV_SYSTEM_UNKNOWN;

    switch (locSvSystem) {
        case GNSS_LOC_SV_SYSTEM_GPS:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_GPS;
        break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_GALILEO;
        break;
        case GNSS_LOC_SV_SYSTEM_SBAS:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_SBAS;
        break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_GLONASS;
        break;
        case GNSS_LOC_SV_SYSTEM_BDS:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_BDS;
        break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_QZSS;
        break;
        case GNSS_LOC_SV_SYSTEM_NAVIC:
            pbSystemType = PB_GNSS_LOC_SV_SYSTEM_NAVIC;
        break;
        default:
        break;
    }
    return pbSystemType;
}

PBSvSearchStatus EngineHubMsgConverter::getPBEnumForSvSearchStatus(
        const Gnss_LocSvSearchStatusEnumT &gnssSearchStatus) {
    PBSvSearchStatus pbSvSearchStaus = PB_GNSS_SV_SRCH_STATUS_UNKNOWN;

    switch (gnssSearchStatus) {
        case GNSS_LOC_SV_SRCH_STATUS_IDLE:
            pbSvSearchStaus = PB_GNSS_SV_SRCH_STATUS_IDLE;
        break;
        case GNSS_LOC_SV_SRCH_STATUS_SEARCH:
            pbSvSearchStaus = PB_GNSS_SV_SRCH_STATUS_SEARCH;
        break;
        case GNSS_LOC_SV_SRCH_STATUS_TRACK:
            pbSvSearchStaus = PB_GNSS_SV_SRCH_STATUS_TRACK;
        break;
        default:
        break;
    }
    return pbSvSearchStaus;
}

PBGnssEphAction EngineHubMsgConverter::getPBEnumForGnssEphAction(
        const GnssEphAction &gnssEphAction) {
    PBGnssEphAction ephAction = PB_GNSS_EPH_ACTION_UPDATE_SRC_UNKNOWN_V02;
    switch (gnssEphAction) {
        case GNSS_EPH_ACTION_UPDATE_SRC_UNKNOWN_V02:
            ephAction = PB_GNSS_EPH_ACTION_UPDATE_SRC_UNKNOWN_V02;
        break;
        case GNSS_EPH_ACTION_UPDATE_SRC_OTA_V02:
            ephAction = PB_GNSS_EPH_ACTION_UPDATE_SRC_OTA_V02;
        break;
        case GNSS_EPH_ACTION_UPDATE_SRC_NETWORK_V02:
            ephAction = PB_GNSS_EPH_ACTION_UPDATE_SRC_NETWORK_V02;
        break;
        case GNSS_EPH_ACTION_UPDATE_MAX_V02:
            ephAction = PB_GNSS_EPH_ACTION_UPDATE_MAX_V02;
        break;
        case GNSS_EPH_ACTION_DELETE_SRC_UNKNOWN_V02:
            ephAction = PB_GNSS_EPH_ACTION_DELETE_SRC_UNKNOWN_V02;
        break;
        case GNSS_EPH_ACTION_DELETE_SRC_NETWORK_V02:
            ephAction = PB_GNSS_EPH_ACTION_DELETE_SRC_NETWORK_V02;
        break;
        case GNSS_EPH_ACTION_DELETE_SRC_OTA_V02:
            ephAction = PB_GNSS_EPH_ACTION_DELETE_SRC_OTA_V02;
        break;
        case GNSS_EPH_ACTION_DELETE_MAX_V02:
            ephAction = PB_GNSS_EPH_ACTION_DELETE_MAX_V02;
        break;
        default:
        break;
    }
    return ephAction;
}

PBGalEphSignalSource EngineHubMsgConverter::getPBEnumForGalEphSignalSource(
        const GalEphSignalSource &galEphSigSrc) {
    PBGalEphSignalSource pbGalEphSigSrc = PB_GAL_EPH_SIGNAL_SRC_UNKNOWN_V02;
    switch (galEphSigSrc) {
        case GAL_EPH_SIGNAL_SRC_E1B_V02:
            pbGalEphSigSrc = PB_GAL_EPH_SIGNAL_SRC_E1B_V02;
            break;
        case GAL_EPH_SIGNAL_SRC_E5A_V02:
            pbGalEphSigSrc = PB_GAL_EPH_SIGNAL_SRC_E5A_V02;
            break;
        case GAL_EPH_SIGNAL_SRC_E5B_V02:
            pbGalEphSigSrc = PB_GAL_EPH_SIGNAL_SRC_E5B_V02;
    }
    return pbGalEphSigSrc;
}

PBGnssSvType EngineHubMsgConverter::getPBEnumForGnssSvType(const GnssSvType &gnssSvType) {
    PBGnssSvType pbGnssSvType = PB_GNSS_SV_TYPE_UNKNOWN;
    switch (gnssSvType) {
        case GNSS_SV_TYPE_GPS:
            pbGnssSvType = PB_GNSS_SV_TYPE_GPS;
            break;
        case GNSS_SV_TYPE_GLONASS:
            pbGnssSvType = PB_GNSS_SV_TYPE_GLONASS;
            break;
        case GNSS_SV_TYPE_QZSS:
            pbGnssSvType = PB_GNSS_SV_TYPE_QZSS;
            break;
        case GNSS_SV_TYPE_BEIDOU:
            pbGnssSvType = PB_GNSS_SV_TYPE_BEIDOU;
            break;
        case GNSS_SV_TYPE_GALILEO:
            pbGnssSvType = PB_GNSS_SV_TYPE_GALILEO;
            break;
        default:
            break;
    }
    return pbGnssSvType;
}

PBLocSessionStatus EngineHubMsgConverter::getPBEnumForLocSessStatus(
        const loc_sess_status &locSessStatus) {
    PBLocSessionStatus pbLocSessStatus = PB_LOC_SESS_FAILURE;
    switch (locSessStatus) {
        case LOC_SESS_SUCCESS:
            pbLocSessStatus = PB_LOC_SESS_SUCCESS;
            break;
        case LOC_SESS_INTERMEDIATE:
            pbLocSessStatus = PB_LOC_SESS_INTERMEDIATE;
            break;
        case LOC_SESS_FAILURE:
            pbLocSessStatus = PB_LOC_SESS_FAILURE;
            break;
        default:
            break;
    }
    return pbLocSessStatus;
}

PBLocReliability EngineHubMsgConverter::getPBEnumForLocReliability(
        const LocReliability &locReliability) {
    PBLocReliability pbLocReliability = PB_LOC_RELIABILITY_NOT_SET;
    switch (locReliability) {
        case LOC_RELIABILITY_NOT_SET:
            pbLocReliability = PB_LOC_RELIABILITY_NOT_SET;
            break;
        case LOC_RELIABILITY_VERY_LOW:
            pbLocReliability = PB_LOC_RELIABILITY_VERY_LOW;
            break;
        case LOC_RELIABILITY_LOW:
            pbLocReliability = PB_LOC_RELIABILITY_LOW;
            break;
        case LOC_RELIABILITY_MEDIUM:
            pbLocReliability = PB_LOC_RELIABILITY_MEDIUM;
            break;
        case LOC_RELIABILITY_HIGH:
            pbLocReliability = PB_LOC_RELIABILITY_HIGH;
            break;
        default:
            break;
    }
    return pbLocReliability;
}

PBLocOutputEngineType EngineHubMsgConverter::getPBEnumForLocOutputEngineType(
            const LocOutputEngineType &locOutputEngType) {
    PBLocOutputEngineType pbLocOutputEngType = PB_LOC_OUTPUT_ENGINE_FUSED;
    switch (locOutputEngType) {
        case LOC_OUTPUT_ENGINE_FUSED:
            pbLocOutputEngType = PB_LOC_OUTPUT_ENGINE_FUSED;
            break;
        case LOC_OUTPUT_ENGINE_SPE:
            pbLocOutputEngType = PB_LOC_OUTPUT_ENGINE_SPE;
            break;
        case LOC_OUTPUT_ENGINE_PPE:
            pbLocOutputEngType = PB_LOC_OUTPUT_ENGINE_PPE;
            break;
        case LOC_OUTPUT_ENGINE_COUNT:
            pbLocOutputEngType = PB_LOC_OUTPUT_ENGINE_COUNT;
            break;
        default:
            break;
    }
    return pbLocOutputEngType;
}

PBLocSvInfoSource EngineHubMsgConverter::getPBEnumForLocSvInfoSource(
        const LocSvInfoSource &locSvSrcInfo) {
    PBLocSvInfoSource pbLocSvInfoSrc = PB_ULP_SVINFO_SOURCE_INVALID;
    switch (locSvSrcInfo) {
        case ULP_SVINFO_IS_FROM_GNSS:
            pbLocSvInfoSrc = PB_ULP_SVINFO_IS_FROM_GNSS;
            break;
        case ULP_SVINFO_IS_FROM_DR:
            pbLocSvInfoSrc = PB_ULP_SVINFO_IS_FROM_DR;
            break;
        default:
            break;
    }
    return pbLocSvInfoSrc;
}

PBCarrierPhaseAmbiguityType EngineHubMsgConverter::getPBEnumForCarrierPhaseAmbiguityType(
            const CarrierPhaseAmbiguityType &carrirPhaseAmbigType){
    PBCarrierPhaseAmbiguityType pbCarrierPhaseAmbigType =
            PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_NONE;
    switch (carrirPhaseAmbigType) {
        case CARRIER_PHASE_AMBIGUITY_RESOLUTION_NONE:
            pbCarrierPhaseAmbigType = PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_NONE;
            break;
        case CARRIER_PHASE_AMBIGUITY_RESOLUTION_FLOAT:
            pbCarrierPhaseAmbigType = PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FLOAT;
            break;
        case CARRIER_PHASE_AMBIGUITY_RESOLUTION_FIXED:
            pbCarrierPhaseAmbigType = PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FIXED;
            break;
        default:
            break;
    }
    return pbCarrierPhaseAmbigType;
}

EHTechId EngineHubMsgConverter::getPBEnumForEHubTechId(const EHubTechId &engHubTechId) {
    EHTechId pbEngHubTechId = EH_NODE_GNSS;
    switch (engHubTechId) {
        case E_HUB_TECH_GNSS:
            pbEngHubTechId = EH_NODE_GNSS;
            break;
        case E_HUB_TECH_PPE:
            pbEngHubTechId = EH_NODE_PPE;
            break;
        case E_HUB_TECH_DR:
            pbEngHubTechId = EH_NODE_DR;
            break;
        case E_HUB_TECH_VPE:
            pbEngHubTechId = EH_NODE_VPE;
            break;
        case E_HUB_TECH_COUNT:
        default:
            LOC_LOGe("Invalid tech id : %d", engHubTechId);
            break;
    }
    return pbEngHubTechId;
}

PBLocPositionMode EngineHubMsgConverter::getPBEnumForLocPositionMode(
        const LocPositionMode &locPosMode) {
    PBLocPositionMode pbLocPosMode = PB_LOC_POSITION_MODE_STANDALONE;
    switch (locPosMode) {
        case LOC_POSITION_MODE_STANDALONE:
            pbLocPosMode = PB_LOC_POSITION_MODE_STANDALONE;
            break;
        case LOC_POSITION_MODE_MS_BASED:
            pbLocPosMode = PB_LOC_POSITION_MODE_MS_BASED;
            break;
        case LOC_POSITION_MODE_MS_ASSISTED:
            pbLocPosMode = PB_LOC_POSITION_MODE_MS_ASSISTED;
            break;
        case LOC_POSITION_MODE_RESERVED_1:
            pbLocPosMode = PB_LOC_POSITION_MODE_RESERVED_1;
            break;
        case LOC_POSITION_MODE_RESERVED_2:
            pbLocPosMode = PB_LOC_POSITION_MODE_RESERVED_2;
            break;
        case LOC_POSITION_MODE_RESERVED_3:
            pbLocPosMode = PB_LOC_POSITION_MODE_RESERVED_3;
            break;
        case LOC_POSITION_MODE_RESERVED_4:
            pbLocPosMode = PB_LOC_POSITION_MODE_RESERVED_4;
            break;
        case LOC_POSITION_MODE_RESERVED_5:
            pbLocPosMode = PB_LOC_POSITION_MODE_RESERVED_5;
            break;
        default:
            break;
    }
    return pbLocPosMode;
}

PBLocGpsPositionRecurrence EngineHubMsgConverter::getPBEnumForLocGpsPositionRecurrence(
        const LocGpsPositionRecurrence &locGpsRecur) {
    PBLocGpsPositionRecurrence pbLocGpsRecur = PB_LOC_GPS_POSITION_RECURRENCE_PERIODIC;
    switch (locGpsRecur) {
        case LOC_GPS_POSITION_RECURRENCE_PERIODIC:
            pbLocGpsRecur = PB_LOC_GPS_POSITION_RECURRENCE_PERIODIC;
            break;
        case LOC_GPS_POSITION_RECURRENCE_SINGLE:
            pbLocGpsRecur = PB_LOC_GPS_POSITION_RECURRENCE_SINGLE;
            break;
        default:
            break;
    }
    return pbLocGpsRecur;
}

PBGnssPowerMode EngineHubMsgConverter::getPBEnumForGnssPowerMode(
        const GnssPowerMode &gnssPwrMode) {
    PBGnssPowerMode pbGnssPwrMode = PB_GNSS_POWER_MODE_INVALID;
    switch (gnssPwrMode) {
        case GNSS_POWER_MODE_M1:
            pbGnssPwrMode = PB_GNSS_POWER_MODE_M1;
            break;
        case GNSS_POWER_MODE_M2:
            pbGnssPwrMode = PB_GNSS_POWER_MODE_M2;
            break;
        case GNSS_POWER_MODE_M3:
            pbGnssPwrMode = PB_GNSS_POWER_MODE_M3;
            break;
        case GNSS_POWER_MODE_M4:
            pbGnssPwrMode = PB_GNSS_POWER_MODE_M4;
            break;
        case GNSS_POWER_MODE_M5:
            pbGnssPwrMode = PB_GNSS_POWER_MODE_M5;
            break;
        default:
            break;
    }
    return pbGnssPwrMode;
}

EHConnectionType EngineHubMsgConverter::getPBEnumForEngineConnectionType(
        const EngineConnectionType &engConnType) {
    EHConnectionType pbEhConnType = EH_ENGINE_CONNECT_TYPE_LOCAL;
    switch (engConnType) {
        case ENGINE_CONNECT_TYPE_LOCAL:
            pbEhConnType = EH_ENGINE_CONNECT_TYPE_LOCAL;
            break;
        case ENGINE_CONNECT_TYPE_INET:
            pbEhConnType = EH_ENGINE_CONNECT_TYPE_INET;
            break;
        default:
            break;
    }
    return pbEhConnType;
}

PBLocDgnssCorrectionSourceType EngineHubMsgConverter::getPBEnumForLocDgnssCorrectionSourceType(
            const LocDgnssCorrectionSourceType &locDgnssCorrnSrcType) {
    PBLocDgnssCorrectionSourceType pbLocDgnssCorrnSrcType = PB_LOC_DGNSS_CORR_SOURCE_TYPE_INVALID;
    switch (locDgnssCorrnSrcType) {
        case LOC_DGNSS_CORR_SOURCE_TYPE_RTCM:
            pbLocDgnssCorrnSrcType = PB_LOC_DGNSS_CORR_SOURCE_TYPE_RTCM;
            break;
        case LOC_DGNSS_CORR_SOURCE_TYPE_3GPP:
            pbLocDgnssCorrnSrcType = PB_LOC_DGNSS_CORR_SOURCE_TYPE_3GPP;
            break;
        default:
            break;
    }
    return pbLocDgnssCorrnSrcType;
}

#ifdef FEATURE_CDFW
// QDGnss  enum conversion to protobuff enums
PBQDgnssDataType EngineHubMsgConverter::getPBEnumForQDgnssDataType(
        const uint32_t &qDgnssDataType) {
    PBQDgnssDataType pbQdgnssDataType = PB_QDGNSS_DATATYPE_INVALID;
    // #define QDGNSS_* in  QDgnssDataType.h
    switch (qDgnssDataType) {
        case QDGNSS_REFERENCE_STATION_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_REFERENCE_STATION_MSG_ID;
            break;
        case QDGNSS_OBSERVATIONS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_OBSERVATIONS_MSG_ID;
            break;
        case QDGNSS_GLONASS_BIAS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_GLONASS_BIAS_MSG_ID;
            break;
        case QDGNSS_SSR_ORBIT_CORRECTIONS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SSR_ORBIT_CORRECTIONS_MSG_ID;
            break;
        case QDGNSS_SSR_CLOCK_CORRECTIONS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SSR_CLOCK_CORRECTIONS_MSG_ID;
            break;
        case QDGNSS_SSR_HIGHRATE_CLOCK_CORRECTIONS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SSR_HIGHRATE_CLOCK_CORRECTIONS_MSG_ID;
            break;
        case QDGNSS_SSR_CODE_BIAS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SSR_CODE_BIAS_MSG_ID;
            break;
        case QDGNSS_SSR_USER_RANGE_ACCURACY_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SSR_USER_RANGE_ACCURACY_MSG_ID;
            break;
        case QDGNSS_SSR_ORBIT_AND_CLOCK_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SSR_ORBIT_AND_CLOCK_MSG_ID;
            break;
        case QDGNSS_GPS_EPHEMERIS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_GPS_EPHEMERIS_MSG_ID;
            break;
        case QDGNSS_GLONASS_EPHEMERIS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_GLONASS_EPHEMERIS_MSG_ID;
            break;
        case QDGNSS_GALILEO_EPHEMERIS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_GALILEO_EPHEMERIS_MSG_ID;
            break;
        case QDGNSS_BDS_EPHEMERIS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_BDS_EPHEMERIS_MSG_ID;
            break;
        case QDGNSS_QZSS_EPHEMERIS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_QZSS_EPHEMERIS_MSG_ID;
            break;
        case QDGNSS_SBAS_EPHEMERIS_MSG_ID:
            pbQdgnssDataType = PB_QDGNSS_SBAS_EPHEMERIS_MSG_ID;
            break;
        default:
            break;
    }
    return pbQdgnssDataType;
}

PBExtendedSatelliteInformation EngineHubMsgConverter::getPBEnumForExtendedSatelliteInformation(
        const ExtendedSatelliteInformation &extSatInfo) {
    PBExtendedSatelliteInformation pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0;
    switch (extSatInfo) {
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_7:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_7;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_6:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_6;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_5:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_5;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_4:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_4;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_3:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_3;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_2:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_2;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_1:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_1;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_1:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_1;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_2:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_2;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_3:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_3;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_4:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_4;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_5:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_5;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_6:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_6;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_RESERVED:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_RESERVED;
            break;
        case DGNSS_GLONASS_FREQUENCY_CHANNEL_UNKNOWN:
            pbExtSatInfo = PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_UNKNOWN;
            break;
        default:
            break;
    }
    return pbExtSatInfo;
}

PBDGnssSbasSignalIdentifier EngineHubMsgConverter::getPBEnumForDGnssSbasSignalIdentifier(
        const DGnssSbasSignalIdentifier &dGnssSbasSignId) {
    PBDGnssSbasSignalIdentifier pbGnssSbasSignId = PB_DGNSS_SIGNAL_SBAS_L1CA;
    switch (dGnssSbasSignId) {
        case DGNSS_SIGNAL_SBAS_L1CA:
            pbGnssSbasSignId = PB_DGNSS_SIGNAL_SBAS_L1CA;
            break;
        case DGNSS_SIGNAL_SBAS_L5I:
            pbGnssSbasSignId = PB_DGNSS_SIGNAL_SBAS_L5I;
            break;
        case DGNSS_SIGNAL_SBAS_L5Q:
            pbGnssSbasSignId = PB_DGNSS_SIGNAL_SBAS_L5Q;
            break;
        case DGNSS_SIGNAL_SBAS_L5IQ:
            pbGnssSbasSignId = PB_DGNSS_SIGNAL_SBAS_L5IQ;
            break;
        default:
            break;
    }
    return pbGnssSbasSignId;
}

PBDGnssBdsSignalIdentifier EngineHubMsgConverter::getPBEnumForDGnssBdsSignalIdentifier(
        const DGnssBdsSignalIdentifier &dGnssBdsSignId) {
    PBDGnssBdsSignalIdentifier pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B12I;
    switch (dGnssBdsSignId) {
        case DGNSS_SIGNAL_BDS_B12I:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B12I;
            break;
        case DGNSS_SIGNAL_BDS_B12Q:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B12Q;
            break;
        case DGNSS_SIGNAL_BDS_B12IQ:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B12IQ;
            break;
        case DGNSS_SIGNAL_BDS_B3I:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B3I;
            break;
        case DGNSS_SIGNAL_BDS_B3Q:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B3Q;
            break;
        case DGNSS_SIGNAL_BDS_B3IQ:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B3IQ;
            break;
        case DGNSS_SIGNAL_BDS_B3A:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B3A;
            break;
        case DGNSS_SIGNAL_BDS_B2I:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2I;
            break;
        case DGNSS_SIGNAL_BDS_B2Q:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2Q;
            break;
        case DGNSS_SIGNAL_BDS_B2IQ:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2IQ;
            break;
        case DGNSS_SIGNAL_BDS_B2AD:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2AD;
            break;
        case DGNSS_SIGNAL_BDS_B2AP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2AP;
            break;
        case DGNSS_SIGNAL_BDS_B2ADP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2ADP;
            break;
        case DGNSS_SIGNAL_BDS_B1CD:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B1CD;
            break;
        case DGNSS_SIGNAL_BDS_B1CP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B1CP;
            break;
        case DGNSS_SIGNAL_BDS_B1CDP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B1CDP;
            break;
        case DGNSS_SIGNAL_BDS_B1A:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B1A;
            break;
        case DGNSS_SIGNAL_BDS_B1N:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B1N;
            break;
        case DGNSS_SIGNAL_BDS_B2BD:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2BD;
            break;
        case DGNSS_SIGNAL_BDS_B2BP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2BP;
            break;
        case DGNSS_SIGNAL_BDS_B2BDP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2BDP;
            break;
        case DGNSS_SIGNAL_BDS_B2D:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2D;
            break;
        case DGNSS_SIGNAL_BDS_B2P:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2P;
            break;
        case DGNSS_SIGNAL_BDS_B2DP:
            pbDGnssBdsSignId = PB_DGNSS_SIGNAL_BDS_B2DP;
            break;
        default:
            break;
    }
    return pbDGnssBdsSignId;
}

PBDGnssQzssSignalIdentifier EngineHubMsgConverter::getPBEnumForDGnssQzssSignalIdentifier(
        const DGnssQzssSignalIdentifier &dGnssQzssSignId) {
    PBDGnssQzssSignalIdentifier pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L1CA;
    switch (dGnssQzssSignId) {
        case DGNSS_SIGNAL_QZSS_L1CA:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L1CA;
            break;
        case DGNSS_SIGNAL_QZSS_LEXS:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_LEXS;
            break;
        case DGNSS_SIGNAL_QZSS_LEXL:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_LEXL;
            break;
        case DGNSS_SIGNAL_QZSS_LEXSL:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_LEXSL;
            break;
        case DGNSS_SIGNAL_QZSS_L2CM:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L2CM;
            break;
        case DGNSS_SIGNAL_QZSS_L2CL:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L2CL;
            break;
        case DGNSS_SIGNAL_QZSS_L2CML:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L2CML;
            break;
        case DGNSS_SIGNAL_QZSS_L5I:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L5I;
            break;
        case DGNSS_SIGNAL_QZSS_L5Q:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L5Q;
            break;
        case DGNSS_SIGNAL_QZSS_L5IQ:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L5IQ;
            break;
        case DGNSS_SIGNAL_QZSS_L1CD:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L1CD;
            break;
        case DGNSS_SIGNAL_QZSS_L1CP:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L1CP;
            break;
        case DGNSS_SIGNAL_QZSS_L1CDP:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L1CDP;
            break;
        case DGNSS_SIGNAL_QZSS_L1S:
            pbDGnssQzssSignId = PB_DGNSS_SIGNAL_QZSS_L1S;
            break;
        default:
            break;
    }
    return pbDGnssQzssSignId;
}

PBDGnssGalileoSignalIdentifier EngineHubMsgConverter::getPBEnumForDGnssGalileoSignalIdentifier(
        const DGnssGalileoSignalIdentifier &dGnssGalSignId) {
    PBDGnssGalileoSignalIdentifier pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E1C;
    switch (dGnssGalSignId) {
        case DGNSS_SIGNAL_GALILEO_E1C:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E1C;
            break;
        case DGNSS_SIGNAL_GALILEO_E1A:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E1A;
            break;
        case DGNSS_SIGNAL_GALILEO_E1B:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E1B;
            break;
        case DGNSS_SIGNAL_GALILEO_E1BC:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E1BC;
            break;
        case DGNSS_SIGNAL_GALILEO_E1ABC:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E1ABC;
            break;
        case DGNSS_SIGNAL_GALILEO_E6C:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E6C;
            break;
        case DGNSS_SIGNAL_GALILEO_E6A:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E6A;
            break;
        case DGNSS_SIGNAL_GALILEO_E6B:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E6B;
            break;
        case DGNSS_SIGNAL_GALILEO_E6BC:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E6BC;
            break;
        case DGNSS_SIGNAL_GALILEO_E6ABC:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E6ABC;
            break;
        case DGNSS_SIGNAL_GALILEO_E5BI:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5BI;
            break;
        case DGNSS_SIGNAL_GALILEO_E5BQ:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5BQ;
            break;
        case DGNSS_SIGNAL_GALILEO_E5BIQ:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5BIQ;
            break;
        case DGNSS_SIGNAL_GALILEO_E5ABI:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5ABI;
            break;
        case DGNSS_SIGNAL_GALILEO_E5ABQ:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5ABQ;
            break;
        case DGNSS_SIGNAL_GALILEO_E5ABIQ:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5ABIQ;
            break;
        case DGNSS_SIGNAL_GALILEO_E5AI:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5AI;
            break;
        case DGNSS_SIGNAL_GALILEO_E5AQ:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5AQ;
            break;
        case DGNSS_SIGNAL_GALILEO_E5AIQ:
            pbDGnssGalSignId = PB_DGNSS_SIGNAL_GALILEO_E5AIQ;
            break;
        default:
            break;
    }
    return pbDGnssGalSignId;
}

PBDGnssGlonassSignalIdentifier EngineHubMsgConverter::getPBEnumForDGnssGlonassSignalIdentifier(
        const DGnssGlonassSignalIdentifier &dGnssGloSignId) {
    PBDGnssGlonassSignalIdentifier pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G1CA;
    switch (dGnssGloSignId) {
        case DGNSS_SIGNAL_GLONASS_G1CA:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G1CA;
            break;
        case DGNSS_SIGNAL_GLONASS_G1P:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G1P;
            break;
        case DGNSS_SIGNAL_GLONASS_G2CA:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G2CA;
            break;
        case DGNSS_SIGNAL_GLONASS_G2P:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G2P;
            break;
        case DGNSS_SIGNAL_GLONASS_G3I:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G3I;
            break;
        case DGNSS_SIGNAL_GLONASS_G3Q:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G3Q;
            break;
        case DGNSS_SIGNAL_GLONASS_G3IQ:
            pbDGnssGloSignId = PB_DGNSS_SIGNAL_GLONASS_G3IQ;
            break;
        default:
            break;
    }
    return pbDGnssGloSignId;
}

PBDGnssGpsSignalIdentifier EngineHubMsgConverter::getPBEnumForDGnssGpsSignalIdentifier(
        const DGnssGpsSignalIdentifier &dGnssGpsSignId) {
    PBDGnssGpsSignalIdentifier pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1CA;
    switch (dGnssGpsSignId) {
        case DGNSS_SIGNAL_GPS_L1CA:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1CA;
            break;
        case DGNSS_SIGNAL_GPS_L1P:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1P;
            break;
        case DGNSS_SIGNAL_GPS_L1Z:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1Z;
            break;
        case DGNSS_SIGNAL_GPS_L1N:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1N;
            break;
        case DGNSS_SIGNAL_GPS_L2CA:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2CA;
            break;
        case DGNSS_SIGNAL_GPS_L2P:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2P;
            break;
        case DGNSS_SIGNAL_GPS_L2Z:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2Z;
            break;
        case DGNSS_SIGNAL_GPS_L2CM:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2CM;
            break;
        case DGNSS_SIGNAL_GPS_L2CL:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2CL;
            break;
        case DGNSS_SIGNAL_GPS_L2CML:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2CML;
            break;
        case DGNSS_SIGNAL_GPS_L2D:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2D;
            break;
        case DGNSS_SIGNAL_GPS_L2N:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L2N;
            break;
        case DGNSS_SIGNAL_GPS_L5I:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L5I;
            break;
        case DGNSS_SIGNAL_GPS_L5Q:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L5Q;
            break;
        case DGNSS_SIGNAL_GPS_L5IQ:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L5IQ;
            break;
        case DGNSS_SIGNAL_GPS_L1CD:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1CD;
            break;
        case DGNSS_SIGNAL_GPS_L1CP:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1CP;
            break;
        case DGNSS_SIGNAL_GPS_L1CDP:
            pbDGnssGpsSignId = PB_DGNSS_SIGNAL_GPS_L1CDP;
            break;
        default:
            break;
    }
    return pbDGnssGpsSignId;
}

PBDGnssConstellationIdentifier EngineHubMsgConverter::getPBEnumForDGnssConstellationIdentifier(
        const DGnssConstellationIdentifier &dGnssConstSignId) {
    PBDGnssConstellationIdentifier pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_GPS;
    switch (dGnssConstSignId) {
        case DGNSS_CONSTELLATION_IDENTIFIER_GPS:
            pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_GPS;
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_SBAS:
            pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_SBAS;
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_QZSS:
            pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_QZSS;
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_GALILEO:
            pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_GALILEO;
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_GLONASS:
            pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_GLONASS;
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_BDS:
            pbDGnssConstId = PB_DGNSS_CONSTELLATION_IDENTIFIER_BDS;
            break;
        default:
            break;
    }
    return pbDGnssConstId;
}
#endif //#ifdef FEATURE_CDFW

uint64_t EngineHubMsgConverter::getPBMaskForGnssLocSvMeasStatus(const uint64_t &measStatusMask) {
    uint64_t pbGnssLocMeasStatusMask = PB_GNSS_LOC_MEAS_STATUS_NULL;

    // Gnss_LocSvMeasStatusMaskType to PBGnssLocSvMeasStatusMaskType
    if (GNSS_LOC_MEAS_STATUS_NULL & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_NULL;
    }
    if (GNSS_LOC_MEAS_STATUS_SM_VALID & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_SM_VALID;
    }
    if (GNSS_LOC_MEAS_STATUS_SB_VALID & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_SB_VALID;
    }
    if (GNSS_LOC_MEAS_STATUS_MS_VALID & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_MS_VALID;
    }
    if (GNSS_LOC_MEAS_STATUS_BE_CONFIRM & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_BE_CONFIRM;
    }
    if (GNSS_LOC_MEAS_STATUS_VELOCITY_VALID & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_VELOCITY_VALID;
    }
    if (GNSS_LOC_MEAS_STATUS_VELOCITY_FINE & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_VELOCITY_FINE;
    }
    if (GNSS_LOC_MEAS_STATUS_LP_VALID & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_LP_VALID;
    }
    if (GNSS_LOC_MEAS_STATUS_LP_POS_VALID & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_LP_POS_VALID;
    }
    if (GNSS_LOC_MEAS_STATUS_FROM_RNG_DIFF & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_FROM_RNG_DIFF;
    }
    if (GNSS_LOC_MEAS_STATUS_FROM_VE_DIFF & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_FROM_VE_DIFF;
    }
    if (GNSS_LOC_MEAS_STATUS_DONT_USE_X & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_DONT_USE_X;
    }
    if (GNSS_LOC_MEAS_STATUS_DONT_USE_M & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_DONT_USE_M;
    }
    if (GNSS_LOC_MEAS_STATUS_DONT_USE_D & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_DONT_USE_D;
    }
    if (GNSS_LOC_MEAS_STATUS_DONT_USE_S & measStatusMask ) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_DONT_USE_S;
    }
    if (GNSS_LOC_MEAS_STATUS_DONT_USE_P & measStatusMask) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_DONT_USE_P;
    }
    if (GNSS_LOC_MEAS_STATUS_GNSS_FRESH_MEAS & measStatusMask) {
        pbGnssLocMeasStatusMask |= PB_GNSS_LOC_MEAS_STATUS_GNSS_FRESH_MEAS;
    }
    return pbGnssLocMeasStatusMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssLocSvInfoMaskT(const uint32_t &gnssSvInfoMask) {
    uint32_t pbSvInfoMask = PB_GNSS_SVINFO_MASK_UNKNOWN;

    if (GNSS_LOC_SVINFO_MASK_HAS_EPHEMERIS & gnssSvInfoMask) {
        pbSvInfoMask |= PB_GNSS_SVINFO_MASK_HAS_EPHEMERIS;
    }
    if (GNSS_LOC_SVINFO_MASK_HAS_ALMANAC & gnssSvInfoMask) {
        pbSvInfoMask |= PB_GNSS_SVINFO_MASK_HAS_ALMANAC;
    }
    return pbSvInfoMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssAdditionalSystemInfoMask(
        const uint32_t &gnssAddnlSysInfoMask) {
    uint32_t maskGnssAddSysInfo = PB_GNSS_ADDITIONAL_SYSTEMINFO_INVALID;
    if (GNSS_ADDITIONAL_SYSTEMINFO_HAS_TAUC & gnssAddnlSysInfoMask) {
        maskGnssAddSysInfo |= PB_GNSS_ADDITIONAL_SYSTEMINFO_HAS_TAUC;
    }
    if (GNSS_ADDITIONAL_SYSTEMINFO_HAS_LEAP_SEC & gnssAddnlSysInfoMask) {
        maskGnssAddSysInfo |= PB_GNSS_ADDITIONAL_SYSTEMINFO_HAS_LEAP_SEC;
    }
    return maskGnssAddSysInfo;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssSvOptionsMask(const uint16_t gnssSvOptionsMask) {
    uint32_t  pbGnssSvOptionsMask = PB_GNSS_SV_OPTIONS_INVALID;
    if (GNSS_SV_OPTIONS_HAS_EPHEMER_BIT & gnssSvOptionsMask) {
        pbGnssSvOptionsMask |= PB_GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
    }
    if (GNSS_SV_OPTIONS_HAS_ALMANAC_BIT & gnssSvOptionsMask) {
        pbGnssSvOptionsMask |= PB_GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
    }
    if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT & gnssSvOptionsMask) {
        pbGnssSvOptionsMask |= PB_GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }
    if (GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT & gnssSvOptionsMask) {
        pbGnssSvOptionsMask |= PB_GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT;
    }
    if (GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT & gnssSvOptionsMask) {
        pbGnssSvOptionsMask |= PB_GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT;
    }
    return pbGnssSvOptionsMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssSignalTypeMask(
        const uint32_t &gnssSignalTypeMask) {
    uint32_t  pbGnssSignalTypeMask = PB_GNSS_SIGNAL_TYPE_UNKNOWN;
    // GnssSignalTypeMask To PBGnssSignalInfoMask
    if (gnssSignalTypeMask & GNSS_SIGNAL_GPS_L1CA) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GPS_L1CA;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GPS_L1C) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GPS_L1C;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GPS_L2) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GPS_L2;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GPS_L5) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GPS_L5;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GLONASS_G1) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GLONASS_G1;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GLONASS_G2) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GLONASS_G2;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GALILEO_E1) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GALILEO_E1;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GALILEO_E5A) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GALILEO_E5A;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_GALILEO_E5B) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_GALILEO_E5B;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_BEIDOU_B1I) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B1I;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_BEIDOU_B1C) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B1C;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_BEIDOU_B2I) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B2I;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_BEIDOU_B2AI) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AI;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_QZSS_L1CA) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_QZSS_L1CA;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_QZSS_L1S) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_QZSS_L1S;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_QZSS_L2) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_QZSS_L2;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_QZSS_L5) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_QZSS_L5;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_SBAS_L1) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_SBAS_L1;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_NAVIC_L5) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_NAVIC_L5;
    }
    if (gnssSignalTypeMask & GNSS_SIGNAL_BEIDOU_B2AQ) {
        pbGnssSignalTypeMask |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AQ;
    }
    return pbGnssSignalTypeMask;
}

// GnssSvPolyStatusMaskValidity To PBGnssSvPolyStatusMaskValidity
uint32_t EngineHubMsgConverter::getPBMaskForGnssSvPolyStatusMaskValidity(
        const uint32_t &gnssSvPolyStatusMaskValidity) {
    uint32_t  pbGnssSvPolyStatMaskValidity = PB_GNSS_SV_POLY_STATUS_INVALID_V02;
    if (gnssSvPolyStatusMaskValidity & GNSS_SV_POLY_SRC_ALM_CORR_VALID_V02) {
        pbGnssSvPolyStatMaskValidity |= PB_GNSS_SV_POLY_SRC_ALM_CORR_VALID_V02;
    }
    if (gnssSvPolyStatusMaskValidity & GNSS_SV_POLY_GLO_STR4_VALID_V02) {
        pbGnssSvPolyStatMaskValidity |= PB_GNSS_SV_POLY_GLO_STR4_VALID_V02;
    }
    if (gnssSvPolyStatusMaskValidity & GNSS_SV_POLY_DELETE_VALID_V02) {
        pbGnssSvPolyStatMaskValidity |= PB_GNSS_SV_POLY_DELETE_VALID_V02;
    }
    if (gnssSvPolyStatusMaskValidity & GNSS_SV_POLY_SRC_GAL_FNAV_OR_INAV_VALID_V02) {
        pbGnssSvPolyStatMaskValidity |= PB_GNSS_SV_POLY_SRC_GAL_FNAV_OR_INAV_VALID_V02;
    }
    return pbGnssSvPolyStatMaskValidity;
}

// GnssSvPolyStatusMask To PBGnssSvPolyStatusMask
uint32_t EngineHubMsgConverter::getPBMaskForGnssSvPolyStatusMask(
        const uint32_t &gnssSvPolyStatusMask) {
    uint32_t  pbGnssSvPolyStatMask = PB_GNSS_SV_POLY_INVALID_V02;
    if (gnssSvPolyStatusMask & GNSS_SV_POLY_SRC_ALM_CORR_V02) {
        pbGnssSvPolyStatMask |= PB_GNSS_SV_POLY_SRC_ALM_CORR_V02;
    }
    if (gnssSvPolyStatusMask & GNSS_SV_POLY_GLO_STR4_V02) {
        pbGnssSvPolyStatMask |= PB_GNSS_SV_POLY_GLO_STR4_V02;
    }
    if (gnssSvPolyStatusMask & GNSS_SV_POLY_DELETE_V02) {
        pbGnssSvPolyStatMask |= PB_GNSS_SV_POLY_DELETE_V02;
    }
    if (gnssSvPolyStatusMask & GNSS_SV_POLY_SRC_GAL_FNAV_OR_INAV_V02) {
        pbGnssSvPolyStatMask |= PB_GNSS_SV_POLY_SRC_GAL_FNAV_OR_INAV_V02;
    }
    return pbGnssSvPolyStatMask;
}

// UlpGnssSvPolyValidFlags to PBUlpGnssSvPolyValidFlags
uint32_t EngineHubMsgConverter::getPBMaskForUlpGnssSvPolyValidFlags(
        const uint32_t &ulpGnssSvPolyValidFlags) {
    uint32_t pbUlpGnssSvPolyValidFlags = PB_ULP_GNSS_SV_POLY_BIT_INVALID;
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GLO_FREQ) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GLO_FREQ;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_T0) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_T0;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_IODE) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_IODE;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_FLAG) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_FLAG;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZ0) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZ0;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZN) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZN;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_OTHER) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_POLYCOEFF_OTHER;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_SV_POSUNC) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_SV_POSUNC;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_IONODELAY) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_IONODELAY;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_IONODOT) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_IONODOT;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_SBAS_IONODELAY) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_SBAS_IONODELAY;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_SBAS_IONODOT) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_SBAS_IONODOT;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_TROPODELAY) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_TROPODELAY;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_ELEVATION) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_ELEVATION;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_ELEVATIONDOT) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_ELEVATIONDOT;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_ELEVATIONUNC) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_ELEVATIONUNC;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_VELO_COEFF) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_VELO_COEFF;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_ENHANCED_IOD) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_ENHANCED_IOD;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L1CA) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GPS_ISC_L1CA;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L2C) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GPS_ISC_L2C;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L5I5) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GPS_ISC_L5I5;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L5Q5) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GPS_ISC_L5Q5;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GPS_TGD) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GPS_TGD;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GLO_TGD_G1G2) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GLO_TGD_G1G2;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_BDS_TGD_B1) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_BDS_TGD_B1;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_BDS_TGD_B2) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_BDS_TGD_B2;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_BDS_TGD_B2A) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_BDS_TGD_B2A;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_BDS_ISC_B2A) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_BDS_ISC_B2A;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GAL_BGD_E1E5A) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GAL_BGD_E1E5A;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_GAL_BGD_E1E5B) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_GAL_BGD_E1E5B;
    }
    if (ulpGnssSvPolyValidFlags & ULP_GNSS_SV_POLY_BIT_NAVIC_TGD_L5) {
        pbUlpGnssSvPolyValidFlags |= PB_ULP_GNSS_SV_POLY_BIT_NAVIC_TGD_L5;
    }
    return pbUlpGnssSvPolyValidFlags;
}

uint32_t EngineHubMsgConverter::getPBMaskForLocPosTechMask(const uint32_t &locPosTechMask) {
    uint32_t  pbLocPosTechMask = PB_LOC_POS_TECH_MASK_DEFAULT;
    if (locPosTechMask & LOC_POS_TECH_MASK_DEFAULT) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_DEFAULT;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_SATELLITE) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_SATELLITE;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_CELLID) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_CELLID;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_WIFI) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_WIFI;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_SENSORS) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_SENSORS;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_REFERENCE_LOCATION) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_REFERENCE_LOCATION;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_AFLT) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_AFLT;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_HYBRID) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_HYBRID;
    }
    if (locPosTechMask & LOC_POS_TECH_MASK_PPE) {
        pbLocPosTechMask |= PB_LOC_POS_TECH_MASK_PPE;
    }
    return pbLocPosTechMask;
}


uint32_t EngineHubMsgConverter::getPBMaskForLocGpsSpoofMask(const uint32_t &locGpsSpoofMask) {
    uint32_t  pbLocGpsSpoofMask = PB_LOC_GPS_LOCATION_NONE_SPOOFED;
    if (locGpsSpoofMask & LOC_GPS_LOCATION_NONE_SPOOFED) {
        pbLocGpsSpoofMask |= PB_LOC_GPS_LOCATION_NONE_SPOOFED;
    }
    if (locGpsSpoofMask & LOC_GPS_LOCATION_POSITION_SPOOFED) {
        pbLocGpsSpoofMask |= PB_LOC_GPS_LOCATION_POSITION_SPOOFED;
    }
    if (locGpsSpoofMask & LOC_GPS_LOCATION_TIME_SPOOFED) {
        pbLocGpsSpoofMask |= PB_LOC_GPS_LOCATION_TIME_SPOOFED;
    }
    if (locGpsSpoofMask & LOC_GPS_LOCATION_NAVIGATION_DATA_SPOOFED) {
        pbLocGpsSpoofMask |= PB_LOC_GPS_LOCATION_NAVIGATION_DATA_SPOOFED;
    }
    return pbLocGpsSpoofMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForLocGpsLocationFlags(const uint32_t &locGpsLocFlags) {
    uint32_t pbLocGpsLocFlags = PB_LOC_GPS_LOCATION_FLAGS_INVALID;
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_ALTITUDE) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_ALTITUDE;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_SPEED) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_SPEED;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_BEARING) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_BEARING;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_ACCURACY) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_ACCURACY;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_SPOOF_MASK) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_SPOOF_MASK;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_SPEED_ACCURACY) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_SPEED_ACCURACY;
    }
    if (locGpsLocFlags & LOC_GPS_LOCATION_HAS_BEARING_ACCURACY) {
        pbLocGpsLocFlags |= PB_LOC_GPS_LOCATION_HAS_BEARING_ACCURACY;
    }
    return pbLocGpsLocFlags;
}


uint32_t EngineHubMsgConverter::getPBMaskForLocNavSolutionMask(const uint32_t &locNavSolnMask) {
    uint32_t pbLocNavSolnMask = PB_LOC_NAV_MASK_INVALID;
    if (locNavSolnMask & LOC_NAV_MASK_SBAS_CORRECTION_IONO) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_SBAS_CORRECTION_IONO;
    }
    if (locNavSolnMask & LOC_NAV_MASK_SBAS_CORRECTION_FAST) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_SBAS_CORRECTION_FAST;
    }
    if (locNavSolnMask & LOC_NAV_MASK_SBAS_CORRECTION_LONG) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_SBAS_CORRECTION_LONG;
    }
    if (locNavSolnMask & LOC_NAV_MASK_SBAS_INTEGRITY) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_SBAS_INTEGRITY;
    }
    if (locNavSolnMask & LOC_NAV_MASK_DGNSS_CORRECTION) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_DGNSS_CORRECTION;
    }
    if (locNavSolnMask & LOC_NAV_MASK_RTK_CORRECTION) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_RTK_CORRECTION;
    }
    if (locNavSolnMask & LOC_NAV_MASK_PPP_CORRECTION) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_PPP_CORRECTION;
    }
    if (locNavSolnMask & LOC_NAV_MASK_RTK_FIXED_CORRECTION) {
        pbLocNavSolnMask |= PB_LOC_NAV_MASK_RTK_FIXED_CORRECTION;
    }
    return pbLocNavSolnMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForDrCalibrationStatusMask(
        const uint32_t &drCalibStatMask) {
    uint32_t pbDrCalibStatMask = PB_DR_CALIBRATION_INVALID;
    if (drCalibStatMask & DR_ROLL_CALIBRATION_NEEDED) {
        pbDrCalibStatMask |= PB_DR_ROLL_CALIBRATION_NEEDED;
    }
    if (drCalibStatMask & DR_PITCH_CALIBRATION_NEEDED) {
        pbDrCalibStatMask |= PB_DR_PITCH_CALIBRATION_NEEDED;
    }
    if (drCalibStatMask & DR_YAW_CALIBRATION_NEEDED) {
        pbDrCalibStatMask |= PB_DR_YAW_CALIBRATION_NEEDED;
    }
    if (drCalibStatMask & DR_ODO_CALIBRATION_NEEDED) {
        pbDrCalibStatMask |= PB_DR_ODO_CALIBRATION_NEEDED;
    }
    if (drCalibStatMask & DR_GYRO_CALIBRATION_NEEDED) {
        pbDrCalibStatMask |= PB_DR_GYRO_CALIBRATION_NEEDED;
    }
    if (drCalibStatMask & DR_TURN_CALIBRATION_LOW) {
        pbDrCalibStatMask |= PB_DR_TURN_CALIBRATION_LOW;
    }
    if (drCalibStatMask & DR_TURN_CALIBRATION_MEDIUM) {
        pbDrCalibStatMask |= PB_DR_TURN_CALIBRATION_MEDIUM;
    }
    if (drCalibStatMask & DR_TURN_CALIBRATION_HIGH) {
        pbDrCalibStatMask |= PB_DR_TURN_CALIBRATION_HIGH;
    }
    if (drCalibStatMask & DR_LINEAR_ACCEL_CALIBRATION_LOW) {
        pbDrCalibStatMask |= PB_DR_LINEAR_ACCEL_CALIBRATION_LOW;
    }
    if (drCalibStatMask & DR_LINEAR_ACCEL_CALIBRATION_MEDIUM) {
        pbDrCalibStatMask |= PB_DR_LINEAR_ACCEL_CALIBRATION_MEDIUM;
    }
    if (drCalibStatMask & DR_LINEAR_ACCEL_CALIBRATION_HIGH) {
        pbDrCalibStatMask |= PB_DR_LINEAR_ACCEL_CALIBRATION_HIGH;
    }
    if (drCalibStatMask & DR_LINEAR_MOTION_CALIBRATION_LOW) {
        pbDrCalibStatMask |= PB_DR_LINEAR_MOTION_CALIBRATION_LOW;
    }
    if (drCalibStatMask & DR_LINEAR_MOTION_CALIBRATION_MEDIUM) {
        pbDrCalibStatMask |= PB_DR_LINEAR_MOTION_CALIBRATION_MEDIUM;
    }
    if (drCalibStatMask & DR_LINEAR_MOTION_CALIBRATION_HIGH) {
        pbDrCalibStatMask |= PB_DR_LINEAR_MOTION_CALIBRATION_HIGH;
    }
    if (drCalibStatMask & DR_STATIC_CALIBRATION_LOW) {
        pbDrCalibStatMask |= PB_DR_STATIC_CALIBRATION_LOW;
    }
    if (drCalibStatMask & DR_STATIC_CALIBRATION_MEDIUM) {
        pbDrCalibStatMask |= PB_DR_STATIC_CALIBRATION_MEDIUM;
    }
    if (drCalibStatMask & DR_STATIC_CALIBRATION_HIGH) {
        pbDrCalibStatMask |= PB_DR_STATIC_CALIBRATION_HIGH;
    }
    return pbDrCalibStatMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForSolutionStatusMask(
        const uint32_t &solStatusMask) {
    uint32_t pbSolStatusMask = PB_VEHICLE_DRSOLUTION_INVALID;
    if (solStatusMask & VEHICLE_SENSOR_SPEED_INPUT_DETECTED) {
        pbSolStatusMask |= PB_VEHICLE_SENSOR_SPEED_INPUT_DETECTED;
    }
    if (solStatusMask & VEHICLE_SENSOR_SPEED_INPUT_USED) {
        pbSolStatusMask |= PB_VEHICLE_SENSOR_SPEED_INPUT_USED;
    }
    if (solStatusMask & DRE_WARNING_UNCALIBRATED) {
        pbSolStatusMask |= PB_DRE_WARNING_UNCALIBRATED;
    }
    if (solStatusMask & DRE_WARNING_GNSS_QUALITY_INSUFFICIENT) {
        pbSolStatusMask |= PB_DRE_WARNING_GNSS_QUALITY_INSUFFICIENT;
    }
    if (solStatusMask & DRE_WARNING_FERRY_DETECTED) {
        pbSolStatusMask |= PB_DRE_WARNING_FERRY_DETECTED;
    }
    if (solStatusMask & DRE_ERROR_6DOF_SENSOR_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_6DOF_SENSOR_UNAVAILABLE;
    }
    if (solStatusMask & DRE_ERROR_VEHICLE_SPEED_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_VEHICLE_SPEED_UNAVAILABLE;
    }
    if (solStatusMask & DRE_ERROR_GNSS_EPH_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_GNSS_EPH_UNAVAILABLE;
    }
    if (solStatusMask & DRE_ERROR_GNSS_MEAS_UNAVAILABLE) {
       pbSolStatusMask |= PB_DRE_ERROR_GNSS_MEAS_UNAVAILABLE;
    }
    if (solStatusMask & DRE_WARNING_INIT_POSITION_INVALID) {
       pbSolStatusMask |= PB_DRE_WARNING_INIT_POSITION_INVALID;
    }
    if (solStatusMask & DRE_WARNING_INIT_POSITION_UNRELIABLE) {
       pbSolStatusMask |= PB_DRE_WARNING_INIT_POSITION_UNRELIABLE;
    }
    if (solStatusMask & DRE_WARNING_POSITON_UNRELIABLE) {
       pbSolStatusMask |= PB_DRE_WARNING_POSITON_UNRELIABLE;
    }
    if (solStatusMask & DRE_ERROR_GENERIC) {
       pbSolStatusMask |= PB_DRE_ERROR_GENERIC;
    }
    if (solStatusMask & DRE_WARNING_SENSOR_TEMP_OUT_OF_RANGE) {
       pbSolStatusMask |= PB_DRE_WARNING_SENSOR_TEMP_OUT_OF_RANGE;
    }
    if (solStatusMask & DRE_WARNING_USER_DYNAMICS_INSUFFICIENT) {
       pbSolStatusMask |= PB_DRE_WARNING_USER_DYNAMICS_INSUFFICIENT;
    }
    if (solStatusMask & DRE_WARNING_FACTORY_DATA_INCONSISTENT) {
       pbSolStatusMask |= PB_DRE_WARNING_FACTORY_DATA_INCONSISTENT;
    }
    return pbSolStatusMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForPositioningEngineMask(const uint32_t &posEngMask) {
    uint32_t pbPosEngMask = PB_POS_ENG_MASK_INVALID;
    if (posEngMask & STANDARD_POSITIONING_ENGINE) {
        pbPosEngMask |= PB_STANDARD_POSITIONING_ENGINE;
    }
    if (posEngMask & DEAD_RECKONING_ENGINE) {
        pbPosEngMask |= PB_DEAD_RECKONING_ENGINE;
    }
    if (posEngMask & PRECISE_POSITIONING_ENGINE) {
        pbPosEngMask |= PB_PRECISE_POSITIONING_ENGINE;
    }
    return pbPosEngMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssMeasUsageInfoValidityMask(
        const uint32_t &gnssMeasUsageInfoValidMask) {
    uint32_t pbGnssMeasUsageInfoValidMask = PB_GNSS_MEAS_USAGE_INFO_INVALID;
    if (gnssMeasUsageInfoValidMask & GNSS_PSEUDO_RANGE_RESIDUAL_VALID) {
        pbGnssMeasUsageInfoValidMask |= PB_GNSS_PSEUDO_RANGE_RESIDUAL_VALID;
    }
    if (gnssMeasUsageInfoValidMask & GNSS_DOPPLER_RESIDUAL_VALID) {
        pbGnssMeasUsageInfoValidMask |= PB_GNSS_DOPPLER_RESIDUAL_VALID;
    }
    if (gnssMeasUsageInfoValidMask & GNSS_CARRIER_PHASE_RESIDUAL_VALID) {
        pbGnssMeasUsageInfoValidMask |= PB_GNSS_CARRIER_PHASE_RESIDUAL_VALID;
    }
    if (gnssMeasUsageInfoValidMask & GNSS_CARRIER_PHASE_AMBIGUITY_TYPE_VALID) {
        pbGnssMeasUsageInfoValidMask |= PB_GNSS_CARRIER_PHASE_AMBIGUITY_TYPE_VALID;
    }
    return pbGnssMeasUsageInfoValidMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssMeasUsageStatusBitMask(
        const uint32_t &gnssMeasUsageStatusBitMask) {
    uint32_t pbGnssMeasUsageStatusBitMask = PB_GNSS_MEAS_USAGE_STATUS_INVALID;
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USED_IN_PVT) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USED_IN_PVT;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_BAD_MEAS) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_BAD_MEAS;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_CNO_TOO_LOW) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_CNO_TOO_LOW;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_ELEVATION_TOO_LOW) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_ELEVATION_TOO_LOW;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_NO_EPHEMERIS) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_NO_EPHEMERIS;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_NO_CORRECTIONS) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_NO_CORRECTIONS;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_CORRECTION_TIMEOUT) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_CORRECTION_TIMEOUT;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_UNHEALTHY) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_UNHEALTHY;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_CONFIG_DISABLED) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_CONFIG_DISABLED;
    }
    if (gnssMeasUsageStatusBitMask & GNSS_MEAS_USAGE_STATUS_OTHER) {
        pbGnssMeasUsageStatusBitMask |= PB_GNSS_MEAS_USAGE_STATUS_OTHER;
    }
    return pbGnssMeasUsageStatusBitMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssLocationPosDataMask(
        const uint32_t &gnssLocPosDataMask,
        const uint32_t &gnssLocPosDataMaskExt) {
    uint32_t pbGnssLocPosDataMask= PB_LOCATION_NAV_DATA_INVALID;
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_YAW_RATE_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_PITCH_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
    }
    if (gnssLocPosDataMask & LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_ROLL_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_YAW_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_BIT;
    }
    if (gnssLocPosDataMaskExt & LOCATION_NAV_DATA_HAS_YAW_UNC_BIT) {
        pbGnssLocPosDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
    }
    return pbGnssLocPosDataMask;
}

uint64_t EngineHubMsgConverter::getPBMaskForGpsLocationExtendedFlags(
        const uint64_t &gpsLocExtFlags) {
    uint64_t pbGpsLocExtFlags = PB_GPS_LOCATION_EXTENDED_LOWER32_INVALID;
    // Lower 32
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_DOP) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_DOP;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_MAG_DEV) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_MAG_DEV;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_MODE_IND) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_MODE_IND;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_VERT_UNC) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_VERT_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_SPEED_UNC) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_SPEED_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_BEARING_UNC) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_BEARING_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_HOR_RELIABILITY) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_RELIABILITY;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_VERT_RELIABILITY) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_VERT_RELIABILITY;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_SV_SOURCE_INFO) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_SV_SOURCE_INFO;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_GPS_TIME) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_GPS_TIME;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_EXT_DOP) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_NORTH_VEL) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_NORTH_VEL;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_EAST_VEL) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_EAST_VEL;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_UP_VEL) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_UP_VEL;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS_STD_DEV) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS_STD_DEV;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT_STD_DEV) {
        pbGpsLocExtFlags |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT_STD_DEV;
    }
    // Upper 32
    uint64_t pbGpsLocExtFlagsUpper32 = PB_GPS_LOCATION_EXTENDED_UPPER32_INVALID;
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_LEAP_SECONDS) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_LEAP_SECONDS;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_TIME_UNC) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_TIME_UNC;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_HEADING_RATE) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_HEADING_RATE;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_CALIBRATION_CONFIDENCE) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_CONFIDENCE;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_MULTIBAND) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_MULTIBAND;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_TYPE) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_TYPE;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_MASK) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_MASK;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_TYPE) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_TYPE;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_ID) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_ID;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_DGNSS_CONSTELLATION_USAGE) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CONSTELLATION_USAGE;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_DGNSS_REF_STATION_ID) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_REF_STATION_ID;
    }
    if (gpsLocExtFlags & GPS_LOCATION_EXTENDED_HAS_DGNSS_DATA_AGE) {
        pbGpsLocExtFlagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_DATA_AGE;
    }
    // Shift pbGpsLocExtFlagsUpper32 right by 31
    pbGpsLocExtFlagsUpper32 = pbGpsLocExtFlagsUpper32<<31;
    // Now mask this with pbGpsLocExtFlags to get full set of flags in uint64
    pbGpsLocExtFlags |= pbGpsLocExtFlagsUpper32;
    return pbGpsLocExtFlags;
}

uint64_t EngineHubMsgConverter::getPBMaskForGpsSvMeasHeaderFlags(
        const uint64_t &gpsSvMeasHdrFlags) {
    uint64_t pbGpsSvMeasHdrFlags = PB_GNSS_SV_MEAS_HEADER_HAS_INVALID;
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_LEAP_SECOND) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_LEAP_SECOND;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_CLOCK_FREQ) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_CLOCK_FREQ;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_AP_TIMESTAMP) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_AP_TIMESTAMP;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPS_GLO_INTER_SYSTEM_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPS_GLO_INTER_SYSTEM_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPS_BDS_INTER_SYSTEM_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPS_BDS_INTER_SYSTEM_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPS_GAL_INTER_SYSTEM_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPS_GAL_INTER_SYSTEM_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_BDS_GLO_INTER_SYSTEM_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_BDS_GLO_INTER_SYSTEM_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GAL_GLO_INTER_SYSTEM_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GAL_GLO_INTER_SYSTEM_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GAL_BDS_INTER_SYSTEM_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GAL_BDS_INTER_SYSTEM_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPSL1L5_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPSL1L5_TIME_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GALE1E5A_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GALE1E5A_TIME_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_BDSB1IB2A_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_BDSB1IB2A_TIME_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME_EXT) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME_EXT;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME_EXT) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME_EXT;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME_EXT) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME_EXT;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME_EXT) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME_EXT;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME_EXT) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME_EXT;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_REF_COUNT_TICKS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_REF_COUNT_TICKS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_TYPE) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_TYPE;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_ID) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_ID;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_DGNSS_REF_STATION_ID) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_DGNSS_REF_STATION_ID;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GPSL1L2C_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GPSL1L2C_TIME_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GLOG1G2_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GLOG1G2_TIME_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_BDSB1IB1C_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_BDSB1IB1C_TIME_BIAS;
    }
    if (gpsSvMeasHdrFlags & GNSS_SV_MEAS_HEADER_HAS_GALE1E5B_TIME_BIAS) {
        pbGpsSvMeasHdrFlags |= PB_GNSS_SV_MEAS_HEADER_HAS_GALE1E5B_TIME_BIAS;
    }
    return pbGpsSvMeasHdrFlags;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssLocInterSystemBiasValidMaskType(
            const uint32_t &gnssLocInterSysBiasValid) {
    uint32_t pbGnssLocInterSysBiasValid = PB_GNSS_SYS_TIME_UNKNOWN;
    if (gnssLocInterSysBiasValid & GNSS_LOC_SYS_TIME_BIAS_VALID) {
        pbGnssLocInterSysBiasValid |= PB_GNSS_SYS_TIME_BIAS_VALID;
    }
    if (gnssLocInterSysBiasValid & GNSS_LOC_SYS_TIME_BIAS_UNC_VALID) {
        pbGnssLocInterSysBiasValid |= PB_GNSS_SYS_TIME_BIAS_UNC_VALID;
    }
    return pbGnssLocInterSysBiasValid;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssAidingDataSvMask(
        const uint32_t &gnssAidDataSvMask) {
    uint32_t pbGnssAidDataSvMask = PB_GNSS_AIDING_DATA_SV_MASK_INVALID;
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_EPHEMERIS_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_EPHEMERIS_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_ALMANAC_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_ALMANAC_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_HEALTH_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_HEALTH_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_DIRECTION_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_DIRECTION_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_STEER_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_STEER_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_ALMANAC_CORR_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_ALMANAC_CORR_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_BLACKLIST_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_BLACKLIST_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_SA_DATA_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_SA_DATA_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_NO_EXIST_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_NO_EXIST_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_IONOSPHERE_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_IONOSPHERE_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_TIME_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_TIME_BIT;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_MB_DATA) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_MB_DATA;
    }
    if (gnssAidDataSvMask & GNSS_AIDING_DATA_SV_POLY_BIT) {
        pbGnssAidDataSvMask |= PB_GNSS_AIDING_DATA_SV_POLY_BIT;
    }
    return pbGnssAidDataSvMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssAidingDataSvTypeMask(
        const uint32_t &gnssAidDataSvTypeMask) {
    uint32_t pbGnssAidDataSvTypeMask = PB_GNSS_AIDING_DATA_SV_TYPE_MASK_INVALID;
    if (gnssAidDataSvTypeMask & GNSS_AIDING_DATA_SV_TYPE_GPS_BIT) {
        pbGnssAidDataSvTypeMask |= PB_GNSS_AIDING_DATA_SV_TYPE_GPS_BIT;
    }
    if (gnssAidDataSvTypeMask & GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT) {
        pbGnssAidDataSvTypeMask |= PB_GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT;
    }
    if (gnssAidDataSvTypeMask & GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT) {
        pbGnssAidDataSvTypeMask |= PB_GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT;
    }
    if (gnssAidDataSvTypeMask & GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT) {
        pbGnssAidDataSvTypeMask |= PB_GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT;
    }
    return pbGnssAidDataSvTypeMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssAidingDataCommonMask(
        const uint32_t &gnssAidDataCommonMask) {
    uint32_t pbGnssAidDataCommonMask = PB_GNSS_AIDING_DATA_COMMON_MASK_INVALID;
    if (gnssAidDataCommonMask & GNSS_AIDING_DATA_COMMON_POSITION_BIT) {
        pbGnssAidDataCommonMask |= PB_GNSS_AIDING_DATA_COMMON_POSITION_BIT;
    }
    if (gnssAidDataCommonMask & GNSS_AIDING_DATA_COMMON_TIME_BIT) {
        pbGnssAidDataCommonMask |= PB_GNSS_AIDING_DATA_COMMON_TIME_BIT;
    }
    if (gnssAidDataCommonMask & GNSS_AIDING_DATA_COMMON_UTC_BIT) {
        pbGnssAidDataCommonMask |= PB_GNSS_AIDING_DATA_COMMON_UTC_BIT;
    }
    if (gnssAidDataCommonMask & GNSS_AIDING_DATA_COMMON_RTI_BIT) {
        pbGnssAidDataCommonMask |= PB_GNSS_AIDING_DATA_COMMON_RTI_BIT;
    }
    if (gnssAidDataCommonMask & GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT) {
        pbGnssAidDataCommonMask |= PB_GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT;
    }
    if (gnssAidDataCommonMask & GNSS_AIDING_DATA_COMMON_CELLDB_BIT) {
        pbGnssAidDataCommonMask |= PB_GNSS_AIDING_DATA_COMMON_CELLDB_BIT;
    }
    return pbGnssAidDataCommonMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForeHubRegReportMask(const uint32_t &ehRegReportMask) {
    uint32_t pbEHubRegRprtMask = EH_REG_REPORT_MASK_UNKNOWN;
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_POSITION) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_POSITION;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_NHZ_POSITION) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_NHZ_POSITION;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_UNPROPAGATED_POSITION) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_UNPROPAGATED_POSITION;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_SV) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_SV;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_SV_POLYNOMIAL) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_SV_POLYNOMIAL;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_SV_MEAS) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_SV_MEAS;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_SV_EPHEMERIS) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_SV_EPHEMERIS;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_NHZ_SV_MEAS) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_NHZ_SV_MEAS;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_KLOBUCHARIONO_MODEL) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_KLOBUCHARIONO_MODEL;
    }
    if (ehRegReportMask & E_HUB_REG_REPORT_MASK_GLONASS_ADDITIONAL_PARAMS) {
        pbEHubRegRprtMask |= EH_REG_REPORT_MASK_GLONASS_ADDITIONAL_PARAMS;
    }
    return pbEHubRegRprtMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForeHubRegStatusMask(const uint32_t &ehRegStatusMask) {
    uint32_t pbEHubRegStatMask = EH_REG_STATUS_MASK_UNKNOWN;
    if (ehRegStatusMask & E_HUB_REG_STATUS_MASK_CONNECTIVITY) {
        pbEHubRegStatMask |= EH_REG_STATUS_MASK_CONNECTIVITY;
    }
    if (ehRegStatusMask & E_HUB_REG_STATUS_MASK_LEVER_ARM_CONFIG) {
        pbEHubRegStatMask |= EH_REG_STATUS_MASK_LEVER_ARM_CONFIG;
    }
    if (ehRegStatusMask & E_HUB_REG_STATUS_MASK_BODY_TO_SENSOR_MOUNT_PARAMS) {
        pbEHubRegStatMask |= EH_REG_STATUS_MASK_BODY_TO_SENSOR_MOUNT_PARAMS;
    }
    return pbEHubRegStatMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForeHubRegReportCorrectionMask(
        const uint32_t &ehRegReportCrrnMask) {
    uint32_t pbEhRegReportCrrnMask = EH_REG_REPORT_MASK_CDFW_QDGNSS_UNKNOWN;
    if (ehRegReportCrrnMask & E_HUB_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION) {
        pbEhRegReportCrrnMask |= EH_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION;
    }
    if (ehRegReportCrrnMask & E_HUB_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION) {
        pbEhRegReportCrrnMask |= EH_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION;
    }
    return pbEhRegReportCrrnMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssConstellationTypeMask(
        const uint32_t &gnssConstlMask) {
    uint32_t pbGnssConstlInfoMask = 0;
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_GPS_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_GPS;
    }
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_GLONASS_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_GLONASS;
    }
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_QZSS_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_QZSS;
    }
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_BEIDOU_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_BEIDOU;
    }
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_GALILEO_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_GALILEO;
    }
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_SBAS_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_SBAS;
    }
    if (gnssConstlMask & GNSS_CONSTELLATION_TYPE_NAVIC_BIT) {
        pbGnssConstlInfoMask |= PB_GNSS_CONSTELLATION_TYPE_NAVIC;
    }
    return pbGnssConstlInfoMask;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssSystemTimeStructTypeFlags(
        const uint32_t &gnssSysTimeStruct) {
    uint32_t pbGnssSysTimeStruct = PB_GNSS_SYSTEM_TIME_FLAG_INVALID;
    if (gnssSysTimeStruct & GNSS_SYSTEM_TIME_WEEK_VALID) {
        pbGnssSysTimeStruct |= PB_GNSS_SYSTEM_TIME_WEEK_VALID;
    }
    if (gnssSysTimeStruct & GNSS_SYSTEM_TIME_WEEK_MS_VALID) {
        pbGnssSysTimeStruct |= PB_GNSS_SYSTEM_TIME_WEEK_MS_VALID;
    }
    if (gnssSysTimeStruct & GNSS_SYSTEM_CLK_TIME_BIAS_VALID) {
        pbGnssSysTimeStruct |= PB_GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
    }
    if (gnssSysTimeStruct & GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID) {
        pbGnssSysTimeStruct |= PB_GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
    }
    if (gnssSysTimeStruct & GNSS_SYSTEM_REF_FCOUNT_VALID) {
        pbGnssSysTimeStruct |= PB_GNSS_SYSTEM_REF_FCOUNT_VALID;
    }
    if (gnssSysTimeStruct & GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID) {
        pbGnssSysTimeStruct |= PB_GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
    }
    return pbGnssSysTimeStruct;
}

uint32_t EngineHubMsgConverter::getPBMaskForGnssGloTimeStructTypeFlags(
        const uint32_t &gloSysTimeStruct) {
    uint32_t pbGloSysTimeStruct = PB_GNSS_CLO_TIME_UNKNOWN;
    if (gloSysTimeStruct & GNSS_CLO_DAYS_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_CLO_DAYS_VALID;
    }
    if (gloSysTimeStruct & GNSS_GLO_MSEC_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_GLO_MSEC_VALID;
    }
    if (gloSysTimeStruct & GNSS_GLO_CLK_TIME_BIAS_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_GLO_CLK_TIME_BIAS_VALID;
    }
    if (gloSysTimeStruct & GNSS_GLO_CLK_TIME_BIAS_UNC_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
    }
    if (gloSysTimeStruct & GNSS_GLO_REF_FCOUNT_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_GLO_REF_FCOUNT_VALID;
    }
    if (gloSysTimeStruct & GNSS_GLO_NUM_CLOCK_RESETS_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_GLO_NUM_CLOCK_RESETS_VALID;
    }
    if (gloSysTimeStruct & GNSS_GLO_FOUR_YEAR_VALID) {
        pbGloSysTimeStruct |= PB_GNSS_GLO_FOUR_YEAR_VALID;
    }
    return pbGloSysTimeStruct;
}

uint32_t EngineHubMsgConverter::getPBMaskForLeverArmTypeMask(const uint32_t &leverArmTypeMask) {
    uint32_t pbLeverArmTypeMask = PB_LEVER_ARM_TYPE_INVALID;
    if (leverArmTypeMask & LEVER_ARM_TYPE_GNSS_TO_VRP_BIT) {
        pbLeverArmTypeMask |= PB_LEVER_ARM_TYPE_GNSS_TO_VRP_BIT;
    }
    if (leverArmTypeMask & LEVER_ARM_TYPE_DR_IMU_TO_GNSS_BIT) {
        pbLeverArmTypeMask |= PB_LEVER_ARM_TYPE_DR_IMU_TO_GNSS_BIT;
    }
    if (leverArmTypeMask & LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS_BIT) {
        pbLeverArmTypeMask |= PB_LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS_BIT;
    }
    return pbLeverArmTypeMask;
}

// LocSvDgnssMeasStatusMask to PBLocSvDgnssMeasStatusMask
uint32_t EngineHubMsgConverter::getPBMaskForLocSvDgnssMeasStatusMask(
        const uint32_t &svDgnssMeasStatMask) {
    uint32_t pbSvDgnssMeasStatMask = PB_LOC_MASK_DGNSS_INVALID;
    if (svDgnssMeasStatMask & LOC_MASK_DGNSS_EPOCH_TIME_VALID) {
        pbSvDgnssMeasStatMask |= PB_LOC_MASK_DGNSS_EPOCH_TIME_VALID;
    }
    if (svDgnssMeasStatMask & LOC_MASK_DGNSS_MEAS_STATUS_PR_VALID) {
        pbSvDgnssMeasStatMask |= PB_LOC_MASK_DGNSS_MEAS_STATUS_PR_VALID;
    }
    if (svDgnssMeasStatMask & LOC_MASK_DGNSS_MEAS_STATUS_PRR_VALID) {
        pbSvDgnssMeasStatMask |= PB_LOC_MASK_DGNSS_MEAS_STATUS_PRR_VALID;
    }
    return pbSvDgnssMeasStatMask;
}

#ifdef FEATURE_CDFW
// QDGnss  mask conversion to protobuff masks
// DGnssSignalStatusFlags to PBDGnssSignalStatusFlags
uint32_t EngineHubMsgConverter::getPBMaskForDGnssSignalStatusFlags(
        const uint32_t &dGnssSignStatFlags) {
    uint32_t pbDGnssSignStatFlags = PB_DGNSS_SIGNAL_STATUS_INVALID;
    if (dGnssSignStatFlags & MEASUREMENT_COMPLETE) {
        pbDGnssSignStatFlags |= PB_MEASUREMENT_COMPLETE;
    }
    if (dGnssSignStatFlags & PSEUDORANGE_VALID) {
        pbDGnssSignStatFlags |= PB_PSEUDORANGE_VALID;
    }
    if (dGnssSignStatFlags & CARRIER_PHASE_VALID) {
        pbDGnssSignStatFlags |= PB_CARRIER_PHASE_VALID;
    }
    if (dGnssSignStatFlags & PHASE_RANGE_RATE_VALID) {
        pbDGnssSignStatFlags |= PB_PHASE_RANGE_RATE_VALID;
    }
    if (dGnssSignStatFlags & HALF_CYCLE_AMBIGUITY_VALID) {
        pbDGnssSignStatFlags |= PB_HALF_CYCLE_AMBIGUITY_VALID;
    }
    if (dGnssSignStatFlags & HALF_CYCLE_AMBIGUITY_UNKNOWN) {
        pbDGnssSignStatFlags |= PB_HALF_CYCLE_AMBIGUITY_UNKNOWN;
    }
    return pbDGnssSignStatFlags;
}

// DGnssConstellationBitMask to PBDGnssConstellationBitMask
uint32_t EngineHubMsgConverter::getPBMaskForDGnssConstellationBitMask(
        const uint32_t &dGnssConstMask) {
    uint32_t pbDGnssConstMask = PB_DGNSS_CONSTELLATION_BIT_INVALID;
    if (dGnssConstMask & DGNSS_CONSTELLATION_GPS_BIT) {
        pbDGnssConstMask |= PB_DGNSS_CONSTELLATION_GPS_BIT;
    }
    if (dGnssConstMask & DGNSS_CONSTELLATION_SBAS_BIT) {
        pbDGnssConstMask |= PB_DGNSS_CONSTELLATION_SBAS_BIT;
    }
    if (dGnssConstMask & DGNSS_CONSTELLATION_QZSS_BIT) {
        pbDGnssConstMask |= PB_DGNSS_CONSTELLATION_QZSS_BIT;
    }
    if (dGnssConstMask & DGNSS_CONSTELLATION_GALILEO_BIT) {
        pbDGnssConstMask |= PB_DGNSS_CONSTELLATION_GALILEO_BIT;
    }
    if (dGnssConstMask & DGNSS_CONSTELLATION_GLONASS_BIT) {
        pbDGnssConstMask |= PB_DGNSS_CONSTELLATION_GLONASS_BIT;
    }
    if (dGnssConstMask & DGNSS_CONSTELLATION_BDS_BIT) {
        pbDGnssConstMask |= PB_DGNSS_CONSTELLATION_BDS_BIT;
    }
    return pbDGnssConstMask;
}
#endif //#ifdef FEATURE_CDFW

int EngineHubMsgConverter::convertGnssInterSysBiasToPB(
        const Gnss_InterSystemBiasStructType &gnssIST,
        PBGnssInterSystemBias* pPBIST) {
    if (nullptr == pPBIST) {
        LOC_LOGe("pPBIST is NULL!, return");
        return 1;
    }
    //uint32         validMask = 1;
    /* Validity mask as per PBLocInterSystemBiasValidMaskType */
    pPBIST->set_validmask(getPBMaskForGnssLocInterSystemBiasValidMaskType(gnssIST.validMask));
    //float           timeBias = 2;
    pPBIST->set_timebias(gnssIST.timeBias);
    // float           timeBiasUnc = 3;
    pPBIST->set_timebiasunc(gnssIST.timeBiasUnc);
    return 0;
}

int EngineHubMsgConverter::convertGnssEphCommonToPB(const GnssEphCommon &gpsEphComn,
        PBGnssEphCommon *pbGnssEph) {
    if (nullptr == pbGnssEph) {
        LOC_LOGe("pbGnssEph is NULL!, return");
        return 1;
    }
    // uint32 gnssSvId = 1;
    pbGnssEph->set_gnsssvid(gpsEphComn.gnssSvId);
    // PBGnssEphAction updateAction = 2;
    pbGnssEph->set_updateaction(getPBEnumForGnssEphAction(gpsEphComn.updateAction));
    // uint32 IODE = 3;
    // float aSqrt = 4;
    // float deltaN = 5;
    // float m0 = 6;
    // float eccentricity = 7;
    // float omega0 = 8;
    // float i0 = 9;
    // float omega = 10;
    // float omegaDot = 11;
    // float iDot = 12;
    pbGnssEph->set_iode(gpsEphComn.IODE);
    pbGnssEph->set_asqrt(gpsEphComn.aSqrt);
    pbGnssEph->set_deltan(gpsEphComn.deltaN);
    pbGnssEph->set_m0(gpsEphComn.m0);
    pbGnssEph->set_eccentricity(gpsEphComn.eccentricity);
    pbGnssEph->set_omega0(gpsEphComn.omega0);
    pbGnssEph->set_i0(gpsEphComn.i0);
    pbGnssEph->set_omega(gpsEphComn.omega);
    pbGnssEph->set_omegadot(gpsEphComn.omegaDot);
    pbGnssEph->set_idot(gpsEphComn.iDot);
    // float cUc = 13;
    // float cUs = 14;
    // float cRc = 15;
    // float cRs = 16;
    // float cIc = 17;
    // float cIs = 18;
    pbGnssEph->set_cuc(gpsEphComn.cUc);
    pbGnssEph->set_cus(gpsEphComn.cUs);
    pbGnssEph->set_crc(gpsEphComn.cRc);
    pbGnssEph->set_crs(gpsEphComn.cRs);
    pbGnssEph->set_cic(gpsEphComn.cIc);
    pbGnssEph->set_cis(gpsEphComn.cIs);
    // uint32 toe = 19;
    // uint32 toc = 20;
    pbGnssEph->set_toe(gpsEphComn.toe);
    pbGnssEph->set_toc(gpsEphComn.toc);
    // float af0 = 21;
    // float af1 = 22;
    // float af2 = 23;
    pbGnssEph->set_af0(gpsEphComn.af0);
    pbGnssEph->set_af1(gpsEphComn.af1);
    pbGnssEph->set_af2(gpsEphComn.af2);
    return 0;
}

int EngineHubMsgConverter::convertGpsEphemerisToPB(const GpsEphemeris &gpsEph,
        PBGpsEphemeris *pbGpsEph) {
    if (nullptr == pbGpsEph) {
        LOC_LOGe("pbGpsEph is NULL!, return");
        return 1;
    }
    // uint32 signalHealth = 2;
    // uint32 URAI = 3;
    // uint32 codeL2 = 4;
    // uint32 dataFlagL2P = 5;
    // float tgd = 6;
    // uint32 fitInterval = 7;
    // uint32 IODC = 8;
    pbGpsEph->set_signalhealth(gpsEph.signalHealth);
    pbGpsEph->set_urai(gpsEph.URAI);
    pbGpsEph->set_codel2(gpsEph.codeL2);
    pbGpsEph->set_dataflagl2p(gpsEph.dataFlagL2P);
    pbGpsEph->set_tgd(gpsEph.tgd);
    pbGpsEph->set_fitinterval(gpsEph.fitInterval);
    pbGpsEph->set_iodc(gpsEph.IODC);

    // PBGnssEphCommon commonEphemerisData = 1;
    PBGnssEphCommon *pbGnssEphCommon = pbGpsEph->mutable_commonephemerisdata();
    if (nullptr != pbGnssEphCommon) {
        if (convertGnssEphCommonToPB(gpsEph.commonEphemerisData, pbGnssEphCommon)) {
            LOC_LOGe("convertGnssEphCommonToPB failed");
        }
    } else {
        LOC_LOGe("mutable_commonephemerisdata is NULL");
    }
    return 0;
}

int EngineHubMsgConverter::convertGpsEphRespToPB(const GpsEphemerisResponse &gpsEphRsp,
            PBGpsEphemerisResponse *pbGpsEphRsp) {
    if (nullptr == pbGpsEphRsp) {
        LOC_LOGe("pbGpsEphRsp is NULL!, return");
        return 1;
    }
    // uint32 numOfEphemeris = 1;
    pbGpsEphRsp->set_numofephemeris(gpsEphRsp.numOfEphemeris);
    // repeated PBGpsEphemeris gpsEphemerisData = 2;
    for (uint16_t i = 0; i < gpsEphRsp.numOfEphemeris; i++) {
        PBGpsEphemeris *gpsEphem = pbGpsEphRsp->add_gpsephemerisdata();
        if (nullptr != gpsEphem) {
            if (convertGpsEphemerisToPB(gpsEphRsp.gpsEphemerisData[i], gpsEphem)) {
                LOC_LOGe("convertGnssEphCommonToPB failed");
            }
        } else {
            LOC_LOGe("add_gpsephemerisdata is NULL");
        }
    }
    return 0;
}

int EngineHubMsgConverter::convertGlonassEphemerisToPB(const GlonassEphemeris &gloEph,
        PBGlonassEphemeris *pbGloEph) {
    if (nullptr == pbGloEph) {
        LOC_LOGe("pbGloEph is NULL!, return");
        return 1;
    }
    // uint32 gnssSvId = 1;
    // PBGnssEphAction updateAction = 2;
    pbGloEph->set_gnsssvid(gloEph.gnssSvId);
    pbGloEph->set_updateaction(getPBEnumForGnssEphAction(gloEph.updateAction));
    // uint32 bnHealth = 3;
    // uint32 lnHealth = 4;
    pbGloEph->set_bnhealth(gloEph.bnHealth);
    pbGloEph->set_lnhealth(gloEph.lnHealth);
    // uint32 tb = 5;
    // uint32 ft = 6;
    // uint32 gloM = 7;
    // uint32 enAge = 8;
    // uint32 gloFrequency = 9;
    // uint32 p1 = 10;
    // uint32 p2 = 11;
    // float deltaTau = 12;
    pbGloEph->set_tb(gloEph.tb);
    pbGloEph->set_ft(gloEph.ft);
    pbGloEph->set_glom(gloEph.gloM);
    pbGloEph->set_enage(gloEph.enAge);
    pbGloEph->set_glofrequency(gloEph.gloFrequency);
    pbGloEph->set_p1(gloEph.p1);
    pbGloEph->set_p2(gloEph.p2);
    pbGloEph->set_deltatau(gloEph.deltaTau);

    int i=0;
    // repeated float position = 13;
    for (i = 0; i < 3; i++) {
        pbGloEph->add_position(gloEph.position[i]);
    }
    // repeated float velocity = 14;
    for (i = 0; i < 3; i++) {
        pbGloEph->add_velocity(gloEph.velocity[i]);
    }
    // repeated float acceleration = 15;
    for (i = 0; i < 3; i++) {
        pbGloEph->add_acceleration(gloEph.acceleration[i]);
    }
    // float tauN = 16;
    // float gamma = 17;
    // float toe = 18;
    // uint32 nt = 19;
    pbGloEph->set_taun(gloEph.tauN);
    pbGloEph->set_gamma(gloEph.gamma);
    pbGloEph->set_toe(gloEph.toe);
    pbGloEph->set_nt(gloEph.nt);
    return 0;
}

int EngineHubMsgConverter::convertGlonassEphRespToPB(const GlonassEphemerisResponse &gloEphRsp,
        PBGlonassEphemerisResponse *pbGloEphRsp) {
    if (nullptr == pbGloEphRsp) {
        LOC_LOGe("pbGloEphRsp is NULL!, return");
        return 1;
    }
    // uint32 numOfEphemeris = 1;
    pbGloEphRsp->set_numofephemeris(gloEphRsp.numOfEphemeris);
    // repeated PBGlonassEphemeris gloEphemerisData = 2;
    for (uint16_t i = 0; i < gloEphRsp.numOfEphemeris; i++) {
        PBGlonassEphemeris *gloEphem = pbGloEphRsp->add_gloephemerisdata();
        if (nullptr != gloEphem) {
            if (convertGlonassEphemerisToPB(gloEphRsp.gloEphemerisData[i], gloEphem)) {
                LOC_LOGe("convertGlonassEphemerisToPB failed");
            } else {
                LOC_LOGe("add_gloephemerisdata is NULL");
            }
        }
    }
    return 0;
}

int EngineHubMsgConverter::convertBdsEphemerisToPB(const BdsEphemeris &bdsEph,
        PBBdsEphemeris *pbBdsEph) {
    if (nullptr == pbBdsEph) {
        LOC_LOGe("pbBdsEph is NULL!, return");
        return 1;
    }
    // uint32 svHealth = 2;
    // uint32 AODC = 3;
    // float tgd1 = 4;
    // float tgd2 = 5;
    // uint32 URAI = 6;
    pbBdsEph->set_svhealth(bdsEph.svHealth);
    pbBdsEph->set_aodc(bdsEph.AODC);
    pbBdsEph->set_tgd1(bdsEph.tgd1);
    pbBdsEph->set_tgd2(bdsEph.tgd2);
    pbBdsEph->set_urai(bdsEph.URAI);
    // PBGnssEphCommon commonEphemerisData = 1;
    PBGnssEphCommon *pbGnssEphCommon = pbBdsEph->mutable_commonephemerisdata();
    if (nullptr != pbGnssEphCommon) {
        if (convertGnssEphCommonToPB(bdsEph.commonEphemerisData, pbGnssEphCommon)) {
            LOC_LOGe("convertGnssEphCommonToPB failed");
        }
    } else {
        LOC_LOGe("mutable_commonephemerisdata failed");
    }
    return 0;
}

int EngineHubMsgConverter::convertBdsEphRespToPB(const BdsEphemerisResponse &bdsEphRsp,
        PBBdsEphemerisResponse *pbBdsEphRsp) {
    if (nullptr == pbBdsEphRsp) {
        LOC_LOGe("pbBdsEphRsp is NULL!, return");
        return 1;
    }
    // uint32 numOfEphemeris = 1;
    pbBdsEphRsp->set_numofephemeris(bdsEphRsp.numOfEphemeris);
    // repeated PBBdsEphemeris bdsEphemerisData = 2;
    for (uint16_t i = 0; i < bdsEphRsp.numOfEphemeris; i++) {
        PBBdsEphemeris *bdsEphem = pbBdsEphRsp->add_bdsephemerisdata();
        if (nullptr != bdsEphem) {
            if (convertBdsEphemerisToPB(bdsEphRsp.bdsEphemerisData[i], bdsEphem)) {
                LOC_LOGe("convertGlonassEphemerisToPB failed");
            }
        } else {
            LOC_LOGe("add_bdsephemerisdata is NULL");
        }
    }
    return 0;
}

int EngineHubMsgConverter::convertGalEphemerisToPB(const GalileoEphemeris &galEph,
        PBGalileoEphemeris *pbGalEph) {
    if (nullptr == pbGalEph) {
        LOC_LOGe("pbGalEph is NULL!, return");
        return 1;
    }
    // PBGalEphSignalSource dataSourceSignal = 2;
    pbGalEph->set_datasourcesignal(getPBEnumForGalEphSignalSource(galEph.dataSourceSignal));
    // uint32 sisIndex = 3;
    // float bgdE1E5a = 4;
    // float bgdE1E5b = 5;
    // uint32 svHealth = 6;
    pbGalEph->set_sisindex(galEph.sisIndex);
    pbGalEph->set_bgde1e5a(galEph.bgdE1E5a);
    pbGalEph->set_bgde1e5b(galEph.bgdE1E5b);
    pbGalEph->set_svhealth(galEph.svHealth);
    // PBGnssEphCommon commonEphemerisData = 1;
    PBGnssEphCommon *pbGnssEphCommon = pbGalEph->mutable_commonephemerisdata();
    if (nullptr != pbGnssEphCommon) {
        if (convertGnssEphCommonToPB(galEph.commonEphemerisData, pbGnssEphCommon)) {
            LOC_LOGe("convertGnssEphCommonToPB failed");
        }
    } else {
        LOC_LOGe("mutable_commonephemerisdata failed");
    }
    return 0;
}

int EngineHubMsgConverter::convertGalileoEphRespToPB(const GalileoEphemerisResponse &galEphRsp,
        PBGalileoEphemerisResponse *pbGalEphRsp) {
    if (nullptr == pbGalEphRsp) {
        LOC_LOGe("pbGalEphRsp is NULL!, return");
        return 1;
    }
    // uint32 numOfEphemeris = 1;
    pbGalEphRsp->set_numofephemeris(galEphRsp.numOfEphemeris);
    // repeated PBGalileoEphemeris galEphemerisData = 2;
    for (uint16_t i = 0; i < galEphRsp.numOfEphemeris; i++) {
        PBGalileoEphemeris *galEphem = pbGalEphRsp->add_galephemerisdata();
        if (nullptr != galEphem) {
            if (convertGalEphemerisToPB(galEphRsp.galEphemerisData[i], galEphem)) {
                LOC_LOGe("convertGlonassEphemerisToPB failed");
            }
        } else {
            LOC_LOGe("add_galephemerisdata is NULL");
        }
    }
    return 0;
}

int EngineHubMsgConverter::convertQzssEphRespToPB(const QzssEphemerisResponse &qzssEphRsp,
        PBQzssEphemerisResponse *pbQzssEphRsp) {
    if (nullptr == pbQzssEphRsp) {
        LOC_LOGe("pbQzssEphRsp is NULL!, return");
        return 1;
    }
    // uint32 numOfEphemeris = 1;
    pbQzssEphRsp->set_numofephemeris(qzssEphRsp.numOfEphemeris);
    // repeated PBGpsEphemeris qzssEphemerisData = 2;
    for (uint16_t i = 0; i < qzssEphRsp.numOfEphemeris; i++) {
        PBGpsEphemeris *qzssEphem = pbQzssEphRsp->add_qzssephemerisdata();
        if (nullptr != qzssEphem) {
            // QzssEphemerisResponse has GpsEphemeris array
            if (convertGpsEphemerisToPB(qzssEphRsp.qzssEphemerisData[i], qzssEphem)) {
                LOC_LOGe("convertGnssEphCommonToPB failed");
            }
        } else {
            LOC_LOGe("add_qzssephemerisdata is NULL");
        }
    }
    return 0;
}

int EngineHubMsgConverter::convertGnssSvUsedInPositionToPB(
        const GnssSvUsedInPosition &gnssSvUsedInFix,
        PBGnssSvUsedInPosition *pbGnssSvUsedInFix) {
    if (nullptr == pbGnssSvUsedInFix) {
        LOC_LOGe("pbGnssSvUsedInFix is NULL!, return");
        return 1;
    }
    // uint64 gps_sv_used_ids_mask = 1;
    // uint64 glo_sv_used_ids_mask = 2;
    // uint64 gal_sv_used_ids_mask = 3;
    // uint64 bds_sv_used_ids_mask = 4;
    // uint64 qzss_sv_used_ids_mask = 5;
    pbGnssSvUsedInFix->set_gps_sv_used_ids_mask(gnssSvUsedInFix.gps_sv_used_ids_mask);
    pbGnssSvUsedInFix->set_glo_sv_used_ids_mask(gnssSvUsedInFix.glo_sv_used_ids_mask);
    pbGnssSvUsedInFix->set_gal_sv_used_ids_mask(gnssSvUsedInFix.gal_sv_used_ids_mask);
    pbGnssSvUsedInFix->set_bds_sv_used_ids_mask(gnssSvUsedInFix.bds_sv_used_ids_mask);
    pbGnssSvUsedInFix->set_qzss_sv_used_ids_mask(gnssSvUsedInFix.qzss_sv_used_ids_mask);
    return 0;
}

int EngineHubMsgConverter::convertGnssSvMbUsedInPositionToPB(
        const GnssSvMbUsedInPosition  &gnssSvMbUsedInFix,
        PBGnssSvMbUsedInPosition *pbGnssSvMbUsedInFix) {
    if (nullptr == pbGnssSvMbUsedInFix) {
        LOC_LOGe("pbGnssSvMbUsedInFix is NULL!, return");
        return 1;
    }
    // uint64 gps_l1ca_sv_used_ids_mask = 1;
    // uint64 gps_l1c_sv_used_ids_mask = 2;
    // uint64 gps_l2_sv_used_ids_mask = 3;
    // uint64 gps_l5_sv_used_ids_mask = 4;
    pbGnssSvMbUsedInFix->set_gps_l1ca_sv_used_ids_mask(
            gnssSvMbUsedInFix.gps_l1ca_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_gps_l1c_sv_used_ids_mask(gnssSvMbUsedInFix.gps_l1c_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_gps_l2_sv_used_ids_mask(gnssSvMbUsedInFix.gps_l2_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_gps_l5_sv_used_ids_mask(gnssSvMbUsedInFix.gps_l5_sv_used_ids_mask);

    // uint64 glo_g1_sv_used_ids_mask = 5;
    // uint64 glo_g2_sv_used_ids_mask = 6;
    pbGnssSvMbUsedInFix->set_glo_g1_sv_used_ids_mask(gnssSvMbUsedInFix.glo_g1_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_glo_g2_sv_used_ids_mask(gnssSvMbUsedInFix.glo_g2_sv_used_ids_mask);

    // uint64 gal_e1_sv_used_ids_mask = 7;
    // uint64 gal_e5a_sv_used_ids_mask = 8;
    // uint64 gal_e5b_sv_used_ids_mask = 9;
    pbGnssSvMbUsedInFix->set_gal_e1_sv_used_ids_mask(gnssSvMbUsedInFix.gal_e1_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_gal_e5a_sv_used_ids_mask(gnssSvMbUsedInFix.gal_e5a_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_gal_e5b_sv_used_ids_mask(gnssSvMbUsedInFix.gal_e5b_sv_used_ids_mask);

    // uint64 bds_b1i_sv_used_ids_mask = 10;
    // uint64 bds_b1c_sv_used_ids_mask = 11;
    // uint64 bds_b2i_sv_used_ids_mask = 12;
    // uint64 bds_b2ai_sv_used_ids_mask = 13;
    pbGnssSvMbUsedInFix->set_bds_b1i_sv_used_ids_mask(gnssSvMbUsedInFix.bds_b1i_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_bds_b1c_sv_used_ids_mask(gnssSvMbUsedInFix.bds_b1c_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_bds_b2i_sv_used_ids_mask(gnssSvMbUsedInFix.bds_b2i_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_bds_b2ai_sv_used_ids_mask(
            gnssSvMbUsedInFix.bds_b2ai_sv_used_ids_mask);

    // uint64 qzss_l1ca_sv_used_ids_mask = 14;
    // uint64 qzss_l1s_sv_used_ids_mask = 15;
    // uint64 qzss_l2_sv_used_ids_mask = 16;
    // uint64 qzss_l5_sv_used_ids_mask = 17;
    pbGnssSvMbUsedInFix->set_qzss_l1ca_sv_used_ids_mask(
            gnssSvMbUsedInFix.qzss_l1ca_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_qzss_l1s_sv_used_ids_mask(
            gnssSvMbUsedInFix.qzss_l1s_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_qzss_l2_sv_used_ids_mask(gnssSvMbUsedInFix.qzss_l2_sv_used_ids_mask);
    pbGnssSvMbUsedInFix->set_qzss_l5_sv_used_ids_mask(gnssSvMbUsedInFix.qzss_l5_sv_used_ids_mask);

    // uint64 sbas_l1_sv_used_ids_mask = 18;
    pbGnssSvMbUsedInFix->set_sbas_l1_sv_used_ids_mask(gnssSvMbUsedInFix.sbas_l1_sv_used_ids_mask);

    // uint64 bds_b2aq_sv_used_ids_mask = 19;
    pbGnssSvMbUsedInFix->set_bds_b2aq_sv_used_ids_mask(
            gnssSvMbUsedInFix.bds_b2aq_sv_used_ids_mask);
    return 0;
}

int EngineHubMsgConverter::convertGnssLocationPositionDynamicsToPB(
            const GnssLocationPositionDynamics  &gnssLocPosDynmcs,
            const GnssLocationPositionDynamicsExt &gnssLocPosDynmcsExt,
            PBGnssLocationPositionDynamics *pbGnssLocPosDynmcs) {
    if (nullptr == pbGnssLocPosDynmcs) {
        LOC_LOGe("pbGnssLocPosDynmcs is NULL!, return");
        return 1;
    }
    // uint32 bodyFrameDataMask = 1;
    pbGnssLocPosDynmcs->set_bodyframedatamask(
            getPBMaskForGnssLocationPosDataMask(gnssLocPosDynmcs.bodyFrameDataMask,
            gnssLocPosDynmcsExt.bodyFrameDataMask));
    // float longAccel = 2;
    // float latAccel = 3;
    // float vertAccel = 4;
    pbGnssLocPosDynmcs->set_longaccel(gnssLocPosDynmcs.longAccel);
    pbGnssLocPosDynmcs->set_lataccel(gnssLocPosDynmcs.latAccel);
    pbGnssLocPosDynmcs->set_vertaccel(gnssLocPosDynmcs.vertAccel);
    // float yawRate = 5;
    // float pitch = 6;
    pbGnssLocPosDynmcs->set_yawrate(gnssLocPosDynmcs.yawRate);
    pbGnssLocPosDynmcs->set_pitch(gnssLocPosDynmcs.pitch);
    // float longAccelUnc = 7;
    // float latAccelUnc = 8;
    // float vertAccelUnc = 9;
    pbGnssLocPosDynmcs->set_longaccelunc(gnssLocPosDynmcs.longAccelUnc);
    pbGnssLocPosDynmcs->set_lataccelunc(gnssLocPosDynmcs.latAccelUnc);
    pbGnssLocPosDynmcs->set_vertaccelunc(gnssLocPosDynmcs.vertAccelUnc);
    // float yawRateUnc = 10;
    // float pitchUnc = 11;
    pbGnssLocPosDynmcs->set_yawrateunc(gnssLocPosDynmcs.yawRateUnc);
    pbGnssLocPosDynmcs->set_pitchunc(gnssLocPosDynmcs.pitchUnc);
    //float pitchRate = 12;
    //float pitchRateUnc = 13;
    pbGnssLocPosDynmcs->set_pitchrate(gnssLocPosDynmcsExt.pitchRate);
    pbGnssLocPosDynmcs->set_pitchrateunc(gnssLocPosDynmcsExt.pitchRateUnc);
    //float roll = 14;
    //float rollUnc = 15;
    pbGnssLocPosDynmcs->set_roll(gnssLocPosDynmcsExt.roll);
    pbGnssLocPosDynmcs->set_rollunc(gnssLocPosDynmcsExt.rollUnc);
    //float rollRate = 16;
    //float rollRateUnc = 17;
    pbGnssLocPosDynmcs->set_rollrate(gnssLocPosDynmcsExt.rollRate);
    pbGnssLocPosDynmcs->set_rollrateunc(gnssLocPosDynmcsExt.rollRateUnc);
    //float yaw = 18;
    //float yawUnc = 19;
    pbGnssLocPosDynmcs->set_yaw(gnssLocPosDynmcsExt.yaw);
    pbGnssLocPosDynmcs->set_yawunc(gnssLocPosDynmcsExt.yawUnc);
    return 0;
}

int EngineHubMsgConverter::convertGnssSystemTimeToPB(const GnssSystemTime &gnssSysTime,
            PBGnssSystemTime *pbGnssSysTime) {
    if (nullptr == pbGnssSysTime) {
        LOC_LOGe("pbGnssSysTime is NULL!, return");
        return 1;
    }
    // PBGnss_LocSvSystemEnumType gnssSystemTimeSrc = 1;
    pbGnssSysTime->set_gnsssystemtimesrc(
            getPBEnumForGnssLocSvSystem(gnssSysTime.gnssSystemTimeSrc));
    // PBSystemTimeStructUnion u = 2;
    PBSystemTimeStructUnion *sysStructUnion = pbGnssSysTime->mutable_u();
    if (nullptr != sysStructUnion) {
        if (GNSS_LOC_SV_SYSTEM_GPS == gnssSysTime.gnssSystemTimeSrc) {
            // >> PBGnssSystemTimeStructType gpsSystemTime = 1;
            PBGnssSystemTimeStructType *pPbGpsSystemTime = sysStructUnion->mutable_gpssystemtime();
            if (nullptr != pPbGpsSystemTime) {
                convertGnssSystemTimeStructToPB(gnssSysTime.u.gpsSystemTime, pPbGpsSystemTime);
            } else {
                LOC_LOGe("mutable_gpssystemtime failed");
            }
        } else if (GNSS_LOC_SV_SYSTEM_GALILEO == gnssSysTime.gnssSystemTimeSrc) {
            // >> PBGnssSystemTimeStructType galSystemTime = 2;
            PBGnssSystemTimeStructType *pPbGalSystemTime = sysStructUnion->mutable_galsystemtime();
            if (nullptr != pPbGalSystemTime) {
                convertGnssSystemTimeStructToPB(gnssSysTime.u.galSystemTime, pPbGalSystemTime);
            } else {
                LOC_LOGe("mutable_galsystemtime failed");
            }
        } else if (GNSS_LOC_SV_SYSTEM_GLONASS == gnssSysTime.gnssSystemTimeSrc) {
            // >> PBGnssGloTimeStructType    gloSystemTime = 5;
            PBGnssGloTimeStructType *pPbGloSystemTime = sysStructUnion->mutable_glosystemtime();
            if (nullptr != pPbGloSystemTime) {
                convertGnssGloTimeInfoToPB(gnssSysTime.u.gloSystemTime, pPbGloSystemTime);
            } else {
                LOC_LOGe("mutable_glosystemtime failed");
            }
        } else if (GNSS_LOC_SV_SYSTEM_BDS == gnssSysTime.gnssSystemTimeSrc) {
             // >> PBGnssSystemTimeStructType bdsSystemTime = 3;
             PBGnssSystemTimeStructType *pPbBdsSystemTime =
                    sysStructUnion->mutable_bdssystemtime();
            if (nullptr != pPbBdsSystemTime) {
                convertGnssSystemTimeStructToPB(gnssSysTime.u.bdsSystemTime, pPbBdsSystemTime);
            } else {
                LOC_LOGe("mutable_bdssystemtime failed");
            }
        } else if (GNSS_LOC_SV_SYSTEM_QZSS == gnssSysTime.gnssSystemTimeSrc) {
            // >> PBGnssSystemTimeStructType qzssSystemTime = 4;
            PBGnssSystemTimeStructType *pPbQzssSystemTime =
                    sysStructUnion->mutable_qzsssystemtime();
            if (nullptr != pPbQzssSystemTime) {
                convertGnssSystemTimeStructToPB(gnssSysTime.u.qzssSystemTime, pPbQzssSystemTime);
            } else {
                LOC_LOGe("mutable_qzsssystemtime failed");
            }
        }
    } else {
        LOC_LOGe("mutable_u is NULL");
    }
    return 0;
}

int EngineHubMsgConverter::convertGnssSVMeasurementStructTypeToPB(
        const Gnss_SVMeasurementStructType &svMeas, PBSVMeasurementStructType* svInfo)
{
    if (nullptr == svInfo) {
        LOC_LOGe("svInfo is NULL!, return");
        return 1;
    }
    // PBGnss_LocSvSystemEnumType  gnssSystem = 1;
    svInfo->set_gnsssystem(getPBEnumForGnssLocSvSystem(svMeas.gnssSystem));
    // fixed32  gnssSignalTypeMask = 2;
    svInfo->set_gnsssignaltypemask(
            getPBMaskForGnssSignalTypeMask(svMeas.gnssSignalTypeMask));
    // uint32   gnssSvId = 3;
    svInfo->set_gnsssvid(svMeas.gnssSvId);
    // uint32   gloFrequency = 4;
    svInfo->set_glofrequency(svMeas.gloFrequency);
    // PBSvSearchStatus   svStatus = 5;
    svInfo->set_svstatus(getPBEnumForSvSearchStatus(svMeas.svStatus));
    // bool    healthStatusValid = 6;
    // bool    healthStatus = 7;
    svInfo->set_healthstatusvalid(svMeas.healthStatus_valid);
    svInfo->set_healthstatus(svMeas.healthStatus);
    // uint32   svInfoMask = 8;
    svInfo->set_svinfomask(getPBMaskForGnssLocSvInfoMaskT(svMeas.svInfoMask));
    // uint64   measurementStatus = 9;
    svInfo->set_measurementstatus(
            getPBMaskForGnssLocSvMeasStatus(svMeas.measurementStatus));
    // uint32   CNo = 10;
    // uint32 gloRfLoss = 11;
    // bool    lossOfLock = 12;
    // int32   measLatency = 13;
    svInfo->set_cno(svMeas.CNo);
    svInfo->set_glorfloss(svMeas.gloRfLoss);
    svInfo->set_lossoflock(svMeas.lossOfLock);
    svInfo->set_measlatency(svMeas.measLatency);
    // PBSVTimeSpeedInfo svTimeSpeed = 14;
    PBSVTimeSpeedInfo* pSvTimeSpeed = svInfo->mutable_svtimespeed();
    if (nullptr != pSvTimeSpeed) {
        if (convertGnssLocSVTimeSpeedToPB(svMeas.svTimeSpeed, pSvTimeSpeed)) {
            LOC_LOGe("convertGnssLocSVTimeSpeedToPB failed");
        }
    } else {
        LOC_LOGe("mutable_svtimespeed failed");
    }
    // float dopplerAccel = 15;
    svInfo->set_doppleraccel(svMeas.dopplerAccel);
    // bool    multipathEstValid = 16;
    // float multipathEstimate = 17;
    svInfo->set_multipathestvalid(svMeas.multipathEstValid);
    svInfo->set_multipathestimate(svMeas.multipathEstimate);
    // bool    fineSpeedValid = 18;
    // float fineSpeed = 19;
    // bool    fineSpeedUncValid = 20;
    // float fineSpeedUnc = 21;
    svInfo->set_finespeedvalid(svMeas.fineSpeedValid);
    svInfo->set_finespeed(svMeas.fineSpeed);
    svInfo->set_finespeeduncvalid(svMeas.fineSpeedUncValid);
    svInfo->set_finespeed(svMeas.fineSpeedUnc);
    // bool    carrierPhaseValid = 22;
    // double carrierPhase = 23;
    svInfo->set_carrierphasevalid(svMeas.carrierPhaseValid);
    svInfo->set_carrierphase(svMeas.carrierPhase);
    // bool    cycleSlipCountValid = 24;
    // uint32  cycleSlipCount = 25;
    svInfo->set_cycleslipcountvalid(svMeas.cycleSlipCountValid);
    svInfo->set_cycleslipcount(svMeas.cycleSlipCount);
    // bool    svDirectionValid = 26;
    // float svAzimuth = 27;
    // float svElevation = 28;
    svInfo->set_svdirectionvalid(svMeas.svDirectionValid);
    svInfo->set_svazimuth(svMeas.svAzimuth);
    svInfo->set_svelevation(svMeas.svElevation);
    // uint64   validMeasStatusMask = 29;
    svInfo->set_validmeasstatusmask(svMeas.validMeasStatusMask);
    // bool carrierPhaseUncValid = 30;
    // float carrierPhaseUnc = 31;
    svInfo->set_carrierphaseuncvalid(svMeas.carrierPhaseUncValid);
    svInfo->set_carrierphaseunc(svMeas.carrierPhaseUnc);
    // PBGnssLocDgnssSVMeasurement     dgnssSvMeas = 32;
    PBGnssLocDgnssSVMeasurement *pbLocDgnssSvMeas = svInfo->mutable_dgnsssvmeas();
    if (nullptr != pbLocDgnssSvMeas) {
        if (convertGnssLocDgnssSVMeasurementToPB(svMeas.dgnssSvMeas, pbLocDgnssSvMeas)) {
            LOC_LOGe("convertGnssLocDgnssSVMeasurementToPB failed");
        }
    } else {
        LOC_LOGe("mutable_dgnsssvmeas failed");
    }
    LOC_LOGe("svid: %u Cno: %u  %u ", svInfo->gnsssvid(), svInfo->cno(), svMeas.CNo);
    return 0;
}

int EngineHubMsgConverter::convertGnssAidingDataSvToPB(
        const GnssAidingDataSv &gnssAidingDataSv, PBGnssAidingDataSv* pbGnssAidingDataSv) {
    if (nullptr == pbGnssAidingDataSv) {
        LOC_LOGe("pbGnssAidingDataSv is NULL!, return");
        return 1;
    }
    // >> uint32 svMask = 1; (PBGnssAidingDataSvMask)
    pbGnssAidingDataSv->set_svmask(getPBMaskForGnssAidingDataSvMask(gnssAidingDataSv.svMask));
    // >> uint32 svTypeMask = 2; (PBGnssAidingDataSvTypeMask)
    pbGnssAidingDataSv->set_svtypemask(
            getPBMaskForGnssAidingDataSvTypeMask(gnssAidingDataSv.svTypeMask));
    return 0;
}

int EngineHubMsgConverter::convertGnssAidingDataCommonToPB(
        const GnssAidingDataCommon &gnssAidingDataCommon,
        PBGnssAidingDataCommon* pbGnssAidingDataCommon) {
    if (nullptr == pbGnssAidingDataCommon) {
        LOC_LOGe("pbGnssAidingDataCommon is NULL!, return");
        return 1;
    }
    // uint32 mask = 1; (PBGnssAidingDataCommonMask)
    pbGnssAidingDataCommon->set_mask(
            getPBMaskForGnssAidingDataCommonMask(gnssAidingDataCommon.mask));
    return 0;
}

int EngineHubMsgConverter::convertLocPosModeToPB(const LocPosMode &locPosMode,
        PBLocPosMode* pbLocPosMode) {
    if (nullptr == pbLocPosMode) {
        LOC_LOGe("pbLocPosMode is NULL!, return");
        return 1;
    }
    // >> PBLocPositionMode mode  = 1;
    pbLocPosMode->set_mode(getPBEnumForLocPositionMode(locPosMode.mode));
    // >> PBLocGpsPositionRecurrence recurrence = 2;
    pbLocPosMode->set_recurrence(getPBEnumForLocGpsPositionRecurrence(locPosMode.recurrence));
    // >> uint32 min_interval = 3;
    pbLocPosMode->set_min_interval(locPosMode.min_interval);
    // >> uint32 preferred_accuracy = 4;
    pbLocPosMode->set_preferred_accuracy(locPosMode.preferred_accuracy);
    // >> uint32 preferred_time = 5;
    pbLocPosMode->set_preferred_time(locPosMode.preferred_time);
    // >> bool share_position = 6;
    pbLocPosMode->set_share_position(locPosMode.share_position);
    // >> string credentials = 7;
    pbLocPosMode->set_credentials(locPosMode.credentials);
    // >> string provider = 8;
    pbLocPosMode->set_provider(locPosMode.provider);
    // >> PBGnssPowerMode powerMode = 9;
    pbLocPosMode->set_powermode(getPBEnumForGnssPowerMode(locPosMode.powerMode));
    // >> uint32 timeBetweenMeasurements = 10;
    pbLocPosMode->set_timebetweenmeasurements(locPosMode.timeBetweenMeasurements);
    return 0;
}

int EngineHubMsgConverter::convertEHubRoutingTableEntryToPB(
        const EHubRoutingTableEntry &eHRoutTblEntry, EHRoutingTableEntry* pbEHRoutTblEntry) {
    if (nullptr == pbEHRoutTblEntry) {
        LOC_LOGe("pbEHRoutTblEntry is NULL!, return");
        return 1;
    }
    // >> EHConnectionType engConnectionType = 1;
    pbEHRoutTblEntry->set_engconnectiontype(
            getPBEnumForEngineConnectionType(eHRoutTblEntry.engConnectionType));

    // >> EHEngineConnectionInfo engConnectInfo = 2;
    EHEngineConnectionInfo *pbEhubEngConnInfo = pbEHRoutTblEntry->mutable_engconnectinfo();
    if (nullptr != pbEhubEngConnInfo) {
        if (convertEngineConnectionInfoToPB(eHRoutTblEntry.engConnectInfo,
                eHRoutTblEntry.engConnectionType, pbEhubEngConnInfo)) {
            LOC_LOGe("convertEngineConnectionInfoToPB failed");
        }
    } else {
        LOC_LOGe("mutable_engconnectinfo failed");
    }

    // >> repeated uint32 regReportMask = 3; (EHRegReportMask)
    for (int i = 0; i < E_HUB_TECH_COUNT; i++) {
        pbEHRoutTblEntry->add_regreportmask(
                getPBMaskForeHubRegReportMask(eHRoutTblEntry.regReportMask[i]));
    }

    // >> uint32 regStatusMask = 4 (EHRegStatusMask);
    pbEHRoutTblEntry->set_regstatusmask(
            getPBMaskForeHubRegStatusMask(eHRoutTblEntry.regStatusMask));

    // >> EHSubModifiers  subModifiers = 5;
    EHSubModifiers *pbSubModifiers = pbEHRoutTblEntry->mutable_submodifiers();
    if (nullptr != pbSubModifiers) {
        if (convertEHubSubModifiersToPB(eHRoutTblEntry.subModifiers, pbSubModifiers)) {
            LOC_LOGe("convertEHubSubModifiersToPB failed");
        }
    } else {
        LOC_LOGe("mutable_submodifiers failed");
    }
    // >> EHRegReportCorrectionMask regCdfwCorrnMask = 6;
    pbEHRoutTblEntry->set_regcdfwcorrnmask(
            getPBMaskForeHubRegReportCorrectionMask(eHRoutTblEntry.regCdfwCorrnMask));

    // >> uint32_t regSessionMsgs = 7;
    pbEHRoutTblEntry->set_regsessionmsgs(eHRoutTblEntry.regSessionMsgs);
    return 0;
}

int EngineHubMsgConverter::convertEHubSubModifiersToPB(const eHubSubModifiers &eHSubModifiers,
        EHSubModifiers* pbEHSubModifiers) {
    if (nullptr == pbEHSubModifiers) {
        LOC_LOGe("pbEHSubModifiers is NULL!, return");
        return 1;
    }
    // >> EHSubscriptionModifier  svMeasSubModifier = 1;
    EHSubscriptionModifier *pbSvMeasSubModifier = pbEHSubModifiers->mutable_svmeassubmodifier();
    if (nullptr != pbSvMeasSubModifier) {
        if (convertEHSubscriptionModifierToPB(eHSubModifiers.svMeasSubModifier,
                pbSvMeasSubModifier)) {
            LOC_LOGe("convertEHSubscriptionModifierToPB failed");
        }
    } else {
        LOC_LOGe("mutable_svmeassubmodifier failed");
    }
    // >> EHSubscriptionModifier  nHzSvMeasSubModifier = 2;
    EHSubscriptionModifier *pbNHzSvMeasSubModifier =
            pbEHSubModifiers->mutable_nhzsvmeassubmodifier();
    if (nullptr != pbNHzSvMeasSubModifier) {
        if (convertEHSubscriptionModifierToPB(eHSubModifiers.nHzSvMeasSubModifier,
                pbNHzSvMeasSubModifier)) {
            LOC_LOGe("convertEHSubscriptionModifierToPB failed");
        }
    } else {
        LOC_LOGe("mutable_nhzsvmeassubmodifier failed");
    }
    // >> EHSubscriptionModifier  svPolySubModifier = 3;
    EHSubscriptionModifier *pbsvPolySubModifier = pbEHSubModifiers->mutable_svpolysubmodifier();
    if (nullptr != pbsvPolySubModifier) {
        if (convertEHSubscriptionModifierToPB(eHSubModifiers.svPolySubModifier,
                pbsvPolySubModifier)) {
            LOC_LOGe("convertEHSubscriptionModifierToPB failed");
        }
    } else {
        LOC_LOGe("mutable_svpolysubmodifier failed");
    }
    // >> EHSubscriptionModifier  svEphSubModifier = 4;
    EHSubscriptionModifier *pbsvEphSubModifier = pbEHSubModifiers->mutable_svephsubmodifier();
    if (nullptr != pbsvEphSubModifier) {
        if (convertEHSubscriptionModifierToPB(eHSubModifiers.svEphSubModifier,
                pbsvEphSubModifier)) {
            LOC_LOGe("convertEHSubscriptionModifierToPB failed");
        }
    } else {
        LOC_LOGe("mutable_svephsubmodifier failed");
    }
    // >> EHSubscriptionModifier  ionoModelSubModifier = 5;
    EHSubscriptionModifier *pbionoModelSubModifier =
            pbEHSubModifiers->mutable_ionomodelsubmodifier();
    if (nullptr != pbionoModelSubModifier) {
        if (convertEHSubscriptionModifierToPB(eHSubModifiers.ionoModelSubModifier,
                pbionoModelSubModifier)) {
            LOC_LOGe("convertEHSubscriptionModifierToPB failed");
        }
    } else {
        LOC_LOGe("mutable_ionomodelsubmodifier failed");
    }
    return 0;
}

int EngineHubMsgConverter::convertEngineConnectionInfoToPB(const engineConnectionInfo &engConnInfo,
        EngineConnectionType engConnType, EHEngineConnectionInfo *pbEhEngConnInfo) {
    if (nullptr == pbEhEngConnInfo) {
        LOC_LOGe("pbEhEngConnInfo is NULL!, return");
        return 1;
    }
    // oneof values
    if (ENGINE_CONNECT_TYPE_INET == engConnType) {
        // >> string socketName = 1;
        pbEhEngConnInfo->set_socketname(engConnInfo.socketName);
    } else if (ENGINE_CONNECT_TYPE_LOCAL == engConnType) {
        // >> EHEngineConnectionIpAddrInfo pluginIpAddrInfo = 2;
        EHEngineConnectionIpAddrInfo *pluginIpAddrInfo =
                pbEhEngConnInfo->mutable_pluginipaddrinfo();
        if (nullptr != pluginIpAddrInfo) {
            // >> string pluginIpAddress = 1;
            // >> uint32 pluginPort = 2;
            pluginIpAddrInfo->set_pluginipaddress(engConnInfo.pluginIpAddrInfo.pluginIpAddress);
            pluginIpAddrInfo->set_pluginport(engConnInfo.pluginIpAddrInfo.pluginPort);
        } else {
            LOC_LOGe("mutable_pluginipaddrinfo failed");
        }

    }
    return 0;
}

int EngineHubMsgConverter::convertEHSubscriptionModifierToPB(
        const eHubSubscriptionModifier &ehSubscModifier,
        EHSubscriptionModifier *pbEhSubscModifier) {
    if (nullptr == pbEhSubscModifier) {
        LOC_LOGe("pbEhSubscModifier is NULL!, return");
        return 1;
    }
    // >> uint32 constellationMask = 1; (PBGnssConstellationInfoMask)
    pbEhSubscModifier->set_constellationmask(
            getPBMaskForGnssConstellationTypeMask(ehSubscModifier.constellationMask));
    // >> uint32 gnssSignalMask = 2; (PBGnssSignalInfoMask)
    pbEhSubscModifier->set_gnsssignalmask(
            getPBMaskForGnssSignalTypeMask(ehSubscModifier.gnssSignalMask));
    return 0;
}

int EngineHubMsgConverter::convertLeverArmConfigInfoToPB(
        const LeverArmConfigInfo &leverArmConfgInfo, PBLeverArmConfigInfo *pbLeverArmConfgInfo) {
    if (nullptr == pbLeverArmConfgInfo) {
        LOC_LOGe("pbLeverArmConfgInfo is NULL!, return");
        return 1;
    }
    // >> PBLeverArmTypeMask leverArmValidMask = 1;
    pbLeverArmConfgInfo->set_leverarmvalidmask(
            getPBMaskForLeverArmTypeMask(leverArmConfgInfo.leverArmValidMask));
    // >> PBLeverArmParams   gnssToVRP = 2;
    PBLeverArmParams *pbGnssToVRP = pbLeverArmConfgInfo->mutable_gnsstovrp();
    if (nullptr != pbGnssToVRP) {
        if (convertLeverArmParamsToPB(leverArmConfgInfo.gnssToVRP, pbGnssToVRP)) {
            LOC_LOGe("convertLeverArmParamsToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gnsstovrp failed");
    }
    // >> PBLeverArmParams   drImuToGnss = 3;
    PBLeverArmParams *pbDrImuToGnss = pbLeverArmConfgInfo->mutable_drimutognss();
    if (nullptr != pbDrImuToGnss) {
        if (convertLeverArmParamsToPB(leverArmConfgInfo.gnssToVRP, pbDrImuToGnss)) {
            LOC_LOGe("convertLeverArmParamsToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gnsstovrp failed");
    }
    // >> PBLeverArmParams   veppImuToGnss = 4;
    PBLeverArmParams *pbVeppImuToGnss = pbLeverArmConfgInfo->mutable_veppimutognss();
    if (nullptr != pbVeppImuToGnss) {
        if (convertLeverArmParamsToPB(leverArmConfgInfo.gnssToVRP, pbVeppImuToGnss)) {
            LOC_LOGe("convertLeverArmParamsToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gnsstovrp failed");
    }
    return 0;
}

int EngineHubMsgConverter::convertLeverArmParamsToPB(const LeverArmParams &leverArmParams,
            PBLeverArmParams *pbLeverArmParams) {
    if (nullptr == pbLeverArmParams) {
        LOC_LOGe("pbLeverArmParams is NULL!, return");
        return 1;
    }
    // >> float forwardOffsetMeters = 1;
    pbLeverArmParams->set_forwardoffsetmeters(leverArmParams.forwardOffsetMeters);
    // >> float sidewaysOffsetMeters = 2;
    pbLeverArmParams->set_sidewaysoffsetmeters(leverArmParams.sidewaysOffsetMeters);
    // >> float upOffsetMeters = 3;
    pbLeverArmParams->set_upoffsetmeters(leverArmParams.upOffsetMeters);
    return 0;
}

int EngineHubMsgConverter::convertB2sMountParamsToPB(const BodyToSensorMountParams &b2sMountParams,
            PBBodyToSensorMountParams *pbB2sMountParams) {
    if (nullptr == pbB2sMountParams) {
        LOC_LOGe("pbB2sMountParams is NULL!, return");
        return 1;
    }
    pbB2sMountParams->set_rolloffset(b2sMountParams.rollOffset);
    pbB2sMountParams->set_pitchoffset(b2sMountParams.pitchOffset);
    pbB2sMountParams->set_yawoffset(b2sMountParams.yawOffset);
    pbB2sMountParams->set_offsetunc(b2sMountParams.offsetUnc);
    return 0;
}

int EngineHubMsgConverter::convertGnssLocDgnssSVMeasurementToPB(
        const Gnss_LocDgnssSVMeasurement &locDgnssSvMeas,
        PBGnssLocDgnssSVMeasurement *pbLocDgnssSvMeas) {
    if (nullptr == pbLocDgnssSvMeas) {
        LOC_LOGe("pbLocDgnssSvMeas is NULL!, return");
        return 1;
    }
    // >> uint32 dgnssMeasStatus = 1 (PBLocSvDgnssMeasStatusMask);
    pbLocDgnssSvMeas->set_dgnssmeasstatus(
            getPBMaskForLocSvDgnssMeasStatusMask((const uint32_t)locDgnssSvMeas.dgnssMeasStatus));
    // >> uint32 diffDataEpochTimeMsec = 2;
    pbLocDgnssSvMeas->set_diffdataepochtimemsec(locDgnssSvMeas.diffDataEpochTimeMsec);
    // >> float prCorrMeters = 3;
    pbLocDgnssSvMeas->set_prcorrmeters(locDgnssSvMeas.prCorrMeters);
    // >> float prrCorrMetersPerSec = 4;
    pbLocDgnssSvMeas->set_prrcorrmeterspersec(locDgnssSvMeas.prrCorrMetersPerSec);
    return 0;
}

int EngineHubMsgConverter::convertGnssLocSVTimeSpeedToPB(
        const Gnss_LocSVTimeSpeedStructType &gnssSvTimeSpeed,
        PBSVTimeSpeedInfo* pPbSVTimeSpeed) {
    if (nullptr == pPbSVTimeSpeed) {
        LOC_LOGe("pPbSVTimeSpeed is NULL!, return");
        return 1;
    }
    // uint32  svMs = 1;
    // float  svSubMs = 2;
    // float  svTimeUncMs = 3;
    pPbSVTimeSpeed->set_svms(gnssSvTimeSpeed.svMs);
    pPbSVTimeSpeed->set_svsubms(gnssSvTimeSpeed.svSubMs);
    pPbSVTimeSpeed->set_svtimeuncms(gnssSvTimeSpeed.svTimeUncMs);
    // float  dopplerShift = 4;
    // float  dopplerShiftUnc = 5;
    pPbSVTimeSpeed->set_dopplershift(gnssSvTimeSpeed.dopplerShift);
    pPbSVTimeSpeed->set_dopplershiftunc(gnssSvTimeSpeed.dopplerShiftUnc);
    return 0;
}

int EngineHubMsgConverter::convertGnssSystemTimeStructToPB(
        const GnssSystemTimeStructType &gnssSystemTime,
        PBGnssSystemTimeStructType * pPBGnssSystemTimeInfo) {
    if (nullptr == pPBGnssSystemTimeInfo) {
        LOC_LOGe("pPBGnssSystemTimeInfo is NULL!, return");
        return 1;
    }
    // uint32 validityMask = 1; - PBGnssSystemTimeStructTypeFlags
    pPBGnssSystemTimeInfo->set_validitymask(
            getPBMaskForGnssSystemTimeStructTypeFlags(gnssSystemTime.validityMask));
    // uint32 systemWeek = 2;
    // uint32 systemMsec = 3;
    pPBGnssSystemTimeInfo->set_systemweek(gnssSystemTime.systemWeek);
    pPBGnssSystemTimeInfo->set_systemmsec(gnssSystemTime.systemMsec);
    // float systemClkTimeBias = 4;
    // float systemClkTimeUncMs = 5;
    pPBGnssSystemTimeInfo->set_systemclktimebias(gnssSystemTime.systemClkTimeBias);
    pPBGnssSystemTimeInfo->set_systemclktimeuncms(gnssSystemTime.systemClkTimeUncMs);
    // uint32 refFCount = 6;
    // uint32 numClockResets = 7;
    pPBGnssSystemTimeInfo->set_reffcount(gnssSystemTime.refFCount);
    pPBGnssSystemTimeInfo->set_numclockresets(gnssSystemTime.numClockResets);
    return 0;
}

int EngineHubMsgConverter::convertGnssGloTimeInfoToPB(
        const GnssGloTimeStructType &gloSystemTime,
        PBGnssGloTimeStructType* pPBGnssGloTimeInfo) {
    if (nullptr == pPBGnssGloTimeInfo) {
        LOC_LOGe("pPBGnssGloTimeInfo is NULL!, return");
        return 1;
    }
    // uint32 gloDays = 1;
    pPBGnssGloTimeInfo->set_glodays(gloSystemTime.gloDays);
    // uint32 validityMask = 2; - PBGnssGloTimeInfoBits
    pPBGnssGloTimeInfo->set_validitymask(
            getPBMaskForGnssGloTimeStructTypeFlags(gloSystemTime.validityMask));
    // uint32 gloMsec = 3;
    pPBGnssGloTimeInfo->set_glomsec(gloSystemTime.gloMsec);
    // float gloClkTimeBias = 4;
    // float gloClkTimeUncMs = 5;
    pPBGnssGloTimeInfo->set_gloclktimebias(gloSystemTime.gloClkTimeBias);
    pPBGnssGloTimeInfo->set_gloclktimeuncms(gloSystemTime.gloClkTimeUncMs);
    // uint32  refFCount = 6;
    // uint32 numClockResets = 7;
    // uint32 gloFourYear = 8;
    pPBGnssGloTimeInfo->set_reffcount(gloSystemTime.refFCount);
    pPBGnssGloTimeInfo->set_numclockresets(gloSystemTime.numClockResets);
    pPBGnssGloTimeInfo->set_glofouryear(gloSystemTime.gloFourYear);
    return 0;
}

int EngineHubMsgConverter::convertGnssLocGnssTimeExtInfoToPB(
        const Gnss_LocGnssTimeExtStructType &gnssSystemTimeExt,
        PBLocGnssTimeExtInfo * pPBGnssSystemTimeExtInfo) {
    if (nullptr == pPBGnssSystemTimeExtInfo) {
        LOC_LOGe("pPBGnssSystemTimeExtInfo is NULL!, return");
        return 1;
    }
    // bool   systemRtcValid = 1;
    pPBGnssSystemTimeExtInfo->set_systemrtcvalid(gnssSystemTimeExt.systemRtc_valid);
    // uint64  systemRtcMs = 2;
    pPBGnssSystemTimeExtInfo->set_systemrtcms(gnssSystemTimeExt.systemRtcMs);
    return 0;
}

int EngineHubMsgConverter::convertUlpLocationToPB(const UlpLocation &ulpLoc,
        PBUlpLocation *pbUlpLoc) {
    if (nullptr == pbUlpLoc) {
        LOC_LOGe("pbUlpLoc is NULL!, return");
        return 1;
    }
    // uint32       position_source    = 2;
    pbUlpLoc->set_position_source(ulpLoc.position_source);
    // uint32  tech_mask    = 3; - PBLocPosTechMask
    pbUlpLoc->set_tech_mask(getPBMaskForLocPosTechMask(ulpLoc.tech_mask));
    // bool    unpropagatedPosition    = 4;
    pbUlpLoc->set_unpropagatedposition(ulpLoc.unpropagatedPosition);
    // PBLocGpsLocation    gpsLocation = 1;
    PBLocGpsLocation *pbLocGpsLoc = pbUlpLoc->mutable_gpslocation();
    if (nullptr != pbLocGpsLoc) {
        if (convertLocGpsLocationToPB(ulpLoc.gpsLocation, pbLocGpsLoc)) {
            LOC_LOGe("convertGpsLocExtToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gpslocation failed");
    }
    return 0;
}

int EngineHubMsgConverter::convertLocGpsLocationToPB(const LocGpsLocation &locGpsLoc,
        PBLocGpsLocation *pbLocGpsLoc) {
    if (nullptr == pbLocGpsLoc) {
        LOC_LOGe("pbLocGpsLoc is NULL!, return");
        return 1;
    }
    // uint32    flags = 1; - PBLocGpsLocationFlags
    pbLocGpsLoc->set_flags(getPBMaskForLocGpsLocationFlags(locGpsLoc.flags));
    // uint32    spoof_mask = 2;
    pbLocGpsLoc->set_spoof_mask(getPBMaskForLocGpsSpoofMask(locGpsLoc.spoof_mask));
    // float     latitude = 3;
    // float     longitude = 4;
    // float     altitude = 5;
    pbLocGpsLoc->set_latitude(locGpsLoc.latitude);
    pbLocGpsLoc->set_longitude(locGpsLoc.longitude);
    pbLocGpsLoc->set_altitude(locGpsLoc.altitude);
    // float      speed = 6;
    // float      bearing = 7;
    // float      accuracy = 8;
    // float      vertUncertainity = 9;
    pbLocGpsLoc->set_speed(locGpsLoc.speed);
    pbLocGpsLoc->set_bearing(locGpsLoc.bearing);
    pbLocGpsLoc->set_accuracy(locGpsLoc.accuracy);
    pbLocGpsLoc->set_vertuncertainity(locGpsLoc.vertUncertainity);
    // uint64     timestamp = 10;
    pbLocGpsLoc->set_timestamp(locGpsLoc.timestamp);
    return 0;
}

int EngineHubMsgConverter::convertGpsLocExtToPB(const GpsLocationExtended &gpsLocExt,
        PBGpsLocationExtended *pbGpsLocExt) {
    if (nullptr == pbGpsLocExt) {
        LOC_LOGe("pbGpsLocExt is NULL!, return");
        return 1;
    }
    // uint64  flags = 1; - PBGpsLocationExtendedFlagsUpper32/Lower32
    pbGpsLocExt->set_flags(getPBMaskForGpsLocationExtendedFlags(gpsLocExt.flags));
    // float  altitudeMeanSeaLevel = 2;
    // float  pdop = 3;
    // float  hdop = 4;
    // float  vdop = 5;
    // float  magneticDeviation = 6;
    pbGpsLocExt->set_altitudemeansealevel(gpsLocExt.altitudeMeanSeaLevel);
    pbGpsLocExt->set_pdop(gpsLocExt.pdop);
    pbGpsLocExt->set_hdop(gpsLocExt.hdop);
    pbGpsLocExt->set_vdop(gpsLocExt.vdop);
    pbGpsLocExt->set_magneticdeviation(gpsLocExt.magneticDeviation );
    // float  vert_unc = 7;
    // float  speed_unc = 8;
    // float  bearing_unc = 9;
    pbGpsLocExt->set_vert_unc(gpsLocExt.vert_unc);
    pbGpsLocExt->set_speed_unc(gpsLocExt.speed_unc);
    pbGpsLocExt->set_bearing_unc(gpsLocExt.bearing_unc);
    // PBLocReliability  horizontal_reliability = 10;
    // PBLocReliability  vertical_reliability = 11;
    pbGpsLocExt->set_horizontal_reliability(
            getPBEnumForLocReliability(gpsLocExt.horizontal_reliability));
    pbGpsLocExt->set_vertical_reliability(
            getPBEnumForLocReliability(gpsLocExt.vertical_reliability));
    // float  horUncEllipseSemiMajor = 12;
    // float  horUncEllipseSemiMinor = 13;
    // float  horUncEllipseOrientAzimuth = 14;
    pbGpsLocExt->set_horuncellipsesemimajor(gpsLocExt.horUncEllipseSemiMajor);
    pbGpsLocExt->set_horuncellipsesemiminor(gpsLocExt.horUncEllipseSemiMinor);
    pbGpsLocExt->set_horuncellipseorientazimuth(gpsLocExt.horUncEllipseOrientAzimuth);

    // PBApTimeStampStructType     timeStamp = 15;
    PBApTimeStampStructType *pPbApTimeStamp = pbGpsLocExt->mutable_timestamp();
    if (nullptr != pPbApTimeStamp) {
        PBTimespec *pPbTimespec = pPbApTimeStamp->mutable_aptimestamp();
        if (nullptr != pPbTimespec) {
            pPbTimespec->set_tv_sec(gpsLocExt.timeStamp.apTimeStamp.tv_sec);
            pPbTimespec->set_tv_nsec(gpsLocExt.timeStamp.apTimeStamp.tv_nsec);
        } else {
            LOC_LOGe("mutable_aptimestamp failed");
        }
        pPbApTimeStamp->set_aptimestampuncms(
                gpsLocExt.timeStamp.apTimeStampUncertaintyMs);
    } else {
        LOC_LOGe("mutable_timestamp failed");
    }

    // PBGnssSvUsedInPosition gnss_sv_used_ids = 16;
    PBGnssSvUsedInPosition *pbGnssSvUseInFix = pbGpsLocExt->mutable_gnss_sv_used_ids();
    if (nullptr != pbGnssSvUseInFix) {
        if (convertGnssSvUsedInPositionToPB(gpsLocExt.gnss_sv_used_ids, pbGnssSvUseInFix)) {
            LOC_LOGe("convertGnssSvUsedInPositionToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gnss_sv_used_ids failed");
    }
    // PBGnssSvMbUsedInPosition gnss_mb_sv_used_ids = 17;
    PBGnssSvMbUsedInPosition *pbGnssSvMbUseInFix = pbGpsLocExt->mutable_gnss_mb_sv_used_ids();
    if (nullptr != pbGnssSvMbUseInFix) {
        if (convertGnssSvMbUsedInPositionToPB(gpsLocExt.gnss_mb_sv_used_ids, pbGnssSvMbUseInFix)) {
            LOC_LOGe("convertGnssSvMbUsedInPositionToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gnss_mb_sv_used_ids failed");
    }
    // uint32  navSolutionMask = 18; - PBLocNavSolutionMask
    pbGpsLocExt->set_navsolutionmask(getPBMaskForLocNavSolutionMask(gpsLocExt.navSolutionMask));
    // uint32 tech_mask = 19; - PBLocPosTechMask
    pbGpsLocExt->set_tech_mask(getPBMaskForLocPosTechMask(gpsLocExt.tech_mask));
    // PBLocSvInfoSource sv_source = 20;
    pbGpsLocExt->set_sv_source(
            getPBEnumForLocSvInfoSource(gpsLocExt.sv_source));

    // PBGnssLocationPositionDynamics bodyFrameData = 21;
    PBGnssLocationPositionDynamics *pbGnssLocPosDynmcs = pbGpsLocExt->mutable_bodyframedata();
    if (nullptr != pbGnssLocPosDynmcs) {
        if (convertGnssLocationPositionDynamicsToPB(gpsLocExt.bodyFrameData,
                gpsLocExt.bodyFrameDataExt, pbGnssLocPosDynmcs)) {
            LOC_LOGe("convertGnssLocationPositionDynamicsToPB failed");
        }
    } else {
        LOC_LOGe("mutable_bodyframedata failed");
    }

    // PBGPSTimeStruct gpsTime = 22;
    PBGPSTimeStruct *pPbGpsTimeStruct = pbGpsLocExt->mutable_gpstime();
    if (nullptr != pPbGpsTimeStruct) {
        pPbGpsTimeStruct->set_gpsweek(gpsLocExt.gpsTime.gpsWeek);
        pPbGpsTimeStruct->set_gpstimeofweekms(gpsLocExt.gpsTime.gpsTimeOfWeekMs);
    } else {
        LOC_LOGe("mutable_gpstime failed");
    }

    // PBGnssSystemTime gnssSystemTime = 23;
    PBGnssSystemTime *pbGnssSysTime = pbGpsLocExt->mutable_gnsssystemtime();
    if (nullptr != pbGnssSysTime) {
        if (convertGnssSystemTimeToPB(gpsLocExt.gnssSystemTime, pbGnssSysTime)) {
            LOC_LOGe("convertGnssSystemTimeToPB failed");
        }
    } else {
        LOC_LOGe("mutable_gnsssystemtime failed");
    }

    // PBLocExtDOP extDOP = 24;
    PBLocExtDOP *pbLocExtDop = pbGpsLocExt->mutable_extdop();
    if (nullptr != pbLocExtDop) {
        pbLocExtDop->set_pdop(gpsLocExt.extDOP.PDOP);
        pbLocExtDop->set_hdop(gpsLocExt.extDOP.HDOP);
        pbLocExtDop->set_vdop(gpsLocExt.extDOP.VDOP);
        pbLocExtDop->set_gdop(gpsLocExt.extDOP.GDOP);
        pbLocExtDop->set_tdop(gpsLocExt.extDOP.TDOP);
    } else {
        LOC_LOGe("mutable_extdop failed");
    }

    // float northStdDeviation = 25;
    // float eastStdDeviation = 26;
    pbGpsLocExt->set_northstddeviation(gpsLocExt.northStdDeviation);
    pbGpsLocExt->set_eaststddeviation(gpsLocExt.eastStdDeviation);
    // float northVelocity = 27;
    // float eastVelocity = 28;
    // float upVelocity = 29;
    // float northVelocityStdDeviation = 30;
    // float eastVelocityStdDeviation = 31;
    // float upVelocityStdDeviation = 32;
    pbGpsLocExt->set_northvelocity(gpsLocExt.northVelocity);
    pbGpsLocExt->set_eastvelocity(gpsLocExt.eastVelocity);
    pbGpsLocExt->set_upvelocity(gpsLocExt.upVelocity);
    pbGpsLocExt->set_northvelocitystddeviation(gpsLocExt.northVelocityStdDeviation);
    pbGpsLocExt->set_eastvelocitystddeviation(gpsLocExt.eastVelocityStdDeviation);
    pbGpsLocExt->set_upvelocitystddeviation(gpsLocExt.upVelocityStdDeviation);
    // float clockbiasMeter = 33;
    // float clockBiasStdDeviationMeter = 34;
    // float clockDrift = 35;
    // float clockDriftStdDeviation = 36;
    pbGpsLocExt->set_clockbiasmeter(gpsLocExt.clockbiasMeter);
    pbGpsLocExt->set_clockbiasstddeviationmeter(gpsLocExt.clockBiasStdDeviationMeter);
    pbGpsLocExt->set_clockdrift(gpsLocExt.clockDrift);
    pbGpsLocExt->set_clockdriftstddeviation(gpsLocExt.clockDriftStdDeviation);
    // uint32 numValidRefStations = 37;
    pbGpsLocExt->set_numvalidrefstations(gpsLocExt.numValidRefStations);

    // repeated uint32 referenceStation = 38;
    for (int i = 0; i < 4; i++) {
        pbGpsLocExt->add_referencestation(gpsLocExt.referenceStation[i]);
    }

    // uint32 numOfMeasReceived = 39;
    pbGpsLocExt->set_numofmeasreceived(gpsLocExt.numOfMeasReceived);
    // repeated PBGpsMeasUsageInfo measUsageInfo = 40;
    uint8_t numOfMeasReceived = gpsLocExt.numOfMeasReceived;
    for (uint8_t i = 0; i < numOfMeasReceived; i++) {
        PBGpsMeasUsageInfo *pbGpsMeasUsageInfo = pbGpsLocExt->add_measusageinfo();
        const GpsMeasUsageInfo &measUsageInfo = gpsLocExt.measUsageInfo[i];
        if (nullptr != pbGpsMeasUsageInfo) {
            // >> uint32 gnssSignalType = 1;
            pbGpsMeasUsageInfo->set_gnsssignaltype(
                    getPBMaskForGnssSignalTypeMask(measUsageInfo.gnssSignalType));
            // >> PBGnss_LocSvSystemEnumType gnssConstellation = 2;
            pbGpsMeasUsageInfo->set_gnssconstellation(
                    getPBEnumForGnssLocSvSystem(measUsageInfo.gnssConstellation));
            // >> uint32 gnssSvId = 3;
            pbGpsMeasUsageInfo->set_gnsssvid(measUsageInfo.gnssSvId);
            // >> uint32 gloFrequency = 4;
            // >> float pseudorangeResidual = 8;
            // >> float dopplerResidual = 9;
            // >> float carrierPhaseResidual = 10;
            // >> float carrierPhasAmbiguity = 11;
            pbGpsMeasUsageInfo->set_glofrequency(measUsageInfo.gloFrequency);
            pbGpsMeasUsageInfo->set_pseudorangeresidual(measUsageInfo.pseudorangeResidual);
            pbGpsMeasUsageInfo->set_dopplerresidual(measUsageInfo.dopplerResidual);
            pbGpsMeasUsageInfo->set_carrierphaseresidual(measUsageInfo.carrierPhaseResidual);
            pbGpsMeasUsageInfo->set_carrierphasambiguity(measUsageInfo.carrierPhasAmbiguity);
            // >> uint32 validityMask = 7;
            pbGpsMeasUsageInfo->set_validitymask(
                    getPBMaskForGnssMeasUsageInfoValidityMask(measUsageInfo.validityMask));
            // >> uint32 measUsageStatusMask = 6;
            pbGpsMeasUsageInfo->set_measusagestatusmask(
                    getPBMaskForGnssMeasUsageStatusBitMask(measUsageInfo.measUsageStatusMask));
            // >> PBCarrierPhaseAmbiguityType carrierPhaseAmbiguityType = 5;
            pbGpsMeasUsageInfo->set_carrierphaseambiguitytype(
                    getPBEnumForCarrierPhaseAmbiguityType(
                        measUsageInfo.carrierPhaseAmbiguityType));
            LOC_LOGd("signaltype: %u Constellation: %u  %u ",
                    pbGpsMeasUsageInfo->gnsssignaltype(), pbGpsMeasUsageInfo->gnssconstellation());
        } else {
            LOC_LOGe("add_measusageinfo failed!");
        }
    }
    LOC_LOGd("numOfMeasReceived = %d", numOfMeasReceived);

    // uint32 leapSeconds = 41;
    // float timeUncMs = 42;
    pbGpsLocExt->set_leapseconds(gpsLocExt.leapSeconds);
    pbGpsLocExt->set_timeuncms(gpsLocExt.timeUncMs);
    // float headingRateDeg = 43;
    pbGpsLocExt->set_headingratedeg(gpsLocExt.headingRateDeg);
    // uint32 calibrationConfidence = 44;
    pbGpsLocExt->set_calibrationconfidence(gpsLocExt.calibrationConfidence);
    // uint32 calibrationStatus = 45; - PBDrCalibrationStatusMask
    pbGpsLocExt->set_calibrationstatus(
            getPBMaskForDrCalibrationStatusMask(gpsLocExt.calibrationStatus));
    // PBLocOutputEngineType locOutputEngType = 46;
    pbGpsLocExt->set_locoutputengtype(
            getPBEnumForLocOutputEngineType(gpsLocExt.locOutputEngType));
    // uint32 locOutputEngMask = 47; - PBPositioningEngineMask
    pbGpsLocExt->set_locoutputengmask(
            getPBMaskForPositioningEngineMask(gpsLocExt.locOutputEngMask));

    // PBLocDgnssCorrectionSourceType dgnssCorrectionSourceType = 48;
    pbGpsLocExt->set_dgnsscorrectionsourcetype(
            getPBEnumForLocDgnssCorrectionSourceType(gpsLocExt.dgnssCorrectionSourceType));
    // uint32 dgnssCorrectionSourceID = 49;
    pbGpsLocExt->set_dgnsscorrectionsourceid(gpsLocExt.dgnssCorrectionSourceID);
    // uint32 dgnssConstellationUsageMask = 50; (PBGnssConstellationInfoMask)
    pbGpsLocExt->set_dgnssconstellationusagemask(
            getPBMaskForGnssConstellationTypeMask(gpsLocExt.dgnssConstellationUsage));
    // uint32 dgnssRefStationId = 51;
    pbGpsLocExt->set_dgnssrefstationid(gpsLocExt.dgnssRefStationId);
    // uint32 dgnssDataAgeMsec = 52;
    pbGpsLocExt->set_dgnssdataagemsec(gpsLocExt.dgnssDataAgeMsec);
    // uint32 drSolutionStatusMask = 53;
    pbGpsLocExt->set_solutionstatusmask(getPBMaskForSolutionStatusMask
                                   (gpsLocExt.drSolutionStatusMask));
    return 0;
}

int EngineHubMsgConverter::convertGnssSvMeasToPB(
        const GnssSvMeasurementHeader &gnssSvMeasHeader,
        PBGnssSvMeasurementHeader* pPbSvMeasHeader) {
    if (nullptr == pPbSvMeasHeader) {
        LOC_LOGe("pPbSvMeasHeader is NULL!, return");
        return 1;
    }

    // uint64 flags = 1; - PBGpsSvMeasHeaderFlags
    pPbSvMeasHeader->set_flags(getPBMaskForGpsSvMeasHeaderFlags(gnssSvMeasHeader.flags));

    if (GNSS_SV_MEAS_HEADER_HAS_LEAP_SECOND & gnssSvMeasHeader.flags) {
        // PBLeapSecondInfo  leapSec = 2;
        PBLeapSecondInfo *pPbLeapSecInfo = pPbSvMeasHeader->mutable_leapsec();
        if (nullptr != pPbLeapSecInfo) {
            pPbLeapSecInfo->set_leapsec(gnssSvMeasHeader.leapSec.leapSec);
            pPbLeapSecInfo->set_leapsecunc(gnssSvMeasHeader.leapSec.leapSecUnc);
        } else {
            LOC_LOGe("mutable_leapsec failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_CLOCK_FREQ & gnssSvMeasHeader.flags) {
        // PBLocRcvrClockFreqInfo       clockFreq = 3;
        PBLocRcvrClockFreqInfo *pPbClockFreq = pPbSvMeasHeader->mutable_clockfreq();
        if (nullptr != pPbClockFreq) {
            pPbClockFreq->set_clockdrift(gnssSvMeasHeader.clockFreq.clockDrift);
            pPbClockFreq->set_clockdriftunc(gnssSvMeasHeader.clockFreq.clockDriftUnc);
            pPbClockFreq->set_sourceoffreq(
                    getPBEnumForGnssLocSourceofFreq(gnssSvMeasHeader.clockFreq.sourceOfFreq));
        } else {
            LOC_LOGe("mutable_clockfreq failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_AP_TIMESTAMP & gnssSvMeasHeader.flags) {
        // PBApTimeStampStructType    apBootTimeStamp = 4;
        PBApTimeStampStructType *pPbApTimeStamp = pPbSvMeasHeader->mutable_apboottimestamp();
        if (nullptr != pPbApTimeStamp) {
            PBTimespec *pPbTimespec = pPbApTimeStamp->mutable_aptimestamp();
            if (nullptr != pPbTimespec) {
                pPbTimespec->set_tv_sec(gnssSvMeasHeader.apBootTimeStamp.apTimeStamp.tv_sec);
                pPbTimespec->set_tv_nsec(gnssSvMeasHeader.apBootTimeStamp.apTimeStamp.tv_nsec);
            } else {
                LOC_LOGe("mutable_aptimestamp failed");
            }
            pPbApTimeStamp->set_aptimestampuncms(
                    gnssSvMeasHeader.apBootTimeStamp.apTimeStampUncertaintyMs);
        } else {
            LOC_LOGe("mutable_apboottimestamp failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPS_GLO_INTER_SYSTEM_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  gpsGloInterSystemBias = 5;
        PBGnssInterSystemBias *pPbGpsGloIST = pPbSvMeasHeader->mutable_gpsglointersystembias();
        if (nullptr != pPbGpsGloIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.gpsGloInterSystemBias, pPbGpsGloIST);
        } else {
            LOC_LOGe("mutable_gpsglointersystembias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPS_BDS_INTER_SYSTEM_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  gpsBdsInterSystemBias = 6;
        PBGnssInterSystemBias *pPbGpsBdsIST = pPbSvMeasHeader->mutable_gpsbdsintersystembias();
        if (nullptr != pPbGpsBdsIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.gpsBdsInterSystemBias, pPbGpsBdsIST);
        } else {
            LOC_LOGe("mutable_gpsbdsintersystembias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPS_GAL_INTER_SYSTEM_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  gpsGalInterSystemBias = 7;
        PBGnssInterSystemBias *pPbGpsGalIST = pPbSvMeasHeader->mutable_gpsgalintersystembias();
        if (nullptr != pPbGpsGalIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.gpsGalInterSystemBias, pPbGpsGalIST);
        } else {
            LOC_LOGe("mutable_gpsgalintersystembias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_BDS_GLO_INTER_SYSTEM_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  bdsGloInterSystemBias = 8;
        PBGnssInterSystemBias *pPbBdsGloIST = pPbSvMeasHeader->mutable_bdsglointersystembias();
        if (nullptr != pPbBdsGloIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.bdsGloInterSystemBias, pPbBdsGloIST);
        } else {
            LOC_LOGe("mutable_bdsglointersystembias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GAL_GLO_INTER_SYSTEM_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  galGloInterSystemBias = 9;
        PBGnssInterSystemBias *pPbGalGloIST = pPbSvMeasHeader->mutable_galglointersystembias();
        if (nullptr != pPbGalGloIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.galGloInterSystemBias, pPbGalGloIST);
        } else {
            LOC_LOGe("mutable_galglointersystembias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GAL_BDS_INTER_SYSTEM_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  galBdsInterSystemBias = 10;
        PBGnssInterSystemBias *pPbGalBdsIST = pPbSvMeasHeader->mutable_galbdsintersystembias();
        if (nullptr != pPbGalBdsIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.galBdsInterSystemBias, pPbGalBdsIST);
        } else {
            LOC_LOGe("mutable_galbdsintersystembias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPSL1L5_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  gpsL1L5TimeBias = 11;
        PBGnssInterSystemBias *pPbGpsL1L5IST = pPbSvMeasHeader->mutable_gpsl1l5timebias();
        if (nullptr != pPbGpsL1L5IST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.gpsL1L5TimeBias, pPbGpsL1L5IST);
        } else {
            LOC_LOGe("mutable_gpsl1l5timebias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GALE1E5A_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  galE1E5aTimeBias = 12;
        PBGnssInterSystemBias *pPbGalE1E5aIST = pPbSvMeasHeader->mutable_gale1e5atimebias();
        if (nullptr != pPbGalE1E5aIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.galE1E5aTimeBias, pPbGalE1E5aIST);
        } else {
            LOC_LOGe("mutable_gale1e5atimebias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_BDSB1IB2A_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  bdsB1iB2aTimeBias = 13;
        PBGnssInterSystemBias *pPbBdsB1iB2aIST = pPbSvMeasHeader->mutable_bdsb1ib2atimebias();
        if (nullptr != pPbBdsB1iB2aIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.bdsB1iB2aTimeBias, pPbBdsB1iB2aIST);
        } else {
            LOC_LOGe("mutable_bdsb1ib2atimebias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME & gnssSvMeasHeader.flags) {
        // PBGnssSystemTimeStructType gpsSystemTime = 14;
        PBGnssSystemTimeStructType *pPbGpsSystemTime = pPbSvMeasHeader->mutable_gpssystemtime();
        if (nullptr != pPbGpsSystemTime) {
            convertGnssSystemTimeStructToPB(gnssSvMeasHeader.gpsSystemTime, pPbGpsSystemTime);
        } else {
            LOC_LOGe("mutable_gpssystemtime failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME & gnssSvMeasHeader.flags) {
        // PBGnssSystemTimeStructType galSystemTime = 15;
        PBGnssSystemTimeStructType *pPbGalSystemTime = pPbSvMeasHeader->mutable_galsystemtime();
        if (nullptr != pPbGalSystemTime) {
            convertGnssSystemTimeStructToPB(gnssSvMeasHeader.galSystemTime, pPbGalSystemTime);
        } else {
            LOC_LOGe("mutable_galsystemtime failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME & gnssSvMeasHeader.flags) {
        // PBGnssSystemTimeStructType bdsSystemTime = 16;
        PBGnssSystemTimeStructType *pPbBdsSystemTime = pPbSvMeasHeader->mutable_bdssystemtime();
        if (nullptr != pPbBdsSystemTime) {
            convertGnssSystemTimeStructToPB(gnssSvMeasHeader.bdsSystemTime, pPbBdsSystemTime);
        } else {
            LOC_LOGe("mutable_bdssystemtime failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME & gnssSvMeasHeader.flags) {
        // PBGnssSystemTimeStructType qzssSystemTime = 17;
        PBGnssSystemTimeStructType *pPbQzssSystemTime = pPbSvMeasHeader->mutable_qzsssystemtime();
        if (nullptr != pPbQzssSystemTime) {
            convertGnssSystemTimeStructToPB(gnssSvMeasHeader.qzssSystemTime, pPbQzssSystemTime);
        } else {
            LOC_LOGe("mutable_qzsssystemtime failed");
        }
    }

    if (GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME & gnssSvMeasHeader.flags) {
        // PBGnssGloTimeStructType    gloSystemTime = 18;
        PBGnssGloTimeStructType *pPbGloSystemTime = pPbSvMeasHeader->mutable_glosystemtime();
        if (nullptr != pPbGloSystemTime) {
            convertGnssGloTimeInfoToPB(gnssSvMeasHeader.gloSystemTime, pPbGloSystemTime);
        } else {
            LOC_LOGe("mutable_glosystemtime failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME_EXT & gnssSvMeasHeader.flags) {
        // PBLocGnssTimeExtInfo   gpsSystemTimeExt = 19;
        PBLocGnssTimeExtInfo *pPbGpsSystemTimeExt = pPbSvMeasHeader->mutable_gpssystemtimeext();
        if (nullptr != pPbGpsSystemTimeExt) {
            convertGnssLocGnssTimeExtInfoToPB(gnssSvMeasHeader.gpsSystemTimeExt,
                    pPbGpsSystemTimeExt);
        } else {
            LOC_LOGe("mutable_gpssystemtimeext failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME_EXT & gnssSvMeasHeader.flags) {
        // PBLocGnssTimeExtInfo   galSystemTimeExt = 20;
        PBLocGnssTimeExtInfo *pPbGalSystemTimeExt = pPbSvMeasHeader->mutable_galsystemtimeext();
        if (nullptr != pPbGalSystemTimeExt) {
            convertGnssLocGnssTimeExtInfoToPB(gnssSvMeasHeader.galSystemTimeExt,
                    pPbGalSystemTimeExt);
        } else {
            LOC_LOGe("mutable_galsystemtimeext failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME_EXT & gnssSvMeasHeader.flags) {
        // PBLocGnssTimeExtInfo   bdsSystemTimeExt = 21;
        PBLocGnssTimeExtInfo *pPbBdsSystemTimeExt = pPbSvMeasHeader->mutable_bdssystemtimeext();
        if (nullptr != pPbBdsSystemTimeExt) {
            convertGnssLocGnssTimeExtInfoToPB(gnssSvMeasHeader.bdsSystemTimeExt,
                    pPbBdsSystemTimeExt);
        } else {
            LOC_LOGe("mutable_bdssystemtimeext failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME_EXT & gnssSvMeasHeader.flags) {
        // PBLocGnssTimeExtInfo   qzssSystemTimeExt = 22;
        PBLocGnssTimeExtInfo *pPbQzssSystemTimeExt = pPbSvMeasHeader->mutable_qzsssystemtimeext();
        if (nullptr != pPbQzssSystemTimeExt) {
            convertGnssLocGnssTimeExtInfoToPB(gnssSvMeasHeader.qzssSystemTimeExt,
                    pPbQzssSystemTimeExt);
        } else {
            LOC_LOGe("mutable_qzsssystemtimeext failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME_EXT & gnssSvMeasHeader.flags) {
        // PBLocGnssTimeExtInfo   gloSystemTimeExt = 23;
        PBLocGnssTimeExtInfo *pPbGloSystemTimeExt = pPbSvMeasHeader->mutable_glosystemtimeext();
        if (nullptr != pPbGloSystemTimeExt) {
            convertGnssLocGnssTimeExtInfoToPB(gnssSvMeasHeader.gloSystemTimeExt,
                    pPbGloSystemTimeExt);
        } else {
            LOC_LOGe("mutable_glosystemtimeext failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_REF_COUNT_TICKS & gnssSvMeasHeader.flags) {
        // uint64  refCountTicks = 24;
        pPbSvMeasHeader->set_refcountticks(gnssSvMeasHeader.refCountTicks);
    }
    if (GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_TYPE & gnssSvMeasHeader.flags) {
        // PBLocDgnssCorrectionSourceType  dgnssCorrectionSourceType = 25;
        pPbSvMeasHeader->set_dgnsscorrectionsourcetype(
            getPBEnumForLocDgnssCorrectionSourceType(gnssSvMeasHeader.dgnssCorrectionSourceType));
    }
    if (GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_ID & gnssSvMeasHeader.flags) {
        // uint32  dgnssCorrectionSourceID = 26;
        pPbSvMeasHeader->set_dgnsscorrectionsourceid(gnssSvMeasHeader.dgnssCorrectionSourceID);
    }
    if (GNSS_SV_MEAS_HEADER_HAS_DGNSS_REF_STATION_ID & gnssSvMeasHeader.flags) {
        // uint32  dgnssRefStationId = 27;
        pPbSvMeasHeader->set_dgnssrefstationid(gnssSvMeasHeader.dgnssRefStationId);
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GPSL1L2C_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  gpsL1L2cTimeBias = 34;
        PBGnssInterSystemBias *pPbGpsL1L2cIST = pPbSvMeasHeader->mutable_gpsl1l2ctimebias();
        if (nullptr != pPbGpsL1L2cIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.gpsL1L2cTimeBias, pPbGpsL1L2cIST);
        } else {
            LOC_LOGe("mutable_gpsl1l2ctimebias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GLOG1G2_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  gloG1G2TimeBias = 35;
        PBGnssInterSystemBias *pPbGloG1G2IST = pPbSvMeasHeader->mutable_glog1g2timebias();
        if (nullptr != pPbGloG1G2IST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.gloG1G2TimeBias, pPbGloG1G2IST);
        } else {
            LOC_LOGe("mutable_glog1g2timebias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_BDSB1IB1C_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  bdsB1iB1cTimeBias = 36;
        PBGnssInterSystemBias *pPbBdsB1iB1cIST = pPbSvMeasHeader->mutable_bdsb1ib1ctimebias();
        if (nullptr != pPbBdsB1iB1cIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.bdsB1iB1cTimeBias, pPbBdsB1iB1cIST);
        } else {
            LOC_LOGe("mutable_bdsb1ib1ctimebias failed");
        }
    }
    if (GNSS_SV_MEAS_HEADER_HAS_GALE1E5B_TIME_BIAS & gnssSvMeasHeader.flags) {
        // PBGnssInterSystemBias  galE1E5bTimeBias = 37;
        PBGnssInterSystemBias *pPbGalE1E5bIST = pPbSvMeasHeader->mutable_gale1e5btimebias();
        if (nullptr != pPbGalE1E5bIST) {
            convertGnssInterSysBiasToPB(gnssSvMeasHeader.galE1E5bTimeBias, pPbGalE1E5bIST);
        } else {
            LOC_LOGe("mutable_gale1e5btimebias failed");
        }
    }
    return 0;
}

#ifdef FEATURE_CDFW
int EngineHubMsgConverter::convertStationStatusIndicatorMaskToPB(
        const StationStatusIndicatorMask &statStatusIndiMask,
        PBStationStatusIndicatorMask *pbStatStatusIndiMask) {
    if (nullptr == pbStatStatusIndiMask) {
        LOC_LOGe("pbStatStatusIndiMask is NULL!, return");
        return 1;
    }
    // uint32  stationIndicator = 1;
    pbStatStatusIndiMask->set_stationindicator(statStatusIndiMask.stationIndicator);
    // uint32  oscillatorIndicator = 2;
    pbStatStatusIndiMask->set_oscillatorindicator(statStatusIndiMask.oscillatorIndicator);
    // uint32  quarterCycleIndicator = 3;
    pbStatStatusIndiMask->set_quartercycleindicator(statStatusIndiMask.quarterCycleIndicator);
    // uint32  heightAvailability = 4;
    pbStatStatusIndiMask->set_heightavailability(statStatusIndiMask.heightAvailability);
    // uint32  referencePointUncertainty = 5;
    pbStatStatusIndiMask->set_referencepointuncertainty(
            statStatusIndiMask.referencePointUncertainty);
    return 0;
}

int EngineHubMsgConverter::convertReferenceStationToPB(const ReferenceStation &refStation,
        PBReferenceStation *pbRefStation) {
    if (nullptr == pbRefStation) {
        LOC_LOGe("pbRefStation is NULL!, return");
        return 1;
    }
    // uint32 stationID = 1;
    pbRefStation->set_stationid(refStation.stationID);
    // PBStationStatusIndicatorMask  statusIndicator = 2;
    PBStationStatusIndicatorMask *pPbStatIndMask = pbRefStation->mutable_statusindicator();
    if (nullptr != pPbStatIndMask) {
        convertStationStatusIndicatorMaskToPB(refStation.statusIndicator, pPbStatIndMask);
    } else {
        LOC_LOGe("pPbStatIndMask failed");
    }
    // pbRefStation->set_statusindicator refStation.statusIndicator
    // uint32 linkedStationID = 3;
    pbRefStation->set_linkedstationid(refStation.linkedStationID);
    // uint32  ITRFYear = 4;
    pbRefStation->set_itrfyear(refStation.ITRFYear);
    // uint32 constellationBitMask = 5; (PBDGnssConstellationBitMask)
    pbRefStation->set_constellationbitmask(
            getPBMaskForDGnssConstellationBitMask(refStation.constellationBitMask));
    // double   ecefX = 6;
    pbRefStation->set_ecefx(refStation.ecefX);
    // double   ecefY = 7;
    pbRefStation->set_ecefy(refStation.ecefY);
    // double   ecefZ = 8;
    pbRefStation->set_ecefz(refStation.ecefZ);
    // float    antennaHeight = 9;
    pbRefStation->set_antennaheight(refStation.antennaHeight);
    // float    uncertaintyX = 10;
    pbRefStation->set_uncertaintyx(refStation.uncertaintyX);
    // float    uncertaintyY = 11;
    pbRefStation->set_uncertaintyy(refStation.uncertaintyY);
    // float    uncertaintyZ = 12;
    pbRefStation->set_uncertaintyz(refStation.uncertaintyZ);
    // uint32  uncertaintyConfidenceX = 13;
    pbRefStation->set_uncertaintyconfidencex(refStation.uncertaintyConfidenceX);
    // uint32  uncertaintyConfidenceY = 14;
    pbRefStation->set_uncertaintyconfidencey(refStation.uncertaintyConfidenceY);
    // uint32  uncertaintyConfidenceZ = 15;
    pbRefStation->set_uncertaintyconfidencez(refStation.uncertaintyConfidenceZ);
    return 0;
}

int EngineHubMsgConverter::convertGnssEpochTimeToPB(
        const DGnssConstellationIdentifier &gnssConstId, const GnssEpochTime &gnssEpoch,
        PBGnssEpochTime *pbGnssEpoch) {
    if (nullptr == pbGnssEpoch) {
        LOC_LOGe("pbGnssEpoch is NULL!, return");
        return 1;
    }
    if (DGNSS_CONSTELLATION_IDENTIFIER_GLONASS == gnssConstId) {
        // glonassEpochTime gloEpochTime = 2;
        PBGnssEpochTime_glonassEpochTime *pbGloEpochTime = pbGnssEpoch->mutable_gloepochtime();
        if (nullptr != pbGloEpochTime) {
            // >> glonassEpochTime uint32 timeOfDay = 1;
            pbGloEpochTime->set_timeofday(gnssEpoch.glonassEpochTime.timeOfDay);
            // >> glonassEpochTime uint32 dayOfWeek = 2;
            pbGloEpochTime->set_dayofweek(gnssEpoch.glonassEpochTime.dayOfWeek);
        }
        else {
            LOC_LOGe("pbGloEpochTime is NULL!");
        }
    } else {
        // uint32  timeOfWeek = 1;
        pbGnssEpoch->set_timeofweek(gnssEpoch.timeOfWeek);
    }
    return 0;
}

int EngineHubMsgConverter::convertDGnssObservationStatusFlagsToPB(
        const DGnssObservationStatusFlags &dGnssObsStatFlags,
        PBDGnssObservationStatusFlags *pbDGnssObsStatFlags) {
    if (nullptr == pbDGnssObsStatFlags) {
        LOC_LOGe("pbDGnssObsStatFlags is NULL!, return");
        return 1;
    }
    // uint32  synchronousFlag                  = 1;
    pbDGnssObsStatFlags->set_synchronousflag(dGnssObsStatFlags.synchronousFlag);
    // uint32  smoothingIndicator               = 2;
    pbDGnssObsStatFlags->set_smoothingindicator(dGnssObsStatFlags.smoothingIndicator);
    // uint32  smoothingInterval                = 3;
    pbDGnssObsStatFlags->set_smoothinginterval(dGnssObsStatFlags.smoothingInterval);
    // uint32  clockSteeringIndicator           = 4;
    pbDGnssObsStatFlags->set_clocksteeringindicator(dGnssObsStatFlags.clockSteeringIndicator);
    // uint32  externalClockIndicator           = 5;
    pbDGnssObsStatFlags->set_externalclockindicator(dGnssObsStatFlags.externalClockIndicator);
    return 0;
}

int EngineHubMsgConverter::convertDGnssMeasurementToPB(
        const DGnssConstellationIdentifier &gnssConstId,
        const DGnssMeasurement &dGnssMeas,
        PBDGnssMeasurement *pbDGnssMeas) {
    if (nullptr == pbDGnssMeas) {
        LOC_LOGe("pbDGnssMeas is NULL!, return");
        return 1;
    }
    // uint32 svId = 1;
    pbDGnssMeas->set_svid(dGnssMeas.svId);
    // PBExtendedSatelliteInformation extendedSatelliteInformation = 2;
    pbDGnssMeas->set_extendedsatelliteinformation(
            getPBEnumForExtendedSatelliteInformation(dGnssMeas.extendedSatelliteInformation));

    // oneof gnssSignalIdentifier
    switch (gnssConstId) {
        case DGNSS_CONSTELLATION_IDENTIFIER_GPS:
            // PBDGnssGpsSignalIdentifier     gpsSignalIdentifier = 3;
            pbDGnssMeas->set_gpssignalidentifier(
                    getPBEnumForDGnssGpsSignalIdentifier(
                    dGnssMeas.gnssSignalIdentifier.gpsSignalIdentifier));
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_SBAS:
            // PBDGnssSbasSignalIdentifier    sbasSignalIdentifier = 8;
            pbDGnssMeas->set_sbassignalidentifier(
                    getPBEnumForDGnssSbasSignalIdentifier(
                    dGnssMeas.gnssSignalIdentifier.sbasSignalIdentifier));
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_QZSS:
            // PBDGnssQzssSignalIdentifier    qzssSignalIdentifier = 6;
            pbDGnssMeas->set_qzsssignalidentifier(
                    getPBEnumForDGnssQzssSignalIdentifier(
                    dGnssMeas.gnssSignalIdentifier.qzssSignalIdentifier));
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_GALILEO:
            // PBDGnssGalileoSignalIdentifier galileoSignalIdentifier = 5;
            pbDGnssMeas->set_galileosignalidentifier(
                    getPBEnumForDGnssGalileoSignalIdentifier(
                    dGnssMeas.gnssSignalIdentifier.galileoSignalIdentifier));
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_GLONASS:
            // PBDGnssGlonassSignalIdentifier glonassSignalIdentifier = 4;
            pbDGnssMeas->set_glonasssignalidentifier(
                    getPBEnumForDGnssGlonassSignalIdentifier(
                    dGnssMeas.gnssSignalIdentifier.glonassSignalIdentifier));
            break;
        case DGNSS_CONSTELLATION_IDENTIFIER_BDS:
            // PBDGnssBdsSignalIdentifier     bdsSignalIdentifier = 7;
            pbDGnssMeas->set_bdssignalidentifier(
                    getPBEnumForDGnssBdsSignalIdentifier(
                    dGnssMeas.gnssSignalIdentifier.bdsSignalIdentifier));
            break;
        default:
            LOC_LOGe("Invalid constellation id %d", gnssConstId);
            break;
    }

    // uint32 signalObservationStatus = 9;
    pbDGnssMeas->set_signalobservationstatus(
            getPBMaskForDGnssSignalStatusFlags(dGnssMeas.signalObservationStatus));
    // double  pseudorange = 10;
    pbDGnssMeas->set_pseudorange(dGnssMeas.pseudorange);
    // double  phaseRange = 11;
    pbDGnssMeas->set_phaserange(dGnssMeas.phaseRange);
    // double  phaseRangeRate = 12;
    pbDGnssMeas->set_phaserangerate(dGnssMeas.phaseRangeRate);
    // uint32 phaseRangeLockTimeIndicator = 13;
    pbDGnssMeas->set_phaserangelocktimeindicator(dGnssMeas.phaseRangeLockTimeIndicator);
    // float signalCNR = 14;
    pbDGnssMeas->set_signalcnr(dGnssMeas.signalCNR);
    return 0;
}

int EngineHubMsgConverter::convertDGnssObservationToPB(const DGnssObservation &dGnssObs,
        PBDGnssObservation *pbDGnssObs) {
    if (nullptr == pbDGnssObs) {
        LOC_LOGe("pbDGnssObs is NULL!, return");
        return 1;
    }
    // uint32                      stationID = 1;
    pbDGnssObs->set_stationid(dGnssObs.stationID);
    // PBGnssEpochTime             epochTime = 2;
    PBGnssEpochTime *pPbGnssEpochTime = pbDGnssObs->mutable_epochtime();
    if (nullptr != pPbGnssEpochTime) {
        convertGnssEpochTimeToPB(dGnssObs.constellationIdentifier, dGnssObs.epochTime,
                pPbGnssEpochTime);
    } else {
        LOC_LOGe("pPbGnssEpochTime failed");
    }
    // PBDGnssObservationStatusFlags   statusFlags = 3;
    PBDGnssObservationStatusFlags *pPbGnssObsStatFlags = pbDGnssObs->mutable_statusflags();
    if (nullptr != pPbGnssObsStatFlags) {
        convertDGnssObservationStatusFlagsToPB(dGnssObs.statusFlags, pPbGnssObsStatFlags);
    } else {
        LOC_LOGe("pPbGnssObsStatFlags failed");
    }
    // uint32                       issueOfDataStation = 4;
    pbDGnssObs->set_issueofdatastation(dGnssObs.issueOfDataStation);
    // PBDGnssConstellationIdentifier  constellationIdentifier = 5;
    pbDGnssObs->set_constellationidentifier(
            getPBEnumForDGnssConstellationIdentifier(dGnssObs.constellationIdentifier));
    // uint32                       numberOfMeasurements = 6;
    pbDGnssObs->set_numberofmeasurements(dGnssObs.numberOfMeasurements);
    // repeated PBDGnssMeasurement dGnssMeasurement = 7;
    uint32_t numberMeas = dGnssObs.numberOfMeasurements;
    for (uint32_t i = 0; i < numberMeas; i++) {
        PBDGnssMeasurement *pPbDGnssMeas = pbDGnssObs->add_dgnssmeasurement();
        if (nullptr != pPbDGnssMeas) {
            if (convertDGnssMeasurementToPB(dGnssObs.constellationIdentifier,
                    dGnssObs.dGnssMeasurement[i], pPbDGnssMeas)) {
                LOC_LOGe("convertDGnssMeasurementToPB failed");
            }
        } else {
            LOC_LOGe("add_dgnssmeasurement is NULL");
        }
    }

    return 0;
}
#endif //#ifdef FEATURE_CDFW

bool EngineHubMsgConverter::ParseFromString(::google::protobuf::MessageLite& engHubMsg,
        const string& data) {
    bool bRetval = engHubMsg.ParseFromString(data);
    if (!bRetval) {
        LOC_LOGe("ParseFromString failed");
    }
    return bRetval;
}

bool EngineHubMsgConverter::ParseFromString(::google::protobuf::MessageLite& engHubMsg,
            const EngineHubMessage& eHubPayLoadMsg) {
    bool bRetval = engHubMsg.ParseFromString(eHubPayLoadMsg.payload());
    if (!bRetval) {
        LOC_LOGe("ParseFromString failed");
    }
    return bRetval;
}

bool EngineHubMsgConverter::ParseFromString(::google::protobuf::MessageLite& engHubMsg,
        uint32_t &msgCounter, string &senderName,
        const EngineHubMessageWithMsgCounter& eHubPayLoadMsg) {
    msgCounter = eHubPayLoadMsg.msgcounter();
    senderName = eHubPayLoadMsg.msgsendername();
    bool bRetval = engHubMsg.ParseFromString(eHubPayLoadMsg.ehmsgpayload());
    if (!bRetval) {
        LOC_LOGe("ParseFromString failed");
    }
    return bRetval;
}

EHubMessageProtocolRegMsg* EngineHubMsgConverter::decodeEHRoutingTableEntry(
        EHRoutingTableEntry &pbEhRoutingTable) {
    // EHConnectionType engConnectionType = 1;
    EngineConnectionType eHubengConnectionType =
            getEnumForPBEHConnectionType(pbEhRoutingTable.engconnectiontype());
    LOC_LOGd("Engine Connection Type is %d", eHubengConnectionType);

    // repeated EHRegReportMask regReportMask = 3;
    // eHubRegReportMask - Max of EH_NODE_COUNT
    // Check if the PB value and our struct value is matching.
    int num_entries = EH_NODE_COUNT;
    if ((int)EH_NODE_COUNT != (int)E_HUB_TECH_COUNT) {
        LOC_LOGe("EH_NODE_COUNT %d and E_HUB_TECH_COUNT %d not matching",
                EH_NODE_COUNT, E_HUB_TECH_COUNT);
        num_entries = std::min((int)EH_NODE_COUNT, (int)E_HUB_TECH_COUNT);
    }
    eHubRegReportMask regReportMask[E_HUB_TECH_COUNT] = {};
    for (int i=0; i < num_entries; i++) {
        regReportMask[i] = getEhubRegReportMaskFromPBEHRegReportMask(
                pbEhRoutingTable.regreportmask(i));
    }

    // EHRegStatusMask regStatusMask = 4;
    eHubRegStatusMask regStatmask = getEhubRegStatusMaskFromPBEHRegStatusMask(
            pbEhRoutingTable.regstatusmask());
    LOC_LOGd("EHub RegStatusMask is %d", regStatmask);

    // EHSubModifiers  subModifiers = 5;
    eHubSubModifiers  subModifiers = pbConvertToEHubSubModifiers(pbEhRoutingTable.submodifiers());

    // EHRegReportCorrectionMask regCdfwCorrnMask = 6;
    eHubRegReportCorrectionMask regCdfwCorrnMask =
            getEhubRegCorrectionMaskFromPBEHRegReportCorrectionMask(
            pbEhRoutingTable.regcdfwcorrnmask());

    int32_t featureId = pbEhRoutingTable.featureid();
    // EHEngineConnectionInfo engConnectInfo = 2;
    EHubMessageProtocolRegMsg* regMsg = nullptr;
    if (eHubengConnectionType == ENGINE_CONNECT_TYPE_LOCAL) {
        regMsg = new EHubMessageProtocolRegMsg(
                pbEhRoutingTable.engconnectinfo().socketname().c_str(),
                regReportMask, regStatmask, subModifiers, regCdfwCorrnMask,
                (pbEhRoutingTable.regsessionmsgs() != 0), featureId);
    } else if (eHubengConnectionType == ENGINE_CONNECT_TYPE_INET) {
        engineConnectionIpAddrInfo connectionInfo = {};
        strlcpy(connectionInfo.pluginIpAddress,
                pbEhRoutingTable.engconnectinfo().pluginipaddrinfo().pluginipaddress().c_str(),
                sizeof(connectionInfo.pluginIpAddress));
        connectionInfo.pluginPort =
                (uint16_t)pbEhRoutingTable.engconnectinfo().pluginipaddrinfo().pluginport();
        regMsg = new EHubMessageProtocolRegMsg(
                connectionInfo,
                regReportMask, regStatmask, subModifiers, regCdfwCorrnMask,
                (pbEhRoutingTable.regsessionmsgs() != 0), featureId);
    } else {
        LOC_LOGe("Unknown engine connection type : %d", eHubengConnectionType);
    }
    return regMsg;
}

EHubMessageReportPosition* EngineHubMsgConverter::decodeEHMessageReportPosition(
        EHMessageReportPosition &pbEhMsgRprtPos) {
    // >> EHTechId sourceTechId = 1;
    EHubTechId techId = getEnumForPBEHTechId(pbEhMsgRprtPos.sourcetechid());
    LOC_LOGd("EHUB Tech Id is %d", techId);

    UlpLocation location;
    GpsLocationExtended locationExtended;
    loc_sess_status status;
    // >> PBUlpLocation location = 2;
    pbConvertToUlpLocation(pbEhMsgRprtPos.location(), location);

    // >> PBGpsLocationExtended locationExtended = 3;
    pbConvertToGpsLocationExtended(pbEhMsgRprtPos.locationextended(), locationExtended);

    // >> PBLocSessionStatus status = 4;
    status = getEnumForPBLocSessionStatus(pbEhMsgRprtPos.status());

    EHubMessageReportPosition* posRprt =
            new EHubMessageReportPosition(techId, location, locationExtended, status,
                                          false, nullptr, 0);
    return posRprt;
}

EHubMessageReportSv* EngineHubMsgConverter::decodeEHMessageReportSv(
        EHMessageReportSv &pbEhMsgRprtSv) {
    // >> EHTechId sourceTechId = 1;
    EHubTechId techId = getEnumForPBEHTechId(pbEhMsgRprtSv.sourcetechid());
    LOC_LOGd("EHUB Tech Id is %d", techId);

    // >> PBGnssSvNotification svNotification = 2;
    GnssSvNotification svNotif;
    pbConvertToGnssSvNotification(pbEhMsgRprtSv.svnotification(), svNotif);

    EHubMessageReportSv* svReportMsg = new EHubMessageReportSv(techId, svNotif);
    return svReportMsg;
}

EHubMessageQWESInstallLicenseReq* EngineHubMsgConverter::decodeEHMessageInstallLicenseReq(
        EHMessageQWESInstallLicenseReq &pbQwesInstallLicenseReq) {
    EHubMessageQWESInstallLicenseReq *licenseReq = new EHubMessageQWESInstallLicenseReq(
            reinterpret_cast<const uint8_t*>(pbQwesInstallLicenseReq.licensebuffer().c_str()),
            pbQwesInstallLicenseReq.licenselength());
   return licenseReq;
}

EHubMessageQWESReqAttestationStatement* EngineHubMsgConverter::decodeEHMessageAttestationReq(
        EHMessageQWESReqAttestationStatement &pbQwesAttestationReq) {
    EHubMessageQWESReqAttestationStatement *attestationReq =
        new EHubMessageQWESReqAttestationStatement(
                reinterpret_cast<const uint8_t*>(pbQwesAttestationReq.nonce().c_str()),
                pbQwesAttestationReq.noncelength(),
                reinterpret_cast<const uint8_t*>(pbQwesAttestationReq.data().c_str()),
                pbQwesAttestationReq.datalength());
    return attestationReq;
}

bool EngineHubMsgConverter::pbPopulateEHMsgReportSvMeasBasedOnSubModifiers(
            const EHubMessageReportSvMeas* inMsg, GnssConstellationTypeMask regConstellationMask,
            GnssSignalTypeMask regSignalTypeMask, uint8_t * &outMsg, size_t &outMsgSize,
            uint32_t msgCounter) {
    if (inMsg == nullptr) {
        LOC_LOGe("inMsg is NULL");
        return false;
    }

    EngineHubMessage ehMsg;
    // EHMsgId     msgId = 1;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_SV_MEAS);
    LOC_LOGd("pbPopulateEHMsgReportSvMeasBasedOnSubModifiers-regConstlMask: %x, regSignlMask: %x",
            regConstellationMask, regSignalTypeMask);

    // Fill up svmeas
    EHMessageReportSvMeas ehMsgReportSvMeas;
    PBGnssSvMeasurementSet *pPbGnssSvMeasSet = ehMsgReportSvMeas.mutable_svmeasset();
    if (nullptr == pPbGnssSvMeasSet) {
        LOC_LOGe("mutable_svmeasset failed");
        return false;
    }

    // EHTechId sourceTechId = 1;
    ehMsgReportSvMeas.set_sourcetechid(getPBEnumForEHubTechId(inMsg->sourceTechId));

    const GnssSvMeasurementSet &inSvMeasSet = inMsg->svMeasSet;
    // PBGnssSvMeasurementSet svMeasSet = 2;
    //   >> bool  isNhz = 1;
    pPbGnssSvMeasSet->set_isnhz(inSvMeasSet.isNhz);
    //   >> uint64  ptpTimeNs = 2;
    pPbGnssSvMeasSet->set_ptptimens(inMsg->ptpTimeNs);

    //   >> PBGnssSvMeasurementHeader  svMeasSetHeader = 3;
    // Fill GnssSvMeasurementHeader
    PBGnssSvMeasurementHeader* pSvMeasurementHeader = pPbGnssSvMeasSet->mutable_svmeassetheader();
    if (nullptr != pSvMeasurementHeader) {
        if (convertGnssSvMeasToPB(inSvMeasSet.svMeasSetHeader, pSvMeasurementHeader)) {
            LOC_LOGe("convertGnssSvMeasToPB failed");
            return false;
        }
    } else {
        LOC_LOGe("mutable_svmeassetheader is NULL");
        return false;
    }

    //   >> repeated PBSVMeasurementStructType  svMeas = 5;
    uint32_t actualOutSvMeasCount = 0;
    for (uint32_t count = 0; count < inSvMeasSet.svMeasCount; count++) {
        const Gnss_SVMeasurementStructType & inSvMeasurement = inSvMeasSet.svMeas[count];
        bool needed = svNeededBasedOnSvSystem(regConstellationMask,
                                              inSvMeasurement.gnssSystem);
        if ((true == needed) && (regSignalTypeMask & inSvMeasurement.gnssSignalTypeMask)) {
            PBSVMeasurementStructType* svInfo = pPbGnssSvMeasSet->add_svmeas();
            if (nullptr != svInfo) {
                convertGnssSVMeasurementStructTypeToPB(inSvMeasSet.svMeas[count], svInfo);
                actualOutSvMeasCount++;
            } else {
                LOC_LOGe("mutable_svmeas failed!");
            }
        }
    }
    LOC_LOGd("svMeasHeadSize:%d, actualOutSvMeasCount=%d",
            pSvMeasurementHeader->ByteSizeLong(), actualOutSvMeasCount);
    //   >> uint32  svMeasCount = 4;
    // Fill in actual svMeasCount in output.
    pPbGnssSvMeasSet->set_svmeascount(actualOutSvMeasCount);

    string ehSubMsg;
    string ehMsgStr;
    string os;
    bool bRetVal = ehMsgReportSvMeas.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportSvMeas(ehMsgReportSvMeas);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportSvMeas failed!");
        return bRetVal;
    }
    // bytes       payload = 2;
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&ehMsgStr)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return false;
    }

    if (pbPopulateEHMsgWithCounterToSend(ehMsgStr, EH_MSG_ID_REPORT_SV_MEAS, msgCounter, os)) {
        LOC_LOGe("pbPopulateEHMsgWithCounterToSend failed");
        return false;
    }

    outMsgSize = os.size();
    outMsg = new uint8_t[outMsgSize];
    if (outMsg) {
        memcpy(outMsg, (uint8_t *) os.c_str(), outMsgSize);
        bRetVal = true;
    } else {
        LOC_LOGe("malloc failure for outMsg");
        outMsgSize = 0;
    }
    LOC_LOGd("pbPopulateEHMsgReportSvMeasBasedOnSubModifiers ehMsg length: %d", outMsgSize);
    return bRetVal;
}

bool EngineHubMsgConverter::pbPopulateEHMsgSvEphemerisBasedOnSubModifiers(
            const EHubMessageReportSvEphemeris* inMsg, GnssSignalTypeMask gnssSignalMask,
            uint8_t * &outMsg, size_t &outMsgSize, uint32_t msgCounter) {
    if (inMsg == nullptr) {
        LOC_LOGe("inMsg is NULL");
        return false;
    }
    EngineHubMessage ehMsg;
    // EHMsgId     msgId = 1;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_SV_EPHEMERIS);
    LOC_LOGd("pbPopulateEHMsgSvEphemerisBasedOnSubModifiers - gnssSignlMask: %x",
            gnssSignalMask);

    EHMessageReportSvEphemeris ehMsgReportSvEph;
    // EHTechId sourceTechId = 1;
    ehMsgReportSvEph.set_sourcetechid(getPBEnumForEHubTechId(inMsg->sourceTechId));

    const GnssSvEphemerisReport &gnssEph = inMsg->svEphemeris;
    // PBGnssSvEphemerisReport svEphemeris = 2;
    PBGnssSvEphemerisReport* svEph = ehMsgReportSvEph.mutable_svephemeris();
    if (nullptr != svEph) {
        // PBGnss_LocSvSystemEnumType gnssConstellation = 1;
        svEph->set_gnssconstellation(
                getPBEnumForGnssLocSvSystem(gnssEph.gnssConstellation));
        // bool isSystemTimeValid = 2;
        svEph->set_issystemtimevalid(gnssEph.isSystemTimeValid);

        // PBGnssSystemTimeStructType systemTime = 3;
        PBGnssSystemTimeStructType* pGnssSysTime = svEph->mutable_systemtime();
        if (nullptr != pGnssSysTime) {
            if (convertGnssSystemTimeStructToPB(gnssEph.systemTime, pGnssSysTime)) {
                LOC_LOGe("convertGnssSystemTimeStructToPB failed");
            }
        } else {
            LOC_LOGe("mutable_systemtime failed");
        }

        // We need to fillup only GnssSvEphemerisReport -> ephInfo(GalileoEphemerisResponse)
        switch (gnssEph.gnssConstellation) {
            case GNSS_LOC_SV_SYSTEM_GPS:
            {
                // PBGpsEphemerisResponse gpsEphemeris = 4;
                PBGpsEphemerisResponse* pbGpsEph = svEph->mutable_gpsephemeris();
                if (nullptr != pbGpsEph) {
                    if (convertGpsEphRespToPB(gnssEph.ephInfo.gpsEphemeris, pbGpsEph)) {
                        LOC_LOGe("convertGpsEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_gpsephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_GALILEO:
            {
                // PBGalileoEphemerisResponse galileoEphemeris = 7;
                PBGalileoEphemerisResponse* pbGalEph = svEph->mutable_galileoephemeris();
                if (nullptr != pbGalEph) {
                    if (convertGalileoEphRespToPB(gnssEph.ephInfo.galileoEphemeris, pbGalEph)) {
                        LOC_LOGe("convertGalileoEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_galileoephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_GLONASS:
            {
                // PBGlonassEphemerisResponse glonassEphemeris = 5;
                PBGlonassEphemerisResponse* pbGloEph = svEph->mutable_glonassephemeris();
                if (nullptr != pbGloEph) {
                    if (convertGlonassEphRespToPB(gnssEph.ephInfo.glonassEphemeris, pbGloEph)) {
                        LOC_LOGe("convertGlonassEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_glonassephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_BDS:
            {
                // PBBdsEphemerisResponse bdsEphemeris = 6;
                PBBdsEphemerisResponse* pbBdsEph = svEph->mutable_bdsephemeris();
                if (nullptr != pbBdsEph) {
                    if (convertBdsEphRespToPB(gnssEph.ephInfo.bdsEphemeris, pbBdsEph)) {
                        LOC_LOGe("convertBdsEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_bdsephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_QZSS:
            {
                // PBQzssEphemerisResponse qzssEphemeris = 8;
                PBQzssEphemerisResponse* pQzssEph = svEph->mutable_qzssephemeris();
                if (nullptr != pQzssEph) {
                    if (convertQzssEphRespToPB(gnssEph.ephInfo.qzssEphemeris, pQzssEph)) {
                        LOC_LOGe("convertQzssEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_qzssephemeris failed");
                }
            }
            break;
            default:
            break;
        }
        //Append Galileo data.
        const GalileoEphemerisResponse &inGalEph = inMsg->svEphemeris.ephInfo.galileoEphemeris;
        PBGalileoEphemerisResponse* pbGalEphRsp = svEph->mutable_galileoephemeris();
        if (nullptr != pbGalEphRsp) {
            uint32_t currentNumOfEphemeris = pbGalEphRsp->numofephemeris();

            // repeated PBGalileoEphemeris galEphemerisData = 2;
            for (int i = 0; i < inGalEph.numOfEphemeris; i++) {
                if (((gnssSignalMask & GNSS_SIGNAL_GALILEO_E1)&&
                    (inGalEph.galEphemerisData[i].dataSourceSignal == GNSS_SIGNAL_GALILEO_E1)) ||
                    ((gnssSignalMask & GNSS_SIGNAL_GALILEO_E5A)&&
                    (inGalEph.galEphemerisData[i].dataSourceSignal == GNSS_SIGNAL_GALILEO_E5A))) {
                    PBGalileoEphemeris *galEphem =
                            pbGalEphRsp->mutable_galephemerisdata(currentNumOfEphemeris);
                    if (nullptr != galEphem) {
                        if (convertGalEphemerisToPB(inGalEph.galEphemerisData[i], galEphem)) {
                            LOC_LOGe("convertGlonassEphemerisToPB failed");
                        }
                        currentNumOfEphemeris++;
                    } else {
                        LOC_LOGe("mutable_galephemerisdata is NULL");
                    }
                }
            }
            // Update the new numOfEphemeris for GalileoEphemerisResponse
            // uint32 numOfEphemeris = 1;
            pbGalEphRsp->set_numofephemeris(currentNumOfEphemeris);
        } else {
            LOC_LOGe("mutable_galileoephemeris failed");
        }
    } else {
        LOC_LOGe("mutable_svephemeris failed");
        return false;
    }

    string ehSubMsg;
    string ehMsgStr;
    string os;
    bool bRetVal = ehMsgReportSvEph.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportSvEphemeris(ehMsgReportSvEph);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportSvEph failed!");
        return bRetVal;
    }
    // bytes       payload = 2;
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&ehMsgStr)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return false;
    }

    if (pbPopulateEHMsgWithCounterToSend(ehMsgStr, EH_MSG_ID_REPORT_SV_EPHEMERIS,
            msgCounter, os)) {
        LOC_LOGe("pbPopulateEHMsgWithCounterToSend failed");
        return false;
    }

    outMsgSize = os.size();
    outMsg = new uint8_t[outMsgSize];
    if (outMsg) {
        memcpy(outMsg, (uint8_t *) os.c_str(), outMsgSize);
        bRetVal = true;
    } else {
        LOC_LOGe("malloc failure for outMsg");
        outMsgSize = 0;
    }
    LOC_LOGd("pbPopulateEHMsgSvEphemerisBasedOnSubModifiers ehMsg length: %d", outMsgSize);
    return bRetVal;
}

uint32_t EngineHubMsgConverter::getEhubRegReportMaskFromPBEHRegReportMask(
        const uint32_t &ehRegReportMask) {
    uint32_t localEhubRegReportMask = 0;
    if (ehRegReportMask & EH_REG_REPORT_MASK_POSITION) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_POSITION;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_NHZ_POSITION) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_NHZ_POSITION;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_UNPROPAGATED_POSITION) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_UNPROPAGATED_POSITION;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_SV) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_SV;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_SV_POLYNOMIAL) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_SV_POLYNOMIAL;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_SV_MEAS) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_SV_MEAS;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_SV_EPHEMERIS) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_SV_EPHEMERIS;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_NHZ_SV_MEAS) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_NHZ_SV_MEAS;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_KLOBUCHARIONO_MODEL) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_KLOBUCHARIONO_MODEL;
    }
    if (ehRegReportMask & EH_REG_REPORT_MASK_GLONASS_ADDITIONAL_PARAMS) {
        localEhubRegReportMask |= E_HUB_REG_REPORT_MASK_GLONASS_ADDITIONAL_PARAMS;
    }
    return localEhubRegReportMask;
}

uint32_t EngineHubMsgConverter::getEhubRegStatusMaskFromPBEHRegStatusMask(
        const uint32_t &ehRegStatusMask) {
    uint32_t localEhubRegStatusMask = 0;
    if (ehRegStatusMask & EH_REG_STATUS_MASK_CONNECTIVITY) {
        localEhubRegStatusMask |= E_HUB_REG_STATUS_MASK_CONNECTIVITY;
    }
    if (ehRegStatusMask & EH_REG_STATUS_MASK_LEVER_ARM_CONFIG) {
        localEhubRegStatusMask |= E_HUB_REG_STATUS_MASK_LEVER_ARM_CONFIG;
    }
    if (ehRegStatusMask & EH_REG_STATUS_MASK_BODY_TO_SENSOR_MOUNT_PARAMS) {
        localEhubRegStatusMask |= E_HUB_REG_STATUS_MASK_BODY_TO_SENSOR_MOUNT_PARAMS;
    }
    return localEhubRegStatusMask;
}

uint32_t EngineHubMsgConverter::getEhubRegCorrectionMaskFromPBEHRegReportCorrectionMask(
        const uint32_t &ehRegReportCrrnMask) {
    uint32_t eHubRegReportCorrnMask = 0;
    if (ehRegReportCrrnMask & EH_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION) {
        eHubRegReportCorrnMask |= E_HUB_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION;
    }
    if (ehRegReportCrrnMask & EH_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION) {
        eHubRegReportCorrnMask |= E_HUB_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION;
    }
    return eHubRegReportCorrnMask;
}

uint32_t EngineHubMsgConverter::getGnssSignalTypeMaskFromPBGnssSignalInfoMask(
        const uint32_t &pbGnssSignalInfoMask) {
    uint32_t gnssSignalTypeMask = 0;
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GPS_L1CA) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L1CA;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GPS_L1C) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L1C;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GPS_L2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L2;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GPS_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L5;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GLONASS_G1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GLONASS_G1;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GLONASS_G2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GLONASS_G2;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GALILEO_E1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E1;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GALILEO_E5A) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E5A;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_GALILEO_E5B) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E5B;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B1I) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1I;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B1C) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1C;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B2I) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2I;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AI) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2AI;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_QZSS_L1CA) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L1CA;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_QZSS_L1S) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L1S;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_QZSS_L2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L2;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_QZSS_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L5;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_SBAS_L1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_SBAS_L1;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_NAVIC_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_NAVIC_L5;
    }
    if (pbGnssSignalInfoMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AQ) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2AQ;
    }
    return gnssSignalTypeMask;
}

uint32_t EngineHubMsgConverter::getGnssConstellationTypeMaskFromPBGnssConstellationInfoMask(
        const uint32_t &pbGnssConstellationInfoMask) {
    uint32_t gnssConstellationTypeMask = 0;
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_GPS) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_GPS_BIT;
    }
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_GLONASS) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_GLONASS_BIT;
    }
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_QZSS) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_QZSS_BIT;
    }
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_BEIDOU) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_BEIDOU_BIT;
    }
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_GALILEO) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_GALILEO_BIT;
    }
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_SBAS) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_SBAS_BIT;
    }
    if (pbGnssConstellationInfoMask & PB_GNSS_CONSTELLATION_TYPE_NAVIC) {
        gnssConstellationTypeMask |= GNSS_CONSTELLATION_TYPE_NAVIC_BIT;
    }
    return gnssConstellationTypeMask;
}

uint32_t EngineHubMsgConverter::getLocPosTechMaskFromPBLocPosTechMask(
        const uint32_t &pbLocPosTechMask) {
    uint32_t  locPosTechMask = LOC_POS_TECH_MASK_DEFAULT;
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_DEFAULT) {
        locPosTechMask |= LOC_POS_TECH_MASK_DEFAULT;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_SATELLITE) {
        locPosTechMask |= LOC_POS_TECH_MASK_SATELLITE;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_CELLID) {
        locPosTechMask |= LOC_POS_TECH_MASK_CELLID;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_WIFI) {
        locPosTechMask |= LOC_POS_TECH_MASK_WIFI;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_SENSORS) {
        locPosTechMask |= LOC_POS_TECH_MASK_SENSORS;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_REFERENCE_LOCATION) {
        locPosTechMask |= LOC_POS_TECH_MASK_REFERENCE_LOCATION;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION) {
        locPosTechMask |= LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_AFLT) {
        locPosTechMask |= LOC_POS_TECH_MASK_AFLT;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_HYBRID) {
        locPosTechMask |= LOC_POS_TECH_MASK_HYBRID;
    }
    if (pbLocPosTechMask & PB_LOC_POS_TECH_MASK_PPE) {
        locPosTechMask |= LOC_POS_TECH_MASK_PPE;
    }
    return locPosTechMask;
}


uint32_t EngineHubMsgConverter::getLocGpsSpoofMaskFromPBLocGpsSpoofMask(
        const uint32_t &pbLocGpsSpoofMask) {
    uint32_t  locGpsSpoofMask = LOC_GPS_LOCATION_NONE_SPOOFED;
    if (pbLocGpsSpoofMask & PB_LOC_GPS_LOCATION_NONE_SPOOFED) {
        locGpsSpoofMask |= LOC_GPS_LOCATION_NONE_SPOOFED;
    }
    if (pbLocGpsSpoofMask & PB_LOC_GPS_LOCATION_POSITION_SPOOFED) {
        locGpsSpoofMask |= LOC_GPS_LOCATION_POSITION_SPOOFED;
    }
    if (pbLocGpsSpoofMask & PB_LOC_GPS_LOCATION_TIME_SPOOFED) {
        locGpsSpoofMask |= LOC_GPS_LOCATION_TIME_SPOOFED;
    }
    if (pbLocGpsSpoofMask & PB_LOC_GPS_LOCATION_NAVIGATION_DATA_SPOOFED) {
        locGpsSpoofMask |= LOC_GPS_LOCATION_NAVIGATION_DATA_SPOOFED;
    }
    return locGpsSpoofMask;
}

uint32_t EngineHubMsgConverter::getLocGpsLocationFlagsFromPBLocGpsLocationFlags(
        const uint32_t &pbLocGpsLocFlags) {
    uint32_t locGpsLocFlags = 0;
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_ALTITUDE) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_ALTITUDE;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_SPEED) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_SPEED;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_BEARING) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_BEARING;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_ACCURACY) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_ACCURACY;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_SPOOF_MASK) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_SPOOF_MASK;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_SPEED_ACCURACY) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_SPEED_ACCURACY;
    }
    if (pbLocGpsLocFlags & PB_LOC_GPS_LOCATION_HAS_BEARING_ACCURACY) {
        locGpsLocFlags |= LOC_GPS_LOCATION_HAS_BEARING_ACCURACY;
    }
    return locGpsLocFlags;
}

uint32_t EngineHubMsgConverter::getGnssSvOptionsMaskFromPBGnssSvOptionsMask(
        const uint16_t &pbGnssSvOptionsMask) {
    uint32_t gnssSvOptionsMask = 0;
    if (pbGnssSvOptionsMask & PB_GNSS_SV_OPTIONS_HAS_EPHEMER_BIT) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
    }
    if (pbGnssSvOptionsMask & PB_GNSS_SV_OPTIONS_HAS_ALMANAC_BIT) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
    }
    if (pbGnssSvOptionsMask & PB_GNSS_SV_OPTIONS_USED_IN_FIX_BIT) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }
    if (pbGnssSvOptionsMask & PB_GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT;
    }
    if (pbGnssSvOptionsMask & PB_GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT;
    }
    return gnssSvOptionsMask;
}

uint32_t EngineHubMsgConverter::getGnssSignalTypeMaskFromPBGnssSignalTypeMask(
        const uint32_t &pbGnssSignalTypeMask) {
    uint32_t  gnssSignalTypeMask = 0;
    // PBGnssSignalInfoMask to GnssSignalTypeMask
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GPS_L1CA) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L1CA;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GPS_L1C) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L1C;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GPS_L2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L2;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GPS_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L5;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GLONASS_G1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GLONASS_G1;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GLONASS_G2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GLONASS_G2;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GALILEO_E1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E1;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GALILEO_E5A) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E5A;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_GALILEO_E5B) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E5B;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B1I) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1I;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B1C) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1C;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B2I) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2I;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AI) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2AI;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_QZSS_L1CA) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L1CA;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_QZSS_L1S) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L1S;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_QZSS_L2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L2;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_QZSS_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L5;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_SBAS_L1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_SBAS_L1;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_NAVIC_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_NAVIC_L5;
    }
    if (pbGnssSignalTypeMask & PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AQ) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2AQ;
    }
    return gnssSignalTypeMask;
}

uint32_t EngineHubMsgConverter::getLocNavSolutionMaskFromPBLocNavSolutionMask(
        const uint32_t &pbLocNavSolnMask) {
    uint32_t locNavSolnMask = 0;
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_SBAS_CORRECTION_IONO) {
        locNavSolnMask |= LOC_NAV_MASK_SBAS_CORRECTION_IONO;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_SBAS_CORRECTION_FAST) {
        locNavSolnMask |= LOC_NAV_MASK_SBAS_CORRECTION_FAST;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_SBAS_CORRECTION_LONG) {
        locNavSolnMask |= LOC_NAV_MASK_SBAS_CORRECTION_LONG;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_SBAS_INTEGRITY) {
        locNavSolnMask |= LOC_NAV_MASK_SBAS_INTEGRITY;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_DGNSS_CORRECTION) {
        locNavSolnMask |= LOC_NAV_MASK_DGNSS_CORRECTION;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_RTK_CORRECTION) {
        locNavSolnMask |= LOC_NAV_MASK_RTK_CORRECTION;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_PPP_CORRECTION) {
        locNavSolnMask |= LOC_NAV_MASK_PPP_CORRECTION;
    }
    if (pbLocNavSolnMask & PB_LOC_NAV_MASK_RTK_FIXED_CORRECTION) {
        locNavSolnMask |= LOC_NAV_MASK_RTK_FIXED_CORRECTION;
    }
    return locNavSolnMask;
}

uint32_t EngineHubMsgConverter::getDrCalibrationStatusMaskFromPBDrCalibrationStatusMask(
        const uint32_t &pbDrCalibStatMask) {
    uint32_t drCalibStatMask = 0;
    if (pbDrCalibStatMask & PB_DR_ROLL_CALIBRATION_NEEDED) {
        drCalibStatMask |= DR_ROLL_CALIBRATION_NEEDED;
    }
    if (pbDrCalibStatMask & PB_DR_PITCH_CALIBRATION_NEEDED) {
        drCalibStatMask |= DR_PITCH_CALIBRATION_NEEDED;
    }
    if (pbDrCalibStatMask & PB_DR_YAW_CALIBRATION_NEEDED) {
        drCalibStatMask |= DR_YAW_CALIBRATION_NEEDED;
    }
    if (pbDrCalibStatMask & PB_DR_ODO_CALIBRATION_NEEDED) {
        drCalibStatMask |= DR_ODO_CALIBRATION_NEEDED;
    }
    if (pbDrCalibStatMask & PB_DR_GYRO_CALIBRATION_NEEDED) {
        drCalibStatMask |= DR_GYRO_CALIBRATION_NEEDED;
    }
    if (pbDrCalibStatMask & PB_DR_TURN_CALIBRATION_LOW) {
        drCalibStatMask |= DR_TURN_CALIBRATION_LOW;
    }
    if (pbDrCalibStatMask & PB_DR_TURN_CALIBRATION_MEDIUM) {
        drCalibStatMask |= DR_TURN_CALIBRATION_MEDIUM;
    }
    if (pbDrCalibStatMask & PB_DR_TURN_CALIBRATION_HIGH) {
        drCalibStatMask |= DR_TURN_CALIBRATION_HIGH;
    }
    if (pbDrCalibStatMask & PB_DR_LINEAR_ACCEL_CALIBRATION_LOW) {
        drCalibStatMask |= DR_LINEAR_ACCEL_CALIBRATION_LOW;
    }
    if (pbDrCalibStatMask & PB_DR_LINEAR_ACCEL_CALIBRATION_MEDIUM) {
        drCalibStatMask |= DR_LINEAR_ACCEL_CALIBRATION_MEDIUM;
    }
    if (pbDrCalibStatMask & PB_DR_LINEAR_ACCEL_CALIBRATION_HIGH) {
        drCalibStatMask |= DR_LINEAR_ACCEL_CALIBRATION_HIGH;
    }
    if (pbDrCalibStatMask & PB_DR_LINEAR_MOTION_CALIBRATION_LOW) {
        drCalibStatMask |= DR_LINEAR_MOTION_CALIBRATION_LOW;
    }
    if (pbDrCalibStatMask & PB_DR_LINEAR_MOTION_CALIBRATION_MEDIUM) {
        drCalibStatMask |= DR_LINEAR_MOTION_CALIBRATION_MEDIUM;
    }
    if (pbDrCalibStatMask & PB_DR_LINEAR_MOTION_CALIBRATION_HIGH) {
        drCalibStatMask |= DR_LINEAR_MOTION_CALIBRATION_HIGH;
    }
    if (pbDrCalibStatMask & PB_DR_STATIC_CALIBRATION_LOW) {
        drCalibStatMask |= DR_STATIC_CALIBRATION_LOW;
    }
    if (pbDrCalibStatMask & PB_DR_STATIC_CALIBRATION_MEDIUM) {
        drCalibStatMask |= DR_STATIC_CALIBRATION_MEDIUM;
    }
    if (pbDrCalibStatMask & PB_DR_STATIC_CALIBRATION_HIGH) {
        drCalibStatMask |= DR_STATIC_CALIBRATION_HIGH;
    }
    return drCalibStatMask;
}

uint32_t EngineHubMsgConverter::getDrSolutionStatusMaskFromPBSolutionStatusMask(
        const uint32_t &pbSolStatusMask) {
    uint32_t drSolStatusMask = PB_VEHICLE_DRSOLUTION_INVALID;
    if (pbSolStatusMask & PB_VEHICLE_SENSOR_SPEED_INPUT_DETECTED) {
        drSolStatusMask |= VEHICLE_SENSOR_SPEED_INPUT_DETECTED;
    }
    if (pbSolStatusMask & PB_VEHICLE_SENSOR_SPEED_INPUT_USED) {
        drSolStatusMask |= VEHICLE_SENSOR_SPEED_INPUT_USED;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_UNCALIBRATED) {
        drSolStatusMask |= DRE_WARNING_UNCALIBRATED;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_GNSS_QUALITY_INSUFFICIENT) {
        drSolStatusMask |= DRE_WARNING_GNSS_QUALITY_INSUFFICIENT;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_FERRY_DETECTED) {
        drSolStatusMask |= DRE_WARNING_FERRY_DETECTED;
    }
    if (pbSolStatusMask & PB_DRE_ERROR_6DOF_SENSOR_UNAVAILABLE) {
        drSolStatusMask |= DRE_ERROR_6DOF_SENSOR_UNAVAILABLE;
    }
    if (pbSolStatusMask & PB_DRE_ERROR_VEHICLE_SPEED_UNAVAILABLE) {
        drSolStatusMask |= DRE_ERROR_VEHICLE_SPEED_UNAVAILABLE;
    }
    if (pbSolStatusMask & PB_DRE_ERROR_GNSS_EPH_UNAVAILABLE) {
        drSolStatusMask |= DRE_ERROR_GNSS_EPH_UNAVAILABLE;
    }
    if (pbSolStatusMask & PB_DRE_ERROR_GNSS_MEAS_UNAVAILABLE) {
       drSolStatusMask |= DRE_ERROR_GNSS_MEAS_UNAVAILABLE;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_INIT_POSITION_INVALID) {
       drSolStatusMask |= DRE_WARNING_INIT_POSITION_INVALID;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_INIT_POSITION_UNRELIABLE) {
       drSolStatusMask |= DRE_WARNING_INIT_POSITION_UNRELIABLE;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_POSITON_UNRELIABLE) {
       drSolStatusMask |= DRE_WARNING_POSITON_UNRELIABLE;
    }
    if (pbSolStatusMask & PB_DRE_ERROR_GENERIC) {
       drSolStatusMask |= DRE_ERROR_GENERIC;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_SENSOR_TEMP_OUT_OF_RANGE) {
       drSolStatusMask |= DRE_WARNING_SENSOR_TEMP_OUT_OF_RANGE;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_USER_DYNAMICS_INSUFFICIENT) {
       drSolStatusMask |= DRE_WARNING_USER_DYNAMICS_INSUFFICIENT;
    }
    if (pbSolStatusMask & PB_DRE_WARNING_FACTORY_DATA_INCONSISTENT) {
       drSolStatusMask |= DRE_WARNING_FACTORY_DATA_INCONSISTENT;
    }
    return drSolStatusMask;
}

uint32_t EngineHubMsgConverter::getPositioningEngineMaskFromPBPositioningEngineMask(
        const uint32_t &pbPosEngMask) {
    uint32_t posEngMask = 0;
    if (pbPosEngMask & PB_STANDARD_POSITIONING_ENGINE) {
        posEngMask |= STANDARD_POSITIONING_ENGINE;
    }
    if (pbPosEngMask & PB_DEAD_RECKONING_ENGINE) {
        posEngMask |= DEAD_RECKONING_ENGINE;
    }
    if (pbPosEngMask & PB_PRECISE_POSITIONING_ENGINE) {
        posEngMask |= PRECISE_POSITIONING_ENGINE;
    }
    return posEngMask;
}

uint32_t EngineHubMsgConverter::getGnssMeasUsageInfoValidityMaskFromPB(
        const uint32_t &pbGnssMeasUsageInfoValidMask) {
    uint32_t gnssMeasUsageInfoValidMask = 0;
    if (pbGnssMeasUsageInfoValidMask & PB_GNSS_PSEUDO_RANGE_RESIDUAL_VALID) {
        gnssMeasUsageInfoValidMask |= GNSS_PSEUDO_RANGE_RESIDUAL_VALID;
    }
    if (pbGnssMeasUsageInfoValidMask & PB_GNSS_DOPPLER_RESIDUAL_VALID) {
        gnssMeasUsageInfoValidMask |= GNSS_DOPPLER_RESIDUAL_VALID;
    }
    if (pbGnssMeasUsageInfoValidMask & PB_GNSS_CARRIER_PHASE_RESIDUAL_VALID) {
        gnssMeasUsageInfoValidMask |= GNSS_CARRIER_PHASE_RESIDUAL_VALID;
    }
    if (pbGnssMeasUsageInfoValidMask & PB_GNSS_CARRIER_PHASE_AMBIGUITY_TYPE_VALID) {
        gnssMeasUsageInfoValidMask |= GNSS_CARRIER_PHASE_AMBIGUITY_TYPE_VALID;
    }
    return gnssMeasUsageInfoValidMask;
}

uint32_t EngineHubMsgConverter::getGnssMeasUsageStatusBitMaskFromPB(
        const uint32_t &pbGnssMeasUsageStatusBitMask) {
    uint32_t gnssMeasUsageStatusBitMask = 0;
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USED_IN_PVT) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USED_IN_PVT;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_BAD_MEAS) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_BAD_MEAS;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_CNO_TOO_LOW) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_CNO_TOO_LOW;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_ELEVATION_TOO_LOW) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_ELEVATION_TOO_LOW;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_NO_EPHEMERIS) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_NO_EPHEMERIS;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_NO_CORRECTIONS) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_NO_CORRECTIONS;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_CORRECTION_TIMEOUT) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_CORRECTION_TIMEOUT;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_UNHEALTHY) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_UNHEALTHY;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_CONFIG_DISABLED) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_CONFIG_DISABLED;
    }
    if (pbGnssMeasUsageStatusBitMask & PB_GNSS_MEAS_USAGE_STATUS_OTHER) {
        gnssMeasUsageStatusBitMask |= GNSS_MEAS_USAGE_STATUS_OTHER;
    }
    return gnssMeasUsageStatusBitMask;
}


uint32_t EngineHubMsgConverter::getGnssSystemTimeStructTypeFlagsFromPB(
        const uint32_t &pbGnssSysTimeStruct) {
    uint32_t gnssSysTimeStruct = 0;
    if (pbGnssSysTimeStruct & PB_GNSS_SYSTEM_TIME_WEEK_VALID) {
        gnssSysTimeStruct |= GNSS_SYSTEM_TIME_WEEK_VALID;
    }
    if (pbGnssSysTimeStruct & PB_GNSS_SYSTEM_TIME_WEEK_MS_VALID) {
        gnssSysTimeStruct |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
    }
    if (pbGnssSysTimeStruct & PB_GNSS_SYSTEM_CLK_TIME_BIAS_VALID) {
        gnssSysTimeStruct |= GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
    }
    if (pbGnssSysTimeStruct & PB_GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID) {
        gnssSysTimeStruct |= GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
    }
    if (pbGnssSysTimeStruct & PB_GNSS_SYSTEM_REF_FCOUNT_VALID) {
        gnssSysTimeStruct |= GNSS_SYSTEM_REF_FCOUNT_VALID;
    }
    if (pbGnssSysTimeStruct & PB_GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID) {
        gnssSysTimeStruct |= GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
    }
    return gnssSysTimeStruct;
}

uint32_t EngineHubMsgConverter::getGnssGloTimeStructTypeFlagsFromPB(
        const uint32_t &pbGloSysTimeStruct) {
    uint32_t gloSysTimeStruct = 0;
    if (pbGloSysTimeStruct & PB_GNSS_CLO_DAYS_VALID) {
        gloSysTimeStruct |= GNSS_CLO_DAYS_VALID;
    }
    if (pbGloSysTimeStruct & PB_GNSS_GLO_MSEC_VALID) {
        gloSysTimeStruct |= GNSS_GLO_MSEC_VALID;
    }
    if (pbGloSysTimeStruct & PB_GNSS_GLO_CLK_TIME_BIAS_VALID) {
        gloSysTimeStruct |= GNSS_GLO_CLK_TIME_BIAS_VALID;
    }
    if (pbGloSysTimeStruct & PB_GNSS_GLO_CLK_TIME_BIAS_UNC_VALID) {
        gloSysTimeStruct |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
    }
    if (pbGloSysTimeStruct & PB_GNSS_GLO_REF_FCOUNT_VALID) {
        gloSysTimeStruct |= GNSS_GLO_REF_FCOUNT_VALID;
    }
    if (pbGloSysTimeStruct & PB_GNSS_GLO_NUM_CLOCK_RESETS_VALID) {
        gloSysTimeStruct |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
    }
    if (pbGloSysTimeStruct & PB_GNSS_GLO_FOUR_YEAR_VALID) {
        gloSysTimeStruct |= GNSS_GLO_FOUR_YEAR_VALID;
    }
    return gloSysTimeStruct;
}

uint32_t EngineHubMsgConverter::getGnssLocationPosDataMaskFromPB(
        const uint32_t &pbGnssLocPosDataMask) {
    uint32_t gnssLocPosDataMask= LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_PITCH_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_PITCH_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
    }
    if (pbGnssLocPosDataMask & PB_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT) {
        gnssLocPosDataMask |= LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
    }
    return gnssLocPosDataMask;
}

EngineConnectionType EngineHubMsgConverter::getEnumForPBEHConnectionType(
        const EHConnectionType &engConnectionType) {
    EngineConnectionType eHubEngConnectionType = ENGINE_CONNECT_TYPE_LOCAL;
    switch (engConnectionType) {
        case EH_ENGINE_CONNECT_TYPE_LOCAL:
            eHubEngConnectionType = ENGINE_CONNECT_TYPE_LOCAL;
            break;
        case EH_ENGINE_CONNECT_TYPE_INET:
            eHubEngConnectionType = ENGINE_CONNECT_TYPE_INET;
            break;
        default:
            eHubEngConnectionType = ENGINE_CONNECT_TYPE_MAX;
            break;
    }
    return eHubEngConnectionType;
}

EHubTechId EngineHubMsgConverter::getEnumForPBEHTechId(const EHTechId &pbEhTechId) {
    EHubTechId eHubTechId = E_HUB_TECH_GNSS;
    switch (pbEhTechId) {
        case EH_NODE_GNSS:
            eHubTechId = E_HUB_TECH_GNSS;
            break;
        case EH_NODE_PPE:
            eHubTechId = E_HUB_TECH_PPE;
            break;
        case EH_NODE_DR:
            eHubTechId = E_HUB_TECH_DR;
            break;
        case EH_NODE_VPE:
            eHubTechId = E_HUB_TECH_VPE;
            break;
        case E_HUB_TECH_COUNT:
        default:
            LOC_LOGe("Invalid tech id : %d", pbEhTechId);
            break;
    }
    return eHubTechId;
}

loc_sess_status EngineHubMsgConverter::getEnumForPBLocSessionStatus(
        const PBLocSessionStatus &pbLocSessStatus) {
    loc_sess_status locSessStatus = LOC_SESS_FAILURE;
    switch (pbLocSessStatus) {
        case PB_LOC_SESS_SUCCESS:
            locSessStatus = LOC_SESS_SUCCESS;
            break;
        case PB_LOC_SESS_INTERMEDIATE:
            locSessStatus = LOC_SESS_INTERMEDIATE;
            break;
        case PB_LOC_SESS_FAILURE:
            locSessStatus = LOC_SESS_FAILURE;
            break;
        default:
            break;
    }
    return locSessStatus;
}

GnssSvType EngineHubMsgConverter::getEnumForPBGnssSvType(const PBGnssSvType &pbGnssSvType) {
    GnssSvType gnssSvType = GNSS_SV_TYPE_UNKNOWN;
    switch (pbGnssSvType) {
        case PB_GNSS_SV_TYPE_GPS:
            gnssSvType = GNSS_SV_TYPE_GPS;
            break;
        case PB_GNSS_SV_TYPE_GLONASS:
            gnssSvType = GNSS_SV_TYPE_GLONASS;
            break;
        case PB_GNSS_SV_TYPE_QZSS:
            gnssSvType = GNSS_SV_TYPE_QZSS;
            break;
        case PB_GNSS_SV_TYPE_BEIDOU:
            gnssSvType = GNSS_SV_TYPE_BEIDOU;
            break;
        case PB_GNSS_SV_TYPE_GALILEO:
            gnssSvType = GNSS_SV_TYPE_GALILEO;
            break;
        default:
            break;
    }
    return gnssSvType;
}

LocReliability EngineHubMsgConverter::getEnumForPBLocReliability(
        const PBLocReliability &pbLocReliability) {
    LocReliability locReliability = LOC_RELIABILITY_NOT_SET;
    switch (pbLocReliability) {
        case PB_LOC_RELIABILITY_NOT_SET:
            locReliability = LOC_RELIABILITY_NOT_SET;
            break;
        case PB_LOC_RELIABILITY_VERY_LOW:
            locReliability = LOC_RELIABILITY_VERY_LOW;
            break;
        case PB_LOC_RELIABILITY_LOW:
            locReliability = LOC_RELIABILITY_LOW;
            break;
        case PB_LOC_RELIABILITY_MEDIUM:
            locReliability = LOC_RELIABILITY_MEDIUM;
            break;
        case PB_LOC_RELIABILITY_HIGH:
            locReliability = LOC_RELIABILITY_HIGH;
            break;
        default:
            break;
    }
    return locReliability;
}

LocSvInfoSource EngineHubMsgConverter::getEnumForPBLocSvInfoSource(
        const PBLocSvInfoSource &pbLocSvSrcInfo) {
    LocSvInfoSource locSvInfoSrc = ULP_SVINFO_IS_FROM_GNSS;
    switch (pbLocSvSrcInfo) {
        case PB_ULP_SVINFO_IS_FROM_GNSS:
            locSvInfoSrc = ULP_SVINFO_IS_FROM_GNSS;
            break;
        case PB_ULP_SVINFO_IS_FROM_DR:
            locSvInfoSrc = ULP_SVINFO_IS_FROM_DR;
            break;
        default:
            break;
    }
    return locSvInfoSrc;
}

LocOutputEngineType EngineHubMsgConverter::getEnumForPBLocOutputEngineType(
        const PBLocOutputEngineType &pbLocOutputEngType) {
    LocOutputEngineType locOutpEngType = LOC_OUTPUT_ENGINE_FUSED;
    switch (pbLocOutputEngType) {
        case PB_LOC_OUTPUT_ENGINE_FUSED:
            locOutpEngType = LOC_OUTPUT_ENGINE_FUSED;
            break;
        case PB_LOC_OUTPUT_ENGINE_SPE:
            locOutpEngType = LOC_OUTPUT_ENGINE_SPE;
            break;
        case PB_LOC_OUTPUT_ENGINE_PPE:
            locOutpEngType = LOC_OUTPUT_ENGINE_PPE;
            break;
        case PB_LOC_OUTPUT_ENGINE_COUNT:
            locOutpEngType = LOC_OUTPUT_ENGINE_COUNT;
            break;
        default:
            break;
    }
    return locOutpEngType;
}

CarrierPhaseAmbiguityType EngineHubMsgConverter::getEnumForPBCarrierPhaseAmbiguityType(
        const PBCarrierPhaseAmbiguityType &pbCarrierPhaseAmbigType) {
    CarrierPhaseAmbiguityType carrierPhaseAmbigType = CARRIER_PHASE_AMBIGUITY_RESOLUTION_NONE;
    switch (pbCarrierPhaseAmbigType) {
        case PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_NONE:
            carrierPhaseAmbigType = CARRIER_PHASE_AMBIGUITY_RESOLUTION_NONE;
            break;
        case PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FLOAT:
            carrierPhaseAmbigType = CARRIER_PHASE_AMBIGUITY_RESOLUTION_FLOAT;
            break;
        case PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FIXED:
            carrierPhaseAmbigType = CARRIER_PHASE_AMBIGUITY_RESOLUTION_FIXED;
            break;
        default:
            break;
    }
    return carrierPhaseAmbigType;
}

Gnss_LocSvSystemEnumType EngineHubMsgConverter::getEnumForPBGnss_LocSvSystemEnumType(
        const PBGnss_LocSvSystemEnumType &pbGnssLocSvSystemEnum) {
    Gnss_LocSvSystemEnumType gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_UNKNOWN;
    switch (pbGnssLocSvSystemEnum) {
        case PB_GNSS_LOC_SV_SYSTEM_GPS:
            gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_GPS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GALILEO:
            gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_GALILEO;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_SBAS:
            gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_SBAS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GLONASS:
            gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_GLONASS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_BDS:
            gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_BDS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_QZSS:
            gnssLocSvSystemEnum = GNSS_LOC_SV_SYSTEM_QZSS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_NAVIC:
            gnssLocSvSystemEnum =  GNSS_LOC_SV_SYSTEM_NAVIC;
        break;
        default:
        break;
    }
    return gnssLocSvSystemEnum;
}

LocDgnssCorrectionSourceType EngineHubMsgConverter::getEnumForPBLocDgnssCorrectionSourceType(
            const PBLocDgnssCorrectionSourceType &pbLocDgnssCorrnSrcType) {
    LocDgnssCorrectionSourceType locDgnssCorrnSrcType = LOC_DGNSS_CORR_SOURCE_TYPE_INVALID;
    switch (pbLocDgnssCorrnSrcType) {
        case PB_LOC_DGNSS_CORR_SOURCE_TYPE_RTCM:
            locDgnssCorrnSrcType = LOC_DGNSS_CORR_SOURCE_TYPE_RTCM;
        break;
        case PB_LOC_DGNSS_CORR_SOURCE_TYPE_3GPP:
            locDgnssCorrnSrcType = LOC_DGNSS_CORR_SOURCE_TYPE_3GPP;
        break;
        default:
        break;
    }
    return locDgnssCorrnSrcType;
}

int EngineHubMsgConverter::pbConvertToGnssSvNotification(const PBGnssSvNotification &pbGnssSvNotif,
        GnssSvNotification &gnssSvNotif) {
    // >> bool gnssSignalTypeMaskValid = 1;
    gnssSvNotif.gnssSignalTypeMaskValid = pbGnssSvNotif.gnsssignaltypemaskvalid();
    // information on a number of SVs
    //   >> repeated PBGnssSv gnssSvs = 2
    int num_svs = pbGnssSvNotif.gnsssvs_size();
    gnssSvNotif.count = num_svs;
    LOC_LOGd("pbConvertToGnssSvNotification - num svs %d", num_svs);
    for (int i=0; i < num_svs; i++) {
        PBGnssSv pPbGnssSv = pbGnssSvNotif.gnsssvs(i);
        // uint32 svId = 1;
        gnssSvNotif.gnssSvs[i].svId = pPbGnssSv.svid();
        // PBGnssSvType type = 2;
        gnssSvNotif.gnssSvs[i].type = getEnumForPBGnssSvType(pPbGnssSv.type());
        // float cN0Dbhz = 3;
        gnssSvNotif.gnssSvs[i].cN0Dbhz = pPbGnssSv.cn0dbhz();
        // float elevation = 4;
        // float azimuth = 5;
        gnssSvNotif.gnssSvs[i].elevation = pPbGnssSv.elevation();
        gnssSvNotif.gnssSvs[i].azimuth = pPbGnssSv.azimuth();
        // uint32 gnssSvOptionsMask = 6;
        gnssSvNotif.gnssSvs[i].gnssSvOptionsMask =
                getGnssSvOptionsMaskFromPBGnssSvOptionsMask(pPbGnssSv.gnsssvoptionsmask());
        // float carrierFrequencyHz = 7;
        gnssSvNotif.gnssSvs[i].carrierFrequencyHz = pPbGnssSv.carrierfrequencyhz();
        // uint32 gnssSignalTypeMask = 8;
        gnssSvNotif.gnssSvs[i].gnssSignalTypeMask =
                getGnssSignalTypeMaskFromPBGnssSignalTypeMask(pPbGnssSv.gnsssignaltypemask());
    }
    return 0;
}

int EngineHubMsgConverter::pbConvertToUlpLocation(const PBUlpLocation &pbUlpLoc,
        UlpLocation &ulpLoc) {
    // >> PBLocGpsLocation    gpsLocation = 1;
    pbConvertToLocGpsLocation(pbUlpLoc.gpslocation(), ulpLoc.gpsLocation);
    // >> uint32       position_source    = 2;
    // >> uint32  tech_mask    = 3;
    // >> bool    unpropagatedPosition    = 4;
    ulpLoc.position_source = pbUlpLoc.position_source();
    ulpLoc.tech_mask = getLocPosTechMaskFromPBLocPosTechMask(pbUlpLoc.tech_mask());
    ulpLoc.unpropagatedPosition = pbUlpLoc.unpropagatedposition();
    LOC_LOGd("pbConvertToUlpLocation Position source %d, Tech Mask %d, unpropagated %d",
                 ulpLoc.position_source, ulpLoc.tech_mask, ulpLoc.unpropagatedPosition);
    return 0;
}

int EngineHubMsgConverter::pbConvertToGpsLocationExtended(const PBGpsLocationExtended &pbGpsLocExt,
        GpsLocationExtended &gpsLocExt) {
    // >> uint64        flags = 1;
    gpsLocExt.flags = pbGpsLocExt.flags();

    // >> float           altitudeMeanSeaLevel = 2;
    // >> float           pdop = 3;
    // >> float           hdop = 4;
    // >> float           vdop = 5;
    // >> float           magneticDeviation = 6;
    gpsLocExt.altitudeMeanSeaLevel = pbGpsLocExt.altitudemeansealevel();
    gpsLocExt.pdop = pbGpsLocExt.pdop();
    gpsLocExt.hdop = pbGpsLocExt.hdop();
    gpsLocExt.vdop = pbGpsLocExt.vdop();
    gpsLocExt.magneticDeviation = pbGpsLocExt.magneticdeviation();

    // >> float           vert_unc = 7;
    // >> float           speed_unc = 8;
    // >> float           bearing_unc = 9;
    gpsLocExt.vert_unc = pbGpsLocExt.vert_unc();
    gpsLocExt.speed_unc = pbGpsLocExt.speed_unc();
    gpsLocExt.bearing_unc = pbGpsLocExt.bearing_unc();

    // >> PBLocReliability  horizontal_reliability = 10;
    gpsLocExt.horizontal_reliability =
            getEnumForPBLocReliability(pbGpsLocExt.horizontal_reliability());
    // >> PBLocReliability  vertical_reliability = 11;
    gpsLocExt.vertical_reliability =
            getEnumForPBLocReliability(pbGpsLocExt.vertical_reliability());

    // >> float           horUncEllipseSemiMajor = 12;
    // >> float           horUncEllipseSemiMinor = 13;
    // >> float           horUncEllipseOrientAzimuth = 14;
    gpsLocExt.horUncEllipseSemiMajor = pbGpsLocExt.horuncellipsesemimajor();
    gpsLocExt.horUncEllipseSemiMinor = pbGpsLocExt.horuncellipsesemiminor();
    gpsLocExt.horUncEllipseOrientAzimuth = pbGpsLocExt.horuncellipseorientazimuth();

    // >> PBApTimeStampStructType         timeStamp = 15;
    PBApTimeStampStructType pPbApTimeStamp = pbGpsLocExt.timestamp();
    PBTimespec pPbTimespec = pPbApTimeStamp.aptimestamp();
    gpsLocExt.timeStamp.apTimeStamp.tv_sec = pPbTimespec.tv_sec();
    gpsLocExt.timeStamp.apTimeStamp.tv_nsec = pPbTimespec.tv_nsec();
    gpsLocExt.timeStamp.apTimeStampUncertaintyMs = pPbApTimeStamp.aptimestampuncms();

    // >> PBGnssSvUsedInPosition gnss_sv_used_ids = 16;
    GnssSvUsedInPosition &gnssSvusedInPos = gpsLocExt.gnss_sv_used_ids;
    const PBGnssSvUsedInPosition &pbGnssSvusedInPos = pbGpsLocExt.gnss_sv_used_ids();
    gnssSvusedInPos.gps_sv_used_ids_mask = pbGnssSvusedInPos.gps_sv_used_ids_mask();
    gnssSvusedInPos.glo_sv_used_ids_mask = pbGnssSvusedInPos.glo_sv_used_ids_mask();
    gnssSvusedInPos.gal_sv_used_ids_mask = pbGnssSvusedInPos.gal_sv_used_ids_mask();
    gnssSvusedInPos.bds_sv_used_ids_mask = pbGnssSvusedInPos.bds_sv_used_ids_mask();
    gnssSvusedInPos.qzss_sv_used_ids_mask = pbGnssSvusedInPos.qzss_sv_used_ids_mask();

    // >> PBGnssSvMbUsedInPosition gnss_mb_sv_used_ids = 17;
    GnssSvMbUsedInPosition &gnssSvMbusedInPos = gpsLocExt.gnss_mb_sv_used_ids;
    const PBGnssSvMbUsedInPosition &pbGnssSvMbusedInPos = pbGpsLocExt.gnss_mb_sv_used_ids();
    gnssSvMbusedInPos.gps_l1ca_sv_used_ids_mask = pbGnssSvMbusedInPos.gps_l1ca_sv_used_ids_mask();
    gnssSvMbusedInPos.gps_l1c_sv_used_ids_mask = pbGnssSvMbusedInPos.gps_l1c_sv_used_ids_mask();
    gnssSvMbusedInPos.gps_l2_sv_used_ids_mask = pbGnssSvMbusedInPos.gps_l2_sv_used_ids_mask();
    gnssSvMbusedInPos.gps_l5_sv_used_ids_mask = pbGnssSvMbusedInPos.gps_l5_sv_used_ids_mask();
    gnssSvMbusedInPos.glo_g1_sv_used_ids_mask = pbGnssSvMbusedInPos.glo_g1_sv_used_ids_mask();
    gnssSvMbusedInPos.glo_g2_sv_used_ids_mask = pbGnssSvMbusedInPos.glo_g2_sv_used_ids_mask();
    gnssSvMbusedInPos.gal_e1_sv_used_ids_mask = pbGnssSvMbusedInPos.gal_e1_sv_used_ids_mask();
    gnssSvMbusedInPos.gal_e5a_sv_used_ids_mask = pbGnssSvMbusedInPos.gal_e5a_sv_used_ids_mask();
    gnssSvMbusedInPos.gal_e5b_sv_used_ids_mask = pbGnssSvMbusedInPos.gal_e5b_sv_used_ids_mask();
    gnssSvMbusedInPos.bds_b1i_sv_used_ids_mask = pbGnssSvMbusedInPos.bds_b1i_sv_used_ids_mask();
    gnssSvMbusedInPos.bds_b1c_sv_used_ids_mask = pbGnssSvMbusedInPos.bds_b1c_sv_used_ids_mask();
    gnssSvMbusedInPos.bds_b2i_sv_used_ids_mask = pbGnssSvMbusedInPos.bds_b2i_sv_used_ids_mask();
    gnssSvMbusedInPos.bds_b2ai_sv_used_ids_mask = pbGnssSvMbusedInPos.bds_b2ai_sv_used_ids_mask();
    gnssSvMbusedInPos.qzss_l1ca_sv_used_ids_mask =
            pbGnssSvMbusedInPos.qzss_l1ca_sv_used_ids_mask();
    gnssSvMbusedInPos.qzss_l1s_sv_used_ids_mask = pbGnssSvMbusedInPos.qzss_l1s_sv_used_ids_mask();
    gnssSvMbusedInPos.qzss_l2_sv_used_ids_mask = pbGnssSvMbusedInPos.qzss_l2_sv_used_ids_mask();
    gnssSvMbusedInPos.qzss_l5_sv_used_ids_mask = pbGnssSvMbusedInPos.qzss_l5_sv_used_ids_mask();
    gnssSvMbusedInPos.sbas_l1_sv_used_ids_mask = pbGnssSvMbusedInPos.sbas_l1_sv_used_ids_mask();
    gnssSvMbusedInPos.bds_b2aq_sv_used_ids_mask = pbGnssSvMbusedInPos.bds_b2aq_sv_used_ids_mask();

    // >> uint32  navSolutionMask = 18; PBLocNavSolutionMask
    gpsLocExt.navSolutionMask = getLocNavSolutionMaskFromPBLocNavSolutionMask(
            pbGpsLocExt.navsolutionmask());

    // >> uint32 tech_mask = 19; PBLocPosTechMask
    gpsLocExt.tech_mask = getLocPosTechMaskFromPBLocPosTechMask(pbGpsLocExt.tech_mask());

    // >> PBLocSvInfoSource sv_source = 20;
    gpsLocExt.sv_source = getEnumForPBLocSvInfoSource(pbGpsLocExt.sv_source());

    // >> PBGnssLocationPositionDynamics bodyFrameData = 21;
    GnssLocationPositionDynamics &gnssLocPosDynmcs = gpsLocExt.bodyFrameData;
    const PBGnssLocationPositionDynamics &pbGnssLocPosDynmcs = pbGpsLocExt.bodyframedata();
    // >>>> uint32 bodyFrameDataMask = 1;
    gnssLocPosDynmcs.bodyFrameDataMask = getGnssLocationPosDataMaskFromPB(
            pbGnssLocPosDynmcs.bodyframedatamask());
    // >>>> float longAccel = 2;
    // >>>> float latAccel = 3;
    // >>>> float vertAccel = 4;
    gnssLocPosDynmcs.longAccel = pbGnssLocPosDynmcs.longaccel();
    gnssLocPosDynmcs.latAccel = pbGnssLocPosDynmcs.lataccel();
    gnssLocPosDynmcs.vertAccel = pbGnssLocPosDynmcs.vertaccel();
    // >>>> float yawRate = 5;
    // >>>> float pitch = 6;
    gnssLocPosDynmcs.yawRate = pbGnssLocPosDynmcs.yawrate();
    gnssLocPosDynmcs.pitch = pbGnssLocPosDynmcs.pitch();
    // >>>> float longAccelUnc = 7;
    // >>>> float latAccelUnc = 8;
    // >>>> float vertAccelUnc = 9;
    gnssLocPosDynmcs.longAccelUnc = pbGnssLocPosDynmcs.longaccelunc();
    gnssLocPosDynmcs.latAccelUnc = pbGnssLocPosDynmcs.lataccelunc();
    gnssLocPosDynmcs.vertAccelUnc = pbGnssLocPosDynmcs.vertaccelunc();
    // >>>> float yawRateUnc = 10;
    // >>>> float pitchUnc = 11;
    gnssLocPosDynmcs.yawRateUnc = pbGnssLocPosDynmcs.yawrateunc();
    gnssLocPosDynmcs.pitchUnc = pbGnssLocPosDynmcs.pitchunc();

    // >>>> PBGPSTimeStruct gpsTime = 22;
    const PBGPSTimeStruct &pPbGpsTimeStruct = pbGpsLocExt.gpstime();
    GPSTimeStruct &gpsTimeStruct = gpsLocExt.gpsTime;
    gpsTimeStruct.gpsWeek = pPbGpsTimeStruct.gpsweek();
    gpsTimeStruct.gpsTimeOfWeekMs = pPbGpsTimeStruct.gpstimeofweekms();

    // >> PBGnssSystemTime gnssSystemTime = 23;
    pbConvertToGnssSystemTime(pbGpsLocExt.gnsssystemtime(), gpsLocExt.gnssSystemTime);

    // >> PBLocExtDOP extDOP = 24;
    LocExtDOP &gpsLocExtDop = gpsLocExt.extDOP;
    const PBLocExtDOP &pbLocExtDop = pbGpsLocExt.extdop();
    gpsLocExtDop.PDOP = pbLocExtDop.pdop();
    gpsLocExtDop.HDOP = pbLocExtDop.hdop();
    gpsLocExtDop.VDOP = pbLocExtDop.vdop();
    gpsLocExtDop.GDOP = pbLocExtDop.gdop();
    gpsLocExtDop.TDOP = pbLocExtDop.tdop();

    // >> float northStdDeviation = 25;
    // >> float eastStdDeviation = 26;
    // >> float northVelocity = 27;
    // >> float eastVelocity = 28;
    // >> float upVelocity = 29;
    gpsLocExt.northStdDeviation = pbGpsLocExt.northstddeviation();
    gpsLocExt.eastStdDeviation = pbGpsLocExt.eaststddeviation();
    gpsLocExt.northVelocity = pbGpsLocExt.northvelocity();
    gpsLocExt.eastVelocity = pbGpsLocExt.eastvelocity();
    gpsLocExt.upVelocity = pbGpsLocExt.upvelocity();

    // >> float northVelocityStdDeviation = 30;
    // >> float eastVelocityStdDeviation = 31;
    // >> float upVelocityStdDeviation = 32;
    gpsLocExt.northVelocityStdDeviation = pbGpsLocExt.northvelocitystddeviation();
    gpsLocExt.eastVelocityStdDeviation = pbGpsLocExt.eastvelocitystddeviation();
    gpsLocExt.upVelocityStdDeviation = pbGpsLocExt.upvelocitystddeviation();

    // >> float clockbiasMeter = 33;
    // >> float clockBiasStdDeviationMeter = 34;
    // >> float clockDrift = 35;
    // >> float clockDriftStdDeviation = 36;
    gpsLocExt.clockbiasMeter = pbGpsLocExt.clockbiasmeter();
    gpsLocExt.clockBiasStdDeviationMeter = pbGpsLocExt.clockbiasstddeviationmeter();
    gpsLocExt.clockDrift = pbGpsLocExt.clockdrift();
    gpsLocExt.clockDriftStdDeviation = pbGpsLocExt.clockdriftstddeviation();

    // >> uint32 numValidRefStations = 37;
    gpsLocExt.numValidRefStations = pbGpsLocExt.numvalidrefstations();

    // >> repeated uint32 referenceStation = 38;
    int i=0;
    int num_ref_stations = gpsLocExt.numValidRefStations;
    for (i=0; i < num_ref_stations; i++) {
        gpsLocExt.referenceStation[i] = pbGpsLocExt.referencestation(i);
    }

    // >> uint32 numOfMeasReceived = 39;
    gpsLocExt.numOfMeasReceived = pbGpsLocExt.numofmeasreceived();

    // >> repeated PBGpsMeasUsageInfo measUsageInfo = 40;
    int num_sveas = gpsLocExt.numOfMeasReceived;
    for (i=0; i < num_sveas; i++) {
        const PBGpsMeasUsageInfo &pbGpsMeasUsageInfo = pbGpsLocExt.measusageinfo(i);
        GpsMeasUsageInfo &measUsageInfo = gpsLocExt.measUsageInfo[i];
        // >> uint32 gnssSignalType = 1;
        measUsageInfo.gnssSignalType = getGnssSignalTypeMaskFromPBGnssSignalInfoMask(
                pbGpsMeasUsageInfo.gnsssignaltype());
        // >> PBGnss_LocSvSystemEnumType gnssConstellation = 2;
        measUsageInfo.gnssConstellation = getEnumForPBGnss_LocSvSystemEnumType(
                pbGpsMeasUsageInfo.gnssconstellation());
        // >> uint32 gnssSvId = 3;
        measUsageInfo.gnssSvId = pbGpsMeasUsageInfo.gnsssvid();
        // >> uint32 gloFrequency = 4;
        // >> float pseudorangeResidual = 8;
        // >> float dopplerResidual = 9;
        // >> float carrierPhaseResidual = 10;
        // >> float carrierPhasAmbiguity = 11;
        measUsageInfo.gloFrequency = pbGpsMeasUsageInfo.glofrequency();
        measUsageInfo.pseudorangeResidual = pbGpsMeasUsageInfo.pseudorangeresidual();
        measUsageInfo.dopplerResidual = pbGpsMeasUsageInfo.dopplerresidual();
        measUsageInfo.carrierPhaseResidual = pbGpsMeasUsageInfo.carrierphaseresidual();
        measUsageInfo.carrierPhasAmbiguity = pbGpsMeasUsageInfo.carrierphasambiguity();
        // >> uint32 validityMask = 7;
        measUsageInfo.validityMask = getGnssMeasUsageInfoValidityMaskFromPB(
                pbGpsMeasUsageInfo.validitymask());
        // >> uint32 measUsageStatusMask = 6;
        measUsageInfo.measUsageStatusMask = getGnssMeasUsageStatusBitMaskFromPB(
                pbGpsMeasUsageInfo.measusagestatusmask());
        // >> PBCarrierPhaseAmbiguityType carrierPhaseAmbiguityType = 5;
        measUsageInfo.carrierPhaseAmbiguityType = getEnumForPBCarrierPhaseAmbiguityType(
                pbGpsMeasUsageInfo.carrierphaseambiguitytype());
    }

    // >> uint32 leapSeconds = 41;
    gpsLocExt.leapSeconds = pbGpsLocExt.leapseconds();

    // >> float timeUncMs = 42;
    gpsLocExt.timeUncMs = pbGpsLocExt.timeuncms();

    // >> float headingRateDeg = 43;
    gpsLocExt.headingRateDeg = pbGpsLocExt.headingratedeg();

    // >> uint32 calibrationConfidence = 44;
    gpsLocExt.calibrationConfidence = pbGpsLocExt.calibrationconfidence();

    // >> uint32 calibrationStatus = 45; PBDrCalibrationStatusMask
    gpsLocExt.calibrationStatus = getDrCalibrationStatusMaskFromPBDrCalibrationStatusMask(
            pbGpsLocExt.calibrationstatus());

    // >> PBLocOutputEngineType locOutputEngType = 46;
    gpsLocExt.locOutputEngType = getEnumForPBLocOutputEngineType(pbGpsLocExt.locoutputengtype());

    // >> uint32 locOutputEngMask = 47;PBPositioningEngineMask
    gpsLocExt.locOutputEngMask =
            getPositioningEngineMaskFromPBPositioningEngineMask(pbGpsLocExt.locoutputengmask());
    LOC_LOGd("pbConvertToGpsLocationExtended GpsLocExt flags %x tech_mask %d",
            gpsLocExt.flags, gpsLocExt.tech_mask);

    // PBLocDgnssCorrectionSourceType dgnssCorrectionSourceType = 48;
    gpsLocExt.dgnssCorrectionSourceType =
            getEnumForPBLocDgnssCorrectionSourceType(pbGpsLocExt.dgnsscorrectionsourcetype());
    // uint32 dgnssCorrectionSourceID = 49;
    gpsLocExt.dgnssCorrectionSourceID = pbGpsLocExt.dgnsscorrectionsourceid();

    // uint32 dgnssConstellationUsageMask = 50; (PBGnssConstellationInfoMask)
    gpsLocExt.dgnssConstellationUsage =
            getGnssConstellationTypeMaskFromPBGnssConstellationInfoMask(
            pbGpsLocExt.dgnssconstellationusagemask());

    // uint32 dgnssRefStationId = 51;
    gpsLocExt.dgnssRefStationId = pbGpsLocExt.dgnssrefstationid();
    // uint32 dgnssDataAgeMsec = 52;
    gpsLocExt.dgnssDataAgeMsec = pbGpsLocExt.dgnssdataagemsec();
    /** uint32 solutionStatusMask = 53; */
    gpsLocExt.drSolutionStatusMask = getDrSolutionStatusMaskFromPBSolutionStatusMask(
                                     pbGpsLocExt.solutionstatusmask());
    return 0;
}

int EngineHubMsgConverter::pbConvertToLocGpsLocation(const PBLocGpsLocation &pbLocGpsLoc,
        LocGpsLocation &locGpsLoc) {
    // uint32    flags = 1; - PBLocGpsLocationFlags
    locGpsLoc.flags = getLocGpsLocationFlagsFromPBLocGpsLocationFlags(pbLocGpsLoc.flags());
    // uint32    spoof_mask = 2;
    locGpsLoc.spoof_mask = getLocGpsSpoofMaskFromPBLocGpsSpoofMask(pbLocGpsLoc.spoof_mask());
    // float     latitude = 3;
    // float     longitude = 4;
    // float     altitude = 5;
    locGpsLoc.latitude = pbLocGpsLoc.latitude();
    locGpsLoc.longitude = pbLocGpsLoc.longitude();
    locGpsLoc.altitude = pbLocGpsLoc.altitude();
    // float      speed = 6;
    // float      bearing = 7;
    // float      accuracy = 8;
    // float      vertUncertainity = 9;
    locGpsLoc.speed = pbLocGpsLoc.speed();
    locGpsLoc.bearing = pbLocGpsLoc.bearing();
    locGpsLoc.accuracy = pbLocGpsLoc.accuracy();
    locGpsLoc.vertUncertainity = pbLocGpsLoc.vertuncertainity();
    // uint64     timestamp = 10;
    locGpsLoc.timestamp = pbLocGpsLoc.timestamp();
    LOC_LOGd("pbConvertToLocGpsLocation Lat %lf Lon %lf Alt %lf timestamp %" PRIu64,
            locGpsLoc.latitude, locGpsLoc.longitude, locGpsLoc.altitude, locGpsLoc.timestamp);
    return 0;
}

int EngineHubMsgConverter::pbConvertToGnssSystemTime(const PBGnssSystemTime &pbGnssSysTime,
            GnssSystemTime &gnssSysTime) {
    // PBGnss_LocSvSystemEnumType gnssSystemTimeSrc = 1;
    gnssSysTime.gnssSystemTimeSrc =
            getEnumForPBGnss_LocSvSystemEnumType(pbGnssSysTime.gnsssystemtimesrc());
    LOC_LOGd("pbConvertToGnssSystemTime Gnss SystemTime src: %d", gnssSysTime.gnssSystemTimeSrc);
    // PBSystemTimeStructUnion u = 2;
    const PBSystemTimeStructUnion &pbSysStructUnion = pbGnssSysTime.u();
    if (GNSS_LOC_SV_SYSTEM_GPS == gnssSysTime.gnssSystemTimeSrc) {
        // >> PBGnssSystemTimeStructType gpsSystemTime = 1;
        const PBGnssSystemTimeStructType &pbGpsSystemTime = pbSysStructUnion.gpssystemtime();
        pbConvertToGnssSystemTimeStructType(pbGpsSystemTime, gnssSysTime.u.gpsSystemTime);
    } else if (GNSS_LOC_SV_SYSTEM_GALILEO == gnssSysTime.gnssSystemTimeSrc) {
        // >> PBGnssSystemTimeStructType galSystemTime = 2;
        const PBGnssSystemTimeStructType &pbGalSystemTime = pbSysStructUnion.galsystemtime();
        pbConvertToGnssSystemTimeStructType(pbGalSystemTime, gnssSysTime.u.galSystemTime);
    } else if (GNSS_LOC_SV_SYSTEM_GLONASS == gnssSysTime.gnssSystemTimeSrc) {
        // >> PBGnssGloTimeStructType    gloSystemTime = 5;
        const PBGnssGloTimeStructType &pbGloSystemTime = pbSysStructUnion.glosystemtime();
        pbConvertToGnssGloTimeStructType(pbGloSystemTime, gnssSysTime.u.gloSystemTime);
    } else if (GNSS_LOC_SV_SYSTEM_BDS == gnssSysTime.gnssSystemTimeSrc) {
        // >> PBGnssSystemTimeStructType bdsSystemTime = 3;
        const PBGnssSystemTimeStructType &pbBdsSystemTime = pbSysStructUnion.bdssystemtime();
        pbConvertToGnssSystemTimeStructType(pbBdsSystemTime, gnssSysTime.u.bdsSystemTime);
    } else if (GNSS_LOC_SV_SYSTEM_QZSS == gnssSysTime.gnssSystemTimeSrc) {
        // >> PBGnssSystemTimeStructType qzssSystemTime = 4;
        const PBGnssSystemTimeStructType &pbQzssSystemTime = pbSysStructUnion.qzsssystemtime();
        pbConvertToGnssSystemTimeStructType(pbQzssSystemTime, gnssSysTime.u.qzssSystemTime);
    }
    return 0;
}

int EngineHubMsgConverter::pbConvertToGnssSystemTimeStructType(
        const PBGnssSystemTimeStructType &pbGnssSysTimeInfo,
        GnssSystemTimeStructType &gnssSystemTime) {
    // uint32 validityMask = 1; - PBGnssSystemTimeStructTypeFlags
    gnssSystemTime.validityMask =
            getGnssSystemTimeStructTypeFlagsFromPB(pbGnssSysTimeInfo.validitymask());
    // uint32 systemWeek = 2;
    // uint32 systemMsec = 3;
    gnssSystemTime.systemWeek = pbGnssSysTimeInfo.systemweek();
    gnssSystemTime.systemMsec = pbGnssSysTimeInfo.systemmsec();
    // float systemClkTimeBias = 4;
    // float systemClkTimeUncMs = 5;
    gnssSystemTime.systemClkTimeBias = pbGnssSysTimeInfo.systemclktimebias();
    gnssSystemTime.systemClkTimeUncMs = pbGnssSysTimeInfo.systemclktimeuncms();
    // uint32 refFCount = 6;
    // uint32 numClockResets = 7;
    gnssSystemTime.refFCount = pbGnssSysTimeInfo.reffcount();
    gnssSystemTime.numClockResets = pbGnssSysTimeInfo.numclockresets();
    LOC_LOGd("pbConvertToGnssSystemTimeStructType Gnss systemWeek: %d", gnssSystemTime.systemWeek);
    return 0;
}

int EngineHubMsgConverter::pbConvertToGnssGloTimeStructType(
        const PBGnssGloTimeStructType &pBGnssGloTimeInfo, GnssGloTimeStructType &gloSystemTime) {
    // uint32 gloDays = 1;
    gloSystemTime.gloDays = pBGnssGloTimeInfo.glodays();
    // uint32 validityMask = 2; - PBGnssGloTimeInfoBits
    gloSystemTime.validityMask =
            getGnssGloTimeStructTypeFlagsFromPB(pBGnssGloTimeInfo.validitymask());
    // uint32 gloMsec = 3;
    gloSystemTime.gloMsec = pBGnssGloTimeInfo.glomsec();
    // float gloClkTimeBias = 4;
    // float gloClkTimeUncMs = 5;
    gloSystemTime.gloClkTimeBias = pBGnssGloTimeInfo.gloclktimebias();
    gloSystemTime.gloClkTimeUncMs = pBGnssGloTimeInfo.gloclktimeuncms();
    // uint32  refFCount = 6;
    // uint32 numClockResets = 7;
    // uint32 gloFourYear = 8;
    gloSystemTime.refFCount = pBGnssGloTimeInfo.reffcount();
    gloSystemTime.numClockResets = pBGnssGloTimeInfo.numclockresets();
    gloSystemTime.gloFourYear = pBGnssGloTimeInfo.glofouryear();
    LOC_LOGd("pbConvertToGnssGloTimeStructType GLO Systime GloDays: %d", gloSystemTime.gloDays);
    return 0;
}
eHubSubModifiers EngineHubMsgConverter::pbConvertToEHubSubModifiers(
        const EHSubModifiers &pbEHSubModifiers) {
    eHubSubModifiers localEhSubModifiers;

    LOC_LOGd("pbConvertToEHubSubModifiers");
    //EHSubscriptionModifier  svMeasSubModifier = 1;
    localEhSubModifiers.svMeasSubModifier =
            pbConvertToEHSubscriptionModifier(pbEHSubModifiers.svmeassubmodifier());
    //EHSubscriptionModifier  nHzSvMeasSubModifier = 2;
    localEhSubModifiers.nHzSvMeasSubModifier =
            pbConvertToEHSubscriptionModifier(pbEHSubModifiers.nhzsvmeassubmodifier());
    //EHSubscriptionModifier  svPolySubModifier = 3;
    localEhSubModifiers.svPolySubModifier =
            pbConvertToEHSubscriptionModifier(pbEHSubModifiers.svpolysubmodifier());
    //EHSubscriptionModifier  svEphSubModifier = 4;
    localEhSubModifiers.svEphSubModifier =
            pbConvertToEHSubscriptionModifier(pbEHSubModifiers.svephsubmodifier());
    //EHSubscriptionModifier  ionoModelSubModifier = 5;
    localEhSubModifiers.ionoModelSubModifier =
            pbConvertToEHSubscriptionModifier(pbEHSubModifiers.ionomodelsubmodifier());
    return localEhSubModifiers;
}

eHubSubscriptionModifier EngineHubMsgConverter::pbConvertToEHSubscriptionModifier(
                const EHSubscriptionModifier &pbEHSubscriptionModifier) {
    eHubSubscriptionModifier localEhSubscriptionModifier;
    //PBGnssConstellationInfoMask constellationMask = 1;
    localEhSubscriptionModifier.constellationMask =
            getGnssConstellationTypeMaskFromPBGnssConstellationInfoMask(
                    pbEHSubscriptionModifier.constellationmask());
    //PBGnssSignalInfoMask gnssSignalMask = 2;
    localEhSubscriptionModifier.gnssSignalMask =
            getGnssSignalTypeMaskFromPBGnssSignalInfoMask(
                    pbEHSubscriptionModifier.gnsssignalmask());
    LOC_LOGd("pbConvertToEHSubscriptionModifier Gnss SignalMask : %x",
            localEhSubscriptionModifier.gnssSignalMask);
    return localEhSubscriptionModifier;
}

int EngineHubMsgConverter::pbPopulateEHMsgWithCounterToSend(const string &ehMsgPayLoad,
        EHMsgId msgId, uint32_t msgCounter, string &os) {
    LOC_LOGd("pbPopulateEHMsgWithCounterToSend for MsgId:%d, MsgCtr:%d", msgId, msgCounter);
    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    // uint32      msgCounter = 1;
    ehMsgWithCtr.set_msgcounter(msgCounter);
    // string      msgSenderName = 2;
    ehMsgWithCtr.set_msgsendername(IPC_MSGSENDER_ENGINE_HUB);
    // bytes       payload = 3;
    ehMsgWithCtr.set_ehmsgpayload(ehMsgPayLoad);

    if (!ehMsgWithCtr.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsgWithCtr failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgReportSvMeas(const GnssSvMeasurementSet &svMeasSet,
        uint64_t ptpTimeNs, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_SV_MEAS);

    // Fill up svmeas
    EHMessageReportSvMeas ehMsgReportSvMeas;
    PBGnssSvMeasurementSet *pPbGnssSvMeasSet = ehMsgReportSvMeas.mutable_svmeasset();
    if (nullptr == pPbGnssSvMeasSet) {
        LOC_LOGe("mutable_svmeasset failed");
        return 1;
    }

    // EHTechId sourceTechId = 1;
    ehMsgReportSvMeas.set_sourcetechid(EH_NODE_GNSS);

    // PBGnssSvMeasurementSet svMeasSet = 2;
    //   >> bool  isNhz = 1;
    pPbGnssSvMeasSet->set_isnhz(svMeasSet.isNhz);
    //   >> uint64  ptpTimeNs = 2;
    pPbGnssSvMeasSet->set_ptptimens(ptpTimeNs);

    //   >> PBGnssSvMeasurementHeader  svMeasSetHeader = 3;
    // Fill GnssSvMeasurementHeader
    PBGnssSvMeasurementHeader* pSvMeasurementHeader = pPbGnssSvMeasSet->mutable_svmeassetheader();
    if (nullptr != pSvMeasurementHeader) {
        if (convertGnssSvMeasToPB(svMeasSet.svMeasSetHeader, pSvMeasurementHeader)) {
            LOC_LOGe("convertGnssSvMeasToPB failed");
            return 1;
        }
    } else {
        LOC_LOGe("mutable_svmeassetheader is NULL");
        return 1;
    }
    //   >> uint32  svMeasCount = 4;
    pPbGnssSvMeasSet->set_svmeascount(svMeasSet.svMeasCount);
    LOC_LOGd("pbPopulateEHMsgReportSvMeas SvMeasCount : %d", svMeasSet.svMeasCount);

    //   >> repeated PBSVMeasurementStructType  svMeas = 5;
    for (uint8_t i = 0; i < svMeasSet.svMeasCount; i++) {
        PBSVMeasurementStructType* svInfo = pPbGnssSvMeasSet->add_svmeas();
        if (nullptr != svInfo) {
            convertGnssSVMeasurementStructTypeToPB(svMeasSet.svMeas[i], svInfo);
        } else {
            LOC_LOGe("mutable_svmeas failed!");
        }
    }
    LOC_LOGd("svMeasHeadSize:%d", pSvMeasurementHeader->ByteSizeLong());

    string ehSubMsg;
    bool bRetVal = ehMsgReportSvMeas.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportSvMeas(ehMsgReportSvMeas);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportSvMeas failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgReportSv(const GnssSvNotification &gnssSvNotif,
        string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_SV);

    EHMessageReportSv ehMsgReportSv;
    PBGnssSvNotification* pbGnssSvNotif = ehMsgReportSv.mutable_svnotification();
    if (nullptr == pbGnssSvNotif) {
        LOC_LOGe("mutable_svnotification failed");
        return 1;
    }

    // EHTechId sourceTechId = 1;
    ehMsgReportSv.set_sourcetechid(EH_NODE_GNSS);
    LOC_LOGd("pbPopulateEHMsgReportSv GnssSvNotif Count : %d", gnssSvNotif.count);

    // PBGnssSvNotification svNotification = 2;
    //   >> bool gnssSignalTypeMaskValid = 1;
    pbGnssSvNotif->set_gnsssignaltypemaskvalid(gnssSvNotif.gnssSignalTypeMaskValid);
    //   >> repeated PBGnssSv gnssSvs = 2;
    for (uint32_t i=0; i < gnssSvNotif.count; i++) {
        PBGnssSv* pPbGnssSv = pbGnssSvNotif->add_gnsssvs();
        const GnssSv &gnssSvs = gnssSvNotif.gnssSvs[i];
        if (nullptr != pPbGnssSv) {
            // uint32 svId = 1;
            pPbGnssSv->set_svid(gnssSvs.svId);
            // PBGnssSvType type = 2;
            pPbGnssSv->set_type(getPBEnumForGnssSvType(gnssSvs.type));
            // float cN0Dbhz = 3;
            pPbGnssSv->set_cn0dbhz(gnssSvs.cN0Dbhz);
            // float elevation = 4;
            // float azimuth = 5;
            pPbGnssSv->set_elevation(gnssSvs.elevation);
            pPbGnssSv->set_azimuth(gnssSvs.azimuth);
            // uint32 gnssSvOptionsMask = 6;
            pPbGnssSv->set_gnsssvoptionsmask(
                    getPBMaskForGnssSvOptionsMask(gnssSvs.gnssSvOptionsMask));
            // float carrierFrequencyHz = 7;
            pPbGnssSv->set_carrierfrequencyhz(gnssSvs.carrierFrequencyHz);
            // uint32 gnssSignalTypeMask = 8;
            pPbGnssSv->set_gnsssignaltypemask(
                    getPBMaskForGnssSignalTypeMask(gnssSvs.gnssSignalTypeMask));
        } else {
            LOC_LOGe("add_gnsssvs failed!");
        }
    }

    string ehSubMsg;
    bool bRetVal = ehMsgReportSv.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportSv(ehMsgReportSv);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportSv failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgSvPolynomial(const GnssSvPolynomial &svPoly,
        string &os) {
    EngineHubMessage ehMsg;
    int i=0;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_SV_POLYNOMIAL);

    EHMessageReportSvPolynomial ehMsgReportSvPoly;
    PBGnssSvPolynomial* pbGnssSvPoly = ehMsgReportSvPoly.mutable_svpolynomial();
    if (nullptr == pbGnssSvPoly) {
        LOC_LOGe("mutable_svnotification failed");
        return 1;
    }

    // EHTechId sourceTechId = 1;
    ehMsgReportSvPoly.set_sourcetechid(EH_NODE_GNSS);
    LOC_LOGd("pbPopulateEHMsgSvPolynomial SvPoly GnssSvId : %d", svPoly.gnssSvId);

    // PBGnssSvPolynomial svPolynomial = 2;
    // uint32 gnssSvId = 1;
    pbGnssSvPoly->set_gnsssvid(svPoly.gnssSvId);
    // uint32 freqNum = 2;
    pbGnssSvPoly->set_freqnum(svPoly.freqNum);
    // uint32  svPolyStatusMaskValidity = 3;
    pbGnssSvPoly->set_svpolystatusmaskvalidity(
            getPBMaskForGnssSvPolyStatusMaskValidity(svPoly.svPolyStatusMaskValidity));
    // uint32  svPolyStatusMask = 4;
    pbGnssSvPoly->set_svpolystatusmask(getPBMaskForGnssSvPolyStatusMask(svPoly.svPolyStatusMask));
    // uint32 is_valid = 5;
    pbGnssSvPoly->set_is_valid(getPBMaskForUlpGnssSvPolyValidFlags(svPoly.is_valid));
    // uint32  iode = 6;
    // float T0 = 7;
    pbGnssSvPoly->set_iode(svPoly.iode);
    pbGnssSvPoly->set_t0(svPoly.T0);
    // repeated float polyCoeffXYZ0 = 8;
    for (i = 0; i < GNSS_SV_POLY_XYZ_0_TH_ORDER_COEFF_MAX_SIZE; i++) {
        pbGnssSvPoly->add_polycoeffxyz0(svPoly.polyCoeffXYZ0[i]);
    }
    // repeated float polyCoefXYZN = 9;
    for (i = 0; i < GNSS_SV_POLY_XYZ_N_TH_ORDER_COEFF_MAX_SIZE; i++) {
        pbGnssSvPoly->add_polycoefxyzn(svPoly.polyCoefXYZN[i]);
    }
    // repeated float  polyCoefOther = 10;
    for (i = 0; i < GNSS_SV_POLY_SV_CLKBIAS_COEFF_MAX_SIZE; i++) {
        pbGnssSvPoly->add_polycoefother(svPoly.polyCoefOther[i]);
    }
    // float  svPosUnc = 11;
    pbGnssSvPoly->set_svposunc(svPoly.svPosUnc);
    // float  ionoDelay = 12;
    // float  ionoDot = 13;
    pbGnssSvPoly->set_ionodelay(svPoly.ionoDelay);
    pbGnssSvPoly->set_ionodot(svPoly.ionoDot);
    // float  sbasIonoDelay = 14;
    // float  sbasIonoDot = 15;
    pbGnssSvPoly->set_sbasionodelay(svPoly.sbasIonoDelay);
    pbGnssSvPoly->set_sbasionodot(svPoly.sbasIonoDot);
    // float  tropoDelay = 16;
    pbGnssSvPoly->set_tropodelay(svPoly.tropoDelay);
    // float  elevation = 17;
    // float  elevationDot = 18;
    // float  elevationUnc = 19;
    pbGnssSvPoly->set_elevation(svPoly.elevation);
    pbGnssSvPoly->set_elevationdot(svPoly.elevationDot);
    pbGnssSvPoly->set_elevationunc(svPoly.elevationUnc);
    // repeated float velCoef = 20;
    for (i = 0; i < GNSS_SV_POLY_VELOCITY_COEF_MAX_SIZE; i++) {
        pbGnssSvPoly->add_velcoef(svPoly.velCoef[i]);
    }
    // uint32  enhancedIOD = 21;
    pbGnssSvPoly->set_enhancediod(svPoly.enhancedIOD);
    // float gpsIscL1ca = 22;
    // float gpsIscL2c = 23;
    // float gpsIscL5I5 = 24;
    // float gpsIscL5Q5 = 25;
    // float gpsTgd = 26;
    pbGnssSvPoly->set_gpsiscl1ca(svPoly.gpsIscL1ca);
    pbGnssSvPoly->set_gpsiscl2c(svPoly.gpsIscL2c);
    pbGnssSvPoly->set_gpsiscl5i5(svPoly.gpsIscL5I5);
    pbGnssSvPoly->set_gpsiscl5q5(svPoly.gpsIscL5Q5);
    pbGnssSvPoly->set_gpstgd(svPoly.gpsTgd);
    // float gloTgdG1G2 = 27;
    pbGnssSvPoly->set_glotgdg1g2(svPoly.gloTgdG1G2);
    // float bdsTgdB1 = 28;
    // float bdsTgdB2 = 29;
    // float bdsTgdB2a = 30;
    // float bdsIscB2a = 31;
    pbGnssSvPoly->set_bdstgdb1(svPoly.bdsTgdB1);
    pbGnssSvPoly->set_bdstgdb2(svPoly.bdsTgdB2);
    pbGnssSvPoly->set_bdstgdb2a(svPoly.bdsTgdB2a);
    pbGnssSvPoly->set_bdsiscb2a(svPoly.bdsIscB2a);
    // float galBgdE1E5a = 32;
    // float galBgdE1E5b = 33;
    pbGnssSvPoly->set_galbgde1e5a(svPoly.galBgdE1E5a);
    pbGnssSvPoly->set_galbgde1e5b(svPoly.galBgdE1E5b);
    // float navicTgdL5 = 34;
    pbGnssSvPoly->set_navictgdl5(svPoly.navicTgdL5);
    // uint32 polyOrder = 35;
    pbGnssSvPoly->set_polyorder(svPoly.polyOrder);
    // uint32 validDuration = 36;
    pbGnssSvPoly->set_validduration(svPoly.validDuration);
    // repeated double polyCoeffXYZ = 37;
    for (int i =0; i < GNSS_SV_POLY_XYZ_COEFF_SIZE_MAX; i++) {
        pbGnssSvPoly->add_polycoeffxyz(svPoly.polyCoeffXYZ[i]);
    }
    // repeated double polyClockBias = 38;
    for (int i =0; i < GNSS_SV_POLY_CLKBIAS_COEFF_SIZE_MAX; i++) {
        pbGnssSvPoly->add_polyclockbias(svPoly.polyClockBias[i]);
    }
    string ehSubMsg;
    bool bRetVal = ehMsgReportSvPoly.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportSvPolynomial(ehMsgReportSvPoly);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportSvPoly failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgSvEphemeris(const GnssSvEphemerisReport &gnssEph,
        string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_SV_EPHEMERIS);

    EHMessageReportSvEphemeris ehMsgReportSvEph;
    // EHTechId sourceTechId = 1;
    ehMsgReportSvEph.set_sourcetechid(EH_NODE_GNSS);
    LOC_LOGd("pbPopulateEHMsgSvEphemeris");

    // PBGnssSvEphemerisReport svEphemeris = 2;
    PBGnssSvEphemerisReport* svEph = ehMsgReportSvEph.mutable_svephemeris();
    if (nullptr != svEph) {
        // PBGnss_LocSvSystemEnumType gnssConstellation = 1;
        svEph->set_gnssconstellation(
                getPBEnumForGnssLocSvSystem(gnssEph.gnssConstellation));
        // bool isSystemTimeValid = 2;
        svEph->set_issystemtimevalid(gnssEph.isSystemTimeValid);

        // PBGnssSystemTimeStructType systemTime = 3;
        PBGnssSystemTimeStructType* pGnssSysTime = svEph->mutable_systemtime();
        if (nullptr != pGnssSysTime) {
            if (convertGnssSystemTimeStructToPB(gnssEph.systemTime, pGnssSysTime)) {
                LOC_LOGe("convertGnssSystemTimeStructToPB failed");
            }
        } else {
            LOC_LOGe("mutable_systemtime failed");
        }

        switch (gnssEph.gnssConstellation) {
            case GNSS_LOC_SV_SYSTEM_GPS:
            {
                // PBGpsEphemerisResponse gpsEphemeris = 4;
                PBGpsEphemerisResponse* pbGpsEph = svEph->mutable_gpsephemeris();
                if (nullptr != pbGpsEph) {
                    if (convertGpsEphRespToPB(gnssEph.ephInfo.gpsEphemeris, pbGpsEph)) {
                        LOC_LOGe("convertGpsEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_gpsephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_GALILEO:
            {
                // PBGalileoEphemerisResponse galileoEphemeris = 7;
                PBGalileoEphemerisResponse* pbGalEph = svEph->mutable_galileoephemeris();
                if (nullptr != pbGalEph) {
                    if (convertGalileoEphRespToPB(gnssEph.ephInfo.galileoEphemeris, pbGalEph)) {
                        LOC_LOGe("convertGalileoEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_galileoephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_GLONASS:
            {
                // PBGlonassEphemerisResponse glonassEphemeris = 5;
                PBGlonassEphemerisResponse* pbGloEph = svEph->mutable_glonassephemeris();
                if (nullptr != pbGloEph) {
                    if (convertGlonassEphRespToPB(gnssEph.ephInfo.glonassEphemeris, pbGloEph)) {
                        LOC_LOGe("convertGlonassEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_glonassephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_BDS:
            {
                // PBBdsEphemerisResponse bdsEphemeris = 6;
                PBBdsEphemerisResponse* pbBdsEph = svEph->mutable_bdsephemeris();
                if (nullptr != pbBdsEph) {
                    if (convertBdsEphRespToPB(gnssEph.ephInfo.bdsEphemeris, pbBdsEph)) {
                        LOC_LOGe("convertBdsEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_bdsephemeris failed");
                }
            }
            break;
            case GNSS_LOC_SV_SYSTEM_QZSS:
            {
                // PBQzssEphemerisResponse qzssEphemeris = 8;
                PBQzssEphemerisResponse* pQzssEph = svEph->mutable_qzssephemeris();
                if (nullptr != pQzssEph) {
                    if (convertQzssEphRespToPB(gnssEph.ephInfo.qzssEphemeris, pQzssEph)) {
                        LOC_LOGe("convertQzssEphRespToPB failed");
                    }
                } else {
                    LOC_LOGe("mutable_qzssephemeris failed");
                }
            }
            break;
            default:
            break;
        }
    } else {
        LOC_LOGe("mutable_svephemeris failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehMsgReportSvEph.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportSvEphemeris(ehMsgReportSvEph);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportSvEph failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgReportPosition(const UlpLocation &location,
        const GpsLocationExtended &locationExtended, enum loc_sess_status status, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_POSITION);

    EHMessageReportPosition ehMsgReportPos;
    // EHTechId sourceTechId = 1;
    ehMsgReportPos.set_sourcetechid(EH_NODE_GNSS);
    // PBLocSessionStatus status = 4;
    ehMsgReportPos.set_status(getPBEnumForLocSessStatus(status));
    LOC_LOGd("pbPopulateEHMsgReportPosition : Loc Sess status: %d", status);

    // PBUlpLocation location = 2;
    PBUlpLocation *ulpLoc = ehMsgReportPos.mutable_location();
    if (nullptr != ulpLoc) {
        if (convertUlpLocationToPB(location, ulpLoc)) {
            LOC_LOGe("convertUlpLocationToPB failed");
        }
    } else {
        LOC_LOGe("mutable_location failed");
        return 1;
    }

    // PBGpsLocationExtended locationExtended = 3;
    PBGpsLocationExtended *gpsLocExt = ehMsgReportPos.mutable_locationextended();
    if (nullptr != gpsLocExt) {
        if (convertGpsLocExtToPB(locationExtended, gpsLocExt)) {
            LOC_LOGe("convertGpsLocExtToPB failed");
        }
    } else {
        LOC_LOGe("mutable_locationextended failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehMsgReportPos.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportPosition(ehMsgReportPos);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportPos failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgKlobucharIonoModel(
        const GnssKlobucharIonoModel &klobIonoMdl, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_KLOBUCHAR_IONO_MODEL);

    EHMessageReportKlobucharIonoModel ehMsgReportKlobIono;
    PBGnssKlobucharIonoModel *pbKlobucharIonoModel =
            ehMsgReportKlobIono.mutable_klobucharionomodel();
    if (nullptr == pbKlobucharIonoModel) {
        LOC_LOGe("mutable_klobucharionomodel failed");
        return 1;
    }
    LOC_LOGd("pbPopulateEHMsgKlobucharIonoModel");
    // EHTechId sourceTechId = 1;
    ehMsgReportKlobIono.set_sourcetechid(EH_NODE_GNSS);

    // PBGnssKlobucharIonoModel klobucharIonoModel = 2;
    // bool isSystemTimeValid = 1;
    pbKlobucharIonoModel->set_issystemtimevalid(klobIonoMdl.isSystemTimeValid);
    // PBGnssSystemTimeStructType systemTime = 2;
    PBGnssSystemTimeStructType* pGnssSysTime = pbKlobucharIonoModel->mutable_systemtime();
    if (nullptr != pGnssSysTime) {
        if (convertGnssSystemTimeStructToPB(klobIonoMdl.systemTime, pGnssSysTime)) {
            LOC_LOGe("convertGnssSystemTimeStructToPB failed");
        }
    } else {
        LOC_LOGe("mutable_systemtime failed");
        return 1;
    }
    // PBGnss_LocSvSystemEnumType gnssConstellation = 3;
    pbKlobucharIonoModel->set_gnssconstellation(
            getPBEnumForGnssLocSvSystem(klobIonoMdl.gnssConstellation));
    // float alpha0 = 4;
    // float alpha1 = 5;
    // float alpha2 = 6;
    // float alpha3 = 7;
    pbKlobucharIonoModel->set_alpha0(klobIonoMdl.alpha0);
    pbKlobucharIonoModel->set_alpha1(klobIonoMdl.alpha1);
    pbKlobucharIonoModel->set_alpha2(klobIonoMdl.alpha2);
    pbKlobucharIonoModel->set_alpha3(klobIonoMdl.alpha3);
    // float beta0 = 8;
    // float beta1 = 9;
    // float beta2 = 10;
    // float beta3 = 11;
    pbKlobucharIonoModel->set_beta0(klobIonoMdl.beta0);
    pbKlobucharIonoModel->set_beta1(klobIonoMdl.beta1);
    pbKlobucharIonoModel->set_beta2(klobIonoMdl.beta2);
    pbKlobucharIonoModel->set_beta3(klobIonoMdl.beta3);

    string ehSubMsg;
    bool bRetVal = ehMsgReportKlobIono.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportKlobucharIonoModel(ehMsgReportKlobIono);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportKlobIono failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMsgGlonassAddnParams(
        const GnssAdditionalSystemInfo &gnssAddSysInfo, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_GLONASS_ADDITIONAL_PARAMS);

    EHMessageReportGlonassAdditionalParams ehMsgReportGlonassAddParam;
    PBGnssAdditionalSystemInfo *pbAdditionalSystemInfo =
            ehMsgReportGlonassAddParam.mutable_additionalsysteminfo();
    if (nullptr == pbAdditionalSystemInfo) {
        LOC_LOGe("mutable_additionalsysteminfo failed");
        return 1;
    }
    // EHTechId sourceTechId = 1;
    ehMsgReportGlonassAddParam.set_sourcetechid(EH_NODE_GNSS);
    LOC_LOGd("pbPopulateEHMsgGlonassAddnParams");

    // PBGnssAdditionalSystemInfo additionalSystemInfo = 2;
    // bool isSystemTimeValid = 1;
    pbAdditionalSystemInfo->set_issystemtimevalid(gnssAddSysInfo.isSystemTimeValid);
    // PBGnssSystemTimeStructType systemTime = 2;
    PBGnssSystemTimeStructType* pGnssSysTime = pbAdditionalSystemInfo->mutable_systemtime();
    if (nullptr != pGnssSysTime) {
        if (convertGnssSystemTimeStructToPB(gnssAddSysInfo.systemTime, pGnssSysTime)) {
            LOC_LOGe("convertGnssSystemTimeStructToPB failed");
        }
    } else {
        LOC_LOGe("mutable_systemtime failed");
        return 1;
    }
    // uint32 validityMask = 3; - PBGnssAdditionalSystemInfoMask
    pbAdditionalSystemInfo->set_validitymask(
            getPBMaskForGnssAdditionalSystemInfoMask(gnssAddSysInfo.validityMask));
    // float tauC = 4;
    pbAdditionalSystemInfo->set_tauc((float)gnssAddSysInfo.tauC);
    // uint32 leapSec = 5;
    pbAdditionalSystemInfo->set_leapsec(gnssAddSysInfo.leapSec);

    string ehSubMsg;
    bool bRetVal = ehMsgReportGlonassAddParam.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportGlonassAdditionalParams(ehMsgReportGlonassAddParam);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgReportGlonassAddParam failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageSessionStart(string &pbStr) {
    EngineHubMessage startMsg;
    startMsg.set_msgid(EH_MSG_ID_SESSION_START);
    if (!startMsg.SerializeToString(&pbStr)) {
        LOC_LOGe("SerializeToString on startMsg failed!");
        return 1;
    } else {
        LOC_LOGd("startMsg protobuff output length: %d", pbStr.size());
        return 0;
    }
}

int EngineHubMsgConverter::pbPopulateEHMessageSessionStop(string &pbStr) {
    EngineHubMessage stopMsg;
    stopMsg.set_msgid(EH_MSG_ID_SESSION_STOP);
    if (!stopMsg.SerializeToString(&pbStr)) {
        LOC_LOGe("SerializeToString on stopMsg failed!");
        return 1;
    } else {
        LOC_LOGd("stopMsg protobuff output length: %d", pbStr.size());
        return 0;
    }
}

int EngineHubMsgConverter::pbPopulateEHMessageProtocolEngineHubUp(string &pbStr) {
    EngineHubMessage engHubUp;
    engHubUp.set_msgid(EH_MSG_ID_PROTOCOL_ENGINE_HUB_UP);
    if (!engHubUp.SerializeToString(&pbStr)) {
        LOC_LOGe("SerializeToString on engHubUp failed!");
        return 1;
    } else {
        LOC_LOGd("engHubUp protobuff output length: %d", pbStr.size());
        return 0;
    }
}

int EngineHubMsgConverter::pbPopulateEHMessageStatusConnectivity(string &pbStr, bool connected) {
    string ehSubMsg;
    EngineHubMessage connectMsg;
    connectMsg.set_msgid(EH_MSG_ID_STATUS_CONNECTIVITY);

    EHMessageStatusConnectivity connectStatus;
    connectStatus.set_isconnected(connected);
    if (!connectStatus.SerializeToString(&ehSubMsg)) {
        LOC_LOGe("SerializeToString on connectStatus failed!");
        return 1;
    } else {
        connectMsg.set_payload(ehSubMsg);
        if (!connectMsg.SerializeToString(&pbStr)) {
            LOC_LOGe("SerializeToString on EHMessageStatusConnectivity failed!");
            return 1;
        } else {
            LOC_LOGd("connectStatus protobuff output length: %d", pbStr.size());
            return 0;
        }
    }
}

int EngineHubMsgConverter::pbPopulateEHMessageStatusLeverArmConfig(
        const LeverArmConfigInfo &leverArmConfgInfo, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_STATUS_LEVER_ARM_CONFIG);

    EHMessageStatusLeverArmConfig ehLeverArmConfig;
    // >> PBLeverArmConfigInfo leverArmInfo = 1;
    PBLeverArmConfigInfo *pbLeverArmConfgInfo = ehLeverArmConfig.mutable_leverarminfo();
    if (nullptr != pbLeverArmConfgInfo) {
        if (convertLeverArmConfigInfoToPB(leverArmConfgInfo, pbLeverArmConfgInfo)) {
            LOC_LOGe("convertLeverArmConfigInfoToPB failed");
        }
    } else {
        LOC_LOGe("mutable_leverarminfo failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehLeverArmConfig.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageStatusLeverArmConfig(ehLeverArmConfig);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehLeverArmConfig failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageStatusB2sMountParams(
   const BodyToSensorMountParams &b2sParams, string &os) {

    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_STATUS_BODY_TO_SENSOR_MOUNT_PARAMS);

    EHMessageStatusB2sMountParams ehB2sMountParams;
    PBBodyToSensorMountParams *pbB2sMountParams = ehB2sMountParams.mutable_b2smountparams();

    if (nullptr != pbB2sMountParams) {
        if (convertB2sMountParamsToPB(b2sParams, pbB2sMountParams)) {
            LOC_LOGe("convertB2sMountParamsToPB failed");
        }
    } else {
        LOC_LOGe("mmutable_b2smountparams failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehB2sMountParams.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageStatusB2sMountParams(ehB2sMountParams);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehB2sMountParams failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageSessionDeleteAidingData(
        const GnssAidingData &gnssAidData, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_SESSION_DELETE_AIDING_DATA);

    EHMessageSessionDeleteAidingData ehMsgDelAidingData;
    PBGnssAidingData *pbGnssAidingData = ehMsgDelAidingData.mutable_gpsaidingdata();
    if (nullptr == pbGnssAidingData) {
        LOC_LOGe("mutable_gpsaidingdata failed");
        return 1;
    }

    // >> bool deleteAll  = 1;
    pbGnssAidingData->set_deleteall(gnssAidData.deleteAll);
    // >> PBGnssAidingDataSv sv = 2;
    PBGnssAidingDataSv *satVeh = pbGnssAidingData->mutable_sv();
    if (nullptr != satVeh) {
        if (convertGnssAidingDataSvToPB(gnssAidData.sv, satVeh)) {
            LOC_LOGe("convertGnssAidingDataSvToPB failed");
        }
    } else {
        LOC_LOGe("mutable_sv failed");
        return 1;
    }
    // >> PBGnssAidingDataCommon common = 3;
    PBGnssAidingDataCommon *aidDataCommon = pbGnssAidingData->mutable_common();
    if (nullptr != satVeh) {
        if (convertGnssAidingDataCommonToPB(gnssAidData.common, aidDataCommon)) {
            LOC_LOGe("convertGnssAidingDataCommonToPB failed");
        }
    } else {
        LOC_LOGe("mutable_common failed");
        return 1;
    }
    // >> uint32 posEngineMask = 4 (PBPositioningEngineMask);
    pbGnssAidingData->set_posenginemask(
            getPBMaskForPositioningEngineMask(gnssAidData.posEngineMask));

    string ehSubMsg;
    bool bRetVal = ehMsgDelAidingData.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageSessionDeleteAidingData(ehMsgDelAidingData);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgDelAidingData failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    LOC_LOGd("EHMessageSessionDeleteAidingData protobuff output length: %d", os.size());
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageSessionSetFixMode(const LocPosMode &locPosMode,
        string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_SESSION_SET_FIX_MODE);

    EHMessageSessionSetFixMode ehSessSetFixMode;
    // >> PBLocPosMode fixMode = 1;
    PBLocPosMode *pbLocposmode = ehSessSetFixMode.mutable_fixmode();
    if (nullptr != pbLocposmode) {
        if (convertLocPosModeToPB(locPosMode, pbLocposmode)) {
            LOC_LOGe("convertLocPosModeToPB failed");
        }
    } else {
        LOC_LOGe("mutable_fixmode failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehSessSetFixMode.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageSessionSetFixMode(ehSessSetFixMode);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehSessSetFixMode failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    LOC_LOGd("EHMessageSessionSetFixMode protobuff output length: %d", os.size());
    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageProtocolRoutingTableUpdate(
        const EHubMessageProtocolRoutingTableUpdate &eHubMsgPrtclRoutngTblUpdt, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_PROTOCOL_ROUTING_TABLE_UPDATE);

    EHMessageProtocolRoutingTableUpdate pbEhMsgPrtclRoutngTblUpdt;
    // >> uint32 eHubNumEntries = 1;
    pbEhMsgPrtclRoutngTblUpdt.set_ehubnumentries(eHubMsgPrtclRoutngTblUpdt.eHubNumEntries);

    uint32_t num_entries = eHubMsgPrtclRoutngTblUpdt.eHubNumEntries;
    // >> repeated EHRoutingTableEntry eHubRoutingTable = 2;
    for (uint32_t i = 0; i < num_entries; i++) {
        EHRoutingTableEntry *pbEhRoutTblEntry =
                pbEhMsgPrtclRoutngTblUpdt.add_ehubroutingtable();
        if (nullptr != pbEhRoutTblEntry) {
            if (convertEHubRoutingTableEntryToPB(eHubMsgPrtclRoutngTblUpdt.eHubRoutingTable[i],
                    pbEhRoutTblEntry)) {
                LOC_LOGe("convertGnssEphCommonToPB failed");
            }
        } else {
            LOC_LOGe("add_ehubroutingtable is NULL");
        }
    }

    string ehSubMsg;
    bool bRetVal = pbEhMsgPrtclRoutngTblUpdt.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageProtocolRoutingTableUpdate(pbEhMsgPrtclRoutngTblUpdt);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on pbEhMsgPrtclRoutngTblUpdt failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }
    LOC_LOGd("EHMessageProtocolRoutingTableUpdate protobuff output length: %d", os.size());
    return 0;
}

#ifdef FEATURE_CDFW
// Conversion function for QDGnss information to protobuf
int EngineHubMsgConverter::pbPopulateEHMessageQDgnssReferenceStation(
        const ReferenceStation &refStation, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_CDFW_QDGNSS_REF_STATION);

    EHMessageReportCdfwQDgnssReferenceStation  ehMsgQdgnssRefStation;
    // EHTechId sourceTechId = 1;
    ehMsgQdgnssRefStation.set_sourcetechid(EH_NODE_GNSS);

    // PBReferenceStation qDgnssRefStation = 2;
    PBReferenceStation *pbQdgnssRefStation = ehMsgQdgnssRefStation.mutable_qdgnssrefstation();
    if (nullptr != pbQdgnssRefStation) {
        if (convertReferenceStationToPB(refStation, pbQdgnssRefStation)) {
            LOC_LOGe("convertReferenceStationToPB failed");
        }
    } else {
        LOC_LOGe("mutable_qdgnssrefstation failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehMsgQdgnssRefStation.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportCdfwQDgnssReferenceStation(ehMsgQdgnssRefStation);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgQdgnssRefStation failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageQDgnssObservation(
        const DGnssObservation &dGnssObs, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_CDFW_QDGNSS_OBSERVATION);

    EHMessageReportCdfwQDgnssObservation ehMsgQdgnssObs;
    // EHTechId sourceTechId = 1;
    ehMsgQdgnssObs.set_sourcetechid(EH_NODE_GNSS);

    // PBDGnssObservation qDgnssObserv = 2;
    PBDGnssObservation *pbQdgnssObs = ehMsgQdgnssObs.mutable_qdgnssobserv();
    if (nullptr != pbQdgnssObs) {
        if (convertDGnssObservationToPB(dGnssObs, pbQdgnssObs)) {
            LOC_LOGe("convertDGnssObservationToPB failed");
        }
    } else {
        LOC_LOGe("mutable_qdgnssobserv failed");
        return 1;
    }

    string ehSubMsg;
    bool bRetVal = ehMsgQdgnssObs.SerializeToString(&ehSubMsg);
    EHMsgUtils::freeUpEHMessageReportCdfwQDgnssObservation(ehMsgQdgnssObs);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgQdgnssObs failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    return 0;
}
#endif

int EngineHubMsgConverter::pbPopulateEHMessageQWESFeatureStatusInfo(const ehFeatureStatus &status,
        const ehLicenseType &type, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_QWES_FEATURE_STATUS);

    EHMessageQWESFeatureStatusInfo ehMsgFeatureStatus;

    EHFeatureStatus featureStatus = static_cast<EHFeatureStatus>(status);
    EHLicenseType featureType = static_cast<EHLicenseType>(static_cast<uint8_t>(type) -1);
    ehMsgFeatureStatus.set_featurestatus(featureStatus);
    ehMsgFeatureStatus.set_featuretype(featureType);

    string ehSubMsg;
    bool bRetVal = ehMsgFeatureStatus.SerializeToString(&ehSubMsg);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgFeatureStatus failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageQWESInstallLicenseResp(
        const ehInstallLicenseStatus & status,
        string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_PROVIDE_INSTALL_LICENSE_STATUS);

    EHMessageQWESInstallLicenseResp ehMsgInstallLicenseResp;

    EHInstallLicenseStatus ehStatus =
            static_cast<EHInstallLicenseStatus>(static_cast<uint8_t>(status) - 1);
    ehMsgInstallLicenseResp.set_status(ehStatus);

    string ehSubMsg;
    bool bRetVal = ehMsgInstallLicenseResp.SerializeToString(&ehSubMsg);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgInstallLicenseResp failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageQWESProvideAttestationStatement(
        const uint8_t *outBuff, uint32_t bufLen, const ehAttestationStatus &status, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_PROVIDE_ATTESTATION_STATEMENT);

    EHMessageQWESProvideAttestationStatement ehMsgAttestation;
    std::string bufStr(outBuff, outBuff+ bufLen);
    EHAttestationStatus ehStatus =
            static_cast<EHAttestationStatus>(static_cast<uint8_t>(status) - 1);
    ehMsgAttestation.set_outbuff(bufStr);
    ehMsgAttestation.set_buflength(bufLen);
    ehMsgAttestation.set_status(ehStatus);
    string ehSubMsg;
    bool bRetVal = ehMsgAttestation.SerializeToString(&ehSubMsg);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgAttestation failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    return 0;
}

int EngineHubMsgConverter::pbPopulateEHMessageQWESForceSyncNotify(int32_t status, string &os) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_QWES_FORCE_SYNC_NOTIFY);

    EHMessageQWESForceSyncNotify ehMsgQwesSync;
    ehMsgQwesSync.set_status(status);

    string ehSubMsg;
    bool bRetVal = ehMsgQwesSync.SerializeToString(&ehSubMsg);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgQwesSync failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    if (!ehMsg.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    return 0;
}

bool EngineHubMsgConverter::svNeededBasedOnSvSystem(GnssConstellationTypeMask regConstellationMask,
        Gnss_LocSvSystemEnumType gnssSystem) {
    bool retVal = false;

    switch (gnssSystem) {
    case GNSS_LOC_SV_SYSTEM_GPS:
        if (GNSS_CONSTELLATION_TYPE_GPS_BIT & regConstellationMask) {
            retVal = true;
        }
        break;
    case GNSS_LOC_SV_SYSTEM_GALILEO:
        if (GNSS_CONSTELLATION_TYPE_GALILEO_BIT & regConstellationMask) {
            retVal = true;
        }
        break;
    case GNSS_LOC_SV_SYSTEM_SBAS:
        if (GNSS_CONSTELLATION_TYPE_SBAS_BIT & regConstellationMask) {
            retVal = true;
        }
        break;
    case GNSS_LOC_SV_SYSTEM_GLONASS:
        if (GNSS_CONSTELLATION_TYPE_GLONASS_BIT & regConstellationMask) {
            retVal = true;
        }
        break;
    case GNSS_LOC_SV_SYSTEM_BDS:
        if (GNSS_CONSTELLATION_TYPE_BEIDOU_BIT & regConstellationMask) {
            retVal = true;
        }
        break;
    case GNSS_LOC_SV_SYSTEM_QZSS:
        if (GNSS_CONSTELLATION_TYPE_QZSS_BIT & regConstellationMask) {
            retVal = true;
        }
        break;
    }

    return retVal;
}

// Engine Hub Msg converter implementation
static EngineHubMsgConverter gEngHubMsgConverter;

static int pbPopulateEHMsgWithCounterToSend(const string &ehMsgPayLoad, EHMsgId msgId,
        uint32_t msgCounter, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgWithCounterToSend(ehMsgPayLoad, msgId,
            msgCounter, os);
}

static int pbPopulateEHMsgReportSvMeas(const GnssSvMeasurementSet &svMeasSet,
        uint64_t ptpTimeNs, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgReportSvMeas(svMeasSet, ptpTimeNs, os);
}

static int pbPopulateEHMsgReportSv(const GnssSvNotification &gnssSvNotif, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgReportSv(gnssSvNotif, os);
}

static int pbPopulateEHMsgSvPolynomial(const GnssSvPolynomial &svPoly, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgSvPolynomial(svPoly, os);
}

static int pbPopulateEHMsgSvEphemeris(const GnssSvEphemerisReport &gnssEph, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgSvEphemeris(gnssEph, os);
}

static int pbPopulateEHMsgReportPosition(const UlpLocation &location,
        const GpsLocationExtended &locationExtended, enum loc_sess_status status,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgReportPosition(location, locationExtended,
            status, os);
}

static int pbPopulateEHMsgKlobucharIonoModel(const GnssKlobucharIonoModel &klobIonoMdl,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgKlobucharIonoModel(klobIonoMdl, os);
}

static int pbPopulateEHMsgGlonassAddnParams(const GnssAdditionalSystemInfo &gnssAddSysInfo,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMsgGlonassAddnParams(gnssAddSysInfo, os);
}

static int pbPopulateEHMessageSessionStart(string &pbStr) {
    return gEngHubMsgConverter.pbPopulateEHMessageSessionStart(pbStr);
}

static int pbPopulateEHMessageSessionStop(string &pbStr) {
    return gEngHubMsgConverter.pbPopulateEHMessageSessionStop(pbStr);
}

static int pbPopulateEHMessageProtocolEngineHubUp(string &pbStr) {
    return gEngHubMsgConverter.pbPopulateEHMessageProtocolEngineHubUp(pbStr);
}

static int pbPopulateEHMessageStatusConnectivity(string &pbStr, bool connected) {
    return gEngHubMsgConverter.pbPopulateEHMessageStatusConnectivity(pbStr, connected);
}

static int pbPopulateEHMessageStatusLeverArmConfig(const LeverArmConfigInfo &leverArmConfgInfo,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageStatusLeverArmConfig(leverArmConfgInfo, os);
}

static int pbPopulateEHMessageStatusB2sMountParams(const BodyToSensorMountParams &b2sParams,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageStatusB2sMountParams(b2sParams, os);
}

static int pbPopulateEHMessageSessionDeleteAidingData(const GnssAidingData &gnssAidData,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageSessionDeleteAidingData(gnssAidData, os);
}

static int pbPopulateEHMessageSessionSetFixMode(const LocPosMode &locPosMode, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageSessionSetFixMode(locPosMode, os);
}

static int pbPopulateEHMessageProtocolRoutingTableUpdate(
        const EHubMessageProtocolRoutingTableUpdate &eHubMsgPrtclRoutngTblUpdt, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageProtocolRoutingTableUpdate(
            eHubMsgPrtclRoutngTblUpdt, os);
}

#ifdef FEATURE_CDFW
static int pbPopulateEHMessageQDgnssReferenceStation(const ReferenceStation &refStation,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageQDgnssReferenceStation(refStation, os);
}

static int pbPopulateEHMessageQDgnssObservation(const DGnssObservation &dGnssObs, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageQDgnssObservation(dGnssObs, os);
}
#endif

static bool decodeEHubWthMsgCounterPayLoadMsg(const string& eHubWithCtrPayLoadMsg,
        string& engHubMsgPayload, uint32_t &msgId, uint32_t &msgCounter, string &senderName) {
    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    if (!gEngHubMsgConverter.ParseFromString(ehMsgWithCtr, eHubWithCtrPayLoadMsg)) {
        LOC_LOGe("Failed to parse ehMsgWithCtr from input stream!! length: %u",
                eHubWithCtrPayLoadMsg.size());
        return false;
    }
    EngineHubMessage ehMsg;
    if (!gEngHubMsgConverter.ParseFromString(ehMsg, msgCounter, senderName, ehMsgWithCtr)) {
        LOC_LOGe("Failed to parse ehMsg from input stream!! length: %u",
                eHubWithCtrPayLoadMsg.size());
        msgCounter = 0;
        return false;
    }
    msgId = ehMsg.msgid();
    LOC_LOGd("ehMsg.msgid : %d", msgId);
    engHubMsgPayload = ehMsg.payload();
    return true;
}

static EHubMessageProtocolRegMsg* decodeEHRoutingTableEntry(const string& pbEhRoutingTablePayld) {
    // EHRoutingTableEntry -> EHubMessageProtocolRegMsg
    EHRoutingTableEntry pbEngHubRoutTable;
    if (gEngHubMsgConverter.ParseFromString(pbEngHubRoutTable, pbEhRoutingTablePayld)) {
        return gEngHubMsgConverter.decodeEHRoutingTableEntry(pbEngHubRoutTable);
    } else {
        return nullptr;
    }
}

static EHubMessageReportPosition* decodeEHMessageReportPosition(const string& pbEhMsgRptPosPayld) {
    // EHMessageReportPosition --> EHubMessageReportPosition
    EHMessageReportPosition  pbEngHubPosRpt;
    if (gEngHubMsgConverter.ParseFromString(pbEngHubPosRpt, pbEhMsgRptPosPayld)) {
        return gEngHubMsgConverter.decodeEHMessageReportPosition(pbEngHubPosRpt);
    } else {
        return nullptr;
    }
}

static EHubMessageReportSv* decodeEHMessageReportSv(const string& pbEhMsgRprtSvPayload) {
    // EHMessageReportSv --> EHubMessageReportSv
    EHMessageReportSv pbEngHubPosSv;
    if (gEngHubMsgConverter.ParseFromString(pbEngHubPosSv, pbEhMsgRprtSvPayload)) {
        return gEngHubMsgConverter.decodeEHMessageReportSv(pbEngHubPosSv);
    } else {
        return nullptr;
    }
}

static EHubMessageQWESInstallLicenseReq* decodeEHMessageInstallLicenseReq(
        const string& pbEhMsgInstallLicensePayload) {
    EHMessageQWESInstallLicenseReq pbEngHubInstallLicense;
    if (gEngHubMsgConverter.ParseFromString(pbEngHubInstallLicense, pbEhMsgInstallLicensePayload)) {
        return gEngHubMsgConverter.decodeEHMessageInstallLicenseReq(pbEngHubInstallLicense);
    } else {
        return nullptr;
    }
}

static EHubMessageQWESReqAttestationStatement* decodeEHMessageAttestationReq(
        const string& pbEhMsgAttestationPayload) {
    EHMessageQWESReqAttestationStatement pbEngHubAttestation;
    if (gEngHubMsgConverter.ParseFromString(pbEngHubAttestation, pbEhMsgAttestationPayload)) {
        return gEngHubMsgConverter.decodeEHMessageAttestationReq(pbEngHubAttestation);
    } else {
        return nullptr;
    }
}

static bool pbPopulateEHMsgReportSvMeasBasedOnSubModifiers(
        const EHubMessageReportSvMeas* inMsg, GnssConstellationTypeMask regConstellationMask,
        GnssSignalTypeMask regSignalTypeMask, uint8_t * &outMsg, size_t &outMsgSize,
        uint32_t msgCounter) {
    return gEngHubMsgConverter.pbPopulateEHMsgReportSvMeasBasedOnSubModifiers(inMsg,
            regConstellationMask, regSignalTypeMask, outMsg, outMsgSize, msgCounter);
}

static bool pbPopulateEHMsgSvEphemerisBasedOnSubModifiers(
        const EHubMessageReportSvEphemeris* inMsg, GnssSignalTypeMask gnssSignalMask,
        uint8_t * &outMsg, size_t &outMsgSize, uint32_t msgCounter) {
    return gEngHubMsgConverter.pbPopulateEHMsgSvEphemerisBasedOnSubModifiers(inMsg,
            gnssSignalMask, outMsg, outMsgSize, msgCounter);
}

static int pbPopulateEHMessageQWESFeatureStatusInfo(const ehFeatureStatus &status,
        const ehLicenseType &type, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageQWESFeatureStatusInfo(status, type, os);
}

static int pbPopulateEHMessageQWESInstallLicenseResp(const ehInstallLicenseStatus & status,
        string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageQWESInstallLicenseResp(status, os);
}

static int pbPopulateEHMessageQWESProvideAttestationStatement(const uint8_t *outBuff,
        uint32_t bufLen, const ehAttestationStatus &status, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageQWESProvideAttestationStatement(
            outBuff, bufLen, status, os);
}

static int pbPopulateEHMessageQWESForceSyncNotify(int32_t status, string &os) {
    return gEngHubMsgConverter.pbPopulateEHMessageQWESForceSyncNotify(status, os);
}
// Library entry point for enghub msg converter
static const EngHubMsgConvInterface gEngHubMsgConv = {
    sizeof(EngHubMsgConvInterface),
    pbPopulateEHMsgWithCounterToSend,
    pbPopulateEHMsgReportSvMeas,
    pbPopulateEHMsgReportSv,
    pbPopulateEHMsgSvPolynomial,
    pbPopulateEHMsgSvEphemeris,
    pbPopulateEHMsgReportPosition,
    pbPopulateEHMsgKlobucharIonoModel,
    pbPopulateEHMsgGlonassAddnParams,
    pbPopulateEHMessageSessionStart,
    pbPopulateEHMessageSessionStop,
    pbPopulateEHMessageProtocolEngineHubUp,
    pbPopulateEHMessageStatusConnectivity,
    pbPopulateEHMessageStatusLeverArmConfig,
    pbPopulateEHMessageStatusB2sMountParams,
    pbPopulateEHMessageSessionDeleteAidingData,
    pbPopulateEHMessageSessionSetFixMode,
    pbPopulateEHMessageProtocolRoutingTableUpdate,
#ifdef FEATURE_CDFW
    pbPopulateEHMessageQDgnssReferenceStation,
    pbPopulateEHMessageQDgnssObservation,
#endif
    pbPopulateEHMsgReportSvMeasBasedOnSubModifiers,
    pbPopulateEHMsgSvEphemerisBasedOnSubModifiers,
    pbPopulateEHMessageQWESFeatureStatusInfo,
    pbPopulateEHMessageQWESInstallLicenseResp,
    pbPopulateEHMessageQWESProvideAttestationStatement,
    pbPopulateEHMessageQWESForceSyncNotify,
    // Decode routines
    decodeEHubWthMsgCounterPayLoadMsg,
    decodeEHRoutingTableEntry,
    decodeEHMessageReportPosition,
    decodeEHMessageReportSv,
    decodeEHMessageInstallLicenseReq,
    decodeEHMessageAttestationReq,
    // Util functions
};

#ifndef DEBUG_X86
extern "C" const EngHubMsgConvInterface* getEngHubMsgConvIface()
#else
const EngHubMsgConvInterface* getEngHubMsgConvIface()
#endif // DEBUG_X86
{
    return &gEngHubMsgConv;
}
