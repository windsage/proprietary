/**
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <string.h>
#include <stdexcept>
#include <cinttypes>
#include <mutex>
#include <stdint.h>
#include <sched.h>
#include "SlimSscConnection.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "qmi_client.h"
#include <map>
#include "utils/SystemClock.h"
#include <condition_variable>
#include "SlimSscConnection.h"
#include "slim_os_log_api.h"
#include "SlimSscUtils.h"
#include <memory>
#include <loc_pla.h>

using namespace std;
using namespace google::protobuf::io;

#define MAX_SVC_INFO_ARRAY_SIZE 5

/* number of times to wait for sensors service */
static const int SENSORS_SERVICE_DISCOVERY_TRIES = 30;
static const int SENSORS_SERVICE_DISCOVERY_TRIES_POST_SENSORLIST = 30;

/* timeout for each try */
static auto SENSORS_SERVICE_DISCOVERY_TIMEOUT_IN_SEC = 1;
static auto SENSORS_SERVICE_DISCOVERY_TIMEOUT_POST_SENSORLIST_IN_MSEC = 1000;
static uint32_t g_qmierr_cnt;

/* exception type defining errors related to qmi API calls */
struct qmi_error : public runtime_error
{
    qmi_error(int error_code, const std::string& what = "") :
        runtime_error(what + ": " + error_code_to_string(error_code)) { }

    static string error_code_to_string(int code)
    {
        static const map<int, string> error_map = {
            { QMI_NO_ERR, "qmi no error" },
            { QMI_INTERNAL_ERR, "qmi internal error" },
            { QMI_TIMEOUT_ERR, "qmi timeout" },
            { QMI_XPORT_BUSY_ERR, "qmi transport busy" },
            { QMI_SERVICE_ERR, "qmi service error" },
        };
        string msg;
        try {
            msg = error_map.at(code);
        } catch (out_of_range& e) {
            msg = "qmi error";
        }
        return msg + " (" + to_string(code) + ")";
    }
};

/* implementation of ssc_connection using qmi */
class ssc_qmi_connection
{
public:
    /* establish new qmi connection to ssc */
    ssc_qmi_connection(ssc_event_cb event_cb);

    /* disconnect from ssc */
    ~ssc_qmi_connection();

    /* send encoded request message to ssc */
    void send_request(const string& pb_req_message_encoded);

    /* ssc connection error Call back */
    void register_error_cb(ssc_error_cb cb) { _error_cb = cb; }

    /* ssc connection response Call back */
    void register_resp_cb(ssc_resp_cb cb) { _resp_cb = cb; }

private:
    ssc_event_cb _event_cb;
    qmi_client_type _qmi_handle = nullptr;
    bool _service_ready;
    bool _service_accessed;
    std::mutex _mutex;
    std::condition_variable _cv;
    qmi_cci_os_signal_type _os_params;
    /* flag to see if connection is being reset */
    bool _reconnecting;

    /*flag to check disconnection*/
    bool _connection_closed;
    /* Call back functions */
    ssc_error_cb _error_cb;
    ssc_resp_cb _resp_cb;

    void qmi_connect();
    void qmi_disconnect();
    void qmi_wait_for_service();

    static void qmi_notify_cb(qmi_client_type user_handle,
                              qmi_idl_service_object_type service_obj,
                              qmi_client_notify_event_type service_event,
                              void *notify_cb_data);

    static void qmi_indication_cb(qmi_client_type user_handle,
                                  unsigned int msg_id,
                                  void* ind_buf,
                                  unsigned int ind_buf_len,
                                  void* ind_cb_data);

    static void qmi_error_cb(qmi_client_type user_handle,
                             qmi_client_error_type error,
                             void* err_cb_data);

   /**
    * Handle a sns_client_report_ind_msg_v01 or sns_client_jumbod_report_ind_msg_v01
    * indication message received from SEE's Client Manager.
    *
    * @param[i] msg_id One of SNS_CLIENT_REPORT_IND_V01 or SNS_CLIENT_JUMBO_REPORT_IND_V01
    * @param[i] ind_buf The encoded QMI message buffer
    * @param[i] ind_buf_len Length of ind_buf
    */
    void handle_indication(unsigned int msg_id,
                            void *ind_buf,
                            unsigned int ind_buf_len);

    static const uint32_t QMI_ERRORS_HANDLED_ATTEMPTS = 10000;
    static const uint32_t QMI_RESPONSE_TIMEOUT_MS = 2000;
};

ssc_qmi_connection::ssc_qmi_connection(ssc_event_cb event_cb) :
    _event_cb(event_cb),
    _reconnecting(false),
    _service_ready(false),
    _service_accessed(false),
    _connection_closed(false)
{
    qmi_connect();
    SLIM_LOGD("_service_accessed %d", (int)_service_accessed);
}

ssc_qmi_connection::~ssc_qmi_connection()
{
    _connection_closed = true;
    qmi_disconnect();
}

void ssc_qmi_connection::qmi_notify_cb(qmi_client_type user_handle,
                                       qmi_idl_service_object_type service_obj,
                                       qmi_client_notify_event_type service_event,
                                       void *notify_cb_data)
{
    ssc_qmi_connection *conn = (ssc_qmi_connection *) notify_cb_data;
    conn->_service_ready = true;
    conn->_cv.notify_one();
}

void ssc_qmi_connection::qmi_wait_for_service()
{
    qmi_client_type notifier_handle = nullptr;
    qmi_client_error_type qmi_err;
    qmi_cci_os_signal_type os_params;
    int num_tries = SENSORS_SERVICE_DISCOVERY_TRIES;

    qmi_err = qmi_client_notifier_init(SNS_CLIENT_SVC_get_service_object_v01(),
                                       &os_params, &notifier_handle);
    if (QMI_NO_ERR != qmi_err) {
        SLIM_LOGE("qmi_client_notifier_init() failed. qmi_err=%d", qmi_err);
        return;
    }

    /* register a callback and wait until service becomes available */
    qmi_err = qmi_client_register_notify_cb(notifier_handle, qmi_notify_cb,
                                            this);
    if (qmi_err != QMI_NO_ERR) {
        qmi_client_release(notifier_handle);
        notifier_handle = nullptr;
        SLIM_LOGE("qmi_client_register_notify_cb() failed. qmi_err=%d", qmi_err);
        return;
    }

    if (_service_accessed) {
      num_tries = SENSORS_SERVICE_DISCOVERY_TRIES_POST_SENSORLIST;
    }

    std::unique_lock<std::mutex> lk(_mutex);
    bool timeout = false;
    while (num_tries > 0 && (_service_ready != true)) {
        num_tries--;
        if (_service_accessed)
            timeout = !_cv.wait_for(lk,
             std::chrono::milliseconds(SENSORS_SERVICE_DISCOVERY_TIMEOUT_POST_SENSORLIST_IN_MSEC),
             [this]{ return _service_ready; });
        else
            timeout = !_cv.wait_for(lk,
                   std::chrono::seconds(SENSORS_SERVICE_DISCOVERY_TIMEOUT_IN_SEC),
                   [this]{ return _service_ready; });

        if (timeout) {
            if (num_tries == 0) {
                if (lk.owns_lock()) {
                    lk.unlock();
                }
                if (nullptr != notifier_handle) {
                    qmi_client_release(notifier_handle);
                    notifier_handle = nullptr;
                }
                SLIM_LOGE(
                    "FATAL: could not find sensors QMI service");
            }
            SLIM_LOGE("timeout while waiting for sensors QMI service: "
                     "will try %d more time(s)", num_tries);
        } else {
            _service_accessed = true;
        }
    }
    if (lk.owns_lock()) {
        lk.unlock();
    }
    if (nullptr != notifier_handle) {
        qmi_client_release(notifier_handle);
        notifier_handle = nullptr;
    }
}

void ssc_qmi_connection::qmi_connect()
{
    qmi_idl_service_object_type svc_obj =
        SNS_CLIENT_SVC_get_service_object_v01();

    qmi_client_error_type qmi_err;
    qmi_service_info svc_info_array[MAX_SVC_INFO_ARRAY_SIZE];
    uint32_t num_services, num_entries = MAX_SVC_INFO_ARRAY_SIZE;

    /*svc_info_array[5] - Initialized to 0 to avoid static analysis errors*/
    for (uint32_t i = 0 ; i < num_entries ; i++)
      memset(&svc_info_array[i], 0, sizeof(svc_info_array[i]));

    SLIM_LOGV("waiting for sensors qmi service");
    qmi_wait_for_service();
    SLIM_LOGV("connecting to qmi service");
    qmi_err = qmi_client_get_service_list(svc_obj, svc_info_array,
                                          &num_entries, &num_services);
    if (QMI_NO_ERR != qmi_err) {
        SLIM_LOGE("qmi_client_get_service_list() failed. qmi_err=%d", qmi_err);
    }

    if (num_entries == 0) {
        SLIM_LOGE("sensors service has no available instances");
    }

    if (_connection_closed) {
        SLIM_LOGI("connection got closed do not open qmi_channel");
        return ;
    }

    /* As only one qmi service is expected for sensors, use the 1st instance */
    qmi_service_info svc_info = svc_info_array[0];


    std::unique_lock<std::mutex> lk(_mutex);
    qmi_err = qmi_client_init(&svc_info, svc_obj, qmi_indication_cb,
                              (void*)this, &_os_params, &_qmi_handle);
    if (QMI_IDL_LIB_NO_ERR != qmi_err) {
        if (lk.owns_lock()) {
            lk.unlock();
        }
        SLIM_LOGE("qmi_client_init() failed. qmi_err=%d", qmi_err);
    }

    qmi_err = qmi_client_register_error_cb(_qmi_handle, qmi_error_cb, this);
    if (QMI_NO_ERR != qmi_err) {
        if (lk.owns_lock()) {
            lk.unlock();
        }
        qmi_client_release(_qmi_handle);
        SLIM_LOGE("qmi_client_register_error_cb() failed. qmi_err=%d", qmi_err);
    }
    if (lk.owns_lock()) {
        lk.unlock();
    }
    SLIM_LOGV("connected to ssc for %p", (void *)this);
}

void ssc_qmi_connection::qmi_disconnect()
{
    std::unique_lock<std::mutex> lk(_mutex);
    if (_qmi_handle != nullptr) {
        qmi_client_release(_qmi_handle);
        _qmi_handle = nullptr;
        /*in ssr call back and sensor disabled , so notify qmi_connect to comeout*/
        if (_connection_closed)
            _cv.notify_one();
    }
    /*explicit unlock not required , just added not to miss the logic*/
    if (lk.owns_lock()) {
        lk.unlock();
    }
    SLIM_LOGV("disconnected from ssc for %p", (void *)this);
}

void ssc_qmi_connection::handle_indication(unsigned int msg_id, void *ind_buf,
                                           unsigned int ind_buf_len)
{
  int32_t qmi_err;
  uint32_t ind_size;
  size_t buffer_len;
  uint8_t *buffer = NULL;

  SLIM_LOGV("msg_id %d ", msg_id);
  if (SNS_CLIENT_REPORT_IND_V01 == msg_id){
    sns_client_report_ind_msg_v01 *ind = NULL;
    ind = (sns_client_report_ind_msg_v01 *)calloc(1, sizeof(sns_client_report_ind_msg_v01));
    if (ind == NULL){
      SLIM_LOGE("Error while creating memory for ind");
      return;
    }
    ind_size = sizeof(sns_client_report_ind_msg_v01);
    qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
        QMI_IDL_INDICATION,
        msg_id,
        ind_buf,
        ind_buf_len,
        (void*)ind,
        ind_size);
    if (QMI_IDL_LIB_NO_ERR != qmi_err) {
      SLIM_LOGE("qmi_idl_message_decode() failed. qmi_err=%d SNS_CLIENT_REPORT_IND_V01", qmi_err);
      free(ind);
      ind = NULL;
      return;
    }
    SLIM_LOGV("indication, payload_len=%u", ind->payload_len);

    buffer_len = ind->payload_len;
    buffer = (uint8_t*)calloc(1, buffer_len);
    if (buffer == NULL){
      SLIM_LOGE("buffer failed to creat ");
      free(ind);
      ind = NULL;
      return;
    }
    memscpy(buffer, buffer_len, ind->payload, buffer_len);
    free(ind);
    this->_event_cb(buffer, buffer_len);
    free(buffer);
  }
  else if (SNS_CLIENT_JUMBO_REPORT_IND_V01 == msg_id){
    sns_client_jumbo_report_ind_msg_v01 *ind_jumbo = NULL;
    ind_jumbo = (sns_client_jumbo_report_ind_msg_v01 *)calloc(1,
                                    sizeof(sns_client_jumbo_report_ind_msg_v01));
    if (ind_jumbo == NULL){
      SLIM_LOGE("Error while creating memory for ind_jumbo");
      return;
    }
    ind_size = sizeof(sns_client_jumbo_report_ind_msg_v01);
    qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
        QMI_IDL_INDICATION,
        msg_id,
        ind_buf,
        ind_buf_len,
        (void*)ind_jumbo,
        ind_size);
    if (QMI_IDL_LIB_NO_ERR != qmi_err) {
      SLIM_LOGE("qmi_idl_message_decode() failed. qmi_err=%d SNS_CLIENT_JUMBO_REPORT_IND_V01",
                qmi_err);
      free(ind_jumbo);
      ind_jumbo = NULL;
      return;
    }
    SLIM_LOGV("indication, payload_len=%u", ind_jumbo->payload_len);
    buffer_len = ind_jumbo->payload_len;
    buffer = (uint8_t*)calloc(1, buffer_len);
    if (buffer == NULL){
      SLIM_LOGE("buffer failed to creat ind_jumbo ");
      free(ind_jumbo);
      ind_jumbo = NULL;
      return;
    }
    memscpy(buffer, buffer_len, ind_jumbo->payload, buffer_len);
    free(ind_jumbo);
    this->_event_cb(buffer, buffer_len);
    free(buffer);
  }
  else{
    SLIM_LOGE("Unknown indication message ID %i", msg_id);
    return;
  }
}

void ssc_qmi_connection::qmi_indication_cb(qmi_client_type user_handle,
                                           unsigned int msg_id, void* ind_buf,
                                           unsigned int ind_buf_len,
                                           void* ind_cb_data)
{
    ssc_qmi_connection *conn = (ssc_qmi_connection*)ind_cb_data;
    conn->handle_indication(msg_id, ind_buf, ind_buf_len);
}

void ssc_qmi_connection::qmi_error_cb(qmi_client_type user_handle,
                                      qmi_client_error_type error,
                                      void* err_cb_data)
{
    ssc_qmi_connection* conn = (ssc_qmi_connection*)err_cb_data;
    SLIM_LOGE("error=%d", error);

    if (error != QMI_NO_ERR) {
        conn->_reconnecting = false;

    if (conn->_error_cb) {
        conn->_error_cb(SSC_CONNECTION_RESET);
    }

        SLIM_LOGI("qmi connection re-established!!!!!!!!!");
    }
}

void ssc_qmi_connection::send_request(const string& pb_req_msg_encoded)
{

    if (_reconnecting) {
        SLIM_LOGE("qmi connection failed, cannot send data");
        return;
    }

    sns_client_req_msg_v01 req_msg;

    if (pb_req_msg_encoded.size() > SNS_CLIENT_REQ_LEN_MAX_V01) {
        SLIM_LOGE("error: payload size too large");
    }

    memscpy(req_msg.payload, SNS_CLIENT_REQ_LEN_MAX_V01, pb_req_msg_encoded.c_str(),
           pb_req_msg_encoded.size());

    req_msg.use_jumbo_report_valid = true;
    req_msg.use_jumbo_report = true;
    req_msg.payload_len = pb_req_msg_encoded.size();
    qmi_client_error_type qmi_err;
    sns_client_resp_msg_v01 resp = {};
    /* send a sync message to ssc */
    qmi_err = qmi_client_send_msg_sync(_qmi_handle, SNS_CLIENT_REQ_V01,
                                       (void*)&req_msg, sizeof(req_msg),
                                       &resp,
                                       sizeof(sns_client_resp_msg_v01),
                                       QMI_RESPONSE_TIMEOUT_MS);
    if (qmi_err != QMI_NO_ERR){
      g_qmierr_cnt++;
      if (g_qmierr_cnt > QMI_ERRORS_HANDLED_ATTEMPTS) {
        SLIM_LOGD("triggred ssr _qmi_err_cnt %d", g_qmierr_cnt);
        g_qmierr_cnt = 0;
        /*if QMI_NO_ERR and ssr triggered it is surely ssr_simulate*/
        SLIM_LOGE(
            "ssr triggered after qmi_client_send_msg_sync (client_id= %" PRIu64 ") result= %d "
            "qmi_err=%d",
            resp.client_id, resp.result, qmi_err);
      } else {
        SLIM_LOGE(
            "qmi_client_send_msg_sync() failed, (client_id= %" PRIu64 ") result= %d qmi_err=%d",
            resp.client_id, resp.result, qmi_err);
      }
    } else {
      /*occassional failure of QMI , recovered with in QMI_ERRORS_HANDLED_ATTEMPTS*/
      if (g_qmierr_cnt)
        g_qmierr_cnt = 0;
    }
}

/* creates new connection to ssc */
ssc_connection::ssc_connection(ssc_event_cb event_cb) :
    _qmi_conn(make_unique<ssc_qmi_connection>(event_cb))
{
    SLIM_LOGV("ssc connected");
}

ssc_connection::~ssc_connection()
{
    SLIM_LOGV("ssc disconnected");
}

/* send encoded client request message to ssc */
void ssc_connection::send_request(const std::string& pb_req_message_encoded)
{
  if (_qmi_conn)
    _qmi_conn->send_request(pb_req_message_encoded);
  else
    SLIM_LOGE("_qmi_conn is NULL");
}

/* Error Call back function */
void ssc_connection::register_error_cb(ssc_error_cb cb)
{
    _qmi_conn->register_error_cb(cb);
}

/* Response Call back function */
void ssc_connection::register_resp_cb(ssc_resp_cb cb)
{
    _qmi_conn->register_resp_cb(cb);
}
