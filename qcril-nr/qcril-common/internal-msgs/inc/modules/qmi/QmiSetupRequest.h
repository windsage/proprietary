/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"
#include "framework/legacy.h"

class QmiSetupRequest : public SolicitedMessage<string>,
                        public add_message_id<QmiSetupRequest>
{
 public:
  static constexpr const char * MESSAGE_NAME = "QMI_CLIENT_SETUP_REQ";
  inline QmiSetupRequest(string clientToken, size_t dataSize, void *dataPtr,
                         qcril_instance_id_e_type id,
                         GenericCallback<string> *callback):
      SolicitedMessage<string>(get_class_message_id()),
      mToken(clientToken),
      mId(id)
  {
    (void)dataSize;
    (void)dataPtr;
    setCallback(callback);
  }

  inline QmiSetupRequest(string clientToken, size_t dataSize, void *dataPtr,
                         GenericCallback<string> *callback):
      SolicitedMessage<string>(get_class_message_id()),
      mToken(clientToken),
      mId(QCRIL_DEFAULT_INSTANCE_ID)
  {
    (void)dataSize;
    (void)dataPtr;
    setCallback(callback);
  }

  qcril_instance_id_e_type getInstanceId() { return mId; }

  ~QmiSetupRequest();

  string dump();

 private:
  string mToken;
  qcril_instance_id_e_type mId;
};
