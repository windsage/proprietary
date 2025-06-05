/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include "interfaces/nas/nas_types.h"

/**
 * Unsol message to notify CSG ID change
 */
class RilUnsolEngineerModeMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolEngineerModeMessage> {
 public:
  static constexpr const char* MESSAGE_NAME = "RilUnsolEngineerModeMessage";

  RilUnsolEngineerModeMessage() : UnSolicitedMessage(get_class_message_id()) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    auto msg = std::make_shared<RilUnsolEngineerModeMessage>();
    if (msg) {
      msg->mCdmaFtmDataValid = mCdmaFtmDataValid;
      msg->mCdmaFtmData = mCdmaFtmData;
      msg->mGsmFtmDataValid = mGsmFtmDataValid;
      msg->mGsmFtmData = mGsmFtmData;
      msg->mWcdmaFtmDataValid = mWcdmaFtmDataValid;
      msg->mWcdmaFtmData = mWcdmaFtmData;
    }
    return msg;
  }

  string dump() {
    return RilUnsolEngineerModeMessage::MESSAGE_NAME;
  }

  void setCdmaFtmData(bool valid, cdma_ftm_data data) {
    mCdmaFtmDataValid = valid;
    mCdmaFtmData = data;
  }
  bool isCdmaFtmDataValid() {
    return mCdmaFtmDataValid;
  }
  cdma_ftm_data &getCdmaFtmData() {
    return mCdmaFtmData;
  }
  void setGsmFtmData(bool valid, gsm_ftm_data data) {
    mGsmFtmDataValid = valid;
    mGsmFtmData = data;
  }
  bool isGsmFtmDataValid() {
    return mGsmFtmDataValid;
  }
  gsm_ftm_data &getGsmFtmData() {
    return mGsmFtmData;
  }
  void setWcdmaFtmData(bool valid, wcdma_ftm_data data) {
    mWcdmaFtmDataValid = valid;
    mWcdmaFtmData = data;
  }
  bool isWcdmaFtmDataValid() {
    return mWcdmaFtmDataValid;
  }
  wcdma_ftm_data &getWcdmaFtmData() {
    return mWcdmaFtmData;
  }

 private:
  bool mCdmaFtmDataValid;
  cdma_ftm_data mCdmaFtmData;
  bool mGsmFtmDataValid;
  gsm_ftm_data mGsmFtmData;
  bool mWcdmaFtmDataValid;
  wcdma_ftm_data mWcdmaFtmData;
};
