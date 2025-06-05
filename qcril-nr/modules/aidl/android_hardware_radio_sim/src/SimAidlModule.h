/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "sim_aidl_service.h"

#include "interfaces/gstk/GstkUnsolIndMsg.h"
#include "interfaces/nas/RilUnsolCdmaSubscriptionSourceChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccAppsStatusChangedMessage.h"
#include "interfaces/nas/RilUnsolUiccSubsStatusChangedMessage.h"
#include "interfaces/pbm/QcRilUnsolAdnRecordsOnSimMessage.h"
#include "interfaces/pbm/QcRilUnsolPhonebookRecordsUpdatedMessage.h"
#include "interfaces/uim/UimSimRefreshIndication.h"
#include "interfaces/uim/UimSimStatusChangedInd.h"
#include "request/SetCarrierInfoImsiEncryptionMessage.h"

#include "UnSolMessages/CarrierInfoForImsiEncryptionRefreshMessage.h"

class SimAidlModule : public Module {
 public:
  SimAidlModule();
  ~SimAidlModule();
  void init();

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);
  void registerAidlService(qcril_instance_id_e_type instance_id);

  void handleCarrierInfoForImsiEncryptionRefreshMessage(std::shared_ptr<rildata::CarrierInfoForImsiEncryptionRefreshMessage> msg);
  void handleCdmaSubscriptionSourceChangedMessage(
      std::shared_ptr<RilUnsolCdmaSubscriptionSourceChangedMessage> msg);
  void handlePhonebookRecordsChangedMessage(
      std::shared_ptr<QcRilUnsolPhonebookRecordsUpdatedMessage> msg);
  void handleAdnRecordsOnSimMessage(std::shared_ptr<QcRilUnsolAdnRecordsOnSimMessage> msg);
  void handleUimSimRefreshIndication(std::shared_ptr<UimSimRefreshIndication> msg);
  void handleUimSimStatusChangedInd(std::shared_ptr<UimSimStatusChangedInd> msg);
  void handleGstkUnsolIndMsg(std::shared_ptr<GstkUnsolIndMsg> msg);
  void handleUiccSubsStatusChangedMessage(std::shared_ptr<RilUnsolUiccSubsStatusChangedMessage> msg);
  void handleUiccAppsStatusChangedMessage(std::shared_ptr<RilUnsolUiccAppsStatusChangedMessage> msg);

  std::shared_ptr<IRadioSimImpl> mIRadioSimAidlImpl;
};
