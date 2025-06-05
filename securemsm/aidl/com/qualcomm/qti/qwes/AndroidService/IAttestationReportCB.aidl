/* Copyright (c) 2019 Qualcomm Technologies, Inc.
  * All Rights Reserved.
  * Confidential and Proprietary - Qualcomm Technologies, Inc.
  */

// IAttestationReportCB.aidl
package com.qualcomm.qti.qwes.AndroidService;

interface IAttestationReportCB {

     oneway void handleAttestationReport(int status, in byte[] report);

}