/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include <interfaces/QcRilRequestMessage.h>

/* Request to send a IMS SMS Message (including GSM and CDMA)
   @Receiver: SmsModule

   Response:
     errorCode     : Valid error codes
     responseData  : std::shared_ptr<RilSendSmsResult_t>
*/
class RilRequestImsSendSmsMessage : public QcRilRequestMessage,
                                    public add_message_id<RilRequestImsSendSmsMessage> {
public:
  static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_IMS_SEND_SMS";
  RilRequestImsSendSmsMessage() = delete;
  ~RilRequestImsSendSmsMessage() {}

  explicit RilRequestImsSendSmsMessage(std::shared_ptr<MessageContext> context, uint32_t msgRef,
                                       RIL_RadioTechnologyFamily tech, bool retry)
      : QcRilRequestMessage(get_class_message_id(), context), mRef(msgRef),
        mRetry(retry), mTech(tech) {}

  template <typename T1, typename T2> inline void setGsmPayload(T1 &&smscPdu, T2 &&pdu) {
    if (mTech == RADIO_TECH_3GPP) {
      mGsmSmscPdu = std::forward<T1>(smscPdu);
      mGsmPdu = std::forward<T2>(pdu);
    }
  }
  void setCdmaPayload(const RIL_CDMA_SMS_Message &payload) {
    if (mTech == RADIO_TECH_3GPP2) {
        mCdmaPayload = payload;
    }
  };

  RIL_CDMA_SMS_Message &getCdmaPayload() {
    return mCdmaPayload;
  };
  const string &getGsmSmscPdu() {
    return mGsmSmscPdu;
  };
  const string &getGsmPdu() {
    return mGsmPdu;
  };

  bool isRetry() {
    return mRetry;
  };
  uint32_t getMessageReference() {
    return mRef;
  };
  RIL_RadioTechnologyFamily getRadioTechFamily() {
    return mTech;
  };

  string dump() {
    return mName + std::string(" { ref = ") + std::to_string(mRef) +
      std::string(" tech = ") + std::string((mTech == RADIO_TECH_3GPP) ? "3GPP" : "3GPP2") +
      std::string(" retry = ") + std::string(mRetry ? "true" : "false") + std::string(" }");
  };

private:
  const uint32_t mRef;
  const bool mRetry;
  const RIL_RadioTechnologyFamily mTech;
  string mGsmSmscPdu;
  string mGsmPdu;
  RIL_CDMA_SMS_Message mCdmaPayload;
};
