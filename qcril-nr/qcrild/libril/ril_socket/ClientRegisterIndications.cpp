/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#undef TAG
#define TAG "RILQ"
#include <ril_socket_api.h>
#include <Marshal.h>
#include <framework/Log.h>
#include <telephony/ril.h>
#include "RilSocketClientController.h"
#include "ril_socket.h"

#ifndef RIL_FOR_MDM_LE
#include <utils/Log.h>
#else
#include <utils/Log2.h>
#endif

namespace ril {
namespace socket {
namespace api {
void dispatchRegisterIndicationsRequest(std::shared_ptr<SocketRequestContext> context, Marshal& p) {
  RIL_Errno errorCode = RIL_Errno::RIL_E_SUCCESS;
  int32_t indicationId =  0;
  p.read(indicationId);
  QCRIL_LOG_DEBUG("Unsol indication id to register is %d ", indicationId);
  errorCode = RilSocketClientController::getInstance().registerClientForIndications(context->getClientFd(), indicationId);

  //sendResponse
  sendResponse(context, errorCode, nullptr);

  //Send RIL_UNSOL_RIL_CONNECTED to clients after registering
  if (errorCode == RIL_E_SUCCESS && indicationId == RIL_UNSOL_RIL_CONNECTED) {
    notifyClientServerIsReady(context->getClientFd());
  }
}
}
}
}

