/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "uimremoteserver_stable_aidl_service.h"
#include <framework/Log.h>
#include <cutils/properties.h>
#include "interfaces/uim/UimSAPRequestMsg.h"
#include "interfaces/uim/UimSAPConnectionRequestMsg.h"
#include <inttypes.h>

#undef TAG
#define TAG "RILQ"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim_remote_server {
namespace implementation {

void IUimRemoteServiceServerImpl::setInstanceId(qcril_instance_id_e_type instId)
{
  mInstanceId = instId;
}
qcril_instance_id_e_type IUimRemoteServiceServerImpl::getInstanceId()
{
  return mInstanceId;
}

std::shared_ptr<UimRemoteServerContext> IUimRemoteServiceServerImpl::getContext(int32_t serial)
{
  return std::make_shared<UimRemoteServerContext>(mInstanceId, serial);
}

std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse>
IUimRemoteServiceServerImpl::getResponseCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mResponseCb;
}

std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication>
IUimRemoteServiceServerImpl::getIndicationCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

void IUimRemoteServiceServerImpl::clearCallbacks_nolock()
{
  QCRIL_LOG_DEBUG("clearCallbacks_nolock");
  mResponseCb = nullptr;
  mIndicationCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

void IUimRemoteServiceServerImpl::clearCallbacks()
{
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void IUimRemoteServiceServerImpl::deathNotifier(void*)
{
  QCRIL_LOG_DEBUG("IUimRemoteServiceServer::serviceDied: Client died, Cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie)
{
  IUimRemoteServiceServerImpl* uimRemoteServiceServerImpl =
      static_cast<IUimRemoteServiceServerImpl*>(cookie);
  if (uimRemoteServiceServerImpl != nullptr) {
    uimRemoteServiceServerImpl->deathNotifier(cookie);
  }
}

void IUimRemoteServiceServerImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse>& in_responseCallback,
    const std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication>& in_indicationCallback)
{
  QCRIL_LOG_DEBUG("IUimRemoteServiceServer::setResponseFunctions_nolock");
  mResponseCb = in_responseCallback;
  mIndicationCb = in_indicationCallback;
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::setCallback(
    const std::shared_ptr<aidlimports::IUimRemoteServiceServerResponse>& in_responseCallback,
    const std::shared_ptr<aidlimports::IUimRemoteServiceServerIndication>& in_indicationCallback)
{
  QCRIL_LOG_DEBUG("IUimRemoteServiceServer::setResposeFuntions");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mResponseCb != nullptr) {
    AIBinder_unlinkToDeath(
        mResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
  }
  setResponseFunctions_nolock(in_responseCallback, in_indicationCallback);
  if (mResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(
          mResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerApduReq(
    int32_t in_token,
    aidlimports::UimRemoteServiceServerApduType apduType,
    const std::vector<uint8_t>& command)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerApduReq token=%d", in_token);
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_APDU_REQ, command);
  (void)apduType;
  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb((
        [this, in_token, command](std::shared_ptr<Message> solicitedMsg,
                                  Message::Callback::Status status,
                                  std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void
        {
          UimRemoteServiceServerResultCode resp =
              UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
          std::vector<uint8_t> apdu_data = {};

          if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS) {
            apdu_data = responseDataPtr->atr_apdu_data;

            switch (responseDataPtr->result) {
              case RIL_UIM_SAP_SUCCESS:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_SUCCESS;
                break;
              case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSSIBLE;
                break;
              case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF;
                break;
              case RIL_UIM_SAP_CARD_REMOVED:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED;
                break;
              default:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
            }
          }

          sendApduResponse(in_token, resp, apdu_data);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    std::vector<uint8_t> apdu_data = {};
    sendApduResponse(in_token,
                     UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE,
                     apdu_data);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerConnectReq(
    int32_t in_token, int32_t in_maxMsgSize)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerConnectReq token=%d", in_token);
  auto msg = std::make_shared<UimSAPConnectionRequestMsg>(RIL_UIM_SAP_CONNECT);
  (void)in_maxMsgSize;
  if (msg) {
    GenericCallback<RIL_UIM_SAP_ConnectionResponse> cb((
        [this, in_token](std::shared_ptr<Message> solicitedMsg,
                         Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_SAP_ConnectionResponse> responseDataPtr) -> void
        {
          UimRemoteServiceServerConnectRsp resp =
              UimRemoteServiceServerConnectRsp::UIM_REMOTE_SERVICE_SERVER_CONNECT_FAILURE;

          if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS &&
              *responseDataPtr == RIL_UIM_SAP_CONNECTION_SUCCESS) {
            resp = UimRemoteServiceServerConnectRsp::UIM_REMOTE_SERVICE_SERVER_SUCCESS;
          }

          sendConnectResponse(in_token, resp, 0);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendConnectResponse(
        in_token, UimRemoteServiceServerConnectRsp::UIM_REMOTE_SERVICE_SERVER_CONNECT_FAILURE, 0);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerDisconnectReq(
    int32_t in_token)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerDisconnectReq token=%d", in_token);
  auto msg = std::make_shared<UimSAPConnectionRequestMsg>(RIL_UIM_SAP_DISCONNECT);
  if (msg) {
    GenericCallback<RIL_UIM_SAP_ConnectionResponse> cb((
        [this, in_token](std::shared_ptr<Message> solicitedMsg,
                         Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_SAP_ConnectionResponse> responseDataPtr) -> void
        {
          (void)status;
          (void)responseDataPtr;
          (void)solicitedMsg;
          sendDisconnectResponse(in_token);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendDisconnectResponse(in_token);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerPowerReq(int32_t in_token,
                                                                                 bool state)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerPowerReq token=%d state:%d", in_token, state);
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_POWER_REQ, state);
  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb((
        [this, in_token](std::shared_ptr<Message> solicitedMsg,
                         Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void
        {
          UimRemoteServiceServerResultCode resp =
              UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;

          if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS) {
            switch (responseDataPtr->result) {
              case RIL_UIM_SAP_SUCCESS:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_SUCCESS;
                break;
              case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSSIBLE;
                break;
              case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF;
                break;
              case RIL_UIM_SAP_CARD_ALREADY_POWERED_ON:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_ON;
                break;
              case RIL_UIM_SAP_CARD_REMOVED:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED;
                break;
              default:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
            }
          }
          sendPowerResponse(in_token, resp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendPowerResponse(in_token,
                      UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerResetSimReq(int32_t in_token)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerResetSimReq token=%d", in_token);
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_RESET_SIM_REQ);
  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb((
        [this, in_token](std::shared_ptr<Message> solicitedMsg,
                         Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void
        {
          UimRemoteServiceServerResultCode resp =
              UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;

          if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS) {
            switch (responseDataPtr->result) {
              case RIL_UIM_SAP_SUCCESS:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_SUCCESS;
                break;
              case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSSIBLE;
                break;
              case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF;
                break;
              case RIL_UIM_SAP_CARD_REMOVED:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED;
                break;
              default:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
            }
          }

          sendResetSimResponse(in_token, resp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResetSimResponse(
        in_token, UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerSetTransferProtocolReq(
    int32_t in_token, aidlimports::UimRemoteServiceServerTransferProtocol transferProtocol)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerSetTransferProtocolReq token=%d", in_token);
  (void)transferProtocol;
  sendTransferProtocolResponse(
      in_token, UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerTransferAtrReq(
    int32_t in_token)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerTransferAtrReq token=%d", in_token);
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_TRANSFER_ATR_REQ);
  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb((
        [this, in_token](std::shared_ptr<Message> solicitedMsg,
                         Message::Callback::Status status,
                         std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void
        {
          UimRemoteServiceServerResultCode resp =
              UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
          std::vector<uint8_t> atr_data = {};

          if (solicitedMsg && responseDataPtr && status == Message::Callback::Status::SUCCESS) {
            atr_data = responseDataPtr->atr_apdu_data;

            switch (responseDataPtr->result) {
              case RIL_UIM_SAP_SUCCESS:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_SUCCESS;
                break;
              case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_DATA_NOT_AVAILABLE;
                break;
              case RIL_UIM_SAP_CARD_ALREADY_POWERED_OFF:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_ALREADY_POWERED_OFF;
                break;
              case RIL_UIM_SAP_CARD_REMOVED:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_CARD_REMOVED;
                break;
              default:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
            }
          }

          sendTransferAtrResponse(in_token, resp, atr_data);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    std::vector<uint8_t> atr_data = {};
    sendTransferAtrResponse(
        in_token,
        UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE,
        atr_data);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimRemoteServiceServerImpl::uimRemoteServiceServerTransferCardReaderStatusReq(
    int32_t in_token)
{
  QCRIL_LOG_DEBUG("uimRemoteServiceServerTransferCardReaderStatusReq token=%d", in_token);
  auto msg = std::make_shared<UimSAPRequestMsg>(RIL_UIM_SAP_TRANSFER_CARD_READER_STATUS_REQ);
  if (msg) {
    GenericCallback<RIL_UIM_SAP_Response> cb((
        [this, in_token](std::shared_ptr<Message> solicitedMsg,
                         Message::Callback::Status ret_status,
                         std::shared_ptr<RIL_UIM_SAP_Response> responseDataPtr) -> void
        {
          UimRemoteServiceServerResultCode resp =
              UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
          int32_t status = -1;

          if (solicitedMsg && responseDataPtr && ret_status == Message::Callback::Status::SUCCESS) {
            status = responseDataPtr->reader_status;
            switch (responseDataPtr->result) {
              case RIL_UIM_SAP_SUCCESS:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_SUCCESS;
                break;
              case RIL_UIM_SAP_CARD_NOT_ACCESSSIBLE:
              case RIL_UIM_SAP_CARD_REMOVED:
                resp =
                    UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_DATA_NOT_AVAILABLE;
                break;
              default:
                resp = UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE;
            }
          }

          sendTransferCardReaderStatusResponse(in_token, resp, status);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    int32_t status = -1;
    sendTransferCardReaderStatusResponse(
        in_token,
        UimRemoteServiceServerResultCode::UIM_REMOTE_SERVICE_SERVER_GENERIC_FAILURE,
        status);
  }
  return ndk::ScopedAStatus::ok();
}

void IUimRemoteServiceServerImpl::sendApduResponse(
    int32_t token,
    aidlimports::UimRemoteServiceServerResultCode resultCode,
    const std::vector<uint8_t>& in_apduRsp)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerApduResponse token=%d ", token);
    auto ret = respCb->uimRemoteServiceServerApduResponse(token, resultCode, in_apduRsp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendConnectResponse(
    int32_t token, aidlimports::UimRemoteServiceServerConnectRsp sapConnectRsp, int32_t maxMsgSize)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerConnectResponse token=%d", token);
    auto ret = respCb->uimRemoteServiceServerConnectResponse(token, sapConnectRsp, maxMsgSize);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendDisconnectResponse(int32_t token)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerDisconnectResponse token=%d", token);
    auto ret = respCb->uimRemoteServiceServerDisconnectResponse(token);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendErrorResponse(int32_t token)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerErrorResponse token=%d", token);
    auto ret = respCb->uimRemoteServiceServerErrorResponse(token);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendPowerResponse(
    int32_t token, aidlimports::UimRemoteServiceServerResultCode resultCode)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerPowerResponse token=%d", token);
    auto ret = respCb->uimRemoteServiceServerPowerResponse(token, resultCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendResetSimResponse(
    int32_t token, aidlimports::UimRemoteServiceServerResultCode resultCode)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerResetSimResponse token=%d", token);
    auto ret = respCb->uimRemoteServiceServerResetSimResponse(token, resultCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendTransferAtrResponse(
    int32_t token,
    aidlimports::UimRemoteServiceServerResultCode resultCode,
    const std::vector<uint8_t>& in_atr)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerTransferAtrResponse token=%d ", token);
    auto ret = respCb->uimRemoteServiceServerTransferAtrResponse(token, resultCode, in_atr);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendTransferCardReaderStatusResponse(
    int32_t token, aidlimports::UimRemoteServiceServerResultCode resultCode, int32_t cardReaderStatus)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerTransferCardReaderStatusResponse token=%d ", token);
    auto ret = respCb->uimRemoteServiceServerTransferCardReaderStatusResponse(
        token, resultCode, cardReaderStatus);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::sendTransferProtocolResponse(
    int32_t token, aidlimports::UimRemoteServiceServerResultCode resultCode)
{
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerTransferProtocolResponse token=%d", token);
    auto ret = respCb->uimRemoteServiceServerTransferProtocolResponse(token, resultCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::uimRemoteServiceServerDisconnectIndication(
    aidlimports::UimRemoteServiceServerDisconnectType disconnectType)
{
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerDisconnectIndication");
    auto ret = indCb->uimRemoteServiceServerDisconnectIndication(disconnectType);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimRemoteServiceServerImpl::uimRemoteServiceServerStatusIndication(
    aidlimports::UimRemoteServiceServerStatus status)
{
  auto indCb = getIndicationCallback();
  if (indCb) {
    QCRIL_LOG_DEBUG("uimRemoteServiceServerStatusIndication");
    auto ret = indCb->uimRemoteServiceServerStatusIndication(status);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send Indication. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

}  // namespace implementation
}  // namespace uim_remote_server
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
