/* Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qwes.AndroidService;

import com.qualcomm.qti.qwes.AndroidService.IAttestationReportCB;

interface IQWESAS {
    /**
     * Create attestation report with given token cert.
     *
     * Put user data and the nonce into a haven token and return the
     * token to the client.  The token will be base-64 encoded.
     *
     * Client code will normally send the report to an OEM server, which
     * will forward it to the QWES cloud.  The QWES cloud will unpack the report and
     * return the app data to the OEM server.
     *
     * @param[in] tokenCert      Token certificate from the QWES cloud (required)
     * @param[in] nonce          Nonce from the QWES cloud
     * @param[in] data           Application-defined data, may be null
     * @param[out] report        Buffer to hold report
     * @param[out] reportLenOut  Length of generated report
     *
     * @return QAPI_OK if the report was created successfully.
     * @return QAPI_ERR_NO_MEMORY if report_len is too small for generated haven report.
     *                    No data will be written to the report buffer, but
     *                    report_len_out will be set to the required size.
     * @return QAPI_ERR_NO_RESOURCE if the QWES service is not accessible.
     * @return QAPI_ERR_INVALID_STATE if the device security has been compromised.
     * @return QAPI_ERR_CERT_EXPIRED_OR_NOTYETVALID if the license is expired or not yet valid.
     * @return QAPI_ERR_CERT_OEM if the OEM id is invalid or not allowed for this vendor.
     * @return QAPI_ERR_CERT_HWVERSION if the HW version is not allowed for this chip family.
     * @return QAPI_ERR_CERT_DEVICEID if the device id is not allowed for this device.
     * @return QAPI_ERR_QWES_BAD_TOKEN_CERT if the provided token cert could not be used.
     * @return QAPI_ERR_CERT_PRODUCT_ID if the product id is not allowed for this model.
     * @return QAPI_ERROR if tokenCert is not provided, or for any other errors.
     */
    int createAttestationReport(in byte[] tokenCert, in byte[] nonce, in byte[] data, out byte[] report, out int[] reportLenOut);

    void createAttestationReportAuto(in byte[] data, in boolean bringUpNetwork, in IAttestationReportCB callback);

}