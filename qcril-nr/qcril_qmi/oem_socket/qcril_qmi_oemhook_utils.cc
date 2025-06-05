/******************************************************************************
#  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qcril_qmi_oemhook_utils.h"
#include "qcril_qmi_oemhook_agent.h"
#include "qcril_qmi_oem_reqlist.h"
#include "qcrili.h"
#include "qcril_memory_management.h"
#include "qcril_otheri.h"
#include <string.h>
#include <framework/Log.h>

#undef TAG
#define TAG "RILQ"

namespace oemhook {
namespace utils {

uint8_t* constructOemHookRaw
(
  uint32_t* length,
  int unsol_event,
  uint8_t* data,
  uint32_t data_len
)
{
    size_t payload_len = (QCRIL_QMI_OEM_INT_VAL_LEN + QCRIL_OTHER_OEM_NAME_LENGTH +
                          sizeof(unsol_event) + sizeof(data_len) + data_len);
    uint8_t *payload = new uint8_t[payload_len];
    uint32_t index = 0;
    if ( NULL != payload )
    {
        /* Unsolicited oemhook message has following message format
          [RIL_UNSOL_OEM_HOOK_RAW (4)]
          [OEM_NAME(8)] [Message Id (4) ] [Payload Length (4) ] [Payload]
        */
        int unsol_hook_raw = RIL_UNSOL_OEM_HOOK_RAW;
        memcpy( payload, &(unsol_hook_raw), QCRIL_QMI_OEM_INT_VAL_LEN );
        index += QCRIL_QMI_OEM_INT_VAL_LEN;

        memcpy( &payload[index], QCRIL_HOOK_OEM_NAME, QCRIL_OTHER_OEM_NAME_LENGTH );
        index += QCRIL_OTHER_OEM_NAME_LENGTH;

        memcpy( &payload[index], &unsol_event, sizeof(unsol_event) );
        index += sizeof(unsol_event);

        memcpy( &payload[index], &data_len, sizeof(data_len) );
        index += sizeof(data_len);

        if( data!= NULL && data_len > 0 ) {
            memcpy( &payload[index], data, data_len );
        }
        index += data_len;
    }

    *length = index;
    return payload;
}

bool encodeGenericResponsePayLoad(const uint8_t* rawData, uint32_t rawLen, uint16_t serviceId,
                                  uint16_t messageId, RIL_Errno& errorCode, uint8_t*& respPayLoad,
                                  uint32_t& respPayloadLen) {
  /* Response oemhook has following message format
     [Oemhook Req Id (4) ] [Payload Length (4) ] [Payload]
     In case of VT|Presence OemHook Service
     Payload :
     [SRV_ID(2)][MSG_ID(2)][Error Code(2)][Actual Payload]
   */
  QCRIL_LOG_DEBUG("Generic Response Message Encode");

  auto srvObj = qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object(
      (qmi_ril_oem_hook_qmi_tunneling_service_id_type)serviceId);
  if (!srvObj) {
    QCRIL_LOG_DEBUG("Could not create required srv obj");
    return false;
  }
  uint32_t tlvStreamLen = 0;
  char* payload = nullptr;
  auto idlError = qmi_idl_get_max_message_len(srvObj, QMI_IDL_RESPONSE, messageId, &tlvStreamLen);
  if (QMI_NO_ERR != idlError) {
    QCRIL_LOG_ERROR("QMI IDL - request decode error %d", (int)idlError);
    return false;
  }
  QCRIL_LOG_DEBUG("max length = %d, msg_id = %d", tlvStreamLen, messageId);
  payload = (char*)qcril_malloc(tlvStreamLen + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE);
  if (!payload) {
    QCRIL_LOG_DEBUG("Memory Allocation Failed");
    return false;
  }
  uint32_t encodedFact = QMI_RIL_ZERO;
  if (rawData && rawLen) {
    idlError = qmi_idl_message_encode(srvObj, QMI_IDL_RESPONSE, messageId, rawData, rawLen,
                                      payload + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE,
                                      tlvStreamLen, &encodedFact);
  }

  // complete the oem hook tunneling header
  char* ptr = payload;
  // oemhookRequestId
  uint32_t oemhookRequestId = QCRIL_REQ_HOOK_REQ_GENERIC;
  memcpy(payload, &oemhookRequestId, OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE);
  ptr += OEM_HOOK_QMI_TUNNELING_RESP_REQUEST_ID_SIZE;
  // payload_size
  uint32_t payloadSize = encodedFact + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE +
                         OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE + OEM_HOOK_QMI_TUNNELING_ERROR_CODE_SIZE;
  memcpy(ptr, &payloadSize, OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE);
  ptr += OEM_HOOK_QMI_TUNNELING_RESP_RESP_SZ_SIZE;
  // service_id
  memcpy(ptr, &serviceId, OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE);
  ptr += OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE;
  // message_id
  memcpy(ptr, &messageId, OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE);
  ptr += OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;
  // error_code
  uint16_t tempErrorCode = errorCode;
  if (idlError != QMI_NO_ERR) {
    tempErrorCode = idlError;
  }
  memcpy(ptr, &tempErrorCode, OEM_HOOK_QMI_TUNNELING_ERROR_CODE_SIZE);

  respPayLoad = (uint8_t*)payload;
  respPayloadLen = encodedFact + OEM_HOOK_QMI_TUNNELING_RESP_OVERHEAD_SIZE;
  return true;
}

bool encodeGenericUnsolPayload(uint8_t*& rawData, uint32_t& rawLen, uint16_t serviceId,
                               uint16_t messageId) {
  /* Unsolicited oemhook message has following message format
     [RIL_UNSOL_OEM_HOOK_RAW (4)]
     [OEM_NAME(8)] [Message Id (4) ] [Payload Length (4) ] [Payload]
     In case of VT|Presence OemHook Service
     Payload :
     [SRV_ID(2)][MSG_ID(2)][Actual Payload]
   */
  QCRIL_LOG_DEBUG("Generic Unsol Message Encode");
  if (rawData == nullptr || rawLen < 1) {
    QCRIL_LOG_DEBUG("Invalid Message received for encoding");
    return false;
  }
  auto srvObj = qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object(
      static_cast<qmi_ril_oem_hook_qmi_tunneling_service_id_type>(serviceId));
  if (srvObj) {
    uint32_t tlvStreamLen = 0;
    auto idlError =
        qmi_idl_get_max_message_len(srvObj, QMI_IDL_INDICATION, messageId, &tlvStreamLen);
    if (QMI_NO_ERR == idlError) {
      QCRIL_LOG_DEBUG("max length = %d, msg_id = %d", tlvStreamLen, messageId);
      uint32_t SRV_MSG_ID_LEN =
          OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE + OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;
      char* payload = qcril_malloc2(payload, tlvStreamLen + SRV_MSG_ID_LEN);
      if (payload) {
        uint32_t encodedFact = QMI_RIL_ZERO;
        idlError = qmi_idl_message_encode(srvObj, QMI_IDL_INDICATION, messageId, rawData, rawLen,
                                          payload + SRV_MSG_ID_LEN, tlvStreamLen, &encodedFact);

        if (QMI_NO_ERR == idlError) {
          // complete the oem hook tunneling header
          memcpy(payload, &serviceId, OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE);
          memcpy(payload + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE, &messageId,
                 OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE);
          rawData = (uint8_t*)payload;
          rawLen = encodedFact + SRV_MSG_ID_LEN;
          uint32_t bufLen = 0;
          uint8_t* buf = oemhook::utils::constructOemHookRaw(&bufLen, QCRIL_REQ_HOOK_UNSOL_GENERIC,
                                                             rawData, rawLen);
          if (buf) {
            rawData = buf;
            rawLen = bufLen;
          }
        } else {
          QCRIL_LOG_ERROR("QMI IDL - request decode error %d", (int)idlError);
          qcril_free(payload);
          return false;
        }
      } else {
        QCRIL_LOG_DEBUG("Memory Allocation Failed");
        return false;
      }
    } else {
      QCRIL_LOG_ERROR("QMI IDL - request decode error %d", (int)idlError);
      return false;
    }
  } else {
    QCRIL_LOG_DEBUG("Could not create required srv obj");
    return false;
  }
  return true;
}
bool decodeGenericRequest(const uint8_t* data, uint32_t dataLen, uint16_t& serviceId,
                          uint16_t& messageId, uint8_t*& msgData, uint32_t& msgDataLen) {
  QCRIL_LOG_FUNC_ENTRY();
  /* OEMHOOK request format
     [8 byte OemIdentifier][4 bytes RequestID][4bytes PayloadSize][PayloadSize bytes Payload]
     QCRIL_REQ_HOOK_REQ_GENERIC is used for VT and Presence.
     Actual message id need to be extracted from the payload
     Payload format is
     [8bytes ModemID][2bytes ServiceID][2bytes MessageID][ActualPayload]
  */
  // ignore the modem id size
  if (dataLen < OEM_HOOK_QMI_TUNNELING_REQ_MODEM_ID_SIZE + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE
                + OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE) {
    QCRIL_LOG_ERROR("Invalid data Length: %d",dataLen);
    return false;
  }
  data = data + OEM_HOOK_QMI_TUNNELING_REQ_MODEM_ID_SIZE;
  // extract serviceId
  memcpy(&serviceId, data, OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE);
  data = data + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE;
  // extract messageId
  memcpy(&messageId, data, OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE);
  data = data + OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;
  dataLen = dataLen - OEM_HOOK_QMI_TUNNELING_REQ_MODEM_ID_SIZE -
            OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE - OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;
  QCRIL_LOG_DEBUG("serviceId = %d, messageId = %d, dataLen = %d", serviceId, messageId, dataLen);
  auto srvObj = qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object(
      static_cast<qmi_ril_oem_hook_qmi_tunneling_service_id_type>(serviceId));
  if (!srvObj) {
    QCRIL_LOG_ERROR("Could not create required srv obj");
    return false;
  }
  uint32_t substitutedDataLen = 0;
  uint8_t* substitutedData = nullptr;
  auto idlError =
      qmi_idl_get_message_c_struct_len(srvObj, QMI_IDL_REQUEST, messageId, &substitutedDataLen);
  if (idlError != QMI_NO_ERR) {
    QCRIL_LOG_ERROR("QMI IDL - qmi_idl_get_message_c_struct_len error %d", (int)idlError);
    return false;
  }
  QCRIL_LOG_DEBUG("substitutedDataLen = %d", substitutedDataLen);
  if (substitutedDataLen) {
    substitutedData = qcril_malloc2(substitutedData, substitutedDataLen);
    if (!substitutedData) {
      QCRIL_LOG_DEBUG("Memory Allocation Failed");
      return false;
    }
    idlError = qmi_idl_message_decode(srvObj, QMI_IDL_REQUEST, messageId, data, dataLen,
                                      substitutedData, substitutedDataLen);
    if (idlError != QMI_NO_ERR) {
      QCRIL_LOG_ERROR("QMI IDL - qmi_idl_message_decode error %d", (int)idlError);
      qcril_free(substitutedData);
      return false;
    }
  }
  msgData = substitutedData;
  msgDataLen = substitutedDataLen;
  return true;
}

bool dataLengthCheck(uint32_t data_index, uint32_t data_len) {
  if( data_index >= data_len) {
    QCRIL_LOG_ERROR("Invalid parameters; invalid data length");
    return false;
  }
  return true;
}

} // namespace utils
} // namespace oemhook
