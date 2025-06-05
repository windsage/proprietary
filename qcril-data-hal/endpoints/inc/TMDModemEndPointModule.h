/**
* Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef TMDMODEMENDPOINTMODULE
#define TMDMODEMENDPOINTMODULE
#include "thermal_mitigation_device_service_v01.h"
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/ModemEndPointModule.h"
#include "MessageCommon.h"
#define QCRIL_DATA_QMI_TIMEOUT 10000

using namespace rildata;

class TMDModemEndPointModule : public ModemEndPointModule
{
private:
  qmi_idl_service_object_type getServiceObject() override;

public:
  TMDModemEndPointModule(string name, ModemEndPoint& owner);
  void handleQmiClientSetup(std::shared_ptr<QmiSetupRequest> shared_setupMsg);
  void handleQmiServiceUp(std::shared_ptr<QmiServiceUpIndMessage> msg);
  virtual ~TMDModemEndPointModule();
  void init();
};

#endif
