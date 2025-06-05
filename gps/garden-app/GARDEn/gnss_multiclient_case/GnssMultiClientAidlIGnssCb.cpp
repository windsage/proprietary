/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_AidlIGnssCb"

#include "GnssMultiClientAidlIGnssCb.h"
#include "GnssMultiClientCaseUtils.h"
#include "GnssMultiClientAidlIGnss.h"

namespace garden {

GnssMultiClientAidlIGnssCb::GnssMultiClientAidlIGnssCb() {
}

GnssMultiClientAidlIGnssCb::~GnssMultiClientAidlIGnssCb() {
}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssCb::gnssSetCapabilitiesCb(int capabilities) {
    CONDPRINTLN("%s: capabilities=%u\n", __func__, capabilities);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssCb::gnssSetSignalTypeCapabilitiesCb(
    const std::vector<GnssSignalType>& in_gnssSignalTypes) {
    CONDPRINTLN("%s: Signal Set capabilities\n", __func__);

    const char* constellationString[] = { "UNKNOWN", "GPS", "SBAS",
        "GLONASS", "QZSS", "BEIDOU", "GALILEO", "NAVIC" };

    for (int i = 0; i < in_gnssSignalTypes.size(); i++) {
        int temp;

        temp = (int)in_gnssSignalTypes[i].constellation;
        if (temp > 7) {
            temp = 0;
        }

        gardenPrint("%02d : constellation: %s,"
                    " carrierFrequencyHz: %.2f,"
                    " codeType: %s",
                    i + 1,
                    constellationString[temp],
                    in_gnssSignalTypes[i].carrierFrequencyHz,
                    in_gnssSignalTypes[i].codeType.c_str());
    }

    return ndk::ScopedAStatus::ok();
}

GnssMultiClientAidlIGnssMeasCb::GnssMultiClientAidlIGnssMeasCb() {
}

GnssMultiClientAidlIGnssMeasCb::~GnssMultiClientAidlIGnssMeasCb() {}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssMeasCb::gnssMeasurementCb(const GnssDataAidl& data) {

    CONDPRINTLN("## %s]: () ##, num of measurements: %zu", __FUNCTION__, data.measurements.size());
    for (int i = 0; i < data.measurements.size(); i++) {
        gardenPrint("%02d : flags: 0x%08x,"
                    " svid: %d,"
                    " signalType.constellation: %u,"
                    " signalType.carrierFrequencyHz: %.2f,"
                    " signalType.codeType: %s,"
                    " timeOffsetNs: %.2f,"
                    " state: 0x%08x,"
                    " receivedSvTimeInNs: %" PRIu64
                    " receivedSvTimeUncertaintyInNs: %" PRIu64
                    " antennaCN0DbHz: %.2f,"
                    " basebandCN0DbHz: %.2f,"
                    " pseudorangeRateMps : %.2f,"
                    " pseudorangeRateUncertaintyMps : %.2f,\n"
                    "      accumulatedDeltaRangeState: 0x%08x,"
                    " accumulatedDeltaRangeM: %.2f, "
                    " accumulatedDeltaRangeUncertaintyM : %.2f, "
                    " carrierCycles: %" PRIu64
                    " carrierPhase: %.2f,"
                    " carrierPhaseUncertainty: %.2f,"
                    " multipathIndicator: %u,"
                    " snrDb: %.2f,"
                    " agcLevelDb: %.2f,"
                    " fullInterSignalBiasNs: %.2f,"
                    " fullInterSignalBiasUncertaintyNs: %.2f,"
                    " satelliteInterSignalBiasNs: %.2f,"
                    " satelliteInterSignalBiasUncertaintyNs: %.2f",
                    i + 1,
                    data.measurements[i].flags,
                    data.measurements[i].svid,
                    data.measurements[i].signalType.constellation,
                    data.measurements[i].signalType.carrierFrequencyHz,
                    data.measurements[i].signalType.codeType.c_str(),
                    data.measurements[i].timeOffsetNs,
                    data.measurements[i].state,
                    data.measurements[i].receivedSvTimeInNs,
                    data.measurements[i].receivedSvTimeUncertaintyInNs,
                    data.measurements[i].antennaCN0DbHz,
                    data.measurements[i].basebandCN0DbHz,
                    data.measurements[i].pseudorangeRateMps,
                    data.measurements[i].pseudorangeRateUncertaintyMps,
                    data.measurements[i].accumulatedDeltaRangeState,
                    data.measurements[i].accumulatedDeltaRangeM,
                    data.measurements[i].accumulatedDeltaRangeUncertaintyM,
                    data.measurements[i].carrierCycles,
                    data.measurements[i].carrierPhase,
                    data.measurements[i].carrierPhaseUncertainty,
                    data.measurements[i].multipathIndicator,
                    data.measurements[i].snrDb,
                    data.measurements[i].agcLevelDb,
                    data.measurements[i].fullInterSignalBiasNs,
                    data.measurements[i].fullInterSignalBiasUncertaintyNs,
                    data.measurements[i].satelliteInterSignalBiasNs,
                    data.measurements[i].satelliteInterSignalBiasUncertaintyNs);
        gardenPrint("      satellitePvt.satPosEcef.posXMeters: %.2f,"
                    " satellitePvt.satPosEcef.posYMeters: %.2f,"
                    " satellitePvt.satPosEcef.posZMeters: %.2f,"
                    " satellitePvt.satPosEcef.ureMeters: %.2f,"
                    " satellitePvt.satVelEcef.velXMps: %.2f,"
                    " satellitePvt.satVelEcef.velYMps: %.2f,"
                    " satellitePvt.satVelEcef.velZMps: %.2f,"
                    " satellitePvt.satVelEcef.ureRateMps: %.2f,"
                    " satellitePvt.satClockInfo.satHardwareCodeBiasMeters: %.2f,"
                    " satellitePvt.satClockInfo.satTimeCorrectionMeters: %.2f,"
                    " satellitePvt.satClockInfo.satClkDriftMps: %.2f,"
                    " satellitePvt.ionoDelayMeters: %.2f,"
                    " satellitePvt.tropoDelayMeters: %.2f"
                    " satellitePvt.TOC: %" PRIi64 ""
                    " satellitePvt.IODC: %d"
                    " satellitePvt.TOE: %" PRIx64 ""
                    " satellitePvt.IODE: %d"
                    " satellitePvt.ephemerisSource: %d",
                    data.measurements[i].satellitePvt.satPosEcef.posXMeters,
                    data.measurements[i].satellitePvt.satPosEcef.posYMeters,
                    data.measurements[i].satellitePvt.satPosEcef.posZMeters,
                    data.measurements[i].satellitePvt.satPosEcef.ureMeters,
                    data.measurements[i].satellitePvt.satVelEcef.velXMps,
                    data.measurements[i].satellitePvt.satVelEcef.velYMps,
                    data.measurements[i].satellitePvt.satVelEcef.velZMps,
                    data.measurements[i].satellitePvt.satVelEcef.ureRateMps,
                    data.measurements[i].satellitePvt.satClockInfo.satHardwareCodeBiasMeters,
                    data.measurements[i].satellitePvt.satClockInfo.satTimeCorrectionMeters,
                    data.measurements[i].satellitePvt.satClockInfo.satClkDriftMps,
                    data.measurements[i].satellitePvt.ionoDelayMeters,
                    data.measurements[i].satellitePvt.tropoDelayMeters,
                    data.measurements[i].satellitePvt.timeOfClockSeconds,
                    data.measurements[i].satellitePvt.issueOfDataClock,
                    data.measurements[i].satellitePvt.timeOfEphemerisSeconds,
                    data.measurements[i].satellitePvt.issueOfDataEphemeris,
                    data.measurements[i].satellitePvt.ephemerisSource
                );
    }
    gardenPrint(" Clocks Info "
                " gnssClockFlags: 0x%04x,"
                " leapSecond: %d,"
                " timeNs: %" PRId64
                " timeUncertaintyNs: %.2f,"
                " fullBiasNs: %" PRId64
                " biasNs: %.2f,"
                " biasUncertaintyNs: %.2f,"
                " driftNsps: %.2f,"
                " driftUncertaintyNsps: %.2f,"
                " hwClockDiscontinuityCount: %u,"
                " referenceSignalTypeForIsb.constellation: %u,"
                " referenceSignalTypeForIsb.carrierFrequencyHz: %.2f,"
                " referenceSignalTypeForIsb.codeType: %s",
                data.clock.gnssClockFlags,
                data.clock.leapSecond,
                data.clock.timeNs,
                data.clock.timeUncertaintyNs,
                data.clock.fullBiasNs,
                data.clock.biasNs,
                data.clock.biasUncertaintyNs,
                data.clock.driftNsps,
                data.clock.driftUncertaintyNsps,
                data.clock.hwClockDiscontinuityCount,
                data.clock.referenceSignalTypeForIsb.constellation,
                data.clock.referenceSignalTypeForIsb.carrierFrequencyHz,
                data.clock.referenceSignalTypeForIsb.codeType.c_str());
    gardenPrint(" ElapsedRealtime "
                " flags: 0x%08x,"
                " timestampNs: %" PRId64", "
                " timeUncertaintyNs: %.2f",
                data.elapsedRealtime.flags,
                data.elapsedRealtime.timestampNs,
                data.elapsedRealtime.timeUncertaintyNs);
    for (size_t i = 0; i < data.gnssAgcs.size(); i++) {
        gardenPrint("%02d : "
                    " agcLevelDb: %.2f,"
                    " signalType.constellation: %u,"
                    " signalType.carrierFrequencyHz:%" PRIi64 "",
                    i,
                    data.gnssAgcs[i].agcLevelDb,
                    data.gnssAgcs[i].constellation,
                    data.gnssAgcs[i].carrierFrequencyHz);
    }
    gardenPrint("isFullTracking = %d", data.isFullTracking);

    return ndk::ScopedAStatus::ok();
}

GnssMultiClientAidlIGnssPowerIndCb::GnssMultiClientAidlIGnssPowerIndCb() {}

GnssMultiClientAidlIGnssPowerIndCb::~GnssMultiClientAidlIGnssPowerIndCb() {}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssPowerIndCb::gnssPowerStatsCb(
        const GnssDataPowerStatsAidl& gnssPowerStats) {
    gardenPrint("gnssPowerStats.elapsedRealtime.flags: 0x%08X"
                " gnssPowerStats.elapsedRealtime.timestampNs: %" PRId64", "
                " gnssPowerStats.elapsedRealtime.timeUncertaintyNs: %.2f,"
                " gnssPowerStatistics.totalEnergyMilliJoule = %.2f",
                gnssPowerStats.elapsedRealtime.flags,
                gnssPowerStats.elapsedRealtime.timestampNs,
                gnssPowerStats.elapsedRealtime.timeUncertaintyNs,
                gnssPowerStats.totalEnergyMilliJoule);

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssPowerIndCb::setCapabilitiesCb(
        int32_t capabilities) {
    CONDPRINTLN("%s: capabilities=%u\n", __func__, capabilities);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssVisibCtrlCb::nfwNotifyCb(
        const IGnssVisibilityControlCallback::NfwNotification& notification) {
    gardenPrint("NfwNotification, proxyAppPackageName: %s protocolStack: %d, requestorId: %s",
            notification.proxyAppPackageName.c_str(), notification.protocolStack,
            notification.requestorId.c_str());
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssVisibCtrlCb::isInEmergencySession(bool* _aidl_return) {
    gardenPrint("isInEmergencySession: %d", *_aidl_return);
    return ndk::ScopedAStatus::ok();
}


::ndk::ScopedAStatus GnssMultiClientAidlIGnssMeasCorrCb::setCapabilitiesCb(int32_t capabilities) {
    gardenPrint("capabilities: %08x", capabilities);
    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus GnssMultiClientAidlIGnssAntennaInfoCb::gnssAntennaInfoCb(
        const std::vector<IGnssAntennaInfoCallback::GnssAntennaInfo>& gnssAntennaInfos) {
    gardenPrint("# of gnssAntennaInfo: %zu", gnssAntennaInfos.size());
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssMultiClientAidlIAGnssCb::agnssStatusCb(IAGnssCallback::AGnssType type,
        IAGnssCallback::AGnssStatusValue status) {
    gardenPrint("AgnssType: %d, AgnssStatus: %d", type, status);
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus GnssMultiClientAidlIGnssBatchingCb::gnssLocationBatchCb(
        const std::vector<GnssLocation>& locations) {
    for (size_t i = 0; i < locations.size(); i++) {
        gardenPrint("%02d : flags: %x"
                    " lat: %.2f,"
                    " long: %.2f, alt: %.2f",
                    i, locations[i].gnssLocationFlags, locations[i].latitudeDegrees,
                    locations[i].longitudeDegrees, locations[i].altitudeMeters);
    }
    return ndk::ScopedAStatus::ok();
}
} // namespace garden
