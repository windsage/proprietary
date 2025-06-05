/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qwes.AndroidService;

interface IQwesStatus {

    const int QAPI_OK = 0;
    const int QAPI_ERROR = -1;
    const int QAPI_ERROR_NO_MEMORY = -3;
    const int QAPI_ERR_NO_RESOURCE = -4;
    const int QAPI_ERR_INVALID_STATE = -13;
    const int QAPI_ERR_CERT_EXPIRED_OR_NOTYETVALID = -220007;
    const int QAPI_ERR_CERT_OEM = -220008;
    const int QAPI_ERR_CERT_HWVERSION = -220009;
    const int QAPI_ERR_CERT_DEVICEID = -220010;
    const int QAPI_ERR_QWES_BAD_TOKEN_CERT = -220013;
    const int QAPI_ERR_CERT_PRODUCT_ID = -220014;

}