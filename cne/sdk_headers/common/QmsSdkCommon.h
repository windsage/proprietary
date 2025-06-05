/*==============================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: status types should be global defined in a header for all SDKs.
typedef enum {
    /* No error in the request */
    SUCCESS = 0,
    /* Invalid Status */
    INVALID = -1,
    /* Invalid Parameter[s] */
    INVALID_ARG = -2,
    /* Unexpected error occurred during processing */
    INTERNAL_ERR = -3,
    /* Unsupported command */
    NOT_SUPPORTED = -4,
    /* Request is issued on unexpected nDDS */
    INVALID_OPERATION = -5,
    /* Response to client register when the service is up */
    SERVICE_UP = -6,
    /* Response to client register when the service is down */
    SERVICE_DOWN = -7,
    /* No sim available on which the parameters are configured */
    INVALID_SLOT = -8,
    /* License needs to be registered again */
    LICENSE_RESET = -9,
    /* License unknown */
    LICENSE_UNKNOWN = -10,
    /* License invalid */
    LICENSE_INVALID = -11,
    /* License valid */
    LICENSE_VALID = -12
} status_e;

/*===========================================================================
  FUNCTION:  service_status_ind
===========================================================================*/
/** @ingroup service_status_ind

    Indication function to send the service status updates to the client.

    @param[in] status, service status (up / down).

    @return none

    @dependencies This API will be triggered when there is a change in service status.
*/
/*=========================================================================*/
typedef void (*service_status_ind)(status_e status);
// TODO: This might potentially update the QMCT client as well. Check if ok. Else leave as service_status_cb

#ifdef __cplusplus
} // Extern "C"
#endif
