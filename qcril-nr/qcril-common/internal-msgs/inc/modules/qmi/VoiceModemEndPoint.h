/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <vector>

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/VoiceModemEndPointModule.h"
#include "framework/Log.h"

class QmiVoiceNotifyClirModeIndMessage
    : public QmiIndicationMessage<QmiVoiceNotifyClirModeIndMessage,
                                  voice_notify_clir_mode_ind_msg_v02>,
      public add_message_id<QmiVoiceNotifyClirModeIndMessage> {
 public:
  static const constexpr char* MESSAGE_NAME = "QmiVoiceNotifyClirModeIndMessage";
  using payload_t = voice_notify_clir_mode_ind_msg_v02;

  QmiVoiceNotifyClirModeIndMessage() = delete;
  ~QmiVoiceNotifyClirModeIndMessage() = default;

  QmiVoiceNotifyClirModeIndMessage(const voice_notify_clir_mode_ind_msg_v02& data)
      : QmiIndicationMessage(data) {
  }
  std::string dump() {
    return MESSAGE_NAME;
  }
  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QmiVoiceNotifyClirModeIndMessage>(getPayload());
  }
  virtual bool isSameMessage(std::shared_ptr<Message> msg) override {
    if (msg) {
      return (get_message_id() == msg->get_message_id());
    }
    return false;
  }
};

class QmiVoiceNetworkDetectedEccNumberIndMessage
    : public QmiIndicationMessage<QmiVoiceNetworkDetectedEccNumberIndMessage,
                                  voice_network_detected_ecc_number_ind_msg_v02>,
      public add_message_id<QmiVoiceNetworkDetectedEccNumberIndMessage> {
 public:
  static const constexpr char* MESSAGE_NAME = "QmiVoiceNetworkDetectedEccNumberIndMessage";
  using payload_t = voice_network_detected_ecc_number_ind_msg_v02;

  QmiVoiceNetworkDetectedEccNumberIndMessage() = delete;
  ~QmiVoiceNetworkDetectedEccNumberIndMessage() = default;

  QmiVoiceNetworkDetectedEccNumberIndMessage(
      const voice_network_detected_ecc_number_ind_msg_v02& data)
      : QmiIndicationMessage(data) {
  }
  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<QmiVoiceNetworkDetectedEccNumberIndMessage>(getPayload());
  }
  virtual bool isSameMessage(std::shared_ptr<Message> msg) override {
    if (msg) {
      return (get_message_id() == msg->get_message_id());
    }
    return false;
  }
};

class VoiceModemEndPoint : public ModemEndPoint {
 public:
  static constexpr const char *NAME = "VOICE";
  VoiceModemEndPoint() : ModemEndPoint(NAME) {
    mModule = new VoiceModemEndPointModule("VoiceModemEndPointModule", *this);
    mModule->init();
  }
  ~VoiceModemEndPoint() {
      Log::getInstance().d("[VoiceModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, qcril_instance_id_e_type id,
        GenericCallback<string>* cb);
};
