/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <framework/Module.h>
#include <framework/QcrilInitMessage.h>
#include "network_aidl_service.h"
#include "interfaces/nas/RilUnsolCellBarringMessage.h"
#include "interfaces/cellinfo/RilUnsolCellInfoListMessage.h"
#include "interfaces/nas/RilUnsolSignalStrengthMessage.h"
#include "interfaces/nas/RilUnsolVoiceRadioTechChangedMessage.h"
#include "interfaces/nas/RilUnsolNitzTimeReceivedMessage.h"
#include "interfaces/nas/RilUnsolRestrictedStateChangedMessage.h"
#include "interfaces/NwRegistration/RilUnsolNwRegistrationRejectMessage.h"

class NetworkAidlModule : public Module {
 public:
  NetworkAidlModule();
  ~NetworkAidlModule();
  void init();
#ifdef QMI_RIL_UTF
  void cleanup();
#endif

 private:
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage>);
  void registerAidlService(qcril_instance_id_e_type);
  void handleCdmaPrlChangedMessage(
       std::shared_ptr<RilUnsolCdmaPrlChangedMessage>);
  void handleNetworkScanResultMessage(
       std::shared_ptr<RilUnsolNetworkScanResultMessage>);
  void handleNetworkStateChangedMessage(
      std::shared_ptr<RilUnsolNetworkStateChangedMessage>);
  void handleImsNetworkStateChangedMessage(
      std::shared_ptr<RilUnsolImsNetworkStateChangedMessage>);
  void handleQcRilUnsolSuppSvcNotificationMessage(
      std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage>);
  void handleCellBarringMessage(
    std::shared_ptr<RilUnsolCellBarringMessage> msg);
  void handleCellInfoListMessage(
    std::shared_ptr<RilUnsolCellInfoListMessage> msg);
  void handleSignalStrengthMessage(
    std::shared_ptr<RilUnsolSignalStrengthMessage> msg);
  void handleVoiceRadioTechChangedMessage(
    std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg);
  void handleNitzTimeReceivedMessage(
    std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg);
  void handleLinkCapIndMessage(
      std::shared_ptr<rildata::LinkCapIndMessage> msg);
  void handlePhysicalConfigStructUpdateMessage(
      std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg);
  void handleNwRegistrationRejectMessage(
      std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg);
  void handleRestrictedStateChangedMessage(
    std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg);

  std::shared_ptr<IRadioNetworkImpl> mIRadioNetworkAidlImpl;

};

