/*
 * Copyright (c) 2019-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <framework/TimeKeeper.h>
#include "IState.h"
#include "MessageCommon.h"
#include "DataCommon.h"
#include "voice_service_v02.h"
#include  "voice_service_common_v02.h"
#include "data_system_determination_v01.h"
#include "UnSolMessages/VoiceIndMessage.h"
#include "event/DdsSwitchRecommendInternalMessage.h"
#include <list>

#define INVALID_SUB_ID -1

namespace rildata {

using SM::IState;
class PreferredDataMessageList;

enum PreferredDataStateEnum : int {
    Initial = 0,
    Initialized,
    DefaultData,
    NonDefaultData,
    Switching,
    ApTriggered,
    MpTriggered,
    IAHandling,
};

enum PreferredDataEventEnum : int {
    SetPreferredDataModem = 0,
    DDSSwitchInd,
    CurrentDDSInd,
    DDSSwitchTimerExpired,
    DDSSwitchApStarted,
    DDSSwitchCompleted,
    DDSSwitchMpStarted,
    TempDDSSwitchTimerExpired,
    IAStarted,
    IACompletedOnMasterRIL,
    IACompletedOnSlaveRIL,
    IAStartRequest,
    DDSSwitchInternal,
    ServiceDown,
    IATimerExpired,
    GetCurrentStateRequest,
    GetCurrentStateResponse,
    setIsDataAllowedOnNDDS,
    ExitState
};

enum DDSSwitchTypeEnum : int {
     Permenant = 0,
     Temp,
};

struct PreferredDataEventType {
    PreferredDataEventEnum event;
    string getEventName() const;
    void *data;
};

enum class DDSSwitchRes : int {
   ERROR = -1,
   FAIL = 0,
   ALLOWED = 1,
};

struct DDSSwitchInfo_t {
    int subId;
    shared_ptr<Message> msg;
    dsd_dds_switch_type_enum_v01 switch_type;
    DDSSwitchRes switchAllowed;
    DdsSwitchLevel_t tempSwitchLevel;
    bool isTempDDS;
    int permanentDDS;
    PreferredDataStateEnum slaveRILState;
    bool isDataAllowedOnNDDS;
};

struct PreferredDataInfo_t {
    int dds;
    dsd_dds_switch_type_enum_v01 currentDDSSwitchType; //This is the actual dds switchtype at modem
    bool isRilIpcNotifier;
    VoiceIndResp mVoiceCallInfo;
    dsd_dds_switch_type_enum_v01  switch_type; //Based on the voicecall info, we set this value and send it with qmi dds query
    shared_ptr<Message> mPendingMessage;
    std::shared_ptr<PreferredDataMessageList> mPreferredDataMsgList;
    std::optional<DDSSwitchInfo_t> tempDdsSwitchRecommendInfo;
    bool tempDdsSwitched; //This is set in case of recommendation based temp dds
    int permanentDDSSub;  //This sets the old dds only when current dds is changed with recommendation
    TimeKeeper::timer_id iaTimer;
    bool isDataAllowedOnNDDS;
};

enum PendingEvents : int {
  ddsRequest,
  IAOnMasterRIL,
  IAOnSlaveRIL
};

struct PendingMessage {
  PendingEvents event;
  std::shared_ptr<Message> msg;
};

class PreferredDataMessageList {
  private:
    std::list<std::shared_ptr<PendingMessage>> mRequestList;
  public:
    void insertMessage(std::shared_ptr<PendingMessage> msg);
    std::shared_ptr<PendingMessage> getNextMessageFromList();
    void removeMessage(std::shared_ptr<PendingMessage> msg);
    void sendFailureForOldRequest(PendingEvents event);
    int noOfRequestsForEvent(PendingEvents event);
    bool isEmpty();
    void clear();
};

class PreferredDataState : public IState<PreferredDataEventType> {
public:
    PreferredDataState(string name, function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    ~PreferredDataState();
    virtual void enter() {};
    virtual void exit() {};
    virtual int handleEvent(const PreferredDataEventType&) = 0;
    void ddsTransition();
    void sendResponse(std::shared_ptr<Message> m, SetPreferredDataModemResult_t error);
    void sendResponse(std::shared_ptr<PendingMessage> m, SetPreferredDataModemResult_t error);
    void onDDSSwitchRequestExpired(void *);
    void iaTimerExpired(void *);
    void sendRecommendationToFramework(DDSSwitchInfo_t *switchInfo);
    void disconnectCallFromOldDDSSub();
    TimeKeeper::timer_id tempDDSSwitchRequestTimer;
    bool TempddsSwitchRequestPending = false;

protected:
    static constexpr TimeKeeper::millisec DDS_SWITCH_REQUEST_TIMEOUT = 17000;
    #ifdef QMI_RIL_UTF
    static constexpr TimeKeeper::millisec IA_ON_SLAVE_REQUEST_TIMEOUT = 2000;
    #else
    static constexpr TimeKeeper::millisec IA_ON_SLAVE_REQUEST_TIMEOUT = 500;
    static constexpr TimeKeeper::millisec IA_ON_MASTER_REQUEST_TIMEOUT = 1000;
    #endif
    shared_ptr<PreferredDataInfo_t> mStateInfo;
    TimeKeeper::timer_id ddsSwitchRequestTimer;
};

class InitialState : public PreferredDataState {
public:
    InitialState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    int handleEvent(const PreferredDataEventType& event) override;
};

class InitializedState : public PreferredDataState {
public:
    InitializedState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    int handleEvent(const PreferredDataEventType& event) override;
};

class SwitchingState : public PreferredDataState {
public:
    SwitchingState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    int handleEvent(const PreferredDataEventType& event) override;
};

class DefaultDataState : public PreferredDataState {
public:
    DefaultDataState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    int handleEvent(const PreferredDataEventType& event) override;
};

class NonDefaultDataState : public PreferredDataState {
public:
    NonDefaultDataState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    int handleEvent(const PreferredDataEventType& event) override;
};

class ApTriggeredState : public PreferredDataState {
public:
    ApTriggeredState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    void enter() override;
    void exit() override;
    int handleEvent(const PreferredDataEventType& event) override;
};

class MpTriggeredState : public PreferredDataState {
public:
    MpTriggeredState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    void enter() override;
    int handleEvent(const PreferredDataEventType& event) override;
};

class IAHandlingState : public PreferredDataState {
private:
    PreferredDataStateEnum nextState;
public:
    IAHandlingState(function<void(int)> fn, shared_ptr<PreferredDataInfo_t> info);
    void enter() override;
    int handleEvent(const PreferredDataEventType& event) override;
};

}
