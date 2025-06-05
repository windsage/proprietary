/******************************************************************************
* Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
* All rights reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef RIL_FOR_MDM_LE
#ifndef VOICECALLMODEMENDPOINT
#define VOICECALLMODEMENDPOINT
#include "modules/qmi/ModemEndPoint.h"
#include "VoiceCallModemEndPointModule.h"
#include "framework/Log.h"
#include "voice_service_v02.h"

#define QCRIL_DATA_QMI_TIMEOUT 10000
#define VOICEIND_WAITING_TIMEOUT 1000

using namespace rildata;

class VoiceCallModemEndPoint : public ModemEndPoint
{
public:
  VoiceCallModemEndPoint(std::string moduleName, SubId sub) : ModemEndPoint(moduleName)
  {
    mModule = new VoiceCallModemEndPointModule("VoiceCallModemEndPointModule", *this, sub);
    mModule->init();
    mSub = sub;
    mCallState = VoiceCallStateEnum::CALL_STATE_ENUM_MIN_ENUM_VAL;
    Log::getInstance().d("[VoiceCallModemEndPoint]: xtor");
  }
  ~VoiceCallModemEndPoint()
  {
    Log::getInstance().d("[VoiceCallModemEndPoint]: destructor");
    delete mModule;
    mModule = nullptr;
  }
  void requestSetup(string clientToken, qcril_instance_id_e_type id, GenericCallback<string>* cb);
  bool isVoiceCall();
  VoiceCallStateEnum getCallState() {return mCallState;}
  void setCallState(VoiceCallStateEnum state) {mCallState = state;}
  bool isVoiceCallInActiveState();
  TimeKeeper::timer_id mVoiceCallOrigTimer;
  bool mIsVoiceCallOrigTimer = false;

private:
  SubId mSub;
  VoiceCallStateEnum mCallState;
};

#endif
#endif//RIL_FOR_MDM_LE