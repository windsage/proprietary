/******************************************************************************
#  Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <cstring>
#include "QtiRadioStableAidl.h"
#include <framework/Log.h>
#include "stable_aidl_impl/qti_radio_stable_aidl_service.h"

static load_module<QtiRadioStableAidl> sQtiRadio;

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
QtiRadioStableAidl::QtiRadioStableAidl() {
  mName = "QtiRadioStableAidl";
  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, QtiRadioStableAidl::handleQcrilInit),
    HANDLER(rildata::DataNrIconTypeIndMessage, QtiRadioStableAidl::handleDataNrIconTypeIndMessage),
    HANDLER(QcRilUnsolImeiMessage, QtiRadioStableAidl::handleQcRilUnsolImeiMessage),
    HANDLER(rildata::AutoDdsSwitchControlIndMessage, QtiRadioStableAidl::handleAutoDdsSwitchControlIndMessage),
    HANDLER(rildata::DdsSwitchRecommendationIndMessage, QtiRadioStableAidl::handleDdsSwitchRecommendationIndMessage),
    HANDLER(rildata::ConfigureDeactivateDelayTimeMessage, QtiRadioStableAidl::handleConfigureDeactivateDelayTimeMessage),
    HANDLER(rildata::DataCapabilityChangeIndMessage, QtiRadioStableAidl::handleDataCapabilityChangeIndMessage),
    HANDLER(QcRilUnsolPdcRefreshMessage, QtiRadioStableAidl::handlePdcRefreshMessage),
    HANDLER(RilUnsolNetworkScanResultMessage, QtiRadioStableAidl::handleNetworkScanResultMessage),
    HANDLER(UimSimPersoLockStatusInd, QtiRadioStableAidl::handleSimPersoLockStatusInd),
  };
}

/* Follow RAII.
 */
QtiRadioStableAidl::~QtiRadioStableAidl() {
}

/*
 * Module specific initialization that does not belong to RAII .
 */
void QtiRadioStableAidl::init() {
  Module::init();
}

void QtiRadioStableAidl::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " + std::to_string(instance_id));
  if (mQtiRadioStable == nullptr) {
    mQtiRadioStable = ndk::SharedRefBase::make<aidlimplimports::QtiRadioStable>();
    std::string serviceInstance = mServiceName + "/slot" + std::to_string(instance_id + 1);
    binder_status_t status =
        AServiceManager_addService(mQtiRadioStable->asBinder().get(), serviceInstance.c_str());
    QCRIL_LOG_INFO("QtiRadio Stable AIDL addService, status=%d", status);
    if (status != STATUS_OK) {
      mQtiRadioStable = nullptr;
      QCRIL_LOG_ERROR("Error registering service %s", serviceInstance.c_str());
    }
  }
}

void QtiRadioStableAidl::handleDataNrIconTypeIndMessage(
    std::shared_ptr<rildata::DataNrIconTypeIndMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  aidlimports::NrIconType iconType = aidlutils::convert_nrIconType_from_indication(msg);
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyOnNrIconTypeChange(iconType);
  }
}

void QtiRadioStableAidl::handleQcRilUnsolImeiMessage(
    std::shared_ptr<QcRilUnsolImeiMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  aidlimports::ImeiInfo imeiInfo;
  aidlutils::convert_imei_info(imeiInfo, msg->getImeiInfo());
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyOnImeiChange(imeiInfo);
  }
}

void QtiRadioStableAidl::handleAutoDdsSwitchControlIndMessage(
    std::shared_ptr<rildata::AutoDdsSwitchControlIndMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if(mQtiRadioStable == nullptr) {
    Log::getInstance().d("[" + mName + "]: QtiRadioStableAidl service is not created");
    return;
  }

  if(msg->getAutoDdsSwitchControl() == rildata
      ::AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_CAPABILITY_CHANGED) {
     mQtiRadioStable->notifyDdsSwitchCapabilityChange();
  } else if(msg->getAutoDdsSwitchControl() == rildata
      ::AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_ENABLED) {
    mQtiRadioStable->notifyDdsSwitchCriteriaChange(true);
  } else if(msg->getAutoDdsSwitchControl() == rildata
      ::AutoDdsSwitchControl_t::AUTO_DDS_SWITCH_DISABLED) {
    mQtiRadioStable->notifyDdsSwitchCriteriaChange(false);
  }
}

void QtiRadioStableAidl::handleDdsSwitchRecommendationIndMessage(
    std::shared_ptr<rildata::DdsSwitchRecommendationIndMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyDdsSwitchRecommendation((int32_t)msg->getSubscriptionId());
  }
}

void QtiRadioStableAidl::handleConfigureDeactivateDelayTimeMessage(
    std::shared_ptr<rildata::ConfigureDeactivateDelayTimeMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyConfigureDeactivateDelayTime((int64_t)msg->getDelayTimeMilliSecs());
  }
}

void QtiRadioStableAidl::handleDataCapabilityChangeIndMessage(
    std::shared_ptr<rildata::DataCapabilityChangeIndMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyDataCapabilityChange(msg->getCIWlanSupported());
  }
}

void QtiRadioStableAidl::handlePdcRefreshMessage(
    std::shared_ptr<QcRilUnsolPdcRefreshMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyOnPdcRefresh(msg);
  }
}

void QtiRadioStableAidl::handleNetworkScanResultMessage(
    std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mQtiRadioStable) {
    mQtiRadioStable->sendNetworkScanResult(msg);
  }
}

void QtiRadioStableAidl::handleSimPersoLockStatusInd(
    std::shared_ptr<UimSimPersoLockStatusInd> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  if (mQtiRadioStable) {
    mQtiRadioStable->notifyOnSimPersoLockStatus(msg);
  }
}

void QtiRadioStableAidl::cleanup()
{
  if (mQtiRadioStable) {
    mQtiRadioStable->cleanup();
  }
}

void qti_radio_aidl_module_reboot_cleanup()
{
  sQtiRadio.get_module().cleanup();
}
