/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef PDCMODEMENDPOINT
#define PDCMODEMENDPOINT
#include "modules/qmi/ModemEndPoint.h"
#include "PdcModemEndPointModule.h"
#include "framework/Log.h"

namespace rildata {

class PdcModemEndPoint : public ModemEndPoint
{
public:
  static constexpr const char *NAME = "PdcModemEndPoint";
  PdcModemEndPoint() : ModemEndPoint(NAME)
  {
    mModule = new PdcModemEndPointModule("PdcModemEndPointModule", *this);
    mModule->init();
    Log::getInstance().d("[PdcModemEndPoint]: xtor");
  }
  ~PdcModemEndPoint()
  {
    Log::getInstance().d("[PdcModemEndPoint]: destructor");
    delete mModule;
    mModule = nullptr;
  }
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
};

}

#endif
