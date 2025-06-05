/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qsh_utils_interface_reference.h"
#include "qsh_utils_qmi_reference.h"
#include "qsh_utils_glink_reference.h"

ssc_interface* ssc_interface::create(ssc_connection_type connection_type, ssc_conn_config config) {
  switch (connection_type) {
  case SSC_QMI:
    return new ssc_qmi();
  case SSC_GLINK:
    return new ssc_glink(config.glink_config);
  default:
    return nullptr;
  }
}

ssc_interface::~ssc_interface(){

}
