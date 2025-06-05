/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "modules/uim_remote/UimRemoteSrvcQcci.h"

class UimRmtClientRequestCallbackAidl : public GenericCallback<qmi_uim_remote_resp_msg> {
 public:
  inline UimRmtClientRequestCallbackAidl(string str) : GenericCallback(str)
  {
  }
  inline ~UimRmtClientRequestCallbackAidl()
  {
  }
  Message::Callback* clone() const;
  void onResponse(std::shared_ptr<Message> solicitedMsg,
                  Message::Callback::Status status,
                  std::shared_ptr<qmi_uim_remote_resp_msg> responseDataPtr);
  string mClientToken;
};