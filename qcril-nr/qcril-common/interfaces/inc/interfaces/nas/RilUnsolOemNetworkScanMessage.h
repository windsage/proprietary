/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * Unsol message to report network scan.
 */
class RilUnsolOemNetworkScanMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolOemNetworkScanMessage> {
 private:
  std::vector<RIL_CellInfo_v12> mCellInfoList;
  char *mData;
  uint16_t mDataLen;

 public:
  static constexpr const char* MESSAGE_NAME = "QCRIL_REQ_HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND";
  ~RilUnsolOemNetworkScanMessage() {
  }

  RilUnsolOemNetworkScanMessage(char *data, uint16_t data_len)
      : UnSolicitedMessage(get_class_message_id()), mData(data), mDataLen(data_len) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolOemNetworkScanMessage>(mData, mDataLen);
  }

  const char *getData() {
    return mData;
  }

  uint16_t getDataLen() {
    return mDataLen;
  }

  string dump() {
    return RilUnsolOemNetworkScanMessage::MESSAGE_NAME;
  }
};
