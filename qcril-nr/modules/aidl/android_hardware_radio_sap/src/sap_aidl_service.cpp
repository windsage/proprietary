/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "sap_aidl_service.h"
#include "framework/Log.h"

#include "interfaces/uim/UimSAPConnectionRequestMsg.h"
#include "interfaces/uim/UimSAPRequestMsg.h"
#include "interfaces/uim/UimSapStatusIndMsg.h"

/*===========================================================================

FUNCTION: ISapImpl::clearCallbacks

===========================================================================*/
void ISapImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mSapServiceResponseCb = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

/*===========================================================================

FUNCTION: ISapImpl::deathNotifier

===========================================================================*/


void ISapImpl::deathNotifier(void *) {
  QCRIL_LOG_DEBUG("ISapImpl::serviceDied: Client died, Cleaning up callbacks");
  clearCallbacks();
}

ISapImpl::ISapImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {}

ISapImpl::~ISapImpl() {
}

/*===========================================================================

FUNCTION: deathRecpCallback

===========================================================================*/

static void deathRecpCallback(void *cookie) {
  ISapImpl *SapImpl = static_cast<ISapImpl *>(cookie);
  if (SapImpl != nullptr) {
    SapImpl->deathNotifier(cookie);
  }
}

/*===========================================================================

FUNCTION: ISapImpl::setCallback

===========================================================================*/

::ndk::ScopedAStatus ISapImpl::setCallback(
    const std::shared_ptr<aidlimports::ISapCallback> &in_uimSapResponse) {
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mSapServiceResponseCb != nullptr) {
    AIBinder_unlinkToDeath(mSapServiceResponseCb->asBinder().get(),
                           mDeathRecipient, reinterpret_cast<void *>(this));
  }
  mSapServiceResponseCb = in_uimSapResponse;
  if (mSapServiceResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(mSapServiceResponseCb->asBinder().get(),
                           mDeathRecipient, reinterpret_cast<void *>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

/*===========================================================================

FUNCTION: ISapImpl::sendresponseforconnect

===========================================================================*/

void ISapImpl::sendresponseforconnect(int token,
                                      aidlimports::SapConnectRsp resp) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->connectResponse(token, resp, 0);
  } else {
    QCRIL_LOG_ERROR("SapImpl::connectReq ResponseCallback is null");
  }
}


/*===========================================================================

FUNCTION: ISapImpl::connectReq

===========================================================================*/

::ndk::ScopedAStatus ISapImpl::connectReq(int32_t token, int32_t maxMsgSize) {
  auto msg = std::make_shared<UimSAPConnectionRequestMsg>(RIL_UIM_SAP_CONNECT);

  QCRIL_LOG_DEBUG("SapImpl::connectReq");

  (void)maxMsgSize;

  if (msg) {
    GenericCallback<RIL_UIM_SAP_ConnectionResponse> cb(
        ([this, token](std::shared_ptr<Message> solicitedMsg,
                       Message::Callback::Status status,
                       std::shared_ptr<RIL_UIM_SAP_ConnectionResponse>
                           responseDataPtr) -> void {
          aidlimports::SapConnectRsp resp =
              aidlimports::SapConnectRsp::CONNECT_FAILURE;

          if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS &&
              *responseDataPtr == RIL_UIM_SAP_CONNECTION_SUCCESS) {
            resp = aidlimports::SapConnectRsp::SUCCESS;
          }
          sendresponseforconnect(token, resp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendresponseforconnect(token, aidlimports::SapConnectRsp::CONNECT_FAILURE);
  }

  return ndk::ScopedAStatus::ok();
} /* SapImpl::connectReq */

/*===========================================================================

FUNCTION: ISapImpl::sendresponsefordisconnect

===========================================================================*/

void ISapImpl::sendresponsefordisconnect(int32_t token) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->disconnectResponse(token);
  }
}

/*===========================================================================

FUNCTION: ISapImpl::disconnectReq

===========================================================================*/

::ndk::ScopedAStatus ISapImpl::disconnectReq(int32_t token) {
  auto msg =
      std::make_shared<UimSAPConnectionRequestMsg>(RIL_UIM_SAP_DISCONNECT);

  QCRIL_LOG_DEBUG("SapImpl::disconnectReq");

  if (msg) {
    GenericCallback<RIL_UIM_SAP_ConnectionResponse> cb(
        ([this, token](std::shared_ptr<Message> solicitedMsg,
                       Message::Callback::Status status,
                       std::shared_ptr<RIL_UIM_SAP_ConnectionResponse>
                           responseDataPtr) -> void {
          (void)status;
          (void)responseDataPtr;
          (void)solicitedMsg;
          sendresponsefordisconnect(token);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendresponsefordisconnect(token);
  }

  return ndk::ScopedAStatus::ok();
} /* SapImpl::disconnectReq */

/*===========================================================================

FUNCTION: ISapImpl::sendresponseforapdu

===========================================================================*/

void ISapImpl::sendresponseforapdu(int32_t token,
                                   ::aidlimports::SapResultCode result,
                                   std::vector<uint8_t> &apdu_data) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->apduResponse(token, result, apdu_data);
  }
}

/*===========================================================================

FUNCTION: ISapImpl::apduReq

===========================================================================*/

::ndk::ScopedAStatus ISapImpl::apduReq(int32_t token,
                                       ::aidlimports::SapApduType type,
                                       const std::vector<uint8_t> &command) {
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_APDU_REQ, command);

  QCRIL_LOG_DEBUG("SapImpl::apduReq");

  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb(
        ([this, token](
             std::shared_ptr<Message> solicitedMsg,
             Message::Callback::Status status,
             std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void {
          ::aidlimports::SapResultCode resp =
              ::aidlimports::SapResultCode::GENERIC_FAILURE;
          std::vector<uint8_t> apdu_data = {};

          if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS) {
            apdu_data = responseDataPtr->atr_apdu_data;

            switch (responseDataPtr->result) {
            case RIL_UIM_SAP_SUCCESS:
              resp = ::aidlimports::SapResultCode::SUCCESS;
              break;
            case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
              resp = ::aidlimports::SapResultCode::CARD_NOT_ACCESSSIBLE;
              break;
            case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
              resp = ::aidlimports::SapResultCode::CARD_ALREADY_POWERED_OFF;
              break;
            case RIL_UIM_SAP_CARD_REMOVED:
              resp = ::aidlimports::SapResultCode::CARD_REMOVED;
              break;
            default:
              resp = ::aidlimports::SapResultCode::GENERIC_FAILURE;
            }
          }
          sendresponseforapdu(token, resp, apdu_data);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    std::vector<uint8_t> apdu_data = {};
    sendresponseforapdu(token, ::aidlimports::SapResultCode::GENERIC_FAILURE,
                        apdu_data);
  }

  return ndk::ScopedAStatus::ok();
} /* SapImpl::apduReq */


/*===========================================================================

FUNCTION: ISapImpl::sendresponsefortransferatr

===========================================================================*/

void ISapImpl::sendresponsefortransferatr(int32_t token,
                                          ::aidlimports::SapResultCode result,
                                          std::vector<uint8_t> atr_data) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->transferAtrResponse(token, result, atr_data);
  }
}

/*===========================================================================

FUNCTION:  SapImpl::transferAtrReq

===========================================================================*/
::ndk::ScopedAStatus ISapImpl::transferAtrReq(int32_t token) {
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_TRANSFER_ATR_REQ);

  QCRIL_LOG_INFO("SapImpl::transferAtrReq");

  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb(
        ([this, token](
             std::shared_ptr<Message> solicitedMsg,
             Message::Callback::Status status,
             std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void {
          ::aidlimports::SapResultCode resp =
              ::aidlimports::SapResultCode::GENERIC_FAILURE;
          std::vector<uint8_t> atr_data = {};

          if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS) {
            atr_data = responseDataPtr->atr_apdu_data;

            switch (responseDataPtr->result) {
            case RIL_UIM_SAP_SUCCESS:
              resp = ::aidlimports::SapResultCode::SUCCESS;
              break;
            case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
              resp = ::aidlimports::SapResultCode::DATA_NOT_AVAILABLE;
              break;
            case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
              resp = ::aidlimports::SapResultCode::CARD_ALREADY_POWERED_OFF;
              break;
            case RIL_UIM_SAP_CARD_REMOVED:
              resp = ::aidlimports::SapResultCode::CARD_REMOVED;
              break;
            default:
              resp = ::aidlimports::SapResultCode::GENERIC_FAILURE;
            }
          }
          sendresponsefortransferatr(token, resp, atr_data);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    std::vector<uint8_t> atr_data = {};
    sendresponsefortransferatr(
        token, ::aidlimports::SapResultCode::GENERIC_FAILURE, atr_data);
  }

  return ndk::ScopedAStatus::ok();
} /* SapImpl::transferAtrReq */

/*===========================================================================

FUNCTION: ISapImpl::sendresponseforsap

===========================================================================*/

void ISapImpl::sendresponseforsap(int32_t token,
                                  ::aidlimports::SapResultCode resp) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->powerResponse(token, resp);
  }
}

/*===========================================================================

FUNCTION:  SapImpl::powerReq

===========================================================================*/
ndk::ScopedAStatus ISapImpl::powerReq(int32_t token, bool state) {
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_POWER_REQ, state);

  QCRIL_LOG_INFO("SapImpl::powerReq");

  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb(
        ([this, token](
             std::shared_ptr<Message> solicitedMsg,
             Message::Callback::Status status,
             std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void {
          ::aidlimports::SapResultCode resp =
              ::aidlimports::SapResultCode::GENERIC_FAILURE;

          if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS) {
            switch (responseDataPtr->result) {
            case RIL_UIM_SAP_SUCCESS:
              resp = ::aidlimports::SapResultCode::SUCCESS;
              break;
            case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
              resp = ::aidlimports::SapResultCode::CARD_NOT_ACCESSSIBLE;
              break;
            case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
              resp = ::aidlimports::SapResultCode::CARD_ALREADY_POWERED_OFF;
              break;
            case RIL_UIM_SAP_CARD_ALREADY_POWERED_ON:
              resp = ::aidlimports::SapResultCode::CARD_ALREADY_POWERED_ON;
              break;
            case RIL_UIM_SAP_CARD_REMOVED:
              resp = ::aidlimports::SapResultCode::CARD_REMOVED;
              break;
            default:
              resp = ::aidlimports::SapResultCode::GENERIC_FAILURE;
            }
          }
          sendresponseforsap(token, resp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendresponseforsap(token, ::aidlimports::SapResultCode::GENERIC_FAILURE);
  }

  return ndk::ScopedAStatus::ok();
} /* SapImpl::powerReq */

/*===========================================================================

FUNCTION: ISapImpl::sendresponseforsapreset

===========================================================================*/


void ISapImpl::sendresponseforsapreset(int32_t token,
                                       ::aidlimports::SapResultCode result) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->resetSimResponse(token, result);
  }
}
/*===========================================================================

FUNCTION:  SapImpl::resetSimReq

===========================================================================*/
ndk::ScopedAStatus ISapImpl::resetSimReq(int32_t token) {
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_RESET_SIM_REQ);

  QCRIL_LOG_INFO("SapImpl::resetSimReq");

  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb(
        ([this, token](
             std::shared_ptr<Message> solicitedMsg,
             Message::Callback::Status status,
             std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void {
          ::aidlimports::SapResultCode resp =
              ::aidlimports::SapResultCode::GENERIC_FAILURE;
          if (solicitedMsg && responseDataPtr &&
              status == Message::Callback::Status::SUCCESS) {
            switch (responseDataPtr->result) {
            case RIL_UIM_SAP_SUCCESS:
              resp = ::aidlimports::SapResultCode::SUCCESS;
              break;
            case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
              resp = ::aidlimports::SapResultCode::CARD_NOT_ACCESSSIBLE;
              break;
            case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
              resp = ::aidlimports::SapResultCode::CARD_ALREADY_POWERED_OFF;
              break;
            case RIL_UIM_SAP_CARD_REMOVED:
              resp = ::aidlimports::SapResultCode::CARD_REMOVED;
              break;
            default:
              resp = ::aidlimports::SapResultCode::GENERIC_FAILURE;
            }
          }
          sendresponseforsapreset(token, resp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendresponseforsapreset(token,
                            ::aidlimports::SapResultCode::GENERIC_FAILURE);
  }
  return ndk::ScopedAStatus::ok();
} /* SapImpl::resetSimReq */


/*===========================================================================

FUNCTION: ISapImpl::sendresponseforcardreaderstatus

===========================================================================*/

void ISapImpl::sendresponseforcardreaderstatus(
    int32_t token, ::aidlimports::SapResultCode result, int32_t status) {
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->transferCardReaderStatusResponse(token, result,
                                                            status);
  }
}

/*===========================================================================

FUNCTION:  SapImpl::transferCardReaderStatusReq

===========================================================================*/
ndk::ScopedAStatus ISapImpl::transferCardReaderStatusReq(int32_t token) {
  auto msg = std::make_shared<UimSAPRequestMsg>(
      RIL_UIM_SAP_TRANSFER_CARD_READER_STATUS_REQ);

  QCRIL_LOG_INFO("SapImpl::transferCardReaderStatusReq");

  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb(
        ([this, token](
             std::shared_ptr<Message> solicitedMsg,
             Message::Callback::Status ret_status,
             std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void {
          ::aidlimports::SapResultCode resp =
              ::aidlimports::SapResultCode::GENERIC_FAILURE;
          int32_t status = -1;

          if (solicitedMsg && responseDataPtr &&
              ret_status == Message::Callback::Status::SUCCESS) {
            status = responseDataPtr->reader_status;
            switch (responseDataPtr->result) {
            case RIL_UIM_SAP_SUCCESS:
              resp = ::aidlimports::SapResultCode::SUCCESS;
              break;
            case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
            case RIL_UIM_SAP_CARD_REMOVED:
              resp = ::aidlimports::SapResultCode::DATA_NOT_AVAILABLE;
              break;
            default:
              resp = ::aidlimports::SapResultCode::GENERIC_FAILURE;
            }
          }
          sendresponseforcardreaderstatus(token, resp, status);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    int32_t status = -1;
    sendresponseforcardreaderstatus(
        token, ::aidlimports::SapResultCode::GENERIC_FAILURE, status);
  }
  return ndk::ScopedAStatus::ok();
} /* SapImpl::transferCardReaderStatusReq */

/*===========================================================================

FUNCTION:  SapImpl::setTransferProtocolReq

===========================================================================*/
ndk::ScopedAStatus ISapImpl::setTransferProtocolReq(
    int32_t token, ::aidlimports::SapTransferProtocol transferProtocol) {
  QCRIL_LOG_INFO("SapImpl::setTransferProtocolReq");

  (void)transferProtocol;
  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->transferProtocolResponse(
        token, ::aidlimports::SapResultCode::NOT_SUPPORTED);
  }
  return ndk::ScopedAStatus::ok();

} /* SapImpl::setTransferProtocolReq */

/*===========================================================================

FUNCTION: ISapImpl::sendStatusInd

===========================================================================*/

void ISapImpl::sendStatusInd(int32_t token, ::aidlimports::SapStatus status) {
  QCRIL_LOG_INFO("SapImpl::SendStatusInd");

  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->statusIndication(token, status);
  }
}

/*===========================================================================

FUNCTION: ISapImpl::sendDisconnectInd

===========================================================================*/

void ISapImpl::sendDisconnectInd(
    int32_t token, ::aidlimports::SapDisconnectType disconnectType) {
  QCRIL_LOG_INFO("SapImpl::SendDisconnectInd");

  if (mSapServiceResponseCb != nullptr) {
    mSapServiceResponseCb->disconnectIndication(token, disconnectType);
  }
}
