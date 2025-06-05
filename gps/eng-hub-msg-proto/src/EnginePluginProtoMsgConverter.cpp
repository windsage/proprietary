/******************************************************************************
    Copyright (c) 2018-2023 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include <stdio.h>
#include <string>
#include <errno.h>
#include "gps_extended.h"
#include <loc_pla.h>
#include <log_util.h>
#include <map>
#include <stdbool.h>
#include <math.h>
#include <string>
#include <sstream>
#include "EngineHubMsg.h"
#include "EnginePluginAPI.h"
#include "EnginePluginProtoMsgConverter.h"
#include "EHMsgUtils.h"

#ifdef FEATURE_CDFW
// QDGnss structs and enums
#include "QDgnssDataType.h"
#endif

#undef LOG_TAG
#define LOG_TAG "EPProtoMsg_Conv"
#define INVALID_SYSTEM_WEEK 65535

using namespace std;

namespace engine_plugin {

/* Map of previous CycleSlipCount per Constellation/svId*/
#define GNSS_LOC_SV_SYSTEM_TYPE_MAX     (GNSS_LOC_SV_SYSTEM_QZSS + 5)
#define MAX_ALLOWED_CP_UNC (9000.0)

static void pbFillInterSystemTimeBias(const char* interSystem,
        const PBGnssInterSystemBias& eHInterSystemBias,
        epGnssInterSystemBiasStructType &ePInterSysBias)
{
    uint32_t validMask = eHInterSystemBias.validmask();
    LOC_LOGv("%s] Mask:%d, TimeBias:%f, TimeBiasUnc:%f,\n",
             interSystem, validMask, eHInterSystemBias.timebias(),
             eHInterSystemBias.timebiasunc());

    if (validMask & PB_GNSS_SYS_TIME_BIAS_VALID) {
        ePInterSysBias.validMask |= EP_MEAS_IMPL_STATUS_TIME_BIAS_VALID;
        ePInterSysBias.timeBias = eHInterSystemBias.timebias();
    }
    if (validMask & PB_GNSS_SYS_TIME_BIAS_UNC_VALID) {
        ePInterSysBias.validMask |= EP_MEAS_IMPL_STATUS_TIME_BIAS_UNC_VALID;
        ePInterSysBias.timeBiasUnc = eHInterSystemBias.timebiasunc();
    }
}

static epPositionFlags pbConvertEpPositionFlags(uint32_t posTechMask)
{
    epPositionFlags posFlags = LOC_POS_TECH_MASK_DEFAULT;
    LOC_LOGd("posTechMask %d", posTechMask);

    if (posTechMask & PB_LOC_POS_TECH_MASK_SATELLITE) {
        posFlags |= EP_POSITION_FLAG_GNSS_USED;
    }
    if (posTechMask & PB_LOC_POS_TECH_MASK_CELLID) {
        posFlags |= EP_POSITION_FLAG_CELLID_USED;
    }
    if (posTechMask & PB_LOC_POS_TECH_MASK_WIFI) {
        posFlags |= EP_POSITION_FLAG_WIFI_USED;
    }
    if (posTechMask & PB_LOC_POS_TECH_MASK_SENSORS) {
        posFlags |= EP_POSITION_FLAG_DR;
    }
    if (posTechMask & PB_LOC_POS_TECH_MASK_PPE) {
        posFlags |= EP_POSITION_FLAG_RTK_CORR;
    }
    return posFlags;
}

epGnssConstellationTypeMask EpProtoMsgConverter::pbGetEPGnssConstellationType
(
    PBGnss_LocSvSystemEnumType system
)
{
    epGnssConstellationTypeMask type = 0;
    switch (system) {
        case PB_GNSS_LOC_SV_SYSTEM_GPS:
        type = EP_GNSS_CONSTELLATION_GPS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GALILEO:
        type = EP_GNSS_CONSTELLATION_GALILEO;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_SBAS:
        type = EP_GNSS_CONSTELLATION_SBAS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_GLONASS:
        type = EP_GNSS_CONSTELLATION_GLONASS;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_BDS:
        type = EP_GNSS_CONSTELLATION_BEIDOU;
        break;
        case PB_GNSS_LOC_SV_SYSTEM_QZSS:
        type = EP_GNSS_CONSTELLATION_QZSS;
        break;
    }
    return type;
}

/*Get epGnssConstellationTypeMask from PBGnssConstellationInfoMask */
epGnssConstellationTypeMask EpProtoMsgConverter::pbGetEPGnssConstellationType
(
    uint32_t system
)
{
    epGnssConstellationTypeMask type = 0;
    if (system & PB_GNSS_CONSTELLATION_TYPE_GPS) {
        type |= EP_GNSS_CONSTELLATION_GPS;
    }
    if (system & PB_GNSS_CONSTELLATION_TYPE_GALILEO) {
        type |= EP_GNSS_CONSTELLATION_GALILEO;
    }
    if (system & PB_GNSS_CONSTELLATION_TYPE_SBAS) {
        type |= EP_GNSS_CONSTELLATION_SBAS;
    }
    if (system & PB_GNSS_CONSTELLATION_TYPE_GLONASS) {
        type |= EP_GNSS_CONSTELLATION_GLONASS;
    }
    if (system & PB_GNSS_CONSTELLATION_TYPE_BEIDOU) {
        type |= EP_GNSS_CONSTELLATION_BEIDOU;
    }
    if (system & PB_GNSS_CONSTELLATION_TYPE_QZSS) {
        type |= EP_GNSS_CONSTELLATION_QZSS;
    }
    return type;
}

uint32_t EpProtoMsgConverter::pbGetEHGnssConstellationType
(
    epGnssConstellationTypeMask eptype
)
{
    uint32_t type = PB_GNSS_CONSTELLATION_TYPE_UNKNOWN;
    if (eptype & EP_GNSS_CONSTELLATION_GPS) {
        type |= PB_GNSS_CONSTELLATION_TYPE_GPS;
    }
    if (eptype & EP_GNSS_CONSTELLATION_GALILEO) {
        type |= PB_GNSS_CONSTELLATION_TYPE_GALILEO;
    }
    if (eptype & EP_GNSS_CONSTELLATION_SBAS) {
        type |= PB_GNSS_CONSTELLATION_TYPE_SBAS;
    }
    if (eptype & EP_GNSS_CONSTELLATION_GLONASS) {
        type |= PB_GNSS_CONSTELLATION_TYPE_GLONASS;
    }
    if (eptype & EP_GNSS_CONSTELLATION_BEIDOU) {
        type |= PB_GNSS_CONSTELLATION_TYPE_BEIDOU;
    }
    if (eptype & EP_GNSS_CONSTELLATION_QZSS) {
        type |= PB_GNSS_CONSTELLATION_TYPE_QZSS;
    }
    return type;
}

uint32_t EpProtoMsgConverter::pbGetEHubGnssSignalTypeMask
(
   epGnssSignalTypeMask gnssSignalType
)
{
    uint32_t type = PB_GNSS_SIGNAL_TYPE_UNKNOWN;
    if (gnssSignalType & EP_GNSS_SIGNAL_GPS_L1CA) {
        type |= PB_GNSS_SIGNAL_TYPE_GPS_L1CA;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GPS_L1C) {
        type |= PB_GNSS_SIGNAL_TYPE_GPS_L1C;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GPS_L2C_L) {
        type |= PB_GNSS_SIGNAL_TYPE_GPS_L2;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GPS_L5_Q) {
        type |= PB_GNSS_SIGNAL_TYPE_GPS_L5;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GLONASS_G1_CA) {
        type |= PB_GNSS_SIGNAL_TYPE_GLONASS_G1;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GLONASS_G2_CA) {
        type |= PB_GNSS_SIGNAL_TYPE_GLONASS_G2;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GALILEO_E1_C) {
        type |= PB_GNSS_SIGNAL_TYPE_GALILEO_E1;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GALILEO_E5A_Q) {
        type |= PB_GNSS_SIGNAL_TYPE_GALILEO_E5A;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_GALILIEO_E5B_Q) {
        type |= PB_GNSS_SIGNAL_TYPE_GALILEO_E5B;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_BEIDOU_B1_I) {
        type |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B1I;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_BEIDOU_B1_C) {
        type |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B1C;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_BEIDOU_B2_I) {
        type |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B2I;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_BEIDOU_B2A_I) {
        type |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AI;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_QZSS_L1CA) {
        type |= PB_GNSS_SIGNAL_TYPE_QZSS_L1CA;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_QZSS_L1S) {
        type |= PB_GNSS_SIGNAL_TYPE_QZSS_L1S;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_QZSS_L2C_L) {
        type |= PB_GNSS_SIGNAL_TYPE_QZSS_L2;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_QZSS_L5_Q) {
        type |= PB_GNSS_SIGNAL_TYPE_QZSS_L5;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_SBAS_L1_CA) {
        type |= PB_GNSS_SIGNAL_TYPE_SBAS_L1;
    }
    if (gnssSignalType & EP_GNSS_SIGNAL_BEIDOU_B2A_Q) {
        type |= PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AQ;
    }
    return type;
}

PBGnss_LocSvSystemEnumType EpProtoMsgConverter::pbGetEHubGnssConstellationType
(
   epGnssConstellationTypeMask  system
)
{
    PBGnss_LocSvSystemEnumType type = PB_GNSS_LOC_SV_SYSTEM_UNKNOWN;
    switch (system) {
        case EP_GNSS_CONSTELLATION_GPS:
        type = PB_GNSS_LOC_SV_SYSTEM_GPS;
        break;
        case EP_GNSS_CONSTELLATION_GALILEO:
        type = PB_GNSS_LOC_SV_SYSTEM_GALILEO;
        break;
        case EP_GNSS_CONSTELLATION_SBAS:
        type = PB_GNSS_LOC_SV_SYSTEM_SBAS;
        break;
        case EP_GNSS_CONSTELLATION_GLONASS:
        type = PB_GNSS_LOC_SV_SYSTEM_GLONASS;
        break;
        case EP_GNSS_CONSTELLATION_BEIDOU:
        type = PB_GNSS_LOC_SV_SYSTEM_BDS;
        break;
        case EP_GNSS_CONSTELLATION_QZSS:
        type = PB_GNSS_LOC_SV_SYSTEM_QZSS;
        break;
    }
    return type;
}

uint32_t EpProtoMsgConverter::pbGetMeasUsageStatusMask
(
   epGnssMeasUsageStatusBitMask epMeasUsageStatusMask
)
{
    uint32_t measUsageStatusMask = 0;
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USED_IN_PVT) {
        measUsageStatusMask |= PB_GNSS_MEAS_USED_IN_PVT;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_BAD_MEAS) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_BAD_MEAS;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_CNO_TOO_LOW) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_CNO_TOO_LOW;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_ELEVATION_TOO_LOW) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_ELEVATION_TOO_LOW;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_NO_EPHEMERIS) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_NO_EPHEMERIS;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_NO_CORRECTIONS) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_NO_CORRECTIONS;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_CORRECTION_TIMEOUT) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_CORRECTION_TIMEOUT;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_UNHEALTHY) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_UNHEALTHY;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_CONFIG_DISABLED) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_CONFIG_DISABLED;
    }
    if (epMeasUsageStatusMask & EP_GNSS_MEAS_USAGE_STATUS_OTHER) {
        measUsageStatusMask |= PB_GNSS_MEAS_USAGE_STATUS_OTHER;
    }
    return measUsageStatusMask;
}

void EpProtoMsgConverter::pbFillEpGnssSystemTimeInfo
(
    const PBGnssSystemTimeStructType &gnssTime,
    epGnssSystemTimeStructType &epSystemTime
)
{
    uint32_t validityMask = gnssTime.validitymask();
    if (validityMask & PB_GNSS_SYSTEM_TIME_WEEK_VALID) {
        epSystemTime.systemWeek = gnssTime.systemweek();
        if (INVALID_SYSTEM_WEEK != epSystemTime.systemWeek)
            epSystemTime.validityMask.isSystemWeekValid = 1;
    }
    if (validityMask & PB_GNSS_SYSTEM_TIME_WEEK_MS_VALID) {
        epSystemTime.systemMsec = gnssTime.systemmsec();
        epSystemTime.validityMask.isSystemWeekMsecValid = 1;
    }
    if (validityMask & PB_GNSS_SYSTEM_CLK_TIME_BIAS_VALID) {
        epSystemTime.systemClkTimeBias = gnssTime.systemclktimebias();
        epSystemTime.validityMask.isSystemClkTimeBiasValid = 1;
    }
    if (validityMask & PB_GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID) {
        epSystemTime.systemClkTimeUncMs = gnssTime.systemclktimeuncms();
        epSystemTime.validityMask.isSystemClkTimeUncMsValid = 1;
    }
    if (validityMask & PB_GNSS_SYSTEM_REF_FCOUNT_VALID) {
        epSystemTime.refFCount = gnssTime.reffcount();
        epSystemTime.validityMask.isRefFCountValid = 1;
    }
    if (validityMask & PB_GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID) {
        epSystemTime.numClockResets = gnssTime.numclockresets();
        epSystemTime.validityMask.isNumClockResetsValid = 1;
    }
}

void EpProtoMsgConverter::pbFillEpGloSystemTimeInfo
(
    const PBGnssGloTimeStructType &gloTime,
    epGnssGloTimeStructType &gloSystemTime
)
{
    uint32_t validityMask = gloTime.validitymask();
    // gloDays, gloMsec and gloFourYear are manditory fileds
    if (validityMask & PB_GNSS_CLO_DAYS_VALID) {
        gloSystemTime.gloDays = gloTime.glodays();
        gloSystemTime.validityMask.isGloDaysValid = 1;
    }

    if (validityMask & PB_GNSS_GLO_MSEC_VALID) {
        gloSystemTime.gloMsec = gloTime.glomsec();
        gloSystemTime.validityMask.isGloMsecValid = 1;
    }

    if (validityMask & PB_GNSS_GLO_FOUR_YEAR_VALID) {
        gloSystemTime.gloFourYear = gloTime.glofouryear();
        gloSystemTime.validityMask.isGloFourYearValid = 1;
    }

    if (validityMask & PB_GNSS_GLO_CLK_TIME_BIAS_VALID) {
        gloSystemTime.gloClkTimeBias = gloTime.gloclktimebias();
        gloSystemTime.validityMask.isGloClkTimeBiasValid = 1;
    }

    if (validityMask & PB_GNSS_GLO_CLK_TIME_BIAS_UNC_VALID) {
        gloSystemTime.gloClkTimeUncMs = gloTime.gloclktimeuncms();
        gloSystemTime.validityMask.isGloClkTimeUncMsValid = 1;
    }
    if (validityMask & PB_GNSS_GLO_REF_FCOUNT_VALID) {
        gloSystemTime.refFCount = gloTime.reffcount();
        gloSystemTime.validityMask.isRefFCountValid = 1;
    }
    if (validityMask & PB_GNSS_GLO_NUM_CLOCK_RESETS_VALID) {
        gloSystemTime.numClockResets = gloTime.numclockresets();
        gloSystemTime.validityMask.isNumClockResetsValid = 1;
    }
}

void EpProtoMsgConverter::pbFillEpDgnssSVMeasurement
(
    const PBGnssLocDgnssSVMeasurement &dgnssSvMeas,
    epDgnssSVMeasurement &epDgnssSvMeas
)
{
    epDgnssSvMeas.dgnssMeasStatus = dgnssSvMeas.dgnssmeasstatus();
    epDgnssSvMeas.diffDataEpochTimeMsec = dgnssSvMeas.diffdataepochtimemsec();
    epDgnssSvMeas.prCorrMeters = dgnssSvMeas.prcorrmeters();
    epDgnssSvMeas.prrCorrMetersPerSec = dgnssSvMeas.prrcorrmeterspersec();
}

void pbGetEPGnssSignalMask(uint32_t eHSignalMask, epGnssSignalTypeMask &gnssSignalMask) {

    switch (eHSignalMask) {
    case PB_GNSS_SIGNAL_TYPE_GPS_L1CA:
        gnssSignalMask = EP_GNSS_SIGNAL_GPS_L1CA;
        break;
    case PB_GNSS_SIGNAL_TYPE_GPS_L1C:
        gnssSignalMask = EP_GNSS_SIGNAL_GPS_L1C;
        break;
    case PB_GNSS_SIGNAL_TYPE_GPS_L2:
        gnssSignalMask = EP_GNSS_SIGNAL_GPS_L2C_L;
        break;
    case PB_GNSS_SIGNAL_TYPE_GPS_L5:
        gnssSignalMask = EP_GNSS_SIGNAL_GPS_L5_Q;
        break;
    case PB_GNSS_SIGNAL_TYPE_GLONASS_G1:
        gnssSignalMask = EP_GNSS_SIGNAL_GLONASS_G1_CA;
        break;
    case PB_GNSS_SIGNAL_TYPE_GLONASS_G2:
        gnssSignalMask = EP_GNSS_SIGNAL_GLONASS_G2_CA;
        break;
    case PB_GNSS_SIGNAL_TYPE_GALILEO_E1:
        gnssSignalMask = EP_GNSS_SIGNAL_GALILEO_E1_C;
        break;
    case PB_GNSS_SIGNAL_TYPE_GALILEO_E5A:
        gnssSignalMask = EP_GNSS_SIGNAL_GALILEO_E5A_Q;
        break;
    case PB_GNSS_SIGNAL_TYPE_GALILEO_E5B:
        gnssSignalMask = EP_GNSS_SIGNAL_GALILIEO_E5B_Q;
        break;
    case PB_GNSS_SIGNAL_TYPE_BEIDOU_B1I:
        gnssSignalMask = EP_GNSS_SIGNAL_BEIDOU_B1_I;
        break;
    case PB_GNSS_SIGNAL_TYPE_BEIDOU_B1C:
        gnssSignalMask = EP_GNSS_SIGNAL_BEIDOU_B1_C;
        break;
    case PB_GNSS_SIGNAL_TYPE_BEIDOU_B2I:
        gnssSignalMask = EP_GNSS_SIGNAL_BEIDOU_B2_I;
        break;
    case PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AI:
        gnssSignalMask = EP_GNSS_SIGNAL_BEIDOU_B2A_I;
        break;
    case PB_GNSS_SIGNAL_TYPE_QZSS_L1CA:
        gnssSignalMask = EP_GNSS_SIGNAL_QZSS_L1CA;
        break;
    case PB_GNSS_SIGNAL_TYPE_QZSS_L1S:
        gnssSignalMask = EP_GNSS_SIGNAL_QZSS_L1S;
        break;
    case PB_GNSS_SIGNAL_TYPE_QZSS_L2:
        gnssSignalMask = EP_GNSS_SIGNAL_QZSS_L2C_L;
        break;
    case PB_GNSS_SIGNAL_TYPE_QZSS_L5:
        gnssSignalMask = EP_GNSS_SIGNAL_QZSS_L5_Q;
        break;
    case PB_GNSS_SIGNAL_TYPE_SBAS_L1:
        gnssSignalMask = EP_GNSS_SIGNAL_SBAS_L1_CA;
        break;
    case PB_GNSS_SIGNAL_TYPE_BEIDOU_B2AQ:
        gnssSignalMask = EP_GNSS_SIGNAL_BEIDOU_B2A_Q;
        break;
    }
}

/** Below function translates QMI measurement report in to
    EP measurement report (external API)

    Return value:
      True: When a full measurement report is formed and ready to be injected
      False: Full measurement formation not complete
    Input:
      svMeasurementfromQMI: Measurement report from EngineHub
    Output:
      epMesurement: Translated measurement report for delivery to engine client
*/
void EpProtoMsgConverter::pbFillEpPVTReport
(
    const EHMessageReportPosition *pEHubPosRptIn,
    epImplPVTReport &ePImplPVTRpt
)
{
    if (nullptr == pEHubPosRptIn) {
        LOC_LOGe("pEHubPosRptIn = nullptr !! return ");
        return;
    }
     /* Input data structure */
    epGnssSystemTimeStructType * pSystemTime = NULL;
    PBLocSessionStatus status = pEHubPosRptIn->status();
    const PBUlpLocation& location = pEHubPosRptIn->location();
    const PBLocGpsLocation &gpsLocation = location.gpslocation();
    const PBGpsLocationExtended& locationExtended = pEHubPosRptIn->locationextended();
    bool  unpropagatedPosition = location.unpropagatedposition();
    epPVTReport &ePPVTRptOut = ePImplPVTRpt.pvtReport;

    if (status == PB_LOC_SESS_SUCCESS) {
        ePPVTRptOut.statusOfFix = EP_VALID_FIX;
    } else {
        ePPVTRptOut.statusOfFix = EP_NO_FIX;
    }
    uint32_t extFlagsLower32 = (uint32_t)locationExtended.flags();
    uint32_t extFlagsUpper32 = (uint32_t)(locationExtended.flags() >> 32);
    LOC_LOGi("extendedFlags Lower32: 0x%x Upper32: 0x%x",
            extFlagsLower32, extFlagsUpper32);
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK) {
        ePPVTRptOut.posFlags =
                pbConvertEpPositionFlags(locationExtended.tech_mask());
    }
    const PBGnssSystemTime& gnssSystemTime = locationExtended.gnsssystemtime();
    const PBSystemTimeStructUnion& u = gnssSystemTime.u();
    const PBGnss_LocSvSystemEnumType &gnssTimeSource = gnssSystemTime.gnsssystemtimesrc();

    if (PB_GNSS_LOC_SV_SYSTEM_GPS == gnssTimeSource) {
        ePPVTRptOut.epGnssSystemTime.gnssSystemTimeSrc = EP_GNSS_CONSTELLATION_GPS;
        pbFillEpGnssSystemTimeInfo(u.gpssystemtime(),
                                 ePPVTRptOut.epGnssSystemTime.u.gpsSystemTime);
    } else if (PB_GNSS_LOC_SV_SYSTEM_GALILEO == gnssTimeSource) {
        ePPVTRptOut.epGnssSystemTime.gnssSystemTimeSrc = EP_GNSS_CONSTELLATION_GALILEO;
        pbFillEpGnssSystemTimeInfo(u.galsystemtime(),
                                 ePPVTRptOut.epGnssSystemTime.u.galSystemTime);
    } else if (PB_GNSS_LOC_SV_SYSTEM_BDS == gnssTimeSource) {
        ePPVTRptOut.epGnssSystemTime.gnssSystemTimeSrc = EP_GNSS_CONSTELLATION_BEIDOU;
        pbFillEpGnssSystemTimeInfo(u.bdssystemtime(),
                                 ePPVTRptOut.epGnssSystemTime.u.bdsSystemTime);
    } else if (PB_GNSS_LOC_SV_SYSTEM_QZSS == gnssTimeSource) {
        ePPVTRptOut.epGnssSystemTime.gnssSystemTimeSrc = EP_GNSS_CONSTELLATION_QZSS;
        pbFillEpGnssSystemTimeInfo(u.qzsssystemtime(),
                                 ePPVTRptOut.epGnssSystemTime.u.qzssSystemTime);
    } else if (PB_GNSS_LOC_SV_SYSTEM_GLONASS == gnssTimeSource) {
        ePPVTRptOut.epGnssSystemTime.gnssSystemTimeSrc = EP_GNSS_CONSTELLATION_GLONASS;
        pbFillEpGloSystemTimeInfo(u.glosystemtime(),
                                ePPVTRptOut.epGnssSystemTime.u.gloSystemTime);
    }
    ePPVTRptOut.utcTimestampMs = gpsLocation.timestamp();
    ePPVTRptOut.validityMask.isUtcTimestampMsValid = 1;
    if (true == unpropagatedPosition) {
        ePPVTRptOut.posFlags |= EP_POSITION_FLAG_UNPROPAGATED_PVT;
        /* SPE always reports unpropagated fix as intermediate fix, based on below
           condition set the statusOfFix to valid/invalid */
        PBLocReliability horizontal_reliability = locationExtended.horizontal_reliability();
        PBLocReliability vertical_reliability = locationExtended.vertical_reliability();
        if ((horizontal_reliability == PB_LOC_RELIABILITY_MEDIUM ||
            horizontal_reliability == PB_LOC_RELIABILITY_HIGH) &&
            (vertical_reliability == PB_LOC_RELIABILITY_MEDIUM ||
            vertical_reliability == PB_LOC_RELIABILITY_HIGH)) {
                ePPVTRptOut.statusOfFix = EP_VALID_FIX;
        }
    }
    PBApTimeStampStructType timeStamp = locationExtended.timestamp();
    PBTimespec apTimeStamp = timeStamp.aptimestamp();
    const uint32_t gpsLocationFlags = gpsLocation.flags();
    /* AP timesamp is missing in GpsLocationExtended data structure */
    ePPVTRptOut.lastPpsLocaltimeStamp.apTimeStamp.tv_sec = apTimeStamp.tv_sec();
    ePPVTRptOut.lastPpsLocaltimeStamp.apTimeStamp.tv_nsec = apTimeStamp.tv_nsec();
    ePPVTRptOut.lastPpsLocaltimeStamp.apTimeStampUncertaintyMs = timeStamp.aptimestampuncms();
    if (gpsLocationFlags & PB_LOC_GPS_LOCATION_HAS_LAT_LONG) {
        ePPVTRptOut.latitudeDeg = gpsLocation.latitude();
        ePPVTRptOut.validityMask.isLatitudeValid = 1;
        ePPVTRptOut.longitudeDeg = gpsLocation.longitude();
        ePPVTRptOut.validityMask.isLongitudeValid = 1;
    }
    if (gpsLocationFlags & PB_LOC_GPS_LOCATION_HAS_ALTITUDE) {
        ePPVTRptOut.altitudeWrtEllipsoid = gpsLocation.altitude();
        ePPVTRptOut.validityMask.isAltitudeWrtEllipsoidValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL) {
        ePPVTRptOut.altitudeWrtMeanSeaLevel = locationExtended.altitudemeansealevel();
        ePPVTRptOut.validityMask.isAltitudeWrtMeanSeaLevelValid = 1;
    }
    if (gpsLocationFlags & PB_LOC_GPS_LOCATION_HAS_BEARING) {
        ePPVTRptOut.headingDeg = gpsLocation.bearing();
        ePPVTRptOut.validityMask.isHeadingDegValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_BEARING_UNC) {
        ePPVTRptOut.headingUncDeg = locationExtended.bearing_unc();
        ePPVTRptOut.validityMask.isHeadingUncDegValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_VERT_UNC) {
        ePPVTRptOut.altitudeStdDeviation = locationExtended.vert_unc();
        ePPVTRptOut.validityMask.isAltitudeStdDeviationValid = 1;
    }
    if (gpsLocationFlags & PB_LOC_GPS_LOCATION_HAS_SPEED) {
        ePPVTRptOut.horizontalSpeed = gpsLocation.speed();
        ePPVTRptOut.validityMask.isHorizontalSpeedValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_SPEED_UNC) {
        ePPVTRptOut.horizontalSpeedUnc = locationExtended.speed_unc();
        ePPVTRptOut.validityMask.isHorizontalSpeedUncValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_MAG_DEV) {
        ePPVTRptOut.magneticDeviationDeg = locationExtended.magneticdeviation();
        ePPVTRptOut.validityMask.isMagneticDeviationDegValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS) {
        ePPVTRptOut.clockbiasMeter = locationExtended.clockbiasmeter();
        ePPVTRptOut.validityMask.isClockbiasValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS_STD_DEV) {
        ePPVTRptOut.clockBiasStdDeviationMeter = locationExtended.clockbiasstddeviationmeter();
        ePPVTRptOut.validityMask.isClockBiasStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT) {
        ePPVTRptOut.clockDrift = locationExtended.clockdrift();
        ePPVTRptOut.validityMask.isClockDriftValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT_STD_DEV) {
        ePPVTRptOut.clockDriftStdDeviation = locationExtended.clockdriftstddeviation();
        ePPVTRptOut.validityMask.isClockDriftStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR) {
        ePPVTRptOut.horUncEllipseSemiMajor = locationExtended.horuncellipsesemimajor();
        ePPVTRptOut.validityMask.isHorizontalEllipticalUncSemiMajorAxisValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR) {
        ePPVTRptOut.horUncEllipseSemiMinor = locationExtended.horuncellipsesemiminor();
        ePPVTRptOut.validityMask.isHorizontalEllipticalUncSemiMinorAxisValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH) {
        ePPVTRptOut.horUncEllipseOrientAzDeg = locationExtended.horuncellipseorientazimuth();
        ePPVTRptOut.validityMask.isHorizontalEllipticalUncAZValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_NORTH_VEL) {
        ePPVTRptOut.northVelocity = locationExtended.northvelocity();
        ePPVTRptOut.validityMask.isNorthVelocityValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_EAST_VEL) {
        ePPVTRptOut.eastVelocity = locationExtended.eastvelocity();
        ePPVTRptOut.validityMask.isEastVelocityValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_UP_VEL) {
        ePPVTRptOut.upVelocity = locationExtended.upvelocity();
        ePPVTRptOut.validityMask.isUpVelocityValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC) {
        ePPVTRptOut.northVelocityStdDeviation = locationExtended.northvelocitystddeviation();
        ePPVTRptOut.validityMask.isNorthVelocityStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC) {
        ePPVTRptOut.eastVelocityStdDeviation = locationExtended.eastvelocitystddeviation();
        ePPVTRptOut.validityMask.isEastVelocityStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC) {
        ePPVTRptOut.upVelocityStdDeviation = locationExtended.upvelocitystddeviation();
        ePPVTRptOut.validityMask.isUpVelocityStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV) {
        ePPVTRptOut.northStdDeviation = locationExtended.northstddeviation();
        ePPVTRptOut.validityMask.isNorthStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV) {
        ePPVTRptOut.eastStdDeviation = locationExtended.eaststddeviation();
        ePPVTRptOut.validityMask.isEastStdDeviationValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP) {
        const PBLocExtDOP& extDOP = locationExtended.extdop();
        ePPVTRptOut.pDop =  extDOP.pdop();
        ePPVTRptOut.hDop =  extDOP.hdop();
        ePPVTRptOut.vDop =  extDOP.vdop();
        ePPVTRptOut.gDop =  extDOP.gdop();
        ePPVTRptOut.tDop =  extDOP.tdop();
        ePPVTRptOut.validityMask.isPdopValid = 1;
        ePPVTRptOut.validityMask.isHdopValid = 1;
        ePPVTRptOut.validityMask.isVdopValid = 1;
        ePPVTRptOut.validityMask.isGdopValid = 1;
        ePPVTRptOut.validityMask.isTdopValid = 1;
    }
    if (extFlagsLower32 & PB_GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA) {
        const PBGnssLocationPositionDynamics& bodyFrameData = locationExtended.bodyframedata();
        uint32_t bodyFrameDataMask = bodyFrameData.bodyframedatamask();
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT) {
            ePPVTRptOut.longAccel = bodyFrameData.longaccel();
            ePPVTRptOut.validityMask.isLongAccelValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT) {
            ePPVTRptOut.longAccelUnc = bodyFrameData.longaccelunc();
            ePPVTRptOut.validityMask.isLongAccelUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT) {
            ePPVTRptOut.latAccel = bodyFrameData.lataccel();
            ePPVTRptOut.validityMask.isLatAccelValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT) {
            ePPVTRptOut.latAccelUnc = bodyFrameData.lataccelunc();
            ePPVTRptOut.validityMask.isLatAccelUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT) {
            ePPVTRptOut.vertAccel = bodyFrameData.vertaccel();
            ePPVTRptOut.validityMask.isVertAccelValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT) {
            ePPVTRptOut.vertAccelUnc = bodyFrameData.vertaccelunc();
            ePPVTRptOut.validityMask.isVertAccelUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT) {
            ePPVTRptOut.yawRate = bodyFrameData.yawrate();
            ePPVTRptOut.validityMask.isYawRateValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT) {
            ePPVTRptOut.yawRateUnc = bodyFrameData.yawrateunc();
            ePPVTRptOut.validityMask.isYawRateUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_PITCH_BIT) {
            ePPVTRptOut.pitchRad = bodyFrameData.pitch();
            ePPVTRptOut.validityMask.isPitchRadValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT) {
            ePPVTRptOut.pitchRadUnc = bodyFrameData.pitchunc();
            ePPVTRptOut.validityMask.isPitchRadUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT) {
            ePImplPVTRpt.pitchRate = bodyFrameData.pitchrate();
            ePImplPVTRpt.validMask.isPitchRateValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT) {
            ePImplPVTRpt.pitchRateUnc = bodyFrameData.pitchrateunc();
            ePImplPVTRpt.validMask.isPitchRateUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_ROLL_BIT) {
            ePImplPVTRpt.rollRad = bodyFrameData.roll();
            ePImplPVTRpt.validMask.isRollRadValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT) {
            ePImplPVTRpt.rollUnc = bodyFrameData.rollunc();
            ePImplPVTRpt.validMask.isRollUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT) {
            ePImplPVTRpt.rollRate = bodyFrameData.rollrate();
            ePImplPVTRpt.validMask.isRollRateValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT) {
            ePImplPVTRpt.rollRateUnc = bodyFrameData.rollrateunc();
            ePImplPVTRpt.validMask.isRollRateUncValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_YAW_BIT) {
            ePImplPVTRpt.yawRad = bodyFrameData.yaw();
            ePImplPVTRpt.validMask.isYawRadValid = 1;
        }
        if (bodyFrameDataMask & PB_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT) {
            ePImplPVTRpt.yawUnc = bodyFrameData.yawunc();
            ePImplPVTRpt.validMask.isYawUncValid = 1;
        }
    }
    uint32_t numOfMeasReceived = locationExtended.numofmeasreceived();
    if (EP_GNSS_MAX_MEAS > numOfMeasReceived) {
        ePPVTRptOut.numOfMeasReceived = numOfMeasReceived;
    } else {
        ePPVTRptOut.numOfMeasReceived = EP_GNSS_MAX_MEAS;
    }
    for (uint8_t idx = 0; idx < ePPVTRptOut.numOfMeasReceived; idx++) {
        const PBGpsMeasUsageInfo& measUsageInfo = locationExtended.measusageinfo(idx);
        ePPVTRptOut.measUsageInfo[idx].gnssSignalType = measUsageInfo.gnsssignaltype();
        ePPVTRptOut.measUsageInfo[idx].gnssConstellation =
            pbGetEPGnssConstellationType(measUsageInfo.gnssconstellation());
        ePPVTRptOut.measUsageInfo[idx].gnssSvId = measUsageInfo.gnsssvid();
        ePPVTRptOut.measUsageInfo[idx].gloFrequency = measUsageInfo.glofrequency();
        ePPVTRptOut.measUsageInfo[idx].measUsageStatusMask = EP_GNSS_MEAS_USED_IN_PVT;
        uint32_t validityMask = measUsageInfo.validitymask();
        if (validityMask & PB_GNSS_CARRIER_PHASE_AMBIGUITY_TYPE_VALID) {
            ePPVTRptOut.measUsageInfo[idx].carrierPhaseAmbiguityType =
                    measUsageInfo.carrierphaseambiguitytype();
        }
    }
    if (extFlagsUpper32 & PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_TYPE) {
        PBLocDgnssCorrectionSourceType dgnssCorrSourceType =
                locationExtended.dgnsscorrectionsourcetype();
        if (PB_LOC_DGNSS_CORR_SOURCE_TYPE_RTCM == dgnssCorrSourceType) {
            ePImplPVTRpt.dgnssCorrectionSourceType = EP_DGNSS_CORR_SOURCE_TYPE_RTCM;
        } else if (PB_LOC_DGNSS_CORR_SOURCE_TYPE_3GPP == dgnssCorrSourceType) {
            ePImplPVTRpt.dgnssCorrectionSourceType = EP_DGNSS_CORR_SOURCE_TYPE_3GPP;
        }
        ePImplPVTRpt.validMask.isDgnssCorrectionSourceTypeValid = 1;
    }
    if (extFlagsUpper32 & PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_ID) {
        ePImplPVTRpt.dgnssCorrectionSourceID =
                locationExtended.dgnsscorrectionsourceid();
        ePImplPVTRpt.validMask.isDgnssCorrectionSourceIDValid = 1;
    }
    if (extFlagsUpper32 & PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CONSTELLATION_USAGE) {
        ePImplPVTRpt.dgnssConstellationUsage =
                pbGetEPGnssConstellationType(locationExtended.dgnssconstellationusagemask());
        ePImplPVTRpt.validMask.isDgnssConstellationUsageValid = 1;
    }
    if (extFlagsUpper32 & PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_REF_STATION_ID) {
        ePImplPVTRpt.dgnssRefStationId = locationExtended.dgnssrefstationid();
        ePImplPVTRpt.validMask.isDgnssRefStationValid = 1;
    }
    if (extFlagsUpper32 & PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_DATA_AGE) {
        ePImplPVTRpt.dgnssDataAgeMsec = locationExtended.dgnssdataagemsec();
        ePImplPVTRpt.validMask.isDgnssDataAgeValid = 1;
    }
}

void EpProtoMsgConverter::pbFillEpIntraSystemTimeBiasInfo
(
    const PBGnssInterSystemBias &istInput,
    epGnssSystemTimeStructType &ePISTOut,
    uint8_t whichOut
)
{
    uint32_t validMask = istInput.validmask();
    if ((validMask & PB_GNSS_SYS_TIME_BIAS_VALID) &&
        (validMask & PB_GNSS_SYS_TIME_BIAS_UNC_VALID)) {
        if (1 == whichOut) {
            ePISTOut.intraSystemTimeBias1 = istInput.timebias();
            ePISTOut.intraSystemTimeBias1Unc = istInput.timebiasunc();
            ePISTOut.validityMask.isIntraSystemTimeBias1AndUncValid = 1;
        } else if (2 == whichOut) {
            ePISTOut.intraSystemTimeBias2 = istInput.timebias();
            ePISTOut.intraSystemTimeBias2Unc = istInput.timebiasunc();
            ePISTOut.validityMask.isIntraSystemTimeBias2AndUncValid = 1;
        }
    }
}

void EpProtoMsgConverter::pbFillEpGloIntraSystemTimeBiasInfo
(
    const PBGnssInterSystemBias &istInput,
    epGnssGloTimeStructType &ePISTOut
)
{
    uint32_t validMask = istInput.validmask();
    if ((validMask & PB_GNSS_SYS_TIME_BIAS_VALID) &&
        (validMask & PB_GNSS_SYS_TIME_BIAS_UNC_VALID)) {
        ePISTOut.intraSystemTimeBias1 = istInput.timebias();
        ePISTOut.intraSystemTimeBias1Unc = istInput.timebiasunc();
        ePISTOut.validityMask.isIntraSystemTimeBias1AndUncValid = 1;
    }
}

bool EpProtoMsgConverter::pbFillEpGnssMeasReport
(
    const EHMessageReportSvMeas *pEHubMsrRptIn,
    epGnssImplMeasurementReport &ePImplMsrRptOut,
    uint64_t &measurementStatus
)
{
    if (nullptr == pEHubMsrRptIn) {
        LOC_LOGe("pEHubMsrRptIn = nullptr !! return ");
        return false;
    }
    /* Outgoing data structures */
    epGnssSystemTimeStructType * pGnssSystemTime = NULL;
    epGnssGloTimeStructType *pGloSystemTime = NULL;
    epGnssSVMeasurementStructType *pSVMeasDst = NULL;
    epGnssSVImplMeasurementStructType *pSVImplMeasDest = NULL;
    epGnssSignalTypeMask gnssSignalMask = EP_GNSS_SIGNAL_UNKNOWN;
    epGnssConstellationTypeMask gnssSystem = EP_GNSS_CONSTELLATION_UNKNOWN;
    uint32_t eHgnssSignalTypeMask = 0;

    /* Other variables */
    uint8_t  numMeasurement = 0;
    uint8_t  lockPointSet = 0;
    if (NULL == pEHubMsrRptIn) {
        LOC_LOGe("pEHubMsrRptIn == NULL !! return");
        return false;
    }
    const PBGnssSvMeasurementSet& svMeasSet = pEHubMsrRptIn->svmeasset();
    bool isNhz = svMeasSet.isnhz();
    uint32_t svMeasCount = svMeasSet.svmeascount();
    LOC_LOGd(" nhz: %d, measurement count: %d ", isNhz, svMeasCount);

    if (isNhz) {
        ePImplMsrRptOut.msrReport.gnssMeasReportType = EP_MEASUREMENT_NHZ_REPORT;
    } else {
        ePImplMsrRptOut.msrReport.gnssMeasReportType = EP_MEASUREMENT_1HZ_REPORT;
    }
    /* Leap second handling */
    const PBGnssSvMeasurementHeader& svMeasSetHeader = svMeasSet.svmeassetheader();
    uint32_t headerFlags = svMeasSetHeader.flags();
    const PBLeapSecondInfo& leapSec = svMeasSetHeader.leapsec();
    if ((headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_LEAP_SECOND) &&
        (leapSec.leapsecunc() == 0)) {

        ePImplMsrRptOut.msrReport.leapSec = leapSec.leapsec();
        ePImplMsrRptOut.msrReport.validityMask.isLeapSecValid = 1;
    }
    /* System time handling */
    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME) {
        const PBGnssSystemTimeStructType &gpsSystemTime = svMeasSetHeader.gpssystemtime();
        pbFillEpGnssSystemTimeInfo(gpsSystemTime,
                ePImplMsrRptOut.msrReport.gpsSystemTime);
        ePImplMsrRptOut.msrReport.validityMask.isGpsSystemTimeValid = 1;
        /* Fill GPS L1 to L5 Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GPSL1L5_TIME_BIAS) {
            const PBGnssInterSystemBias& gpsL1L5TimeBias = svMeasSetHeader.gpsl1l5timebias();
            pbFillEpIntraSystemTimeBiasInfo(gpsL1L5TimeBias,
                    ePImplMsrRptOut.msrReport.gpsSystemTime, 1);
        }
        /* Fill GPS L1 to L2C Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GPSL1L2C_TIME_BIAS) {
            const PBGnssInterSystemBias& gpsL1L2CTimeBias = svMeasSetHeader.gpsl1l5timebias();
            pbFillEpIntraSystemTimeBiasInfo(gpsL1L2CTimeBias,
                    ePImplMsrRptOut.msrReport.gpsSystemTime, 2);
        }
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME) {
        const PBGnssSystemTimeStructType &bdsSystemTime = svMeasSetHeader.bdssystemtime();
        pbFillEpGnssSystemTimeInfo(bdsSystemTime,
                    ePImplMsrRptOut.msrReport.bdsSystemTime);
        ePImplMsrRptOut.msrReport.validityMask.isBdsSystemTimeValid = 1;
        /* Fill BDS B1I to B2A Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_BDSB1IB2A_TIME_BIAS) {
            const PBGnssInterSystemBias& bdsB1iB2aTimeBias = svMeasSetHeader.bdsb1ib2atimebias();
            pbFillEpIntraSystemTimeBiasInfo(bdsB1iB2aTimeBias,
                    ePImplMsrRptOut.msrReport.bdsSystemTime, 1);
        }
        /* Fill BDS B1I to B1C Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_BDSB1IB1C_TIME_BIAS) {
            const PBGnssInterSystemBias& bdsB1iB1cTimeBias = svMeasSetHeader.bdsb1ib1ctimebias();
            pbFillEpIntraSystemTimeBiasInfo(bdsB1iB1cTimeBias,
                    ePImplMsrRptOut.msrReport.bdsSystemTime, 2);
        }
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME) {
        const PBGnssSystemTimeStructType &galSystemTime = svMeasSetHeader.galsystemtime();
        pbFillEpGnssSystemTimeInfo(galSystemTime,
                                 ePImplMsrRptOut.msrReport.galSystemTime);
        ePImplMsrRptOut.msrReport.validityMask.isGalSystemTimeValid = 1;
        /* Fill GAL E1 to E5a Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GALE1E5A_TIME_BIAS) {
            const PBGnssInterSystemBias& galE1E5aTimeBias = svMeasSetHeader.gale1e5atimebias();
            pbFillEpIntraSystemTimeBiasInfo(galE1E5aTimeBias,
                    ePImplMsrRptOut.msrReport.galSystemTime, 1);
        }
        /* Fill GAL E1 to E5b Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GALE1E5B_TIME_BIAS) {
            const PBGnssInterSystemBias& galE1E5bTimeBias = svMeasSetHeader.gale1e5btimebias();
            pbFillEpIntraSystemTimeBiasInfo(galE1E5bTimeBias,
                    ePImplMsrRptOut.msrReport.galSystemTime, 2);
        }
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME) {
        const PBGnssSystemTimeStructType &qzssSystemTime = svMeasSetHeader.qzsssystemtime();
        pbFillEpGnssSystemTimeInfo(qzssSystemTime,
                                 ePImplMsrRptOut.msrReport.qzssSystemTime);
        ePImplMsrRptOut.msrReport.validityMask.isQzssSystemTimeValid = 1;
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GPSL1L5_TIME_BIAS) {
            const PBGnssInterSystemBias& gpsL1L5TimeBias = svMeasSetHeader.gpsl1l5timebias();
            pbFillEpIntraSystemTimeBiasInfo(gpsL1L5TimeBias,
                    ePImplMsrRptOut.msrReport.qzssSystemTime, 1);
        }
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GLO_SYSTEM_TIME) {
        const PBGnssGloTimeStructType& gloSystemTime = svMeasSetHeader.glosystemtime();
        pbFillEpGloSystemTimeInfo(gloSystemTime,
                    ePImplMsrRptOut.msrReport.gloSystemTime);
        ePImplMsrRptOut.msrReport.validityMask.isGloSystemTimeValid = 1;
        /* Fill GLO G1 to G2 Time Bias info */
        if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GLOG1G2_TIME_BIAS) {
            const PBGnssInterSystemBias& gloG1G2TimeBias = svMeasSetHeader.glog1g2timebias();
            pbFillEpGloIntraSystemTimeBiasInfo(gloG1G2TimeBias,
                    ePImplMsrRptOut.msrReport.gloSystemTime);
        }
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GPS_SYSTEM_TIME_EXT) {
        const PBLocGnssTimeExtInfo& gpsSystemTimeExt = svMeasSetHeader.gpssystemtimeext();
        ePImplMsrRptOut.gpsSystemTime.systemRtc_valid = gpsSystemTimeExt.systemrtcvalid();
        ePImplMsrRptOut.gpsSystemTime.systemRtcMs = gpsSystemTimeExt.systemrtcms();
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_BDS_SYSTEM_TIME_EXT) {
        const PBLocGnssTimeExtInfo& bdsSystemTimeExt = svMeasSetHeader.bdssystemtimeext();
        ePImplMsrRptOut.bdsSystemTime.systemRtc_valid = bdsSystemTimeExt.systemrtcvalid();
        ePImplMsrRptOut.bdsSystemTime.systemRtcMs = bdsSystemTimeExt.systemrtcms();
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_GAL_SYSTEM_TIME_EXT) {
        const PBLocGnssTimeExtInfo& galSystemTimeExt = svMeasSetHeader.galsystemtimeext();
        ePImplMsrRptOut.galSystemTime.systemRtc_valid = galSystemTimeExt.systemrtcvalid();
        ePImplMsrRptOut.galSystemTime.systemRtcMs = galSystemTimeExt.systemrtcms();
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_QZSS_SYSTEM_TIME_EXT) {
        const PBLocGnssTimeExtInfo& qzssSystemTimeExt = svMeasSetHeader.qzsssystemtimeext();
        ePImplMsrRptOut.qzssSystemTime.systemRtc_valid = qzssSystemTimeExt.systemrtcvalid();
        ePImplMsrRptOut.qzssSystemTime.systemRtcMs = qzssSystemTimeExt.systemrtcms();
    }

    /* Note intersystem bias need to copied as well for Internal API */
    pbFillInterSystemTimeBias("gpsGloInterSystemBias",
            svMeasSetHeader.gpsglointersystembias(),
            ePImplMsrRptOut.gpsGloInterSystemBias);
    pbFillInterSystemTimeBias("gpsBdsInterSystemBias",
            svMeasSetHeader.gpsbdsintersystembias(),
            ePImplMsrRptOut.gpsBdsInterSystemBias);
    pbFillInterSystemTimeBias("gpsGalInterSystemBias",
            svMeasSetHeader.gpsgalintersystembias(),
            ePImplMsrRptOut.gpsGalInterSystemBias);
    pbFillInterSystemTimeBias("bdsGloInterSystemBias",
            svMeasSetHeader.bdsglointersystembias(),
            ePImplMsrRptOut.bdsGloInterSystemBias);
    pbFillInterSystemTimeBias("galGloInterSystemBias",
            svMeasSetHeader.galglointersystembias(),
            ePImplMsrRptOut.galGloInterSystemBias);
    pbFillInterSystemTimeBias("galBdsInterSystemBias",
            svMeasSetHeader.galbdsintersystembias(),
            ePImplMsrRptOut.galBdsInterSystemBias);

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_CLOCK_FREQ) {
        const PBLocRcvrClockFreqInfo& clockFreq = svMeasSetHeader.clockfreq();
        ePImplMsrRptOut.msrReport.clockDrift = clockFreq.clockdrift();
        ePImplMsrRptOut.msrReport.clockDriftStdDeviation = clockFreq.clockdriftunc();
        ePImplMsrRptOut.msrReport.validityMask.isClockDriftValid = 1;
        ePImplMsrRptOut.msrReport.validityMask.isClockDriftStdDeviationValid = 1;
        PBLocSourceofFreqEnumType sourceOfFreq = clockFreq.sourceoffreq();
        if (PB_GNSS_FREQ_SOURCE_INVALID == sourceOfFreq) {
            ePImplMsrRptOut.sourceOfFreq = EP_GNSS_FREQ_SOURCE_INVALID;
        } else if (PB_GNSS_FREQ_SOURCE_EXTERNAL == sourceOfFreq) {
            ePImplMsrRptOut.sourceOfFreq = EP_GNSS_FREQ_SOURCE_EXTERNAL;
        } else if (PB_GNSS_FREQ_SOURCE_PE_CLK_REPORT == sourceOfFreq) {
            ePImplMsrRptOut.sourceOfFreq = EP_GNSS_FREQ_SOURCE_PE_CLK_REPORT;
        } else {
            ePImplMsrRptOut.sourceOfFreq = EP_GNSS_FREQ_SOURCE_UNKNOWN;
        }
    }

    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_TYPE) {
        PBLocDgnssCorrectionSourceType dgnssCorrectionSourceType =
                svMeasSetHeader.dgnsscorrectionsourcetype();
        if (PB_LOC_DGNSS_CORR_SOURCE_TYPE_RTCM == dgnssCorrectionSourceType) {
            ePImplMsrRptOut.dgnssCorrectionSourceType = EP_DGNSS_CORR_SOURCE_TYPE_RTCM;
        } else if (PB_LOC_DGNSS_CORR_SOURCE_TYPE_3GPP == dgnssCorrectionSourceType) {
            ePImplMsrRptOut.dgnssCorrectionSourceType = EP_DGNSS_CORR_SOURCE_TYPE_3GPP;
        }
    }
    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_DGNSS_CORRECTION_SOURCE_ID) {
        ePImplMsrRptOut.dgnssCorrectionSourceID = svMeasSetHeader.dgnsscorrectionsourceid();
    }
    if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_DGNSS_REF_STATION_ID) {
        ePImplMsrRptOut.dgnssRefStationId = svMeasSetHeader.dgnssrefstationid();
    }

    // ePImplMsrRptOut.msrReport.lastPpsLocaltimeStamp.apTimeStamp.tv_sec will be used
    // to deliver ptptime to VPE engine. ptpTimeNs coming from upper layers could be 0
    // for following reasons:
    // 1. Did not get QTimer tick count from MPSS
    // 2. Failed to get ptp time associated with the QTimer tick.
    // TODO: Need to handle the case if ptpTimeNs is 0 and engine type is VPE.
    uint64_t ptpTimeNs = svMeasSet.ptptimens();
    if (0 != ptpTimeNs) {
        ePImplMsrRptOut.msrReport.lastPpsLocaltimeStamp.apTimeStamp.tv_sec = ptpTimeNs;
    } else if (headerFlags & PB_GNSS_SV_MEAS_HEADER_HAS_AP_TIMESTAMP) {
        const PBApTimeStampStructType& apBootTimeStamp = svMeasSetHeader.apboottimestamp();
        const PBTimespec& apTimeStamp = apBootTimeStamp.aptimestamp();
        epGnssApTimeStampStructType& lastPpsLocaltime =
                ePImplMsrRptOut.msrReport.lastPpsLocaltimeStamp;
        lastPpsLocaltime.apTimeStamp.tv_sec = apTimeStamp.tv_sec();
        lastPpsLocaltime.apTimeStamp.tv_nsec = apTimeStamp.tv_nsec();
        lastPpsLocaltime.apTimeStampUncertaintyMs = apBootTimeStamp.aptimestampuncms();
    }

    ePImplMsrRptOut.msrReport.numMeas = 0;
    /* Copy satellite measurement Information */
    for (numMeasurement = 0; (numMeasurement < svMeasCount) &&
         (numMeasurement < EP_GNSS_MAX_MEAS); numMeasurement++) {
        eHgnssSignalTypeMask = 0;
        gnssSignalMask = 0;
        lockPointSet = 0;
        const PBSVMeasurementStructType& svMeasSrc = svMeasSet.svmeas(numMeasurement);
        pSVMeasDst = &ePImplMsrRptOut.msrReport.svMeasurement[numMeasurement];
        uint32_t gnssSignalTypeMask = svMeasSrc.gnsssignaltypemask();
        const PBGnss_LocSvSystemEnumType& eHGnsssystem = svMeasSrc.gnsssystem();
        switch (eHGnsssystem)
        {
            case PB_GNSS_LOC_SV_SYSTEM_GPS:
                eHgnssSignalTypeMask = ((gnssSignalTypeMask != 0) ?
                        gnssSignalTypeMask : PB_GNSS_SIGNAL_TYPE_GPS_L1CA);
                gnssSystem = EP_GNSS_CONSTELLATION_GPS;
                if (ePImplMsrRptOut.msrReport.validityMask.isGpsSystemTimeValid) {
                    pGnssSystemTime = &ePImplMsrRptOut.msrReport.gpsSystemTime;
                }
                break;
            case PB_GNSS_LOC_SV_SYSTEM_BDS:
                eHgnssSignalTypeMask = ((gnssSignalTypeMask != 0) ?
                          gnssSignalTypeMask : PB_GNSS_SIGNAL_TYPE_BEIDOU_B1I);
                gnssSystem = EP_GNSS_CONSTELLATION_BEIDOU;
                if (ePImplMsrRptOut.msrReport.validityMask.isBdsSystemTimeValid) {
                    pGnssSystemTime = &ePImplMsrRptOut.msrReport.bdsSystemTime;
                }
                break;
            case PB_GNSS_LOC_SV_SYSTEM_GALILEO:
                eHgnssSignalTypeMask = ((gnssSignalTypeMask != 0) ?
                          gnssSignalTypeMask : PB_GNSS_SIGNAL_TYPE_GALILEO_E1);
                gnssSystem = EP_GNSS_CONSTELLATION_GALILEO;
                if (ePImplMsrRptOut.msrReport.validityMask.isGalSystemTimeValid) {
                    pGnssSystemTime = &ePImplMsrRptOut.msrReport.galSystemTime;
                }
                break;
            case PB_GNSS_LOC_SV_SYSTEM_QZSS:
                eHgnssSignalTypeMask = ((gnssSignalTypeMask != 0) ?
                          gnssSignalTypeMask : PB_GNSS_SIGNAL_TYPE_QZSS_L1CA);
                gnssSystem = EP_GNSS_CONSTELLATION_QZSS;
                if (ePImplMsrRptOut.msrReport.validityMask.isQzssSystemTimeValid) {
                    pGnssSystemTime = &ePImplMsrRptOut.msrReport.qzssSystemTime;
                }
                break;
            case PB_GNSS_LOC_SV_SYSTEM_GLONASS:
                eHgnssSignalTypeMask = ((gnssSignalTypeMask != 0) ?
                          gnssSignalTypeMask : PB_GNSS_SIGNAL_TYPE_GLONASS_G1);
                gnssSystem = EP_GNSS_CONSTELLATION_GLONASS;
                if (ePImplMsrRptOut.msrReport.validityMask.isGloSystemTimeValid) {
                    pGloSystemTime = &ePImplMsrRptOut.msrReport.gloSystemTime;
                }
                break;
            default:
                LOC_LOGd("Unsupported GNSS Sytem %d", eHGnsssystem);
        }
        pbGetEPGnssSignalMask(eHgnssSignalTypeMask, gnssSignalMask);

        pSVMeasDst->gnssSystem = gnssSystem;
        pSVMeasDst->gnssSignal = gnssSignalMask;
        pSVMeasDst->gnssSvId   = svMeasSrc.gnsssvid();

        if (PB_GNSS_LOC_SV_SYSTEM_GLONASS == eHGnsssystem) {
            pSVMeasDst->gloFrequency = svMeasSrc.glofrequency();
        }
        pSVMeasDst->svSearchState = svMeasSrc.svstatus();

        const PBSVTimeSpeedInfo& svTimeSpeed = svMeasSrc.svtimespeed();
        pSVMeasDst->svMs = svTimeSpeed.svms();
        pSVMeasDst->validityMask.isSVMSValid = 1;
        pSVMeasDst->svSubMs = svTimeSpeed.svsubms();
        pSVMeasDst->validityMask.isSVSubMSValid = 1;
        pSVMeasDst->svTimeUncMs = svTimeSpeed.svtimeuncms();
        pSVMeasDst->validityMask.isSVTimeUncValid = 1;

       /* Initialize pseudorange parameters */
        pSVMeasDst->pseudorange = 0.0;
        pSVMeasDst->pseudorange_uncertainty = 0.0;
        pSVMeasDst->validityMask.isPseudorangeValid = 0;
        pSVMeasDst->validityMask.isPseudorangeUncValid = 0;

        pSVMeasDst->dopplerShift = svTimeSpeed.dopplershift();
        pSVMeasDst->dopplerShiftUnc = svTimeSpeed.dopplershiftunc();
        /* Currently, our 10Hz loop cannot provide true doppler.
         * What we have is the doppler from the prior 1 hz
         * measurement which would be stale and not correct.
         * For the 10Hz Doppler measurement set validity flag to false
         */
        if (true == isNhz) {
            pSVMeasDst->validityMask.isDopplerValid = 0;
            pSVMeasDst->validityMask.isDopplerUncValid = 0;
        } else {
            /* 1Hz */
            pSVMeasDst->validityMask.isDopplerValid = 1;
            pSVMeasDst->validityMask.isDopplerUncValid = 1;
        }
        bool healthStatusValid = svMeasSrc.healthstatusvalid();
        bool healthStatus = svMeasSrc.healthstatus();
        pSVMeasDst->validityMask.isHealthStatusValid = 1;
        if ((1 == healthStatusValid) && (1 == healthStatus)) {
            pSVMeasDst->healthStatus = 1; //Healthy
        } else if ( (1 == healthStatusValid) && (0 == healthStatus)) {
            pSVMeasDst->healthStatus = 0; //Unhealthy
        } else {
            pSVMeasDst->healthStatus = 2; // Unknown
        }
        uint32_t svInfoMask = svMeasSrc.svinfomask();
        if (PB_GNSS_SVINFO_MASK_HAS_EPHEMERIS == svInfoMask) {
            pSVMeasDst->svInfoMask = EP_GNSS_SVINFO_MASK_HAS_EPHEMERIS;
            pSVMeasDst->validityMask.isSvInfoMaskValid  = 1;
        } else if (PB_GNSS_SVINFO_MASK_HAS_ALMANAC == svInfoMask) {
            pSVMeasDst->svInfoMask = EP_GNSS_SVINFO_MASK_HAS_ALMANAC;
            pSVMeasDst->validityMask.isSvInfoMaskValid  = 1;
        }

        measurementStatus = svMeasSrc.measurementstatus();
        /* Copy all bits except reserved. Reserved bit shall be copied to internal API */
        pSVMeasDst->measurementStatus = measurementStatus & (~EP_MEAS_STATUS_RESERVED);

        pSVMeasDst->cN0dbHz = svMeasSrc.cno() / 10.0;
        pSVMeasDst->validityMask.isCNoValid = 1;

        bool carrierPhaseValid = svMeasSrc.carrierphasevalid();
        if (carrierPhaseValid) {
            pSVMeasDst->carrierPhase = svMeasSrc.carrierphase();
            pSVMeasDst->validityMask.isCarrierPhaseValid = 1;
        } else {
            pSVMeasDst->lossOfLock.lossOfContinousLock = 1;
        }
        bool carrierPhaseUncValid = svMeasSrc.carrierphaseuncvalid();
        if (carrierPhaseUncValid) {
            pSVMeasDst->carrierPhaseUnc = svMeasSrc.carrierphaseunc();
            pSVMeasDst->validityMask.isCarrierPhaseUncValid = 1;
        }
        bool cycleSlipCountValid = svMeasSrc.cycleslipcountvalid();
        uint32_t cycleSlipCount = svMeasSrc.cycleslipcount();
        if (cycleSlipCountValid) {
            pSVMeasDst->cycleSlipCount = cycleSlipCount;
            pSVMeasDst->validityMask.isCycleSlipCountValid = 1;
        }

        pSVMeasDst->validityMask.isLossOfLockValid  = 1;

        /*~~~~~ Internal API only ~~~~~*/
        pSVImplMeasDest = &ePImplMsrRptOut.svImplMeasurement[numMeasurement];
        pSVImplMeasDest->rfLoss = svMeasSrc.glorfloss() / 10;
        pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_RF_LOSS_VALID;
        pSVImplMeasDest->measLatency = svMeasSrc.measlatency();
        pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_MEAS_LATENCY_VALID;
        pSVImplMeasDest->gnssSystem = gnssSystem;
        pSVImplMeasDest->gnssSvId = pSVMeasDst->gnssSvId;
        if (svMeasSrc.finespeedvalid()) {
            pSVImplMeasDest->fineSpeed = svMeasSrc.finespeed();
            pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_FINE_SPEED_VALID;
        }
        if (svMeasSrc.finespeeduncvalid()) {
            pSVImplMeasDest->fineSpeedUnc = svMeasSrc.finespeedunc();
            pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_FINE_SPEED_UNC_VALID;
        }
        pSVImplMeasDest->dopplerAccel = svMeasSrc.doppleraccel();
        pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_DOPPLER_ACCEL_VALID;
        if (svMeasSrc.multipathestvalid()) {
            pSVImplMeasDest->multipathEstimate = svMeasSrc.multipathestimate();
            pSVImplMeasDest->msrImplStatusMask |=
                    EP_MEAS_IMPL_STATUS_MULTIPATH_ESTIMATE_VALID;
        }

        if (svMeasSrc.svdirectionvalid()) {
            pSVImplMeasDest->svAzimuth = svMeasSrc.svazimuth();
            pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_SV_AZIMUTH_VALID;
            pSVImplMeasDest->svElevation = svMeasSrc.svelevation();
            pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_IMPL_STATUS_SV_ELEVATION_VALID;
        }

        if ((measurementStatus & PB_GNSS_LOC_MEAS_STATUS_VELOCITY_FINE)
                     == PB_GNSS_LOC_MEAS_STATUS_VELOCITY_FINE) {
            pSVImplMeasDest->msrImplStatusMask |= EP_MEAS_STATUS_VELOCITY_FINE;
        }
        pbFillEpDgnssSVMeasurement(svMeasSrc.dgnsssvmeas(), pSVImplMeasDest->dgnssSvMeas);
    }
    ePImplMsrRptOut.msrReport.numMeas = numMeasurement;
    return true;
}

void EpProtoMsgConverter::pbFillEpGnssSvPolyTgd
(
    const EHMessageReportSvPolynomial *pEHubSvPolyIn,
    epGnssSvPolynomial &epSvPolyOut
)
{
    if (nullptr == pEHubSvPolyIn) {
        LOC_LOGe("pEHubSvPolyIn = nullptr !! return ");
        return;
    }
    const PBGnssSvPolynomial& svPolynomial = pEHubSvPolyIn->svpolynomial();
    uint32_t validFlags = svPolynomial.is_valid();
    /* GPS TGD */
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L1CA) {
        epSvPolyOut.tgd.gps.iscL1ca = svPolynomial.gpsiscl1ca();
        epSvPolyOut.tgd.gps.valid.isIscL1caValid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L2C) {
        epSvPolyOut.tgd.gps.iscL2 = svPolynomial.gpsiscl2c();
        epSvPolyOut.tgd.gps.valid.isIscL2cValid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L5I5) {
        epSvPolyOut.tgd.gps.iscL5I5 = svPolynomial.gpsiscl5i5();
        epSvPolyOut.tgd.gps.valid.isIscL5I5Valid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GPS_ISC_L5Q5) {
        epSvPolyOut.tgd.gps.iscL5Q5 = svPolynomial.gpsiscl5q5();
        epSvPolyOut.tgd.gps.valid.isIscL5Q5Valid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GPS_TGD) {
        epSvPolyOut.tgd.gps.tgdGps = svPolynomial.gpstgd();
        epSvPolyOut.tgd.gps.valid.isTgdValid = 1;
    }
    /* GLO TGD */
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GLO_TGD_G1G2) {
        epSvPolyOut.tgd.glo.tgdG1G2 = svPolynomial.glotgdg1g2();
        epSvPolyOut.tgd.glo.valid.isTgdG1G2Valid = 1;
    }
    /* BDS TGD */
    if (validFlags & ULP_GNSS_SV_POLY_BIT_BDS_TGD_B1) {
        epSvPolyOut.tgd.bds.tgdB1 = svPolynomial.bdstgdb1();
        epSvPolyOut.tgd.bds.valid.isTgdB1Valid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_BDS_TGD_B2) {
        epSvPolyOut.tgd.bds.tgdB2 = svPolynomial.bdstgdb2();
        epSvPolyOut.tgd.bds.valid.isTgdB2Valid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_BDS_TGD_B2A) {
        epSvPolyOut.tgd.bds.tgdB2a = svPolynomial.bdstgdb2a();
        epSvPolyOut.tgd.bds.valid.isTgdB2AValid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_BDS_ISC_B2A) {
        epSvPolyOut.tgd.bds.iscB2a = svPolynomial.bdsiscb2a();
        epSvPolyOut.tgd.bds.valid.isIscB2AValid = 1;
    }
    /* GAL TGD */
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GAL_BGD_E1E5A) {
        epSvPolyOut.tgd.gal.bgdE1E5a = svPolynomial.galbgde1e5a();
        epSvPolyOut.tgd.gal.valid.isTgdE1E5aValid = 1;
    }
    if (validFlags & ULP_GNSS_SV_POLY_BIT_GAL_BGD_E1E5B) {
        epSvPolyOut.tgd.gal.bgdE1E5b = svPolynomial.galbgde1e5b();
        epSvPolyOut.tgd.gal.valid.isTgdE1E5bValid = 1;
    }
    /* NAVIC TGD */
    if (validFlags & ULP_GNSS_SV_POLY_BIT_NAVIC_TGD_L5) {
        epSvPolyOut.tgd.navic.tgdL5 = svPolynomial.navictgdl5();
        epSvPolyOut.tgd.navic.valid.isTgdL5Valid = 1;
    }
}

eHubSubModifiers EpProtoMsgConverter::pbConvertToEHubSubModifiers(
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

eHubSubscriptionModifier EpProtoMsgConverter::pbConvertToEHSubscriptionModifier(
                const EHSubscriptionModifier &pbEHSubscriptionModifier) {
    eHubSubscriptionModifier localEhSubscriptionModifier;
    uint32_t pbGnssConstellationInfoMask = pbEHSubscriptionModifier.constellationmask();
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
    localEhSubscriptionModifier.constellationMask = gnssConstellationTypeMask;

    uint32_t pbGnssSignalMask = pbEHSubscriptionModifier.gnsssignalmask();
    uint32_t gnssSignalMask = 0;
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_GPS) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_GPS_BIT;
    }
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_GLONASS) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_GLONASS_BIT;
    }
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_QZSS) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_QZSS_BIT;
    }
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_BEIDOU) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_BEIDOU_BIT;
    }
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_GALILEO) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_GALILEO_BIT;
    }
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_SBAS) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_SBAS_BIT;
    }
    if (pbGnssSignalMask & PB_GNSS_CONSTELLATION_TYPE_NAVIC) {
        gnssSignalMask |= GNSS_CONSTELLATION_TYPE_NAVIC_BIT;
    }
    localEhSubscriptionModifier.gnssSignalMask = gnssSignalMask;

    LOC_LOGd("pbConvertToEHSubscriptionModifier Gnss SignalMask : %x",
            localEhSubscriptionModifier.gnssSignalMask);
    return localEhSubscriptionModifier;
}

void EpProtoMsgConverter::pbFillEpGnssSvPolynomial
(
    const EHMessageReportSvPolynomial *pEHubSvPolyIn,
    epImplGnssSvPolynomial &epSvPoly
)
{
    LOC_LOGd(" -> \n");
    if (NULL == pEHubSvPolyIn) {
        LOC_LOGd("pEHubSvPolyIn == NULL !! return");
        return ;
    }

    memset(&epSvPoly, 0, sizeof(epSvPoly));
    const PBGnssSvPolynomial& svPolynomial = pEHubSvPolyIn->svpolynomial();
    uint8_t polyCoeffXYZLen = 0, polyClockBiasLen = 0;
    epGnssSvPolynomial  &epSvPolyOut = epSvPoly.svPoly;
    epSvPolyOut.gnssSvId  = svPolynomial.gnsssvid();
    epSvPolyOut.freqNum   = svPolynomial.freqnum();
    epSvPolyOut.polyStatus = 0;
    epSvPolyOut.polyAction = EP_UPDATE_POLY; //Default value
    uint32_t validFlags = svPolynomial.is_valid();
    //TODO Update these ULp_XX definitions with PB_XXX
    if (validFlags & ULP_GNSS_SV_POLY_BIT_FLAG ) {
        uint32_t svPolyStatusMaskValidity =
                svPolynomial.svpolystatusmaskvalidity();
        uint32_t svPolyStatusMask = svPolynomial.svpolystatusmask();
        if ((svPolyStatusMaskValidity & PB_GNSS_SV_POLY_SRC_ALM_CORR_VALID_V02) &&
            (svPolyStatusMask & PB_GNSS_SV_POLY_SRC_ALM_CORR_V02)) {
            epSvPolyOut.polyStatus |= EP_SV_POLY_SRC_ALM_CORR;
        }
        if ((svPolyStatusMaskValidity & PB_GNSS_SV_POLY_GLO_STR4_VALID_V02) &&
            (svPolyStatusMask & PB_GNSS_SV_POLY_GLO_STR4_V02)) {
            epSvPolyOut.polyStatus |= EP_SV_POLY_GLO_STR4;
        }
        if (svPolyStatusMaskValidity & PB_GNSS_SV_POLY_SRC_GAL_FNAV_OR_INAV_VALID_V02) {
            if (svPolyStatusMask & PB_GNSS_SV_POLY_SRC_GAL_FNAV_OR_INAV_V02) {
                epSvPolyOut.polyStatus |= EP_SV_POLY_GAL_FNAV;
            } else {
                epSvPolyOut.polyStatus |= EP_SV_POLY_GAL_INAV;
            }
        }
        if ((svPolyStatusMaskValidity & PB_GNSS_SV_POLY_DELETE_VALID_V02) &&
            (svPolyStatusMask & PB_GNSS_SV_POLY_DELETE_V02)) {
            epSvPolyOut.polyAction = EP_INVALIDATE_POLY;
        }
    }
    if (EP_INVALIDATE_POLY != epSvPolyOut.polyAction) {
        if (validFlags & ULP_GNSS_SV_POLY_BIT_IODE)
        {
            epSvPolyOut.iode = svPolynomial.iode();
            epSvPolyOut.validityMask.isIodeValid = true;
        }
        epSvPolyOut.T0 = svPolynomial.t0();
        epSvPolyOut.validityMask.isT0Valid = true;

        if (validFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZ0) {
            epSvPolyOut.validityMask.isPolyCoeffXYZ0Valid = true;
            for (int i = 0; i < EP_SV_POLY_XYZ_0_TH_ORDER_COEFF_MAX_SIZE; i++) {
                epSvPolyOut.polyCoeffXYZ0[i] = svPolynomial.polycoeffxyz0(i);
            }
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZN) {
            epSvPolyOut.validityMask.isPolyCoefXYZNValid= true;
            for (int i = 0; i < EP_SV_POLY_XYZ_N_TH_ORDER_COEFF_MAX_SIZE; i++)
            {
                epSvPolyOut.polyCoefXYZN[i] = svPolynomial.polycoefxyzn(i);
            }
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_OTHER) {
            epSvPolyOut.validityMask.isPolyCoefOtherValid  = true;
            for (int i = 0; i < EP_SV_POLY_XYZ_0_TH_ORDER_COEFF_MAX_SIZE; i++)
            {
                epSvPolyOut.polyCoefOther[i] = svPolynomial.polycoefother(i);
            }
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_SV_POSUNC) {
            epSvPolyOut.svPosUnc = svPolynomial.svposunc();
            epSvPolyOut.validityMask.isSvPosUncValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_IONODELAY) {
            epSvPolyOut.ionoDelay = svPolynomial.ionodelay();
            epSvPolyOut.validityMask.isIonoDelayValid  = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_IONODOT) {
            epSvPolyOut.ionoDot = svPolynomial.ionodot();
            epSvPolyOut.validityMask.isIonoDotValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_SBAS_IONODELAY) {
            epSvPolyOut.sbasIonoDelay = svPolynomial.sbasionodelay();
            epSvPolyOut.validityMask.isSbasIonoDelayValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_SBAS_IONODOT) {
            epSvPolyOut.sbasIonoDot = svPolynomial.sbasionodot();
            epSvPolyOut.validityMask.isSbasIonoDotValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_TROPODELAY) {
            epSvPolyOut.tropoDelay = svPolynomial.tropodelay();
            epSvPolyOut.validityMask.isTropoDelayValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_ELEVATION) {
            epSvPolyOut.elevation = svPolynomial.elevation();
            epSvPolyOut.validityMask.isElevationValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_ELEVATIONDOT) {
            epSvPolyOut.elevationDot = svPolynomial.elevationdot();
            epSvPolyOut.validityMask.isElevationDotValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_ELEVATIONUNC) {
            epSvPolyOut.elevationUnc = svPolynomial.elevationunc();
            epSvPolyOut.validityMask.isElevationUncValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_VELO_COEFF) {
            epSvPolyOut.validityMask.isVelCoefValid = true;
            for (int i = 0; i < EP_SV_POLY_VELOCITY_COEF_MAX_SIZE; i++)
            {
                epSvPolyOut.velCoef[i] = svPolynomial.velcoef(i);
            }
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_ENHANCED_IOD) {
            epSvPolyOut.enhancedIOD = svPolynomial.enhancediod();
            epSvPolyOut.validityMask.isEnhancedIODValid = true;
        }

        if (validFlags & ULP_GNSS_SV_POLY_BIT_POLY_ORDER) {
            epSvPoly.polyOrder = svPolynomial.polyorder();
            epSvPoly.validityStatus.isPolyOrderValid = true;
        }

        if (EP_IMPL_GNSS_SV_POLY_ORDER_DEFAULT < epSvPoly.polyOrder &&
                EP_IMPL_GNSS_SV_POLY_ORDER_SIZE_MAX >= epSvPoly.polyOrder) {
            if (validFlags & ULP_GNSS_SV_POLY_BIT_POLY_DURATION) {
                epSvPoly.validDuration = svPolynomial.validduration();
                epSvPoly.validityStatus.validDurationValidity = true;
            }

            polyCoeffXYZLen = 3 * (epSvPoly.polyOrder) + 3;

            if (validFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_XYZ) {
                epSvPoly.validityStatus.isPolyCoeffXYZValid = true;
                for (uint8_t i = 0; i < polyCoeffXYZLen &&
                        i < EP_IMPL_POLY_XYZ_COEFF_MAX_SIZE; i++) {
                    epSvPoly.polyCoeffXYZ[i] = svPolynomial.polycoeffxyz(i);
                }
            }

            polyClockBiasLen = epSvPoly.polyOrder + 1;

            if (validFlags & ULP_GNSS_SV_POLY_BIT_POLYCOEFF_CLKBIAS) {
                epSvPoly.validityStatus.isPolyClockBiasValid = true;
                for (uint8_t i = 0; i < polyClockBiasLen &&
                        i < EP_IMPL_SV_POLY_CLKBIAS_COEFF_MAX_SIZE; i++) {
                    epSvPoly.polyClkBias[i] = svPolynomial.polyclockbias(i);
                }
            }
        }

        pbFillEpGnssSvPolyTgd(pEHubSvPolyIn, epSvPolyOut);
    }
}

void EpProtoMsgConverter::pbFillEpSvReport(const EHMessageReportSv *pEhubSvRpt,
                                    epGnssSvNotification &ePSvReport)
{
    if (NULL == pEhubSvRpt) {
        LOC_LOGd("pEhubSvRpt == NULL !! return");
        return ;
    }
    int numSVMax, i;
    const PBGnssSvNotification& svNotification = pEhubSvRpt->svnotification();
    numSVMax = svNotification.gnsssvs_size();
    if (numSVMax > EP_GNSS_MAX_MEAS) {
        numSVMax = EP_GNSS_MAX_MEAS;
    }
    epGnssSignalTypeMask gnssSignalMask = 0;
    ePSvReport.svCount = 0;
    for (i = 0; i < numSVMax; i++)
    {
        const PBGnssSv& gnssSv = svNotification.gnsssvs(i);
        PBGnssSvType type = gnssSv.type();
        epGnssSvStatusMask mask = 0;

        switch (type)
        {
            case PB_GNSS_SV_TYPE_GPS:
                ePSvReport.gnssSvs[ePSvReport.svCount].gnssConstellation =
                        EP_GNSS_CONSTELLATION_GPS;
                gnssSignalMask = EP_GNSS_SIGNAL_GPS_L1CA;
            break;
            case PB_GNSS_SV_TYPE_GALILEO:
                ePSvReport.gnssSvs[ePSvReport.svCount].gnssConstellation =
                        EP_GNSS_CONSTELLATION_GALILEO;
                gnssSignalMask = EP_GNSS_SIGNAL_GALILEO_E1_C;
            break;
            case PB_GNSS_SV_TYPE_SBAS:
                ePSvReport.gnssSvs[ePSvReport.svCount].gnssConstellation =
                        EP_GNSS_CONSTELLATION_SBAS;
                gnssSignalMask = EP_GNSS_SIGNAL_SBAS_L1_CA;
            break;
            case PB_GNSS_SV_TYPE_GLONASS:
                ePSvReport.gnssSvs[ePSvReport.svCount].gnssConstellation =
                        EP_GNSS_CONSTELLATION_GLONASS;
                gnssSignalMask = EP_GNSS_SIGNAL_GLONASS_G1_CA;
            break;
            case PB_GNSS_SV_TYPE_BEIDOU:
                ePSvReport.gnssSvs[ePSvReport.svCount].gnssConstellation =
                        EP_GNSS_CONSTELLATION_BEIDOU;
                gnssSignalMask = EP_GNSS_SIGNAL_BEIDOU_B1_I;
            break;
            case PB_GNSS_SV_TYPE_QZSS:
                ePSvReport.gnssSvs[ePSvReport.svCount].gnssConstellation =
                        EP_GNSS_CONSTELLATION_QZSS;
                gnssSignalMask = EP_GNSS_SIGNAL_QZSS_L1CA;
            break;
            default:
            break;
        }

        ePSvReport.gnssSvs[ePSvReport.svCount].gnssSvId = gnssSv.svid();
        ePSvReport.gnssSvs[ePSvReport.svCount].gnssSignalType =
                (svNotification.gnsssignaltypemaskvalid() ?
                gnssSv.gnsssignaltypemask() : gnssSignalMask);
        ePSvReport.gnssSvs[ePSvReport.svCount].cN0dbHz = gnssSv.cn0dbhz();
        ePSvReport.gnssSvs[ePSvReport.svCount].elevation = gnssSv.elevation();
        ePSvReport.gnssSvs[ePSvReport.svCount].validityMask.isElevationValid = 1;
        ePSvReport.gnssSvs[ePSvReport.svCount].azimuth = gnssSv.azimuth();
        ePSvReport.gnssSvs[ePSvReport.svCount].validityMask.isAzimuthValid = 1;
        uint32_t gnssSvOptionsMask = gnssSv.gnsssvoptionsmask();

        if (gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_EPHEMER_BIT) {
            mask |= EP_GNSS_SV_OPTIONS_HAS_EPHEMERIS_BIT;
        }
        if (gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_ALMANAC_BIT) {
              mask |= EP_GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
        }
        if (gnssSvOptionsMask & GNSS_SV_OPTIONS_USED_IN_FIX_BIT) {
              mask |= EP_GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
        }

        ePSvReport.gnssSvs[ePSvReport.svCount].gnssSvStatusMask = mask;
        ePSvReport.svCount++;
    }
}

void EpProtoMsgConverter::pbFillEHubPosFlags(epPositionFlags epPosFlags,
        PBLocPosTechMask &techMask)

{
    uint32_t pbTechMask = PB_LOC_POS_TECH_MASK_DEFAULT;
    if (epPosFlags & EP_POSITION_FLAG_GNSS_USED) {
        pbTechMask |= PB_LOC_POS_TECH_MASK_SATELLITE;
    }
    if (epPosFlags & EP_POSITION_FLAG_DR_ONLY) {
        pbTechMask |= PB_LOC_POS_TECH_MASK_SENSORS;
    }
    if (epPosFlags & (EP_POSITION_FLAG_RTK_CORR | EP_POSITION_FLAG_PPP_CORR)) {
        pbTechMask |= PB_LOC_POS_TECH_MASK_PPE;
    }
    techMask = (PBLocPosTechMask)pbTechMask;
}

void EpProtoMsgConverter::pbFillEHubGnssSystemTimeInfo
(
    const epGnssSystemTimeStructType* pSystemTime,
    PBGnssSystemTimeStructType *gnssTime
)
{
    if (nullptr == pSystemTime) {
        LOC_LOGe("pSystemTime = nullptr !! return ");
        return;
    }
    uint32_t pbValidityMask = PB_GNSS_SYSTEM_TIME_FLAG_INVALID;
    if (pSystemTime->validityMask.isSystemWeekValid) {
        gnssTime->set_systemweek(pSystemTime->systemWeek);
        pbValidityMask |= PB_GNSS_SYSTEM_TIME_WEEK_VALID;
    }
    if (pSystemTime->validityMask.isSystemWeekMsecValid) {
        gnssTime->set_systemmsec(pSystemTime->systemMsec);
        pbValidityMask |= PB_GNSS_SYSTEM_TIME_WEEK_MS_VALID;
    }
    if (pSystemTime->validityMask.isSystemClkTimeBiasValid) {
        gnssTime->set_systemclktimebias(pSystemTime->systemClkTimeBias);
        pbValidityMask |= PB_GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
    }
    if (pSystemTime->validityMask.isSystemClkTimeUncMsValid) {
        gnssTime->set_systemclktimeuncms(pSystemTime->systemClkTimeUncMs);
        pbValidityMask |= PB_GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
    }
    if (pSystemTime->validityMask.isRefFCountValid) {
        gnssTime->set_reffcount(pSystemTime->refFCount);
        pbValidityMask |= PB_GNSS_SYSTEM_REF_FCOUNT_VALID;
    }
    if (pSystemTime->validityMask.isNumClockResetsValid) {
        gnssTime->set_numclockresets(pSystemTime->numClockResets);
        pbValidityMask |= PB_GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
    }
    gnssTime->set_validitymask(pbValidityMask);
}

void EpProtoMsgConverter::pbFillEHubGloSystemTimeInfo
(
    const epGnssGloTimeStructType* pSystemTime,
    PBGnssGloTimeStructType *gloTime
)
{
    if (nullptr == pSystemTime) {
        LOC_LOGe("pSystemTime = nullptr !! return ");
        return;
    }
    // gloDays, gloMsec and gloFourYear are manditory fileds
    gloTime->set_glodays(pSystemTime->gloDays);
    uint32_t validityMask = PB_GNSS_CLO_TIME_UNKNOWN;
    validityMask |= PB_GNSS_CLO_DAYS_VALID;
    gloTime->set_glomsec(pSystemTime->gloMsec);
    validityMask |= GNSS_GLO_MSEC_VALID;
    gloTime->set_glofouryear(pSystemTime->gloFourYear);
    validityMask |= GNSS_GLO_FOUR_YEAR_VALID;
    if (pSystemTime->validityMask.isGloClkTimeBiasValid) {
        gloTime->set_gloclktimebias(pSystemTime->gloClkTimeBias);
        validityMask |= GNSS_GLO_CLK_TIME_BIAS_VALID;
    }
    if (pSystemTime->validityMask.isGloClkTimeUncMsValid) {
        gloTime->set_gloclktimeuncms(pSystemTime->gloClkTimeUncMs);
        validityMask |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
    }
    if (pSystemTime->validityMask.isRefFCountValid) {
        gloTime->set_reffcount(pSystemTime->refFCount);
        validityMask |= GNSS_GLO_REF_FCOUNT_VALID;
    }
    if (pSystemTime->validityMask.isNumClockResetsValid) {
        gloTime->set_numclockresets(pSystemTime->numClockResets);
        validityMask |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
    }
    gloTime->set_validitymask(validityMask);
}

void EpProtoMsgConverter::pbFillEHubNavSolutionMask
(
    const epPositionFlags &ePPosFlags,
    uint32_t &pbNavSolutionMask
)
{
    uint32_t navSolutionMask = PB_LOC_NAV_MASK_INVALID;
    if (EP_POSITION_FLAG_SBAS_IONO_CORR & ePPosFlags) {
        navSolutionMask |= PB_LOC_NAV_MASK_SBAS_CORRECTION_IONO;
    }
    if (EP_POSITION_FLAG_SBAS_FAST_CORR & ePPosFlags) {
        navSolutionMask |= PB_LOC_NAV_MASK_SBAS_CORRECTION_FAST;
    }
    if (EP_POSITION_FLAG_SBAS_LONG_CORR & ePPosFlags) {
        navSolutionMask |= PB_LOC_NAV_MASK_SBAS_CORRECTION_LONG;
    }
    if (EP_POSITION_FLAG_RTK_CORR & ePPosFlags) {
        navSolutionMask |= PB_LOC_NAV_MASK_RTK_CORRECTION;
    }
    if (EP_POSITION_FLAG_DGNSS_CORR & ePPosFlags) {
        navSolutionMask |= PB_LOC_NAV_MASK_DGNSS_CORRECTION;
    }
    if (EP_POSITION_FLAG_PPP_CORR & ePPosFlags) {
        navSolutionMask |= PB_LOC_NAV_MASK_PPP_CORRECTION;
    }
    pbNavSolutionMask = (PBLocNavSolutionMask)navSolutionMask;
}

void EpProtoMsgConverter::pbFillEHubPVTReport
(
    const epImplPVTReport* pEPImplPVTRptIn,
    const Gnss_ApTimeStampStructType& apTimestamp,
    PBUlpLocation *pbLocation,
    PBGpsLocationExtended *pbLocationExtended
)
{
    if (nullptr == pEPImplPVTRptIn) {
        LOC_LOGe("pEPImplPVTRptIn = nullptr !! return ");
        return;
    }
    LOC_LOGd(" ->");
    if (NULL == pEPImplPVTRptIn) {
        LOC_LOGw("pEPImplPVTRptIn == NULL !! return!");
        return ;
    }
    const epPVTReport* pEPPVTRptIn = &pEPImplPVTRptIn->pvtReport;
    uint32_t navSolutionMask = 0;
    //PBGpsLocationExtendedFlagsLower32 flagsLower32 = PB_GPS_LOCATION_EXTENDED_LOWER32_INVALID;
    //PBGpsLocationExtendedFlagsUpper32 flagsUpper32 = PB_GPS_LOCATION_EXTENDED_UPPER32_INVALID;
    uint32_t flagsLower32 = PB_GPS_LOCATION_EXTENDED_LOWER32_INVALID;
    uint32_t flagsUpper32 = PB_GPS_LOCATION_EXTENDED_UPPER32_INVALID;
    PBLocPosTechMask eXtendedTechMask = PB_LOC_POS_TECH_MASK_DEFAULT;

    pbFillEHubNavSolutionMask(pEPPVTRptIn->posFlags, navSolutionMask);
    pbLocationExtended->set_navsolutionmask(navSolutionMask);
    if (0 != navSolutionMask) {
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK;
    }
    /*Fill position flags */
    pbFillEHubPosFlags(pEPPVTRptIn->posFlags, eXtendedTechMask);
    pbLocationExtended->set_tech_mask(eXtendedTechMask);
    pbLocation->set_tech_mask(eXtendedTechMask);
    PBApTimeStampStructType *pbTimestamp = pbLocationExtended->mutable_timestamp();
    /*Fill ap timestamp */
    if (nullptr != pbTimestamp) {
        PBTimespec *pTimespec = pbTimestamp->mutable_aptimestamp();
        if (nullptr != pTimespec) {
            pTimespec->set_tv_sec(apTimestamp.apTimeStamp.tv_sec);
            pTimespec->set_tv_nsec(apTimestamp.apTimeStamp.tv_nsec);
        }
        pbTimestamp->set_aptimestampuncms(apTimestamp.apTimeStampUncertaintyMs);
    }

    if (PB_LOC_POS_TECH_MASK_DEFAULT != eXtendedTechMask) {
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK;
    }
    PBGnssSystemTime *gnssSystemTime = pbLocationExtended->mutable_gnsssystemtime();
    if (NULL == gnssSystemTime) {
        LOC_LOGw("gnssSystemTime == NULL !! return!");
        return ;
    }
    PBGnss_LocSvSystemEnumType gnssSystemTimeSrc = PB_GNSS_LOC_SV_SYSTEM_UNKNOWN;
    PBSystemTimeStructUnion *pUnion = gnssSystemTime->mutable_u();
    if (NULL == pUnion) {
        LOC_LOGw("pUnion == NULL !! return!");
        return ;
    }
    if (EP_GNSS_CONSTELLATION_GPS == pEPPVTRptIn->epGnssSystemTime.gnssSystemTimeSrc) {
        gnssSystemTimeSrc = PB_GNSS_LOC_SV_SYSTEM_GPS;
        PBGnssSystemTimeStructType *gpsSystemTime = pUnion->mutable_gpssystemtime();
        if (NULL == gpsSystemTime) {
            LOC_LOGw("gpsSystemTime == NULL !! return!");
            return ;
        }
        pbFillEHubGnssSystemTimeInfo(&pEPPVTRptIn->epGnssSystemTime.u.gpsSystemTime, gpsSystemTime);
        /* Support legacy behaviour */
        if (pEPPVTRptIn->epGnssSystemTime.u.gpsSystemTime.validityMask.isSystemWeekValid &&
            pEPPVTRptIn->epGnssSystemTime.u.gpsSystemTime.validityMask.isSystemWeekMsecValid) {
            PBGPSTimeStruct *gpsTime = pbLocationExtended->mutable_gpstime();
            if (nullptr != gpsTime) {
                gpsTime->set_gpsweek(pEPPVTRptIn->epGnssSystemTime.u.gpsSystemTime.systemWeek);
                gpsTime->set_gpstimeofweekms(
                        pEPPVTRptIn->epGnssSystemTime.u.gpsSystemTime.systemMsec);
                flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_GPS_TIME;
            }
        }
    } else if (EP_GNSS_CONSTELLATION_GALILEO ==
            pEPPVTRptIn->epGnssSystemTime.gnssSystemTimeSrc) {
        gnssSystemTimeSrc = PB_GNSS_LOC_SV_SYSTEM_GALILEO;
        PBGnssSystemTimeStructType *galSystemTime = pUnion->mutable_galsystemtime();
        if (NULL == galSystemTime) {
            LOC_LOGw("galSystemTime == NULL !! return!");
            return ;
        }
        pbFillEHubGnssSystemTimeInfo(&pEPPVTRptIn->epGnssSystemTime.u.galSystemTime, galSystemTime);
    } else if (EP_GNSS_CONSTELLATION_BEIDOU ==
            pEPPVTRptIn->epGnssSystemTime.gnssSystemTimeSrc) {
        gnssSystemTimeSrc = PB_GNSS_LOC_SV_SYSTEM_BDS;
        PBGnssSystemTimeStructType *bdsSystemTime = pUnion->mutable_bdssystemtime();
        if (NULL == bdsSystemTime) {
            LOC_LOGw("bdsSystemTime == NULL !! return!");
            return ;
        }
        pbFillEHubGnssSystemTimeInfo(&pEPPVTRptIn->epGnssSystemTime.u.bdsSystemTime, bdsSystemTime);
    } else if (EP_GNSS_CONSTELLATION_QZSS ==
            pEPPVTRptIn->epGnssSystemTime.gnssSystemTimeSrc) {
        gnssSystemTimeSrc = PB_GNSS_LOC_SV_SYSTEM_QZSS;
        PBGnssSystemTimeStructType *qzssSystemTime = pUnion->mutable_qzsssystemtime();
        if (NULL == qzssSystemTime) {
            LOC_LOGw("qzssSystemTime == NULL !! return!");
            return ;
        }
        pbFillEHubGnssSystemTimeInfo(&pEPPVTRptIn->epGnssSystemTime.u.qzssSystemTime,
                qzssSystemTime);
    } else if (EP_GNSS_CONSTELLATION_GLONASS ==
            pEPPVTRptIn->epGnssSystemTime.gnssSystemTimeSrc) {
        gnssSystemTimeSrc = PB_GNSS_LOC_SV_SYSTEM_GLONASS;
        PBGnssGloTimeStructType *gloSystemTime = pUnion->mutable_glosystemtime();
        if (NULL == gloSystemTime) {
            LOC_LOGw("gloSystemTime == NULL !! return!");
            return ;
        }
        pbFillEHubGloSystemTimeInfo(&pEPPVTRptIn->epGnssSystemTime.u.gloSystemTime, gloSystemTime);
    }
    PBLocGpsLocation *gpsLocation = pbLocation->mutable_gpslocation();
    if (NULL == gpsLocation) {
        LOC_LOGw("gpsLocation == NULL !! return!");
        return ;
    }
    uint32_t gpsLocationFlags = 0;

    if (pEPPVTRptIn->validityMask.isUtcTimestampMsValid) {
        gpsLocation->set_timestamp(pEPPVTRptIn->utcTimestampMs);
    }
    if (pEPPVTRptIn->validityMask.isLatitudeValid) {
        gpsLocation->set_latitude(pEPPVTRptIn->latitudeDeg);
    }
    if (pEPPVTRptIn->validityMask.isLongitudeValid) {
        gpsLocation->set_longitude(pEPPVTRptIn->longitudeDeg);
    }
    if (pEPPVTRptIn->validityMask.isLatitudeValid &&
            pEPPVTRptIn->validityMask.isLongitudeValid) {
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_LAT_LONG;
    }
    if (pEPPVTRptIn->validityMask.isAltitudeWrtEllipsoidValid) {
        gpsLocation->set_altitude(pEPPVTRptIn->altitudeWrtEllipsoid);
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_ALTITUDE;
    }
    if (pEPPVTRptIn->validityMask.isAltitudeWrtMeanSeaLevelValid) {
        pbLocationExtended->set_altitudemeansealevel(pEPPVTRptIn->altitudeWrtMeanSeaLevel);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL;
    }
    if (pEPPVTRptIn->validityMask.isHeadingDegValid) {
        gpsLocation->set_bearing(pEPPVTRptIn->headingDeg);
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_BEARING;
    }
    if (pEPPVTRptIn->validityMask.isHeadingUncDegValid) {
        pbLocationExtended->set_bearing_unc(pEPPVTRptIn->headingUncDeg);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_BEARING_UNC;
    }
    if (pEPPVTRptIn->validityMask.isAltitudeStdDeviationValid) {
        pbLocationExtended->set_vert_unc(pEPPVTRptIn->altitudeStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_VERT_UNC;
    }
    if (pEPPVTRptIn->validityMask.isHorizontalSpeedValid) {
        gpsLocation->set_speed(pEPPVTRptIn->horizontalSpeed);
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_SPEED;
    } else if (pEPPVTRptIn->validityMask.isNorthVelocityValid &&
                pEPPVTRptIn->validityMask.isEastVelocityValid) {
        float speed = sqrtf(
                (pEPPVTRptIn->northVelocity *
                 pEPPVTRptIn->northVelocity) +
                (pEPPVTRptIn->eastVelocity *
                 pEPPVTRptIn->eastVelocity));
        gpsLocation->set_speed(speed);
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_SPEED;
    }
    if (pEPPVTRptIn->validityMask.isHorizontalSpeedUncValid) {
        pbLocationExtended->set_speed_unc(pEPPVTRptIn->horizontalSpeedUnc);
        flagsLower32 |= GPS_LOCATION_EXTENDED_HAS_SPEED_UNC;
    } else if (pEPPVTRptIn->validityMask.isNorthVelocityStdDeviationValid &&
                pEPPVTRptIn->validityMask.isEastVelocityStdDeviationValid) {
        float speed_unc = sqrtf(
                (pEPPVTRptIn->northVelocityStdDeviation *
                 pEPPVTRptIn->northVelocityStdDeviation) +
                (pEPPVTRptIn->eastVelocityStdDeviation *
                 pEPPVTRptIn->eastVelocityStdDeviation));
        pbLocationExtended->set_speed_unc(speed_unc);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_SPEED_UNC;

    }
    if (pEPPVTRptIn->validityMask.isMagneticDeviationDegValid) {
        pbLocationExtended->set_magneticdeviation(pEPPVTRptIn->magneticDeviationDeg);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_MAG_DEV;
    }
    if (pEPPVTRptIn->validityMask.isHorizontalEllipticalUncSemiMajorAxisValid) {
        pbLocationExtended->set_horuncellipsesemimajor(pEPPVTRptIn->horUncEllipseSemiMajor);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR;
    }
    if (pEPPVTRptIn->validityMask.isHorizontalEllipticalUncSemiMinorAxisValid) {
        pbLocationExtended->set_horuncellipsesemiminor(pEPPVTRptIn->horUncEllipseSemiMinor);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR;
    }
    if (pEPPVTRptIn->validityMask.isHorizontalEllipticalUncAZValid) {
        pbLocationExtended->set_horuncellipseorientazimuth(pEPPVTRptIn->horUncEllipseOrientAzDeg);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH;
    }
    if (pEPPVTRptIn->validityMask.isNorthStdDeviationValid) {
        pbLocationExtended->set_northstddeviation(pEPPVTRptIn->northStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV;
    }
    if (pEPPVTRptIn->validityMask.isEastStdDeviationValid) {
        pbLocationExtended->set_eaststddeviation(pEPPVTRptIn->eastStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV;
    }
    if (pEPPVTRptIn->validityMask.isNorthStdDeviationValid &&
            pEPPVTRptIn->validityMask.isEastStdDeviationValid) {
        float accuracy = sqrtf(
                (pEPPVTRptIn->eastStdDeviation *
                 pEPPVTRptIn->eastStdDeviation) +
                (pEPPVTRptIn->northStdDeviation *
                 pEPPVTRptIn->northStdDeviation));
        gpsLocation->set_accuracy(accuracy);
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_ACCURACY;
    } else if (pEPPVTRptIn->validityMask.isHorizontalEllipticalUncSemiMajorAxisValid &&
            pEPPVTRptIn->validityMask.isHorizontalEllipticalUncSemiMinorAxisValid) {
        float accuracy = sqrtf(
                (pEPPVTRptIn->horUncEllipseSemiMajor *
                 pEPPVTRptIn->horUncEllipseSemiMajor) +
                (pEPPVTRptIn->horUncEllipseSemiMinor *
                 pEPPVTRptIn->horUncEllipseSemiMinor));
        gpsLocation->set_accuracy(accuracy);
        gpsLocationFlags |= PB_LOC_GPS_LOCATION_HAS_ACCURACY;
    }
    if (pEPPVTRptIn->validityMask.isNorthVelocityValid) {
        pbLocationExtended->set_northvelocity(pEPPVTRptIn->northVelocity);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_NORTH_VEL;
    }
    if (pEPPVTRptIn->validityMask.isEastVelocityValid) {
        pbLocationExtended->set_eastvelocity(pEPPVTRptIn->eastVelocity);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EAST_VEL;
    }
    if (pEPPVTRptIn->validityMask.isUpVelocityValid) {
        pbLocationExtended->set_upvelocity(pEPPVTRptIn->upVelocity);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_UP_VEL;
    }
    if (pEPPVTRptIn->validityMask.isNorthVelocityStdDeviationValid) {
        pbLocationExtended->set_northvelocitystddeviation(pEPPVTRptIn->northVelocityStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC;
    }
    if (pEPPVTRptIn->validityMask.isEastVelocityStdDeviationValid) {
        pbLocationExtended->set_eastvelocitystddeviation(pEPPVTRptIn->eastVelocityStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC;
    }
    if (pEPPVTRptIn->validityMask.isUpVelocityStdDeviationValid) {
        pbLocationExtended->set_upvelocitystddeviation(pEPPVTRptIn->upVelocityStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC;
    }
    if (pEPPVTRptIn->validityMask.isClockbiasValid) {
        pbLocationExtended->set_clockbiasmeter(pEPPVTRptIn->clockbiasMeter);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS;
    }
    if (pEPPVTRptIn->validityMask.isClockBiasStdDeviationValid) {
        pbLocationExtended->set_clockbiasstddeviationmeter(pEPPVTRptIn->clockBiasStdDeviationMeter);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS_STD_DEV;
    }
    if (pEPPVTRptIn->validityMask.isClockDriftValid) {
        pbLocationExtended->set_clockdrift(pEPPVTRptIn->clockDrift);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT;
    }
    if (pEPPVTRptIn->validityMask.isClockDriftStdDeviationValid) {
        pbLocationExtended->set_clockdriftstddeviation(pEPPVTRptIn->clockDriftStdDeviation);
        flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT_STD_DEV;
    }
    PBLocExtDOP *extDOP = pbLocationExtended->mutable_extdop();
    if (nullptr != extDOP) {
        if (pEPPVTRptIn->validityMask.isPdopValid) {
            extDOP->set_pdop(pEPPVTRptIn->pDop);
            flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        }
        if (pEPPVTRptIn->validityMask.isHdopValid) {
            extDOP->set_hdop(pEPPVTRptIn->hDop);
            flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        }
        if (pEPPVTRptIn->validityMask.isVdopValid) {
            extDOP->set_vdop(pEPPVTRptIn->vDop);
            flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        }
        if (pEPPVTRptIn->validityMask.isGdopValid) {
            extDOP->set_gdop(pEPPVTRptIn->gDop);
            flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        }
        if (pEPPVTRptIn->validityMask.isTdopValid) {
            extDOP->set_tdop(pEPPVTRptIn->tDop);
            flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        }
    }
    PBGnssLocationPositionDynamics *bodyFrameData = pbLocationExtended->mutable_bodyframedata();
    uint32_t bodyFrameDataMask = 0;
    if (nullptr != bodyFrameData) {
        if (pEPPVTRptIn->validityMask.isLongAccelValid) {
            bodyFrameData->set_longaccel(pEPPVTRptIn->longAccel);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
        }
        if (pEPPVTRptIn->validityMask.isLongAccelUncValid) {
            bodyFrameData->set_longaccelunc(pEPPVTRptIn->longAccelUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
        }
        if (pEPPVTRptIn->validityMask.isLatAccelValid) {
            bodyFrameData->set_lataccel(pEPPVTRptIn->latAccel);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
        }
        if (pEPPVTRptIn->validityMask.isLatAccelUncValid) {
            bodyFrameData->set_lataccelunc(pEPPVTRptIn->latAccelUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT ;
        }
        if (pEPPVTRptIn->validityMask.isVertAccelValid) {
            bodyFrameData->set_vertaccel(pEPPVTRptIn->vertAccel);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
        }
        if (pEPPVTRptIn->validityMask.isVertAccelUncValid) {
            bodyFrameData->set_vertaccelunc(pEPPVTRptIn->vertAccelUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
        }
        if (pEPPVTRptIn->validityMask.isYawRateValid) {
            bodyFrameData->set_yawrate(pEPPVTRptIn->yawRate);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
        }
        if (pEPPVTRptIn->validityMask.isYawRateUncValid) {
            bodyFrameData->set_yawrateunc(pEPPVTRptIn->yawRateUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
        }
        if (pEPPVTRptIn->validityMask.isPitchRadValid) {
            bodyFrameData->set_pitch(pEPPVTRptIn->pitchRad);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_BIT;
        }
        if (pEPPVTRptIn->validityMask.isPitchRadUncValid) {
            bodyFrameData->set_pitchunc(pEPPVTRptIn->pitchRadUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isPitchRateValid) {
            bodyFrameData->set_pitchrate(pEPImplPVTRptIn->pitchRate);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isPitchRateUncValid) {
            bodyFrameData->set_pitchrateunc(pEPImplPVTRptIn->pitchRateUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isRollRadValid) {
            bodyFrameData->set_roll(pEPImplPVTRptIn->rollRad);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isRollUncValid) {
            bodyFrameData->set_rollunc(pEPImplPVTRptIn->rollUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isRollRateValid) {
            bodyFrameData->set_rollrate(pEPImplPVTRptIn->rollRate);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isRollRateUncValid) {
            bodyFrameData->set_rollrateunc(pEPImplPVTRptIn->rollRateUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isYawRadValid) {
            bodyFrameData->set_yaw(pEPImplPVTRptIn->yawRad);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_BIT;
        }
        if (pEPImplPVTRptIn->validMask.isYawUncValid) {
            bodyFrameData->set_yawunc(pEPImplPVTRptIn->yawUnc);
            bodyFrameDataMask |= PB_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
        }
        bodyFrameData->set_bodyframedatamask(bodyFrameDataMask);
        if (0 != bodyFrameDataMask) {
            flagsLower32 |= PB_GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA;
        }
    }
    if (pEPPVTRptIn->validityMask.isHeadingRateDegValid) {
        pbLocationExtended->set_headingratedeg(pEPPVTRptIn->headingRateDeg);
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_HEADING_RATE;
    }

    if (pEPPVTRptIn->validityMask.isReferenceStationValid) {
        pbLocationExtended->set_numvalidrefstations(pEPPVTRptIn->numValidRefStations);
        int maxCount = pbLocationExtended->referencestation_size();
        for (int cnt = 0; ((cnt < pEPPVTRptIn->numValidRefStations) &&
                           (cnt < maxCount)); cnt++) {
            pbLocationExtended->add_referencestation(pEPPVTRptIn->referenceStation[cnt]);
        }
    }
    uint32_t numOfMeasReceived = 0;
    uint16_t gnssSvIdUsed = 0;
    /* Measurement Usage Information */
    if (EP_GNSS_MAX_MEAS > pEPPVTRptIn->numOfMeasReceived) {
        numOfMeasReceived = pEPPVTRptIn->numOfMeasReceived;
    } else {
        numOfMeasReceived = EP_GNSS_MAX_MEAS;
    }
    pbLocationExtended->set_numofmeasreceived(numOfMeasReceived);
    flagsLower32 |= GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA;
    for (uint8_t idx = 0; idx < numOfMeasReceived; idx++) {
        const epMeasUsageInfo &measUsageInfo = pEPPVTRptIn->measUsageInfo[idx];
        gnssSvIdUsed = measUsageInfo.gnssSvId;
        PBGpsMeasUsageInfo *pbMeasUsageInfo = pbLocationExtended->add_measusageinfo();
        if (nullptr != pbMeasUsageInfo) {
            uint32_t pbGnssSignalType = pbGetEHubGnssSignalTypeMask(measUsageInfo.gnssSignalType);
            pbMeasUsageInfo->set_gnsssignaltype(pbGnssSignalType);
            PBGnss_LocSvSystemEnumType pbGnssConstellation =
                    pbGetEHubGnssConstellationType(measUsageInfo.gnssConstellation);
            pbMeasUsageInfo->set_gnssconstellation(pbGnssConstellation);
            pbMeasUsageInfo->set_gnsssvid(gnssSvIdUsed);
            pbMeasUsageInfo->set_glofrequency(measUsageInfo.gloFrequency);
            uint32_t pbMeasUsageStatusMask =
                    pbGetMeasUsageStatusMask(measUsageInfo.measUsageStatusMask);
            pbMeasUsageInfo->set_measusagestatusmask(
                    (PBGnssMeasUsageStatusBitMask)pbMeasUsageStatusMask);
            if (EP_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FLOAT ==
                    measUsageInfo.carrierPhaseAmbiguityType) {
                pbMeasUsageInfo->set_carrierphaseambiguitytype(
                        PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FLOAT);
            } else if (EP_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FIXED ==
                    measUsageInfo.carrierPhaseAmbiguityType) {
                pbMeasUsageInfo->set_carrierphaseambiguitytype(
                        PB_CARRIER_PHASE_AMBIGUITY_RESOLUTION_FIXED);
            }
            pbMeasUsageInfo->set_validitymask(PB_GNSS_CARRIER_PHASE_AMBIGUITY_TYPE_VALID);
            PBGnssSvUsedInPosition *gnss_sv_used_ids =
                    pbLocationExtended->mutable_gnss_sv_used_ids();
            if (nullptr != gnss_sv_used_ids) {
                if (gnssSvIdUsed <= GPS_SV_PRN_MAX) {
                    uint64_t gps_sv_used_ids_mask = 0;
                    gps_sv_used_ids_mask |= (1ULL << (gnssSvIdUsed - GPS_SV_PRN_MIN));
                    gnss_sv_used_ids->set_gps_sv_used_ids_mask(gps_sv_used_ids_mask);
                } else if ((gnssSvIdUsed >= GLO_SV_PRN_MIN) && (gnssSvIdUsed <= GLO_SV_PRN_MAX)) {
                    uint64_t glo_sv_used_ids_mask = 0;
                    glo_sv_used_ids_mask |= (1ULL << (gnssSvIdUsed - GLO_SV_PRN_MIN));
                    gnss_sv_used_ids->set_glo_sv_used_ids_mask(glo_sv_used_ids_mask);
                } else if ((gnssSvIdUsed >= BDS_SV_PRN_MIN) && (gnssSvIdUsed <= BDS_SV_PRN_MAX)) {
                    uint64_t bds_sv_used_ids_mask = 0;
                    bds_sv_used_ids_mask |= (1ULL << (gnssSvIdUsed - BDS_SV_PRN_MIN));
                    gnss_sv_used_ids->set_bds_sv_used_ids_mask(bds_sv_used_ids_mask);
                } else if ((gnssSvIdUsed >= GAL_SV_PRN_MIN) && (gnssSvIdUsed <= GAL_SV_PRN_MAX)) {
                    uint64_t gal_sv_used_ids_mask = 0;
                    gal_sv_used_ids_mask |= (1ULL << (gnssSvIdUsed - GAL_SV_PRN_MIN));
                    gnss_sv_used_ids->set_gal_sv_used_ids_mask(gal_sv_used_ids_mask);
                } else if ((gnssSvIdUsed >= QZSS_SV_PRN_MIN) && (gnssSvIdUsed <= QZSS_SV_PRN_MAX)) {
                    uint64_t qzss_sv_used_ids_mask = 0;
                    qzss_sv_used_ids_mask |= (1ULL << (gnssSvIdUsed - QZSS_SV_PRN_MIN));
                    gnss_sv_used_ids->set_qzss_sv_used_ids_mask(qzss_sv_used_ids_mask);
                }
            }
        }
    }
    if (pEPImplPVTRptIn->validMask.isCalibrationConfidenceValid) {
        pbLocationExtended->set_calibrationconfidence(pEPImplPVTRptIn->calibrationConfidence);
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_CONFIDENCE;
    }
    uint32_t calibrationStatus = 0;
    if (pEPImplPVTRptIn->validMask.isCalibrationCueValid) {
        if (pEPImplPVTRptIn->calibrationCue.isRollCalibrationNeeded) {
            calibrationStatus |= PB_DR_ROLL_CALIBRATION_NEEDED;
        }
        if (pEPImplPVTRptIn->calibrationCue.isPitchCalibrationNeeded) {
            calibrationStatus |= PB_DR_PITCH_CALIBRATION_NEEDED;
        }
        if (pEPImplPVTRptIn->calibrationCue.isYawCalibrationNeeded) {
            calibrationStatus |= PB_DR_YAW_CALIBRATION_NEEDED;
        }
        if (pEPImplPVTRptIn->calibrationCue.isOdoCalibrationNeeded) {
            calibrationStatus |= PB_DR_ODO_CALIBRATION_NEEDED;
        }
        if (pEPImplPVTRptIn->calibrationCue.isGyroCalibrationNeeded) {
            calibrationStatus |= PB_DR_GYRO_CALIBRATION_NEEDED;
        }
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
    }
    if (pEPImplPVTRptIn->calibrationCue.turnCalibrationStatus) {
        switch (pEPImplPVTRptIn->calibrationCue.turnCalibrationStatus) {
            case CALIBRATION_STATUS_LOW :
                calibrationStatus |= PB_DR_TURN_CALIBRATION_LOW;
                break;
            case CALIBRATION_STATUS_MEDIUM :
                calibrationStatus |= PB_DR_TURN_CALIBRATION_MEDIUM;
                break;
            case CALIBRATION_STATUS_HIGH :
                calibrationStatus |= PB_DR_TURN_CALIBRATION_HIGH;
                break;
        }
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
    }
    if (pEPImplPVTRptIn->calibrationCue.linearAccelerationCalibrationStatus) {
        switch (pEPImplPVTRptIn->calibrationCue.linearAccelerationCalibrationStatus) {
            case CALIBRATION_STATUS_LOW :
                calibrationStatus |= PB_DR_LINEAR_ACCEL_CALIBRATION_LOW;
                break;
            case CALIBRATION_STATUS_MEDIUM :
                calibrationStatus |= PB_DR_LINEAR_ACCEL_CALIBRATION_MEDIUM;
                break;
            case CALIBRATION_STATUS_HIGH :
                calibrationStatus |= PB_DR_LINEAR_ACCEL_CALIBRATION_HIGH;
                break;
        }
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
    }
    if (pEPImplPVTRptIn->calibrationCue.linearMotionCalibrationStatus) {
        switch (pEPImplPVTRptIn->calibrationCue.linearMotionCalibrationStatus) {
            case CALIBRATION_STATUS_LOW :
                calibrationStatus |= PB_DR_LINEAR_MOTION_CALIBRATION_LOW;
                break;
            case CALIBRATION_STATUS_MEDIUM :
                calibrationStatus |= PB_DR_LINEAR_MOTION_CALIBRATION_MEDIUM;
                break;
            case CALIBRATION_STATUS_HIGH :
                calibrationStatus |= PB_DR_LINEAR_MOTION_CALIBRATION_HIGH;
                break;
        }
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
    }
    if (pEPImplPVTRptIn->calibrationCue.staticObservationCalibrationStatus) {
        switch (pEPImplPVTRptIn->calibrationCue.staticObservationCalibrationStatus) {
            case CALIBRATION_STATUS_LOW :
                calibrationStatus |= PB_DR_STATIC_CALIBRATION_LOW;
                break;
            case CALIBRATION_STATUS_MEDIUM :
                calibrationStatus |= PB_DR_STATIC_CALIBRATION_MEDIUM;
                break;
            case CALIBRATION_STATUS_HIGH :
                calibrationStatus |= PB_DR_STATIC_CALIBRATION_HIGH;
                break;
        }
    }

    if (calibrationStatus) {
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
    }
    pbLocationExtended->set_calibrationstatus(calibrationStatus);

    uint32_t pbSolStatusMask = PB_VEHICLE_DRSOLUTION_INVALID;
    if (pEPImplPVTRptIn->solStatusMask & VEHICLE_SENSOR_SPEED_INPUT_DETECTED) {
        pbSolStatusMask |= PB_VEHICLE_SENSOR_SPEED_INPUT_DETECTED;
    }
    if (pEPImplPVTRptIn->solStatusMask & VEHICLE_SENSOR_SPEED_INPUT_USED) {
        pbSolStatusMask |= PB_VEHICLE_SENSOR_SPEED_INPUT_USED;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_UNCALIBRATED) {
        pbSolStatusMask |= PB_DRE_WARNING_UNCALIBRATED;
    }
    if (pEPImplPVTRptIn->solStatusMask & PB_DRE_WARNING_GNSS_QUALITY_INSUFFICIENT) {
        pbSolStatusMask |= PB_DRE_WARNING_GNSS_QUALITY_INSUFFICIENT;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_FERRY_DETECTED) {
        pbSolStatusMask |= PB_DRE_WARNING_FERRY_DETECTED;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_ERROR_6DOF_SENSOR_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_6DOF_SENSOR_UNAVAILABLE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_ERROR_VEHICLE_SPEED_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_VEHICLE_SPEED_UNAVAILABLE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_ERROR_GNSS_EPH_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_GNSS_EPH_UNAVAILABLE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_ERROR_GNSS_MEAS_UNAVAILABLE) {
        pbSolStatusMask |= PB_DRE_ERROR_GNSS_MEAS_UNAVAILABLE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_INIT_POSITION_INVALID) {
        pbSolStatusMask |= PB_DRE_WARNING_INIT_POSITION_INVALID;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_INIT_POSITION_UNRELIABLE) {
        pbSolStatusMask |= PB_DRE_WARNING_INIT_POSITION_UNRELIABLE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_POSITON_UNRELIABLE) {
        pbSolStatusMask |= PB_DRE_WARNING_POSITON_UNRELIABLE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_ERROR_GENERIC) {
        pbSolStatusMask |= PB_DRE_ERROR_GENERIC;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_SENSOR_TEMP_OUT_OF_RANGE) {
        pbSolStatusMask |= PB_DRE_WARNING_SENSOR_TEMP_OUT_OF_RANGE;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_USER_DYNAMICS_INSUFFICIENT) {
        pbSolStatusMask |= PB_DRE_WARNING_USER_DYNAMICS_INSUFFICIENT;
    }
    if (pEPImplPVTRptIn->solStatusMask & DRE_WARNING_FACTORY_DATA_INCONSISTENT) {
        pbSolStatusMask |= PB_DRE_WARNING_FACTORY_DATA_INCONSISTENT;
    }

    pbLocationExtended->set_solutionstatusmask(pbSolStatusMask);
    if (pEPImplPVTRptIn->validMask.isDgnssCorrectionSourceTypeValid) {
        PBLocDgnssCorrectionSourceType dgnssCorrSourceType = PB_LOC_DGNSS_CORR_SOURCE_TYPE_INVALID;
        if (EP_DGNSS_CORR_SOURCE_TYPE_RTCM == pEPImplPVTRptIn->dgnssCorrectionSourceType) {
            dgnssCorrSourceType = PB_LOC_DGNSS_CORR_SOURCE_TYPE_RTCM;
        } else if (EP_DGNSS_CORR_SOURCE_TYPE_3GPP == pEPImplPVTRptIn->dgnssCorrectionSourceType) {
            dgnssCorrSourceType = PB_LOC_DGNSS_CORR_SOURCE_TYPE_3GPP;
        }
        pbLocationExtended->set_dgnsscorrectionsourcetype(dgnssCorrSourceType);
        flagsUpper32 |= GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_TYPE;
    }
    if (pEPImplPVTRptIn->validMask.isDgnssCorrectionSourceIDValid) {
        pbLocationExtended->set_dgnsscorrectionsourceid(pEPImplPVTRptIn->dgnssCorrectionSourceID);
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CORRECTION_SOURCE_ID;
    }
    if (pEPImplPVTRptIn->validMask.isDgnssConstellationUsageValid) {
        pbLocationExtended->set_dgnssconstellationusagemask(
                pbGetEHGnssConstellationType(pEPImplPVTRptIn->dgnssConstellationUsage));
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_CONSTELLATION_USAGE;
    }
    if (pEPImplPVTRptIn->validMask.isDgnssRefStationValid) {
        pbLocationExtended->set_dgnssrefstationid(pEPImplPVTRptIn->dgnssRefStationId);
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_REF_STATION_ID;
    }
    if (pEPImplPVTRptIn->validMask.isDgnssDataAgeValid) {
        pbLocationExtended->set_dgnssdataagemsec(pEPImplPVTRptIn->dgnssDataAgeMsec);
        flagsUpper32 |= PB_GPS_LOCATION_EXTENDED_HAS_DGNSS_DATA_AGE;
    }

    gpsLocation->set_flags(gpsLocationFlags);
    uint64_t extendedFlags = (((uint64_t)flagsUpper32 << 32) | ((uint64_t)flagsLower32));
    pbLocationExtended->set_flags(extendedFlags);
}

void EpProtoMsgConverter::pbPopulateGpsEphemeris
(
    const PBGpsEphemerisResponse &gpsEphemeris,
    epGnssEphemerisReport &ePEphRpt
)
{
    uint32_t numOfEphemeris = gpsEphemeris.numofephemeris();
    ePEphRpt.ephInfo.gpsEphemeris.numOfEphemeris = numOfEphemeris;
    for (int i = 0; i < numOfEphemeris; i++) {
        const PBGpsEphemeris &ehGpsEph = gpsEphemeris.gpsephemerisdata(i);
        epGpsSatelliteEphemerisData &ePGpsEph =
                ePEphRpt.ephInfo.gpsEphemeris.gpsEphemerisData[i];
        const PBGnssEphCommon &commonEphemerisData = ehGpsEph.commonephemerisdata();
        ePGpsEph.gnssSvId = commonEphemerisData.gnsssvid();
        ePGpsEph.iode = commonEphemerisData.iode();
        ePGpsEph.aSqrt = commonEphemerisData.asqrt();
        ePGpsEph.deltaNRadPerSec = commonEphemerisData.deltan();
        ePGpsEph.m0Rad = commonEphemerisData.m0();
        ePGpsEph.eccentricity = commonEphemerisData.eccentricity();
        ePGpsEph.omegaORad = commonEphemerisData.omega0();
        ePGpsEph.inclinationAngleRad = commonEphemerisData.i0();
        ePGpsEph.omegaRad = commonEphemerisData.omega();
        ePGpsEph.omegaDotRadPerSec = commonEphemerisData.omegadot();
        ePGpsEph.iDotRadPerSec = commonEphemerisData.idot();
        ePGpsEph.cUcRad = commonEphemerisData.cuc();
        ePGpsEph.cUsRad = commonEphemerisData.cus();
        ePGpsEph.cRcMeter = commonEphemerisData.crc();
        ePGpsEph.cRsMeter = commonEphemerisData.crs();
        ePGpsEph.cIcRad = commonEphemerisData.cic();
        ePGpsEph.cIsRad = commonEphemerisData.cis();
        ePGpsEph.fullToeSec = commonEphemerisData.toe();
        ePGpsEph.fullTocSec = commonEphemerisData.toc();
        ePGpsEph.af0Sec = commonEphemerisData.af0();
        ePGpsEph.af1 = commonEphemerisData.af1();
        ePGpsEph.af2 = commonEphemerisData.af2();
        ePGpsEph.svHealthMask = ehGpsEph.signalhealth();
        ePGpsEph.urai = ehGpsEph.urai();
        ePGpsEph.codeL2 = ehGpsEph.codel2();
        ePGpsEph.L2PData = ehGpsEph.dataflagl2p();
        ePGpsEph.tgd = ehGpsEph.tgd();
        ePGpsEph.fitInterval = ehGpsEph.fitinterval();
        ePGpsEph.iodc = ehGpsEph.iodc();
    }
}

void EpProtoMsgConverter::pbPopulateGlonassEphemeris
(
    const PBGlonassEphemerisResponse &gloEphemeris,
    epGnssEphemerisReport &ePEphRpt
)
{
    uint32_t numOfEphemeris = gloEphemeris.numofephemeris();
    ePEphRpt.ephInfo.glonassEphemeris.numOfEphemeris = numOfEphemeris;
    for (int i = 0; i < numOfEphemeris; i++) {
        const PBGlonassEphemeris &ehGloEph = gloEphemeris.gloephemerisdata(i);
        epGlonassSatelliteEphemerisData &ePGloEph =
                ePEphRpt.ephInfo.glonassEphemeris.gloEphemerisData[i];
        ePGloEph.gnssSvId = ehGloEph.gnsssvid();
        ePGloEph.bnHealth = ehGloEph.bnhealth();
        ePGloEph.lnHealth = ehGloEph.lnhealth();
        ePGloEph.FT = ehGloEph.ft();
        ePGloEph.M = ehGloEph.glom();
        ePGloEph.En = ehGloEph.enage();
        ePGloEph.freqNo = ehGloEph.glofrequency();
        ePGloEph.P1 = ehGloEph.p1();
        ePGloEph.P2 = ehGloEph.p2();
        ePGloEph.deltaTau = ehGloEph.deltatau();
        ePGloEph.x = ehGloEph.position(0);
        ePGloEph.y = ehGloEph.position(1);
        ePGloEph.z = ehGloEph.position(2);
        ePGloEph.vx = ehGloEph.velocity(0);
        ePGloEph.vy = ehGloEph.velocity(1);
        ePGloEph.vz = ehGloEph.velocity(2);
        ePGloEph.lsx = ehGloEph.acceleration(0);
        ePGloEph.lsy = ehGloEph.acceleration(1);
        ePGloEph.lsz = ehGloEph.acceleration(2);
        ePGloEph.tauN = ehGloEph.taun();
        ePGloEph.gamma = ehGloEph.gamma();
        ePGloEph.fullToeSec = (uint32_t)ehGloEph.toe();
    }
}

void EpProtoMsgConverter::pbPopulateBdsEphemeris
(
    const PBBdsEphemerisResponse &bdsEphemeris,
    epGnssEphemerisReport &ePEphRpt
)
{
    uint32_t numOfEphemeris = bdsEphemeris.numofephemeris();
    ePEphRpt.ephInfo.bdsEphemeris.numOfEphemeris = numOfEphemeris;
    for (int i = 0; i < numOfEphemeris; i++) {
        const PBBdsEphemeris &ehBdsEph = bdsEphemeris.bdsephemerisdata(i);
        epBDSSatelliteEphemerisData &ePBdsEph =
                ePEphRpt.ephInfo.bdsEphemeris.bdsEphemerisData[i];
        const PBGnssEphCommon &commonEphemerisData = ehBdsEph.commonephemerisdata();

        ePBdsEph.gnssSvId = commonEphemerisData.gnsssvid();
        ePBdsEph.aode = commonEphemerisData.iode();
        ePBdsEph.aodc = ehBdsEph.aodc();
        ePBdsEph.aSqrt = commonEphemerisData.asqrt();
        ePBdsEph.deltaNRadPerSec = commonEphemerisData.deltan();
        ePBdsEph.m0Rad = commonEphemerisData.m0();
        ePBdsEph.eccentricity = commonEphemerisData.eccentricity();
        ePBdsEph.omegaORad = commonEphemerisData.omega0();
        ePBdsEph.inclinationAngleRad = commonEphemerisData.i0();
        ePBdsEph.omegaRad = commonEphemerisData.omega();
        ePBdsEph.omegaDotRadPerSec = commonEphemerisData.omegadot();
        ePBdsEph.iDotRadPerSec = commonEphemerisData.idot();
        ePBdsEph.cUcRad = commonEphemerisData.cuc();
        ePBdsEph.cUsRad = commonEphemerisData.cus();
        ePBdsEph.cRcMeter = commonEphemerisData.crc();
        ePBdsEph.cRsMeter = commonEphemerisData.crs();
        ePBdsEph.cIcRad = commonEphemerisData.cic();
        ePBdsEph.cIsRad = commonEphemerisData.cis();
        ePBdsEph.fullToeSec = commonEphemerisData.toe();
        ePBdsEph.fullTocSec = commonEphemerisData.toc();
        ePBdsEph.af0Sec = commonEphemerisData.af0();
        ePBdsEph.af1 = commonEphemerisData.af1();
        ePBdsEph.af2 = commonEphemerisData.af2();
        ePBdsEph.svHealthMask = ehBdsEph.svhealth();
        ePBdsEph.tgd1Ns = ehBdsEph.tgd1();
        ePBdsEph.urai = ehBdsEph.urai();
    }
}

void EpProtoMsgConverter::pbPopulateGalEphemeris
(
    const PBGalileoEphemerisResponse &galEphemeris,
    epGnssEphemerisReport &ePEphRpt
)
{
    uint32_t numOfEphemeris = galEphemeris.numofephemeris();
    ePEphRpt.ephInfo.galileoEphemeris.numOfEphemeris = numOfEphemeris;
    for (int i = 0; i < numOfEphemeris; i++) {
        const PBGalileoEphemeris &ehGalEph = galEphemeris.galephemerisdata(i);
        epGalileoSatelliteEphemerisData &ePGalEph =
                ePEphRpt.ephInfo.galileoEphemeris.galEphemerisData[i];
        const PBGnssEphCommon &commonEphemerisData = ehGalEph.commonephemerisdata();

        ePGalEph.gnssSvId = commonEphemerisData.gnsssvid();
        ePGalEph.iodNav = commonEphemerisData.iode();
        ePGalEph.aSqrt = commonEphemerisData.asqrt();
        ePGalEph.deltaNRadPerSec = commonEphemerisData.deltan();
        ePGalEph.m0Rad = commonEphemerisData.m0();
        ePGalEph.eccentricity = commonEphemerisData.eccentricity();
        ePGalEph.omegaORad = commonEphemerisData.omega0();
        ePGalEph.inclinationAngleRad = commonEphemerisData.i0();
        ePGalEph.omegaRad = commonEphemerisData.omega();
        ePGalEph.omegaDotRadPerSec = commonEphemerisData.omegadot();
        ePGalEph.iDotRadPerSec = commonEphemerisData.idot();
        ePGalEph.cUcRad = commonEphemerisData.cuc();
        ePGalEph.cUsRad = commonEphemerisData.cus();
        ePGalEph.cRcMeter = commonEphemerisData.crc();
        ePGalEph.cRsMeter = commonEphemerisData.crs();
        ePGalEph.cIcRad = commonEphemerisData.cic();
        ePGalEph.cIsRad = commonEphemerisData.cis();
        ePGalEph.fullToeSec = commonEphemerisData.toe();
        ePGalEph.fullTocSec = commonEphemerisData.toc();
        ePGalEph.af0Sec = commonEphemerisData.af0();
        ePGalEph.af1 = commonEphemerisData.af1();
        ePGalEph.af2 = commonEphemerisData.af2();

        switch (ehGalEph.datasourcesignal()) {
            case PB_GAL_EPH_SIGNAL_SRC_E1B_V02:
                ePGalEph.dataSource = EP_E1B;
                break;
            case PB_GAL_EPH_SIGNAL_SRC_E5A_V02:
                ePGalEph.dataSource = EP_E5A;
                break;
            case PB_GAL_EPH_SIGNAL_SRC_E5B_V02:
                ePGalEph.dataSource = EP_E5B;
        }
        ePGalEph.validityMask = 0;
        ePGalEph.sisaIndex = ehGalEph.sisindex();
        ePGalEph.bgdE1E5a = ehGalEph.bgde1e5a();
        ePGalEph.validityMask |= EP_GALILEO_EPHEMERIS_BGDE1E5A;
        ePGalEph.bgdE1E5b = ehGalEph.bgde1e5b();
        ePGalEph.validityMask |= EP_GALILEO_EPHEMERIS_BGDE1E5B;
        ePGalEph.svHealth = ehGalEph.svhealth();
    }
}

void EpProtoMsgConverter::pbPopulateQzssEphemeris
(
    const PBQzssEphemerisResponse &qzssEphemeris,
    epGnssEphemerisReport &ePEphRpt
)
{
    uint32_t numOfEphemeris = qzssEphemeris.numofephemeris();
    ePEphRpt.ephInfo.qzssEphemeris.numOfEphemeris = numOfEphemeris;

    for (int i = 0; i < numOfEphemeris; i++) {
        const PBGpsEphemeris &ehQzssEph = qzssEphemeris.qzssephemerisdata(i);
        epQzssSatelliteEphemerisData &ePQzssEph =
                ePEphRpt.ephInfo.qzssEphemeris.qzssEphemerisData[i];
        const PBGnssEphCommon &commonEphemerisData = ehQzssEph.commonephemerisdata();

        ePQzssEph.gnssSvId = commonEphemerisData.gnsssvid();
        ePQzssEph.iode = commonEphemerisData.iode();
        ePQzssEph.aSqrt = commonEphemerisData.asqrt();
        ePQzssEph.deltaNRadPerSec = commonEphemerisData.deltan();
        ePQzssEph.m0Rad = commonEphemerisData.m0();
        ePQzssEph.eccentricity = commonEphemerisData.eccentricity();
        ePQzssEph.omegaORad = commonEphemerisData.omega0();
        ePQzssEph.inclinationAngleRad = commonEphemerisData.i0();
        ePQzssEph.omegaRad = commonEphemerisData.omega();
        ePQzssEph.omegaDotRadPerSec = commonEphemerisData.omegadot();
        ePQzssEph.iDotRadPerSec = commonEphemerisData.idot();
        ePQzssEph.cUcRad = commonEphemerisData.cuc();
        ePQzssEph.cUsRad = commonEphemerisData.cus();
        ePQzssEph.cRcMeter = commonEphemerisData.crc();
        ePQzssEph.cRsMeter = commonEphemerisData.crs();
        ePQzssEph.cIcRad = commonEphemerisData.cic();
        ePQzssEph.cIsRad = commonEphemerisData.cis();
        ePQzssEph.fullToeSec = commonEphemerisData.toe();
        ePQzssEph.fullTocSec = commonEphemerisData.toc();
        ePQzssEph.af0Sec = commonEphemerisData.af0();
        ePQzssEph.af1 = commonEphemerisData.af1();
        ePQzssEph.af2 = commonEphemerisData.af2();
        ePQzssEph.svHealthMask = ehQzssEph.signalhealth();
        ePQzssEph.urai = ehQzssEph.urai();
        ePQzssEph.L2PData = ehQzssEph.dataflagl2p();
        ePQzssEph.codeL2 = ehQzssEph.codel2();
        ePQzssEph.tgd = ehQzssEph.tgd();
        ePQzssEph.fitInterval = ehQzssEph.fitinterval();
        ePQzssEph.iodc = ehQzssEph.iodc();
    }
}

void EpProtoMsgConverter::pbFillEpGnssEphemeris
(
    const EHMessageReportSvEphemeris *pEHubEphRpt,
    epGnssEphemerisReport &ePEphRpt
)
{
    if (nullptr == pEHubEphRpt) {
        LOC_LOGe("pEHubEphRpt = nullptr !! return ");
        return;
    }
    const PBGnssSvEphemerisReport& svEphemeris = pEHubEphRpt->svephemeris();
    PBGnss_LocSvSystemEnumType gnssConstellation = svEphemeris.gnssconstellation();
    switch (gnssConstellation)
    {
        case PB_GNSS_LOC_SV_SYSTEM_GPS:
        {
            ePEphRpt.gnssConstellation = EP_GNSS_CONSTELLATION_GPS;
            const PBGpsEphemerisResponse& gpsEphemeris = svEphemeris.gpsephemeris();
            pbPopulateGpsEphemeris(gpsEphemeris, ePEphRpt);
        }
            break;
        case PB_GNSS_LOC_SV_SYSTEM_GLONASS:
        {
            ePEphRpt.gnssConstellation = EP_GNSS_CONSTELLATION_GLONASS;
            const PBGlonassEphemerisResponse &glonassEphemeris = svEphemeris.glonassephemeris();
            pbPopulateGlonassEphemeris(glonassEphemeris, ePEphRpt);
        }
            break;
        case PB_GNSS_LOC_SV_SYSTEM_BDS:
        {
            ePEphRpt.gnssConstellation = EP_GNSS_CONSTELLATION_BEIDOU;
            const PBBdsEphemerisResponse &bdsEphemeris = svEphemeris.bdsephemeris();
            pbPopulateBdsEphemeris(bdsEphemeris, ePEphRpt);
        }
            break;
        case PB_GNSS_LOC_SV_SYSTEM_GALILEO:
        {
            ePEphRpt.gnssConstellation = EP_GNSS_CONSTELLATION_GALILEO;
            const PBGalileoEphemerisResponse &galileoEphemeris = svEphemeris.galileoephemeris();
            pbPopulateGalEphemeris(galileoEphemeris, ePEphRpt);
        }
            break;
        case PB_GNSS_LOC_SV_SYSTEM_QZSS:
        {
            ePEphRpt.gnssConstellation = EP_GNSS_CONSTELLATION_QZSS;
            const PBQzssEphemerisResponse &qzssEphemeris = svEphemeris.qzssephemeris();
            pbPopulateQzssEphemeris(qzssEphemeris, ePEphRpt);
        }
            break;
    }
    if (svEphemeris.issystemtimevalid()) {
        pbFillEpGnssSystemTimeInfo(svEphemeris.systemtime(),
                ePEphRpt.gpsSystemTime);
    }
}

void EpProtoMsgConverter::pbFillEpInonData
(
    const EHMessageReportKlobucharIonoModel *pEHubIonoRpt,
    epKlobucharIonoModel &ePIonoRpt
)
{
    if (nullptr == pEHubIonoRpt) {
        LOC_LOGe("pEHubIonoRpt = nullptr !! return ");
        return;
    }
    const PBGnssKlobucharIonoModel &klobucharIonoModel = pEHubIonoRpt->klobucharionomodel();
    if (klobucharIonoModel.issystemtimevalid()) {
        pbFillEpGnssSystemTimeInfo(klobucharIonoModel.systemtime(),
            ePIonoRpt.gpsSystemTime);
    }
    PBGnss_LocSvSystemEnumType gnssConstellation = klobucharIonoModel.gnssconstellation();
    if (PB_GNSS_LOC_SV_SYSTEM_GPS == gnssConstellation) {
        ePIonoRpt.ionoDataSource = EP_IONO_DATA_SOURCE_GPS;
    } else if (PB_GNSS_LOC_SV_SYSTEM_BDS == gnssConstellation) {
        ePIonoRpt.ionoDataSource = EP_IONO_DATA_SOURCE_BDS;
    } else if (PB_GNSS_LOC_SV_SYSTEM_QZSS == gnssConstellation) {
        ePIonoRpt.ionoDataSource = EP_IONO_DATA_SOURCE_QZSS;
    }
    ePIonoRpt.ionoAlpha0 = klobucharIonoModel.alpha0();
    ePIonoRpt.ionoAlpha1 = klobucharIonoModel.alpha1();
    ePIonoRpt.ionoAlpha2 = klobucharIonoModel.alpha2();
    ePIonoRpt.ionoAlpha3 = klobucharIonoModel.alpha3();
    ePIonoRpt.ionoBeta0 = klobucharIonoModel.beta0();
    ePIonoRpt.ionoBeta1 = klobucharIonoModel.beta1();
    ePIonoRpt.ionoBeta2 = klobucharIonoModel.beta2();
    ePIonoRpt.ionoBeta3 = klobucharIonoModel.beta3();
}

void EpProtoMsgConverter::pbFillEpGloAddParamsData
(
    const EHMessageReportGlonassAdditionalParams *pEHubAddParamsRpt,
    epGloAdditionalParameter &ePAddParamsRpt
)
{
    if (nullptr == pEHubAddParamsRpt) {
        LOC_LOGe("pEHubAddParamsRpt = nullptr !! return ");
        return;
    }
    const PBGnssAdditionalSystemInfo& additionalSystemInfo =
            pEHubAddParamsRpt->additionalsysteminfo();
    if (additionalSystemInfo.issystemtimevalid()) {
        pbFillEpGnssSystemTimeInfo(additionalSystemInfo.systemtime(),
            ePAddParamsRpt.gpsSystemTime);
    }
    uint32_t validityMask = additionalSystemInfo.validitymask();
    if (PB_GNSS_ADDITIONAL_SYSTEMINFO_HAS_TAUC & validityMask) {
        ePAddParamsRpt.tauCSec = additionalSystemInfo.tauc();
        ePAddParamsRpt.validityMask.isTauCSecValid = 1;
    }
    if (PB_GNSS_ADDITIONAL_SYSTEMINFO_HAS_LEAP_SEC & validityMask) {
        ePAddParamsRpt.leapSec = additionalSystemInfo.leapsec();
        ePAddParamsRpt.validityMask.isLeapSecValid = 1;
    }
}

#ifdef FEATURE_CDFW
void EpProtoMsgConverter::fillQdgnssStatusIndicator
(
    const PBStationStatusIndicatorMask pbStatusindicationMask,
    StationStatusIndicatorMask  &statusIndicator
)
{
    statusIndicator.stationIndicator = pbStatusindicationMask.stationindicator();
    statusIndicator.oscillatorIndicator = pbStatusindicationMask.oscillatorindicator();
    statusIndicator.quarterCycleIndicator = pbStatusindicationMask.quartercycleindicator();
    statusIndicator.heightAvailability = pbStatusindicationMask.heightavailability();
    statusIndicator.referencePointUncertainty = pbStatusindicationMask.referencepointuncertainty();
}

void EpProtoMsgConverter::fillQdgnssConstellationBitMask
(
    const uint32_t constellationBitMask,
    DGnssConstellationBitMask &qdGnssConstellationBitMask
)
{
    qdGnssConstellationBitMask = 0;
    if (constellationBitMask & PB_DGNSS_CONSTELLATION_GPS_BIT) {
        qdGnssConstellationBitMask |= DGNSS_CONSTELLATION_GPS_BIT;
    }
    if (constellationBitMask & PB_DGNSS_CONSTELLATION_SBAS_BIT) {
        qdGnssConstellationBitMask |= DGNSS_CONSTELLATION_SBAS_BIT;
    }
    if (constellationBitMask & PB_DGNSS_CONSTELLATION_QZSS_BIT) {
        qdGnssConstellationBitMask |= DGNSS_CONSTELLATION_QZSS_BIT;
    }
    if (constellationBitMask & PB_DGNSS_CONSTELLATION_GALILEO_BIT) {
        qdGnssConstellationBitMask |= DGNSS_CONSTELLATION_GALILEO_BIT;
    }
    if (constellationBitMask & PB_DGNSS_CONSTELLATION_GLONASS_BIT) {
        qdGnssConstellationBitMask |= DGNSS_CONSTELLATION_GLONASS_BIT;
    }
    if (constellationBitMask & PB_DGNSS_CONSTELLATION_BDS_BIT) {
        qdGnssConstellationBitMask |= DGNSS_CONSTELLATION_BDS_BIT;
    }
}

void EpProtoMsgConverter::pbFillQDgnssReferenceStationData
(
    const EHMessageReportCdfwQDgnssReferenceStation *pEHubQDgnssRefStationInfo,
    ReferenceStation &qDgnssRefStation
)
{
    if (nullptr == pEHubQDgnssRefStationInfo) {
        LOC_LOGe("pEHubQDgnssRefStationInfo = nullptr !! return ");
        return;
    }
    const PBReferenceStation &pbRefStation = pEHubQDgnssRefStationInfo->qdgnssrefstation();
    qDgnssRefStation.stationID = pbRefStation.stationid();
    fillQdgnssStatusIndicator(pbRefStation.statusindicator(), qDgnssRefStation.statusIndicator);
    qDgnssRefStation.linkedStationID = pbRefStation.linkedstationid();
    qDgnssRefStation.ITRFYear = pbRefStation.itrfyear();
    fillQdgnssConstellationBitMask(pbRefStation.constellationbitmask(),
            qDgnssRefStation.constellationBitMask);
    qDgnssRefStation.ecefX = pbRefStation.ecefx();
    qDgnssRefStation.ecefY = pbRefStation.ecefy();
    qDgnssRefStation.ecefZ = pbRefStation.ecefz();
    qDgnssRefStation.antennaHeight = pbRefStation.antennaheight();
    qDgnssRefStation.uncertaintyX = pbRefStation.uncertaintyx();
    qDgnssRefStation.uncertaintyY = pbRefStation.uncertaintyy();
    qDgnssRefStation.uncertaintyZ = pbRefStation.uncertaintyz();
    qDgnssRefStation.uncertaintyConfidenceX = pbRefStation.uncertaintyconfidencex();
    qDgnssRefStation.uncertaintyConfidenceY = pbRefStation.uncertaintyconfidencey();
    qDgnssRefStation.uncertaintyConfidenceZ = pbRefStation.uncertaintyconfidencez();
}

void EpProtoMsgConverter::fillQDGnssConstellationIdentifier
(
    const PBDGnssConstellationIdentifier &pbConstellationIdentifier,
    DGnssConstellationIdentifier &qdgnssConstellationIdentifier
)
{
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_GPS == pbConstellationIdentifier) {
        qdgnssConstellationIdentifier = DGNSS_CONSTELLATION_IDENTIFIER_GPS;
    }
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_SBAS == pbConstellationIdentifier) {
        qdgnssConstellationIdentifier = DGNSS_CONSTELLATION_IDENTIFIER_SBAS;
    }
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_QZSS == pbConstellationIdentifier) {
        qdgnssConstellationIdentifier = DGNSS_CONSTELLATION_IDENTIFIER_QZSS;
    }
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_GALILEO  == pbConstellationIdentifier) {
        qdgnssConstellationIdentifier = DGNSS_CONSTELLATION_IDENTIFIER_GALILEO ;
    }
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_GLONASS  == pbConstellationIdentifier) {
        qdgnssConstellationIdentifier = DGNSS_CONSTELLATION_IDENTIFIER_GLONASS ;
    }
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_BDS == pbConstellationIdentifier) {
        qdgnssConstellationIdentifier = DGNSS_CONSTELLATION_IDENTIFIER_BDS;
    }
}

void EpProtoMsgConverter::fillQDGnssEpochTime
(
    const PBGnssEpochTime &pbEpochTime,
    const PBDGnssConstellationIdentifier &pbConstellationIdentifier,
    GnssEpochTime &qdgnssEpochTime
)
{
    if (PB_DGNSS_CONSTELLATION_IDENTIFIER_GLONASS  == pbConstellationIdentifier) {
        qdgnssEpochTime.glonassEpochTime.timeOfDay = pbEpochTime.gloepochtime().timeofday();
        qdgnssEpochTime.glonassEpochTime.dayOfWeek = pbEpochTime.gloepochtime().dayofweek();
    } else {
        qdgnssEpochTime.timeOfWeek = pbEpochTime.timeofweek();
    }
}

void EpProtoMsgConverter::fillQDGnssObservationStatusFlags
(
    const PBDGnssObservationStatusFlags &pbStatusFlags,
    DGnssObservationStatusFlags &qdgnssStatusFlags
)
{
    qdgnssStatusFlags.synchronousFlag = pbStatusFlags.synchronousflag();
    qdgnssStatusFlags.smoothingIndicator = pbStatusFlags.smoothingindicator();
    qdgnssStatusFlags.smoothingInterval = pbStatusFlags.smoothinginterval();
    qdgnssStatusFlags.clockSteeringIndicator = pbStatusFlags.clocksteeringindicator();
    qdgnssStatusFlags.externalClockIndicator = pbStatusFlags.externalclockindicator();
}

void EpProtoMsgConverter::fillQdgnssExtendedSatelliteInformation
(
    const PBExtendedSatelliteInformation &pbExtendedSatelliteInformation,
    ExtendedSatelliteInformation &qdgnssExtendedSatelliteInformation
)
{
    switch (pbExtendedSatelliteInformation) {
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_7:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_7;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_6:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_6;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_5:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_5;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_4:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_4;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_3:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_3;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_2:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_2;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_1:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_MINUS_1;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_0;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_1:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_1;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_2:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_2;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_3:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_3;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_4:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_4;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_5:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_5;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_6:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_NUMBER_6;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_RESERVED:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_RESERVED;
        break;
        case PB_DGNSS_GLONASS_FREQUENCY_CHANNEL_UNKNOWN:
            qdgnssExtendedSatelliteInformation = DGNSS_GLONASS_FREQUENCY_CHANNEL_UNKNOWN;
        break;
    }
}

void EpProtoMsgConverter::fillQDgnssGpsSignalIdentifier
(
    const PBDGnssGpsSignalIdentifier &pbGpsSignalIdentifier,
    DGnssGpsSignalIdentifier &qdgnssGpsSignalIdentifier
)
{
    switch (pbGpsSignalIdentifier) {
        case PB_DGNSS_SIGNAL_GPS_L1CA:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1CA;
        break;
        case PB_DGNSS_SIGNAL_GPS_L1P:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1P;
        break;
        case PB_DGNSS_SIGNAL_GPS_L1Z:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1Z;
        break;
        case PB_DGNSS_SIGNAL_GPS_L1N:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1N;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2CA:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2CA;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2P:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2P;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2Z:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2Z;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2CM:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2CM;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2CL:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2CL;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2CML:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2CML;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2D:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2D;
        break;
        case PB_DGNSS_SIGNAL_GPS_L2N:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L2N;
        break;
        case PB_DGNSS_SIGNAL_GPS_L5I:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L5I;
        break;
        case PB_DGNSS_SIGNAL_GPS_L5Q:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L5Q;
        break;
        case PB_DGNSS_SIGNAL_GPS_L5IQ:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L5IQ;
        break;
        case PB_DGNSS_SIGNAL_GPS_L1CD:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1CD;
        break;
        case PB_DGNSS_SIGNAL_GPS_L1CP:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1CP;
        break;
        case PB_DGNSS_SIGNAL_GPS_L1CDP:
            qdgnssGpsSignalIdentifier = DGNSS_SIGNAL_GPS_L1CDP;
        break;
    }
}

void EpProtoMsgConverter::fillQDgnssSbasSignalIdentifier
(
    const PBDGnssSbasSignalIdentifier &pbSbasSignalIdentifier,
    DGnssSbasSignalIdentifier &qdgnssSbasSignalIdentifier
)
{
    switch (pbSbasSignalIdentifier) {
        case PB_DGNSS_SIGNAL_SBAS_L1CA:
            qdgnssSbasSignalIdentifier = DGNSS_SIGNAL_SBAS_L1CA;
        break;
        case PB_DGNSS_SIGNAL_SBAS_L5I:
            qdgnssSbasSignalIdentifier = DGNSS_SIGNAL_SBAS_L5I;
        break;
        case PB_DGNSS_SIGNAL_SBAS_L5Q:
            qdgnssSbasSignalIdentifier = DGNSS_SIGNAL_SBAS_L5Q;
        break;
        case PB_DGNSS_SIGNAL_SBAS_L5IQ:
            qdgnssSbasSignalIdentifier = DGNSS_SIGNAL_SBAS_L5IQ;
        break;
    }
}

void EpProtoMsgConverter::fillQDgnssQzssSignalIdentifier
(
    const PBDGnssQzssSignalIdentifier &pbQzssSignalIdentifier,
    DGnssQzssSignalIdentifier &qdgnssQzssSignalIdentifier
)
{
    switch (pbQzssSignalIdentifier) {
        case PB_DGNSS_SIGNAL_QZSS_L1CA:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L1CA;
        break;
        case PB_DGNSS_SIGNAL_QZSS_LEXS:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_LEXS;
        break;
        case PB_DGNSS_SIGNAL_QZSS_LEXL:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_LEXL;
        break;
        case PB_DGNSS_SIGNAL_QZSS_LEXSL:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_LEXSL;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L2CM:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L2CM;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L2CL:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L2CL;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L2CML:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L2CML;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L5I:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L5I;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L5Q:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L5Q;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L5IQ:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L5IQ;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L1CD:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L1CD;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L1CP:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L1CP;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L1CDP:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L1CDP;
        break;
        case PB_DGNSS_SIGNAL_QZSS_L1S:
            qdgnssQzssSignalIdentifier = DGNSS_SIGNAL_QZSS_L1S;
        break;
    }
}

void EpProtoMsgConverter::fillQDgnssGalileoSignalIdentifier
(
    const PBDGnssGalileoSignalIdentifier &pbGalileoSignalIdentifier,
    DGnssGalileoSignalIdentifier &qdgnssGalieloSignalIdentifier
)
{
    switch (pbGalileoSignalIdentifier) {
        case PB_DGNSS_SIGNAL_GALILEO_E1C:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E1C;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E1A:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E1A;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E1B:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E1B;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E1BC:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E1BC;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E1ABC:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E1ABC;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E6C:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E6C;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E6A:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E6A;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E6B:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E6B;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E6BC:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E6BC;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E6ABC:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E6ABC;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5BI:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5BI;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5BQ:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5BQ;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5BIQ:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5BIQ;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5ABI:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5ABI;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5ABQ:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5ABQ;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5ABIQ:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5ABIQ;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5AI:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5AI;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5AQ:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5AQ;
        break;
        case PB_DGNSS_SIGNAL_GALILEO_E5AIQ:
            qdgnssGalieloSignalIdentifier = DGNSS_SIGNAL_GALILEO_E5AIQ;
        break;
    }
}

void EpProtoMsgConverter::fillQDgnssGlonassSignalIdentifier
(
    const PBDGnssGlonassSignalIdentifier &pbGlonassSignalIdentifier,
    DGnssGlonassSignalIdentifier &qdgnssGlonassSignalIdentifier
)
{
    switch (pbGlonassSignalIdentifier) {
        case PB_DGNSS_SIGNAL_GLONASS_G1CA:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G1CA;
        break;
        case PB_DGNSS_SIGNAL_GLONASS_G1P:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G1P;
        break;
        case PB_DGNSS_SIGNAL_GLONASS_G2CA:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G2CA;
        break;
        case PB_DGNSS_SIGNAL_GLONASS_G2P:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G2P;
        break;
        case PB_DGNSS_SIGNAL_GLONASS_G3I:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G3I;
        break;
        case PB_DGNSS_SIGNAL_GLONASS_G3Q:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G3Q;
        break;
        case PB_DGNSS_SIGNAL_GLONASS_G3IQ:
            qdgnssGlonassSignalIdentifier = DGNSS_SIGNAL_GLONASS_G3IQ;
        break;
    }
}

void EpProtoMsgConverter::fillQDgnssBdsSignalIdentifier
(
    const PBDGnssBdsSignalIdentifier &pbBdsSignalIdentifier,
    DGnssBdsSignalIdentifier &qdgnssBdsSignalIdentifier
)
{
    switch (pbBdsSignalIdentifier) {
        case PB_DGNSS_SIGNAL_BDS_B12I:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B12I;
        break;
        case PB_DGNSS_SIGNAL_BDS_B12Q:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B12Q;
        break;
        case PB_DGNSS_SIGNAL_BDS_B12IQ:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B12IQ;
        break;
        case PB_DGNSS_SIGNAL_BDS_B3I:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B3I;
        break;
        case PB_DGNSS_SIGNAL_BDS_B3Q:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B3Q;
        break;
        case PB_DGNSS_SIGNAL_BDS_B3IQ:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B3IQ;
        break;
        case PB_DGNSS_SIGNAL_BDS_B3A:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B3A;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2I:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2I;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2Q:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2Q;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2IQ:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2IQ;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2AD:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2AD;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2AP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2AP;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2ADP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2ADP;
        break;
        case PB_DGNSS_SIGNAL_BDS_B1CD:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B1CD;
        break;
        case PB_DGNSS_SIGNAL_BDS_B1CP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B1CP;
        break;
        case PB_DGNSS_SIGNAL_BDS_B1CDP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B1CDP;
        break;
        case PB_DGNSS_SIGNAL_BDS_B1A:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B1A;
        break;
        case PB_DGNSS_SIGNAL_BDS_B1N:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B1N;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2BD:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2BD;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2BP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2BP;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2BDP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2BDP;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2D:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2D;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2P:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2P;
        break;
        case PB_DGNSS_SIGNAL_BDS_B2DP:
            qdgnssBdsSignalIdentifier = DGNSS_SIGNAL_BDS_B2DP;
        break;
    }
}

void EpProtoMsgConverter::fillQDGnssSignalObservationStatus
(
    const uint32_t &pbSignalObservationStatus,
    DGnssSignalStatusFlags &qdgnssSignalObservationStatus
)
{
    if (pbSignalObservationStatus & PB_MEASUREMENT_COMPLETE) {
        qdgnssSignalObservationStatus |= MEASUREMENT_COMPLETE;
    }
    if (pbSignalObservationStatus & PB_PSEUDORANGE_VALID) {
        qdgnssSignalObservationStatus |= PSEUDORANGE_VALID;
    }
    if (pbSignalObservationStatus & PB_CARRIER_PHASE_VALID) {
        qdgnssSignalObservationStatus |= CARRIER_PHASE_VALID;
    }
    if (pbSignalObservationStatus & PB_PHASE_RANGE_RATE_VALID) {
        qdgnssSignalObservationStatus |= PHASE_RANGE_RATE_VALID;
    }
    if (pbSignalObservationStatus & PB_HALF_CYCLE_AMBIGUITY_VALID) {
        qdgnssSignalObservationStatus |= HALF_CYCLE_AMBIGUITY_VALID;
    }
    if (pbSignalObservationStatus & PB_HALF_CYCLE_AMBIGUITY_UNKNOWN) {
        qdgnssSignalObservationStatus |= HALF_CYCLE_AMBIGUITY_UNKNOWN;
    }
}

void EpProtoMsgConverter::fillQDGnssMeasurementInfo
(
    const PBDGnssMeasurement &pbDGnssMeasurement,
    const PBDGnssConstellationIdentifier &gnssConstId,
    DGnssMeasurement &qDGnssMeasurement
)
{
    qDGnssMeasurement.svId = pbDGnssMeasurement.svid();
    fillQdgnssExtendedSatelliteInformation(
            pbDGnssMeasurement.extendedsatelliteinformation(),
            qDGnssMeasurement.extendedSatelliteInformation);

    switch (gnssConstId) {
        case PB_DGNSS_CONSTELLATION_IDENTIFIER_GPS:
        {
            const PBDGnssGpsSignalIdentifier &gpsSignalIdentifier =
                    pbDGnssMeasurement.gpssignalidentifier();
            fillQDgnssGpsSignalIdentifier(gpsSignalIdentifier,
                    qDGnssMeasurement.gnssSignalIdentifier.gpsSignalIdentifier);
        }
        break;
        case PB_DGNSS_CONSTELLATION_IDENTIFIER_SBAS:
        {
            const PBDGnssSbasSignalIdentifier &sbasSignalIdentifier =
                    pbDGnssMeasurement.sbassignalidentifier();
            fillQDgnssSbasSignalIdentifier(sbasSignalIdentifier,
                    qDGnssMeasurement.gnssSignalIdentifier.sbasSignalIdentifier);
        }
        break;
        case PB_DGNSS_CONSTELLATION_IDENTIFIER_QZSS:
        {
            const PBDGnssQzssSignalIdentifier &qzssSignalIdentifier =
                    pbDGnssMeasurement.qzsssignalidentifier();
            fillQDgnssQzssSignalIdentifier(qzssSignalIdentifier,
                    qDGnssMeasurement.gnssSignalIdentifier.qzssSignalIdentifier);
        }
        break;
        case PB_DGNSS_CONSTELLATION_IDENTIFIER_GALILEO:
        {
            const PBDGnssGalileoSignalIdentifier &galileoSignalIdentifier =
                    pbDGnssMeasurement.galileosignalidentifier();
            fillQDgnssGalileoSignalIdentifier(galileoSignalIdentifier,
                    qDGnssMeasurement.gnssSignalIdentifier.galileoSignalIdentifier);
        }
        break;
        case PB_DGNSS_CONSTELLATION_IDENTIFIER_GLONASS:
        {
            const PBDGnssGlonassSignalIdentifier &glonassSignalIdentifier =
                    pbDGnssMeasurement.glonasssignalidentifier();
            fillQDgnssGlonassSignalIdentifier(glonassSignalIdentifier,
                    qDGnssMeasurement.gnssSignalIdentifier.glonassSignalIdentifier);
        }
        break;
        case PB_DGNSS_CONSTELLATION_IDENTIFIER_BDS:
        {
            const PBDGnssBdsSignalIdentifier &bdsSignalIdentifier =
                    pbDGnssMeasurement.bdssignalidentifier();
            fillQDgnssBdsSignalIdentifier(bdsSignalIdentifier,
                    qDGnssMeasurement.gnssSignalIdentifier.bdsSignalIdentifier);
        }
        break;
    }
    fillQDGnssSignalObservationStatus(pbDGnssMeasurement.signalobservationstatus(),
            qDGnssMeasurement.signalObservationStatus);
    qDGnssMeasurement.pseudorange = pbDGnssMeasurement.pseudorange();
    qDGnssMeasurement.phaseRange = pbDGnssMeasurement.phaserange();
    qDGnssMeasurement.phaseRangeRate = pbDGnssMeasurement.phaserangerate();
    qDGnssMeasurement.phaseRangeLockTimeIndicator =
            pbDGnssMeasurement.phaserangelocktimeindicator();
    qDGnssMeasurement.signalCNR = pbDGnssMeasurement.signalcnr();
}

void EpProtoMsgConverter::pbFillQDgnssObservation
(
    const EHMessageReportCdfwQDgnssObservation *pEHubQDgnssObserInfo,
    DGnssObservation &qDgnssObserv
)
{
    if (nullptr == pEHubQDgnssObserInfo) {
        LOC_LOGe("pEHubQDgnssObserInfo = nullptr !! return ");
        return;
    }
    const PBDGnssObservation &pbQDgnssObserv = pEHubQDgnssObserInfo->qdgnssobserv();
    qDgnssObserv.stationID = pbQDgnssObserv.stationid();
    fillQDGnssEpochTime(pbQDgnssObserv.epochtime(),
            pbQDgnssObserv.constellationidentifier(),
            qDgnssObserv.epochTime);
    fillQDGnssObservationStatusFlags(pbQDgnssObserv.statusflags(), qDgnssObserv.statusFlags);
    qDgnssObserv.issueOfDataStation = pbQDgnssObserv.issueofdatastation();
    fillQDGnssConstellationIdentifier(pbQDgnssObserv.constellationidentifier(),
            qDgnssObserv.constellationIdentifier);
    qDgnssObserv.numberOfMeasurements = pbQDgnssObserv.numberofmeasurements();
    if (qDgnssObserv.numberOfMeasurements > DGNSS_MEASUREMENT_MAX) {
        qDgnssObserv.numberOfMeasurements = DGNSS_MEASUREMENT_MAX;
    }
    for (uint8_t i = 0; i < qDgnssObserv.numberOfMeasurements; i++) {
        const PBDGnssMeasurement &pbDGnssMeasurement = pbQDgnssObserv.dgnssmeasurement(i);
        fillQDGnssMeasurementInfo(pbDGnssMeasurement, pbQDgnssObserv.constellationidentifier(),
                qDgnssObserv.dGnssMeasurement[i]);
    }
}
#endif //FEATURE_CDFW

void EpProtoMsgConverter::pbFillEpLeverArmParams
(
    const PBLeverArmParams &eHubLeverArmParams,
    epLeverArmParams &ePLeverArmParams
)
{
    ePLeverArmParams.forwardOffsetMeters = eHubLeverArmParams.forwardoffsetmeters();
    ePLeverArmParams.sidewaysOffsetMeters = eHubLeverArmParams.sidewaysoffsetmeters();
    ePLeverArmParams.upOffsetMeters = eHubLeverArmParams.upoffsetmeters();
}

void EpProtoMsgConverter::pbFillEpLeverArmConfData
(
    const EHMessageStatusLeverArmConfig *pEHubLeverArmConfRpt,
    epLeverArmConfigInfo &ePLeverArmConfigData
)
{
    if (nullptr == pEHubLeverArmConfRpt) {
        LOC_LOGe("pEHubLeverArmConfRpt = nullptr !! return ");
        return;
    }
    const PBLeverArmConfigInfo &leverArmInfo = pEHubLeverArmConfRpt->leverarminfo();
    uint32_t leverArmValidMask = leverArmInfo.leverarmvalidmask();
    if (leverArmValidMask & PB_LEVER_ARM_TYPE_GNSS_TO_VRP_BIT) {
        ePLeverArmConfigData.leverArmValidMask |= EP_LEVER_ARM_TYPE_GNSS_TO_VRP_BIT;
        pbFillEpLeverArmParams(leverArmInfo.gnsstovrp(),
                ePLeverArmConfigData.gnssToVRP);
    }
    if (leverArmValidMask & PB_LEVER_ARM_TYPE_DR_IMU_TO_GNSS_BIT) {
        ePLeverArmConfigData.leverArmValidMask |= EP_LEVER_ARM_TYPE_DR_IMU_TO_GNSS_BIT;
        pbFillEpLeverArmParams(leverArmInfo.drimutognss(),
                ePLeverArmConfigData.drImuToGnss);
    }
    if (leverArmValidMask & PB_LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS_BIT) {
        ePLeverArmConfigData.leverArmValidMask |= EP_LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS_BIT;
        pbFillEpLeverArmParams(leverArmInfo.veppimutognss(),
                ePLeverArmConfigData.veppImuToGnss);
    }
}

int EpProtoMsgConverter::pbPopulateQwesInstallLicenseReqWithCtr
(
    const uint8_t *licenseBuf,
    uint32_t licenseLen,
    const string &epUniqueName,
    uint32_t sendMsgCounter,
    string &os
) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REQUEST_INSTALL_LICENSE);

    EHMessageQWESInstallLicenseReq ehMsgLicenseInstallReq;
    std::string licenseBufStr(licenseBuf, licenseBuf + licenseLen);

    ehMsgLicenseInstallReq.set_licensebuffer(licenseBufStr);
    ehMsgLicenseInstallReq.set_licenselength(licenseLen);

    string ehSubMsg;
    bool bRetVal = ehMsgLicenseInstallReq.SerializeToString(&ehSubMsg);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgLicenseInstallReq failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    string ehMsgPayload;
    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    if (!ehMsg.SerializeToString(&ehMsgPayload)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    ehMsgWithCtr.set_ehmsgpayload(ehMsgPayload);
    ehMsgWithCtr.set_msgsendername(epUniqueName);
    ehMsgWithCtr.set_msgcounter(sendMsgCounter);
    if (!ehMsgWithCtr.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsgWithCtr failed!");
        return 1;
    }
    return 0;
}

int EpProtoMsgConverter::pbPopulateAttestationStatementReqWithCtr
(
    const uint8_t *nonce,
    uint32_t nonceLen,
    const uint8_t *data,
    uint32_t dataLen,
    const string &epUniqueName,
    uint32_t sendMsgCounter,
    string &os
) {
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REQUEST_ATTESTATION_STATEMENT);

    EHMessageQWESReqAttestationStatement ehMsgAttestation;
    std::string nonceBuf(nonce, nonce + nonceLen);
    std::string dataBuf(data, data + dataLen);

    ehMsgAttestation.set_nonce(nonceBuf);
    ehMsgAttestation.set_noncelength(nonceLen);
    ehMsgAttestation.set_data(dataBuf);
    ehMsgAttestation.set_datalength(dataLen);

    string ehSubMsg;
    bool bRetVal = ehMsgAttestation.SerializeToString(&ehSubMsg);
    if (!bRetVal) {
        LOC_LOGe("SerializeToString on ehMsgAttestation failed!");
        return 1;
    }
    ehMsg.set_payload(ehSubMsg);

    string ehMsgPayload;
    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    if (!ehMsg.SerializeToString(&ehMsgPayload)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return 1;
    }

    ehMsgWithCtr.set_ehmsgpayload(ehMsgPayload);
    ehMsgWithCtr.set_msgsendername(epUniqueName);
    ehMsgWithCtr.set_msgcounter(sendMsgCounter);
    if (!ehMsgWithCtr.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsgWithCtr failed!");
        return 1;
    }
    return 0;
}

uint32_t pbGetEhubConstellationTypeMask
(
   epSubscriptionModifier epSub
)
{
    uint32_t eHubMask = (GnssConstellationTypeMask)0;

    if (epSub.constellationType & EP_GNSS_CONSTELLATION_GPS) {
        eHubMask |= PB_GNSS_CONSTELLATION_TYPE_GPS;
    }

    if (epSub.constellationType & EP_GNSS_CONSTELLATION_GALILEO) {
        eHubMask |= PB_GNSS_CONSTELLATION_TYPE_GALILEO;
    }

    if (epSub.constellationType & EP_GNSS_CONSTELLATION_SBAS) {
        eHubMask |= PB_GNSS_CONSTELLATION_TYPE_SBAS;
    }

    if (epSub.constellationType & EP_GNSS_CONSTELLATION_GLONASS) {
        eHubMask |= PB_GNSS_CONSTELLATION_TYPE_GLONASS;
    }

    if (epSub.constellationType & EP_GNSS_CONSTELLATION_BEIDOU) {
        eHubMask |= PB_GNSS_CONSTELLATION_TYPE_BEIDOU;
    }

    if (epSub.constellationType & EP_GNSS_CONSTELLATION_QZSS) {
        eHubMask |= PB_GNSS_CONSTELLATION_TYPE_QZSS;
    }

    return eHubMask;
}

uint32_t EpProtoMsgConverter::pbGetGnssSubMask
(
    const epReportSubscriptionMask &epSubInfoFlags
)
{
    uint32_t subMask = 0;
    if (epSubInfoFlags & EP_SUBSCRIPTION_PVT_1HZ_REPORT) {
        subMask |= EH_REG_REPORT_MASK_POSITION;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_PVT_NHZ_REPORT) {
        subMask |= EH_REG_REPORT_MASK_NHZ_POSITION;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_UNPROPAGATED_PVT_1HZ_REPORT) {
        subMask |= EH_REG_REPORT_MASK_UNPROPAGATED_POSITION;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_MEASUREMENT_1HZ_REPORT) {
        subMask |= EH_REG_REPORT_MASK_SV_MEAS;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_MEASUREMENT_NHZ_REPORT) {
        subMask |= EH_REG_REPORT_MASK_NHZ_SV_MEAS;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_EPHEMERIS_REPORT) {
        subMask |= EH_REG_REPORT_MASK_SV_EPHEMERIS;
        /* Glo additional paramters will be reported only when engine subscribes
         * for Ephemeris
         */
        subMask |= EH_REG_REPORT_MASK_GLONASS_ADDITIONAL_PARAMS;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_POLYNOMIAL_REPORT) {
        subMask |= EH_REG_REPORT_MASK_SV_POLYNOMIAL;
    }
    if (epSubInfoFlags & EP_SUBSCRIPTION_IONO_MODEL_REPORT) {
        subMask |= EH_REG_REPORT_MASK_KLOBUCHARIONO_MODEL;
    }
    return subMask;
}

bool EpProtoMsgConverter::pbRegisterWithEHub
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
)
{
    bool retval = true;

    EHRoutingTableEntry pbRoutingTableEntry;
    EHEngineConnectionInfo *pbEngConnectInfo = pbRoutingTableEntry.mutable_engconnectinfo();
    EHSubModifiers *pbSubModifiers = pbRoutingTableEntry.mutable_submodifiers();
    bool regSessionMsgs = false;

    LOC_LOGi("--> ");

    uint32_t gnssRegReportMask = 0, ppeRegReportMask = 0;

    pbRoutingTableEntry.set_featureid(featureId);
    if (E_HUB_TECH_PPE == techID) {
        gnssRegReportMask = pbGetGnssSubMask(epSubInfo.flags);
        pbRoutingTableEntry.set_regstatusmask(EH_REG_STATUS_MASK_CONNECTIVITY);
        pbRoutingTableEntry.set_regcdfwcorrnmask(EH_REG_REPORT_MASK_CDFW_QDGNSS_UNKNOWN);

    } else if (E_HUB_TECH_DR == techID) {
        gnssRegReportMask = pbGetGnssSubMask(epSubInfo.flags);
        ppeRegReportMask = EH_REG_REPORT_MASK_POSITION;
        pbRoutingTableEntry.set_regstatusmask(EH_REG_STATUS_MASK_LEVER_ARM_CONFIG |
                EH_REG_STATUS_MASK_BODY_TO_SENSOR_MOUNT_PARAMS);
        pbRoutingTableEntry.set_regcdfwcorrnmask(EH_REG_REPORT_MASK_CDFW_QDGNSS_UNKNOWN);
    } else if (E_HUB_TECH_VPE == techID) {
        gnssRegReportMask = pbGetGnssSubMask(epSubInfo.flags);
        if (epSubInfo.flags & EP_SUBSCRIPTION_CORRECTION_DATA) {
            pbRoutingTableEntry.set_regcdfwcorrnmask(EH_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION |
                    EH_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION);
        }
    }
    /*Idex 0 - GNSS/SPE subscription */
    pbRoutingTableEntry.add_regreportmask(gnssRegReportMask);
    /*Idex 1 - PPE subscription , nothing*/
    pbRoutingTableEntry.add_regreportmask(ppeRegReportMask);
    /*Idex 2 - DRE subscription, nothing */
    pbRoutingTableEntry.add_regreportmask(EH_REG_REPORT_MASK_UNKNOWN);
    /*Index 3 - VPE subscription, nothing */
    pbRoutingTableEntry.add_regreportmask(EH_REG_REPORT_MASK_UNKNOWN);

    if (nullptr != pbSubModifiers) {
        pbFillSubscriptionMask(epSubInfo, pbSubModifiers);
            LOC_LOGd("sv meas constellation mask 0x%x, "
             "sv poly constellation mask 0x%x, sv eph constellation mask 0x%x",
             pbSubModifiers->svmeassubmodifier().constellationmask(),
             pbSubModifiers->svpolysubmodifier().constellationmask(),
             pbSubModifiers->svephsubmodifier().constellationmask());
    }
    regSessionMsgs = true;

    pbRoutingTableEntry.set_regsessionmsgs(regSessionMsgs);
    LOC_LOGd("report mask: 0x%x 0x%x 0x%x 0x%x, regStatusMask 0x%x,  Corrn Mask 0x%x, "
             "reg session msg %d",
             pbRoutingTableEntry.regreportmask(0), pbRoutingTableEntry.regreportmask(1),
             pbRoutingTableEntry.regreportmask(2), pbRoutingTableEntry.regreportmask(3),
             pbRoutingTableEntry.regstatusmask(), pbRoutingTableEntry.regcdfwcorrnmask(),
             pbRoutingTableEntry.regsessionmsgs());

    EngineHubMessage ehMsg;
    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    ehMsg.set_msgid(EH_MSG_ID_PROTOCOL_REGISTRATION_REQ);
    string ehMsgPayLoad, osTableEntry;
    string epUniqueName = "EP-" + std::to_string(getpid());

    if (EP_ENGINE_PROCESSOR_TYPE_LOCAL == engineProcessorType) {
        //Allocate and set?
        pbEngConnectInfo->set_socketname(sockName);
        LOC_LOGd("socket name %s", pbEngConnectInfo->socketname().c_str());
        pbRoutingTableEntry.SerializeToString(&osTableEntry);
        ehMsg.set_payload(osTableEntry);
        EHMsgUtils::freeUpEHRoutingTableEntry(pbRoutingTableEntry);
        ehMsg.SerializeToString(&ehMsgPayLoad);

        // Fill up EngineHubMessageWithMsgCounter
        ehMsgWithCtr.set_msgcounter(sendMsgCounter);
        // unique name for sender
        ehMsgWithCtr.set_msgsendername(epUniqueName);
        ehMsgWithCtr.set_ehmsgpayload(ehMsgPayLoad);
        ehMsgWithCtr.SerializeToString(&protoStr);
    } else {
        pbRoutingTableEntry.set_engconnectiontype(EH_ENGINE_CONNECT_TYPE_INET);
        EHEngineConnectionIpAddrInfo *engConnectIpAddrInfo =
                pbEngConnectInfo->mutable_pluginipaddrinfo();
        engConnectIpAddrInfo->set_pluginipaddress(ePServerIpAddr);
        engConnectIpAddrInfo->set_pluginport(atoi(ePServerPort));
        LOC_LOGd("ipAddr %s port %d", engConnectIpAddrInfo->pluginipaddress().c_str(),
                engConnectIpAddrInfo->pluginport());
        pbRoutingTableEntry.SerializeToString(&osTableEntry);
        ehMsg.set_payload(osTableEntry);
        EHMsgUtils::freeUpEHRoutingTableEntry(pbRoutingTableEntry);
        ehMsg.SerializeToString(&ehMsgPayLoad);

        // Fill up EngineHubMessageWithMsgCounter
        ehMsgWithCtr.set_msgcounter(sendMsgCounter);
        // unique name for sender
        ehMsgWithCtr.set_msgsendername(epUniqueName);
        ehMsgWithCtr.set_ehmsgpayload(ehMsgPayLoad);
        ehMsgWithCtr.SerializeToString(&protoStr);
    }
    return retval;
}

void EpProtoMsgConverter::pbFillSubscriptionMask
(
    const epSubscriptionInfo &epSubInfo,
    EHSubModifiers *subMods
)
{
    if (nullptr == subMods) {
        LOC_LOGe("subMods = nullptr !! return ");
        return;
    }
    uint32_t constellationMask = 0;
    uint32_t gnssSignalMask = 0;
    if (EP_GNSS_CONSTELLATION_UNKNOWN !=
                epSubInfo.subscriptionModMeasurement.constellationType) {
        EHSubscriptionModifier  *svMeasSubModifier = subMods->mutable_svmeassubmodifier();
        constellationMask = pbGetEhubConstellationTypeMask(
                epSubInfo.subscriptionModMeasurement);
        svMeasSubModifier->set_constellationmask((PBGnssConstellationInfoMask)constellationMask);
        if (EP_GNSS_SIGNAL_UNKNOWN !=
                    epSubInfo.subscriptionModMeasurement.gnssSignal) {
            gnssSignalMask = pbGetEHubGnssSignalTypeMask(
                    epSubInfo.subscriptionModMeasurement.gnssSignal);
        } else {
            gnssSignalMask = GNSS_SIGNAL_DEFAULT_L1_MASK;
        }
        svMeasSubModifier->set_gnsssignalmask((PBGnssSignalInfoMask)gnssSignalMask);
    }
    if (EP_GNSS_CONSTELLATION_UNKNOWN !=
                epSubInfo.subscriptionModMeasurementNHz.constellationType) {
        EHSubscriptionModifier *nHzSvMeasSubModifier = subMods->mutable_nhzsvmeassubmodifier();
        constellationMask =
                pbGetEhubConstellationTypeMask(epSubInfo.subscriptionModMeasurementNHz);
        nHzSvMeasSubModifier->set_constellationmask((PBGnssConstellationInfoMask)constellationMask);
        if (EP_GNSS_SIGNAL_UNKNOWN !=
                    epSubInfo.subscriptionModMeasurementNHz.gnssSignal) {
            gnssSignalMask = pbGetEHubGnssSignalTypeMask(
                    epSubInfo.subscriptionModMeasurementNHz.gnssSignal);
        } else {
            gnssSignalMask = GNSS_SIGNAL_DEFAULT_L1_MASK;
        }
        nHzSvMeasSubModifier->set_gnsssignalmask((PBGnssSignalInfoMask)gnssSignalMask);
    }
    if (EP_GNSS_CONSTELLATION_UNKNOWN !=
                epSubInfo.subscriptionModPolynomial.constellationType) {
        EHSubscriptionModifier *svPolySubModifier = subMods->mutable_svpolysubmodifier();
        constellationMask =
                pbGetEhubConstellationTypeMask(epSubInfo.subscriptionModPolynomial);
        svPolySubModifier->set_constellationmask((PBGnssConstellationInfoMask)constellationMask);
        if (EP_GNSS_SIGNAL_UNKNOWN !=
                    epSubInfo.subscriptionModPolynomial.gnssSignal) {
            gnssSignalMask = pbGetEHubGnssSignalTypeMask(
                    epSubInfo.subscriptionModPolynomial.gnssSignal);
        } else {
            gnssSignalMask = GNSS_SIGNAL_DEFAULT_L1_MASK;
        }
        svPolySubModifier->set_gnsssignalmask((PBGnssSignalInfoMask)gnssSignalMask);
    }
    if (EP_GNSS_CONSTELLATION_UNKNOWN !=
                epSubInfo.subscriptionModEphemeris.constellationType) {
        EHSubscriptionModifier *svEphSubModifier = subMods->mutable_svephsubmodifier();
        constellationMask =
                pbGetEhubConstellationTypeMask(epSubInfo.subscriptionModEphemeris);
        svEphSubModifier->set_constellationmask((PBGnssConstellationInfoMask)constellationMask);
        if (EP_GNSS_SIGNAL_UNKNOWN !=
                    epSubInfo.subscriptionModEphemeris.gnssSignal) {
            gnssSignalMask = pbGetEHubGnssSignalTypeMask(
                    epSubInfo.subscriptionModEphemeris.gnssSignal);
        } else {
            gnssSignalMask = GNSS_SIGNAL_DEFAULT_L1_MASK;
        }
        svEphSubModifier->set_gnsssignalmask((PBGnssSignalInfoMask)gnssSignalMask);
    }
    if (EP_GNSS_CONSTELLATION_UNKNOWN !=
                epSubInfo.subscriptionModIONO.constellationType) {
        EHSubscriptionModifier *ionoModelSubModifier = subMods->mutable_ionomodelsubmodifier();
        constellationMask =
                pbGetEhubConstellationTypeMask(epSubInfo.subscriptionModIONO);
        ionoModelSubModifier->set_constellationmask((PBGnssConstellationInfoMask)constellationMask);
        if (EP_GNSS_SIGNAL_UNKNOWN !=
                    epSubInfo.subscriptionModIONO.gnssSignal) {
            gnssSignalMask = pbGetEHubGnssSignalTypeMask(
                    epSubInfo.subscriptionModIONO.gnssSignal);
        } else {
            gnssSignalMask = GNSS_SIGNAL_DEFAULT_L1_MASK;
        }
        ionoModelSubModifier->set_gnsssignalmask((PBGnssSignalInfoMask)gnssSignalMask);
    }
}

bool EpProtoMsgConverter::pbEpImplPvtReportConvertProto
(
    const epImplPVTReport * pEpImplPvtReport,
    const Gnss_ApTimeStampStructType &bootTimeStamp,
    const string &epUniqueName,
    uint32_t sendMsgCounter,
    string &os
)
{
    bool retVal = false;
    if (nullptr == pEpImplPvtReport) {
        LOC_LOGe("pEpImplPvtReport = nullptr !! return ");
        return retVal;
    }

    EHMessageReportPosition pbEHubPosRptOut;
    PBUlpLocation *pbLocation = pbEHubPosRptOut.mutable_location();
    PBGpsLocationExtended *pbLocationExtended = pbEHubPosRptOut.mutable_locationextended();
    if ((nullptr != pbLocation) && (nullptr != pbLocationExtended)) {
        pbFillEHubPVTReport(pEpImplPvtReport, bootTimeStamp, pbLocation, pbLocationExtended);
        PBLocSessionStatus pbStatusValue;
        if (EP_NO_FIX == pEpImplPvtReport->pvtReport.statusOfFix) {
            pbStatusValue = PB_LOC_SESS_INTERMEDIATE;
        } else if (EP_VALID_FIX == pEpImplPvtReport->pvtReport.statusOfFix) {
            pbStatusValue = PB_LOC_SESS_SUCCESS;
        } else {
            pbStatusValue = PB_LOC_SESS_FAILURE;
        }
        pbEHubPosRptOut.set_status(pbStatusValue);
        retVal = true;
    }

    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    if (pbFillReportPositionEngineHubMsgWithCntr(pbEHubPosRptOut, epUniqueName, ehMsgWithCtr)) {
        if (pbEhMsgWithCtrSerializeToString(ehMsgWithCtr, sendMsgCounter, os)) {
            LOC_LOGe("pbEhMsgWithCtrSerializeToString complete");
            retVal = true;
        } else {
            retVal = false;
            LOC_LOGe("pbEhMsgWithCtrSerializeToString failed");
        }
    } else {
        retVal = false;
        LOC_LOGe("pbFillReportPositionEngineHubMsgWithCntr failed");
    }
    EHMsgUtils::freeUpEHMessageReportPosition(pbEHubPosRptOut);

    return retVal;
}

bool EpProtoMsgConverter::pbFillepGnssSvNotification
(
    const EHMessageReportSv *svRpt,
    epGnssSvNotification &ePGnssSv,
    EHTechId &sourceTechId
)
{
    memset(&ePGnssSv, 0, sizeof(ePGnssSv));
    if (nullptr == svRpt) {
        LOC_LOGe("svRpt = nullptr !! return ");
        return false;
    }
    sourceTechId = svRpt->sourcetechid();
    /* Convert in to Engine Plugin API structures */
    pbFillEpSvReport(svRpt, ePGnssSv);
    return true;
}

bool EpProtoMsgConverter::pbFillepGnssSvPolynomial
(
    const EHMessageReportSvPolynomial *pEhubSvPoly,
    epImplGnssSvPolynomial &ePSvPoly,
    EHTechId &sourceTechId
)
{
    memset(&ePSvPoly, 0, sizeof(ePSvPoly));
    if (nullptr == pEhubSvPoly) {
        LOC_LOGe("pEhubSvPoly = nullptr !! return ");
        return false;
    }
    sourceTechId = pEhubSvPoly->sourcetechid();
    pbFillEpGnssSvPolynomial(pEhubSvPoly, ePSvPoly);
    return true;
}

bool EpProtoMsgConverter::pbFillepDeadReckoningEngineConfig
(
    const EHMessageStatusB2sMountParams *pEHubB2sInfo,
    epDeadReckoningEngineConfig &epDREConfParams
)
{
    memset(&epDREConfParams, 0, sizeof(epDREConfParams));
    if (nullptr == pEHubB2sInfo) {
        LOC_LOGe("pEHubB2sInfo = nullptr !! return ");
        return false;
    }
    epDREConfParams.validMask = EP_BODY_TO_SENSOR_MOUNT_PARAMS_VALID;
    epBodyToSensorMountParams &epB2sParams = epDREConfParams.bodyToSensorMountParams;
    const PBBodyToSensorMountParams &b2sParams = pEHubB2sInfo->b2smountparams();
    epB2sParams.rollOffset = b2sParams.rolloffset();
    epB2sParams.yawOffset = b2sParams.yawoffset();
    epB2sParams.pitchOffset = b2sParams.pitchoffset();
    epB2sParams.offsetUnc = b2sParams.offsetunc();
    return true;
}

bool EpProtoMsgConverter::pbFillepGloAdditionalParameter
(
    const EHMessageReportGlonassAdditionalParams *pEHubAddParamsRpt,
    epGloAdditionalParameter &ePAdditionalData,
    EHTechId &sourceTechId
)
{
    memset(&ePAdditionalData, 0, sizeof(ePAdditionalData));
    if (nullptr == pEHubAddParamsRpt) {
        LOC_LOGe("pEHubAddParamsRpt = nullptr !! return ");
        return false;
    }
    sourceTechId = pEHubAddParamsRpt->sourcetechid();
    pbFillEpGloAddParamsData(pEHubAddParamsRpt, ePAdditionalData);
    return true;
}

bool EpProtoMsgConverter::pbFillepKlobucharIonoModel
(
    const EHMessageReportKlobucharIonoModel *pEHubIonoRpt,
    epKlobucharIonoModel &ePIonoRpt,
    EHTechId &sourceTechId
)
{
    memset(&ePIonoRpt, 0, sizeof(ePIonoRpt));
    if (nullptr == pEHubIonoRpt) {
        LOC_LOGe("pEHubIonoRpt = nullptr !! return ");
        return false;
    }
    sourceTechId = pEHubIonoRpt->sourcetechid();
    pbFillEpInonData(pEHubIonoRpt, ePIonoRpt);
    return true;
}

bool EpProtoMsgConverter::pbFillepGnssEphemerisReport
(
    const EHMessageReportSvEphemeris *pEHubEphRpt,
    epGnssEphemerisReport &ePEphRpt,
    EHTechId &sourceTechId
)
{
    memset(&ePEphRpt, 0, sizeof(ePEphRpt));
    if (nullptr == pEHubEphRpt) {
        LOC_LOGe("pEHubEphRpt = nullptr !! return ");
        return false;
    }
    sourceTechId = pEHubEphRpt->sourcetechid();
    pbFillEpGnssEphemeris(pEHubEphRpt, ePEphRpt);
    return true;
}

bool EpProtoMsgConverter::pbFillepImplPVTReport
(
    const EHMessageReportPosition *pEHubPosRpt,
    epImplPVTReport &ePImplPVTRpt,
    EHTechId &sourceTechId
)
{
    if (nullptr == pEHubPosRpt) {
        LOC_LOGe("pEHubPosRpt = nullptr !! return ");
        return false;
    }
    const PBUlpLocation& location = pEHubPosRpt->location();
    const PBLocGpsLocation &gpsLocation = location.gpslocation();
    const PBGpsLocationExtended& locationExtended = pEHubPosRpt->locationextended();
    bool  unpropagatedPosition = location.unpropagatedposition();
    LOC_LOGd("tech_mask %x Lat %lf Lon %lf Alt %lf timestamp %" PRIu64 " unpropagated %d",
                 locationExtended.tech_mask(),
                 gpsLocation.latitude(),
                 gpsLocation.longitude(),
                 gpsLocation.altitude(),
                 gpsLocation.timestamp(),
                 unpropagatedPosition);
    memset(&ePImplPVTRpt, 0, sizeof(ePImplPVTRpt));
    sourceTechId = pEHubPosRpt->sourcetechid();
    pbFillEpPVTReport(pEHubPosRpt, ePImplPVTRpt);
    return true;
}

bool EpProtoMsgConverter::pbFillepSetPositionModeCmdReq
(
    const EHMessageSessionSetFixMode *pEhubFixMode,
    epCommandRequest &epCmd
)
{
    if (nullptr == pEhubFixMode) {
        LOC_LOGe("pEhubFixMode = nullptr !! return ");
        return false;
    }
    epCmd.command = EP_COMMAND_SET_POSITION_MODE;
    PBLocPosMode poMode = pEhubFixMode->fixmode();
    epCmd.cmdReqParameter.positionModeInfo.interval = poMode.min_interval();
    return true;
}

uint32_t EpProtoMsgConverter::getPosEngineMaskFromDeleteAidingData
(
    const EHMessageSessionDeleteAidingData *pEhubDeleteData
)
{
    if (nullptr == pEhubDeleteData) {
        LOC_LOGe("pEhubDeleteData = nullptr !! return ");
        return 0;
    }
    const PBGnssAidingData& pbAidingData = pEhubDeleteData->gpsaidingdata();
    // PBPositioningEngineMask
    uint32_t posEngineMask = pbAidingData.posenginemask();
    return posEngineMask;
}

bool EpProtoMsgConverter::pbFillepDeleteAidingDataCmdReq
(
    const EHMessageSessionDeleteAidingData *pEhubDeleteData,
    epCommandRequest &epCmd
)
{
    memset(&epCmd, 0, sizeof(epCmd));
    if (nullptr == pEhubDeleteData) {
        LOC_LOGe("pEhubDeleteData = nullptr !! return ");
        return false;
    }
    epCmd.command = EP_COMMAND_DELETE_AIDING_DATA;
    const PBGnssAidingData& gpsAidingData = pEhubDeleteData->gpsaidingdata();
    if (true == gpsAidingData.deleteall()) {
        epCmd.cmdReqParameter.deleteAidingData.flags = EP_DELETE_ALL_AIDING_DATA;
    } else {
        const PBGnssAidingDataSv& svAiding = gpsAidingData.sv();
        // PBGnssAidingDataSvMask
        uint32_t svMask = svAiding.svmask();
        // PBGnssAidingDataSvTypeMask
        uint32_t svTypeMask = svAiding.svtypemask();

        if (PB_GNSS_AIDING_DATA_SV_EPHEMERIS_BIT & svMask) {
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GPS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GPS_EPHEMERIS_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GLO_EPHEMERIS_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_BDS_EPHEMERIS_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GAL_EPHEMERIS_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_QZSS_EPHEMERIS_DATA;
            }
        }
        if (PB_GNSS_AIDING_DATA_SV_POLY_BIT & svMask) {
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GPS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GPS_POLYNOMIAL_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GLO_POLYNOMIAL_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_BDS_POLYNOMIAL_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GAL_POLYNOMIAL_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_QZSS_POLYNOMIAL_DATA;
            }
        }
        if (PB_GNSS_AIDING_DATA_SV_IONOSPHERE_BIT & svMask) {
            if (PB_GNSS_AIDING_DATA_SV_TYPE_GPS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_GPS_IONO_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_BDS_IONO_DATA;
            }
            if (PB_GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT & svTypeMask) {
                epCmd.cmdReqParameter.deleteAidingData.flags |= EP_DELETE_QZSS_IONO_DATA;
            }
        }
    }
    return true;
}

void EpProtoMsgConverter::getNhzInfoFromSvMeas
(
    const EHMessageReportSvMeas *pEHubMsrRpt,
    EHTechId &sourceTechId,
    bool &isNhz
)
{
    if (nullptr == pEHubMsrRpt) {
        LOC_LOGe("pEHubMsrRpt = nullptr !! return ");
        isNhz = false;
        return;
    }

    sourceTechId = pEHubMsrRpt->sourcetechid();
    const PBGnssSvMeasurementSet &svMeasSet = pEHubMsrRpt->svmeasset();
    isNhz = svMeasSet.isnhz();
    return;
}

void EpProtoMsgConverter::getClkTickCountInfoFromSvMeas
(
    const EHMessageReportSvMeas *pEHubMsrRpt,
    uint64_t &headerFlags,
    uint64_t &refCountTicks,
    uint64_t &ptpTimens
)
{
    if (nullptr == pEHubMsrRpt) {
        LOC_LOGe("pEHubMsrRpt = nullptr !! return ");
        headerFlags = 0;
        refCountTicks = 0;
        ptpTimens = 0;
        return ;
    }

    const PBGnssSvMeasurementSet& svMeasSet = pEHubMsrRpt->svmeasset();
    const PBGnssSvMeasurementHeader& svMeasSetHeader = svMeasSet.svmeassetheader();
    headerFlags = svMeasSetHeader.flags();
    refCountTicks = svMeasSetHeader.refcountticks();
    ptpTimens = svMeasSet.ptptimens();
    return;
}

bool EpProtoMsgConverter::ParseFromString(::google::protobuf::MessageLite& ePMsg,
        const string& data) {
    bool bRetval = ePMsg.ParseFromString(data);
    if (!bRetval) {
        LOC_LOGe("ParseFromString failed");
    }
    return bRetval;
}

bool EpProtoMsgConverter::ParseFromString(::google::protobuf::MessageLite* ePMsg,
        const EngineHubMessage& ePPayLoadMsg) {
    bool bRetval = ePMsg->ParseFromString(ePPayLoadMsg.payload());
    if (!bRetval) {
        LOC_LOGe("ParseFromString failed");
    }
    return bRetval;
}

bool EpProtoMsgConverter::ParseFromString(::google::protobuf::MessageLite& ePMsg,
        uint32_t &msgCounter, string &senderName,
        const EngineHubMessageWithMsgCounter& ePPayLoadMsg) {
    msgCounter = ePPayLoadMsg.msgcounter();
    senderName = ePPayLoadMsg.msgsendername();
    bool bRetval = ePMsg.ParseFromString(ePPayLoadMsg.ehmsgpayload());
    if (!bRetval) {
        LOC_LOGe("ParseFromString failed");
    }
    return bRetval;
}

uint32_t EpProtoMsgConverter::getMsgIdFromEHubMessage(const EngineHubMessage& ePMsg) {
    uint32_t msgID = ePMsg.msgid();
    LOC_LOGd("ehMsg.msgid : %d", msgID);
    return msgID;
}

bool EpProtoMsgConverter::getConnectionStatusFromEHMsgStatusConnectivity
(
    const EHMessageStatusConnectivity &pBConStatus
)
{
    bool isConnected = pBConStatus.isconnected();
    LOC_LOGd("EH_MSG_ID_STATUS_CONNECTIVITY isConnected %d", isConnected);
    return isConnected;
}

void EpProtoMsgConverter::getInstallLicenseRespFromEHMsgQWESInstallLicenseResp
(
    const EHMessageQWESInstallLicenseResp &pBQwesResp,
    ehInstallLicenseStatus &status
)
{
    EHInstallLicenseStatus pbStatus = pBQwesResp.status();
    status = static_cast<ehInstallLicenseStatus>(static_cast<uint8_t>(pbStatus) + 1);
}

void EpProtoMsgConverter::getFeatureStatusFromEHMsgQWESFeatureStatusInfo
(
    const EHMessageQWESFeatureStatusInfo &pBQwesStatus,
    ehFeatureStatus &featureStatus,
    ehLicenseType &licenseType
)
{
    EHFeatureStatus pbFeatureStatus = pBQwesStatus.featurestatus();
    EHLicenseType pbLicenseType = pBQwesStatus.featuretype();
    featureStatus = static_cast<ehFeatureStatus>(pbFeatureStatus);
    licenseType = static_cast<ehLicenseType>(static_cast<uint8_t>(pbLicenseType) + 1);
}

void EpProtoMsgConverter::getAttestationStatementFromEHMsgQwesAttestationStatement
(
    const EHMessageQWESProvideAttestationStatement &pBAttestation,
    std::string &outBuf,
    uint32_t &bufLen,
    ehAttestationStatus &status
)
{
    outBuf = pBAttestation.outbuff();
    bufLen = pBAttestation.buflength();
    EHAttestationStatus pbStatus = pBAttestation.status();
    status = static_cast<ehAttestationStatus>(static_cast<uint8_t>(pbStatus) + 1);
}

int32_t EpProtoMsgConverter::getQwesSyncStatusFromEHMsgQWESForceSyncNotify
(
    const EHMessageQWESForceSyncNotify &pBNotify
)
{
    return pBNotify.status();
}

void EpProtoMsgConverter::getDestinationSocketNameEHRoutingTblEntry
(
    const EHRoutingTableEntry &routingTableEntry,
    EHubRoutingTableEntry &eHubRoutingTable
)
{
    EHConnectionType pbEngConnectionType = routingTableEntry.engconnectiontype();
    EngineConnectionType eHubEngConnectionType = ENGINE_CONNECT_TYPE_LOCAL;
    switch (pbEngConnectionType) {
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
    eHubRoutingTable.engConnectionType = eHubEngConnectionType;

    EHEngineConnectionInfo pbEngConnectInfo = routingTableEntry.engconnectinfo();
    string destSocket;
    if (EH_ENGINE_CONNECT_TYPE_LOCAL == pbEngConnectionType) {
        destSocket = pbEngConnectInfo.socketname();
        strlcpy(eHubRoutingTable.engConnectInfo.socketName, destSocket.c_str(),
                sizeof(eHubRoutingTable.engConnectInfo.socketName));
        LOC_LOGd("Sending to client %s", destSocket.c_str());
    } else if (EH_ENGINE_CONNECT_TYPE_INET == pbEngConnectionType) {
        const EHEngineConnectionIpAddrInfo &pluginIpAddrInfo =
                pbEngConnectInfo.pluginipaddrinfo();
        destSocket = pluginIpAddrInfo.pluginipaddress();
        uint16_t portNum = pluginIpAddrInfo.pluginport();
        strlcpy(eHubRoutingTable.engConnectInfo.pluginIpAddrInfo.pluginIpAddress,
                destSocket.c_str(),
                sizeof(eHubRoutingTable.engConnectInfo.pluginIpAddrInfo.pluginIpAddress));
        eHubRoutingTable.engConnectInfo.pluginIpAddrInfo.pluginPort = portNum;

        LOC_LOGd("Sending to client %s[port: %d]", destSocket.c_str(), portNum);
    }

    int num_entries = EH_NODE_COUNT;
    if ((int)EH_NODE_COUNT != (int)E_HUB_TECH_COUNT) {
        LOC_LOGe("EH_NODE_COUNT %d and E_HUB_TECH_COUNT %d not matching",
                EH_NODE_COUNT, E_HUB_TECH_COUNT);
        num_entries = std::min((int)EH_NODE_COUNT, (int)E_HUB_TECH_COUNT);
    }
    for (int i=0; i < num_entries; i++) {
        uint32_t localEhubRegReportMask = 0;
        uint32_t ehRegReportMask = routingTableEntry.regreportmask(i);
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
        eHubRoutingTable.regReportMask[i] = localEhubRegReportMask;
    }

    uint32_t ehRegStatusMask = routingTableEntry.regstatusmask();;
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
    eHubRoutingTable.regStatusMask = localEhubRegStatusMask;

    eHubRoutingTable.subModifiers = pbConvertToEHubSubModifiers(routingTableEntry.submodifiers());

    uint32_t ehRegReportCrrnMask = routingTableEntry.regcdfwcorrnmask();
    uint32_t eHubRegReportCorrnMask = 0;
    if (ehRegReportCrrnMask & EH_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION) {
        eHubRegReportCorrnMask |= E_HUB_REG_REPORT_MASK_CDFW_QDGNSS_REF_STATION;
    }
    if (ehRegReportCrrnMask & EH_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION) {
        eHubRegReportCorrnMask |= E_HUB_REG_REPORT_MASK_CDFW_QDGNSS_OBSERVATION;
    }
    eHubRoutingTable.regCdfwCorrnMask = eHubRegReportCorrnMask;

    eHubRoutingTable.regSessionMsgs = routingTableEntry.regsessionmsgs();
}

uint32_t EpProtoMsgConverter::getEhubMsgRoutingTblNumberOfEntries
(
    const EHMessageProtocolRoutingTableUpdate *ptable
)
{
   if (nullptr == ptable) {
        LOC_LOGe("ptable = nullptr !! return ");
        return 0;
    }
    uint32_t numEntries = ptable->ehubnumentries();
    LOC_LOGd("EHMessageProtocolRoutingTableUpdate num entries: %d", numEntries);
    return numEntries;
}

bool EpProtoMsgConverter::pbFillReportPositionEngineHubMsgWithCntr
(
    const EHMessageReportPosition &posRpt,
    const string &epUniqueName,
    EngineHubMessageWithMsgCounter &ehMsgWithCtr
)
{
    string ehMsgPayLoad, osPosition;
    EngineHubMessage ehMsg;
    ehMsg.set_msgid(EH_MSG_ID_REPORT_POSITION);
    posRpt.SerializeToString(&osPosition);
    ehMsg.set_payload(osPosition);
    if (!ehMsg.SerializeToString(&ehMsgPayLoad)) {
        LOC_LOGe("SerializeToString on ehMsg failed!");
        return false;
    }
    ehMsgWithCtr.set_msgsendername(epUniqueName);
    ehMsgWithCtr.set_ehmsgpayload(ehMsgPayLoad);
    return true;
}

bool EpProtoMsgConverter::checkIfSendReportToRoutingTblEntry
(
    const EHMessageProtocolRoutingTableUpdate pbRoutingTable,
    const EHubTechId &techID,
    unsigned int index,
    EHRoutingTableEntry &eHubRoutingTable
)
{
    eHubRoutingTable = pbRoutingTable.ehubroutingtable(index);
    // EHRegReportMask
    const uint32_t regReportMask = eHubRoutingTable.regreportmask(techID);
    return (regReportMask & EH_REG_REPORT_MASK_POSITION) ? true: false;
}

bool EpProtoMsgConverter::pbEhMsgWithCtrSerializeToString
(
    EngineHubMessageWithMsgCounter &ehMsgWithCtr,
    uint32_t sendMsgCounter,
    string &os
)
{
    ehMsgWithCtr.set_msgcounter(sendMsgCounter);
    if (!ehMsgWithCtr.SerializeToString(&os)) {
        LOC_LOGe("SerializeToString on ehMsgWithCtr failed!");
        return false;
    }
    return true;
}

// Engine Plugin Msg converter dyn load export function implementation
static EpProtoMsgConverter gEngPluginMsgConverter;

static bool pbRegisterWithEHub
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
) {
    return gEngPluginMsgConverter.pbRegisterWithEHub(techID, featureStatus, engineProcessorType,
            sendMsgCounter, epSubInfo, sockName, ePServerIpAddr, ePServerPort, featureId, protoStr);
}

static bool encodeEHubMessageReportPosition
(
    const epImplPVTReport *pEpImplPvtReport,
    const Gnss_ApTimeStampStructType &bootTimeStamp,
    const string &epUniqueName,
    uint32_t sendMsgCounter,
    string &protoStr
) {
    return gEngPluginMsgConverter.pbEpImplPvtReportConvertProto(pEpImplPvtReport, bootTimeStamp,
            epUniqueName, sendMsgCounter, protoStr);
}

static bool decodeEHubWthMsgCounterPayLoadMsg
(
    const string& eHubWithCtrPayLoadMsg,
    string& engHubMsgPayload,
    uint32_t &msgId,
    uint32_t &msgCounter,
    string &senderName
) {
    EngineHubMessageWithMsgCounter ehMsgWithCtr;
    if (!gEngPluginMsgConverter.ParseFromString(ehMsgWithCtr, eHubWithCtrPayLoadMsg)) {
        LOC_LOGe("Failed to parse ehMsgWithCtr from input stream!! length: %u",
                eHubWithCtrPayLoadMsg.size());
        return false;
    }
    EngineHubMessage ehMsg;
    if (!gEngPluginMsgConverter.ParseFromString(ehMsg, msgCounter, senderName, ehMsgWithCtr)) {
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

static EHubMessageProtocolRoutingTableUpdate* decodeEHMessageProtocolRoutTblUpdt
(
    const string& pbEhMsgProtoRoutTblUpdt
) {
    EHMessageProtocolRoutingTableUpdate pbEpRoutingTable;
    if (gEngPluginMsgConverter.ParseFromString(pbEpRoutingTable, pbEhMsgProtoRoutTblUpdt)) {
        EHubRoutingTableEntry eHubRoutingTable[MAX_E_HUB_ENTITY_COUNT];
        uint32_t numEntries = pbEpRoutingTable.ehubnumentries();
        LOC_LOGd("EHMessageProtocolRoutingTableUpdate num entries: %d", numEntries);

        for (uint32_t index = 0; index < numEntries; index++) {
            const EHRoutingTableEntry &pbEHubRoutingTable =
                    pbEpRoutingTable.ehubroutingtable(index);
            gEngPluginMsgConverter.getDestinationSocketNameEHRoutingTblEntry(
                    pbEHubRoutingTable, eHubRoutingTable[index]);
        }

        EHubMessageProtocolRoutingTableUpdate *pEpRoutingTable =
                new EHubMessageProtocolRoutingTableUpdate(numEntries, eHubRoutingTable);
        return pEpRoutingTable;
    } else {
        LOC_LOGe("ParseFromString EHMessageProtocolRoutingTableUpdate failed");
        return nullptr;
    }
}

static bool pbFillepSetPositionModeCmdReq
(
    const string& ehMsgPayload,
    epCommandRequest &epCmd
) {
    EHMessageSessionSetFixMode ehubFixMode;
    if (!gEngPluginMsgConverter.ParseFromString(ehubFixMode, ehMsgPayload)) {
        LOC_LOGe("Failed to parse ehubFixMode from input stream!! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepSetPositionModeCmdReq(&ehubFixMode, epCmd);
}

static bool pbFillepDeleteAidingDataCmdReq
(
    const string& ehMsgPayload,
    epCommandRequest &epCmd
) {
    EHMessageSessionDeleteAidingData delAidData;
    if (!gEngPluginMsgConverter.ParseFromString(delAidData, ehMsgPayload)) {
        LOC_LOGe("Failed to parse delAidData from input stream!! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepDeleteAidingDataCmdReq(&delAidData, epCmd);
}

static uint32_t getPosEngineMaskFromDeleteAidingData
(
    const string& ehMsgMsrDelAidDataPayload
) {
    EHMessageSessionDeleteAidingData delAidData;
    if (!gEngPluginMsgConverter.ParseFromString(delAidData, ehMsgMsrDelAidDataPayload)) {
        LOC_LOGe("Failed to parse delAidData from input stream!! length: %u",
                ehMsgMsrDelAidDataPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.getPosEngineMaskFromDeleteAidingData(&delAidData);
}

static bool pbFillepImplPVTReport
(
    const string& ehMsgPayload,
    epImplPVTReport &ePImplPVTRpt,
    EHTechId &sourceTechId
) {
    EHMessageReportPosition posReport;
    if (!gEngPluginMsgConverter.ParseFromString(posReport, ehMsgPayload)) {
        LOC_LOGe("Failed to parse posReport from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepImplPVTReport(&posReport, ePImplPVTRpt, sourceTechId);
}

static bool pbFillepGnssSvNotification
(
    const string& ehMsgPayload,
    epGnssSvNotification &ePGnssSv,
    EHTechId &sourceTechId
) {
    EHMessageReportSv svReport;
    if (!gEngPluginMsgConverter.ParseFromString(svReport, ehMsgPayload)) {
        LOC_LOGe("Failed to parse svReport from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepGnssSvNotification(&svReport, ePGnssSv, sourceTechId);
}

static bool pbFillepGnssSvPolynomial
(
    const string& ehMsgPayload,
    epImplGnssSvPolynomial &ePSvPoly,
    EHTechId &sourceTechId
) {
    EHMessageReportSvPolynomial svPoly;
    if (!gEngPluginMsgConverter.ParseFromString(svPoly, ehMsgPayload)) {
        LOC_LOGe("Failed to parse svPoly from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepGnssSvPolynomial(&svPoly, ePSvPoly, sourceTechId);
}

static void getNhzInfoFromSvMeas
(
    const string& ehMsgMsrRptPayload,
    EHTechId &sourceTechId,
    bool &isNhz
) {
    EHMessageReportSvMeas svMeas;
    if (!gEngPluginMsgConverter.ParseFromString(svMeas, ehMsgMsrRptPayload)) {
        LOC_LOGe("Failed to parse svMeas from input stream! length: %u",
                ehMsgMsrRptPayload.size());
        return;
    }
    gEngPluginMsgConverter.getNhzInfoFromSvMeas(&svMeas, sourceTechId, isNhz);
}

static bool pbFillepGnssMeasReport
(
    const string& ehMsgPayload,
    epGnssImplMeasurementReport &ePImplMsrRptOut,
    uint64_t &measurementStatus
) {
    EHMessageReportSvMeas svMeas;
    if (!gEngPluginMsgConverter.ParseFromString(svMeas, ehMsgPayload)) {
        LOC_LOGe("Failed to parse svMeas from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillEpGnssMeasReport(&svMeas, ePImplMsrRptOut,
            measurementStatus);
}

static void getClkTickCountInfoFromSvMeas
(
    const string& ehMsgMsrRptPayload,
    uint64_t &headerFlags,
    uint64_t &refCountTicks,
    uint64_t &ptpTimens
) {
    EHMessageReportSvMeas svMeas;
    if (!gEngPluginMsgConverter.ParseFromString(svMeas, ehMsgMsrRptPayload)) {
        LOC_LOGe("Failed to parse svMeas from input stream! length: %u",
                ehMsgMsrRptPayload.size());
        return;
    }
    gEngPluginMsgConverter.getClkTickCountInfoFromSvMeas(&svMeas, headerFlags,
            refCountTicks, ptpTimens);
}

static bool getConnectionStatusFromEHMsgStatusConnectivity
(
    const string& ehMsgMsgStatConnectivity
) {
    EHMessageStatusConnectivity connectStat;
    if (!gEngPluginMsgConverter.ParseFromString(connectStat, ehMsgMsgStatConnectivity)) {
        LOC_LOGe("Failed to parse connectStat from input stream! length: %u",
                ehMsgMsgStatConnectivity.size());
        return false;
    }
    return gEngPluginMsgConverter.getConnectionStatusFromEHMsgStatusConnectivity(connectStat);
}

static void getInstallLicenseRespFromEHMsgQWESInstallLicenseResp
(
    const string &ehMsgQwesResp,
    ehInstallLicenseStatus &status
) {
    EHMessageQWESInstallLicenseResp licenseResp;
    if (gEngPluginMsgConverter.ParseFromString(licenseResp, ehMsgQwesResp)) {
        gEngPluginMsgConverter.getInstallLicenseRespFromEHMsgQWESInstallLicenseResp(licenseResp,
                status);
    }
}

static void getFeatureStatusFromEHMsgQWESFeatureStatusInfo
(
    const string &ehMsgQwesStatus,
    ehFeatureStatus &featureStatus,
    ehLicenseType &licenseType
) {
    EHMessageQWESFeatureStatusInfo featureStatusInfo;
    if (gEngPluginMsgConverter.ParseFromString(featureStatusInfo, ehMsgQwesStatus)) {
        gEngPluginMsgConverter.getFeatureStatusFromEHMsgQWESFeatureStatusInfo(
                featureStatusInfo, featureStatus, licenseType);
    }
}

static void getAttestationStatementFromEHMsgQwesAttestationStatement
(
    const string &ehMsgAttestation,
    std::string &outBuf,
    uint32_t &bufLen,
    ehAttestationStatus &status
) {
    EHMessageQWESProvideAttestationStatement attestationMsg;
    if (gEngPluginMsgConverter.ParseFromString(attestationMsg, ehMsgAttestation)) {
        gEngPluginMsgConverter.getAttestationStatementFromEHMsgQwesAttestationStatement(
                attestationMsg, outBuf, bufLen, status);
    }
}

static int32_t getQwesSyncStatusFromEHMsgQWESForceSyncNotify
(
    const string &ehMsgNotify
) {
    EHMessageQWESForceSyncNotify qwesNotify;
    if (gEngPluginMsgConverter.ParseFromString(qwesNotify, ehMsgNotify)) {
        return gEngPluginMsgConverter.getQwesSyncStatusFromEHMsgQWESForceSyncNotify(qwesNotify);
    } else {
        return -1;
    }
}

static bool pbFillepGnssEphemerisReport
(
    const string& ehMsgPayload,
    epGnssEphemerisReport &ePEphRpt,
    EHTechId &sourceTechId
) {
    EHMessageReportSvEphemeris svEph;
    if (!gEngPluginMsgConverter.ParseFromString(svEph, ehMsgPayload)) {
        LOC_LOGe("Failed to parse svEph from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepGnssEphemerisReport(&svEph, ePEphRpt, sourceTechId);
}

static bool pbFillepKlobucharIonoModel
(
    const string& ehMsgPayload,
    epKlobucharIonoModel &ePIonoRpt,
    EHTechId &sourceTechId
) {
    EHMessageReportKlobucharIonoModel kloIonoModelRprt;
    if (!gEngPluginMsgConverter.ParseFromString(kloIonoModelRprt, ehMsgPayload)) {
        LOC_LOGe("Failed to parse kloIonoModelRprt from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepKlobucharIonoModel(&kloIonoModelRprt, ePIonoRpt,
            sourceTechId);
}

static bool pbFillepGloAdditionalParameter
(
    const string& ehMsgPayload,
    epGloAdditionalParameter &ePAdditionalData,
    EHTechId &sourceTechId
) {
    EHMessageReportGlonassAdditionalParams gloAddParams;
    if (!gEngPluginMsgConverter.ParseFromString(gloAddParams, ehMsgPayload)) {
        LOC_LOGe("Failed to parse gloAddParams from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepGloAdditionalParameter(&gloAddParams, ePAdditionalData,
            sourceTechId);
}

#ifdef FEATURE_CDFW
static void pbFillQDgnssReferenceStationData
(
    const string& ehMsgPayload,
    ReferenceStation &qDgnssRefStation
) {
    EHMessageReportCdfwQDgnssReferenceStation qDgnssRefStatEhMsg;
    if (!gEngPluginMsgConverter.ParseFromString(qDgnssRefStatEhMsg, ehMsgPayload)) {
        LOC_LOGe("Failed to parse qDgnssRefStatEhMsg from input stream! length: %u",
                ehMsgPayload.size());
        return;
    }
    return gEngPluginMsgConverter.pbFillQDgnssReferenceStationData(&qDgnssRefStatEhMsg,
            qDgnssRefStation);
}

static void pbFillQDgnssObservation
(
    const string& ehMsgPayload,
    DGnssObservation &qDgnssObserv
) {
    EHMessageReportCdfwQDgnssObservation qDgnssObsEhMsg;
    if (!gEngPluginMsgConverter.ParseFromString(qDgnssObsEhMsg, ehMsgPayload)) {
        LOC_LOGe("Failed to parse qDgnssObsEhMsg from input stream! length: %u",
                ehMsgPayload.size());
        return;
    }
    return gEngPluginMsgConverter.pbFillQDgnssObservation(&qDgnssObsEhMsg, qDgnssObserv);
}
#endif

static void pbFillepLeverArmConfData
(
    const string& ehMsgPayload,
    epLeverArmConfigInfo &ePLeverArmConfigData
) {
    EHMessageStatusLeverArmConfig leverArmCfg;
    if (!gEngPluginMsgConverter.ParseFromString(leverArmCfg, ehMsgPayload)) {
        LOC_LOGe("Failed to parse leverArmCfg from input stream! length: %u",
                ehMsgPayload.size());
        return;
    }
    return gEngPluginMsgConverter.pbFillEpLeverArmConfData(&leverArmCfg, ePLeverArmConfigData);
}

static bool pbFillepDeadReckoningEngineConfig
(
    const string& ehMsgPayload,
    epDeadReckoningEngineConfig &epDREConfParams
) {
    EHMessageStatusB2sMountParams b2SMntParams;
    if (!gEngPluginMsgConverter.ParseFromString(b2SMntParams, ehMsgPayload)) {
        LOC_LOGe("Failed to parse b2SMntParams from input stream! length: %u",
                ehMsgPayload.size());
        return false;
    }
    return gEngPluginMsgConverter.pbFillepDeadReckoningEngineConfig(&b2SMntParams,
            epDREConfParams);

}

static int pbPopulateQwesInstallLicenseReqWithCtr
(
    const uint8_t *licenseBuf,
    uint32_t licenseLen,
    const string &epUniqueName,
    uint32_t sendMsgCounter,
    string &os
) {
    return gEngPluginMsgConverter.pbPopulateQwesInstallLicenseReqWithCtr(licenseBuf, licenseLen,
            epUniqueName, sendMsgCounter, os);
}

static int pbPopulateAttestationStatementReqWithCtr
(
    const uint8_t *nonce,
    uint32_t nonceLen,
    const uint8_t *data,
    uint32_t dataLen,
    const string &epUniqueName,
    uint32_t sendMsgCounter,
    string &os
) {
    return gEngPluginMsgConverter.pbPopulateAttestationStatementReqWithCtr(nonce, nonceLen,
            data, dataLen, epUniqueName, sendMsgCounter, os);
}

// Library entry point for eng plugin msg converter
static const EngPluginMsgConvInterface gEngPluginMsgConv = {
    sizeof(EngPluginMsgConvInterface),
    // Encode routines
    pbRegisterWithEHub,
    encodeEHubMessageReportPosition,
    // Decode routines
    decodeEHubWthMsgCounterPayLoadMsg,
    decodeEHMessageProtocolRoutTblUpdt,
    pbFillepSetPositionModeCmdReq,
    pbFillepDeleteAidingDataCmdReq,
    getPosEngineMaskFromDeleteAidingData,
    pbFillepImplPVTReport,
    pbFillepGnssSvNotification,
    pbFillepGnssSvPolynomial,
    getNhzInfoFromSvMeas,
    pbFillepGnssMeasReport,
    getClkTickCountInfoFromSvMeas,
    getConnectionStatusFromEHMsgStatusConnectivity,
    getInstallLicenseRespFromEHMsgQWESInstallLicenseResp,
    getFeatureStatusFromEHMsgQWESFeatureStatusInfo,
    getAttestationStatementFromEHMsgQwesAttestationStatement,
    getQwesSyncStatusFromEHMsgQWESForceSyncNotify,
    pbFillepGnssEphemerisReport,
    pbFillepKlobucharIonoModel,
    pbFillepGloAdditionalParameter,
#ifdef FEATURE_CDFW
    pbFillQDgnssReferenceStationData,
    pbFillQDgnssObservation,
#endif
    pbFillepLeverArmConfData,
    pbFillepDeadReckoningEngineConfig,
    pbPopulateQwesInstallLicenseReqWithCtr,
    pbPopulateAttestationStatementReqWithCtr,
};

// Entry point for this library - get engine plugin msg converter
// interface
#ifndef DEBUG_X86
extern "C" const EngPluginMsgConvInterface* getEngPluginMsgConvIface()
#else
const EngPluginMsgConvInterface* getEngPluginMsgConvIface()
#endif // DEBUG_X86
{
    return &gEngPluginMsgConv;
}

}
