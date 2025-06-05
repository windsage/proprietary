/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/qmi/SmsModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

using std::to_string;
constexpr const char *SmsModemEndPoint::NAME;

void SmsModemEndPoint::requestSetup(string clientToken,
        qcril_instance_id_e_type id, GenericCallback<string>* callback) {
  Log::getInstance().d("[SmsModemEndPoint]: requestSetup");
  auto setupMsg = std::make_shared<QmiSetupRequest>
    (clientToken, 0, nullptr, id, callback);
  mModule->dispatch(setupMsg);
}

