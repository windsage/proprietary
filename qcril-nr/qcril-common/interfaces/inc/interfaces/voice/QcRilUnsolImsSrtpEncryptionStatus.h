/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/*
 * Unsol message to notify HLOS which SRTP encryption category is supported.
 * This Unsol is sent when modem is configured
 *
 */
class QcRilUnsolImsSrtpEncryptionStatus : public UnSolicitedMessage,
                                          public add_message_id<QcRilUnsolImsSrtpEncryptionStatus> {
 private:
  uint8_t mCallId;
  bool mVoiceEnable;
  bool mVideoEnable;
  bool mTextEnable;

 public:
  static constexpr const char* MESSAGE_NAME = "QcRilUnsolImsSrtpEncryptionStatus";
  ~QcRilUnsolImsSrtpEncryptionStatus()
  {
  }

  QcRilUnsolImsSrtpEncryptionStatus() = delete;

  QcRilUnsolImsSrtpEncryptionStatus(const uint8_t callId,
                                    const bool voice,
                                    const bool video,
                                    const bool text)
      : UnSolicitedMessage(get_class_message_id()),
        mCallId(callId),
        mVoiceEnable(voice),
        mVideoEnable(video),
        mTextEnable(text)
  {
  }

  std::shared_ptr<UnSolicitedMessage> clone()
  {
    auto msg = std::make_shared<QcRilUnsolImsSrtpEncryptionStatus>(
        mCallId, mVoiceEnable, mVideoEnable, mTextEnable);
    return msg;
  }

  uint8_t getCallId() const
  {
    return mCallId;
  }

  bool getVoiceEnable() const
  {
    return mVoiceEnable;
  }

  bool getVideoEnable() const
  {
    return mVideoEnable;
  }

  bool getTextEnable() const
  {
    return mTextEnable;
  }

  string dumpCallId()
  {
    return std::string(" mCallId = ") + std::to_string(mCallId);
  }

  string dumpVoiceEnable()
  {
    return std::string(" mVoiceEnable = ") + (mVoiceEnable ? "true" : "false");
  }

  string dumpVideoEnable()
  {
    return std::string(" mVideoEnable = ") + (mVideoEnable ? "true" : "false");
  }

  string dumpTextEnable()
  {
    return std::string(" mTextEnable = ") + (mTextEnable ? "true" : "false");
  }

  virtual string dump()
  {
    std::string os;
    os += QcRilUnsolImsSrtpEncryptionStatus::MESSAGE_NAME;
    os += "{";
    os += dumpCallId();
    os += dumpVoiceEnable();
    os += dumpVideoEnable();
    os += dumpTextEnable();
    os += "}";
    return os;
  }
};
