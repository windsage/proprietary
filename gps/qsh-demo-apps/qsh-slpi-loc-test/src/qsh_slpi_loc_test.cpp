/*=============================================================================
  @file QSH_SLPI_LOC_TEST.c

  Location test application.  This application will open a QMI connection to
  the Sensors Service (which resides on the SSC).  It will send a SUID Lookup
  Request for the data type "location".

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
#include <time.h>

#include "qmi_client.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "qsh_loc_test.pb.h"
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
#define LOG_TAG "QSH_SLPI_LOC_TEST"

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define SENSOR_NAME "qsh_loc_test"

#define BUFFERSIZE              200
#define GNSS_MAX_GEOFENCES      30

typedef struct {
    uint32_t breach_type_mask;
    uint32_t responsiveness;
    uint32_t dwell_time;
    int32_t latitude;
    int32_t longitude;
    float radius;
} storedGeofences;

#define OPEN_REQUEST            0
#define LOCATION_START_REQUEST  1
#define LOCATION_STOP_REQUEST   2
#define DATA_START_REQUEST      3
#define DATA_STOP_REQUEST       4
#define QUIT_REQUEST            5

storedGeofences myGeofences[GNSS_MAX_GEOFENCES];
int myRequest = 0;
uint32_t myInterval = 0;
uint32_t myCount = 0;

static ssc_interface *connection = nullptr;
static sensor_uid mySuid;

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/
// Timer related
pid_t pid;
int signum;
struct timespec timeout;
sigset_t newmask;
siginfo_t info;

/**
 * Send an loc_test config request for the specified SUID.
 */
static void send_config_req(ssc_interface *conn, sensor_uid suid)
{
    string pb_req_msg_encoded;
    string loc_test_config_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_loc_test_config loc_test_config;
    qsh_loc_test_geofence_specification* loc_test_spec[myCount];

    LOC_LOGd("Send config request");

    if (nullptr == conn) {
        LOC_LOGe("conn is nullptr!");
        return;
    }

    loc_test_config.set_loc_test_request((qsh_loc_test_request)myRequest);
    loc_test_config.set_interval(myInterval);

    for (uint32_t i = 0; i < myCount; i++) {
        loc_test_spec[i] = loc_test_config.add_specs();
        loc_test_spec[i]->clear_id();
        loc_test_spec[i]->set_breach_type_mask(myGeofences[i].breach_type_mask);
        loc_test_spec[i]->set_responsiveness(myGeofences[i].responsiveness);
        loc_test_spec[i]->set_dwell_time(myGeofences[i].dwell_time);
        loc_test_spec[i]->set_latitude(myGeofences[i].latitude);
        loc_test_spec[i]->set_longitude(myGeofences[i].longitude);
        loc_test_spec[i]->set_radius(myGeofences[i].radius);
    }

    loc_test_config.SerializeToString(&loc_test_config_encoded);

    pb_req_msg.set_msg_id(QSH_LOC_TEST_MSGID_QSH_LOC_TEST_CONFIG);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(loc_test_config_encoded);
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
* Event callback function, as registered with ssc_interface.
*/
static void event_cb(const uint8_t *data, size_t size, uint64_t ts) {
    sns_client_event_msg pb_event_msg;
    UNUSED_VAR(ts);

    LOC_LOGd("Received QMI indication with length %zu", size);

    if (nullptr == data) {
        LOC_LOGe("data is nullptr!");
        return;
    }

    pb_event_msg.ParseFromArray(data, size);
    for (int i = 0; i < pb_event_msg.events_size(); i++) {
        const sns_client_event_msg_sns_client_event &pb_event = pb_event_msg.events(i);
        LOC_LOGd("Event[%i] msg_id=%i, ts=%llu", i, pb_event.msg_id(),
                 (unsigned long long)pb_event.timestamp());

        if (SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id()) {
            sns_std_error_event error;
            error.ParseFromString(pb_event.payload());

            LOC_LOGe("Received error event %i", error.error());
        }
        else if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == pb_event.msg_id()) {
            sns_std_attr_event attr_event;
            int32_t attr_id;

            attr_event.ParseFromString(pb_event.payload());
            attr_id = attr_event.attributes(0).attr_id();
            LOC_LOGd("Received attr_event attr_id %d", attr_id);
            send_config_req(connection, mySuid);
        }
        else {
            LOC_LOGe("Received unknown message ID %i", pb_event.msg_id());
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

int main(int argc, char *argv[]) {
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);

    int ret = -1;
    int ret2 = -1;
    char buf[BUFFERSIZE], *gfStr;
    char* tmp = NULL;
    char* substr;
    uint32_t i = 0;
    uint32_t k = 0;
    FILE *gf_fp = NULL;

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

    LOC_LOGd("Begin Location Test threadid = %ld", pthread_self());

    suid_lookup lookup(suid_cb);
    lookup.request_suid(SENSOR_NAME);

    memset(myGeofences, 0, sizeof(myGeofences));
    k = 0;

    if ((gf_fp = fopen("/vendor/etc/loc_test.txt", "r")) != NULL) {
        while (1) {
            gfStr = fgets(buf, BUFFERSIZE, gf_fp);
            if (NULL == gfStr) {
                break;
            } else if ('#' != gfStr[0]) {
                if (0 == myRequest) {
                    i = 0;
                    for (substr = strtok_r(gfStr, " ", &tmp);
                         substr != NULL;
                         substr = strtok_r(NULL, " ", &tmp)) {
                        if (0 == strncmp(substr, "QSH_LOC_TEST_POSITION_MEAS_AND_CLK",
                                         strlen("QSH_LOC_TEST_POSITION_MEAS_AND_CLK"))) {
                            myRequest = QSH_LOC_TEST_POSITION_MEAS_AND_CLK;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_MEAS_AND_CLK",
                                         strlen("QSH_LOC_TEST_MEAS_AND_CLK"))) {
                            myRequest = QSH_LOC_TEST_MEAS_AND_CLK;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_POSITION",
                                         strlen("QSH_LOC_TEST_POSITION"))) {
                            myRequest = QSH_LOC_TEST_POSITION;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_GEOFENCE",
                                        strlen("QSH_LOC_TEST_GEOFENCE"))) {
                            myRequest = QSH_LOC_TEST_GEOFENCE;
                            break;
                        }
                    }
                    continue;
                } else if (QSH_LOC_TEST_GEOFENCE != myRequest) {
                    // get the interval
                    substr = strtok_r(gfStr, " ", &tmp);
                    if (NULL != substr) {
                        myInterval = atoi(substr);
                    } else {
                        myInterval = 0;
                    }
                    break;
                } else {
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
                    if (i) {
                        k++;
                    }
                    if (GNSS_MAX_GEOFENCES == k) {
                        break;
                    }
                }
            }
        }
        myCount = k;
        fclose(gf_fp);
    } else {
        LOC_LOGe("Cannot open loc_test.txt for read!");
        printf("Cannot open loc_test.txt for read!\n");
        return -1;
    }
    printf("myRequest = %d myInterval = %d myCount = %d\n",
           myRequest, myInterval, myCount);
    if (0 == myRequest) {
        printf("Error (must select and option), please re-configure!\n");
        exit(0);
    }
    if (QSH_LOC_TEST_GEOFENCE != myRequest && 0 == myInterval) {
        printf("Error (interval must be non zero), please re-configure!\n");
        exit(0);
    }
    if (QSH_LOC_TEST_GEOFENCE == myRequest && 0 == myCount) {
        printf("Error (must have at least one geofence), please re-configure!\n");
        exit(0);
    }
    while (true) {
        sleep(1);
    }

    return ret;
}
