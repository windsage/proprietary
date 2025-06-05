/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "device_management_service_v01.h"
#include "qmi_client.h"
#include "common_v01.h"
#include "voice_service_v02.h"

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

class VoiceModemEndPointModule : public ModemEndPointModule {
 public:
  VoiceModemEndPointModule(string name, ModemEndPoint& owner);
  ~VoiceModemEndPointModule();

 private:
  qmi_idl_service_object_type getServiceObject() override;
  bool handleQmiBinding(qcril_instance_id_e_type instanceId, int8_t stackId) override;
  void onQmiUnsolIndCb(qmi_client_type user_handle, unsigned int msg_id, void* ind_buf,
                       unsigned int ind_buf_len) override;
};
