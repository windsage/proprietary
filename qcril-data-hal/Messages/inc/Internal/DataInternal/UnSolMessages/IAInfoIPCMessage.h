/*
 * Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "framework/Message.h"
#include "qtibus/IPCMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

enum class IAInfoIPCMessageState : int {
    /*This is sent by Slave RIL to inform IA completion to Master RIL*/
    IACompletedOnSlaveRIL = 0,
    /*This is sent by slave RIL to inform Master RIl about IA on slave RIL*/
    IAStartRequest,
    /* This is sent by Master RIL as an ack of IAStartRequest.
       On receiving IAStartResponse, SlaveRIL starts processing IA. */
    IAStartResponse,
    /* This is sent by Master RIL during Master RIL's initialization
       to know about the slave RIL's current state.This is specifically
       for the cases where master RIL is restarted*/
    GetCurrentStateRequest,
     /* This is sent by slave RIL as a response of GetCurrentStateRequest*/
    GetCurrentStateResponse
};

enum IAInfoPreferredDataStates: int {
    InitialState = 0,
    InitializedState,
    ApTriggeredState,
    MpTriggeredState,
    IAHandlingState,
    UNKNOWN,
};

/********************** Class Definitions *************************/
class IAInfoIPCMessage: public IPCMessage,
                           public add_message_id<IAInfoIPCMessage> {

private:
  IAInfoIPCMessageState state;
  IAInfoPreferredDataStates currentPreferredDataState;

public:
  static constexpr const char *MESSAGE_NAME = "IAInfoIPCMessage";

  IAInfoIPCMessage(IAInfoIPCMessageState setState);
  ~IAInfoIPCMessage();

  void serialize(IPCOStream &os) override;
  void deserialize(IPCIStream &is) override;

  IAInfoIPCMessageState getState();

  void setPreferredDataState(IAInfoPreferredDataStates e);
  IAInfoPreferredDataStates getPreferredDataState();

  std::shared_ptr<UnSolicitedMessage> clone();
  string dump();
};

} //namespace
