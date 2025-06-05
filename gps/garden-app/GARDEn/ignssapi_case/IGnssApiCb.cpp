/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include "GardenUtil.h"
#include "IGnssApiCb.h"

using std::lock_guard;

namespace garden {

IGnssApiCb::IGnssApiCb(IGnssApiCase* gCase): mNetworkInitiatedResponse({}) {
    mGnssCase = gCase;
}

IGnssApiCb::~IGnssApiCb() {
    if (mGnssCase) {
        mGnssCase = nullptr;
    }
}

void IGnssApiCb::gnssLocationCb(const Location& location) {
    GnssCbBase::gnssLocationCb(location);
}

void IGnssApiCb::gnssStatusCb(uint32_t status) {
    GnssCbBase::gnssStatusCb(status);
}

void IGnssApiCb::gnssSvStatusCb(const GnssSvNotification& svNotify) {
    GnssCbBase::gnssSvStatusCb(svNotify);
    for (size_t i = 0; i < svNotify.count; ++i) {
        gardenPrint("%02d : SVID: %04d, CNR: %09.4f, "
                "ELE: %09.4f, AZI: %09.4f , FREQ: %9.0f, "
                "TYPE: %u EPH: %u ALM: %u USED: %u FRE: %u",
                i+1, svNotify.gnssSvs[i].svId, svNotify.gnssSvs[i].cN0Dbhz,
                svNotify.gnssSvs[i].elevation, svNotify.gnssSvs[i].azimuth,
                svNotify.gnssSvs[i].carrierFrequencyHz,
                svNotify.gnssSvs[i].type,
                (svNotify.gnssSvs[i].gnssSvOptionsMask &
                        GNSS_SV_OPTIONS_HAS_EPHEMER_BIT) ==
                GNSS_SV_OPTIONS_HAS_EPHEMER_BIT,
                (svNotify.gnssSvs[i].gnssSvOptionsMask &
                        GNSS_SV_OPTIONS_HAS_ALMANAC_BIT) ==
                GNSS_SV_OPTIONS_HAS_ALMANAC_BIT,
                (svNotify.gnssSvs[i].gnssSvOptionsMask &
                        GNSS_SV_OPTIONS_USED_IN_FIX_BIT) ==
                GNSS_SV_OPTIONS_USED_IN_FIX_BIT,
                (svNotify.gnssSvs[i].gnssSvOptionsMask &
                        GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT) ==
                GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT);
    }
}

void IGnssApiCb::gnssMeasurementsCb(
        const GnssMeasurementsNotification& gnssMeasurementsNotification) {

    for (int i = 0; i< gnssMeasurementsNotification.count && i < GNSS_MEASUREMENTS_MAX; i++) {
        gardenPrint("%02d : Const: %d,  svid: %d,"
            " time_offset_ns: %.2f, state %d,"
            " c_n0_dbhz: %.2f, c_freq_Hz: %.2f,"
            " adrStateMask: 0x%02x, adrM: %.2f, adrUncM: %.6f,"
            " pseudorange_rate_mps: %.2f,"
            " pseudorange_rate_uncertainty_mps: %.2f,"
            " agcLevelDb: %.2f, codeType: %d, flags: 0x%08x",
            i + 1,
            gnssMeasurementsNotification.measurements[i].svType,
            gnssMeasurementsNotification.measurements[i].svId,
            gnssMeasurementsNotification.measurements[i].timeOffsetNs,
            gnssMeasurementsNotification.measurements[i].stateMask,
            gnssMeasurementsNotification.measurements[i].carrierToNoiseDbHz,
            gnssMeasurementsNotification.measurements[i].carrierFrequencyHz,
            gnssMeasurementsNotification.measurements[i].adrStateMask,
            gnssMeasurementsNotification.measurements[i].adrMeters,
            gnssMeasurementsNotification.measurements[i].adrUncertaintyMeters,
            gnssMeasurementsNotification.measurements[i].pseudorangeRateMps,
            gnssMeasurementsNotification.measurements[i].pseudorangeRateUncertaintyMps,
            gnssMeasurementsNotification.measurements[i].agcLevelDb,
            gnssMeasurementsNotification.measurements[i].codeType,
            gnssMeasurementsNotification.measurements[i].flags);

        gardenPrint(" Clocks Info");
        gardenPrint(" time_ns: %" PRId64 " full_bias_ns: %" PRId64 ""
            " bias_ns: %g, bias_uncertainty_ns: %g,"
            " drift_nsps: %g, drift_uncertainty_nsps: %g,"
            " hw_clock_discontinuity_count: %d, flags: 0x%04x",
            gnssMeasurementsNotification.clock.timeNs,
            gnssMeasurementsNotification.clock.fullBiasNs,
            gnssMeasurementsNotification.clock.biasNs,
            gnssMeasurementsNotification.clock.biasUncertaintyNs,
            gnssMeasurementsNotification.clock.driftNsps,
            gnssMeasurementsNotification.clock.driftUncertaintyNsps,
            gnssMeasurementsNotification.clock.hwClockDiscontinuityCount,
            gnssMeasurementsNotification.clock.flags);
    }
}

void IGnssApiCb::gnssNmeaCb(uint64_t timestamp, const char* nmea, int length) {

    gardenPrint ("## %s]: () ##:: Timestamp: %" PRIu64 " NMEA string Length: %d",
            __FUNCTION__, timestamp, length);
    if (length > 0) {
        gardenPrint ("## %s]: () ##:: NMEA String: %s", __FUNCTION__, nmea);
    }
}

void IGnssApiCb::gnssDataCb(const GnssDataNotification& dataNotify) {

    GnssCbBase::gnssDataCb(dataNotify);

    for (int sig = GNSS_LOC_SIGNAL_TYPE_GPS_L1CA;
        sig < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
        if (GNSS_LOC_DATA_JAMMER_IND_BIT ==
            (dataNotify.gnssDataMask[sig] & GNSS_LOC_DATA_JAMMER_IND_BIT)) {
            gardenPrint("jammerInd[%d] = %f", sig, dataNotify.jammerInd[sig]);
        }
        if (GNSS_LOC_DATA_AGC_BIT ==
            (dataNotify.gnssDataMask[sig] & GNSS_LOC_DATA_AGC_BIT)) {
            gardenPrint("agc[%d] = %f", sig, dataNotify.agc[sig]);
        }
    }
}

void IGnssApiCb::gnssNiNotifyCb(uint32_t id, const GnssNiNotification& notification) {

    GnssCbBase::gnssNiNotifyCb(id, notification);
}

void IGnssApiCb::gnssLocInfoNotifCb(GnssLocationInfoNotification gnssLocInfoNotif) {

    GnssCbBase::gnssLocInfoNotifCb(gnssLocInfoNotif);
}

void IGnssApiCb::setNiParam(int niCount, int niResPatCount, int* networkInitiatedResponse) {
}
} // namespace garden
