/*
 * Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include "qsh_utils_interface_reference.h"

class ssc_register_cb {
public:
  ssc_register_cb(ssc_resp_cb resp_cb,
                  ssc_error_cb error_cb,
                  ssc_event_cb event_cb
                  ) {
    _resp_cb = resp_cb;
    _error_cb = error_cb;
    _event_cb = event_cb;
  }

  inline ssc_resp_cb get_resp_cb() { return _resp_cb;}
  inline ssc_error_cb get_error_cb() { return _error_cb;}
  inline ssc_event_cb get_event_cb() { return _event_cb;}

private:
  ssc_resp_cb _resp_cb;
  ssc_error_cb _error_cb;
  ssc_event_cb _event_cb;
};
