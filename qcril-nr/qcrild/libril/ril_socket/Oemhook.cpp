/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <ril_socket_api.h>
#include "interfaces/dms/dms_types.h"
#include "interfaces/dms/RilRequestGetMaxDataAllowedMessage.h"
#include "interfaces/nas/RilRequestGetLPlusLFeatureSupportStatusMessage.h"
#include "interfaces/nas/RilRequestGetCsgIdMessage.h"
#include "interfaces/nas/RilRequestSetUiStatusMessage.h"
#include "interfaces/nas/RilRequestGetPreferredNeworkBandPrefMessage.h"
#include "interfaces/nas/RilRequestSetPreferredNeworkBandPrefMessage.h"
#include "interfaces/nas/RilRequestEnableEngineerModeMessage.h"
#include "interfaces/nas/RilRequestCsgPerformNetworkScanMessage.h"
#include "interfaces/nas/RilRequestCsgSetSysSelPrefMessage.h"
#include "interfaces/nas/nas_types.h"
#include "interfaces/nv/RilRequestNvReadMessage.h"
#include "interfaces/nv/RilRequestNvWriteMessage.h"
#include "interfaces/rfrpe/RilRequestSetRfmScenarioMessage.h"
#include "interfaces/rfrpe/RilRequestGetRfmScenarioMessage.h"
#include "interfaces/rfrpe/RilRequestGetProvTableRevMessage.h"
#include "interfaces/sar/RilRequestGetSarRevKeyMessage.h"
#include "interfaces/sar/RilRequestSetTransmitPowerMessage.h"
#include <interfaces/voice/QcRilRequestRejectIncomingCallMessage.h>
#include <interfaces/nas/RilRequestStartNetworkScanMessage.h>
#include <interfaces/nas/RilRequestStopNetworkScanMessage.h>
#include <interfaces/nas/RilRequestGetSubProvisioningPrefMessage.h>
#include <interfaces/nas/RilRequestSetSubProvisioningPrefMessage.h>
#include <interfaces/nas/RilRequestGetSimIccidMessage.h>
#include <Marshal.h>
#include "qcril_qmi_oem_reqlist.h"
#include "qcril_qmi_oemhook_utils.h"
#include "misc.h"
#include "qcrili.h"
#include "services/qtuner_v01.h"

#undef TAG
#define TAG "RILQ"

namespace ril {
namespace socket {
namespace api {

RIL_Errno rejectIncomingCallWithCause21(std::shared_ptr<SocketRequestContext> context,
                                                uint8_t * /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<QcRilRequestRejectIncomingCallMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getMaxDataAllowed(std::shared_ptr<SocketRequestContext> context, uint8_t* /*data*/,
                            uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetMaxDataAllowedMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          uint8_t maxDataAllowed = 1;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::MaxDataAllowedResult_t>(resp->data);
            if (respData) {
              maxDataAllowed = respData->maxDataSubscriptions;
            }
          }
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (p) {
            auto marshalResult = p->write((const char*)&maxDataAllowed, sizeof(maxDataAllowed));
            if (marshalResult != Marshal::Result::SUCCESS) {
              QCRIL_LOG_ERROR("Failed to write to marshal");
              p = nullptr;
              errorCode = RIL_E_GENERIC_FAILURE;
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}
RIL_Errno getLPlusLFeatureSupportStatus(std::shared_ptr<SocketRequestContext> context,
                                        uint8_t* /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetLPlusLFeatureSupportStatusMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          uint8_t lPlusLStatus = FALSE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::RilLPlusLFeatureSupportStatus_t>(
                    resp->data);
            if (respData) {
              if (respData->mSupported) {
                lPlusLStatus = TRUE;
              }
            }
          }
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (p) {
            auto marshalResult = p->write((const char*)&lPlusLStatus, sizeof(lPlusLStatus));
            if (marshalResult != Marshal::Result::SUCCESS) {
              QCRIL_LOG_ERROR("Failed to write to marshal");
              p = nullptr;
              errorCode = RIL_E_GENERIC_FAILURE;
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno setUiStatus(std::shared_ptr<SocketRequestContext> context, uint8_t* data, uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    if (!data || !dataLen) {
      QCRIL_LOG_ERROR("invalid parameter: data is empty");
      result = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    uint8_t uiReady = 0;
    if (dataLen >= sizeof(uiReady)) {
      memcpy(&uiReady, data, sizeof(uiReady));
    }
    auto msg = std::make_shared<RilRequestSetUiStatusMessage>(context, uiReady);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getPreferredNetworkBandPref(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                                      uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    if (!data || !dataLen) {
      QCRIL_LOG_ERROR("invalid parameter: data is empty");
      result = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    uint32_t ratBandType = 0;
    if (dataLen >= 4) {
      memcpy(&ratBandType, data, 4);
    }
    auto msg = std::make_shared<RilRequestGetPreferredNeworkBandPrefMessage>(
        context, static_cast<qcril::interfaces::RilRatBandType>(ratBandType));
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::GetPreferredNeworkBandPrefResult_t>(
                    resp->data);
            if (respData) {
              uint32_t ratBandMap = static_cast<uint32_t>(respData->bandPrefMap);
              if (p) {
                auto marshalResult = p->write((const char*)&ratBandMap, sizeof(ratBandMap));
                if (marshalResult != Marshal::Result::SUCCESS) {
                  QCRIL_LOG_ERROR("Failed to write to marshal");
                  p = nullptr;
                  errorCode = RIL_E_GENERIC_FAILURE;
                }
              }
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno setPreferredNetworkBandPref(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                                      uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    if (!data || !dataLen) {
      QCRIL_LOG_ERROR("invalid parameter: data is empty");
      result = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    uint32_t bandPrefMap = 0;
    if (dataLen >= 4) {
      memcpy(&bandPrefMap, data, 4);
    }
    auto msg = std::make_shared<RilRequestSetPreferredNeworkBandPrefMessage>(
        context, static_cast<qcril::interfaces::RilBandPrefType>(bandPrefMap));
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getCsgId(std::shared_ptr<SocketRequestContext> context, uint8_t* /*data*/,
                   uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetCsgIdMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          uint32_t csgId = FALSE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::GetCsgIdResult_t>(resp->data);
            if (respData) {
              csgId = respData->mCsgId;
            }
          }
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (p) {
            auto marshalResult = p->write((const char*)&csgId, sizeof(csgId));
            if (marshalResult != Marshal::Result::SUCCESS) {
              QCRIL_LOG_ERROR("Failed to write to marshal");
              p = nullptr;
              errorCode = RIL_E_GENERIC_FAILURE;
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno enableEngineerMode(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                              uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    auto msg = std::make_shared<RilRequestEnableEngineerModeMessage>(context);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    result = msg->setData(data, dataLen);
    if (result != RIL_E_SUCCESS) {
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getSarRevKey(std::shared_ptr<SocketRequestContext> context, uint8_t* /*data*/,
                   uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetSarRevKeyMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          uint32_t key = 0;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::GetSarRevKeyResult>(resp->data);
            if (respData) {
              key = respData->key;
            }
          }
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (p) {
            auto marshalResult = p->write((const char*)&key, sizeof(key));
            if (marshalResult != Marshal::Result::SUCCESS) {
              QCRIL_LOG_ERROR("Failed to write to marshal");
              p = nullptr;
              errorCode = RIL_E_GENERIC_FAILURE;
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno setSarTransmitPower(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                              uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    auto msg = std::make_shared<RilRequestSetTransmitPowerMessage>(context);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    result = msg->setData(data, dataLen);
    if (result != RIL_E_SUCCESS) {
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno setRfmScenarioReq(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                            uint32_t dataLen) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  if (data && dataLen) {
    Qtuner_set_scenario_req_v01* req = (Qtuner_set_scenario_req_v01*)data;
    auto msg = std::make_shared<RilRequestSetRfmScenarioMessage>(context, *req);
    if (msg) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                     std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            uint8_t* encodedResp = nullptr;
            uint32_t encodedRespLen = 0;
            if (oemhook::utils::encodeGenericResponsePayLoad(
                    nullptr, 0, QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER,
                    QMI_Qtuner_SET_RFM_SCENARIO_RESP_V01, errorCode, encodedResp, encodedRespLen)) {
              errorCode = RIL_E_SUCCESS;
              QCRIL_LOG_DEBUG("encodedResp");
              qcril_qmi_print_hex(encodedResp, encodedRespLen);
            }
            std::shared_ptr<Marshal> p;
            if (encodedResp && encodedRespLen) {
              p = std::make_shared<Marshal>();
              if (p) {
                auto marshalResult = p->write(encodedResp, encodedRespLen);
                if (marshalResult != Marshal::Result::SUCCESS) {
                  QCRIL_LOG_ERROR("Failed to write to marshal");
                  p = nullptr;
                }
              }
            }
            sendResponse(context, errorCode, p);
            if (encodedResp) {
              free(encodedResp);
            }
          }));
      msg->setCallback(&cb);
      msg->dispatch();
      result = RIL_E_SUCCESS;
    }
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getRfmScenarioReq(std::shared_ptr<SocketRequestContext> context, uint8_t* /*data*/,
                            uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetRfmScenarioMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::GetRfmScenarioResult> respData;
          uint8_t* rawResp = nullptr;
          uint32_t rawRespLen = 0;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData = std::static_pointer_cast<qcril::interfaces::GetRfmScenarioResult>(resp->data);
            if (respData) {
              rawResp = (uint8_t*)&(respData->response);
              rawRespLen = sizeof(respData->response);
            }
          }
          uint8_t* encodedResp = nullptr;
          uint32_t encodedRespLen = 0;
          if (oemhook::utils::encodeGenericResponsePayLoad(
                  rawResp, rawRespLen, QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER,
                  QMI_Qtuner_GET_RFM_SCENARIO_RESP_V01, errorCode, encodedResp, encodedRespLen)) {
            errorCode = RIL_E_SUCCESS;
            QCRIL_LOG_DEBUG("encodedResp");
            qcril_qmi_print_hex(encodedResp, encodedRespLen);
          }
          std::shared_ptr<Marshal> p;
          if (encodedResp && encodedRespLen) {
            p = std::make_shared<Marshal>();
            if (p) {
              auto marshalResult = p->write(encodedResp, encodedRespLen);
              if (marshalResult != Marshal::Result::SUCCESS) {
                QCRIL_LOG_ERROR("Failed to write to marshal");
                p = nullptr;
              }
            }
          }
          sendResponse(context, errorCode, p);
          if (encodedResp) {
            free(encodedResp);
          }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getProvisionedTableRevisionReq(std::shared_ptr<SocketRequestContext> context,
                                         uint8_t* /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;
  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetProvTableRevMessage>(context);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::GetProvTableRevResult> respData;
          uint8_t* rawResp = nullptr;
          uint32_t rawRespLen = 0;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            respData =
                std::static_pointer_cast<qcril::interfaces::GetProvTableRevResult>(resp->data);
            if (respData) {
              rawResp = (uint8_t*)&(respData->response);
              rawRespLen = sizeof(respData->response);
            }
          }
          uint8_t* encodedResp = nullptr;
          uint32_t encodedRespLen = 0;
          if (oemhook::utils::encodeGenericResponsePayLoad(
                  rawResp, rawRespLen, QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER,
                  QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_RESP_V01, errorCode, encodedResp,
                  encodedRespLen)) {
            errorCode = RIL_E_SUCCESS;
            QCRIL_LOG_DEBUG("encodedResp");
            qcril_qmi_print_hex(encodedResp, encodedRespLen);
          }
          std::shared_ptr<Marshal> p;
          if (encodedResp && encodedRespLen) {
            p = std::make_shared<Marshal>();
            if (p) {
              auto marshalResult = p->write(encodedResp, encodedRespLen);
              if (marshalResult != Marshal::Result::SUCCESS) {
                QCRIL_LOG_ERROR("Failed to write to marshal");
                p = nullptr;
              }
            }
          }
          sendResponse(context, errorCode, p);
          if (encodedResp) {
            free(encodedResp);
          }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno performCsgNeworkScan(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                               uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    auto msg = std::make_shared<RilRequestCsgPerformNetworkScanMessage>(context);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    result = msg->setData(data, dataLen);
    if (result != RIL_E_SUCCESS) {
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData =
                std::static_pointer_cast<qcril::interfaces::CsgPerformNwScanResult_t>(resp->data);
            if (respData) {
              std::vector<uint8_t> tlvStream = respData->toTlv();
              if (tlvStream.size()) {
                if (p) {
                  auto marshalResult = p->write(tlvStream.data(), tlvStream.size());
                  if (marshalResult != Marshal::Result::SUCCESS) {
                    QCRIL_LOG_ERROR("Failed to write to marshal");
                    p = nullptr;
                    errorCode = RIL_E_GENERIC_FAILURE;
                  }
                }
              }
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno setCsgSystemSelectionPreference(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                               uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    auto msg = std::make_shared<RilRequestCsgSetSysSelPrefMessage>(context);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    result = msg->setData(data, dataLen);
    if (result != RIL_E_SUCCESS) {
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                   std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno nvRead(std::shared_ptr<SocketRequestContext> context, uint8_t *data, uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    auto msg = std::make_shared<RilRequestNvReadMessage>(context);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    result = msg->setData(data, dataLen);
    if (result != RIL_E_SUCCESS) {
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            auto respData = std::static_pointer_cast<qcril::interfaces::NvReadResult_t>(resp->data);
            if (respData) {
              if (respData->nvItemValueSize) {
                if (p) {
                  auto marshalResult = p->write((const char *)&(respData->nvItemValue),
                                                respData->nvItemValueSize);
                  if (marshalResult != Marshal::Result::SUCCESS) {
                    QCRIL_LOG_ERROR("Failed to write to marshal");
                    p = nullptr;
                    errorCode = RIL_E_GENERIC_FAILURE;
                  }
                }
              }
            }
          }
          sendResponse(context, errorCode, p);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno nvWrite(std::shared_ptr<SocketRequestContext> context, uint8_t *data, uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno result = RIL_E_NO_MEMORY;
  do {
    auto msg = std::make_shared<RilRequestNvWriteMessage>(context);
    if (!msg) {
      result = RIL_E_NO_MEMORY;
      break;
    }
    result = msg->setData(data, dataLen);
    if (result != RIL_E_SUCCESS) {
      QCRIL_LOG_ERROR("Invalid data received : err = %d", result);
      break;
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = dispatchStatusToRilErrno(status);
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  } while (0);

  QCRIL_LOG_FUNC_RETURN();
  return result;
}


RIL_Errno processGenericRequest(std::shared_ptr<SocketRequestContext> context, uint8_t* data,
                                uint32_t dataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
  uint16_t serviceId = 0;
  uint16_t messageId = 0;
  uint8_t* msgData = nullptr;
  uint32_t msgDataLen = 0;

  bool result = oemhook::utils::decodeGenericRequest(data, dataLen, serviceId, messageId, msgData,
                                                     msgDataLen);

  if (result) {
    QCRIL_LOG_DEBUG("serviceId = %d, messageId = 0x%X", serviceId, messageId);
    if (serviceId == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER) {
      switch (messageId) {
        case QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01:
          errorCode = setRfmScenarioReq(context, msgData, msgDataLen);
          break;
        case QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01:
          errorCode = getRfmScenarioReq(context, msgData, msgDataLen);
          break;
        case QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01:
          errorCode = getProvisionedTableRevisionReq(context, msgData, msgDataLen);
          break;
        default:
          errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
          break;
      }
    }
  }
  if (msgData) {
    free(msgData);
  }

  return errorCode;
}

RIL_Errno startIncrementalNetworkScan(std::shared_ptr<SocketRequestContext> context,
                                       uint8_t * /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;

  QCRIL_LOG_FUNC_ENTRY();

  auto msg = std::make_shared<RilRequestStartNetworkScanMessage>(context);
  msg->setRequestSource(RilRequestStartNetworkScanMessage::RequestSource::OEM);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                  std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }

  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno cancelIncrementalNetworkScan(std::shared_ptr<SocketRequestContext> context,
                                       uint8_t * /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;

  QCRIL_LOG_FUNC_ENTRY();

  auto msg = std::make_shared<RilRequestStopNetworkScanMessage>(context);

  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);  // TODO - map radio error
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}


RIL_Errno getSubProvPreference(std::shared_ptr<SocketRequestContext> context,
                                       uint8_t * /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;

  QCRIL_LOG_FUNC_ENTRY();

  auto msg = std::make_shared<RilRequestGetSubProvisioningPrefMessage>(context);

  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;

            auto prefResp =
              std::static_pointer_cast<qcril::interfaces::RILSubProvStatus_t>(resp->data);
            std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
            if (p) {
              int32_t resp[2];
              resp[0] = prefResp->user_preference;
              resp[1] = prefResp->current_sub_preference;
              auto marshalResult = p->write((uint8_t *)resp, sizeof(resp));
              if (marshalResult != Marshal::Result::SUCCESS) {
                QCRIL_LOG_ERROR("Failed to write to marshal");
                p = nullptr;
                errorCode = RIL_E_GENERIC_FAILURE;
              }
              sendResponse(context, errorCode, p);
            } else {
              sendResponse(context, errorCode, nullptr);
            }
          } else {
            sendResponse(context, errorCode, nullptr);
          }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno setSubProvPreference(std::shared_ptr<SocketRequestContext> context,
                                       uint8_t *data, uint32_t dataLen) {
  RIL_Errno result = RIL_E_NO_MEMORY;

  QCRIL_LOG_FUNC_ENTRY();
  uint16_t dataIndex = 0;


  if ((NULL == data) || (2 > dataLen)) {
    result = RIL_E_INVALID_ARGUMENTS;
    return result;
  }

  // fetch sub ID
  int32_t act_status = data[dataIndex];
  dataIndex++;

  int32_t sub_id = data[dataIndex];
  dataIndex++;

  qcril::interfaces::RILSetSubProvState_t provState(act_status, sub_id);

  auto msg = std::make_shared<RilRequestSetSubProvisioningPrefMessage>(context, provState);

  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponse(context, errorCode, nullptr);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

RIL_Errno getSimIccid(std::shared_ptr<SocketRequestContext> context,
                                       uint8_t * /*data*/, uint32_t /*dataLen*/) {
  RIL_Errno result = RIL_E_NO_MEMORY;

  QCRIL_LOG_FUNC_ENTRY();
  auto msg = std::make_shared<RilRequestGetSimIccidMessage>(context);

  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([context](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            auto prefResp =
              std::static_pointer_cast<qcril::interfaces::RilIccidResult_t>(resp->data);
            std::shared_ptr<Marshal> p = std::make_shared<Marshal>();
            if (p) {
              std::string iccid = prefResp->mData;
              auto marshalResult = p->write((const char *)iccid.c_str(),iccid.length());
              if (marshalResult != Marshal::Result::SUCCESS) {
                QCRIL_LOG_ERROR("Failed to write to marshal");
                p = nullptr;
                errorCode = RIL_E_GENERIC_FAILURE;
              }
              sendResponse(context, errorCode, p);
            } else {
              sendResponse(context, errorCode, nullptr);
            }
          } else {
            sendResponse(context, errorCode, nullptr);
          }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    result = RIL_E_SUCCESS;
  }
  QCRIL_LOG_FUNC_RETURN();
  return result;
}

/*
 * Oemhook Request has the following message format
 *  [OEMNAME (8 bytes MANDATORY)]
 *  [COMMAND ID (4 bytes MANDATORY)]
 *  [REQ DATA LENGTH (4 bytes OPTIONAL)]
 *  [REQ DATA (REQ DATA LENGTH bytes OPTIONAL)]
 */
void dispatchOemhookRaw(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  QCRIL_LOG_FUNC_ENTRY();
  RIL_Errno errorCode = RIL_E_SUCCESS;
  uint8_t *data = nullptr;
  size_t dataLen = 0;
  do {
    if (p.dataAvail()) {
      p.readAndAlloc(data, dataLen);
    }

    uint8_t *dataPtr = data;
    if (dataPtr == nullptr) {
      QCRIL_LOG_ERROR("Invalid parameters; data is null");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    qcril_qmi_print_hex(dataPtr, dataLen);
    // [OEMNAME (8 bytes MANDATORY)]
    if (dataLen < QCRIL_OTHER_OEM_NAME_LENGTH) {
      QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }
    dataLen -= QCRIL_OTHER_OEM_NAME_LENGTH;
    dataPtr += QCRIL_OTHER_OEM_NAME_LENGTH;

    uint32_t cmd_id = 0;
    // [COMMAND ID (4 bytes MANDATORY)]
    if (dataLen < QCRIL_OTHER_OEM_REQUEST_ID_LEN) {
      QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      return;
    }
    memcpy(&cmd_id, dataPtr, QCRIL_OTHER_OEM_REQUEST_ID_LEN);
    dataLen -= QCRIL_OTHER_OEM_REQUEST_ID_LEN;
    dataPtr += QCRIL_OTHER_OEM_REQUEST_ID_LEN;

    uint32_t reqLen = 0;
    uint8_t *reqData = nullptr;
    // [REQ DATA LENGTH (4 bytes OPTIONAL)]
    if (dataLen > QCRIL_OTHER_OEM_REQUEST_DATA_LEN) {
      memcpy(&reqLen, dataPtr, QCRIL_OTHER_OEM_REQUEST_DATA_LEN);
      dataLen -= QCRIL_OTHER_OEM_REQUEST_DATA_LEN;
      dataPtr += QCRIL_OTHER_OEM_REQUEST_DATA_LEN;
      // [REQ DATA (REQ DATA LENGTH bytes OPTIONAL)]
      reqData = dataPtr;
    }

    // Return failure if REQ DATA LENGTH is larger than the actual data length.
    if (reqLen > dataLen) {
      QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
      errorCode = RIL_E_INVALID_ARGUMENTS;
      break;
    }

    QCRIL_LOG_DEBUG("cmd_id = 0x%X, reqLen = %d", cmd_id, reqLen);
    switch (cmd_id) {
      case QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21:
        errorCode = rejectIncomingCallWithCause21(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ:
        errorCode = getMaxDataAllowed(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ:
        errorCode = getLPlusLFeatureSupportStatus(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS:
        errorCode = setUiStatus(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF:
        errorCode = getPreferredNetworkBandPref(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF:
        errorCode = setPreferredNetworkBandPref(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_CSG_ID:
        errorCode = getCsgId(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_ENABLE_ENGINEER_MODE:
        errorCode = enableEngineerMode(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_SAR_REV_KEY:
        errorCode = getSarRevKey(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_SET_TRANSMIT_POWER:
        errorCode = setSarTransmitPower(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN:
        errorCode = performCsgNeworkScan(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF:
        errorCode = setCsgSystemSelectionPreference(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_NV_READ:
        errorCode = nvRead(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_NV_WRITE:
        errorCode = nvWrite(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_PERFORM_INCREMENTAL_NW_SCAN:
        errorCode = startIncrementalNetworkScan(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_CANCEL_QUERY_AVAILABLE_NETWORK:
        errorCode = cancelIncrementalNetworkScan(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ:
        errorCode = getSubProvPreference(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ:
        errorCode = setSubProvPreference(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_GET_SIM_ICCID_REQ:
        errorCode = getSimIccid(context, reqData, reqLen);
        break;
      case QCRIL_REQ_HOOK_REQ_GENERIC:
        errorCode = processGenericRequest(context, reqData, reqLen);
        if (errorCode != RIL_E_REQUEST_NOT_SUPPORTED) {
          break;
        }
        // fallthrough if request not supported cases for default implementation.
        [[fallthrough]];
      default:
        errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
        break;
    }
  } while (FALSE);

  p.release(data);

  if (errorCode != RIL_E_SUCCESS) {
    sendResponse(context, errorCode, nullptr);
  }
  QCRIL_LOG_FUNC_RETURN();
}

}  // namespace api
}  // namespace socket
}  // namespace ril
