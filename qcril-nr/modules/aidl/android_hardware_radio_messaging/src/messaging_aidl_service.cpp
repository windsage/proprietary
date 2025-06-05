/******************************************************************************
  @file    messaging_aidl_service.cpp
  @brief   messaging_aidl_service

  DESCRIPTION
    Implements the server side of the IRadioMessaging interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#define TAG "RILQ"

#include <string>
#include <vector>
#include <framework/Log.h>
#include <framework/GenericCallback.h>
#include "messaging_aidl_service.h"
#include "wake_lock_utils.h"
#include "messaging_aidl_utils.h"
#include "interfaces/QcRilRequestMessage.h"

void IRadioMessagingImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlmessaging::IRadioMessagingResponse>& messagingResponse,
    const std::shared_ptr<aidlmessaging::IRadioMessagingIndication>& messagingIndication) {
  mIRadioMessagingResponse = messagingResponse;
  mIRadioMessagingIndication = messagingIndication;
}

void IRadioMessagingImpl::deathNotifier(void *cookie) {
  QCRIL_LOG_DEBUG("IRadioMessagingImpl: Client died, cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IRadioMessagingImpl* impl = static_cast<IRadioMessagingImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier(cookie);
  }
}

/*
 *   @brief
 *   Registers the callback for IRadioMessaging using the
 *   IRadioMessagingResponse and IRadioMessagingIndication objects
 *   being passed in by the client as parameters
 *
 */
::ndk::ScopedAStatus IRadioMessagingImpl::setResponseFunctions(
    const std::shared_ptr<aidlmessaging::IRadioMessagingResponse>& in_messagingResponse,
    const std::shared_ptr<aidlmessaging::IRadioMessagingIndication>& in_messagingIndication) {

  QCRIL_LOG_INFO("IRadioMessagingImpl::setResponseFunctions: Set client callback");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

  if (mIRadioMessagingResponse != nullptr) {
    AIBinder_unlinkToDeath(mIRadioMessagingResponse->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
  }

  setResponseFunctions_nolock(in_messagingResponse, in_messagingIndication);

  if (mIRadioMessagingResponse != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient != nullptr) {
      AIBinder_linkToDeath(mIRadioMessagingResponse->asBinder().get(), mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

IRadioMessagingImpl::IRadioMessagingImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

IRadioMessagingImpl::~IRadioMessagingImpl() {
}

void IRadioMessagingImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mIRadioMessagingResponse = nullptr;
    mIRadioMessagingIndication = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

//AIDL API implementation
void IRadioMessagingImpl::sendResponseForSendSms(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<RilSendSmsResult_t> smsResult) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlmessaging::SendSmsResult result = { -1, "", -1 };
  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  if (errorCode == RIL_E_SUCCESS) {
    if (smsResult) {
      result = makeSendSmsResult(smsResult);
    } else {
      resp.error = aidlradio::RadioError::INTERNAL_ERR;
    }
  }

  QCRIL_LOG_DEBUG("sendSmsResponse: serial=%d, error=%d result: %s",
      serial, errorCode, result.toString().c_str());
  auto ret = respCb->sendSmsResponse(resp, result);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::sendSms(int32_t in_serial,
  const aidlmessaging::GsmSmsMessage& in_message) {
  QCRIL_LOG_DEBUG("sendSms: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestSendSmsMessage>(this->getContext(in_serial),
                                                        in_message.smscPdu,
                                                        in_message.pdu);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilSendSmsResult_t> sendSmsResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
          }
          this->sendResponseForSendSms(in_serial, errorCode, sendSmsResult);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendSms(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSendSMSExpectMore(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<RilSendSmsResult_t> smsResult) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlmessaging::SendSmsResult result = { -1, "", -1 };
  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  if (errorCode == RIL_E_SUCCESS) {
    if (smsResult) {
      result = makeSendSmsResult(smsResult);
    } else {
      resp.error = aidlradio::RadioError::INTERNAL_ERR;
    }
  }

  QCRIL_LOG_DEBUG("sendResponseForSendSMSExpectMore: serial=%d, error=%d result: %s",
      serial, errorCode, result.toString().c_str());
  auto ret = respCb->sendSmsExpectMoreResponse(resp, result);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::sendSmsExpectMore(int32_t in_serial,
   const aidlmessaging::GsmSmsMessage& in_message) {
  QCRIL_LOG_DEBUG("sendSmsExpectMore: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestSendSmsMessage>(this->getContext(in_serial),
                                                        in_message.smscPdu,
                                                        in_message.pdu, true);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilSendSmsResult_t> sendSmsResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
          }
          this->sendResponseForSendSMSExpectMore(in_serial, errorCode, sendSmsResult);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendSMSExpectMore(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSendCdmaSms(int32_t serial, RIL_Errno errorCode,
                                   std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlmessaging::SendSmsResult result = { -1, "", -1 };
  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  if (errorCode == RIL_E_SUCCESS) {
    if (sendSmsResult) {
      result = makeSendSmsResult(sendSmsResult);
    } else {
      resp.error = aidlradio::RadioError::INTERNAL_ERR;
    }
  }

  QCRIL_LOG_DEBUG("sendCdmaSmsResponse: serial=%d, error=%d result: %s", serial, errorCode,
      result.toString().c_str());
  auto ret = respCb->sendCdmaSmsResponse(resp, result);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::sendCdmaSms(int32_t in_serial,
    const aidlmessaging::CdmaSmsMessage& in_sms) {
  QCRIL_LOG_DEBUG("sendCdmaSms: serial=%d", in_serial);

  RIL_CDMA_SMS_Message rcsm{};
  constructCdmaSms(rcsm, in_sms);

  auto msg = std::make_shared<RilRequestCdmaSendSmsMessage>(this->getContext(in_serial), rcsm);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilSendSmsResult_t> sendSmsResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
          }
          this->sendResponseForSendCdmaSms(in_serial, errorCode, sendSmsResult);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendCdmaSms(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSendCdmaSmsExpectMore(
      int32_t serial, RIL_Errno errorCode, std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlmessaging::SendSmsResult result = { -1, "", -1 };
  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  if (errorCode == RIL_E_SUCCESS) {
    if (sendSmsResult) {
      result = makeSendSmsResult(sendSmsResult);
    } else {
      resp.error = aidlradio::RadioError::INTERNAL_ERR;
    }
  }

  QCRIL_LOG_DEBUG("sendResponseForSendCdmaSmsExpectMore: serial=%d, error=%d result = %s",
       serial, errorCode, result.toString().c_str());
  auto ret = respCb->sendCdmaSmsExpectMoreResponse(resp, result);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::sendCdmaSmsExpectMore(int32_t in_serial,
    const aidlmessaging::CdmaSmsMessage& in_sms) {
  QCRIL_LOG_DEBUG("sendCdmaSmsExpectMore: serial=%d", in_serial);

  RIL_CDMA_SMS_Message rcsm{};
  constructCdmaSms(rcsm, in_sms);
  rcsm.expectMore = 1;

  auto msg = std::make_shared<RilRequestCdmaSendSmsMessage>(this->getContext(in_serial), rcsm);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilSendSmsResult_t> sendSmsResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
          }
          this->sendResponseForSendCdmaSmsExpectMore(in_serial, errorCode, sendSmsResult);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSendCdmaSmsExpectMore(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSendImsSms(int32_t serial, RIL_Errno errorCode,
                                   std::shared_ptr<RilSendSmsResult_t> sendSmsResult) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlmessaging::SendSmsResult result = { -1, "", -1 };
  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  if (errorCode == RIL_E_SUCCESS) {
    if (sendSmsResult) {
      result = makeSendSmsResult(sendSmsResult);
    } else {
      resp.error = aidlradio::RadioError::INTERNAL_ERR;
    }
  }
  QCRIL_LOG_DEBUG("sendImsSmsResponse: serial=%d, error=%d result: %s", serial, errorCode,
      result.toString().c_str());

  auto ret = respCb->sendImsSmsResponse(resp, result);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::sendImsSms(int32_t in_serial,
    const ::aidl::android::hardware::radio::messaging::ImsSmsMessage& in_message) {
  QCRIL_LOG_DEBUG("sendImsSms: serial=%d", in_serial);
  RIL_Errno errResp = RIL_E_SUCCESS;

  do {
    RIL_RadioTechnologyFamily tech = convertAidlRadioTechToRil(in_message.tech);
    auto msg = std::make_shared<RilRequestImsSendSmsMessage>(
        this->getContext(in_serial), in_message.messageRef, tech, in_message.retry);
    if (msg == nullptr) {
      errResp = RIL_E_NO_MEMORY;
      break;
    }
    if (tech == RADIO_TECH_3GPP) {
      std::string payload;
      if (in_message.gsmMessage.size() != 1) {
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
      }
      std::string gsmPdu;
      std::string gsmSmsc;
      QCRIL_LOG_INFO("sendImsSms: pdu size = %d", in_message.gsmMessage[0].pdu.size());

      if (in_message.gsmMessage[0].pdu.size() > 0) {
        gsmPdu = in_message.gsmMessage[0].pdu.c_str();
      }

      if (in_message.gsmMessage[0].smscPdu.size() > 0) {
        gsmSmsc = in_message.gsmMessage[0].smscPdu.c_str();
      }
      msg->setGsmPayload(gsmSmsc, gsmPdu);
    } else {
      // set cmda payload
      if (in_message.cdmaMessage.size() != 1) {
        errResp = RIL_E_INVALID_ARGUMENTS;
        break;
      }

      RIL_CDMA_SMS_Message rcsm = {};
      constructCdmaSms(rcsm, in_message.cdmaMessage[0]);
      msg->setCdmaPayload(rcsm);
    }

    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilSendSmsResult_t> sendSmsResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            sendSmsResult = std::static_pointer_cast<RilSendSmsResult_t>(resp->data);
          }
          this->sendResponseForSendImsSms(in_serial, errorCode, sendSmsResult);
    }));
    msg->setCallback(&cb);
    msg->dispatch();
  } while (false);

  if (errResp != RIL_E_SUCCESS) {
    this->sendResponseForSendImsSms(in_serial, errResp, nullptr);
  }

  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSetSmscAddress(int32_t serial, RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("setSmscAddressResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};


  auto ret = respCb->setSmscAddressResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::setSmscAddress(int32_t in_serial,
    const std::string& in_smsc) {
  QCRIL_LOG_DEBUG("setSmscAddress: serial=%d", in_serial);
  auto msg =
    std::make_shared<RilRequestSetSmscAddressMessage>(this->getContext(in_serial), in_smsc);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetSmscAddress(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetSmscAddress(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForGetSmscAddress(int32_t serial, RIL_Errno errorCode,
                                      std::shared_ptr<RilGetSmscAddrResult_t> smscAddrResult) {
  QCRIL_LOG_DEBUG("getSmscAddressResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  std::string smsc;
  if (errorCode == RIL_E_SUCCESS && smscAddrResult) {
    smsc = smscAddrResult->smscAddr;
  }

  auto ret = respCb->getSmscAddressResponse(resp, smsc);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::getSmscAddress(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getSmscAddress: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestGetSmscAddressMessage>(this->getContext(in_serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilGetSmscAddrResult_t> smscAddrResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            smscAddrResult = std::static_pointer_cast<RilGetSmscAddrResult_t>(resp->data);
          }
          this->sendResponseForGetSmscAddress(in_serial, errorCode, smscAddrResult);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetSmscAddress(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForAcknowledgeLastIncomingGsmSms(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("acknowledgeLastIncomingGsmSmsResponse: serial=%d, error=%d", serial,
                   errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->acknowledgeLastIncomingGsmSmsResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::acknowledgeLastIncomingGsmSms(int32_t in_serial,
                        bool in_success, aidlmessaging::SmsAcknowledgeFailCause in_cause) {
  QCRIL_LOG_DEBUG("acknowledgeLastIncomingGsmSms: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestAckGsmSmsMessage>(this->getContext(in_serial), in_success,
                                                          static_cast<int32_t>(in_cause));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForAcknowledgeLastIncomingGsmSms(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForAcknowledgeLastIncomingGsmSms(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForAcknowledgeIncomingGsmSmsWithPdu(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("sendResponseForAcknowledgeIncomingGsmSmsWithPdu: serial=%d, error=%d", serial,
                   errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->acknowledgeIncomingGsmSmsWithPduResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::acknowledgeIncomingGsmSmsWithPdu(int32_t in_serial,
    bool in_success, const std::string& in_ackPdu) {
  QCRIL_LOG_DEBUG("acknowledgeIncomingGsmSmsWithPdu: serial=%d", in_serial);
  (void)in_success;
  (void)in_ackPdu;
  this->sendResponseForAcknowledgeIncomingGsmSmsWithPdu(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForAcknowledgeLastIncomingCdmaSms(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("sendResponseForAcknowledgeLastIncomingCdmaSms: serial=%d, error=%d", serial,
                   errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->acknowledgeLastIncomingCdmaSmsResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::acknowledgeLastIncomingCdmaSms(int32_t in_serial,
    const aidlmessaging::CdmaSmsAck& in_smsAck) {
  QCRIL_LOG_DEBUG("acknowledgeLastIncomingCdmaSms: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestAckCdmaSmsMessage>(this->getContext(in_serial),
               in_smsAck.errorClass, in_smsAck.smsCauseCode);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForAcknowledgeLastIncomingCdmaSms(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForAcknowledgeLastIncomingCdmaSms(in_serial, RIL_E_NO_MEMORY);
  }

  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForReportSmsMemoryStatus(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("reportSmsMemoryStatusResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->reportSmsMemoryStatusResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::reportSmsMemoryStatus(int32_t in_serial,
    bool in_available) {
  QCRIL_LOG_DEBUG("reportSmsMemoryStatus: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestReportSmsMemoryStatusMessage>(this->getContext(in_serial),
                                                                      in_available);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForReportSmsMemoryStatus(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForReportSmsMemoryStatus(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForWriteSmsToRuim(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  int32 recordNumber = -1;
  if (errorCode == RIL_E_SUCCESS && writeSmsResponse) {
    recordNumber = writeSmsResponse->recordNumber;
  }

  QCRIL_LOG_DEBUG("writeSmsToRuimResponse: serial=%d, error=%d index: %d",
      serial, errorCode, recordNumber);
  auto ret = respCb->writeSmsToRuimResponse(resp, recordNumber);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::writeSmsToRuim(int32_t in_serial,
    const aidlmessaging::CdmaSmsWriteArgs& in_cdmaSms) {
  QCRIL_LOG_DEBUG("writeSmsToRuim: serial=%d", in_serial);
  RIL_CDMA_SMS_Message rcsm = {};
  constructCdmaSms(rcsm, in_cdmaSms.message);

  auto msg = std::make_shared<RilRequestCdmaWriteSmsToRuimMessage>(
      this->getContext(in_serial), rcsm, static_cast<int>(in_cdmaSms.status));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            writeSmsResponse = std::static_pointer_cast<RilWriteSmsToSimResult_t>(resp->data);
          }
          this->sendResponseForWriteSmsToRuim(in_serial, errorCode, writeSmsResponse);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForWriteSmsToRuim(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForWriteSmsToSim(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  int32 recordNumber = -1;
  if (errorCode == RIL_E_SUCCESS && writeSmsResponse) {
    recordNumber = writeSmsResponse->recordNumber;
  }

  QCRIL_LOG_DEBUG("writeSmsToSimResponse: serial=%d, error=%d index: %d",
      serial, errorCode, recordNumber);
  auto ret = respCb->writeSmsToSimResponse(resp, recordNumber);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::writeSmsToSim(int32_t in_serial,
    const aidlmessaging::SmsWriteArgs& in_smsWriteArgs) {
  QCRIL_LOG_DEBUG("writeSmsToSim: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestWriteSmsToSimMessage>(
    this->getContext(in_serial), in_smsWriteArgs.smsc, in_smsWriteArgs.pdu,
    static_cast<int>(in_smsWriteArgs.status));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            writeSmsResponse = std::static_pointer_cast<RilWriteSmsToSimResult_t>(resp->data);
          }
          this->sendResponseForWriteSmsToSim(in_serial, errorCode, writeSmsResponse);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForWriteSmsToSim(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForDeleteSmsOnRuim(int32_t serial, RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("deleteSmsOnRuimResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->deleteSmsOnRuimResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::deleteSmsOnRuim(int32_t in_serial, int32_t in_index) {
  QCRIL_LOG_DEBUG("deleteSmsOnRuim: serial=%d", in_serial);
  auto msg =
    std::make_shared<RilRequestCdmaDeleteSmsOnRuimMessage>(this->getContext(in_serial), in_index);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForDeleteSmsOnRuim(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForDeleteSmsOnRuim(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForDeleteSmsOnSim(int32_t serial, RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("deleteSmsOnSimResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->deleteSmsOnSimResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::deleteSmsOnSim(int32_t in_serial, int32_t in_index) {
  QCRIL_LOG_DEBUG("deleteSmsOnSim: serial=%d", in_serial);
  auto msg =
      std::make_shared<RilRequestDeleteSmsOnSimMessage>(this->getContext(in_serial), in_index);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForDeleteSmsOnSim(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForDeleteSmsOnSim(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForGetCdmaBroadcastConfig(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilGetCdmaBroadcastConfigResult_t> broadcastConfigResp) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  std::vector<aidlmessaging::CdmaBroadcastSmsConfigInfo> configs;
  if (errorCode == RIL_E_SUCCESS && broadcastConfigResp) {
    // generate config list
    auto num = broadcastConfigResp->configList.size();
    configs.resize(num);
    for (unsigned int i = 0; i < num; i++) {
      auto& item = broadcastConfigResp->configList[i];
      configs[i].serviceCategory = item.service_category;
      configs[i].language = item.language;
      configs[i].selected = item.selected == 1 ? true : false;
    }
  }

  QCRIL_LOG_DEBUG("getCdmaBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
  for (auto &config : configs) {
    QCRIL_LOG_DEBUG("config: %s ",config.toString().c_str());
  }
  auto ret = respCb->getCdmaBroadcastConfigResponse(resp, configs);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::getCdmaBroadcastConfig(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getCdmaBroadcastConfig: serial=%d", in_serial);
  auto msg =
    std::make_shared<RilRequestGetCdmaBroadcastConfigMessage>(this->getContext(in_serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilGetCdmaBroadcastConfigResult_t> broadcastConfigResp{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            broadcastConfigResp =
                std::static_pointer_cast<RilGetCdmaBroadcastConfigResult_t>(resp->data);
          }
          this->sendResponseForGetCdmaBroadcastConfig(in_serial, errorCode, broadcastConfigResp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetCdmaBroadcastConfig(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForGetGsmBroadcastConfig(
      int32_t serial, RIL_Errno errorCode,
      std::shared_ptr<RilGetGsmBroadcastConfigResult_t> broadcastConfigResp) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

    std::vector<aidlmessaging::GsmBroadcastSmsConfigInfo> configs;
    if (errorCode == RIL_E_SUCCESS && broadcastConfigResp) {
      // generate config list
      auto num = broadcastConfigResp->configList.size();
      configs.resize(num);
      for (unsigned int i = 0; i < num; i++) {
        auto& item = broadcastConfigResp->configList[i];
        configs[i].fromServiceId = item.fromServiceId;
        configs[i].toServiceId = item.toServiceId;
        configs[i].fromCodeScheme = item.fromCodeScheme;
        configs[i].toCodeScheme = item.toCodeScheme;
        configs[i].selected = item.selected == 1 ? true : false;
      }
    }

  QCRIL_LOG_DEBUG("getGsmBroadcastConfigResponse: serial=%d, error=%d ", serial, errorCode);
  for (auto &config : configs) {
    QCRIL_LOG_DEBUG("config: %s ",config.toString().c_str());
  }
  auto ret = respCb->getGsmBroadcastConfigResponse(resp, configs);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::getGsmBroadcastConfig(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getGsmBroadcastConfig: serial=%d", in_serial);
  auto msg = std::make_shared<RilRequestGetGsmBroadcastConfigMessage>(this->getContext(in_serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilGetGsmBroadcastConfigResult_t> broadcastConfigResp{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            broadcastConfigResp =
                std::static_pointer_cast<RilGetGsmBroadcastConfigResult_t>(resp->data);
          }
          this->sendResponseForGetGsmBroadcastConfig(in_serial, errorCode, broadcastConfigResp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetGsmBroadcastConfig(in_serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSetCdmaBroadcastActivation(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("setCdmaBroadcastActivationResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};


  auto ret = respCb->setCdmaBroadcastActivationResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::setCdmaBroadcastActivation(int32_t in_serial,
    bool in_activate) {
  QCRIL_LOG_DEBUG("setCdmaBroadcastActivation: serial=%d", in_serial);
  auto msg =
    std::make_shared<RilRequestCdmaSmsBroadcastActivateMessage>(this->getContext(in_serial),
                                                                in_activate);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCdmaBroadcastActivation(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetCdmaBroadcastActivation(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSetGsmBroadcastActivation(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("setGsmBroadcastActivationResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};


  auto ret = respCb->setGsmBroadcastActivationResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::setGsmBroadcastActivation(int32_t in_serial,
    bool in_activate)  {
  QCRIL_LOG_DEBUG("setGsmBroadcastActivation: serial=%d", in_serial);
  auto msg =
    std::make_shared<RilRequestGsmSmsBroadcastActivateMessage>(this->getContext(in_serial),
                                                               in_activate);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetGsmBroadcastActivation(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetGsmBroadcastActivation(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSetCdmaBroadcastConfig(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("setCdmaBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};


  auto ret = respCb->setCdmaBroadcastConfigResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::setCdmaBroadcastConfig(int32_t in_serial,
    const std::vector<aidlmessaging::CdmaBroadcastSmsConfigInfo>& in_configInfo) {
  QCRIL_LOG_DEBUG("setCdmaBroadcastConfig: serial=%d", in_serial);
  int num = in_configInfo.size();
  std::vector<RIL_CDMA_BroadcastSmsConfigInfo> configVec(num);

  for (int i = 0; i < num; i++) {
    configVec[i].service_category = in_configInfo[i].serviceCategory;
    configVec[i].language = in_configInfo[i].language;
    configVec[i].selected = (in_configInfo[i].selected ? 1 : 0);
  }

  auto msg = std::make_shared<RilRequestCdmaSetBroadcastSmsConfigMessage>(
      this->getContext(in_serial), std::move(configVec));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCdmaBroadcastConfig(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetCdmaBroadcastConfig(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendResponseForSetGsmBroadcastConfig(int32_t serial,
    RIL_Errno errorCode) {
  QCRIL_LOG_DEBUG("setGsmBroadcastConfigResponse: serial=%d, error=%d", serial, errorCode);
  auto respCb = getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioMessaging: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};


  auto ret = respCb->setGsmBroadcastConfigResponse(resp);
  if (!ret.isOk()) {
     QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioMessagingImpl::setGsmBroadcastConfig(int32_t in_serial,
    const std::vector<aidlmessaging::GsmBroadcastSmsConfigInfo>& in_configInfo) {
  QCRIL_LOG_DEBUG("setGsmBroadcastConfig: serial=%d", in_serial);
  int num = in_configInfo.size();
  std::vector<RIL_GSM_BroadcastSmsConfigInfo> configVec(num);

  for (int i = 0; i < num; i++) {
    configVec[i].fromServiceId = in_configInfo[i].fromServiceId;
    configVec[i].toServiceId = in_configInfo[i].toServiceId;
    configVec[i].fromCodeScheme = in_configInfo[i].fromCodeScheme;
    configVec[i].toCodeScheme = in_configInfo[i].toCodeScheme;
    configVec[i].selected = (in_configInfo[i].selected ? 1 : 0);
  }

  auto msg =
    std::make_shared<RilRequestGsmSetBroadcastSmsConfigMessage>(this->getContext(in_serial),
                                                                std::move(configVec));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, in_serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetGsmBroadcastConfig(in_serial, errorCode);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetGsmBroadcastConfig(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

//Unsol indications
::ndk::ScopedAStatus IRadioMessagingImpl::responseAcknowledgement() {
  QCRIL_LOG_DEBUG("IRadioMessaging: responseAcknowledgement");
  qti::ril::utils::releaseWakeLock();
  return ndk::ScopedAStatus::ok();
}

void IRadioMessagingImpl::sendNewSms(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    std::vector<uint8_t> inSms = msg->getPayload();
    std::vector<uint8_t> pdu = inSms;
    // The ATEL will acknowledge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();

    QCRIL_LOG_DEBUG("UNSOL: newSms");
    auto ret = ind->newSms(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, pdu);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioMessagingImpl::sendNewCdmaSms(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    aidlmessaging::CdmaSmsMessage smsMsg{};
    makeCdmaSmsMessage(smsMsg, msg->getCdmaSms());

    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: cdmaNewSms");
    auto ret = ind->cdmaNewSms(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, smsMsg);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}

void IRadioMessagingImpl::sendNewSmsOnSim(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: newSmsOnSim");
    auto ret =
      ind->newSmsOnSim(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, msg->getRecordNumber());
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioMessagingImpl::sendNewBroadcastSms(
    std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    std::vector<uint8_t> pdu;
    pdu = msg->getPayload();
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: newBroadcastSms");
    auto ret = ind->newBroadcastSms(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, pdu);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioMessagingImpl::sendNewSmsStatusReport(
    std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    std::vector<uint8_t> pdu;
    pdu = msg->getPayload();

    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();

    QCRIL_LOG_DEBUG("UNSOL: newSmsStatusReport");
    auto ret = ind->newSmsStatusReport(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, pdu);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioMessagingImpl::sendCdmaRuimSmsStorageFull(
    std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: cdmaRuimSmsStorageFull");
    auto ret = ind->cdmaRuimSmsStorageFull(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioMessagingImpl::sendSimSmsStorageFull(
    std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg) {
  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind && msg) {
    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();
    QCRIL_LOG_DEBUG("UNSOL: simSmsStorageFull");
    auto ret = ind->simSmsStorageFull(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}
