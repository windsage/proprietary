/**
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * OEMHOOK Utils
 */

#ifndef OEMHOOKUTILS_HPP
#define OEMHOOKUTILS_HPP

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include "RilApiSession.hpp"
#include "qmi_client.h"

#define QCRIL_REQ_HOOK_BASE (0x80000)

#define QCRIL_REQ_HOOK_NV_READ (QCRIL_REQ_HOOK_BASE + 1)
#define QCRIL_REQ_HOOK_NV_WRITE (QCRIL_REQ_HOOK_BASE + 2)
#define QCRIL_REQ_HOOK_ENABLE_ENGINEER_MODE (QCRIL_REQ_HOOK_BASE + 19)
#define QCRIL_REQ_HOOK_GET_CSG_ID (QCRIL_REQ_HOOK_BASE + 24)
#define QCRIL_REQ_HOOK_SET_ATEL_UI_STATUS (QCRIL_REQ_HOOK_BASE + 26)
#define QCRIL_REQ_HOOK_SET_PREFERRED_NETWORK_BAND_PREF (QCRIL_REQ_HOOK_BASE + 37)
#define QCRIL_REQ_HOOK_GET_PREFERRED_NETWORK_BAND_PREF (QCRIL_REQ_HOOK_BASE + 38)
#define QCRIL_REQ_HOOK_GET_MAX_DATA_ALLOWED_REQ (QCRIL_REQ_HOOK_BASE + 93)
#define QCRIL_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ (QCRIL_REQ_HOOK_BASE + 94)
#define QCRIL_REQ_HOOK_REQ_GENERIC (QCRIL_REQ_HOOK_BASE + 100)
#define QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN (QCRIL_REQ_HOOK_BASE + 150)
#define QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF (QCRIL_REQ_HOOK_BASE + 151)
#define QCRIL_REQ_HOOK_GET_SAR_REV_KEY (QCRIL_REQ_HOOK_BASE + 200)
#define QCRIL_REQ_HOOK_SET_TRANSMIT_POWER (QCRIL_REQ_HOOK_BASE + 201)

#define QCRIL_REQ_HOOK_GET_SUB_PROVISION_PREFERENCE_REQ (QCRIL_REQ_HOOK_BASE + 90)
#define QCRIL_REQ_HOOK_SET_SUB_PROVISION_PREFERENCE_REQ (QCRIL_REQ_HOOK_BASE + 91)
#define QCRIL_REQ_HOOK_GET_SIM_ICCID_REQ (QCRIL_REQ_HOOK_BASE + 92)
#define QCRIL_REQ_HOOK_PERFORM_INCREMENTAL_NW_SCAN (QCRIL_REQ_HOOK_BASE + 18)
#define QCRIL_REQ_HOOK_CANCEL_QUERY_AVAILABLE_NETWORK (QCRIL_REQ_HOOK_BASE + 95)

#define QCRIL_REQ_HOOK_UNSOL_ENGINEER_MODE (QCRIL_REQ_HOOK_BASE + 1012)
#define QCRIL_REQ_HOOK_UNSOL_AUDIO_STATE_CHANGED (QCRIL_REQ_HOOK_BASE + 1015)
#define QCRIL_REQ_HOOK_UNSOL_CSG_ID_CHANGE_IND (QCRIL_REQ_HOOK_BASE + 1052)
#define QCRIL_REQ_HOOK_UNSOL_MAX_DATA_CHANGE_IND (QCRIL_REQ_HOOK_BASE + 1054)
#define QCRIL_REQ_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND (QCRIL_REQ_HOOK_BASE + 1011)
#define QCRIL_REQ_HOOK_UNSOL_SUB_PROVISION_STATUS (QCRIL_REQ_HOOK_BASE + 1028)

#define QCRIL_REQ_HOOK_UNSOL_SPEECH_CODEC_INFO (QCRIL_REQ_HOOK_BASE + 7001)

enum TunnelingServiceId {
  TUNNELING_SERVICE_NONE,
  TUNNELING_SERVICE_VT,
  TUNNELING_SERVICE_EMBMS,
  TUNNELING_SERVICE_PRESENCE,
  TUNNELING_SERVICE_QTUNER,
};

std::tuple<const char*, size_t> packOemhookRequest(uint32_t messageId, const uint8_t* reqData,
                                                   size_t reqDataSize);
std::tuple<uint32_t, const char*, size_t> unpackOemhookIndication(const char* data, size_t dataLen);

qmi_idl_service_object_type getServieObject(TunnelingServiceId serviceId);

std::tuple<RIL_Errno, void*, size_t> unpackGenericResponsePayload(const char* msgData,
                                                                  uint32_t msgDataLen);

std::tuple<const uint8_t*, size_t> packGenericRequestPayload(uint16_t serviceId, uint16_t messageId,
                                                             const void* src, uint32_t srcLen);

void printBuffer(const char* data, size_t dataLen);
#endif // OEMHOOKUTILS_HPP
