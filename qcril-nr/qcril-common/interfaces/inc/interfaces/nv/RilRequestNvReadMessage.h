/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef __RILREQUESTNVREADMESSAGE_H_
#define __RILREQUESTNVREADMESSAGE_H_

#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include "interfaces/nv/nv_items.h"
#include "qcril_qmi_oemhook_utils.h"

namespace qcril {
namespace interfaces {

struct NvReadResult_t : public qcril::interfaces::BasePayload {
  nv_items_enum_type nvItemId;
  nv_item_type nvItemValue;
  size_t nvItemValueSize;
};

}  // namespace interfaces
}  // namespace qcril

/**
 * Request to read NV item.
 * @Receiver: common
 *
 * Response:
 *   errorCode    : Valid error codes
 *   responseData : std::shared_ptr<qcril::interfaces::NvReadResult_t>
 */
class RilRequestNvReadMessage : public QcRilRequestMessage,
                                public add_message_id<RilRequestNvReadMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilRequestNvReadMessage";

  RilRequestNvReadMessage() = delete;

  inline RilRequestNvReadMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  nv_items_enum_type getNvItemId() const {
    return mNvItemId;
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
    return RIL_E_SUCCESS;
  }

 private:
  nv_items_enum_type mNvItemId;
};

#endif  //__RILREQUESTNVREADMESSAGE_H_
