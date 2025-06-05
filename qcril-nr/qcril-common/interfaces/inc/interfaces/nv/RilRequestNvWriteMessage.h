/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __RILREQUESTNVWRITEMESSAGE_H_
#define __RILREQUESTNVWRITEMESSAGE_H_

#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nv/nv_items.h"
#include "qcril_legacy_apis.h"

#define QMI_RIL_SPC_TLV_NVWRITE "persist.vendor.radio.spc_tlv_nvwrite"
#define SPC_MAX_LEN 6

/**
 * Request to write NV item.
 * @Receiver: common
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : nullptr
 */
class RilRequestNvWriteMessage : public QcRilRequestMessage,
                                 public add_message_id<RilRequestNvWriteMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestNvWriteMessage";

  RilRequestNvWriteMessage() = delete;

  inline RilRequestNvWriteMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  nv_items_enum_type getNvItemId() const {
    return mNvItemId;
  }

  nv_item_type getNvItem() const {
    return mNvItem;
  }

  bool hasSpc() const {
    return mSpcValid;
  }

  char* getSpc() const {
    return (char*)mSpc;
  }

  RIL_Errno setData(const uint8_t* data, uint32_t dataLen) {
    if (!data) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    if (dataLen < QCRIL_OTHER_OEM_ITEMID_LEN) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    uint32_t nvItemId = 0;
    memcpy(&nvItemId, data, QCRIL_OTHER_OEM_ITEMID_LEN);
    mNvItemId = static_cast<nv_items_enum_type>(nvItemId);
    if (!isValidNvItem(mNvItemId)) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    dataLen -= QCRIL_OTHER_OEM_ITEMID_LEN;
    data += QCRIL_OTHER_OEM_ITEMID_LEN;

    if (dataLen < QCRIL_OTHER_OEM_ITEMID_DATA_LEN) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    uint32_t nvItemLen;
    memcpy(&nvItemLen, data, QCRIL_OTHER_OEM_ITEMID_DATA_LEN);
    dataLen -= QCRIL_OTHER_OEM_ITEMID_DATA_LEN;
    data += QCRIL_OTHER_OEM_ITEMID_DATA_LEN;

    if (!isValidNvItemLen(mNvItemId, nvItemLen)) {
      return RIL_E_INVALID_ARGUMENTS;
    }

    if (dataLen < nvItemLen) {
      return RIL_E_INVALID_ARGUMENTS;
    }
    memcpy((void*)((char*)(&mNvItem)), data, nvItemLen);
    dataLen -= nvItemLen;
    data += nvItemLen;

    int spc_tlv_valid = FALSE;
    qmi_ril_get_property_value_from_integer(QMI_RIL_SPC_TLV_NVWRITE, &spc_tlv_valid, FALSE);

    if (spc_tlv_valid) {
      if ((dataLen > SPC_MAX_LEN) || dataLen < 1) {
        return RIL_E_GENERIC_FAILURE;
      }
      mSpcValid = true;
      memcpy(mSpc, data, dataLen);
    }
    return RIL_E_SUCCESS;
  }

 private:
  nv_items_enum_type mNvItemId;
  nv_item_type mNvItem;
  bool mSpcValid = false;
  char mSpc[SPC_MAX_LEN];
};
#endif  //__RILREQUESTNVWRITEMESSAGE_H_
