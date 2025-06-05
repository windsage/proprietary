/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/add_message_id.h>
#include <interfaces/QcRilRequestMessage.h>
#include <interfaces/ims/ims.h>

/*
 * Request to set the ims sub media config from lower layers
 *
 * Response:
 *   errorCode    : Valid error codes
 *                   SUCCESS
 *                   GENERIC_FAILURE
 *   responseData : none
 */

class QcRilRequestImsSetMediaInfoMessage
    : public QcRilRequestMessage,
      public add_message_id<QcRilRequestImsSetMediaInfoMessage> {
private:
  std::optional<int> mMaxAvcCodecWidth;
  std::optional<int> mMaxAvcCodecHeight;
  std::optional<int> mMaxHevcCodecWidth;
  std::optional<int> mMaxHevcCodecHeight;
  std::optional<int> mScreenWidth;
  std::optional<int> mScreenHeight;

public:
  static constexpr const char *MESSAGE_NAME = "QcRilRequestImsSetMediaInfoMessage";

  QcRilRequestImsSetMediaInfoMessage() = delete;

  ~QcRilRequestImsSetMediaInfoMessage() {};

  inline QcRilRequestImsSetMediaInfoMessage(std::shared_ptr<MessageContext> context)
      : QcRilRequestMessage(get_class_message_id(), context) {
    mName = MESSAGE_NAME;
  }

  bool hasMaxAvcCodecWidth() { return mMaxAvcCodecWidth ? true : false; }
  int getMaxAvcCodecWidth() { return *mMaxAvcCodecWidth; }
  void setMaxAvcCodecWidth(int val) { mMaxAvcCodecWidth = val; }
  inline string dumpMaxAvcCodecWidth() {
    std::ostringstream strs;
    if (mMaxAvcCodecWidth) {
      strs << *mMaxAvcCodecWidth;
    } else {
      strs << "<invalid>";
    }
    return " mMaxAvcCodecWidth = " + strs.str();
  }

  bool hasMaxAvcCodecHeight() { return mMaxAvcCodecHeight ? true : false; }
  int getMaxAvcCodecHeight() { return *mMaxAvcCodecHeight; }
  void setMaxAvcCodecHeight(int val) { mMaxAvcCodecHeight = val; }
  inline string dumpMaxAvcCodecHeight() {
    std::ostringstream strs;
    if (mMaxAvcCodecHeight) {
      strs << *mMaxAvcCodecHeight;
    } else {
      strs << "<invalid>";
    }
    return " mMaxAvcCodecHeight = " + strs.str();
  }
  bool hasMaxHevcCodecWidth() { return mMaxHevcCodecWidth ? true : false; }
  int getMaxHevcCodecWidth() { return *mMaxHevcCodecWidth; }
  void setMaxHevcCodecWidth(int val) { mMaxHevcCodecWidth = val; }
  inline string dumpMaxHevcCodecWidth() {
    std::ostringstream strs;
    if (mMaxHevcCodecWidth) {
      strs << *mMaxHevcCodecWidth;
    } else {
      strs << "<invalid>";
    }
    return " mMaxHevcCodecWidth = " + strs.str();
  }

  bool hasMaxHevcCodecHeight() { return mMaxHevcCodecHeight ? true : false; }
  int getMaxHevcCodecHeight() { return *mMaxHevcCodecHeight; }
  void setMaxHevcCodecHeight(int val) { mMaxHevcCodecHeight = val; }
  inline string dumpMaxHevcCodecHeight() {
    std::ostringstream strs;
    if (mMaxHevcCodecHeight) {
      strs << *mMaxHevcCodecHeight;
    } else {
      strs << "<invalid>";
    }
    return " mMaxHevcCodecHeight = " + strs.str();
  }

  bool hasScreenWidth() { return mScreenWidth ? true : false; }
  int getScreenWidth() { return *mScreenWidth; }
  void setScreenWidth(int val) { mScreenWidth = val; }
  inline string dumpScreenWidth() {
    std::ostringstream strs;
    if (mScreenHeight) {
      strs << *mScreenWidth;
    } else {
      strs << "<invalid>";
    }
    return " mScreenWidth = " + strs.str();
  }

  bool hasScreenHeight() { return mScreenHeight ? true : false; }
  int getScreenHeight() { return *mScreenHeight; }
  void setScreenHeight(int val) { mScreenHeight = val; }
  inline string dumpScreenHeight() {
    std::ostringstream strs;
    if (mScreenHeight) {
      strs << *mScreenHeight;
    } else {
      strs << "<invalid>";
    }
    return " mScreenHeight = " + strs.str();
  }

  virtual string dump() {
    return QcRilRequestMessage::dump() +
      "{" + dumpMaxAvcCodecWidth() + dumpMaxAvcCodecHeight() + dumpMaxHevcCodecWidth()
      + dumpMaxHevcCodecHeight() + dumpScreenWidth() + dumpScreenHeight() + "}";
  }
};
