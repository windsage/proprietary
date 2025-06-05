/*===========================================================================
   Copyright (c) 2022 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#pragma once

#include <framework/legacy.h>
#include <QtiMutex.h>
#include <binder/IBinder.h>
#include "BnRadioMessagingDefault.h"
#include "IRadioMessagingContext.h"
#include "interfaces/sms/qcril_qmi_sms_types.h"
#include <interfaces/sms/RilRequestSendSmsMessage.h>
#include <interfaces/sms/RilRequestCdmaSendSmsMessage.h>
#include <interfaces/sms/RilRequestImsSendSmsMessage.h>
#include <interfaces/sms/RilRequestAckGsmSmsMessage.h>
#include <interfaces/sms/RilRequestAckCdmaSmsMessage.h>
#include <interfaces/sms/RilRequestDeleteSmsOnSimMessage.h>
#include <interfaces/sms/RilRequestWriteSmsToSimMessage.h>
#include <interfaces/sms/RilRequestCdmaWriteSmsToRuimMessage.h>
#include <interfaces/sms/RilRequestCdmaDeleteSmsOnRuimMessage.h>
#include <interfaces/sms/RilRequestGetSmscAddressMessage.h>
#include <interfaces/sms/RilRequestSetSmscAddressMessage.h>
#include <interfaces/sms/RilRequestGetGsmBroadcastConfigMessage.h>
#include <interfaces/sms/RilRequestGetCdmaBroadcastConfigMessage.h>
#include <interfaces/sms/RilRequestGsmSetBroadcastSmsConfigMessage.h>
#include <interfaces/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h>
#include <interfaces/sms/RilRequestGsmSmsBroadcastActivateMessage.h>
#include <interfaces/sms/RilRequestCdmaSmsBroadcastActivateMessage.h>
#include <interfaces/sms/RilRequestReportSmsMemoryStatusMessage.h>
#include <interfaces/sms/RilRequestGetImsRegistrationMessage.h>
#include <interfaces/sms/RilRequestAckImsSmsMessage.h>
#include <interfaces/sms/RilUnsolIncoming3GppSMSMessage.h>
#include <interfaces/sms/RilUnsolIncoming3Gpp2SMSMessage.h>
#include <interfaces/sms/RilUnsolNewSmsOnSimMessage.h>
#include <interfaces/sms/RilUnsolNewSmsStatusReportMessage.h>
#include <interfaces/sms/RilUnsolNewBroadcastSmsMessage.h>
#include <interfaces/sms/RilUnsolCdmaRuimSmsStorageFullMessage.h>
#include <interfaces/sms/RilUnsolSimSmsStorageFullMessage.h>

namespace aidlmessaging {
  using namespace aidl::android::hardware::radio::messaging;
}

namespace aidlradio {
  using namespace aidl::android::hardware::radio;
}

class IRadioMessagingImpl : public aidlmessaging::BnRadioMessagingDefault {
 private:
  std::shared_ptr<aidlmessaging::IRadioMessagingResponse> mIRadioMessagingResponse;
  std::shared_ptr<aidlmessaging::IRadioMessagingIndication> mIRadioMessagingIndication;
  AIBinder_DeathRecipient* mDeathRecipient = nullptr;
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;

  std::shared_ptr<aidlmessaging::IRadioMessagingResponse> getResponseCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mIRadioMessagingResponse;
  };
  std::shared_ptr<aidlmessaging::IRadioMessagingIndication> getIndicationCallback() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mIRadioMessagingIndication;
  };

 public:
  IRadioMessagingImpl(qcril_instance_id_e_type instance);
  ~IRadioMessagingImpl();

  void setResponseFunctions_nolock(
      const std::shared_ptr<aidlmessaging::IRadioMessagingResponse>& response,
      const std::shared_ptr<aidlmessaging::IRadioMessagingIndication>& indication);

  void clearCallbacks();

  qcril_instance_id_e_type getInstanceId() {
    return mInstanceId;
  }

  std::shared_ptr<IRadioMessagingContext> getContext(uint32_t serial) {
    std::shared_ptr<IRadioMessagingContext> ctx =
      std::make_shared<IRadioMessagingContext>(mInstanceId, serial);
    return ctx;
  }

  void deathNotifier(void *cookie);

  // AIDL APIs
  ::ndk::ScopedAStatus responseAcknowledgement();
  ::ndk::ScopedAStatus setResponseFunctions(
      const std::shared_ptr<aidlmessaging::IRadioMessagingResponse>& in_radioNetworkResponse,
      const std::shared_ptr<aidlmessaging::IRadioMessagingIndication>& in_radioNetworkIndication
  ) override;
::ndk::ScopedAStatus sendSms(int32_t in_serial, const aidlmessaging::GsmSmsMessage& in_message);
::ndk::ScopedAStatus sendSmsExpectMore(int32_t in_serial,
   const aidlmessaging::GsmSmsMessage& in_message);
::ndk::ScopedAStatus sendCdmaSms(int32_t in_serial,
    const aidlmessaging::CdmaSmsMessage& in_sms);
::ndk::ScopedAStatus sendCdmaSmsExpectMore(int32_t in_serial,
    const aidlmessaging::CdmaSmsMessage& in_sms);
::ndk::ScopedAStatus sendImsSms(int32_t in_serial,
    const ::aidl::android::hardware::radio::messaging::ImsSmsMessage& in_message);
::ndk::ScopedAStatus setSmscAddress(int32_t in_serial,
    const std::string& in_smsc);
::ndk::ScopedAStatus getSmscAddress(int32_t in_serial);
::ndk::ScopedAStatus acknowledgeLastIncomingGsmSms(int32_t in_serial,
                        bool in_success, aidlmessaging::SmsAcknowledgeFailCause in_cause);
::ndk::ScopedAStatus acknowledgeIncomingGsmSmsWithPdu(int32_t in_serial,
    bool in_success, const std::string& in_ackPdu);
::ndk::ScopedAStatus acknowledgeLastIncomingCdmaSms(int32_t in_serial,
    const aidlmessaging::CdmaSmsAck& in_smsAck);
::ndk::ScopedAStatus reportSmsMemoryStatus(int32_t in_serial,
    bool in_available);
::ndk::ScopedAStatus writeSmsToRuim(int32_t in_serial,
    const aidlmessaging::CdmaSmsWriteArgs& in_cdmaSms);
::ndk::ScopedAStatus writeSmsToSim(int32_t in_serial,
    const aidlmessaging::SmsWriteArgs& in_smsWriteArgs);
::ndk::ScopedAStatus deleteSmsOnRuim(int32_t in_serial, int32_t in_index);
::ndk::ScopedAStatus deleteSmsOnSim(int32_t in_serial, int32_t in_index);
::ndk::ScopedAStatus getCdmaBroadcastConfig(int32_t in_serial);
::ndk::ScopedAStatus getGsmBroadcastConfig(int32_t in_serial);
::ndk::ScopedAStatus setCdmaBroadcastActivation(int32_t in_serial,
    bool in_activate);
::ndk::ScopedAStatus setGsmBroadcastActivation(int32_t in_serial,
    bool in_activate);
::ndk::ScopedAStatus setCdmaBroadcastConfig(int32_t in_serial,
    const std::vector<aidlmessaging::CdmaBroadcastSmsConfigInfo>& in_configInfo);
::ndk::ScopedAStatus setGsmBroadcastConfig(int32_t in_serial,
    const std::vector<aidlmessaging::GsmBroadcastSmsConfigInfo>& in_configInfo);

//response functions
  void sendResponseForSendSms(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilSendSmsResult_t> smsResult);
  void sendResponseForSendSMSExpectMore(int32_t serial,
    RIL_Errno errorCode, std::shared_ptr<RilSendSmsResult_t> smsResult);
  void sendResponseForSendCdmaSms(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilSendSmsResult_t> sendSmsResult);
  void sendResponseForSendCdmaSmsExpectMore(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilSendSmsResult_t> sendSmsResult);
  void sendResponseForSendImsSms(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilSendSmsResult_t> sendSmsResult);
  void sendResponseForSetSmscAddress(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetSmscAddress(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilGetSmscAddrResult_t> smscAddrResult);
  void sendResponseForAcknowledgeLastIncomingGsmSms(int32_t serial, RIL_Errno errorCode);
  void sendResponseForAcknowledgeIncomingGsmSmsWithPdu(int32_t serial, RIL_Errno errorCode);
  void sendResponseForAcknowledgeLastIncomingCdmaSms(int32_t serial, RIL_Errno errorCode);
  void sendResponseForReportSmsMemoryStatus(int32_t serial, RIL_Errno errorCode);
  void sendResponseForWriteSmsToRuim(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse);
  void sendResponseForWriteSmsToSim(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilWriteSmsToSimResult_t> writeSmsResponse);
  void sendResponseForDeleteSmsOnRuim(int32_t serial, RIL_Errno errorCode);
  void sendResponseForDeleteSmsOnSim(int32_t serial, RIL_Errno errorCode);
  void sendResponseForGetCdmaBroadcastConfig(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilGetCdmaBroadcastConfigResult_t> broadcastConfigResp);
  void sendResponseForGetGsmBroadcastConfig(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilGetGsmBroadcastConfigResult_t> broadcastConfigResp);
  void sendResponseForSetCdmaBroadcastActivation(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetGsmBroadcastActivation(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetCdmaBroadcastConfig(int32_t serial, RIL_Errno errorCode);
  void sendResponseForSetGsmBroadcastConfig(int32_t serial, RIL_Errno errorCode);

//indications
  void sendNewSms(std::shared_ptr<RilUnsolIncoming3GppSMSMessage> msg);
  void sendNewCdmaSms(std::shared_ptr<RilUnsolIncoming3Gpp2SMSMessage> msg);
  void sendNewSmsOnSim(std::shared_ptr<RilUnsolNewSmsOnSimMessage> msg);
  void sendNewBroadcastSms(std::shared_ptr<RilUnsolNewBroadcastSmsMessage> msg);
  void sendNewSmsStatusReport(std::shared_ptr<RilUnsolNewSmsStatusReportMessage> msg);
  void sendCdmaRuimSmsStorageFull(std::shared_ptr<RilUnsolCdmaRuimSmsStorageFullMessage> msg);
  void sendSimSmsStorageFull(std::shared_ptr<RilUnsolSimSmsStorageFullMessage> msg);

};
