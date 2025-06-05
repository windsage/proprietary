/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK Utils
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>
#include <iomanip>

#include "OemhookUtils.hpp"
#include "services/qtuner_v01.h"
#include "qmi_client.h"

static bool debug = false;

std::tuple<const char*, size_t> packOemhookRequest(uint32_t messageId, const uint8_t* reqData,
                                                   size_t reqDataSize) {
  /*
   * Oemhook Request message format
   * [OEM_NAME(8)] [Request Id (4)] [Payload Length (4)] [Payload]
   */
  size_t dataLen = 0;
  dataLen += 8;  // [OEM_NAME(8)]
  dataLen += 4;  // [Request Id (4)]
  dataLen += 4;  // [Payload Length (4)]
  if (reqDataSize && reqData) {
    dataLen += reqDataSize;  // [Payload]
  }

  char* data = new char[dataLen];
  if (data) {
    if (dataLen >= 8) {
      // Identified as "QOEMHOOK"  : this is fixed for all OEMHOOK requests
      data[0] = 0x51;  // Q
      data[1] = 0x4F;  // O
      data[2] = 0x45;  // E
      data[3] = 0x4D;  // M
      data[4] = 0x48;  // H
      data[5] = 0x4F;  // O
      data[6] = 0x4F;  // O
      data[7] = 0x4B;  // K
    }

    if (dataLen >= 12) {
      // Request id  : OEMHOOK request ids
      data[11] = (messageId >> 24) & 0xFF;
      data[10] = (messageId >> 16) & 0xFF;
      data[9] = (messageId >> 8) & 0xFF;
      data[8] = (messageId)&0xFF;
    }

    if (dataLen >= 16) {
      // Payload Length
      data[15] = (reqDataSize >> 24) & 0xFF;
      data[14] = (reqDataSize >> 16) & 0xFF;
      data[13] = (reqDataSize >> 8) & 0xFF;
      data[12] = (reqDataSize)&0xFF;
    }

    if (reqDataSize && (dataLen == (16 + reqDataSize))) {
      for (size_t i = 0; i < reqDataSize; i++) {
        data[i + 16] = reqData[i];
      }
    }
  }

  if (debug) {
    std::cout << "packOemhookRequest: len = " << dataLen << ", data = ";
    if (data) {
      for (uint32_t i = 0; i < dataLen; i++) {
        std::cout << static_cast<uint32_t>(data[i]) << " ";
      }
    }
    std::cout << std::endl;
  }

  return std::make_tuple(data, dataLen);
}

std::tuple<uint32_t, const char*, size_t> unpackOemhookIndication(const char* data, size_t dataLen) {
  uint32_t messageId = 0;
  const char* respData = nullptr;
  uint32_t respDataLen = 0;
  /*
   * Unsolicited oemhook message format
   * [RIL_UNSOL_OEM_HOOK_RAW (4)]
   * [OEM_NAME(8)] [Message Id (4) ] [Payload Length (4) ] [Payload]
   */
  if (data) {
    if (dataLen >= 16) {
      memcpy((void*)&messageId, data + 12, sizeof(messageId));
    }
    if (dataLen >= 20) {
      memcpy((void*)&respDataLen, data + 16, sizeof(respDataLen));
    }
    if (respDataLen && dataLen >= respDataLen + 20) {
      respData = data + 20;
    }
  }

  return std::make_tuple(messageId, respData, respDataLen);
}

qmi_idl_service_object_type getServieObject(TunnelingServiceId serviceId) {
  if (serviceId == TUNNELING_SERVICE_QTUNER) {
    return Qtuner_get_service_object_v01();
  }
  return nullptr;
}

std::tuple<RIL_Errno, void*, size_t> unpackGenericResponsePayload(const char* msgData,
                                                                  uint32_t msgDataLen) {
  /* Response oemhook has following message format
     [Message Id (4) ] [Payload Length (4) ] [Payload]
     In case of VT|Presence OemHook Service
     Payload :
     [SRV_ID(2)][MSG_ID(2)][Error Code(2)][Actual Payload]
   */

  if (debug) {
    std::cout << "unpackGenericResponsePayload: msgDataLen = " << msgDataLen << std::endl;
  }

  /// Minimum length check
  if (msgDataLen < 4 + 4) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }
  // request_id
  uint32_t requestId = 0;
  memcpy(&requestId, msgData, 4);
  msgData += 4;
  msgDataLen -= 4;
  if (debug) {
    std::cout << "unpackGenericResponsePayload: requestId = 0x" << std::hex << requestId
              << std::endl;
  }
  if (requestId != QCRIL_REQ_HOOK_REQ_GENERIC) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }
  // payload_size
  uint32_t payloadSize = 0;
  memcpy(&payloadSize, msgData, 4);
  msgData += 4;
  msgDataLen -= 4;
  if (debug) {
    std::cout << "unpackGenericResponsePayload: payloadSize = " << payloadSize << std::endl;
  }
  if (payloadSize <= 0) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }

  // length not sufficient to read next param
  if (msgDataLen < 2) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }
  // service_id
  uint16_t serviceId = 0;
  memcpy(&serviceId, msgData, 2);
  msgData += 2;
  msgDataLen -= 2;
  if (debug) {
    std::cout << "unpackGenericResponsePayload: serviceId = " << serviceId << std::endl;
  }
  if (serviceId != TUNNELING_SERVICE_QTUNER) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }

  // length not sufficient to read next param
  if (msgDataLen < 2) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }
  // message_id
  uint16_t messageId = 0;
  memcpy(&messageId, msgData, 2);
  msgData += 2;
  msgDataLen -= 2;
  if (debug) {
    std::cout << "unpackGenericResponsePayload: messageId = 0x" << std::hex << messageId
              << std::endl;
  }

  // length not sufficient to read next param
  if (msgDataLen < 2) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }
  // error_code
  uint16_t tempErrorCode = 0;
  memcpy(&tempErrorCode, msgData, 2);
  msgData += 2;
  msgDataLen -= 2;
  RIL_Errno errorCode = (RIL_Errno)tempErrorCode;
  if (debug) {
    std::cout << "unpackGenericResponsePayload: tempErrorCode = " << tempErrorCode << std::endl;
  }

  qmi_idl_service_object_type srvObj = getServieObject((TunnelingServiceId)serviceId);
  if (!srvObj) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }

  uint32_t substitutedDataLen = 0;
  char* substitutedData = nullptr;
  auto idlError =
      qmi_idl_get_message_c_struct_len(srvObj, QMI_IDL_RESPONSE, messageId, &substitutedDataLen);
  if (idlError != QMI_NO_ERR) {
    return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
  }

  if (debug) {
    std::cout << "unpackGenericResponsePayload: substitutedDataLen = " << substitutedDataLen
              << std::endl;
  }
  if (substitutedDataLen) {
    substitutedData = (char*)malloc(substitutedDataLen);
    if (!substitutedData) {
      return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
    }
    idlError = qmi_idl_message_decode(srvObj, QMI_IDL_RESPONSE, messageId, msgData, msgDataLen,
                                      substitutedData, substitutedDataLen);
    if (idlError != QMI_NO_ERR) {
      free(substitutedData);
      return std::make_tuple(RIL_E_GENERIC_FAILURE, nullptr, 0);
    }
  }

  return std::make_tuple(errorCode, substitutedData, substitutedDataLen);
}

std::tuple<const uint8_t*, size_t> packGenericRequestPayload(uint16_t serviceId, uint16_t messageId,
                                                             const void* src, uint32_t srcLen) {
  /*
     [8bytes ModemID][2bytes ServiceID][2bytes MessageID][ActualPayload]
   */
  qmi_idl_service_object_type srvObj = getServieObject((TunnelingServiceId)serviceId);
  if (!srvObj) {
    return std::make_tuple(nullptr, 0);
    ;
  }
  uint32_t tlvStreamLen = 0;
  uint8_t* payload = nullptr;
  if (debug) {
    std::cout << "packGenericRequestPayload: messageId = " << messageId << std::endl;
  }
  auto idlError = qmi_idl_get_max_message_len(srvObj, QMI_IDL_REQUEST, messageId, &tlvStreamLen);
  if (QMI_NO_ERR != idlError) {
    return std::make_tuple(nullptr, 0);
  }
  if (debug) {
    std::cout << "packGenericRequestPayload: tlvStreamLen = " << tlvStreamLen << std::endl;
  }
  size_t payloadLen = (tlvStreamLen + 8 + 2 + 2);
  payload = (uint8_t*)malloc(payloadLen);
  if (!payload) {
    if (debug) {
      std::cout << "packGenericRequestPayload: malloc failed" << std::endl;
    }
    return std::make_tuple(nullptr, 0);
  }
  uint32_t encodedFact = 0;
  if (src && srcLen) {
    idlError = qmi_idl_message_encode(srvObj, QMI_IDL_REQUEST, messageId, src, srcLen,
                                      payload + 8 + 2 + 2, tlvStreamLen, &encodedFact);
    if (debug) {
      std::cout << "packGenericRequestPayload: idlError = " << idlError
                << ", encodedFact = " << encodedFact << std::endl;
    }
  }
  uint8_t* ptr = payload;
  ptr += 8;
  // service_id
  memcpy(ptr, &serviceId, 2);
  ptr += 2;
  // service_id
  memcpy(ptr, &messageId, 2);
  return std::make_tuple(payload, encodedFact + 8 + 2 + 2);
}

void printBuffer(const char* data, size_t dataLen) {
  std::ostringstream rawData{};
  for (int i = 0; i < dataLen; i++) {
    rawData << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[i]) << " ";
  }
  std::cout << "dataLen : " << dataLen << std::endl;
  std::cout << "data : " << rawData.str() << std::endl;
}
