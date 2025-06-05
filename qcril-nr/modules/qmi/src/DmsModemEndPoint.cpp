/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/DmsModemEndPoint.h"
#include "common_v01.h"
#include "device_management_service_v01.h"
#include "qmi_client.h"
#include "voice_service_v02.h"
#include "modules/qmi/QmiSetupRequest.h"

using std::to_string;

void DmsModemEndPoint::requestSetup(string clientToken, qcril_instance_id_e_type id,
                                    GenericCallback<string>* callback) {
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}
