/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <string>

/**
 * @brief Struct to represent sensor's unique ID (128-bit)
 */
struct sensor_uid
{
    sensor_uid() : low(0), high(0) {}
    sensor_uid(uint64_t low, uint64_t high): low(low), high(high) {}
    bool operator==(const sensor_uid& rhs) const
    {
        return (low == rhs.low && high == rhs.high);
    }
    uint64_t low, high;
};

/*
 * Type of transport to establish the connection
 * SSC_QMI : for QMI as transport layer
 * SSC_GLINK: For Glink as transport layer
 * */
typedef enum ssc_connection_type{
  SSC_QMI,
  SSC_GLINK,
}ssc_connection_type;

 /*
 * error type if any from ssc_interface point of view
 * */
enum ssc_interface_error
{
    SSC_INTERFACE_RESET
};

/**
 * type alias for ssc_interface resp, error and event callbacks respectively.
 */
using ssc_resp_cb = std::function<void(const uint32_t error_value, uint64_t client_connect_id)>;
using ssc_error_cb = std::function<void(ssc_interface_error error)>;
using ssc_event_cb = std::function<void(const uint8_t *data, size_t size, uint64_t ts)>;

/*
* @brief: Configuration parameters exposed to client
*         for QMI transport layer.
* */
typedef struct ssc_qmi_config{
}ssc_qmi_config;

/*
* @brief: Configuration parameters exposed to client
*         for Glink transport layer.
* @Param: glink transport name for the requested connection.
* */
typedef struct ssc_glink_config{
  char* trasnport_name;
}ssc_glink_config;

/*
* @brief: Configuration parameters exposed to client
*         for the requested connection
* @Paras:  can be one of connection type config
* */
typedef struct ssc_conn_config {
  union {
    ssc_qmi_config qmi_config;
    ssc_glink_config glink_config;
  };
}ssc_conn_config;

/*
 * Interface APIs to interact with sensors core from sensors Apps
 * */
class ssc_interface {
public:
  /* @brief: Create the interface between the clients and sensors core.
   * @param: transport layer type as per ssc_connection_type
   *
   * */
  static ssc_interface* create(ssc_connection_type connection_type, ssc_conn_config config_params);

  /**
   * @brief registers the callbacks for the given suid.
   *        Client needs to call only once per given suid.
   *
   * @param: callback pointers, resp, error and event.
   *         All are mandatory. Client can pass nullptr incase do not need any.
   *
   * @out: 0 upon success & -1 upon failure.
   */
  virtual int register_cb(sensor_uid suid, ssc_resp_cb resp_cb,
                          ssc_error_cb error_cb, ssc_event_cb event_cb) = 0;

  /**
   * @brief send proto encoded message to sensors core via QMI.
   *
   * @param: suid of the given sensor
   *
   * @param: use_jumbo_request True if client needs jumbo event packet , else false
   *
   * @param: encoded_buffer: proto encoded request message
   */
  virtual void send_request(sensor_uid suid,
                            bool use_jumbo_request, std::string encoded_buffer) = 0;
  virtual ~ssc_interface();
protected:
};
