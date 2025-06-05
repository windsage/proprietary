/******************************************************************************
#  Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"

using namespace std;
using namespace SM;
using namespace rildata;

void DisconnectedState::enter(void) {
  for (auto& psp : callInfo.pduSessionParams) {
    if (psp.pendingAck) {
      sendSetApnPreferredSystemRequest(callInfo.dsd_endpoint, psp.dnnName, callInfo.preferredRAT);
      psp.pendingAck = false;
    }
  }
  cleanupKeepAlive(callInfo.cid);
}

void DisconnectedState::exit(void) {
}

int DisconnectedState::handleEvent(const CallEventType &) {
  return IState<CallEventType>::HANDLED;
}
