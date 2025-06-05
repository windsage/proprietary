/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qsh_utils_glink_reference.h"

ssc_glink::ssc_glink(ssc_glink_config config) {

}

ssc_glink::~ssc_glink() {

}
int ssc_glink::register_cb(sensor_uid suid, ssc_resp_cb resp_cb,
                           ssc_error_cb error_cb, ssc_event_cb event_cb) {
  return 0;
}
void ssc_glink::send_request(sensor_uid suid, bool use_jumbo_request, std::string encoded_buffer) {

}
