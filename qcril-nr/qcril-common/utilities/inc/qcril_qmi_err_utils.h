/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef QCRIL_QMI_ERR_UTILS_H
#define QCRIL_QMI_ERR_UTILS_H

#include "telephony/ril.h"
#include "qcril_qmi_client.h"
#include "qmi_client.h"
#include "common_v01.h"

RIL_Errno qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
    qmi_client_error_type qmi_transport_error, qmi_response_type_v01* qmi_service_response);

RIL_Errno qcril_qmi_util_convert_qmi_response_codes_to_ril_result_ex(
    qmi_client_error_type qmi_transport_error, qmi_response_type_v01* qmi_service_response,
    qmi_ril_err_context_e_type context, void* any);

#endif
