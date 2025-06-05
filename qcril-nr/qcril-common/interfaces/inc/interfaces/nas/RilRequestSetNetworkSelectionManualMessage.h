/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/add_message_id.h"
#include "interfaces/QcRilRequestMessage.h"
#include <string>


/* Request to select network manually
   @Receiver: NasModule
   */

class RilRequestSetNetworkSelectionManualMessage : public QcRilRequestMessage,
                              public add_message_id<RilRequestSetNetworkSelectionManualMessage>
{

public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.set_network_selection_manual";
  RilRequestSetNetworkSelectionManualMessage() = delete;
  ~RilRequestSetNetworkSelectionManualMessage() {}

  template<typename T>
  explicit inline RilRequestSetNetworkSelectionManualMessage(std::shared_ptr<MessageContext> context,
        T operatorNumeric, RIL_RadioTechnology rat = RADIO_TECH_UNKNOWN) :
        QcRilRequestMessage(get_class_message_id(), context),
        mOperatorNumeric(std::forward<T>(operatorNumeric)), mRat(rat) {
    mName = MESSAGE_NAME;
  }

  const std::string& getOperatorNumeric() { return mOperatorNumeric; }

  RIL_RadioTechnology getRilRadioTechnology() { return mRat; }

  void setAccessMode(RIL_AccessMode accessMode){
    mAccessMode = accessMode;
  }

  bool hasAccessMode() {
    return mAccessMode ? true : false;
  }

  RIL_AccessMode getAccessMode() {
    return *mAccessMode;
  }

  void setCagId(uint32_t cagId) {
     mCagId = cagId;
  }

  bool hasCagId() {
    return mCagId ? true : false;
  }

  uint32_t getCagId() {
    return *mCagId;
  }

  void setSnpnId(std::vector<uint8_t> snpnNid) {
    mSnpnNid = std::move(snpnNid);
  }

  const std::vector<uint8_t> &getSnpnId() {
    return mSnpnNid;
  }

private:
  std::string mOperatorNumeric;
  RIL_RadioTechnology mRat;
  std::optional<RIL_AccessMode> mAccessMode;
  std::optional<uint32_t> mCagId;
  std::vector<uint8_t> mSnpnNid;
};

