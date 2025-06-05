/*
 * Copyright (c) 2017-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <InstallKeybox.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "cutils/properties.h"

#include "common_v01.h"
#include "device_management_service_v01.h"
#include "qmi_client.h"

#define QMI_CLIENT_SRV_REG_TIMEOUT (5000)
#define QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT (5000)

using namespace std;

int InstallKeybox::provisionDeviceID(void) {
    qmi_client_error_type qmi_client_error = 0;
    dms_get_device_serial_numbers_resp_msg_v01 dms_dev_serial_num_resp;
    qmi_idl_service_object_type service_object = dms_get_service_object_v01();
    qmi_client_os_params os_params;
    bool disconnect = false;
    qmi_client_type client_handle = NULL;
    bool with_modem = true;
    dms_bind_subscription_req_msg_v01 dms_bind_request;
    dms_bind_subscription_resp_msg_v01 dms_bind_resp;

    if (mProvisionDeviceId != true) return KM_ERROR_OK;

    memset(&dms_dev_serial_num_resp, 0, sizeof(dms_dev_serial_num_resp));
    memset(&dms_bind_request, 0, sizeof(dms_bind_request));
    memset(&dms_bind_resp, 0, sizeof(dms_bind_resp));

    if (!service_object) {
        cerr << "service_object NULL" << endl;
        return -1;
    }

    qmi_client_error =
            qmi_client_init_instance(service_object, QMI_CLIENT_INSTANCE_ANY, NULL, NULL,
                                     &os_params, QMI_CLIENT_SRV_REG_TIMEOUT, &client_handle);
    if (qmi_client_error != 0) {
        if (qmi_client_error == QMI_TIMEOUT_ERR) {
            with_modem = false;
        } else {
            cerr << "qmi_client_init_instance error" << qmi_client_error << endl;
            return -1;
        }
    }
    disconnect = true;
    // ** fetch data
    if (with_modem) {
        size_t dev_id_cnt = 0;
        for(dev_id_cnt = 0; dev_id_cnt < MAX_SUBS; dev_id_cnt++)  {

            memset(&dms_dev_serial_num_resp, 0, sizeof(dms_dev_serial_num_resp));
            memset( &dms_bind_request, 0, sizeof(dms_bind_request));
            memset( &dms_bind_resp, 0, sizeof(dms_bind_resp));

            if (dev_id_cnt == 0) {
                dms_bind_request.bind_subs = DMS_PRIMARY_SUBS_V01;
            }
            else if (dev_id_cnt == 1) {
                dms_bind_request.bind_subs = DMS_SECONDARY_SUBS_V01;
            }
            else {
                dms_bind_request.bind_subs = DMS_TERTIARY_SUBS_V01;
            }

            qmi_client_error = qmi_client_send_msg_sync(client_handle,
                                                        QMI_DMS_BIND_SUBSCRIPTION_REQ_V01,
                                                        (void*)&dms_bind_request,
                                                        sizeof(dms_bind_request),
                                                        (void*)&dms_bind_resp,
                                                        sizeof(dms_bind_resp),
                                                        QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);

            if (0 != qmi_client_error) {
                cerr << "qmi_client_send_msg_sync error for bind_sub" << qmi_client_error << " " << dev_id_cnt << endl;
                break;
            }

            qmi_client_error = qmi_client_send_msg_sync(client_handle,
                                                        QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
                                                        NULL,
                                                        0,  // empty request payload
                                                        (void*)&dms_dev_serial_num_resp,
                                                        sizeof(dms_dev_serial_num_resp),
                                                        QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);

            if (0 != qmi_client_error) {
                cerr << "qmi_client_send_msg_sync error for get dev_ser_num" << qmi_client_error << endl;
                break;
            }
            cerr << "qmi_client_send_msg_sync success for get dev_ser_num " << dev_id_cnt <<  dms_dev_serial_num_resp.imei_valid << dms_dev_serial_num_resp.meid_valid << endl;
            if (dms_dev_serial_num_resp.imei_valid) {
                Imei.dev_id[Imei.dev_id_count].assign(dms_dev_serial_num_resp.imei, strlen(dms_dev_serial_num_resp.imei));
                cerr << "Imei: " << Imei.dev_id[Imei.dev_id_count] << endl;
                Imei.dev_id_count++;
            }
            if (dms_dev_serial_num_resp.meid_valid) {
                Meid.dev_id[Meid.dev_id_count].assign(dms_dev_serial_num_resp.meid, strlen(dms_dev_serial_num_resp.meid));
                cerr << "Meid: " << Meid.dev_id[Meid.dev_id_count] << endl;
                Meid.dev_id_count++;
            }
        }

        if (disconnect) {
            disconnect = false;
            qmi_client_release(client_handle);
        }

        /* Return error only if we are not able to get device serial numbers for atlest
           one subscription when modem is present, API call to Bind SUB may fail for SS devices
           hence dont return error */
        if (0 != qmi_client_error && dev_id_cnt == 0) {
            return -1;
        }
    }

    char paramstr[PROPERTY_VALUE_MAX];
    property_get("ro.product.brand", paramstr, "");
    Brand.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.device", paramstr, "");
    Device.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.name", paramstr, "");
    Product.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.serialno", paramstr, "");
    SerialNum.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.manufacturer", paramstr, "");
    Manufacturer.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.model", paramstr, "");
    Model.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    cerr << "Brand: " << Brand << endl;
    cerr << "Device: " << Device << endl;
    cerr << "Product: " << Product << endl;
    cerr << "SerialNum: " << SerialNum << endl;
    cerr << "Manufacturer: " << Manufacturer << endl;
    cerr << "Model: " << Model << endl;

    int result = provisioningClient->KeyMasterProvisionDeviceId(Brand, Device, Product, SerialNum,
                                                                Manufacturer, Model, Imei, Meid);
    if (result) {
        cerr << "KeyMasterProvisionDeviceId error" << result << endl;
        if (disconnect) qmi_client_release(client_handle);
        return result;
    }
    return 0;
}
