/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include "qsh_utils_interface_reference.h"

class ssc_glink : public ssc_interface {
public:
  ssc_glink(ssc_glink_config config);
  ~ssc_glink();
  int register_cb(sensor_uid suid, ssc_resp_cb resp_cb,
                  ssc_error_cb error_cb, ssc_event_cb event_cb);
  void send_request(sensor_uid suid, bool use_jumbo_request, std::string encoded_buffer);
};
