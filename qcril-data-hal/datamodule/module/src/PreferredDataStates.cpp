/*
 * Copyright (c) 2019-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "modules/qmi/ModemEndPointFactory.h"
#include "DSDModemEndPoint.h"
#include "PreferredDataStates.h"
#include "UnSolMessages/DDSSwitchIPCMessage.h"
#include "UnSolMessages/IAInfoIPCMessage.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "request/SetPreferredDataModemRequestMessage.h"
#include "UnSolMessages/ProcessInitialAttachRequestMessage.h"
#include "UnSolMessages/NewDDSInfoMessage.h"
#include "UnSolMessages/VoiceIndMessage.h"
#include "UnSolMessages/DDSSwitchTimeoutMessage.h"
#include "UnSolMessages/DdsSwitchRecommendationIndMessage.h"
#include "UnSolMessages/IATimeoutMessage.h"
#include "UnSolMessages/InformDDSSUBChangedMessage.h"
#include "UnSolMessages/ExitFromCurrentStateRequestMessage.h"
#include "qcril_data.h"
#include "DataCommon.h"

#include "SMLog.h"

using namespace SM;
using namespace rildata;
using std::placeholders::_1;

PreferredDataState::PreferredDataState(
    string name,
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> stateInfo
): IState(name, fn), mStateInfo(stateInfo), ddsSwitchRequestTimer(TimeKeeper::no_timer)
{
    mStateInfo->mPreferredDataMsgList = std::make_shared<PreferredDataMessageList>();
    mStateInfo->mPreferredDataMsgList->clear();
}

PreferredDataState::~PreferredDataState()
{
    mStateInfo->mPreferredDataMsgList->clear();
}

InitialState::InitialState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("Initial", fn, info)
{}

InitializedState::InitializedState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("Initialized", fn, info)
{}

SwitchingState::SwitchingState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("Switching", fn, info)
{}

DefaultDataState::DefaultDataState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("DefaultData", fn, info)
{}

NonDefaultDataState::NonDefaultDataState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("NonDefaultData", fn, info)
{}

ApTriggeredState::ApTriggeredState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("ApTriggered", fn, info)
{}

MpTriggeredState::MpTriggeredState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("MpTriggered", fn, info)
{}

IAHandlingState::IAHandlingState(
    function<void(int)> fn,
    shared_ptr<PreferredDataInfo_t> info
) : PreferredDataState("IAHandling", fn, info)
{
    nextState = PreferredDataStateEnum::Initialized;
}

string PreferredDataEventType::getEventName() const {
    switch (event) {
        case PreferredDataEventEnum::SetPreferredDataModem:
            return "SetPreferredDataModem";
        case PreferredDataEventEnum::DDSSwitchInd:
            return "DDSSwitchInd";
        case PreferredDataEventEnum::CurrentDDSInd:
            return "CurrentDDSInd";
        case PreferredDataEventEnum::DDSSwitchTimerExpired:
            return "DDSSwitchTimerExpired";
        case PreferredDataEventEnum::DDSSwitchApStarted:
            return "DDSSwitchApStarted";
        case PreferredDataEventEnum::DDSSwitchCompleted:
            return "DDSSwitchCompleted";
        case PreferredDataEventEnum::DDSSwitchMpStarted:
            return "DDSSwitchMpStarted";
        case PreferredDataEventEnum::TempDDSSwitchTimerExpired:
            return "TempDDSSwitchTimerExpired";
        case PreferredDataEventEnum::IAStarted:
            return "IAStarted";
        case PreferredDataEventEnum::IACompletedOnMasterRIL:
            return "IACompletedOnMasterRIL";
        case PreferredDataEventEnum::IACompletedOnSlaveRIL:
            return "IACompletedOnSlaveRIL";
        case PreferredDataEventEnum::IAStartRequest:
            return "IAStartRequest";
        case PreferredDataEventEnum::DDSSwitchInternal:
            return "DDSSwitchInternal";
        case PreferredDataEventEnum::ServiceDown:
            return "ServiceDown";
        case PreferredDataEventEnum::IATimerExpired:
            return "IATimerExpired";
        case PreferredDataEventEnum::GetCurrentStateRequest:
            return "GetCurrentStateRequest";
        case PreferredDataEventEnum::GetCurrentStateResponse:
            return "GetCurrentStateResponse";
        case PreferredDataEventEnum::setIsDataAllowedOnNDDS:
            return "setIsDataAllowedOnNDDS";
        case PreferredDataEventEnum::ExitState:
            return "ExitState";
        default:
            return "UNKNOWN";
    }
}

void PreferredDataState::ddsTransition() {
    int nextState = (mStateInfo->dds == global_instance_id) ? DefaultData : NonDefaultData;
    Log::getInstance().d("ddsTransition() next state=" + std::to_string(nextState));
    transitionTo(nextState);
}

void PreferredDataState::sendResponse(std::shared_ptr<PendingMessage> msg,
                                      SetPreferredDataModemResult_t error) {
    if (msg == nullptr)
        return;
    if (msg->event != PendingEvents::ddsRequest) {
        SM_LOGD("Pending msg is not SetPreferredDataModem");
        return;
    }
    sendResponse(msg->msg, error);
}

void PreferredDataState::sendResponse(std::shared_ptr<Message> msg,
                                      SetPreferredDataModemResult_t error) {
    SetPreferredDataModemResponse_t response;
    auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(msg);
    response.errCode = error;
    if (m != nullptr) {
        SM_LOGD("responding to SetPreferredDataModem with error=" +
                             std::to_string(static_cast<int>(error)));
        auto r = make_shared<SetPreferredDataModemResponse_t>(response);
        if (error == SetPreferredDataModemResult_t::NO_ERROR) {
            m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
        } else {
            m->sendResponse(m, Message::Callback::Status::FAILURE, r);
        }
    }
    PendingMessage ddsMsg = {ddsRequest, msg};
    mStateInfo->mPreferredDataMsgList->removeMessage(std::make_shared<PendingMessage>(ddsMsg));
}

void PreferredDataState::onDDSSwitchRequestExpired(void *)
{
    Log::getInstance().d("PreferredDataState::onDDSSwitchRequestExpired ENTRY");
    DDSTimeOutSwitchType type = DDSTimeOutSwitchType::DDSTimeOutSwitch;
    auto msg = std::make_shared<DDSSwitchTimeoutMessage>(type);
    msg->broadcast();
}

void PreferredDataState::iaTimerExpired(void *) {
    Log::getInstance().d("PreferredDataState::iaTimerExpired ENTRY");
    auto msg = std::make_shared<IATimeoutMessage>();
    msg->broadcast();
}

void PreferredDataState::sendRecommendationToFramework(DDSSwitchInfo_t *switchInfo) {
    if ((switchInfo->switch_type != DSD_DDS_SWITCH_PERMANENT_V01 || switchInfo->subId != mStateInfo->dds) &&
                  ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint()->getUserDdsSwitchSelection()) {
        auto recInd = std::make_shared<DdsSwitchRecommendationIndMessage>((SubscriptionId_t)switchInfo->subId);
        recInd->broadcast();
    }
}

void PreferredDataState::disconnectCallFromOldDDSSub()
{
    Log::getInstance().d("PreferredDataState::disconnectCallFromOldDDSSub ENTRY");
    if(!mStateInfo->isDataAllowedOnNDDS) {
        auto msg = std::make_shared<InformDDSSUBChangedMessage>();
        msg->broadcast();
    }
}

int InitialState::handleEvent(const PreferredDataEventType& e) {
    SM_LOGD(getName() + ": handling " + e.getEventName());
    switch (e.event) {
        case SetPreferredDataModem:
        {
            DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
            SetPreferredDataModemResponse_t resp;
            resp.errCode = SetPreferredDataModemResult_t::INVALID_OPERATION;
            auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
            if (r != nullptr) {
                SM_LOGE("responding to SetPreferredDataModem with error=" +
                                    std::to_string(static_cast<int>(r->errCode)));
                m->sendResponse(m, Message::Callback::Status::FAILURE, r);
            }
            return HANDLED;
        }
        case IAStarted:
        {
            DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
            if(switchInfo != nullptr)
            {
                PendingMessage iaMsg;
                #ifdef QMI_RIL_UTF
                usleep(800);
                #endif
                if(mStateInfo->isRilIpcNotifier) {
                    iaMsg = {IAOnMasterRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
                } else {
                    iaMsg = {IAOnSlaveRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
                }
                mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));
                transitionTo(IAHandling);
            }
            return HANDLED;
        }
        case IAStartRequest:
        {
            #ifndef QMI_RIL_UTF
            //For UTF, don't save IA request message of Slave RIL in master RIL's pendingMessageList
            PendingMessage iaMsg = {IAOnSlaveRIL, nullptr};
            mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));
            #endif
            transitionTo(IAHandling);
            return HANDLED;
        }
        case GetCurrentStateRequest:
        {
            //slave RIL responds with its current state to Master RIL
            if(!mStateInfo->isRilIpcNotifier) {
                auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::GetCurrentStateResponse);
                msg->setPreferredDataState(IAInfoPreferredDataStates::InitialState);
                msg->broadcast();
            }
            return HANDLED;
        }
        case CurrentDDSInd:
        {
            DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
            if(switchInfo != nullptr) {
                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
                ddsTransition();
            }
            return HANDLED;
        }
        case setIsDataAllowedOnNDDS:
        {
            DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
            if(switchInfo != nullptr) {
                mStateInfo->isDataAllowedOnNDDS = switchInfo->isDataAllowedOnNDDS;
                if(mStateInfo->isRilIpcNotifier) {
                    auto ipcMsg = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::SetDataAllowedOnNDDS, mStateInfo->dds,
                        mStateInfo->currentDDSSwitchType, mStateInfo->tempDdsSwitched, mStateInfo->permanentDDSSub, mStateInfo->isDataAllowedOnNDDS);
                    ipcMsg->broadcast();

                }
            }
            return HANDLED;
        }
        default:
            break;
    }
    return HANDLED;
}

int InitializedState::handleEvent(const PreferredDataEventType& e) {
    DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
    SM_LOGD(getName() + ": handling " + e.getEventName());
    switch (e.event) {
        case SetPreferredDataModem:
        {
            // save request msg so we can respond after CURRENT_DDS_IND arrives
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);

            if (mStateInfo->tempDdsSwitchRecommendInfo.has_value() &&
                mStateInfo->tempDdsSwitchRecommendInfo.value().subId != switchInfo->subId &&
                mStateInfo->currentDDSSwitchType == DSD_DDS_SWITCH_PERMANENT_V01 &&
                mStateInfo->tempDdsSwitchRecommendInfo.value().tempSwitchLevel == DdsSwitchLevel_t::Revoke) {
                Log::getInstance().d("[InitializedState]: dds recommend revoke to subId="+
                                      std::to_string(mStateInfo->tempDdsSwitchRecommendInfo.value().subId)+
                                      " dds request subId="+std::to_string(switchInfo->subId));
                SetPreferredDataModemResponse_t resp;
                resp.errCode = SetPreferredDataModemResult_t::DDS_SWITCH_NOT_ALLOWED;
                auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
                mStateInfo->tempDdsSwitchRecommendInfo.reset();
                return HANDLED;
            }
            if (switchInfo->subId == mStateInfo->dds) {
                // respond with success
                SM_LOGD("[InitializedState]: responding to SetPreferredDataModem with success");
                SetPreferredDataModemResponse_t resp;
                resp.errCode = SetPreferredDataModemResult_t::NO_ERROR;
                auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
            } else {
                PendingMessage ddsMsg = {ddsRequest, std::static_pointer_cast<Message>(switchInfo->msg)};
                mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(ddsMsg));
                transitionTo(ApTriggered);
            }
            return HANDLED;
        }
        case DDSSwitchApStarted:
        {
            transitionTo(ApTriggered);
            return HANDLED;
            break;
        }
        case CurrentDDSInd:
        case DDSSwitchMpStarted:
        {
            if (switchInfo->subId == mStateInfo->dds) {
                SM_LOGD(getName() + ": already on the triggered dds sub. Ignoring " + e.getEventName());
            } else {
                // cache current dds
                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
                transitionTo(MpTriggered);
                SM_LOGD(getName() + " switch dds to sub " + std::to_string(mStateInfo->dds));
            }
            return HANDLED;
        }
        case TempDDSSwitchTimerExpired:
        {
            SM_LOGD("Temp DDS timer Expired");
            int error = static_cast<int>(SetPreferredDataModemResult_t::NO_ERROR);
            mStateInfo->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
            SM_LOGD("Initiating Permanent DDS switch with sub" + std::to_string(mStateInfo->dds));
            ModemEndPointFactory<DSDModemEndPoint>::getInstance()
                         .buildEndPoint()->triggerDDSSwitch(mStateInfo->dds, error, mStateInfo->switch_type);
            return HANDLED;
        }
        case DDSSwitchInternal:
        {
            if(mStateInfo->isRilIpcNotifier) {
                Log::getInstance().d("[InitializedState]: DDSSwitchInternal on master RIL subId="
                                    +std::to_string((int)switchInfo->subId)+
                                    " switchType="+std::to_string((int)switchInfo->switch_type)+
                                    " level="+std::to_string((int)switchInfo->tempSwitchLevel));
                mStateInfo->tempDdsSwitchRecommendInfo = *switchInfo;
                if(switchInfo->switch_type == DSD_DDS_SWITCH_PERMANENT_V01 &&
                   switchInfo->subId == mStateInfo->dds) {
                    if (mStateInfo->tempDdsSwitched) {
                        PendingMessage ddsMsg = {ddsRequest, nullptr};
                        mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(ddsMsg));
                        transitionTo(ApTriggered);
                        return HANDLED;
                    } else {
                        //This case is when Smart Temp dds is disabled at modem
                        //But Modem sends permanent recommendation when call is ended
                        //Ignoring the recommendation for this case
                        mStateInfo->tempDdsSwitchRecommendInfo.reset();
                    }
                }
            }
            sendRecommendationToFramework(switchInfo);
            return HANDLED;
        }
        case ServiceDown:
        {
            if(mStateInfo->tempDdsSwitched)
            {
                mStateInfo->tempDdsSwitched = false;
                mStateInfo->switch_type = DSD_DDS_SWITCH_PERMANENT_V01;
                mStateInfo->dds = mStateInfo->permanentDDSSub;
                mStateInfo->currentDDSSwitchType =DSD_DDS_SWITCH_PERMANENT_V01;

                if(ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint()->getUserDdsSwitchSelection())
                {
                    auto recInd = std::make_shared<DdsSwitchRecommendationIndMessage>((SubscriptionId_t)mStateInfo->dds);
                    recInd->broadcast();
                }
            }
            return HANDLED;
        }
        case GetCurrentStateRequest:
        {
            if(!mStateInfo->isRilIpcNotifier) {
                auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::GetCurrentStateResponse);
                msg->setPreferredDataState(IAInfoPreferredDataStates::InitializedState);
                msg->broadcast();
            }
            return HANDLED;
        }
        default:
            break;
    }
    return UNHANDLED;
}

int DefaultDataState::handleEvent(const PreferredDataEventType& e) {
    std::ignore = e;
    return UNHANDLED;
}

int NonDefaultDataState::handleEvent(const PreferredDataEventType& e) {
    std::ignore = e;
    return UNHANDLED;
}

int SwitchingState::handleEvent(const PreferredDataEventType& e) {
    std::ignore = e;
    return UNHANDLED;
}

void ApTriggeredState::enter() {
    if (mStateInfo->isRilIpcNotifier) {
        SM_LOGD("[ApTriggeredState]: broadcasting IPC switch started");
        // Send ipc message
        auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::ApStarted,
            mStateInfo->dds, mStateInfo->currentDDSSwitchType, mStateInfo->tempDdsSwitched, mStateInfo->permanentDDSSub, mStateInfo->isDataAllowedOnNDDS);
        ipcStarted->broadcast();

        // send dds switch request to modem
        Message::Callback::Status status = Message::Callback::Status::FAILURE;
        int error = static_cast<int>(SetPreferredDataModemResult_t::NO_ERROR);
        dsd_dds_switch_type_enum_v01 switchType = mStateInfo->switch_type;
        if (mStateInfo->tempDdsSwitchRecommendInfo.has_value()) {
            switchType = mStateInfo->tempDdsSwitchRecommendInfo.value().switch_type;
        }

        auto msg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();
        if(msg && msg->event == PendingEvents::ddsRequest)
        {
            if(msg->msg != nullptr)
            {
                //Sending failure for older dds request if there are multiple dds requests in the List.
                if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(ddsRequest) > 1)
                {
                    mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(ddsRequest);
                }
                auto ddsRequestMessage = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(msg->msg);
                if (ddsRequestMessage != nullptr) {
                    status = ModemEndPointFactory<DSDModemEndPoint>::getInstance()
                        .buildEndPoint()->triggerDDSSwitch(ddsRequestMessage->getModemId(), error, switchType);
                } else {
                    SM_LOGE("[ApTriggeredState]: pending SetPreferredDataModem message is null");
                    error = static_cast<int>(SetPreferredDataModemResult_t::INVALID_ARG);
                }
                if (status != Message::Callback::Status::SUCCESS) {
                    sendResponse(ddsRequestMessage, static_cast<SetPreferredDataModemResult_t>(error));
                    auto exitMessage = std::make_shared<ExitFromCurrentStateRequestMessage>();
                    exitMessage->broadcast();
                }
            }
            else
            {
                //This is received when there is a call on another sub and sim is removed from original dds sub
                //With this recommendation, We will make permanent switch request for the current dds sub
                SM_LOGE("[ApTriggeredState]: pending SetPreferredDataModem message is null");
                if (mStateInfo->tempDdsSwitched &&
                    mStateInfo->tempDdsSwitchRecommendInfo.has_value() &&
                    mStateInfo->tempDdsSwitchRecommendInfo.value().switch_type == DSD_DDS_SWITCH_PERMANENT_V01) {
                        ModemEndPointFactory<DSDModemEndPoint>::getInstance()
                            .buildEndPoint()->triggerDDSSwitch(mStateInfo->tempDdsSwitchRecommendInfo.value().subId,
                                                               error, DSD_DDS_SWITCH_PERMANENT_V01);
                        ddsSwitchRequestTimer = TimeKeeper::getInstance().set_timer(
                             std::bind(&PreferredDataState::onDDSSwitchRequestExpired, this, std::placeholders::_1),
                             nullptr,
                             DDS_SWITCH_REQUEST_TIMEOUT);
                        mStateInfo->tempDdsSwitched = false;
                        mStateInfo->tempDdsSwitchRecommendInfo.reset();
                        return;
                 }
             }
        }
    }
}

void ApTriggeredState::exit() {

    if (mStateInfo->isRilIpcNotifier) {
        SM_LOGD("[ApTriggeredState]: broadcasting IPC switch Completed");
        // Send ipc message
        auto ipcCompleted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::Completed,
            mStateInfo->dds, mStateInfo->currentDDSSwitchType, mStateInfo->tempDdsSwitched, mStateInfo->permanentDDSSub, mStateInfo->isDataAllowedOnNDDS);
        ipcCompleted->broadcast();
        TimeKeeper::getInstance().clear_timer(ddsSwitchRequestTimer);
    }
}

int ApTriggeredState::handleEvent(const PreferredDataEventType& e) {
    SM_LOGD(getName() + ": handling " + e.getEventName());
    DDSSwitchInfo_t *switchInfo = nullptr;
    if(e.data != nullptr)
    {
      switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
    }
    switch (e.event) {
        case CurrentDDSInd:
        {
            if (switchInfo != nullptr)
            {
                if(mStateInfo->tempDdsSwitchRecommendInfo.has_value() &&
                   mStateInfo->tempDdsSwitchRecommendInfo.value().switch_type == DSD_DDS_SWITCH_TEMPORARY_V01) {
                    mStateInfo->tempDdsSwitchRecommendInfo.reset();
                    mStateInfo->tempDdsSwitched = true;
                    mStateInfo->permanentDDSSub = mStateInfo->dds;
                }

                if(switchInfo->switch_type != DSD_DDS_SWITCH_TEMPORARY_V01 && mStateInfo->tempDdsSwitched)
                    mStateInfo->tempDdsSwitched = false;

                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;

                if(mStateInfo->mPreferredDataMsgList->getNextMessageFromList()) {
                  sendResponse(mStateInfo->mPreferredDataMsgList->getNextMessageFromList(),
                               SetPreferredDataModemResult_t::NO_ERROR);
                }

                if(mStateInfo->dds != global_instance_id) {
                    disconnectCallFromOldDDSSub();
                }

                //checking for the nextstate for Master RIL
                #ifdef QMI_RIL_UTF
                if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) > 0 ||
                    mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) > 0) {
                    if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) > 0)
                        mStateInfo->isRilIpcNotifier = FALSE;
                #else
                if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) > 0) {
                #endif
                    SM_LOGD("[ApTriggeredState] : found IA request in PendingMessageList");
                    transitionTo(IAHandling);
                }
                else
                {
                    ddsTransition();
                }
            }
            return HANDLED;
        }
        case DDSSwitchInd:
        {
            if (switchInfo != nullptr && switchInfo->switchAllowed == DDSSwitchRes::ALLOWED) {
                return HANDLED;
            }
            ddsTransition();

            if (switchInfo != nullptr && switchInfo->switchAllowed == DDSSwitchRes::FAIL) {
                SM_LOGD("[ApTriggeredState]: DDSSwitch failed");
                if(mStateInfo->mPreferredDataMsgList->getNextMessageFromList()) {
                    sendResponse(mStateInfo->mPreferredDataMsgList->getNextMessageFromList(), SetPreferredDataModemResult_t::DDS_SWITCH_NOT_ALLOWED);
                }
            }

            return HANDLED;
        }
        case DDSSwitchTimerExpired:
        {
            ddsTransition();
            if(mStateInfo->mPreferredDataMsgList->getNextMessageFromList()) {
                sendResponse(mStateInfo->mPreferredDataMsgList->getNextMessageFromList(), SetPreferredDataModemResult_t::DDS_SWITCH_TIMEOUT);
            }
            return HANDLED;
        }
        case SetPreferredDataModem:
        {
            /* If SetPreferredDataModem is triggered while request is still pending,
               respond with error */
            if(switchInfo != nullptr)
            {
              std::shared_ptr<SetPreferredDataModemRequestMessage> m =
                      std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
              if (m != nullptr) {
                  SetPreferredDataModemResponse_t resp;
                  resp.errCode = SetPreferredDataModemResult_t::REQUEST_ALREADY_PENDING;
                  auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                  if (r != nullptr) {
                      SM_LOGE("responding to SetPreferredDataModem with error=" +
                                           std::to_string(static_cast<int>(r->errCode)));
                      m->sendResponse(m, Message::Callback::Status::FAILURE, r);
                  }
              }
            }
            return HANDLED;
        }
        case DDSSwitchCompleted:
        {
            if(switchInfo != nullptr)
            {
              mStateInfo->dds = switchInfo->subId;
              mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
              mStateInfo->tempDdsSwitched = switchInfo->isTempDDS;
              mStateInfo->permanentDDSSub = switchInfo->permanentDDS;

              if(mStateInfo->dds != global_instance_id)
              {
                disconnectCallFromOldDDSSub();
              }

              //checking for the nextState for Slave RIL
              if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) > 0) {
                SM_LOGD("[ApTriggeredState] : found IA request in PendingMessageList");
                transitionTo(IAHandling);
              } else {
                ddsTransition();
              }
            }
            return HANDLED;
        }
        case IAStarted:
        {
            //if IA is received in ApTriggered state, store it.
            SM_LOGD("[ApTriggeredState : Store IA in PendingMessageList]");
            PendingMessage iaMsg;
            if(mStateInfo->isRilIpcNotifier) {
              iaMsg = {IAOnMasterRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
            } else {
              iaMsg = {IAOnSlaveRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
              #ifdef QMI_RIL_UTF
              mStateInfo->isRilIpcNotifier = FALSE;
              #endif
            }
            mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));
            return HANDLED;
        }
        case IAStartRequest:
        {
            return HANDLED;
        }
        case ServiceDown:
        {
            //With SericeDown, Exit ApTrigger state only for Master RIL
            //Slave RIL will come out of ApTrigger state with DDSSwitchCompleted
            if (mStateInfo->isRilIpcNotifier)
            {
                if(mStateInfo->tempDdsSwitchRecommendInfo.has_value() &&
                    mStateInfo->tempDdsSwitchRecommendInfo.value().switch_type == DSD_DDS_SWITCH_TEMPORARY_V01)
                {
                    mStateInfo->tempDdsSwitched = false;
                    mStateInfo->tempDdsSwitchRecommendInfo.reset();
                }
                ddsTransition();

                if(mStateInfo->mPreferredDataMsgList->getNextMessageFromList()) {
                    sendResponse(mStateInfo->mPreferredDataMsgList->getNextMessageFromList(), SetPreferredDataModemResult_t::DDS_SWITCH_FAILED);
                }

                if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) > 0) {
                    SM_LOGD("[ApTriggeredState] : found IA request in PendingMessageList");
                    transitionTo(IAHandling);
                } else {
                    ddsTransition();
                }
            }
            return HANDLED;
        }
        case DDSSwitchInternal:
        {
            //This can only be received by slave RIL in APTriggeredState
            if(switchInfo != nullptr)
            {
                Log::getInstance().d("[ApTriggeredState]: DDSSwitchInternal switch type="+std::to_string((int)switchInfo->switch_type)+
                                 " subId="+std::to_string((int)switchInfo->subId)+" dds="+std::to_string((int)mStateInfo->dds));
                if ((switchInfo->switch_type != DSD_DDS_SWITCH_PERMANENT_V01) &&
                      ModemEndPointFactory<DSDModemEndPoint>::getInstance().buildEndPoint()->getUserDdsSwitchSelection()) {
                    auto recInd = std::make_shared<DdsSwitchRecommendationIndMessage>((SubscriptionId_t)switchInfo->subId);
                    recInd->broadcast();
                }
            }
            return HANDLED;
        }
        case ExitState:
        {
            ddsTransition();
            return HANDLED;
        }
        default:
            break;
    }
    SM_LOGD(getName() + ": ignored " + e.getEventName());
    return UNHANDLED;
}

void MpTriggeredState::enter() {
    if (mStateInfo->isRilIpcNotifier) {
        SM_LOGD("[MpTriggeredState]: broadcasting MP IPC switch started");
        // Send ipc message
        auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::MpStarted,
            mStateInfo->dds, mStateInfo->currentDDSSwitchType, mStateInfo->tempDdsSwitched, mStateInfo->permanentDDSSub, mStateInfo->isDataAllowedOnNDDS);
        ipcStarted->broadcast();
    }
}

int MpTriggeredState::handleEvent(const PreferredDataEventType& e) {
    SM_LOGD(getName() + ": handling " + e.getEventName());
    DDSSwitchInfo_t *switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);
    switch (e.event) {
        case SetPreferredDataModem:
        {
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
            if (switchInfo->subId == mStateInfo->dds) {
                //This should always be true
                auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::ApStarted,
                    mStateInfo->dds, mStateInfo->currentDDSSwitchType, mStateInfo->tempDdsSwitched, mStateInfo->permanentDDSSub, mStateInfo->isDataAllowedOnNDDS);
                ipcStarted->broadcast();
                // respond with success
                SM_LOGD(getName() + " responding to SetPreferredDataModem with success");
                SetPreferredDataModemResponse_t resp;
                resp.errCode = SetPreferredDataModemResult_t::NO_ERROR;
                auto r = make_shared<SetPreferredDataModemResponse_t>(resp);
                m->sendResponse(m, Message::Callback::Status::SUCCESS, r);
                disconnectCallFromOldDDSSub();
                ddsTransition();
            }
            return HANDLED;
        }
        case DDSSwitchApStarted:
        {
            if (switchInfo->subId == mStateInfo->dds) {
                ddsTransition();
            } else {
                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
                transitionTo(ApTriggered);
            }
            return HANDLED;
        }
        case CurrentDDSInd:
        {
            if(switchInfo->subId != mStateInfo->dds) {
                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
                SM_LOGD(getName() + " switch dds to sub " + std::to_string(mStateInfo->dds));

                // Send ipc message
                auto ipcStarted = make_shared<DDSSwitchIPCMessage>(DDSSwitchIPCMessageState::MpStarted,
                    mStateInfo->dds, mStateInfo->currentDDSSwitchType, mStateInfo->tempDdsSwitched, mStateInfo->permanentDDSSub, mStateInfo->isDataAllowedOnNDDS);
                ipcStarted->broadcast();
                SM_LOGD("[MpTriggeredState]: broadcasting MP IPC switch started");
            }
            return HANDLED;
        }
        case DDSSwitchMpStarted:
        {
            if (switchInfo->subId != mStateInfo->dds) {
                // cache current dds
                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
            }
            return HANDLED;
        }
        default:
            break;
    }
    SM_LOGD(getName() + ": ignored " + e.getEventName());
    return UNHANDLED;
}

void IAHandlingState::enter()
{
  std::shared_ptr<PendingMessage> msg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();

  if(msg == nullptr) {
    //This should never be executed
    SM_LOGD("[IAHandlingState]:No message found in pendingList");
    ddsTransition();
    return;
  }

  switch((*msg).event) {
    case IAOnMasterRIL:
    {
        int count = mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL);
        if(count > 1)
        {
            SM_LOGD("[IAHandlingState] : More than one IA request found");
            mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(IAOnMasterRIL);
        }
        msg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();

        if(msg != nullptr) {
            auto m = make_shared<ProcessInitialAttachRequestMessage>(msg->msg);
            m->broadcast();
        }
      break;
    }
    case IAOnSlaveRIL:
    {
        int count = mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL);
        if(mStateInfo->isRilIpcNotifier)
        {
            #ifdef QMI_RIL_UTF
            mStateInfo->isRilIpcNotifier = FALSE;
            #else
            mStateInfo->iaTimer = TimeKeeper::getInstance().set_timer(
                std::bind(&PreferredDataState::iaTimerExpired, this, std::placeholders::_1),
                          nullptr, IA_ON_MASTER_REQUEST_TIMEOUT);
            Log::getInstance().d("Started IA request timer from Master RIL. This will be cleared wehn IA on slave is Completed");
            #endif
            auto mg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
            mg->broadcast();
        }
        else
        {
            if(count > 1)
            {
                mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(IAOnSlaveRIL);
            }
            #ifdef QMI_RIL_UTF
            mStateInfo->isRilIpcNotifier = TRUE;
            #endif
            auto mg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartRequest);
            mg->broadcast();
            //If Master RIL misses this IAStartRequest ipc due to ril restart,
            //This timer ensures that Slave RIL doesn't stuck in IAHandlingState.
            mStateInfo->iaTimer = TimeKeeper::getInstance().set_timer(
                std::bind(&PreferredDataState::iaTimerExpired, this, std::placeholders::_1),
                          nullptr, IA_ON_SLAVE_REQUEST_TIMEOUT);
            Log::getInstance().d("Started IA request timer from Slave RIL");

        }
        break;
    }
    default:
      //This should never be executed
      ddsTransition();
      break;
  }
}

int IAHandlingState::handleEvent(const PreferredDataEventType& e)
{
    SM_LOGD(getName() + ": handling " + e.getEventName());
    DDSSwitchInfo_t *switchInfo;
    switchInfo = static_cast<DDSSwitchInfo_t *>(e.data);

    switch (e.event) {
        case IAStarted:
        {
            if(mStateInfo->isRilIpcNotifier)
            {
              PendingMessage iaMsg = {IAOnMasterRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
              mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));
              if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) == 1)
              {
                //Process this IA request if there are no other pending IA request on master RIL
                SM_LOGD("[IAHandlingState] : Started processing IA");
                auto m = make_shared<ProcessInitialAttachRequestMessage>(iaMsg.msg);
                m->broadcast();
              }
            }
            else
            {
              //slave RIL
              SM_LOGD("[IAHandlingState] : Received another IA, storing this request");
              PendingMessage iaMsg = {IAOnSlaveRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
              mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));

              #ifdef QMI_RIL_UTF
              mStateInfo->isRilIpcNotifier = TRUE;
              #endif
            }
            return HANDLED;
        }
        case IAStartRequest:
        {
            /* Multiple IAOnSlaveRIL is possible in the case where RIL0 has sent IAStartResponse ipc.
              But before RIL1 receives it, iaTimer expires and RIL1 sends another
              IAStartRequest ipc. So RIL0 ignores this ipc in this case. */
            #ifndef QMI_RIL_UTF
            if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) == 0)
            #endif
            {
              PendingMessage iaMsg = {IAOnSlaveRIL, nullptr};
              #ifdef QMI_RIL_UTF
              mStateInfo->isRilIpcNotifier = FALSE;
              #else
              mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));
              mStateInfo->iaTimer = TimeKeeper::getInstance().set_timer(
                std::bind(&PreferredDataState::iaTimerExpired, this, std::placeholders::_1),
                          nullptr, IA_ON_MASTER_REQUEST_TIMEOUT);
              Log::getInstance().d("Started IA request timer from Master RIL. This will be cleared when IA on slave is Completed");
              #endif
              auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
              msg->broadcast();
            }
            auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
            msg->broadcast();
            return HANDLED;
        }
        case IACompletedOnMasterRIL:
        {
            PendingMessage iaMsg = {IAOnMasterRIL, switchInfo->msg};
            mStateInfo->mPreferredDataMsgList->removeMessage(std::make_shared<PendingMessage>(iaMsg));
            if(mStateInfo->mPreferredDataMsgList->isEmpty())
            {
                ddsTransition();
            }
            else if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) == 0 )
            {
                SM_LOGD("[IAHandlingState] : No IA request pending on Slave RIL");

                if(nextState != PreferredDataStateEnum::Initialized)
                {
                    transitionTo(nextState);
                    nextState = PreferredDataStateEnum::Initialized;
                }
                else
                {
                    auto msg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();
                    if(msg != nullptr && msg->event == PendingEvents::ddsRequest)
                    {
                        transitionTo(ApTriggered);
                    }
                    else if(msg != nullptr && msg->event == PendingEvents::IAOnMasterRIL)
                    {
                        if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) > 1)
                        {
                            SM_LOGD("[IAHandlingState] : More than one IA request found");
                            mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(IAOnMasterRIL);
                        }
                        auto iaMsg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();
                        auto m = make_shared<ProcessInitialAttachRequestMessage>(iaMsg->msg);
                        m->broadcast();
                    }
                }
           }
           return HANDLED;
        }
        case IACompletedOnSlaveRIL:
        {
            PendingMessage iaMsg = {IAOnSlaveRIL, nullptr};
            #ifdef QMI_RIL_UTF
            if(!mStateInfo->isRilIpcNotifier)
            #endif
            mStateInfo->mPreferredDataMsgList->removeMessage(std::make_shared<PendingMessage>(iaMsg));
            if(mStateInfo->isRilIpcNotifier)
            {
                TimeKeeper::getInstance().clear_timer(mStateInfo->iaTimer);
                if(nextState != PreferredDataStateEnum::Initialized)
                {
                    transitionTo(nextState);
                    nextState = PreferredDataStateEnum::Initialized;
                }
                else if(mStateInfo->mPreferredDataMsgList->isEmpty())
                {
                    ddsTransition();
                }
                else
                {
                    auto msg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();
                    if(msg != nullptr) {
                        if((mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) == 0) &&
                           (msg->event == PendingEvents::ddsRequest)) {
                            SM_LOGD("[IAHandlingState] : No pending IA on Master RIL and found ddsRequest");
                            transitionTo(ApTriggered);
                        } else if(msg->event == PendingEvents::IAOnMasterRIL){
                            auto m = make_shared<ProcessInitialAttachRequestMessage>(msg->msg);
                            m->broadcast();
                        }
                    }
                       
                }
            }
            else
            {
                #ifdef QMI_RIL_UTF
                mStateInfo->isRilIpcNotifier = TRUE;
                #endif
                //Inform Master RIL about the IACompletion on Slave RIL
                auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IACompletedOnSlaveRIL);
                msg->broadcast();

                if(mStateInfo->mPreferredDataMsgList->isEmpty())
                {
                    ddsTransition();
                }
                else
                {
                    if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) > 1)
                    {
                        SM_LOGD("[IAHandlingState] : More than one IA request found");
                        mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(IAOnSlaveRIL);
                    }
                    auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartRequest);
                    msg->broadcast();
                    mStateInfo->iaTimer = TimeKeeper::getInstance().set_timer(
                          std::bind(&PreferredDataState::iaTimerExpired, this, std::placeholders::_1),
                          nullptr, IA_ON_SLAVE_REQUEST_TIMEOUT);
                }
            }
            return HANDLED;
        }
        case SetPreferredDataModem:
        {
            if(nextState != PreferredDataStateEnum::Initialized)
                nextState = PreferredDataStateEnum::Initialized;

            PendingMessage ddsMsg = {ddsRequest, std::static_pointer_cast<Message>(switchInfo->msg)};
            mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(ddsMsg));

            if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(ddsRequest)> 1)
            {
                mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(ddsRequest);
            }
            if (switchInfo->subId == mStateInfo->dds)
            {
                SM_LOGD("[IAHandlingState]: responding to SetPreferredDataModem with success");
                auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>(switchInfo->msg);
                sendResponse(m, SetPreferredDataModemResult_t::NO_ERROR);
            }
            return HANDLED;
        }
        case CurrentDDSInd:
        {
            if (switchInfo->subId == mStateInfo->dds)
            {
                SM_LOGD("[IAHandlingState]: already on the triggered dds sub. Ignoring ");
            }
            else
            {
                mStateInfo->dds = switchInfo->subId;
                mStateInfo->currentDDSSwitchType = switchInfo->switch_type;
                nextState = PreferredDataStateEnum::MpTriggered;
            }
            return HANDLED;
        }
        case DDSSwitchApStarted:
        {
            if(!mStateInfo->isRilIpcNotifier)
            {
                TimeKeeper::getInstance().clear_timer(mStateInfo->iaTimer);
                transitionTo(ApTriggered);
            }
            return HANDLED;
        }
        case IATimerExpired:
        {
            if(mStateInfo->isRilIpcNotifier)
            {
                //This is when Slave RIL has started IA processing and it is restarted
                //With this master RIl removes IAOnSlaveRIL message from its queue and
                //comes out of the IAHandlingState
                auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::GetCurrentStateRequest);
                msg->broadcast();
                mStateInfo->iaTimer = TimeKeeper::getInstance().set_timer(
                          std::bind(&PreferredDataState::iaTimerExpired, this, std::placeholders::_1),
                          nullptr, IA_ON_SLAVE_REQUEST_TIMEOUT);
            }
            else
            {
                if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) > 1)
                {
                    SM_LOGD("[IAHandlingState] : More than one IA request found");
                    mStateInfo->mPreferredDataMsgList->sendFailureForOldRequest(IAOnSlaveRIL);
                }
                auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartRequest);
                msg->broadcast();
                SM_LOGD("[IAHandlingState] : iaTimer is started");
                mStateInfo->iaTimer = TimeKeeper::getInstance().set_timer(
                    std::bind(&PreferredDataState::iaTimerExpired, this, std::placeholders::_1),
                    nullptr, IA_ON_SLAVE_REQUEST_TIMEOUT);
            }
            return HANDLED;
        }
        case DDSSwitchInternal:
        {
            if(mStateInfo->isRilIpcNotifier) {
                Log::getInstance().d("[IAHandlingState]: DDSSwitchInternal on master RIL subId="
                                    +std::to_string((int)switchInfo->subId)+
                                    " switchType="+std::to_string((int)switchInfo->switch_type)+
                                    " level="+std::to_string((int)switchInfo->tempSwitchLevel));
                mStateInfo->tempDdsSwitchRecommendInfo = *switchInfo;
                if(switchInfo->switch_type == DSD_DDS_SWITCH_PERMANENT_V01 &&
                   switchInfo->subId == mStateInfo->dds) {
                    if (mStateInfo->tempDdsSwitched) {
                        nextState = PreferredDataStateEnum::ApTriggered;
                        PendingMessage ddsMsg = {ddsRequest, nullptr};
                        mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(ddsMsg));
                    } else {
                        //This case is when Smart Temp dds is disabled at modem
                        //But Modem sends permanent recommendation when call is ended
                        //Ignoring the recommendation for this case
                        mStateInfo->tempDdsSwitchRecommendInfo.reset();
                    }
                }
            }
            sendRecommendationToFramework(switchInfo);
            return HANDLED;
        }
        case GetCurrentStateRequest:
        {
            if(!mStateInfo->isRilIpcNotifier) {
                auto msg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::GetCurrentStateResponse);
                msg->setPreferredDataState(IAInfoPreferredDataStates::IAHandlingState);
                msg->broadcast();
            }
            return HANDLED;
        }
        case GetCurrentStateResponse:
        {
            if(mStateInfo->isRilIpcNotifier) {
                TimeKeeper::getInstance().clear_timer(mStateInfo->iaTimer);

                if(switchInfo->slaveRILState == PreferredDataStateEnum::IAHandling) {
                    if(mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnSlaveRIL) == 0) {
                        PendingMessage iaMsg = {IAOnSlaveRIL, std::static_pointer_cast<Message>(switchInfo->msg)};
                        mStateInfo->mPreferredDataMsgList->insertMessage(std::make_shared<PendingMessage>(iaMsg));
                    }
                    /*If slave RIL receives multiple IAStartResponses, it will ignore rest of them
                    as mPreferredDataMsgList will be empty when 2nd IAStartResponse is received */
                    auto mg = make_shared<IAInfoIPCMessage>(IAInfoIPCMessageState::IAStartResponse);
                    mg->broadcast();
                }

                else
                {
                    //If slave RIL is not in IAHandlingState, exit from IAHandlingState
                    //for master RIl. this is possible when slave RIl is restarted while
                    //processing IA and IACompletedOnSlaveRIL is not received by master RIL
                    if(nextState != PreferredDataStateEnum::Initialized)
                    {
                        transitionTo(nextState);
                        nextState = PreferredDataStateEnum::Initialized;
                    }
                    else if(mStateInfo->mPreferredDataMsgList->isEmpty())
                    {
                        ddsTransition();
                    }
                    else
                    {
                        auto msg = mStateInfo->mPreferredDataMsgList->getNextMessageFromList();
                        if((msg != nullptr) &&
                           (mStateInfo->mPreferredDataMsgList->noOfRequestsForEvent(IAOnMasterRIL) == 0) &&
                           (msg->event == PendingEvents::ddsRequest))
                        {
                            SM_LOGD("[IAHandlingState] : No pending IA on Master RIL and found ddsRequest");
                            transitionTo(ApTriggered);
                        }
                    }
                }
            }
            return HANDLED;
        }
        default:
          break;
    }
    return UNHANDLED;
}

void PreferredDataMessageList::insertMessage(std::shared_ptr<PendingMessage> msg)
{
  SM_LOGD("PreferredDataMessageList : Adding message " + std::to_string((int)(msg->event)));
  mRequestList.push_back(msg);
}

std::shared_ptr<PendingMessage> PreferredDataMessageList::getNextMessageFromList()
{
  if(!mRequestList.empty()) {
    SM_LOGD("PreferredDataMessageList : next message in the list is "+ std::to_string((mRequestList.front())->event));
    return mRequestList.front();
  }
  else
    return nullptr;
}

void PreferredDataMessageList::removeMessage(std::shared_ptr<PendingMessage> msg)
{
  std::list<std::shared_ptr<PendingMessage>>::iterator it;
  for(it = mRequestList.begin(); it!= mRequestList.end(); it++) {
    if((*it)->event == msg->event && (msg->msg == nullptr || (*it)->msg == msg->msg)) {
      SM_LOGD("PreferredDataMessageList: removing message with event " + std::to_string((int)msg->event));
      it = mRequestList.erase(it);
      return;
    }
  }
  Log::getInstance().d("msg not found, list size is " + std::to_string(mRequestList.size()));
}

void PreferredDataMessageList::sendFailureForOldRequest(PendingEvents event)
{
  std::list<std::shared_ptr<PendingMessage>>::reverse_iterator rit;
  for(rit = mRequestList.rbegin(); rit!=mRequestList.rend(); ++rit) {
     if((*rit)->event == event)
       break;
  }

  PendingMessage lastIAmsg = {(*rit)->event, (*rit)->msg};

  std::list<std::shared_ptr<PendingMessage>>::iterator it = mRequestList.begin();
  while(it!=mRequestList.end())
  {
    if((*it)->event == event)
    {
        //not sending response for the last IA/dds message as we want to process it
        if((*it)->msg == lastIAmsg.msg)
        {
            it = mRequestList.erase(it);
            break;
        }

        if(event == ddsRequest)
        {
            auto m = std::static_pointer_cast<SetPreferredDataModemRequestMessage>((*it)->msg);
            if( m != nullptr )
            {
                SetPreferredDataModemResponse_t response;
                response.errCode = SetPreferredDataModemResult_t::INVALID_OPERATION;
                auto r = make_shared<SetPreferredDataModemResponse_t>(response);
                m->sendResponse(m, Message::Callback::Status::FAILURE, r);
            }
        }
        else
        {
            auto m = std::static_pointer_cast<SetInitialAttachApnRequestMessage>((*it)->msg);
            if( m != nullptr )
            {
                RIL_Errno result = RIL_E_INTERNAL_ERR;
                auto resp = std::make_shared<RIL_Errno>(result);
                m->sendResponse(m, Message::Callback::Status::FAILURE, resp);
                stringstream ss;
                ss << "[PreferredDataStates]: " << (int)m->getSerial() << "< setInitialAttachResponse resp=";
                ss << (int)result;
                Log::getInstance().d(ss.str());
            }
        }
        it = mRequestList.erase(it);
    }
    else
    {
        it++;
    }
  }
  mRequestList.push_front(std::make_shared<PendingMessage>(lastIAmsg));
}

int PreferredDataMessageList::noOfRequestsForEvent(PendingEvents event)
{
  int count =0;
  if(!mRequestList.empty())
  {
    std::list<std::shared_ptr<PendingMessage>>::iterator it;
    for(it = mRequestList.begin(); it!= mRequestList.end(); it++) {
      if((*it)->event == event) {
        count++;
      }
    }
  }
  return count;
}

bool PreferredDataMessageList::isEmpty()
{
  return mRequestList.empty();
}

void PreferredDataMessageList::clear()
{
    mRequestList.clear();
}
