/*=============================================================================
  @file qsh_location_test.cpp

  Location test application in C++. This application will open a QMI connection to
  the Sensors Service (which resides on the SSC).  It will send a SUID Lookup
  Request for the data type "location".

  Copyright (c) 2016-2018, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <iostream>
#include <cinttypes>
#include <unistd.h>
#include "qsh_utils_suid_util.h"
#include <string>
#include <vector>
#include <inttypes.h>
#include <android/log.h>

#include "sns_std_sensor.pb.h"
#include "sns_std_type.pb.h"
#include "sns_client.pb.h"

#include "qsh_location.pb.h"
#include <loc_pla.h>
#include <loc_cfg.h>
#include <loc_log.h>
#include <log_util.h>
using namespace std;

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#undef LOG_NDEBUG
#define LOG_NDEBUG 0

#undef LOG_TAG
#define LOG_TAG "QSH_LOCATION_TEST"

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define SENSOR_NAME "location"

#define OPEN_REQUEST            0
#define LOCATION_START_REQUEST  1
#define LOCATION_STOP_REQUEST   2
#define DATA_START_REQUEST      3
#define DATA_STOP_REQUEST       4
#define QUIT_REQUEST            5
#define BUFFERSIZE 50

#define NO_OF_RETRIES 5

/*=============================================================================
  Static Data
  ===========================================================================*/

static ssc_interface *connection = nullptr;
static sensor_uid mySuid;
static bool isInitialized = false;

// Timer related
pid_t pid;
int signum;
struct timespec timeout;
sigset_t newmask;
siginfo_t info;
bool bVerboseMode = true;

#define GNSS_MAX_MEASUREMENT  128

typedef struct {
    int32_t sv_id;
    qsh_location_constellation_type constellation_type;
    int64_t time_offset;
    uint32_t state;
    int64_t received_sv_time;
    int64_t received_sv_time_uncertainty;
    float c_n0;
    float pseudorange_rate;
    float pseudorange_rate_uncertainty;
    bool has_carrier_phase;
    double carrier_phase;
    bool has_carrier_phase_uncertainty;
    float carrier_phase_uncertainty;
    bool has_cycle_slip_count;
    uint32_t cycle_slip_count;
    bool has_multipath_indicator;
    qsh_location_multipath_indicator multipath_indicator;
    bool has_snr;
    float snr;
    bool has_carrier_frequency;
    float carrier_frequency;
} qsh_measurements;

typedef struct {
    int64_t time;
    bool has_time_uncertainty;
    float time_uncertainty;
    bool has_full_bias;
    int64_t full_bias;
    bool has_bias;
    float bias;
    bool has_bias_uncertainty;
    float bias_uncertainty;
    bool has_drift;
    float drift;
    bool has_drift_uncertainty;
    float drift_uncertainty;
    bool has_hw_clock_discontinuity_count;
    uint32_t hw_clock_discontinuity_count;
} qsh_clock;

typedef struct {
    uint32_t numberOfMeasurements;
    qsh_clock clock;
    qsh_measurements measurements[GNSS_MAX_MEASUREMENT];
} qsh_location_meas_and_clk;

qsh_location_meas_and_clk allMeasAndClk;

static int measIndex = 0;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

static void send_open_req(ssc_interface *conn, sensor_uid suid);
static void send_close_req(ssc_interface *conn, sensor_uid suid);
static void send_update_req(ssc_interface *conn, sensor_uid suid, bool start,
                            qsh_location_request request, uint32_t interval);

static void consume_meas_and_clk() {
    uint32_t i;

    if (bVerboseMode) {
        printf("clock: t=0x%" PRIx64" tu=%.2f fb=%" PRId64" b=%.2f bu=%.2f d=%.2f du=%.2f hd=%d\n",
            allMeasAndClk.clock.time, allMeasAndClk.clock.time_uncertainty,
            allMeasAndClk.clock.full_bias, allMeasAndClk.clock.bias,
            allMeasAndClk.clock.bias_uncertainty, allMeasAndClk.clock.drift,
            allMeasAndClk.clock.drift_uncertainty,
            allMeasAndClk.clock.hw_clock_discontinuity_count);
        for (i = 0; i < allMeasAndClk.numberOfMeasurements; i++) {
            printf("meas[%d] sv=%d cs=%d to=%" PRId64" st=0x%X rt=%" PRId64" ru=%" PRId64" cn=%.2f"
                " pr=%.2f pu=%.2f cf=%.2f cu=%.2f cs=%d mi=%d sn=%.2f fr=%.2f\n",
                i, allMeasAndClk.measurements[i].sv_id,
                allMeasAndClk.measurements[i].constellation_type,
                allMeasAndClk.measurements[i].time_offset, allMeasAndClk.measurements[i].state,
                allMeasAndClk.measurements[i].received_sv_time,
                allMeasAndClk.measurements[i].received_sv_time_uncertainty,
                allMeasAndClk.measurements[i].c_n0, allMeasAndClk.measurements[i].pseudorange_rate,
                allMeasAndClk.measurements[i].pseudorange_rate_uncertainty,
                allMeasAndClk.measurements[i].carrier_phase,
                allMeasAndClk.measurements[i].carrier_phase_uncertainty,
                allMeasAndClk.measurements[i].cycle_slip_count,
                allMeasAndClk.measurements[i].multipath_indicator,
                allMeasAndClk.measurements[i].snr,
                allMeasAndClk.measurements[i].carrier_frequency);
        }
    }
    LOC_LOGd("clock: t=0x%" PRIx64" tu=%.2f fb=%" PRId64" b=%.2f bu=%.2f d=%.2f du=%.2f hd=%d",
             allMeasAndClk.clock.time, allMeasAndClk.clock.time_uncertainty,
             allMeasAndClk.clock.full_bias, allMeasAndClk.clock.bias,
             allMeasAndClk.clock.bias_uncertainty, allMeasAndClk.clock.drift,
             allMeasAndClk.clock.drift_uncertainty,
             allMeasAndClk.clock.hw_clock_discontinuity_count);

    for (i = 0; i < allMeasAndClk.numberOfMeasurements; i++) {
        LOC_LOGd("meas[%d] sv=%d cs=%d to=%" PRId64" st=0x%X rt=%" PRId64" ru=%" PRId64" cn=%.2f"
                 " pr=%.2f pu=%.2f cf=%.2f cu=%.2f cs=%d mi=%d sn=%.2f fr=%.2f",
                 i, allMeasAndClk.measurements[i].sv_id,
                 allMeasAndClk.measurements[i].constellation_type,
                 allMeasAndClk.measurements[i].time_offset, allMeasAndClk.measurements[i].state,
                 allMeasAndClk.measurements[i].received_sv_time,
                 allMeasAndClk.measurements[i].received_sv_time_uncertainty,
                 allMeasAndClk.measurements[i].c_n0,
                 allMeasAndClk.measurements[i].pseudorange_rate,
                 allMeasAndClk.measurements[i].pseudorange_rate_uncertainty,
                 allMeasAndClk.measurements[i].carrier_phase,
                 allMeasAndClk.measurements[i].carrier_phase_uncertainty,
                 allMeasAndClk.measurements[i].cycle_slip_count,
                 allMeasAndClk.measurements[i].multipath_indicator,
                 allMeasAndClk.measurements[i].snr,
                 allMeasAndClk.measurements[i].carrier_frequency);
    }
}

static void parse_meas_and_clk(qsh_location_meas_and_clk_event* pMeasClkEvent) {
    int idx, tempIndex, tempSize, tempNoOfMeas;
    qsh_location_measurement tempMeas;
    qsh_location_clock tempClock;

    tempIndex = pMeasClkEvent->measurement_index();
    tempSize = pMeasClkEvent->measurements_size();
    tempNoOfMeas = pMeasClkEvent->number_of_measurements();
    LOC_LOGd("Received location meas and clock event size=%d noOfMeas=%d idx=%d",
             tempSize, tempNoOfMeas, tempIndex);

    if (1 == tempIndex) {
        memset(allMeasAndClk.measurements, 0,
               GNSS_MAX_MEASUREMENT*sizeof(qsh_measurements));
        allMeasAndClk.numberOfMeasurements = tempNoOfMeas;
        tempClock = pMeasClkEvent->clock();
        // get the clock information
        allMeasAndClk.clock.time = tempClock.time();
        allMeasAndClk.clock.has_time_uncertainty = tempClock.has_time_uncertainty();
        allMeasAndClk.clock.time_uncertainty = tempClock.time_uncertainty();
        allMeasAndClk.clock.has_full_bias = tempClock.has_full_bias();
        allMeasAndClk.clock.full_bias = tempClock.full_bias();
        allMeasAndClk.clock.has_bias = tempClock.has_bias();
        allMeasAndClk.clock.bias = tempClock.bias();
        allMeasAndClk.clock.has_bias_uncertainty = tempClock.has_bias_uncertainty();
        allMeasAndClk.clock.bias_uncertainty = tempClock.bias_uncertainty();
        allMeasAndClk.clock.has_drift = tempClock.has_drift();
        allMeasAndClk.clock.drift = tempClock.drift();
        allMeasAndClk.clock.has_drift_uncertainty = tempClock.has_drift_uncertainty();
        allMeasAndClk.clock.drift_uncertainty = tempClock.drift_uncertainty();
        allMeasAndClk.clock.has_hw_clock_discontinuity_count =
                tempClock.has_hw_clock_discontinuity_count();
        allMeasAndClk.clock.hw_clock_discontinuity_count =
                tempClock.hw_clock_discontinuity_count();
        measIndex = 0;
    }

    for (idx = 0; idx < tempSize; idx++) {
        tempMeas  = pMeasClkEvent->measurements(idx);
        allMeasAndClk.measurements[measIndex + idx].sv_id = tempMeas.sv_id();
        allMeasAndClk.measurements[measIndex + idx].constellation_type =
                tempMeas.constellation_type();
        allMeasAndClk.measurements[measIndex + idx].time_offset = tempMeas.time_offset();
        allMeasAndClk.measurements[measIndex + idx].state = tempMeas.state();
        allMeasAndClk.measurements[measIndex + idx].received_sv_time = tempMeas.received_sv_time();
        allMeasAndClk.measurements[measIndex + idx].received_sv_time_uncertainty =
                tempMeas.received_sv_time_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].c_n0 = tempMeas.c_n0();
        allMeasAndClk.measurements[measIndex + idx].pseudorange_rate = tempMeas.pseudorange_rate();
        allMeasAndClk.measurements[measIndex + idx].pseudorange_rate_uncertainty =
                tempMeas.pseudorange_rate_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].has_carrier_phase =
                tempMeas.has_carrier_phase();
        allMeasAndClk.measurements[measIndex + idx].carrier_phase = tempMeas.carrier_phase();
        allMeasAndClk.measurements[measIndex + idx].has_carrier_phase_uncertainty =
                tempMeas.has_carrier_phase_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].carrier_phase_uncertainty =
                tempMeas.carrier_phase_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].has_cycle_slip_count =
                tempMeas.has_cycle_slip_count();
        allMeasAndClk.measurements[measIndex + idx].cycle_slip_count = tempMeas.cycle_slip_count();
        allMeasAndClk.measurements[measIndex + idx].has_multipath_indicator =
                tempMeas.has_multipath_indicator();
        allMeasAndClk.measurements[measIndex + idx].multipath_indicator =
                tempMeas.multipath_indicator();
        allMeasAndClk.measurements[measIndex + idx].has_snr = tempMeas.has_snr();
        allMeasAndClk.measurements[measIndex + idx].snr = tempMeas.snr();
        allMeasAndClk.measurements[measIndex + idx].has_carrier_frequency =
                tempMeas.has_carrier_frequency();
        allMeasAndClk.measurements[measIndex + idx].carrier_frequency =
                tempMeas.carrier_frequency();
    }
    measIndex += tempSize;

    if (measIndex == tempNoOfMeas) {
        consume_meas_and_clk();
    }
}

/**
 * Event callback function, as registered with ssc_interface.
 */
static void event_cb(const uint8_t *data, size_t size, uint64_t ts) {
    sns_client_event_msg pb_event_msg;
    UNUSED_VAR(ts);

    union sigval sig_value;

    LOC_LOGd("Received QMI indication with length %zu", size);

    if (nullptr == data) {
        LOC_LOGe("data is nullptr!");
        return;
    }

    sig_value.sival_int = 1;

    pb_event_msg.ParseFromArray(data, size);
    for (int i = 0; i < pb_event_msg.events_size(); i++) {
        const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
        LOC_LOGd("Event[%i] msg_id=%i, ts=%llu", i, pb_event.msg_id(),
                 (unsigned long long)pb_event.timestamp());

        if (SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id()) {
            sns_std_error_event error;
            error.ParseFromString(pb_event.payload());

            LOC_LOGe("Received error event %i", error.error());
        } else if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == pb_event.msg_id()) {
            sns_std_attr_event attr_event;
            int32_t attr_id;

            attr_event.ParseFromString(pb_event.payload());
            attr_id = attr_event.attributes(0).attr_id();
            LOC_LOGd("Received attr_event attr_id %d", attr_id);
        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_ACK == pb_event.msg_id()) {
            qsh_location_ack ack;
            ack.ParseFromString(pb_event.payload());

            LOC_LOGd("Received location ack err=%d cmd=%d "
                     "version=0x%X capabilities=0x%X",
                     ack.error(), ack.command(),
                     ack.version(), ack.capabilities());
            sigqueue(pid, signum, sig_value);

        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_POSITION_EVENT == pb_event.msg_id()) {
            qsh_location_position_event posEvent;
            posEvent.ParseFromString(pb_event.payload());

            if (bVerboseMode) {
                printf("Received position event\n");
                printf("clock: t=0x%" PRIx64" lat=%d lon=%d alt=%.2f hacc=%.2f vacc=%.2f"
                        " sp=%.2f sacc=%.2f b=%.2f bacc=%.2f ci=%.2f\n",
                        posEvent.timestamp(), posEvent.latitude(), posEvent.longitude(),
                        posEvent.altitude(), posEvent.horizontal_accuracy(),
                        posEvent.vertical_accuracy(),
                        posEvent.speed(), posEvent.speed_accuracy(), posEvent.bearing(),
                        posEvent.bearing_accuracy(), posEvent.conformity_index());
            }
            LOC_LOGd("timestamp=%" PRIu64" latitude=%d",
                     posEvent.timestamp(),
                     posEvent.latitude());
            LOC_LOGd("longitude=%d altitude=%f",
                     posEvent.longitude(),
                     posEvent.altitude());
            LOC_LOGd("horizontal_accuracy=%f vertical_accuracy=%f",
                     posEvent.horizontal_accuracy(),
                     posEvent.vertical_accuracy());
            LOC_LOGd("speed=%f speed_accuracy=%f",
                     posEvent.speed(),
                     posEvent.speed_accuracy());
            LOC_LOGd("bearing=%f bearing_accuracy=%f",
                     posEvent.bearing(),
                     posEvent.bearing_accuracy());
            LOC_LOGd("conformity_index=%f", posEvent.conformity_index());
        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_MEAS_AND_CLK_EVENT == pb_event.msg_id()) {
            qsh_location_meas_and_clk_event measClkEvent;
            measClkEvent.ParseFromString(pb_event.payload());
            parse_meas_and_clk(&measClkEvent);
        } else {
            LOC_LOGe("Received unknown message ID %i", pb_event.msg_id());
        }
    }
}

/**
* Send an location open request for the specified SUID.
*/
static void send_open_req(ssc_interface *conn, sensor_uid suid) {
    string pb_req_msg_encoded;
    string location_open_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_location_open location_open;

    LOC_LOGd("Send location open request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    location_open.set_version(0x01000000);
    location_open.SerializeToString(&location_open_encoded);

    pb_req_msg.set_msg_id(QSH_LOCATION_MSGID_QSH_LOCATION_OPEN);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(location_open_encoded);
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(suid.high);
        pStdSuid->set_suid_low(suid.low);
    } else {
        pb_req_msg.clear_request();
        LOC_LOGe("mutable_suid failed!");
        return;
    }
    sns_client_request_msg_suspend_config* pSuspendCfg = pb_req_msg.mutable_susp_config();
    if (pSuspendCfg) {
        pSuspendCfg->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
        pSuspendCfg->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    } else {
        pb_req_msg.clear_request();
        pb_req_msg.clear_suid();
        LOC_LOGe("mutable_susp_config failed!");
        return;
    }

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(suid, true, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
* Send an location close request for the specified SUID.
*/
static void send_close_req(ssc_interface *conn, sensor_uid suid) {
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    LOC_LOGd("Send location close request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    pb_req_msg.set_msg_id(QSH_LOCATION_MSGID_QSH_LOCATION_CLOSE);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->clear_payload();
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(suid.high);
        pStdSuid->set_suid_low(suid.low);
    } else {
        pb_req_msg.clear_request();
        LOC_LOGe("mutable_suid failed!");
        return;
    }
    sns_client_request_msg_suspend_config* pSuspendCfg = pb_req_msg.mutable_susp_config();
    if (pSuspendCfg) {
        pSuspendCfg->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
        pSuspendCfg->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    } else {
        pb_req_msg.clear_request();
        pb_req_msg.clear_suid();
        LOC_LOGe("mutable_susp_config failed!");
        return;
    }

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(suid, true, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
* Send an location update request for the specified SUID.
*/
static void send_update_req(ssc_interface *conn, sensor_uid suid, bool start,
                            qsh_location_request request, uint32_t interval) {
    string pb_req_msg_encoded;
    string location_update_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_location_update location_update;

    LOC_LOGd("Send location update request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    location_update.set_start(start);
    location_update.set_location_request(request);
    location_update.set_interval(interval);
    location_update.SerializeToString(&location_update_encoded);

    pb_req_msg.set_msg_id(QSH_LOCATION_MSGID_QSH_LOCATION_UPDATE);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(location_update_encoded);
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(suid.high);
        pStdSuid->set_suid_low(suid.low);
    } else {
        pb_req_msg.clear_request();
        LOC_LOGe("mutable_suid failed!");
        return;
    }
    sns_client_request_msg_suspend_config* pSuspendCfg = pb_req_msg.mutable_susp_config();
    if (pSuspendCfg) {
        pSuspendCfg->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
        pSuspendCfg->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    } else {
        pb_req_msg.clear_request();
        pb_req_msg.clear_suid();
        LOC_LOGe("mutable_susp_config failed!");
        return;
    }

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(suid, true, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
* Send an attribute request for the specified SUID.
*/
static void send_attr_req(ssc_interface *conn, sensor_uid suid) {
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    mySuid = suid;
    LOC_LOGd("Sending attr request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    pb_req_msg.set_msg_id(SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->clear_payload();
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(suid.high);
        pStdSuid->set_suid_low(suid.low);
    } else {
        pb_req_msg.clear_request();
        LOC_LOGe("mutable_suid failed!");
        return;
    }
    sns_client_request_msg_suspend_config* pSuspendCfg = pb_req_msg.mutable_susp_config();
    if (pSuspendCfg) {
        pSuspendCfg->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
        pSuspendCfg->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    } else {
        pb_req_msg.clear_request();
        pb_req_msg.clear_suid();
        LOC_LOGe("mutable_susp_config failed!");
        return;
    }

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(suid, true, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
 * SUID callback as registered with suid_lookup.
 */
static void suid_cb(const std::string& datatype, const std::vector<sensor_uid>& suids) {

    LOC_LOGd("Received SUID event with length %zu", suids.size());
    if (suids.size() > 0) {
        sensor_uid suid = suids.at(0);

        ssc_conn_config conn_config;
        connection = ssc_interface::create(SSC_QMI, conn_config);
        if (nullptr != connection) {
            connection->register_cb(suids[0], nullptr, nullptr, event_cb);

            LOC_LOGd("Received SUID %" PRIx64 "%" PRIx64 " for '%s'",
                     suid.high, suid.low, datatype.c_str());

            send_attr_req(connection, suid);
        } else {
            LOC_LOGe("connection is nullptr");
        }
    } else {
        LOC_LOGd("%s sensor is not available", SENSOR_NAME);
        exit(-1);
    }
}

int wait_for_ack(uint8_t request) {
    int ret = -1;
    sigset_t oldmask;

    pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);
    LOC_LOGd("Wait for ack request=%d pid=%d", request, pid);
    while (true) {
        ret = sigtimedwait(&newmask, &info, &timeout);
        if (-1 == ret) {
            LOC_LOGw("Timeout waiting for ack errno=%d", errno);
            if (EAGAIN == errno) {
                break;
            }
            else if (EINTR == errno) {
                continue;
            }
            LOC_LOGw("Timeout waiting for ack errno=%d", errno);
            break;
        }
        if (signum == info.si_signo) {
            LOC_LOGd("Got signal from ACK");
            break;
        }
    }
    pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
    return ret;
}

int test_loc_start(uint32_t request, uint32_t rate) {
    int ret = -1;
    int retries;

    LOC_LOGd("test_loc_start");
    if (!isInitialized) {
        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_open_req(connection, mySuid);
            ret = wait_for_ack(OPEN_REQUEST);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for OPEN_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        isInitialized = true;
    }

    switch (request) {
    case LOCATION_START_REQUEST:
        LOC_LOGd("Start Location");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(connection, mySuid, true, QSH_LOCATION_POSITION_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for LOCATION_START_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case LOCATION_STOP_REQUEST:
        LOC_LOGd("Stop Location");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(connection, mySuid, false, QSH_LOCATION_POSITION_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for LOCATION_STOP_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case DATA_START_REQUEST:
        LOC_LOGd("Start meas and clk");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(connection, mySuid, true, QSH_LOCATION_MEAS_AND_CLK_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for DATA_START_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case DATA_STOP_REQUEST:
        LOC_LOGd("Stop meas and clk");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(connection, mySuid, false, QSH_LOCATION_MEAS_AND_CLK_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for DATA_STOP_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case QUIT_REQUEST:
        LOC_LOGd("Quit");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_close_req(connection, mySuid);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for QUIT_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
        }
        goto bail;
        break;
    default:
        goto bail;
        break;
    }
    return 0;

bail:
    LOC_LOGd("Bye bye");
    if (-1 == ret) {
        send_close_req(connection, mySuid);
    }
    return ret;
}

int main(int argc, char *argv[]) {
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);

    int ret = -1;
    int ret2 = -1;
    char buf[16], *p;
    int exit_loop = 0;
    uint locRate = 1000, measRate = 1000;
    char str[BUFFERSIZE];
    char* pch;
    static int locInProgress = 0;
    static int measInProgress = 0;

    pid = getpid();
    signum = SIGUSR2;
    timeout.tv_sec = 10;
    timeout.tv_nsec = 0;
    sigemptyset(&newmask);

    const loc_param_s_type gps_conf_params[] = {
    };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gps_conf_params);

    LOC_LOGd("SIGRTMIN = %d, signum=%d", SIGRTMIN, signum);

    ret2 = sigaddset(&newmask, signum);

    LOC_LOGd("after sigaddset ret2=%d , errno=%d, signum=%d", ret2, errno, signum);
    pthread_sigmask(SIG_BLOCK, &newmask, NULL);
    sigfillset(&newmask);

    LOC_LOGd("Begin Location Test threadid = %ld", pthread_self());

    suid_lookup lookup(suid_cb);
    lookup.request_suid(SENSOR_NAME);

    while (0 == exit_loop) {
        if (bVerboseMode) {
            printf("\n\n"
                   "1: start/stop location test\n"
                   "2: start/stop measurement test\n"
                   "3: turn off screen output\n"
                   "q: quit\n"
                   "\nEnter Command:");
        } else {
            printf("\n\n"
                   "1: start/stop location test\n"
                   "2: start/stop measurement test\n"
                   "3: turn on screen output\n"
                   "q: quit\n"
                   "\nEnter Command:");
        }

        fflush(stdout);
        p = fgets(buf, 16, stdin);
        if (NULL == p) {
            printf("Error: fgets returned nullptr !!\n");
            continue;
        }
        switch (p[0]) {
        case '1':
            if (locInProgress) {
                ret = test_loc_start(LOCATION_STOP_REQUEST, locRate);
                locInProgress = 0;
                if (0 == ret) {
                    printf("success\n");
                } else {
                    printf("failed\n");
                    exit_loop = 1;
                }
            }
            else {
                printf("Enter rate in milliseconds (>=1000): ");
                pch = fgets(str, BUFFERSIZE, stdin);
                if (pch != NULL) {
                    locRate = atoi(pch);
                }
                ret = test_loc_start(LOCATION_START_REQUEST, locRate);
                locInProgress = 1;
                if (0 == ret) {
                    printf("success\n");
                } else {
                    printf("failed\n");
                    exit_loop = 1;
                }
            }
            break;
        case '2':
            if (measInProgress) {
                ret = test_loc_start(DATA_STOP_REQUEST, measRate);
                measInProgress = 0;
                if (0 == ret) {
                    printf("success\n");
                } else {
                    printf("failed\n");
                    exit_loop = 1;
                }
            } else {
                printf("Enter rate in milliseconds (>=1000): ");
                pch = fgets(str, BUFFERSIZE, stdin);
                if (pch != NULL) {
                    measRate = atoi(pch);
                }
                ret = test_loc_start(DATA_START_REQUEST, measRate);
                measInProgress = 1;
                if (0 == ret) {
                    printf("success\n");
                } else {
                    printf("failed\n");
                    exit_loop = 1;
                }
            }
            break;
        case '3':
            bVerboseMode = !bVerboseMode;
            break;
        case 'q':
            if (locInProgress) {
                ret = test_loc_start(LOCATION_STOP_REQUEST, locRate);
            }
            if (measInProgress) {
                ret = test_loc_start(DATA_STOP_REQUEST, measRate);
            }
            test_loc_start(QUIT_REQUEST, 0);
            exit_loop = 1;
            break;
        default:
            break;
        }
    }

    delete connection;
    LOC_LOGd("Sensors Location Test completed - Exiting");
    return ret;
}
