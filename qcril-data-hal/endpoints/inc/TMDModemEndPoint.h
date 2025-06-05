/**
* Copyright (c) 2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#ifndef TMDMODEMENDPOINT
#define TMDMODEMENDPOINT
#include "modules/qmi/ModemEndPoint.h"
#include "TMDModemEndPointModule.h"
#include "request/SetDataThrottlingRequestMessage.h"
#include "framework/Log.h"

namespace rildata {

enum DataThrottlingDevice_t : uint8_t {
  ANCHOR_CARRIER    = 0,
  SECONDARY_CARRIER = 1,
};

struct DeviceMitigationLevel_t {
  DataThrottlingDevice_t carrier;
  uint8_t maxLevel;
  uint8_t current;
  uint8_t requested;
  uint8_t target;
  TimeKeeper::timer_id timerId;
  TimeKeeper::millisec interval;
};

class TMDModemEndPoint : public ModemEndPoint
{
public:
  static constexpr const char *NAME = "TMDModemEndPoint";

  TMDModemEndPoint() : ModemEndPoint(NAME) {
    mModule = new TMDModemEndPointModule("TMDModemEndPointModule", *this);
    mModule->init();
    Log::getInstance().d("[TMDModemEndPoint]: xtor");
  }

  ~TMDModemEndPoint() {
    Log::getInstance().d("[TMDModemEndPoint]: destructor");
    delete mModule;
    mModule = nullptr;
  }
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);

  DeviceMitigationLevel_t getCurrentMitigationLevel(std::string device);
  Message::Callback::Status setMitigationLevel(std::string device, int level);
};

} // namespace
#endif