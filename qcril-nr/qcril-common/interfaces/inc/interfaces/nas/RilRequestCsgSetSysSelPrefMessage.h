/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "telephony/ril.h"
#include "network_access_service_v01.h"

/**
 * Request to perform CSG network scan.
 * @Receiver: NasModule
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 **/
class RilRequestCsgSetSysSelPrefMessage : public QcRilRequestMessage,
                                          public add_message_id<RilRequestCsgSetSysSelPrefMessage> {
 private:
  bool mCsgInfoValid;
  nas_csg_nw_iden_type_v01 mCsgInfo;

 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestCsgSetSysSelPrefMessage";

  RilRequestCsgSetSysSelPrefMessage() = delete;
  ~RilRequestCsgSetSysSelPrefMessage() {
  }

  inline RilRequestCsgSetSysSelPrefMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

#define QCRIL_OEMHOOK_CSG_TLV_TYPE_SIZE 1
#define QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE 2

#define QCRIL_OEMHOOK_CSG_TAG_CSG_INFO 0x20

  RIL_Errno setData(const uint8_t* data, uint32_t dataLen) {
    if (!data) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    memset(&mCsgInfo, 0, sizeof(mCsgInfo));

    RIL_Errno result = RIL_E_SUCCESS;
    uint16_t len;
    for (uint32_t i = 0; i < dataLen;) {
      uint8_t type = *data;
      i += QCRIL_OEMHOOK_CSG_TLV_TYPE_SIZE;
      data += QCRIL_OEMHOOK_CSG_TLV_TYPE_SIZE;
      switch (type) {
        case QCRIL_OEMHOOK_CSG_TAG_CSG_INFO:
          result = RIL_E_INVALID_ARGUMENTS;
          if (i + QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE <= dataLen) {
            memcpy(&len, data, QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE);
            i += QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE;
            data += QCRIL_OEMHOOK_CSG_TLV_LEGTH_SIZE;
            if (len == 10) {
              mCsgInfoValid = true;
              memcpy(&mCsgInfo.mcc, data, 2);
              i += 2;
              data += 2;
              memcpy(&mCsgInfo.mnc, data, 2);
              i += 2;
              data += 2;
              memcpy(&mCsgInfo.mnc_includes_pcs_digit, data, 1);
              i += 1;
              data += 1;
              memcpy(&mCsgInfo.id, data, 4);
              i += 4;
              data += 4;
              memcpy(&mCsgInfo.rat, data, 1);
              i += 1;
              data += 1;
              result = RIL_E_SUCCESS;
            } else if (len == 0) {
              result = RIL_E_SUCCESS;
            }
          }
          break;
        default:
          result = RIL_E_INVALID_ARGUMENTS;
          break;
      }
      if (result != RIL_E_SUCCESS) {
        break;
      }
    }
    return result;
  }

  bool hasCsgInfo() {
    return mCsgInfoValid;
  }

  nas_csg_nw_iden_type_v01 getCsgInfo() {
    return mCsgInfo;
  }
};
