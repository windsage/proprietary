
/********************************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *********************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include "cutils/properties.h"

#include "common_v01.h"
#include "device_management_service_v01.h"
#include "qmi_client.h"
#include "InstallKeybox.h"
#include "KeymasterKeyProvision.h"

using namespace std;

namespace keymasterdevice {

#define QMI_CLIENT_SRV_REG_TIMEOUT (5000)
#define QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT (5000)

typedef enum {
    TAG_NUM_OF_PARAMS = 700,
    TAG_ATTESTATION_ID_BRAND,
    TAG_ATTESTATION_ID_DEVICE,
    TAG_ATTESTATION_ID_PRODUCT,
    TAG_ATTESTATION_ID_SERIAL,
    TAG_ATTESTATION_ID_IMEI,
    TAG_ATTESTATION_ID_MEID,
    TAG_ATTESTATION_ID_MANUFACTURER,
    TAG_ATTESTATION_ID_MODEL
} device_id_tag;

int InstallKeybox::provisionDeviceIds() {

    char paramstr[PROPERTY_VALUE_MAX];
    qmi_client_error_type qmi_client_error = 0;
    dms_get_device_serial_numbers_resp_msg_v01 dms_dev_serial_num_resp;
    qmi_idl_service_object_type service_object = dms_get_service_object_v01();
    qmi_client_os_params os_params;
    qmi_client_type client_handle = NULL;
    dms_bind_subscription_req_msg_v01 dms_bind_request;
    dms_bind_subscription_resp_msg_v01 dms_bind_resp;
    bool is_modem = true;

    //Get IMEI/MEID from the modem
    memset(&dms_dev_serial_num_resp, 0, sizeof(dms_dev_serial_num_resp));
    memset(&dms_bind_request, 0, sizeof(dms_bind_request));
    memset(&dms_bind_resp, 0, sizeof(dms_bind_resp));

    if (!service_object) {
        cerr << "service_object NULL" << endl;
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }

    qmi_client_error =
            qmi_client_init_instance(service_object, QMI_CLIENT_INSTANCE_ANY, NULL, NULL,
                    &os_params, QMI_CLIENT_SRV_REG_TIMEOUT, &client_handle);
    if (qmi_client_error != 0) {
        if (qmi_client_error == QMI_TIMEOUT_ERR) {
            is_modem = false;
        } else {
            cerr << "qmi_client_init_instance error: " << qmi_client_error << endl;
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (is_modem) {
        size_t dev_id_cnt = 0;
        for(dev_id_cnt = 0; dev_id_cnt < KM_MAX_SUBS; dev_id_cnt++)  {

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
                cerr << "qmi_client_send_msg_sync error for bind_sub: " << qmi_client_error
                        << ", count: " << dev_id_cnt << endl;
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
                cerr << "qmi_client_send_msg_sync error for get dev_ser_num: " << qmi_client_error << endl;
                break;
            }
            cerr << "qmi_client_send_msg_sync success for get dev_ser_num " << dev_id_cnt <<  dms_dev_serial_num_resp.imei_valid << dms_dev_serial_num_resp.meid_valid << endl;
            if (dms_dev_serial_num_resp.imei_valid) {
                mImei.dev_id[mImei.dev_id_count].assign(dms_dev_serial_num_resp.imei, strlen(dms_dev_serial_num_resp.imei));
                cerr << "mImei: " << mImei.dev_id[mImei.dev_id_count] << endl;
                mImei.dev_id_count++;
            }
            if (dms_dev_serial_num_resp.meid_valid) {
                mMeid.dev_id[mMeid.dev_id_count].assign(dms_dev_serial_num_resp.meid, strlen(dms_dev_serial_num_resp.meid));
                cerr << "mMeid: " << mMeid.dev_id[mMeid.dev_id_count] << endl;
                mMeid.dev_id_count++;
            }
        }

        qmi_client_release(client_handle);

        /* Return error only if we are not able to get device serial numbers for
         * at least one subscription when modem is present, API call to Bind SUB
         * may fail for some devices hence don't return error
         */
        if (0 != qmi_client_error && dev_id_cnt == 0) {
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }

    property_get("ro.product.brand", paramstr, "");
    mBrand.assign(paramstr, strlen(paramstr));
    cout << "brand " <<  paramstr << endl;
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.device", paramstr, "");
    mDevice.assign(paramstr, strlen(paramstr));
    cout << "device " <<  paramstr << endl;
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.name", paramstr, "");
    mProduct.assign(paramstr, strlen(paramstr));
    cout << "name " <<  paramstr << endl;
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.serialno", paramstr, "");
    mSerialNum.assign(paramstr, strlen(paramstr));
    cout << "serial " <<  paramstr << endl;
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.manufacturer", paramstr, "");
    mManufacturer.assign(paramstr, strlen(paramstr));
    cout << "manufacturer " <<  paramstr << endl;
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.model", paramstr, "");
    mModel.assign(paramstr, strlen(paramstr));
    cout << "model " <<  paramstr << endl;
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    uint8_t prov_cmd_buffer[SPU_KM_SHARED_BUF_LEN];
    uint32_t prov_cmd_buffer_size = sizeof(prov_cmd_buffer);

    int result = mProvisioningClient->KeyMasterProvisionDeviceId(
            mBrand, mDevice, mProduct, mSerialNum, mManufacturer,
            mModel, mImei, mMeid, prov_cmd_buffer, &prov_cmd_buffer_size);

    if (result) {
        cerr << "Failed to create device IDs provisioninig request: " << result << endl;
        return result;
    }

    keymaster_error_t ret = mKeymasterTzUtils->tzSpuKmProvisionDeviceId(
            prov_cmd_buffer, prov_cmd_buffer_size);
    if (ret != KM_ERROR_OK) {
        cerr << "Failed to provision device IDs: " << ret << endl;
        return ret;
    }

    ret = mKeymasterTzUtils->tzServiceLock(KeymasterTzUtils::TZ_KEY_ID_DEVICE_IDS);
    if (ret != KM_ERROR_OK) {
        cerr << "Failed to lock device IDs partition: " << ret << endl;
        return ret;
    }

    cout << "Successfully provisioned device ID" << endl;

    return KM_ERROR_OK;
}

};  // namespace keymasterdevice

