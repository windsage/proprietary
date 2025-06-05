/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef _QTI_RADIO_STABLE_AIDL_H_
#define _QTI_RADIO_STABLE_AIDL_H_

#include "framework/Module.h"
#include <framework/QcrilInitMessage.h>
#include "stable_aidl_impl/qti_radio_stable_aidl_service.h"
#include "interfaces/dms/QcRilUnsolImeiMessage.h"
#include "UnSolMessages/AutoDdsSwitchControlIndMessage.h"
#include "UnSolMessages/DdsSwitchRecommendationIndMessage.h"
#include "UnSolMessages/ConfigureDeactivateDelayTimeMessage.h"
#include "UnSolMessages/DataCapabilityChangeIndMessage.h"
#include "interfaces/nas/RilUnsolNetworkScanResultMessage.h"

namespace aidlimplimports {
using namespace aidl::vendor::qti::hardware::radio::qtiradio::implementation;
}

namespace aidlutils {
using namespace aidl::vendor::qti::hardware::radio::qtiradio::utils;
}

class QtiRadioStableAidl : public Module {
 public:
  QtiRadioStableAidl();
  ~QtiRadioStableAidl();
  void init();
  void cleanup();

 private:
  std::string mServiceName = "vendor.qti.hardware.radio.qtiradio.IQtiRadioStable";
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void handleDataNrIconTypeIndMessage(std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg);
  void handleQcRilUnsolImeiMessage(std::shared_ptr<QcRilUnsolImeiMessage> msg);
  void handleAutoDdsSwitchControlIndMessage(std::shared_ptr<rildata::AutoDdsSwitchControlIndMessage> msg);
  void handleDdsSwitchRecommendationIndMessage(std::shared_ptr<rildata::DdsSwitchRecommendationIndMessage> msg);
  void handleConfigureDeactivateDelayTimeMessage(std::shared_ptr<rildata::ConfigureDeactivateDelayTimeMessage> msg);
  void handleDataCapabilityChangeIndMessage(std::shared_ptr<rildata::DataCapabilityChangeIndMessage> msg);
  void handlePdcRefreshMessage(std::shared_ptr<QcRilUnsolPdcRefreshMessage> msg);
  void handleNetworkScanResultMessage(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg);
  void handleSimPersoLockStatusInd(std::shared_ptr<UimSimPersoLockStatusInd> msg);
  std::shared_ptr<aidlimplimports::QtiRadioStable> mQtiRadioStable = nullptr;
};

#endif  // _QTI_RADIO_STABLE_AIDL_H_
