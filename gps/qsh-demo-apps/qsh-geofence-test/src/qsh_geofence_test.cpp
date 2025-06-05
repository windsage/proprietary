/*=============================================================================
  @file qsh_geofence_test.cpp

  Geofence test application.  This application will open a QMI connection to
  the Sensors Service (which resides on the SSC).  It will send a SUID Lookup
  Request for the data type "geofence".

  Copyright (c) 2016-2018, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include <unistd.h>
#include "qsh_utils_suid_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include "qmi_client.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "qsh_geofence.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
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
#define LOG_TAG "QSH_GEOFENCE_TEST"

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define SENSOR_NAME "geofence"

#define OPEN_REQUEST            0
#define QUIT_REQUEST            10
#define BUFFERSIZE              200
#define GNSS_MAX_GEOFENCES      30

#define NO_OF_RETRIES 5

typedef enum {
    GEOFENCE_INACTIVE   = 0,
    GEOFENCE_ACTIVE     = 1,
    GEOFENCE_PAUSED     = 2,
    GEOFENCE_ALL        = 8
} geofenceMode;

typedef struct {
    int32_t latitude;
    int32_t longitude;
    float radius;
} qsh_geofence_data;

typedef struct {
    uint32_t breach_type_mask;
    uint32_t responsiveness;
    uint32_t dwell_time;
} qsh_geofence_option;

typedef struct {
    geofenceMode mode;
    uint32_t id;
    uint32_t breach_type_mask;
    uint32_t responsiveness;
    uint32_t dwell_time;
    int32_t latitude;
    int32_t longitude;
    float radius;
} storedGeofences;

storedGeofences myGeofences[GNSS_MAX_GEOFENCES];
uint32_t gf[GNSS_MAX_GEOFENCES];
qsh_geofence_option gfOptions[GNSS_MAX_GEOFENCES];

static ssc_interface *connection = nullptr;
static sensor_uid mySuid;
static bool isInitialized = false;

// Timer related
pid_t pid;
int signum;
struct timespec timeout;
sigset_t newmask;
siginfo_t info;

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/
/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

static void send_open_req(ssc_interface *conn, sensor_uid suid);
static void send_close_req(ssc_interface *conn, sensor_uid suid);
static int wait_for_ack(uint8_t request);

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
        } else if (QSH_GEOFENCE_MSGID_QSH_GEOFENCE_ACK == pb_event.msg_id()) {
            qsh_geofence_ack ack;
            ack.ParseFromString(pb_event.payload());

            int i;
            bool error = ack.error();
            int command = ack.command();
            int subcommand = ack.subcommand();
            int idsSize = ack.ids_size();
            int errsSize = ack.errs_size();

            LOC_LOGd("Received geofence ack err=%d cmd=%d "
                     "version=0x%X subcommand=%d "
                     "idsSize=%d, errsSize=%d ",
                     error, command,
                     ack.version(), subcommand,
                     idsSize, errsSize);

            if (true == error && idsSize == errsSize) {
                sig_value.sival_int = 1;
                switch (command) {
                case QSH_GEOFENCE_MSGID_QSH_GEOFENCE_OPEN:
                    LOC_LOGd("Received ack for OPEN request");
                    break;
                case QSH_GEOFENCE_MSGID_QSH_GEOFENCE_UPDATE:
                    LOC_LOGd("Received ack for UPDATE request, subcommand=%d", subcommand);
                    switch (subcommand) {
                    case QSH_GEOFENCE_ADD:
                        for (i = 0; i < idsSize; i++) {
                            if (0 == ack.errs(i)) {
                                myGeofences[gf[i]].mode = GEOFENCE_ACTIVE;
                                myGeofences[gf[i]].id = ack.ids(i);
                            } else {
                                LOC_LOGe("Error %d received when adding geofence %d",
                                         ack.errs(i), gf[i]);
                            }
                        }
                        break;
                    case QSH_GEOFENCE_REMOVE:
                        for (i = 0; i < idsSize; i++) {
                            if (0 == ack.errs(i)) {
                                for (uint32_t j = 0; j < ARR_SIZE(myGeofences); j++) {
                                    if (ack.ids(i) == myGeofences[j].id) {
                                        myGeofences[j].mode = GEOFENCE_INACTIVE;
                                        myGeofences[j].id = 0;
                                    }
                                }
                            } else {
                                LOC_LOGe("Error %d received when removing geofence with id %d",
                                         ack.errs(i), gf[i]);
                            }
                        }
                        break;
                    case QSH_GEOFENCE_MODIFY:
                        for (i = 0; i < idsSize; i++) {
                            if (0 == ack.errs(i)) {
                                for (uint32_t j = 0; j < ARR_SIZE(myGeofences); j++) {
                                    if (ack.ids(i) == myGeofences[j].id) {
                                        myGeofences[j].breach_type_mask =
                                                gfOptions[i].breach_type_mask;
                                        myGeofences[j].responsiveness =
                                                gfOptions[i].responsiveness;
                                        myGeofences[j].dwell_time = gfOptions[i].dwell_time;
                                    }
                                }
                            } else {
                                LOC_LOGe("Error %d received when modfying geofence with id %d",
                                         ack.errs(i), gf[i]);
                            }
                        }
                        break;
                    case QSH_GEOFENCE_PAUSE:
                        for (i = 0; i < idsSize; i++) {
                            if (0 == ack.errs(i)) {
                                for (uint32_t j = 0; j < ARR_SIZE(myGeofences); j++) {
                                    if (ack.ids(i) == myGeofences[j].id) {
                                        myGeofences[gf[i]].mode = GEOFENCE_PAUSED;
                                    }
                                }
                            } else {
                                LOC_LOGe("Error %d received when pausing geofence with id %d",
                                         ack.errs(i), gf[i]);
                            }
                        }
                        break;
                    case QSH_GEOFENCE_RESUME:
                        for (i = 0; i < idsSize; i++) {
                            if (0 == ack.errs(i)) {
                                for (uint32_t j = 0; j < ARR_SIZE(myGeofences); j++) {
                                    if (ack.ids(i) == myGeofences[j].id) {
                                        myGeofences[gf[i]].mode = GEOFENCE_ACTIVE;
                                    }
                                }
                            } else {
                                LOC_LOGe("Error %d received when resuming geofence with id %d",
                                        ack.errs(i), gf[i]);
                             }
                        }
                        break;
                    default:
                        LOC_LOGe("Received ack for unknown subcommand");
                        break;
                    }
                    break;
                case QSH_GEOFENCE_MSGID_QSH_GEOFENCE_CLOSE:
                    LOC_LOGd("Received ack for CLOSE request");
                    break;
                default:
                    LOC_LOGe("Received ack for unknown request");
                    break;
                }
            } else {
                sig_value.sival_int = 2;
                LOC_LOGe("Received NACK for request or idsSize != errsSize");
            }
            sigqueue(pid, signum, sig_value);
        } else if (QSH_GEOFENCE_MSGID_QSH_GEOFENCE_BREACH_EVENT == pb_event.msg_id()) {
            qsh_geofence_breach_event breach;
            breach.ParseFromString(pb_event.payload());

            int idsSize = breach.ids_size();
            char* breachString[] = { "ENTER", "EXIT", "ENTER & DWELL IN", "EXIT & DWELL OUT" };

            printf("Received geofence breach event!\n");
            LOC_LOGd("breach ids_size = %d", idsSize);

            for (int i = 0; i < idsSize; i++) {
                printf("ids[%d]=%d\n", i, breach.ids(i));
                LOC_LOGd("ids[%d]=%d", i, breach.ids(i));
            }
            printf("location_timestamp=%" PRIu64" \n", breach.location_timestamp());
            LOC_LOGd("location_timestamp=%" PRIu64,
                     breach.location_timestamp());

            printf("latitude=%.2f longitude=%.2f altitude=%.2f\n",
                    breach.latitude() / 1e7,
                    breach.longitude() / 1e7,
                    breach.altitude());
            LOC_LOGd("latitude=%.2f longitude=%.2f altitude=%.2f",
                     breach.latitude() / 1e7,
                     breach.longitude() / 1e7,
                     breach.altitude());

            printf("speed=%.2f bearing=%.2f accuracy=%.2f\n",
                    breach.speed(),
                    breach.bearing(),
                    breach.accuracy());
            LOC_LOGd("speed=%.2f bearing=%.2f accuracy=%.2f",
                     breach.speed(),
                     breach.bearing(),
                     breach.accuracy());

            printf("flags=0x%x breach_type=%s\n",
                    breach.flags(),
                    breachString[breach.breach_type()]);
            LOC_LOGd("flags=0x%x breach_type=%s",
                     breach.flags(),
                     breachString[breach.breach_type()]);

            printf("\n");

        } else {
            LOC_LOGd("Received unknown message ID %i", pb_event.msg_id());
        }
    }
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
        LOC_LOGe("%s sensor is not available", SENSOR_NAME);
        exit(-1);
    }
}

static int wait_for_ack(uint8_t request) {
    int ret = -1;
    sigset_t oldmask;

    pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);
    LOC_LOGd("Wait for ack request=%d pid=%d EAGAIN=%d EINTR=%d",
             request, pid, EAGAIN, EINTR);
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
            LOC_LOGd("Got signal from ACK ret=%d errno=%d sival_int=%d",
                     ret, errno, info.si_value.sival_int);
            if (2 == info.si_value.sival_int) {
                ret = -2;
                sleep(2);
            }
            break;
        }
    }
    pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
    return ret;
}

static void send_close_req(ssc_interface *conn, sensor_uid suid) {
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    LOC_LOGd("Send geofence close request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    pb_req_msg.set_msg_id(QSH_GEOFENCE_MSGID_QSH_GEOFENCE_CLOSE);
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

static void send_update_req(ssc_interface *conn, sensor_uid suid,
                            uint32_t request, uint32_t count) {
    string pb_req_msg_encoded;
    string geofence_update_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_geofence_update geofence_update;
    qsh_geofence_specification* geofence_spec[count];

    LOC_LOGd("Send geofence update request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    geofence_update.set_geofence_subcommand((qsh_geofence_subcommand)request);

    for (uint32_t i = 0; i < count; i++) {
        geofence_spec[i] = geofence_update.add_specs();
        switch (request) {
        case QSH_GEOFENCE_ADD:
            geofence_spec[i]->set_breach_type_mask(myGeofences[gf[i]].breach_type_mask);
            geofence_spec[i]->set_responsiveness(myGeofences[gf[i]].responsiveness);
            geofence_spec[i]->set_dwell_time(myGeofences[gf[i]].dwell_time);
            geofence_spec[i]->set_latitude(myGeofences[gf[i]].latitude);
            geofence_spec[i]->set_longitude(myGeofences[gf[i]].longitude);
            geofence_spec[i]->set_radius(myGeofences[gf[i]].radius);
            break;
        case QSH_GEOFENCE_REMOVE:
        case QSH_GEOFENCE_PAUSE:
        case QSH_GEOFENCE_RESUME:
            geofence_spec[i]->set_id(myGeofences[gf[i]].id);
            break;
        case QSH_GEOFENCE_MODIFY:
            geofence_spec[i]->set_id(myGeofences[gf[i]].id);
            geofence_spec[i]->set_breach_type_mask(myGeofences[gf[i]].breach_type_mask);
            geofence_spec[i]->set_responsiveness(myGeofences[gf[i]].responsiveness);
            geofence_spec[i]->set_dwell_time(myGeofences[gf[i]].dwell_time);
            break;
        default:
            LOC_LOGw("Invalid request = %d", request);
        }
    }

    geofence_update.SerializeToString(&geofence_update_encoded);

    pb_req_msg.set_msg_id(QSH_GEOFENCE_MSGID_QSH_GEOFENCE_UPDATE);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(geofence_update_encoded);
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

static void send_open_req(ssc_interface *conn, sensor_uid suid) {
    string pb_req_msg_encoded;
    string geofence_open_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_geofence_open geofence_open;

    LOC_LOGd("Send geofence open request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    geofence_open.set_version(0x01000000);
    geofence_open.SerializeToString(&geofence_open_encoded);

    pb_req_msg.set_msg_id(QSH_GEOFENCE_MSGID_QSH_GEOFENCE_OPEN);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(geofence_open_encoded);
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

int test_gf_start(uint32_t request, uint32_t count) {
    int ret = -1;
    int retries;

    LOC_LOGd("test_gf_start");

    if (!isInitialized) {
        for (retries = 0; retries < NO_OF_RETRIES && ret <= 0; retries++) {
            send_open_req(connection, mySuid);
            ret = wait_for_ack(OPEN_REQUEST);
        }
        if (ret < 0) {
            LOC_LOGw("Did not receive ACK for OPEN_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        isInitialized = true;
    }

    switch (request) {
    case QSH_GEOFENCE_ADD:
    case QSH_GEOFENCE_REMOVE:
    case QSH_GEOFENCE_MODIFY:
    case QSH_GEOFENCE_PAUSE:
    case QSH_GEOFENCE_RESUME:
        ret = -1;
        if (0 != count) {
            for (retries = 0; retries < NO_OF_RETRIES && ret <= 0; retries++) {
                send_update_req(connection, mySuid, request, count);
                ret = wait_for_ack(request);
            }
            if (ret < 0) {
                LOC_LOGw("Did not receive ACK for %d after %d tries! Give up!",
                         request, NO_OF_RETRIES);
                goto bail;
            }

        } else {
            LOC_LOGe("Count has to be > 0!");
        }
        break;

    case QUIT_REQUEST:
        for (retries = 0; retries < NO_OF_RETRIES && ret <= 0; retries++) {
            send_close_req(connection, mySuid);
            ret = wait_for_ack(QUIT_REQUEST);
        }
        if (ret < 0) {
            LOC_LOGw("Did not receive ACK for QUIT_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        ret = 0;
        goto bail;
        break;

    default:
        goto bail;
        break;
    }
    return 0;

bail:
    isInitialized = false;
    LOC_LOGd("Bye bye");
    if (-1 == ret) {
        send_close_req(connection, mySuid);
    }
    return ret;
}

void printGeofences(uint8_t mode_mask) {
    char* statusString[] = { "Inactive", "Active", "Paused" };
    char* breachString[] =
        { "", "ENTER", "EXIT", "", "", "ENTER & DWELL IN", "", "", "", "", "EXIT & DWELL OUT" };

    printf("  #   status  ID   latitude   longitude    radius \
           breach responsiveness dwell_time\n");
    for (uint32_t i = 0; i < ARR_SIZE(myGeofences); i++) {
        if (GEOFENCE_ALL == mode_mask ||
            (GEOFENCE_INACTIVE == mode_mask && GEOFENCE_INACTIVE == myGeofences[i].mode) ||
            ((GEOFENCE_ACTIVE | GEOFENCE_PAUSED) == mode_mask &&
                GEOFENCE_INACTIVE != myGeofences[i].mode)) {

            char* statusStr = "Invalid";
            char* breachMaskStr = "Invalid";
            if (myGeofences[i].mode < ARR_SIZE(statusString)) {
                statusStr = statusString[myGeofences[i].mode];
            } else {
                printf("Invalid mode %d\n", myGeofences[i].mode);
            }
            if (myGeofences[i].breach_type_mask < ARR_SIZE(breachString)) {
                breachMaskStr = breachString[myGeofences[i].breach_type_mask];
            } else {
                printf("Invalid breach mask %d\n", myGeofences[i].breach_type_mask);
            }

            printf("%*d %*s %*d %*d %*d %*.2f %*s %*d %*d\n",
                   3, i, 8, statusStr, 3, myGeofences[i].id,
                   10, myGeofences[i].latitude, 11, myGeofences[i].longitude,
                   9, myGeofences[i].radius,
                   17, breachMaskStr,
                   14, myGeofences[i].responsiveness, 10, myGeofences[i].dwell_time);
        }
    }
}

uint32_t getGfEntries(uint32_t request) {
    uint32_t count = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    char buf[BUFFERSIZE], *gfStr;
    char* tmp = NULL;
    char* substr;

    do {
        memset(gf, 0, sizeof(gf));
        switch (request) {
        case QSH_GEOFENCE_ADD:
            printf("Input geofences # (Inactive, ID = 0) to add (e.g. 0 2 5):");
            break;
        case QSH_GEOFENCE_REMOVE:
            printf("Input geofences # (Active or Paused, ID non 0) to remove (e.g. 0 2 5):");
            break;
        case QSH_GEOFENCE_MODIFY:
            printf("Input geofences # (Active, ID non 0) to modify (e.g. 5 1 3):");
            break;
        case QSH_GEOFENCE_PAUSE:
            printf("Input geofences # (Active, ID non 0) to pause (e.g. 5 1 3):");
            break;
        case QSH_GEOFENCE_RESUME:
            printf("Input geofences # (Paused, ID non 0) to resume (e.g. 5 1 3):");
            break;
        default:
            break;
        }
        gfStr = fgets(buf, 100, stdin);
        i = 0;
        for (substr = strtok_r(gfStr, " ", &tmp);
            substr != NULL;
            substr = strtok_r(NULL, " ", &tmp)) {
            gf[i++] = atoi(substr);
        }
        count = 0;
        // validating the entries, making sure the requested number of GFs is not 0
        for (j = 0; j < i; j++) {
            if (gf[j] >= ARR_SIZE(myGeofences) || gf[j] < 0) {
                printf("Selected geofence (# %d) is not in the database!\n", gf[j]);
                return 0;
            } else {
                switch (request) {
                case QSH_GEOFENCE_ADD:
                    if (GEOFENCE_INACTIVE == myGeofences[gf[j]].mode) {
                        gf[count] = gf[j];
                        count++;
                    } else {
                        printf("Selected geofence (# %d) to add is not Inactive!\n", gf[j]);
                        return 0;
                    }
                    break;
                case QSH_GEOFENCE_REMOVE:
                    if (GEOFENCE_INACTIVE != myGeofences[gf[j]].mode) {
                        gf[count] = gf[j];
                        count++;
                    } else {
                        printf("Selected geofence (# %d) to remove is Inactive!\n", gf[j]);
                        return 0;
                    }
                    break;
                case QSH_GEOFENCE_MODIFY:
                    if (GEOFENCE_ACTIVE == myGeofences[gf[j]].mode) {
                        gf[count] = gf[j];
                        count++;
                    } else {
                        printf("Selected geofence (# %d) to modify is not Active!\n", gf[j]);
                        return 0;
                    }
                    break;
                case QSH_GEOFENCE_PAUSE:
                    if (GEOFENCE_ACTIVE == myGeofences[gf[j]].mode) {
                        gf[count] = gf[j];
                        count++;
                    } else {
                        printf("Selected geofence (# %d) to remove is not Active!\n", gf[j]);
                        return 0;
                    }
                    break;
                case QSH_GEOFENCE_RESUME:
                    if (GEOFENCE_PAUSED == myGeofences[gf[j]].mode) {
                        gf[count] = gf[j];
                        count++;
                    } else {
                        printf("Selected geofence (# %d) to resume is not Paused!\n", gf[j]);
                        return 0;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    } while (0 == count);

    if (QSH_GEOFENCE_MODIFY == request) {
        for (j = 0; j < count; j++) {
            do {
                printf("Input new options for GF with # %d (e.g. 1 200 5):", gf[j]);
                gfStr = fgets(buf, 100, stdin);
                int k = 0;
                for (substr = strtok_r(gfStr, " ", &tmp);
                    substr != NULL;
                    substr = strtok_r(NULL, " ", &tmp)) {
                    switch (k) {
                    case 0:
                        gfOptions[j].breach_type_mask = atoi(substr);
                        break;
                    case 1:
                        gfOptions[j].responsiveness = atoi(substr);
                        break;
                    case 2:
                        gfOptions[j].dwell_time = atoi(substr);
                        break;
                    }
                    k++;
                }
            } while (gfOptions[j].breach_type_mask != 1 &&
                gfOptions[j].breach_type_mask != 2 &&
                gfOptions[j].breach_type_mask != 5 &&
                gfOptions[j].breach_type_mask != 10);
        }
    }

    for (j = 0; j < count; j++) {
        printf("#[%d] = %d\n", j, gf[j]);
    }
    return count;
}

int main(int argc, char *argv[]) {
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);

    int ret = -1;
    int ret2 = -1;
    char buf[BUFFERSIZE], *p, *gfStr;
    int exit_loop = 0;
    char* tmp = NULL;
    char* substr;
    uint32_t i = 0;
    uint32_t k = 0;
    FILE *gf_fp = NULL;
    uint32_t newCount = 0;

    pid = getpid();
    signum = SIGUSR2;
    timeout.tv_sec = 10;
    timeout.tv_nsec = 0;
    sigemptyset(&newmask);
    ret2 = sigaddset(&newmask, signum);
    sigprocmask(SIG_BLOCK, &newmask, NULL);

    LOC_LOGd("after sigaddset ret2=%d , errno=%d, signum=%d", ret2, errno, signum);
    pthread_sigmask(SIG_BLOCK, &newmask, NULL);
    sigfillset(&newmask);

    const loc_param_s_type gps_conf_params[] = {
    };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gps_conf_params);

    LOC_LOGd("Begin Geofence Test threadid = %ld", pthread_self());

    suid_lookup lookup(suid_cb);
    lookup.request_suid(SENSOR_NAME);

    memset(myGeofences, 0, sizeof(myGeofences));
    k = 0;

    if ((gf_fp = fopen("/vendor/etc/geofences.txt", "r")) != NULL) {
        while (1) {
            gfStr = fgets(buf, BUFFERSIZE, gf_fp);
            if (NULL == gfStr) {
                break;
            } else if ('#' != gfStr[0]) {
                i = 0;
                for (substr = strtok_r(gfStr, " ", &tmp);
                    substr != NULL;
                    substr = strtok_r(NULL, " ", &tmp)) {
                    switch (i) {
                    case 0:
                        myGeofences[k].breach_type_mask = atoi(substr);
                        break;
                    case 1:
                        myGeofences[k].responsiveness = atoi(substr);
                        break;
                    case 2:
                        myGeofences[k].dwell_time = atoi(substr);
                        break;
                    case 3:
                        myGeofences[k].latitude = (int32_t)(atof(substr)*1e7);
                        break;
                    case 4:
                        myGeofences[k].longitude = (int32_t)(atof(substr)*1e7);
                        break;
                    case 5:
                        myGeofences[k].radius = atof(substr);
                        break;
                    default:
                        break;
                    }
                    i++;
                    if (6 == i) {
                        break;
                    }
                }
                myGeofences[k].mode = GEOFENCE_INACTIVE;
                myGeofences[k].id = 0;
                k++;
                if (GNSS_MAX_GEOFENCES == k) {
                    break;
                }
            }
        }
        fclose(gf_fp);
    } else {
        LOC_LOGe("Cannot open geofences.txt for read!");
        return -1;
    }

    while (0 == exit_loop) {
        printf("\n\n"
               "1: show all geofences\n"
               "2: show inactive geofence(s)\n"
               "3: show active and paused geofence(s)\n"
               "4: add geofence(s)\n"
               "5: remove geofence(s)\n"
               "6: modify geofence(s)\n"
               "7: pause geofence(s)\n"
               "8: resume geofence(s)\n"
               "q: quit\n"
               "\nEnter Command:");

        fflush(stdout);
        p = fgets(buf, 16, stdin);
        if (NULL == p) {
            printf("Error: fgets returned nullptr !!\n");
            continue;
        }
        switch (p[0]) {
        case '1':
            printGeofences(GEOFENCE_ALL);
            break;
        case '2':
            printGeofences(GEOFENCE_INACTIVE);
            break;
        case '3':
            printGeofences(GEOFENCE_ACTIVE | GEOFENCE_PAUSED);
            break;
        case '4':
            newCount = getGfEntries(QSH_GEOFENCE_ADD);
            ret = test_gf_start(QSH_GEOFENCE_ADD, newCount);
            if (0 == ret) {
                printf("success\n");
            } else {
                printf("failed\n");
                exit_loop = 1;
            }
            break;
        case '5':
            newCount = getGfEntries(QSH_GEOFENCE_REMOVE);
            ret = test_gf_start(QSH_GEOFENCE_REMOVE, newCount);
            if (0 == ret) {
                printf("success\n");
            } else {
                printf("failed\n");
                exit_loop = 1;
            }
            break;
        case '6':
            newCount = getGfEntries(QSH_GEOFENCE_MODIFY);
            ret = test_gf_start(QSH_GEOFENCE_MODIFY, newCount);
            if (0 == ret) {
                printf("success\n");
            } else {
                printf("failed\n");
                exit_loop = 1;
            }
            break;
        case '7':
            newCount = getGfEntries(QSH_GEOFENCE_PAUSE);
            ret = test_gf_start(QSH_GEOFENCE_PAUSE, newCount);
            if (0 == ret) {
                printf("success\n");
            } else {
                printf("failed\n");
                exit_loop = 1;
            }
            break;
        case '8':
            newCount = getGfEntries(QSH_GEOFENCE_RESUME);
            ret = test_gf_start(QSH_GEOFENCE_RESUME, i);
            if (0 == ret) {
                printf("success\n");
            } else {
                printf("failed\n");
                exit_loop = 1;
            }
            break;
        case 'q':
            test_gf_start(QUIT_REQUEST, 0);
            exit_loop = 1;
            break;
        default:
            break;
        }
    }
    LOC_LOGd("Sensors Geofence Test completed - Exiting");
    return ret;
}
