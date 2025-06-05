/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/VoiceModemEndPoint.h"

#include "common_v01.h"
#include "device_management_service_v01.h"
#include "voice_service_v02.h"
#include "modules/qmi/QmiSetupRequest.h"

using std::to_string;
constexpr const char *VoiceModemEndPoint::NAME;

void VoiceModemEndPoint::requestSetup(string clientToken,
        qcril_instance_id_e_type id, GenericCallback<string>* callback) {
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(shared_setupMsg);
}
