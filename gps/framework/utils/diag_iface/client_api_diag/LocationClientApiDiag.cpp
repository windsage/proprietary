/******************************************************************************
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  
  Copyright (c) 2019-2020 The Linux Foundation. All rights reserved
 *******************************************************************************/

#include "LocationClientApiDiag.h"
#include <diag_lsm.h>
#include <msg.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LocClientApiDiag"

#ifndef LOG_GNSS_CLIENT_API_LOCATION_REPORT_C
#define LOG_GNSS_CLIENT_API_LOCATION_REPORT_C (0x1C8F)
#endif
#ifndef LOG_GNSS_CLIENT_API_SV_REPORT_C
#define LOG_GNSS_CLIENT_API_SV_REPORT_C (0x1C90)
#endif
#ifndef LOG_GNSS_CLIENT_API_NMEA_REPORT_C
#define LOG_GNSS_CLIENT_API_NMEA_REPORT_C (0x1CB2)
#endif
#ifndef LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C
#define LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C (0x1CB7)
#endif
#ifndef LOG_GNSS_CLIENT_API_SV_POLY_REPORT_C
#define LOG_GNSS_CLIENT_API_SV_POLY_REPORT_C (0x1CC7)
#endif
#ifndef LOG_GNSS_LATENCY_REPORT_C
#define LOG_GNSS_LATENCY_REPORT_C (0x1CE8)
#endif
#ifndef LOG_GNSS_CLIENT_API_DC_REPORT_C
#define LOG_GNSS_CLIENT_API_DC_REPORT_C (0x1DBC)
#endif
#ifndef LOG_GNSS_CLIENT_API_GEOFENCE_REPORT_C
#define LOG_GNSS_CLIENT_API_GEOFENCE_REPORT_C (0x1E0D)
#endif
#ifndef LOG_GNSS_CLIENT_API_EPH_REPORT_C
#define LOG_GNSS_CLIENT_API_EPH_REPORT_C (0x1E58)
#endif

namespace location_client {

LocationClientApiDiag::LocationClientApiDiag(): mDiagIface(LocDiagIface::getInstance()) {}

void GnssLocationReport::log() {
    if (!mDiagIface) {
        return;
    }

    LOC_LOGd("GnssLocationReport::log");
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssLocationStructType* diagGnssLocPtr = nullptr;
    diagGnssLocPtr = (clientDiagGnssLocationStructType*)
            mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_LOCATION_REPORT_C,
                                 sizeof(clientDiagGnssLocationStructType), &bufferSrc);
    if (diagGnssLocPtr == NULL) {
        LOC_LOGv("diag memory alloc failed");
        return;
    }
    //Initialize the variables with zero expcet header info
    memset((char *)diagGnssLocPtr + sizeof(diagGnssLocPtr->logHeader),
            0, sizeof(clientDiagGnssLocationStructType) - sizeof(diagGnssLocPtr->logHeader));

    populateClientDiagLocation(diagGnssLocPtr, mGnssLocation, mDiagLocationInfoExt);
    diagGnssLocPtr->version = LOG_CLIENT_LOCATION_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssLocPtr, bufferSrc,
                          LOG_GNSS_CLIENT_API_LOCATION_REPORT_C,
                          sizeof(clientDiagGnssLocationStructType));
}

void GeofenceBreachDiagReport::log() {
    if (!mDiagIface) {
        return;
    }

    LOC_LOGd("GeofenceBreachDiagReport::log");
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGeofenceBreachStructType* diagGnssLocPtr = nullptr;
    diagGnssLocPtr = (clientDiagGeofenceBreachStructType*)
            mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_GEOFENCE_REPORT_C,
                                 sizeof(clientDiagGeofenceBreachStructType), &bufferSrc);
    if (diagGnssLocPtr == NULL) {
        LOC_LOGv("diag memory alloc failed");
        return;
    }
    //Initialize the variables with zero expcet header info
    memset((char *)diagGnssLocPtr + sizeof(diagGnssLocPtr->logHeader),
            0, sizeof(clientDiagGeofenceBreachStructType) - sizeof(diagGnssLocPtr->logHeader));

    populateClientDiagGeofenceBreach(diagGnssLocPtr, mGeofenceBreachNotif, mGfVector);
    diagGnssLocPtr->version = LOG_CLIENT_GEOFENCE_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssLocPtr, bufferSrc,
                          LOG_GNSS_CLIENT_API_GEOFENCE_REPORT_C,
                          sizeof(clientDiagGeofenceBreachStructType));
}
void GnssSvReport::log() {
    if (!mDiagIface) {
        return;
    }

    LOC_LOGd("GnssSvReport::log");
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssSvStructType* diagGnssSvPtr = nullptr;
    diagGnssSvPtr = (clientDiagGnssSvStructType*)mDiagIface->logAlloc(
            LOG_GNSS_CLIENT_API_SV_REPORT_C,
            sizeof(clientDiagGnssSvStructType), &bufferSrc);
    if (diagGnssSvPtr == NULL) {
        LOC_LOGv("memory alloc failed");
        return;
    }
    populateClientDiagGnssSv(diagGnssSvPtr, mGnssSvsVector);
    diagGnssSvPtr->version = LOG_CLIENT_SV_REPORT_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssSvPtr, bufferSrc,
            LOG_GNSS_CLIENT_API_SV_REPORT_C,
            sizeof(clientDiagGnssSvStructType));
}

void GnssNmeaReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGd("GnssNmeaReport::log");
    size_t diagBufferSize = sizeof(clientDiagGnssNmeaStructType) + mLength - 1;
    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssNmeaStructType* diagGnssNmeaPtr = nullptr;
    diagGnssNmeaPtr = (clientDiagGnssNmeaStructType*)
        mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_NMEA_REPORT_C,
                diagBufferSize, &bufferSrc);
    if (diagGnssNmeaPtr == NULL) {
        LOC_LOGv("memory alloc failed");
        return;
    }
    diagGnssNmeaPtr->timestamp = mTimestamp;
    diagGnssNmeaPtr->nmeaLength = mLength;
    diagGnssNmeaPtr->engType = mEngType;
    memcpy(&diagGnssNmeaPtr->nmea, mNmea, mLength);
    diagGnssNmeaPtr->version = LOG_CLIENT_NMEA_REPORT_DIAG_MSG_VERSION;

    mDiagIface->logCommit(diagGnssNmeaPtr, bufferSrc,
            LOG_GNSS_CLIENT_API_NMEA_REPORT_C,
            sizeof(clientDiagGnssNmeaStructType));
}

void GnssMeasReport::log() {
    if (!mDiagIface) {
        return;
    }
    uint8_t maxSequence = 1;
    uint32_t measCount = mGnssMeasurements.measurements.size();
    if (0 < measCount) {
        maxSequence = (uint8)(((measCount-1) / CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) + 1);
    }
    for (uint8 i = 0; i < maxSequence; i++) {
        diagBuffSrc bufferSrc = BUFFER_INVALID;
        clientDiagGnssMeasurementsStructType* diagGnssMeasPtr = nullptr;
        diagGnssMeasPtr =
                (clientDiagGnssMeasurementsStructType*)mDiagIface->logAlloc(
                        LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C,
                        sizeof(clientDiagGnssMeasurementsStructType),
                        &bufferSrc);
        if (NULL == diagGnssMeasPtr) {
            LOC_LOGv("memory alloc failed");
            return;
        }

        diagGnssMeasPtr->version = LOG_CLIENT_MEASUREMENTS_DIAG_MSG_VERSION;
        diagGnssMeasPtr->maxSequence = maxSequence;
        diagGnssMeasPtr->sequenceNumber = i + 1;
        diagGnssMeasPtr->count = measCount - i * CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
        if (diagGnssMeasPtr->count > CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) {
            diagGnssMeasPtr->count = CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
        }
        LOC_LOGv("maxSequence = %d, seqNumber = %d, total meas cnt = %d, "
                 "meas count in this packet = %d",
                 diagGnssMeasPtr->maxSequence, diagGnssMeasPtr->sequenceNumber,
                 measCount, (uint32_t) diagGnssMeasPtr->count);

        diagGnssMeasPtr->isNhz = mGnssMeasurements.isNhz;
        diagGnssMeasPtr->clock.flags =
            (clientDiagGnssMeasurementsClockFlagsMask)mGnssMeasurements.clock.flags;
        diagGnssMeasPtr->clock.leapSecond = mGnssMeasurements.clock.leapSecond;
        diagGnssMeasPtr->clock.timeNs = mGnssMeasurements.clock.timeNs;
        diagGnssMeasPtr->clock.timeUncertaintyNs =
                mGnssMeasurements.clock.timeUncertaintyNs;
        diagGnssMeasPtr->clock.fullBiasNs = mGnssMeasurements.clock.fullBiasNs;
        diagGnssMeasPtr->clock.biasNs = mGnssMeasurements.clock.biasNs;
        diagGnssMeasPtr->clock.biasUncertaintyNs =
                mGnssMeasurements.clock.biasUncertaintyNs;
        diagGnssMeasPtr->clock.driftNsps = mGnssMeasurements.clock.driftNsps;
        diagGnssMeasPtr->clock.driftUncertaintyNsps =
                mGnssMeasurements.clock.driftUncertaintyNsps;
        diagGnssMeasPtr->clock.hwClockDiscontinuityCount =
                mGnssMeasurements.clock.hwClockDiscontinuityCount;
        diagGnssMeasPtr->clock.elapsedRealTime = mGnssMeasurements.clock.elapsedRealTime;
        diagGnssMeasPtr->clock.elapsedRealTimeUnc = mGnssMeasurements.clock.elapsedRealTimeUnc;
        diagGnssMeasPtr->clock.elapsedgPTPTime = mGnssMeasurements.clock.elapsedgPTPTime;
        diagGnssMeasPtr->clock.elapsedgPTPTimeUnc = mGnssMeasurements.clock.elapsedgPTPTimeUnc;
        populateClientDiagMeasurements(diagGnssMeasPtr, mGnssMeasurements);
        diagGnssMeasPtr->agcStatusL1 = (clientDiagAgcStatus)mGnssMeasurements.agcStatusL1;
        diagGnssMeasPtr->agcStatusL2 = (clientDiagAgcStatus)mGnssMeasurements.agcStatusL2;
        diagGnssMeasPtr->agcStatusL5 = (clientDiagAgcStatus)mGnssMeasurements.agcStatusL5;
        mDiagIface->logCommit(diagGnssMeasPtr, bufferSrc,
                LOG_GNSS_CLIENT_API_MEASUREMENTS_REPORT_C,
                sizeof(clientDiagGnssMeasurementsStructType));
    }
}

void GnssLatencyReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGv("GnssLatencyReport::log");

    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssLatencyStructType* diagGnssLatencyPtr = nullptr;
    diagGnssLatencyPtr = (clientDiagGnssLatencyStructType*)
            mDiagIface->logAlloc(
                    LOG_GNSS_LATENCY_REPORT_C,
                    sizeof(clientDiagGnssLatencyStructType),
                    &bufferSrc);
    if (diagGnssLatencyPtr == NULL) {
        LOC_LOGe("diag memory alloc failed");
        return;
    }

    diagGnssLatencyPtr->meQtimer1 = mGnssLatencyInfo.meQtimer1;
    diagGnssLatencyPtr->meQtimer2 = mGnssLatencyInfo.meQtimer2;
    diagGnssLatencyPtr->meQtimer3 = mGnssLatencyInfo.meQtimer3;
    diagGnssLatencyPtr->peQtimer1 = mGnssLatencyInfo.peQtimer1;
    diagGnssLatencyPtr->peQtimer2 = mGnssLatencyInfo.peQtimer2;
    diagGnssLatencyPtr->peQtimer3 = mGnssLatencyInfo.peQtimer3;
    diagGnssLatencyPtr->smQtimer1 = mGnssLatencyInfo.smQtimer1;
    diagGnssLatencyPtr->smQtimer2 = mGnssLatencyInfo.smQtimer2;
    diagGnssLatencyPtr->smQtimer3 = mGnssLatencyInfo.smQtimer3;
    diagGnssLatencyPtr->locMwQtimer = mGnssLatencyInfo.locMwQtimer;
    diagGnssLatencyPtr->hlosQtimer1 = mGnssLatencyInfo.hlosQtimer1;
    diagGnssLatencyPtr->hlosQtimer2 = mGnssLatencyInfo.hlosQtimer2;
    diagGnssLatencyPtr->hlosQtimer3 = mGnssLatencyInfo.hlosQtimer3;
    diagGnssLatencyPtr->hlosQtimer4 = mGnssLatencyInfo.hlosQtimer4;
    diagGnssLatencyPtr->hlosQtimer5 = mGnssLatencyInfo.hlosQtimer5;
    diagGnssLatencyPtr->version = LOG_CLIENT_LATENCY_DIAG_MSG_VERSION;

    uint64_t qTimerLatency = 0;

    if (diagGnssLatencyPtr->hlosQtimer2 >= diagGnssLatencyPtr->meQtimer2 &&
        0 != diagGnssLatencyPtr->meQtimer2) {
        /* The starting point to compute overall latency is meQtimer2, and this is because
           between meQtimer1 amd meQtimer2 there is internal processing in ME. Also, we
           need to adjust for the the time between hlosQtimer1 (measurements block arrival time
           to HLOS) and hlosQtimer2 (position arrival time to HLOS), hence below formula */
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer5 -
                (diagGnssLatencyPtr->hlosQtimer2 - diagGnssLatencyPtr->hlosQtimer1) -
                diagGnssLatencyPtr->meQtimer2;
        diagGnssLatencyPtr->overallLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("overallLatency=%" PRIi64 " ", diagGnssLatencyPtr->overallLatency);
    } else {
        LOC_LOGw("overallLatency can't be computed");
        diagGnssLatencyPtr->overallLatency = 0;
    }

    if (diagGnssLatencyPtr->meQtimer2 >= diagGnssLatencyPtr->meQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->meQtimer2 - diagGnssLatencyPtr->meQtimer1;
        diagGnssLatencyPtr->meLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("meLatency1=%" PRIi64 " ", diagGnssLatencyPtr->meLatency1);
    } else {
        LOC_LOGw("meLatency1 can't be computed");
        diagGnssLatencyPtr->meLatency1 = 0;
    }

    if (diagGnssLatencyPtr->meQtimer3 >= diagGnssLatencyPtr->meQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->meQtimer3 - diagGnssLatencyPtr->meQtimer2;
        diagGnssLatencyPtr->meLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("meLatency2=%" PRIi64 " ", diagGnssLatencyPtr->meLatency2);
    } else {
        LOC_LOGw("meLatency2 can't be computed");
        diagGnssLatencyPtr->meLatency2 = 0;
    }

    if (diagGnssLatencyPtr->peQtimer1 >= diagGnssLatencyPtr->meQtimer3) {
        qTimerLatency = diagGnssLatencyPtr->peQtimer1 - diagGnssLatencyPtr->meQtimer3;
        diagGnssLatencyPtr->meToPeLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("meToPeLatency=%" PRIi64 " ", diagGnssLatencyPtr->meToPeLatency);
    } else {
        LOC_LOGw("meToPeLatency can't be computed");
        diagGnssLatencyPtr->meToPeLatency = 0;
    }

    if (diagGnssLatencyPtr->peQtimer2 >= diagGnssLatencyPtr->peQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->peQtimer2 - diagGnssLatencyPtr->peQtimer1;
        diagGnssLatencyPtr->peLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("peLatency1=%" PRIi64 " ", diagGnssLatencyPtr->peLatency1);
    } else {
        LOC_LOGw("peLatency1 can't be computed");
        diagGnssLatencyPtr->peLatency1 = 0;
    }

    if (diagGnssLatencyPtr->peQtimer3 >= diagGnssLatencyPtr->peQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->peQtimer3 - diagGnssLatencyPtr->peQtimer2;
        diagGnssLatencyPtr->peLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("peLatency2=%" PRIi64 " ", diagGnssLatencyPtr->peLatency2);
    } else {
        LOC_LOGw("peLatency2 can't be computed");
        diagGnssLatencyPtr->peLatency2 = 0;
    }

    if (diagGnssLatencyPtr->smQtimer1 >= diagGnssLatencyPtr->peQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->smQtimer1 - diagGnssLatencyPtr->peQtimer1;
        diagGnssLatencyPtr->peToSmLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("peToSmLatency=%" PRIi64 " ", diagGnssLatencyPtr->peToSmLatency);
    } else {
        LOC_LOGw("peToSmLatency can't be computed");
        diagGnssLatencyPtr->peToSmLatency = 0;
    }

    if (diagGnssLatencyPtr->smQtimer2 >= diagGnssLatencyPtr->smQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->smQtimer2 - diagGnssLatencyPtr->smQtimer1;
        diagGnssLatencyPtr->smLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("smLatency1=%" PRIi64 " ", diagGnssLatencyPtr->smLatency1);
    } else {
        LOC_LOGw("smLatency1 can't be computed");
        diagGnssLatencyPtr->smLatency1 = 0;
    }

    if (diagGnssLatencyPtr->smQtimer3 >= diagGnssLatencyPtr->smQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->smQtimer3 - diagGnssLatencyPtr->smQtimer2;
        diagGnssLatencyPtr->smLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("smLatency2=%" PRIi64 " ", diagGnssLatencyPtr->smLatency2);
    } else {
        LOC_LOGw("smLatency2 can't be computed");
        diagGnssLatencyPtr->smLatency2 = 0;
    }

    if (diagGnssLatencyPtr->locMwQtimer >= diagGnssLatencyPtr->smQtimer3) {
        qTimerLatency = diagGnssLatencyPtr->locMwQtimer - diagGnssLatencyPtr->smQtimer3;
        diagGnssLatencyPtr->smToLocMwLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("smToLocMwLatency=%" PRIi64 " ", diagGnssLatencyPtr->smToLocMwLatency);
    } else {
        LOC_LOGw("smToLocMwLatency can't be computed");
        diagGnssLatencyPtr->smToLocMwLatency = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer1 >= diagGnssLatencyPtr->locMwQtimer) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer1 - diagGnssLatencyPtr->locMwQtimer;
        diagGnssLatencyPtr->locMwToHlosLatency = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("locMwToHlosLatency=%" PRIi64 " ", diagGnssLatencyPtr->locMwToHlosLatency);
    } else {
        LOC_LOGw("locMwToHlosLatency can't be computed");
        diagGnssLatencyPtr->locMwToHlosLatency = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer2 >= diagGnssLatencyPtr->hlosQtimer1) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer2 - diagGnssLatencyPtr->hlosQtimer1;
        diagGnssLatencyPtr->hlosLatency1 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency1=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency1);
    } else {
        LOC_LOGw("hlosLatency1 can't be computed");
        diagGnssLatencyPtr->hlosLatency1 = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer3 >= diagGnssLatencyPtr->hlosQtimer2) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer3 - diagGnssLatencyPtr->hlosQtimer2;
        diagGnssLatencyPtr->hlosLatency2 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency2=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency2);
    } else {
        LOC_LOGw("hlosLatency2 can't be computed");
        diagGnssLatencyPtr->hlosLatency2 = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer4 >= diagGnssLatencyPtr->hlosQtimer3) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer4 - diagGnssLatencyPtr->hlosQtimer3;
        diagGnssLatencyPtr->hlosLatency3 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency3=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency3);
    } else {
        LOC_LOGw("hlosLatency3 can't be computed");
        diagGnssLatencyPtr->hlosLatency3 = 0;
    }

    if (diagGnssLatencyPtr->hlosQtimer5 >= diagGnssLatencyPtr->hlosQtimer4) {
        qTimerLatency = diagGnssLatencyPtr->hlosQtimer5 - diagGnssLatencyPtr->hlosQtimer4;
        diagGnssLatencyPtr->hlosLatency4 = qTimerTicksToNanos(double(qTimerLatency));
        LOC_LOGv("hlosLatency4=%" PRIi64 " ", diagGnssLatencyPtr->hlosLatency3);
    } else {
        LOC_LOGw("hlosLatency4 can't be computed");
        diagGnssLatencyPtr->hlosLatency4 = 0;
    }

    mDiagIface->logCommit(diagGnssLatencyPtr, bufferSrc,
            LOG_GNSS_LATENCY_REPORT_C,
            sizeof(clientDiagGnssLatencyStructType));
}

void GnssDiagDcReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGv("GnssDiagDcReport::log");

    diagBuffSrc bufferSrc = BUFFER_INVALID;
    clientDiagGnssDcReportStructType* diagGnssDcReportPtr = (clientDiagGnssDcReportStructType*)
            mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_DC_REPORT_C,
                                 sizeof(clientDiagGnssDcReportStructType),
                                 &bufferSrc);
    if (diagGnssDcReportPtr == NULL) {
        LOC_LOGe("diag memory alloc failed");
        return;
    }

    diagGnssDcReportPtr->dcReportType =
            (clientDiagGnssDcReportType) mGnssDcReport.dcReportType;
    diagGnssDcReportPtr->numValidBits = mGnssDcReport.numValidBits;
    diagGnssDcReportPtr->dcReportDataLen = mGnssDcReport.dcReportData.size();
    memset(diagGnssDcReportPtr->dcReportData, 0, sizeof(diagGnssDcReportPtr->dcReportData));
    int index = 0;
    for (uint8_t byte : mGnssDcReport.dcReportData) {
        if (index < sizeof(diagGnssDcReportPtr->dcReportData)) {
            diagGnssDcReportPtr->dcReportData[index++] = byte;
        }
    }

    diagGnssDcReportPtr->version = LOG_CLIENT_DC_REPORT_DIAG_MSG_VERSION;
    mDiagIface->logCommit(diagGnssDcReportPtr, bufferSrc,
                          LOG_GNSS_CLIENT_API_DC_REPORT_C,
                          sizeof(clientDiagGnssDcReportStructType));
}


void LocationClientApiDiag::translateDiagGnssLocationPositionDynamics(
        clientDiagGnssLocationPositionDynamics& out, const GnssLocationPositionDynamics& in) {
    out.bodyFrameDataMask = in.bodyFrameDataMask;
    out.longAccel = in.longAccel;
    out.latAccel = in.latAccel;
    out.vertAccel = in.vertAccel;
    out.longAccelUnc = in.longAccelUnc;
    out.latAccelUnc = in.latAccelUnc;
    out.vertAccelUnc = in.vertAccelUnc;

    out.roll = in.roll;
    out.rollUnc = in.rollUnc;
    out.rollRate = in.rollRate;
    out.rollRateUnc = in.rollRateUnc;

    out.pitch = in.pitch;
    out.pitchUnc = in.pitchUnc;
    out.pitchRate = in.pitchRate;
    out.pitchRateUnc = in.pitchRateUnc;

    out.yaw = in.yaw;
    out.yawUnc = in.yawUnc;
    out.yawRate = in.yawRate;
    out.yawRateUnc = in.yawRateUnc;
}

clientDiagGnssSystemTimeStructType LocationClientApiDiag::parseDiagGnssTime(
        const GnssSystemTimeStructType &halGnssTime) {
    clientDiagGnssSystemTimeStructType gnssTime;
    memset(&gnssTime, 0, sizeof(gnssTime));
    uint32_t gnssTimeFlags = 0;

    if (GNSS_SYSTEM_TIME_WEEK_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_VALID;
        gnssTime.systemWeek = halGnssTime.systemWeek;
    }
    if (GNSS_SYSTEM_TIME_WEEK_MS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
        gnssTime.systemMsec = halGnssTime.systemMsec;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
        gnssTime.systemClkTimeBias = halGnssTime.systemClkTimeBias;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
        gnssTime.systemClkTimeUncMs = halGnssTime.systemClkTimeUncMs;
    }
    if (GNSS_SYSTEM_REF_FCOUNT_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_REF_FCOUNT_VALID;
        gnssTime.refFCount = halGnssTime.refFCount;
    }
    if (GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
        gnssTime.numClockResets = halGnssTime.numClockResets;
    }

    gnssTime.validityMask = (clientDiagGnssSystemTimeStructTypeFlags)gnssTimeFlags;

    return gnssTime;
}

clientDiagGnssGloTimeStructType LocationClientApiDiag::parseDiagGloTime(
        const GnssGloTimeStructType &halGloTime) {

    clientDiagGnssGloTimeStructType gloTime;
    memset(&gloTime, 0, sizeof(gloTime));
    uint32_t gloTimeFlags = 0;

    if (GNSS_CLO_DAYS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_CLO_DAYS_VALID;
        gloTime.gloDays = halGloTime.gloDays;
    }
    if (GNSS_GLO_MSEC_VALID  & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_MSEC_VALID ;
        gloTime.gloMsec = halGloTime.gloMsec;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_VALID;
        gloTime.gloClkTimeBias = halGloTime.gloClkTimeBias;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_UNC_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
        gloTime.gloClkTimeUncMs = halGloTime.gloClkTimeUncMs;
    }
    if (GNSS_GLO_REF_FCOUNT_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_REF_FCOUNT_VALID;
        gloTime.refFCount = halGloTime.refFCount;
    }
    if (GNSS_GLO_NUM_CLOCK_RESETS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
        gloTime.numClockResets = halGloTime.numClockResets;
    }
    if (GNSS_GLO_FOUR_YEAR_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_FOUR_YEAR_VALID;
        gloTime.gloFourYear = halGloTime.gloFourYear;
    }

    gloTime.validityMask = (clientDiagGnssGloTimeStructTypeFlags)gloTimeFlags;

    return gloTime;
}

void LocationClientApiDiag::translateDiagSystemTime(
        clientDiagGnssSystemTime& out, const GnssSystemTime& in) {

    out.gnssSystemTimeSrc = (clientDiagGnss_LocSvSystemEnumType)in.gnssSystemTimeSrc;
    switch (in.gnssSystemTimeSrc) {
        case GNSS_LOC_SV_SYSTEM_GPS:
           out.u.gpsSystemTime = parseDiagGnssTime(in.u.gpsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
           out.u.galSystemTime = parseDiagGnssTime(in.u.galSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
           out.u.gloSystemTime = parseDiagGloTime(in.u.gloSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_BDS:
           out.u.bdsSystemTime = parseDiagGnssTime(in.u.bdsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
           out.u.qzssSystemTime = parseDiagGnssTime(in.u.qzssSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_NAVIC:
           out.u.navicSystemTime = parseDiagGnssTime(in.u.navicSystemTime);
           break;
        default:
           break;
    }
}

void LocationClientApiDiag::parseLocation(const ::Location &halLocation,
        clientDiagGnssLocationStructType& location) {
    memset(&location, 0, sizeof(location));
    clientDiagLocationFlagsMask flags = 0;

    location.timestamp = halLocation.timestamp;
    location.latitude = halLocation.latitude;
    location.longitude = halLocation.longitude;
    location.altitude = halLocation.altitude;
    location.speed = halLocation.speed;
    location.bearing = halLocation.bearing;
    location.horizontalAccuracy = halLocation.accuracy;
    location.verticalAccuracy = halLocation.verticalAccuracy;
    location.speedAccuracy = halLocation.speedAccuracy;
    location.bearingAccuracy = halLocation.bearingAccuracy;
    location.timeUncMs = halLocation.timeUncMs;
#ifndef FEATURE_EXTERNAL_AP
    location.elapsedRealTimeNs = halLocation.elapsedRealTime;
    location.elapsedRealTimeUncNs = halLocation.elapsedRealTimeUnc;
#else
    location.elapsedRealTimeNs = 0;
    location.elapsedRealTimeUncNs = 0;
#endif

    if (0 != halLocation.timestamp) {
        flags |= CLIENT_DIAG_LOCATION_HAS_TIMESTAMP_BIT;
    }
    if (::LOCATION_HAS_TIME_UNC_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_TIME_UNC_BIT;
    }
    if (::LOCATION_HAS_LAT_LONG_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_LAT_LONG_BIT;
    }
    if (::LOCATION_HAS_ALTITUDE_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_ALTITUDE_BIT;
    }
    if (::LOCATION_HAS_SPEED_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_SPEED_BIT;
    }
    if (::LOCATION_HAS_BEARING_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_BEARING_BIT;
    }
    if (::LOCATION_HAS_ACCURACY_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_ACCURACY_BIT;
    }
    if (::LOCATION_HAS_VERTICAL_ACCURACY_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (::LOCATION_HAS_SPEED_ACCURACY_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (::LOCATION_HAS_BEARING_ACCURACY_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
#ifndef FEATURE_EXTERNAL_AP
    if (::LOCATION_HAS_ELAPSED_REAL_TIME_BIT & halLocation.flags) {
        flags |= CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT;
        flags |= CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_UNC_BIT;
    }
#endif
    location.flags = flags;

    clientDiagLocationTechnologyMask techMask = 0;
    if (::LOCATION_TECHNOLOGY_GNSS_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_TECHNOLOGY_GNSS_BIT;
    }
    if (::LOCATION_TECHNOLOGY_CELL_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_TECHNOLOGY_CELL_BIT;
    }
    if (::LOCATION_TECHNOLOGY_WIFI_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_TECHNOLOGY_WIFI_BIT;
    }
    if (::LOCATION_TECHNOLOGY_SENSORS_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_TECHNOLOGY_SENSORS_BIT;
    }
    if (::LOCATION_TECHNOLOGY_REFERENCE_LOCATION_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_REFERENCE_LOCATION_BIT;
    }
    if (::LOCATION_TECHNOLOGY_INJECTED_COARSE_POSITION_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_INJECTED_COARSE_POSITION_BIT;
    }
    if (::LOCATION_TECHNOLOGY_AFLT_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_AFLT_BIT;
    }
    if (::LOCATION_TECHNOLOGY_HYBRID_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_HYBRID_BIT;
    }
    if (::LOCATION_TECHNOLOGY_PPE_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_PPE_BIT;
    }
    if (::LOCATION_TECHNOLOGY_VEH_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_VEH_BIT;
    }
    if (::LOCATION_TECHNOLOGY_VIS_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_VIS_BIT;
    }
    if (::LOCATION_TECHNOLOGY_PROPAGATED_BIT & halLocation.techMask) {
        techMask |= CLIENT_DIAG_LOCATION_PROPAGATED_BIT;
    }
    location.techMask = techMask;
}

clientDiagGeofenceBreachTypeMask LocationClientApiDiag::parseGeofenceBreachType(
        ::GeofenceBreachTypeMask halBreachType) {
    clientDiagGeofenceBreachTypeMask flags = 0;
    if (GEOFENCE_BREACH_ENTER_BIT & halBreachType) {
        flags |= CLIENT_DIAG_GEOFENCE_BREACH_ENTER_BIT;
    }
    if (GEOFENCE_BREACH_EXIT_BIT & halBreachType) {
        flags |= CLIENT_DIAG_GEOFENCE_BREACH_EXIT_BIT;
    }
    if (GEOFENCE_BREACH_DWELL_IN_BIT & halBreachType) {
        flags |= CLIENT_DIAG_GEOFENCE_BREACH_DWELL_IN_BIT;
    }
    if (GEOFENCE_BREACH_DWELL_OUT_BIT & halBreachType) {
        flags |= CLIENT_DIAG_GEOFENCE_BREACH_DWELL_OUT_BIT;
    }
    return flags;
}
clientDiagGnssLocationSvUsedInPosition LocationClientApiDiag::parseDiagLocationSvUsedInPosition(
        const GnssLocationSvUsedInPosition &halSv) {

    clientDiagGnssLocationSvUsedInPosition clientSv;
    clientSv.gpsSvUsedIdsMask = halSv.gpsSvUsedIdsMask;
    clientSv.gloSvUsedIdsMask = halSv.gloSvUsedIdsMask;
    clientSv.galSvUsedIdsMask = halSv.galSvUsedIdsMask;
    clientSv.bdsSvUsedIdsMask = halSv.bdsSvUsedIdsMask;
    clientSv.qzssSvUsedIdsMask = halSv.qzssSvUsedIdsMask;
    clientSv.navicSvUsedIdsMask = halSv.navicSvUsedIdsMask;

    return clientSv;
}

void LocationClientApiDiag::translateDiagGnssSignalType(
        clientDiagGnssSignalTypeMask& out, GnssSignalTypeMask in) {

    out = (clientDiagGnssSignalTypeMask)0;
    if (in & GNSS_SIGNAL_GPS_L1CA_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L1CA;
    }
    if (in & GNSS_SIGNAL_GPS_L1C_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L1C;
    }
    if (in & GNSS_SIGNAL_GPS_L2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L2;
    }
    if (in & GNSS_SIGNAL_GPS_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L5;
    }
    if (in & GNSS_SIGNAL_GLONASS_G1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G1;
    }
    if (in & GNSS_SIGNAL_GLONASS_G2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G2;
    }
    if (in & GNSS_SIGNAL_GALILEO_E1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E1;
    }
    if (in & GNSS_SIGNAL_GALILEO_E5A_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5A;
    }
    if (in & GNSS_SIGNAL_GALILEO_E5B_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5B;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1I_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1I;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1C_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1C;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2I_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2I;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2AI_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AI;
    }
    if (in & GNSS_SIGNAL_QZSS_L1CA_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1CA;
    }
    if (in & GNSS_SIGNAL_QZSS_L1S_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1S;
    }
    if (in & GNSS_SIGNAL_QZSS_L2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L2;
    }
    if (in & GNSS_SIGNAL_QZSS_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L5;
    }
    if (in & GNSS_SIGNAL_SBAS_L1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_SBAS_L1;
    }
    if (in & GNSS_SIGNAL_NAVIC_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L5;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2AQ_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AQ;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2BI_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2BI;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2BQ_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2BQ;
    }
    if (in & GNSS_SIGNAL_NAVIC_L1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L1;
    }
}

clientDiagGnss_LocSvSystemEnumType LocationClientApiDiag::parseDiagGnssConstellation(
        Gnss_LocSvSystemEnumType gnssConstellation) {

    clientDiagGnss_LocSvSystemEnumType constellation;
    switch (gnssConstellation) {
        case GNSS_LOC_SV_SYSTEM_GPS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS;
            break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO;
            break;
        case GNSS_LOC_SV_SYSTEM_SBAS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_SBAS;
            break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS;
            break;
        case GNSS_LOC_SV_SYSTEM_BDS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS;
            break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS;
            break;
        case GNSS_LOC_SV_SYSTEM_NAVIC:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_NAVIC;
            break;
        default:
            constellation = (clientDiagGnss_LocSvSystemEnumType)~0;
            break;
    }
    return constellation;
}

void LocationClientApiDiag::translateDiagGnssMeasUsageInfo(clientDiagGnssMeasUsageInfo& out,
        const GnssMeasUsageInfo& in) {

    clientDiagGnssSignalTypeMask diagGnssSignalType;
    translateDiagGnssSignalType(diagGnssSignalType, in.gnssSignalType);
    out.gnssSignalType = diagGnssSignalType;
   /** Specifies GNSS Constellation Type */
    out.gnssConstellation = parseDiagGnssConstellation(in.gnssConstellation);
    /**  GNSS SV ID.
     For GPS:      1 to 32
     For GLONASS:  65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
     For SBAS:     120 to 151
     For QZSS-L1CA:193 to 197
     For BDS:      201 to 237
     For GAL:      301 to 336
     For NAVIC:    401 to 414 */
    out.gnssSvId = in.gnssSvId;
}

void LocationClientApiDiag::populateClientDiagLocation(
        clientDiagGnssLocationStructType* diagGnssLocPtr,
        const GnssLocation& gnssLocation,
        const DiagLocationInfoExt & diagLocationInfoExt) {

    diagGnssLocPtr->timestamp = gnssLocation.timestamp;
    diagGnssLocPtr->latitude = gnssLocation.latitude;
    diagGnssLocPtr->longitude = gnssLocation.longitude;
    diagGnssLocPtr->altitude = gnssLocation.altitude;
    diagGnssLocPtr->speed = gnssLocation.speed;
    diagGnssLocPtr->bearing = gnssLocation.bearing;
    diagGnssLocPtr->horizontalAccuracy = gnssLocation.horizontalAccuracy;
    diagGnssLocPtr->verticalAccuracy = gnssLocation.verticalAccuracy;
    diagGnssLocPtr->speedAccuracy = gnssLocation.speedAccuracy;
    diagGnssLocPtr->bearingAccuracy = gnssLocation.bearingAccuracy;
    diagGnssLocPtr->flags = gnssLocation.flags;
    diagGnssLocPtr->techMask = gnssLocation.techMask;

    diagGnssLocPtr->gnssInfoFlags = gnssLocation.gnssInfoFlags;
    diagGnssLocPtr->altitudeMeanSeaLevel = gnssLocation.altitudeMeanSeaLevel;
    diagGnssLocPtr->pdop = gnssLocation.pdop;
    diagGnssLocPtr->hdop = gnssLocation.hdop;
    diagGnssLocPtr->vdop = gnssLocation.vdop;
    diagGnssLocPtr->gdop = gnssLocation.gdop;
    diagGnssLocPtr->tdop = gnssLocation.tdop;
    diagGnssLocPtr->magneticDeviation = gnssLocation.magneticDeviation;
    diagGnssLocPtr->horReliability = (clientDiagLocationReliability)gnssLocation.horReliability;
    diagGnssLocPtr->verReliability = (clientDiagLocationReliability)gnssLocation.verReliability;
    diagGnssLocPtr->horUncEllipseSemiMajor = gnssLocation.horUncEllipseSemiMajor;
    diagGnssLocPtr->horUncEllipseSemiMinor = gnssLocation.horUncEllipseSemiMinor;
    diagGnssLocPtr->horUncEllipseOrientAzimuth = gnssLocation.horUncEllipseOrientAzimuth;
    diagGnssLocPtr->northStdDeviation = gnssLocation.northStdDeviation;
    diagGnssLocPtr->eastStdDeviation = gnssLocation.eastStdDeviation;
    diagGnssLocPtr->northVelocity = gnssLocation.northVelocity;
    diagGnssLocPtr->eastVelocity = gnssLocation.eastVelocity;
    diagGnssLocPtr->upVelocity = gnssLocation.upVelocity;
    diagGnssLocPtr->northVelocityStdDeviation = gnssLocation.northVelocityStdDeviation;
    diagGnssLocPtr->eastVelocityStdDeviation = gnssLocation.eastVelocityStdDeviation;
    diagGnssLocPtr->upVelocityStdDeviation = gnssLocation.upVelocityStdDeviation;
    diagGnssLocPtr->svUsedInPosition =
            parseDiagLocationSvUsedInPosition(gnssLocation.svUsedInPosition);
    diagGnssLocPtr->navSolutionMask = gnssLocation.navSolutionMask;
    diagGnssLocPtr->posTechMask = gnssLocation.posTechMask;
    translateDiagGnssLocationPositionDynamics(diagGnssLocPtr->bodyFrameData,
            gnssLocation.bodyFrameData);
    translateDiagSystemTime(diagGnssLocPtr->gnssSystemTime, gnssLocation.gnssSystemTime);
    diagGnssLocPtr->numOfMeasReceived = (uint8_t)gnssLocation.measUsageInfo.size();
    clientDiagGnssMeasUsageInfo measUsage;
    memset(diagGnssLocPtr->measUsageInfo, 0, sizeof(diagGnssLocPtr->measUsageInfo));
    for (int idx = 0; idx < gnssLocation.measUsageInfo.size(); idx++) {
        translateDiagGnssMeasUsageInfo(measUsage, gnssLocation.measUsageInfo[idx]);
        diagGnssLocPtr->measUsageInfo[idx] = measUsage;
    }
    diagGnssLocPtr->leapSeconds = gnssLocation.leapSeconds;
    diagGnssLocPtr->timeUncMs = gnssLocation.timeUncMs;
    diagGnssLocPtr->numSvUsedInPosition = gnssLocation.numSvUsedInPosition;
    diagGnssLocPtr->calibrationConfidencePercent = gnssLocation.calibrationConfidencePercent;
    diagGnssLocPtr->calibrationStatus = gnssLocation.calibrationStatus;
    diagGnssLocPtr->conformityIndex = gnssLocation.conformityIndex;
    diagGnssLocPtr->llaVRPBased.latitude = gnssLocation.llaVRPBased.latitude;
    diagGnssLocPtr->llaVRPBased.longitude = gnssLocation.llaVRPBased.longitude;
    diagGnssLocPtr->llaVRPBased.altitude = gnssLocation.llaVRPBased.altitude;
    diagGnssLocPtr->enuVelocityVRPBased[0] = gnssLocation.enuVelocityVRPBased[0];
    diagGnssLocPtr->enuVelocityVRPBased[1] = gnssLocation.enuVelocityVRPBased[1];
    diagGnssLocPtr->enuVelocityVRPBased[2] = gnssLocation.enuVelocityVRPBased[2];

    diagGnssLocPtr->locOutputEngType =
            (clientDiagLocOutputEngineType) gnssLocation.locOutputEngType;
    diagGnssLocPtr->locOutputEngMask =
            (clientDiagPositioningEngineMask) gnssLocation.locOutputEngMask;

    struct timespec ts = {};
    clock_gettime(CLOCK_BOOTTIME, &ts);
    diagGnssLocPtr->bootTimestampNs = (ts.tv_sec * 1000000000ULL + ts.tv_nsec);
    diagGnssLocPtr->qtimerTickCnt = getQTimerTickCount();

    diagGnssLocPtr->drSolutionStatusMask =
            (clientDiagDrSolutionStatusMask) gnssLocation.drSolutionStatusMask;
    snprintf(diagGnssLocPtr->processName, sizeof(diagGnssLocPtr->processName),
             "%s%d", getprogname(), (uint32_t)getpid());

    diagGnssLocPtr->altitudeAssumed = gnssLocation.altitudeAssumed;
    diagGnssLocPtr->sessionStatus = (clientDiagLocSessionStatus) gnssLocation.sessionStatus;

    diagGnssLocPtr->integrityRiskUsed = gnssLocation.integrityRiskUsed;
    diagGnssLocPtr->protectAlongTrack = gnssLocation.protectAlongTrack;
    diagGnssLocPtr->protectCrossTrack = gnssLocation.protectCrossTrack;
    diagGnssLocPtr->protectVertical = gnssLocation.protectVertical;

    diagGnssLocPtr->elapsedRealTimeNs = gnssLocation.elapsedRealTimeNs;
    diagGnssLocPtr->elapsedRealTimeUncNs = gnssLocation.elapsedRealTimeUncNs;

    diagGnssLocPtr->capabilitiesMask = diagLocationInfoExt.capMask;
    diagGnssLocPtr->sessionStartBootTimestampNs =
            diagLocationInfoExt.sessionStartBootTimestampNs;
    diagGnssLocPtr->reportTriggerType =
            (clientDiagLocReportTriggerType) diagLocationInfoExt.reportTriggerType;

    size_t count = gnssLocation.dgnssStationId.size();
    uint16_t i = 0;
    for (i = 0; i < (uint16_t) count && i < CLIENT_DIAG_DGNSS_STATION_ID_MAX; i++) {
        diagGnssLocPtr->dgnssStationId[i] = gnssLocation.dgnssStationId[i];
    }
    diagGnssLocPtr->numOfDgnssStationId = i;
    diagGnssLocPtr->elapsedgPTPTime = gnssLocation.elapsedgPTPTime;
    diagGnssLocPtr->elapsedgPTPTimeUnc = gnssLocation.elapsedgPTPTimeUnc;

}

static clientDiagGnssMeasurementsDataFlagsMask parseGnssMeasDataValidityFlags(
        GnssMeasurementsDataFlagsMask lcaMeasDataFlags) {

    uint32_t flags = 0;

    if (GNSS_MEASUREMENTS_DATA_SV_ID_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_ID_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_STATE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_STATE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT;
    }

    if (GNSS_MEASUREMENTS_DATA_ADR_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
    }

    if (GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT;
    }

    if (GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT;
    }
    if (GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT  & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT ;
    }
    if (GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT & lcaMeasDataFlags) {
        flags |= CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT;
    }

    return (clientDiagGnssMeasurementsDataFlagsMask)flags;
}

void LocationClientApiDiag::populateClientDiagMeasurements(
        clientDiagGnssMeasurementsStructType* diagGnssMeasPtr,
        const GnssMeasurements& gnssMeasurements) {

    uint8 adjust = 0;
    if (1 <= diagGnssMeasPtr->sequenceNumber) {
        adjust = (diagGnssMeasPtr->sequenceNumber - 1) * CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
    }
    uint32_t count = diagGnssMeasPtr->count;

    for (uint32_t idx = 0; idx < count; ++idx) {
        uint32_t gIdx = idx + adjust;
        diagGnssMeasPtr->measurements[idx].flags =
                parseGnssMeasDataValidityFlags(gnssMeasurements.measurements[gIdx].flags);
        diagGnssMeasPtr->measurements[idx].svId = gnssMeasurements.measurements[gIdx].svId;
        diagGnssMeasPtr->measurements[idx].svType =
                (clientDiagGnssSvType)gnssMeasurements.measurements[gIdx].svType;
        diagGnssMeasPtr->measurements[idx].timeOffsetNs =
                gnssMeasurements.measurements[gIdx].timeOffsetNs;
        diagGnssMeasPtr->measurements[idx].stateMask =
                (clientDiagGnssMeasurementsStateMask)gnssMeasurements.measurements[gIdx].stateMask;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeNs =
                gnssMeasurements.measurements[gIdx].receivedSvTimeNs;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeSubNs =
                gnssMeasurements.measurements[gIdx].receivedSvTimeSubNs;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeUncertaintyNs =
                gnssMeasurements.measurements[gIdx].receivedSvTimeUncertaintyNs;
        diagGnssMeasPtr->measurements[idx].carrierToNoiseDbHz =
                gnssMeasurements.measurements[gIdx].carrierToNoiseDbHz;
        diagGnssMeasPtr->measurements[idx].pseudorangeRateMps =
                gnssMeasurements.measurements[gIdx].pseudorangeRateMps;
        diagGnssMeasPtr->measurements[idx].pseudorangeRateUncertaintyMps =
                gnssMeasurements.measurements[gIdx].pseudorangeRateUncertaintyMps;
        diagGnssMeasPtr->measurements[idx].adrStateMask =
                (clientDiagGnssMeasurementsAdrStateMask)
                        gnssMeasurements.measurements[gIdx].adrStateMask;
        diagGnssMeasPtr->measurements[idx].adrMeters =
                gnssMeasurements.measurements[gIdx].adrMeters;
        diagGnssMeasPtr->measurements[idx].adrUncertaintyMeters =
                gnssMeasurements.measurements[gIdx].adrUncertaintyMeters;
        diagGnssMeasPtr->measurements[idx].carrierFrequencyHz =
                gnssMeasurements.measurements[gIdx].carrierFrequencyHz;
        diagGnssMeasPtr->measurements[idx].carrierCycles =
                gnssMeasurements.measurements[gIdx].carrierCycles;
        diagGnssMeasPtr->measurements[idx].carrierPhase =
                gnssMeasurements.measurements[gIdx].carrierPhase;
        diagGnssMeasPtr->measurements[idx].carrierPhaseUncertainty =
                gnssMeasurements.measurements[gIdx].carrierPhaseUncertainty;
        diagGnssMeasPtr->measurements[idx].multipathIndicator =
                (clientDiagGnssMeasurementsMultipathIndicator)
                        gnssMeasurements.measurements[gIdx].multipathIndicator;
        diagGnssMeasPtr->measurements[idx].signalToNoiseRatioDb =
                gnssMeasurements.measurements[gIdx].signalToNoiseRatioDb;
        diagGnssMeasPtr->measurements[idx].agcLevelDb =
                gnssMeasurements.measurements[gIdx].agcLevelDb;
        diagGnssMeasPtr->measurements[idx].basebandCarrierToNoiseDbHz =
                gnssMeasurements.measurements[gIdx].basebandCarrierToNoiseDbHz;
        clientDiagGnssSignalTypeMask diagGnssSignalType;
        translateDiagGnssSignalType(diagGnssSignalType,
                gnssMeasurements.measurements[gIdx].gnssSignalType);
        diagGnssMeasPtr->measurements[idx].gnssSignalType = diagGnssSignalType;
        diagGnssMeasPtr->measurements[idx].fullInterSignalBiasNs =
                gnssMeasurements.measurements[gIdx].fullInterSignalBiasNs;
        diagGnssMeasPtr->measurements[idx].fullInterSignalBiasUncertaintyNs =
                gnssMeasurements.measurements[gIdx].fullInterSignalBiasUncertaintyNs;
    }
}

void LocationClientApiDiag::translateDiagGnssSv(clientDiagGnssSv& out, const GnssSv& in) {

    /** Unique Identifier */
    out.svId = in.svId;
    /** type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO, NAVIC) */
    out.type = (clientDiagGnssSvType)in.type;
    /** signal strength */
    out.cN0Dbhz = in.cN0Dbhz;
    /** elevation of SV (in degrees) */
    out.elevation = in.elevation;
    /** azimuth of SV (in degrees) */
    out.azimuth = in.azimuth;
    /** Bitwise OR of GnssSvOptionsBits */
    out.gnssSvOptionsMask = in.gnssSvOptionsMask;
    /** carrier frequency of SV (in Hz) */
    out.carrierFrequencyHz = in.carrierFrequencyHz;
    /** Bitwise OR of clientDiagGnssSignalTypeBits */
    clientDiagGnssSignalTypeMask diagGnssSignalType;
    translateDiagGnssSignalType(diagGnssSignalType, in.gnssSignalTypeMask);
    out.gnssSignalTypeMask = diagGnssSignalType;
    /** baseband signal strength */
    out.basebandCarrierToNoiseDbHz = in.basebandCarrierToNoiseDbHz;
    /** glonass frequency number */
    out.gloFrequency = in.gloFrequency;
}

void LocationClientApiDiag::translateDiagGeofenceData(clientDiagGeofenceData& out,
        const Geofence& in) {
    out.latitude = in.getLatitude();
    out.longitude = in.getLongitude();
    out.radius = in.getRadius();
    out.breachType = parseGeofenceBreachType((::GeofenceBreachTypeMask)in.getBreachType());
    out.responsiveness = in.getResponsiveness();
    out.dwellTime = in.getDwellTime();
}

void LocationClientApiDiag::populateClientDiagGnssSv(clientDiagGnssSvStructType* diagGnssSvPtr,
        const std::vector<GnssSv>& gnssSvs) {

    clientDiagGnssSv diagGnssSv;
    diagGnssSvPtr->count = gnssSvs.size();
    for (int idx = 0; idx < gnssSvs.size(); ++idx) {
        translateDiagGnssSv(diagGnssSv, gnssSvs[idx]);
        diagGnssSvPtr->gnssSvs[idx] = diagGnssSv;
    }
}

void LocationClientApiDiag::populateClientDiagGeofenceBreach(
        clientDiagGeofenceBreachStructType* diagGeofenceBreachPtr,
        const GeofenceBreachNotification& gfBreachNotif, const std::vector<Geofence>& geofences) {
    clientDiagGeofenceData diagGeofenceData;
    diagGeofenceBreachPtr->count = geofences.size();
    parseLocation(gfBreachNotif.location, diagGeofenceBreachPtr->location);
    diagGeofenceBreachPtr->type = parseGeofenceBreachType(gfBreachNotif.type);
    diagGeofenceBreachPtr->timestamp = gfBreachNotif.timestamp;
    for (int idx = 0; idx < geofences.size(); ++idx) {
        translateDiagGeofenceData(diagGeofenceData, geofences[idx]);
        diagGeofenceBreachPtr->geofences[idx] = diagGeofenceData;
    }
}

void LocationClientApiDiag::populateCommanGnssEph(clientDiagGnssEphCommonStruct& out,
            const GnssEphCommonInfo& in)
{

    out.gnssSvId     = in.gnssSvId;
    out.ephSource    = (clientDiagGnssEphSource)in.ephSource;
    out.ephAction    = (clientDiagGnssEphAction)in.action;
    out.IODE         = in.IODE;
    out.aSqrt        = in.aSqrt;
    out.deltaN       = in.deltaN;
    out.m0           = in.m0;
    out.eccentricity = in.eccentricity;
    out.omega0       = in.omega0;
    out.i0           = in.i0;
    out.omega        = in.omega;
    out.omegaDot     = in.omegaDot;
    out.iDot         = in.iDot;
    out.cUc          = in.cUc;
    out.cUs          = in.cUs;
    out.cRc          = in.cRc;
    out.cRs          = in.cRs;
    out.cIc          = in.cIc;
    out.cIs          = in.cIs;
    out.toe          = in.toe;
    out.toc          = in.toc;
    out.af0          = in.af0;
    out.af1          = in.af1;
    out.af2          = in.af2;
}

void LocationClientApiDiag::populateGpsEph(clientDiagGpsQzssEphemerisStruct& out,
        const GpsQzssEphemeris& in) {

    populateCommanGnssEph(out.commonEphemerisData, in.commonEphemerisData);

    out.signalHealth  = in.signalHealth;
    out.URAI          = in.URAI;
    out.codeL2        = in.codeL2;
    out.dataFlagL2P   = in.dataFlagL2P;
    out.fitInterval   = in.fitInterval;
    out.IODC          = in.IODC;
    out.tgd           = in.tgd;
}

void LocationClientApiDiag::populateGalEph(clientDiagGalEphemerisStruct& out,
        const GalileoEphemeris& in) {

    populateCommanGnssEph(out.commonEphemerisData, in.commonEphemerisData);

    out.dataSourceSignal  = (clientDiagGalEphSignalSource)in.dataSourceSignal;
    out.sisIndex          = in.sisIndex;
    out.bgdE1E5a          = in.bgdE1E5a;
    out.bgdE1E5b          = in.bgdE1E5b;
    out.svHealth          = in.svHealth;
}

void LocationClientApiDiag::populateGloEph(clientDiagGloEphemerisStruct& out,
        const GlonassEphemeris& in) {

    out.gnssSvId         = in.gnssSvId;
    out.ephSource        = (clientDiagGnssEphSource)in.ephSource;
    out.ephAction        = (clientDiagGnssEphAction)in.action;
    out.bnHealth         = in.bnHealth;
    out.lnHealth         = in.lnHealth;
    out.tb               = in.tb;
    out.ft               = in.ft;
    out.gloM             = in.gloM;
    out.enAge            = in.enAge;
    out.gloFrequency     = in.gloFrequency;
    out.p1               = in.p1;
    out.p2               = in.p2;
    out.deltaTau         = in.deltaTau;

    out.position[0]      = in.position[0];
    out.position[1]      = in.position[1];
    out.position[2]      = in.position[2];
    out.velocity[0]      = in.velocity[0];
    out.velocity[1]      = in.velocity[1];
    out.velocity[2]      = in.velocity[2];
    out.acceleration[0]  = in.acceleration[0];
    out.acceleration[1]  = in.acceleration[1];
    out.acceleration[2]  = in.acceleration[2];

    out.tauN             = in.tauN;
    out.gamma            = in.gamma;
    out.toe              = in.toe;
    out.nt               = in.nt;
}

void LocationClientApiDiag::populateBdsEph(clientDiagBdsEphemerisStruct& out,
            const BdsEphemeris& in) {

    populateCommanGnssEph(out.commonEphemerisData, in.commonEphemerisData);

    out.svHealth    = in.svHealth;
    out.AODC        = in.AODC;
    out.tgd1        = in.tgd1;
    out.tgd2        = in.tgd2;
    out.URAI        = in.URAI;
}

void LocationClientApiDiag::populateQzssEph(clientDiagQzssEphemerisStruct& out,
            const QzssEphemeris& in) {
    populateGpsEph(out.qzssEphData, in.qzssEphData);
}

void LocationClientApiDiag::populateNavicEph(clientDiagNavicEphemerisStruct& out,
        const NavicEphemeris& in) {

    populateCommanGnssEph(out.commonEphemerisData, in.commonEphemerisData);

    out.weekNum             = in.weekNum;
    out.iodec               = in.iodec;
    out.l5Health            = in.l5Health;
    out.sHealth             = in.sHealth;
    out.inclinationAngleRad = in.inclinationAngleRad;
    out.urai                = in.urai;
    out.tgd                 = in.tgd;
}

void LocationClientApiDiag::populateEphemerisReport
(
    clientDiagGnssEphReportStructType* diagEphReport,
    const GnssEphemeris& ephReport
) {

    diagEphReport->gnssConstellation =
            parseDiagGnssConstellation(ephReport.gnssConstellation);
    diagEphReport->isSystemTimeValid = ephReport.isSystemTimeValid;
    if (diagEphReport->isSystemTimeValid) {
        diagEphReport->systemTime = parseDiagGnssTime(ephReport.systemTime);
    }

    switch (diagEphReport->gnssConstellation) {
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS:
            diagEphReport->numberOfEphemeris = std::min(ephReport.gpsEphemerisData.size(),
                    (size_t)CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE);
            for (uint8_t idx = 0; idx < diagEphReport->numberOfEphemeris; idx++) {
                clientDiagGpsQzssEphemerisStruct eph = {};
                populateGpsEph(eph, ephReport.gpsEphemerisData[idx]);
                diagEphReport->ephInfo.gpsEphemerisData[idx] = eph;
            }
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO:
            diagEphReport->numberOfEphemeris = std::min(ephReport.galEphemerisData.size(),
                    (size_t)CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE);
            for (uint8_t idx = 0; idx < diagEphReport->numberOfEphemeris; idx++) {
                clientDiagGalEphemerisStruct eph = {};
                populateGalEph(eph, ephReport.galEphemerisData[idx]);
                diagEphReport->ephInfo.galEphemerisData[idx] = eph;
            }
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS:
            diagEphReport->numberOfEphemeris = std::min(ephReport.gloEphemerisData.size(),
                    (size_t)CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE);
            for (uint8_t idx = 0; idx < diagEphReport->numberOfEphemeris; idx++) {
                clientDiagGloEphemerisStruct eph = {};
                populateGloEph(eph, ephReport.gloEphemerisData[idx]);
                diagEphReport->ephInfo.gloEphemerisData[idx] = eph;
            }
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS:
            diagEphReport->numberOfEphemeris = std::min(ephReport.bdsEphemerisData.size(),
                    (size_t)CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE);
            for (uint8_t idx = 0; idx < diagEphReport->numberOfEphemeris; idx++) {
                clientDiagBdsEphemerisStruct eph = {};
                populateBdsEph(eph, ephReport.bdsEphemerisData[idx]);
                diagEphReport->ephInfo.bdsEphemerisData[idx] = eph;
            }
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS:
            diagEphReport->numberOfEphemeris = std::min(ephReport.qzssEphemerisData.size(),
                    (size_t)CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE);
            for (uint8_t idx = 0; idx < diagEphReport->numberOfEphemeris; idx++) {
                clientDiagQzssEphemerisStruct eph = {};
                populateQzssEph(eph, ephReport.qzssEphemerisData[idx]);
                diagEphReport->ephInfo.qzssEphemerisData[idx] = eph;
            }
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_NAVIC:
            diagEphReport->numberOfEphemeris = std::min(ephReport.navicEphemerisData.size(),
                    (size_t)CLIENT_DIAG_GNSS_EPH_LIST_MAX_SIZE);
            for (uint8_t idx = 0; idx < diagEphReport->numberOfEphemeris; idx++) {
                clientDiagNavicEphemerisStruct eph = {};
                populateNavicEph(eph, ephReport.navicEphemerisData[idx]);
                diagEphReport->ephInfo.navicEphemerisData[idx] = eph;
            }
            break;
        default:
            LOC_LOGe(" Unknown GNSS system ");
            break;
    }
}

void GnssDiagEphReport::log() {
    if (!mDiagIface) {
        return;
    }
    LOC_LOGv("GnssDiagEphReport::log");

    diagBuffSrc bufferSrc = BUFFER_INVALID;
    size_t size = sizeof(log_hdr_type) + 2*sizeof(uint8_t) +
                    sizeof(clientDiagGnss_LocSvSystemEnumType) + sizeof(bool) +
                    sizeof(clientDiagGnssSystemTimeStructType);
    switch (parseDiagGnssConstellation(mGnssEphReport.gnssConstellation)) {
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS:
            size += sizeof(clientDiagGpsQzssEphemerisStruct)*(mGnssEphReport.
                            gpsEphemerisData.size());
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO:
            size += sizeof(clientDiagGalEphemerisStruct)*(mGnssEphReport.galEphemerisData.size());
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS:
            size += sizeof(clientDiagBdsEphemerisStruct)*(mGnssEphReport.bdsEphemerisData.size());
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS:
            size += sizeof(clientDiagQzssEphemerisStruct)*(mGnssEphReport.
                            qzssEphemerisData.size());
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_NAVIC:
            size += sizeof(clientDiagNavicEphemerisStruct)*(mGnssEphReport.
                            navicEphemerisData.size());
            break;
        case CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS:
            size += sizeof(clientDiagGloEphemerisStruct)*(mGnssEphReport.gloEphemerisData.size());
            break;
        default:
            size = 0;
            LOC_LOGe(" Unknown GNSS system ");
            break;
    }
    if (0 == size) {
        LOC_LOGe(" Size to allocate is 0 ");
        return;
    }
    clientDiagGnssEphReportStructType* diagGnssEphReportPtr = (clientDiagGnssEphReportStructType*)
            mDiagIface->logAlloc(LOG_GNSS_CLIENT_API_EPH_REPORT_C,
                                 size,
                                 &bufferSrc);
    if (NULL == diagGnssEphReportPtr) {
        LOC_LOGe("diag memory alloc failed");
        return;
    }

    diagGnssEphReportPtr->version = LOG_CLIENT_EPH_DIAG_MSG_VERSION;

    populateEphemerisReport(diagGnssEphReportPtr, mGnssEphReport);

    mDiagIface->logCommit(diagGnssEphReportPtr, bufferSrc,
                          LOG_GNSS_CLIENT_API_EPH_REPORT_C,
                          sizeof(clientDiagGnssEphReportStructType));
}

extern "C" {
void LogGnssLocation(const GnssLocation& gnssLocation,
                     const DiagLocationInfoExt & diagLocationInfoExt) {
    GnssLocationReport report(gnssLocation, diagLocationInfoExt);
    report.log();
}

void LogGnssSv(const std::vector<GnssSv>& gnssSvsVector) {
    GnssSvReport report(gnssSvsVector);
    report.log();
}

void LogGnssNmea(uint64_t timestamp, uint32_t length, const char* nmea,
        LocOutputEngineType engType) {
    GnssNmeaReport report(timestamp, length, nmea, engType);
    report.log();
}

void LogGnssMeas(const GnssMeasurements& gnssMeasurements) {
    GnssMeasReport report(gnssMeasurements);
    report.log();
}

void LogGnssLatency(const GnssLatencyInfo& gnssLatencyInfo) {
    GnssLatencyReport report(gnssLatencyInfo);
    report.log();
}

void LogGnssDcReport(const GnssDcReport& gnssDcReport) {
    GnssDiagDcReport report(gnssDcReport);
    report.log();
}

void LogGeofenceBreach(const GeofenceBreachNotification& breachNotif,
        const std::vector<Geofence>& geofences) {
    GeofenceBreachDiagReport report(breachNotif, geofences);
    report.log();
}

void LogGnssEphemeris(const GnssEphemeris& ephInfo) {
    GnssDiagEphReport report(ephInfo);
    report.log();
}

bool LogGnssF3Init(void) {
    uint32_t loadDiagIfaceLib = 1;
    const loc_param_s_type loc_param_table[] =
    {
        { "LOC_DIAGIFACE_ENABLED",   &loadDiagIfaceLib,   NULL, 'n' },
    };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, loc_param_table);
    return (loadDiagIfaceLib && Diag_LSM_Init(NULL));
}

#undef MSG_BUILD_MASK_MSG_SSID_GNSS_HLOS
#define MSG_BUILD_MASK_MSG_SSID_GNSS_HLOS \
        (MSG_LVL_LOW | MSG_LVL_MED | MSG_LVL_HIGH | MSG_LVL_ERROR | MSG_LVL_FATAL)

void LogGnssF3(uint32_t level, char *buf) {

    switch (level) {
    case MSG_QXDM_LOW:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_LOW, "%s", buf);
        break;
    case MSG_QXDM_MED:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_MED, "%s", buf);
        break;
    case MSG_QXDM_HIGH:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_HIGH, "%s", buf);
        break;
    case MSG_QXDM_ERROR:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_ERROR, "%s", buf);
        break;
    default:
        break;
    }
}

}

}
