/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
 * Unsol message to report cell info list.
 */
class RilUnsolCellInfoListMessage : public UnSolicitedMessage,
                                    public add_message_id<RilUnsolCellInfoListMessage> {
 private:
  std::vector<RIL_CellInfo_v12> mCellInfoList;

 public:
  static constexpr const char* MESSAGE_NAME = "RIL_UNSOL_CELL_INFO_LIST";
  ~RilUnsolCellInfoListMessage() {
  }

  template <typename T>
  RilUnsolCellInfoListMessage(T cellInfos)
      : UnSolicitedMessage(get_class_message_id()), mCellInfoList(std::forward<T>(cellInfos)) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return std::make_shared<RilUnsolCellInfoListMessage>(mCellInfoList);
  }

  const std::vector<RIL_CellInfo_v12>& getCellInfoList() {
    return mCellInfoList;
  }

  string dump() {
    return RilUnsolCellInfoListMessage::MESSAGE_NAME;
  }
};
