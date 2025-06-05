/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <interfaces/nas/nas_types.h>

/*
 * Unsol message to notify PLMN change
 *
 */
class QcRilUnsolCurrentPlmnChangedMessage : public UnSolicitedMessage,
            public add_message_id<QcRilUnsolCurrentPlmnChangedMessage> {
 private:
  std::optional<qcril::interfaces::RilPlmnIdInfo> mRegPlmn;
  std::optional<nas_radio_if_enum_v01> mRegRat;

 public:
  static constexpr const char *MESSAGE_NAME =
        "com.qualcomm.qti.qcril.current_plmn_changed_message";
  ~QcRilUnsolCurrentPlmnChangedMessage() { }

  QcRilUnsolCurrentPlmnChangedMessage()
      : UnSolicitedMessage(get_class_message_id()) {
  }

  QcRilUnsolCurrentPlmnChangedMessage(uint16_t mcc, uint16_t mnc,
        uint8_t mnc_includes_pcs_digit)
        : UnSolicitedMessage(get_class_message_id()) {
    mRegPlmn = qcril::interfaces::RilPlmnIdInfo(mcc, mnc, mnc_includes_pcs_digit);
  }

  QcRilUnsolCurrentPlmnChangedMessage(qcril::interfaces::RilPlmnIdInfo plmn)
        : UnSolicitedMessage(get_class_message_id()) {
    mRegPlmn = plmn;
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    if (mRegPlmn) {
        return std::make_shared<QcRilUnsolCurrentPlmnChangedMessage>(*mRegPlmn);
    } else {
        return std::make_shared<QcRilUnsolCurrentPlmnChangedMessage>();
    }
  }

  void setRegisteredRat(nas_radio_if_enum_v01 rat) {
    mRegRat = rat;
  }

  const std::optional<qcril::interfaces::RilPlmnIdInfo>&
  getRegisteredPlmn() { return mRegPlmn; }

  const std::optional<nas_radio_if_enum_v01>&
  getRegisteredRat() { return mRegRat; };

  string dump() {
    return QcRilUnsolCurrentPlmnChangedMessage::MESSAGE_NAME +
        (mRegPlmn ? ("{ mcc: " + std::to_string(mRegPlmn->mcc) + " mnc: "
        + std::to_string(mRegPlmn->mnc) + " mnc_includes_pcs_digit: "
        + std::to_string(mRegPlmn->mnc_includes_pcs_digit) + " }") :
        "{ None }");
  }
};
