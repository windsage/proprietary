/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"

using namespace SM;
using namespace rildata;

void LatchingState::enter(void) {
}

void LatchingState::exit(void) {
}

int LatchingState::handleEvent(const CallEventType& event) {
  Log::getInstance().d("LatchingState::handleEvent="+std::to_string((int)event.type));
  switch (event.type) {
    case CallEventTypeEnum::CleanUp:
    {
      transitionTo(_eDisconnected);
      return IState<CallEventType>::HANDLED;
    }
    default:
      return IState<CallEventType>::UNHANDLED;
  }
}
