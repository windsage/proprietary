/******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "RILQ"

#include "framework/Log.h"
#include "RadioDataServiceImpl.h"
#include "RadioServiceUtils.h"

#include "request/GetRadioDataCallListRequestMessage.h"
#include "request/SetInitialAttachApnRequestMessage.h"
#include "request/SetDataProfileRequestMessage.h"
#include "request/SetDataThrottlingRequestMessage.h"
#include "request/GetSlicingConfigRequestMessage.h"
#include "interfaces/nas/RilRequestAllowDataMessage.h"
#include "request/StartKeepAliveRequestMessage.h"
#include "request/StopKeepAliveRequestMessage.h"
#include "UnSolMessages/QcrilDataClientConnectedMessage.h"
#include "UnSolMessages/QcrilDataClientDisconnectedMessage.h"
#include "WakeLockUtils.h"


RadioDataServiceImpl::RadioDataServiceImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

RadioDataServiceImpl::~RadioDataServiceImpl() {
}

std::shared_ptr<IRadioDataResponse> RadioDataServiceImpl::getResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioDataResponse;
}

std::shared_ptr<IRadioDataIndication> RadioDataServiceImpl::getIndicationCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIRadioDataIndication;
}

void RadioDataServiceImpl::setResponseFunctions_nolock
(
  const std::shared_ptr<IRadioDataResponse>& radioDataResponse,
  const std::shared_ptr<IRadioDataIndication>& radioDataIndication
)
{
  mIRadioDataResponse = radioDataResponse;
  mIRadioDataIndication = radioDataIndication;
}

void RadioDataServiceImpl::deathNotifier()
{
  Log::getInstance().d("[" + mName  + "] : Client died, cleaning up callbacks");
  clearCallbacks();
  auto msg = std::make_shared<QcrilDataClientDisconnectedMessage>();
  msg->broadcast();
}

static void deathRecpCallback(void* cookie)
{
  RadioDataServiceImpl* impl = static_cast<RadioDataServiceImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier();
  }
}

::ndk::ScopedAStatus RadioDataServiceImpl::setResponseFunctions
(
  const std::shared_ptr<IRadioDataResponse>& in_radioDataResponse,
  const std::shared_ptr<IRadioDataIndication>& in_radioDataIndication)
{
  Log::getInstance().d("[" + mName + "] :setResponseFunctions: Set client callback");

  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

    #ifndef QMI_RIL_UTF
    if (mIRadioDataResponse != nullptr) {
      AIBinder_unlinkToDeath(mIRadioDataResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
    }
    #endif

    setResponseFunctions_nolock(in_radioDataResponse, in_radioDataIndication);

    #ifndef QMI_RIL_UTF
    if (mIRadioDataResponse != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mIRadioDataResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
    #endif
  }

  if(mIRadioDataResponse != nullptr && mIRadioDataIndication != nullptr) {
     auto msg = std::make_shared<QcrilDataClientConnectedMessage>();
     msg->broadcast();
  }

  return ndk::ScopedAStatus::ok();
}

void RadioDataServiceImpl::clearCallbacks()
{
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  mIRadioDataResponse = nullptr;
  mIRadioDataIndication = nullptr;
  mDeathRecipient = nullptr;
}

::ndk::ScopedAStatus RadioDataServiceImpl::setupDataCall
(
  int32_t in_serial,
  ::aidl::android::hardware::radio::AccessNetwork in_accessNetwork,
  const ::aidl::android::hardware::radio::data::DataProfileInfo& in_dataProfileInfo,
  bool in_roamingAllowed,
  ::aidl::android::hardware::radio::data::DataRequestReason in_reason,
  const std::vector<::aidl::android::hardware::radio::data::LinkAddress>& in_addresses,
  const std::vector<std::string>& in_dnses,
  int32_t in_pduSessionId,
  const std::optional<::aidl::android::hardware::radio::data::SliceInfo>& in_sliceInfo,
  bool in_matchAllRuleAllowed
)
{
  Log::getInstance().d("[" + mName + "] :setupDataCall");

  std::ignore = in_pduSessionId;
  std::ignore =  in_sliceInfo;
  using namespace rildata;
  using namespace utils;
  std::vector<std::string> radioAddresses;
  std::vector<std::string> radioDnses;

  for (size_t i = 0 ; i < in_addresses.size(); i++ ) {
    radioAddresses.push_back(in_addresses[i].address);
  }
  for (size_t i = 0 ; i < in_dnses.size(); i++ ) {
    radioDnses.push_back(in_dnses[i]);
  }
  AccessNetwork_t accnet = utils::convertAidlAccessNetworkToDataAccessNetwork(in_accessNetwork);
  DataProfileInfo_t profileInfo = utils::convertAidlDataProfileInfoToRil(in_dataProfileInfo);

  auto msg = std::make_shared<rildata::SetupDataCallRequestMessage_1_6>(
    in_serial,
    rildata::RequestSource_t::RADIO,
    accnet,
    profileInfo,
    in_roamingAllowed,
    (DataRequestReason_t)in_reason,
    radioAddresses,
    radioDnses,
    in_matchAllRuleAllowed,
    std::make_shared<std::function<void(int32_t)>>(std::bind(&RadioDataServiceImpl::sendAcknowledgeRequest, this, std::placeholders::_1)));

  if (profileInfo.trafficDescriptor.has_value()) {
    msg->setOptionalTrafficDescriptor(*profileInfo.trafficDescriptor);
  }

  GenericCallback<rildata::SetupDataCallResponse_t> cb([this, in_serial](std::shared_ptr<Message>,
                                          Message::Callback::Status status,
                                          std::shared_ptr<rildata::SetupDataCallResponse_t> rsp) -> void {
    if (rsp != nullptr) {
      auto indMsg = std::make_shared<rildata::SetupDataCallRadioResponseIndMessage>(*rsp, in_serial, status);
      if (indMsg != nullptr) {
          indMsg->broadcast();
      } else {
          Log::getInstance().d("[" + mName + "] setup data call cb failed to allocate message status " + std::to_string((int)status) +
                               " respErr " + std::to_string((int)rsp->respErr));
      }
    } else {
      Log::getInstance().d("[" + mName + "] setupDataCall resp is nullptr");
    }
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::deactivateDataCall
(
  int32_t serial,
  int32_t cid,
  ::aidl::android::hardware::radio::data::DataRequestReason requestReason
)
{
  Log::getInstance().d( "[" + mName + "] deactivateDataCall: serial " + std::to_string(serial));
  using namespace rildata;
  DataRequestReason_t reason = DataRequestReason_t(requestReason);

  auto msg = std::make_shared<DeactivateDataCallRequestMessage>(
    serial,
    cid,
    reason,
    std::make_shared<std::function<void(int32_t)>>(std::bind(&RadioDataServiceImpl::sendAcknowledgeRequest, this, std::placeholders::_1)));
  GenericCallback<ResponseError_t> cb([this,serial](std::shared_ptr<Message>,
                                               Message::Callback::Status status,
                                               std::shared_ptr<ResponseError_t> rsp) -> void {
    if (rsp != nullptr) {
      auto indMsg = std::make_shared<DeactivateDataCallRadioResponseIndMessage>(*rsp, serial, status);
      indMsg->broadcast();
    } else {
      Log::getInstance().d( "[" + mName + "] deactivate data call resp is nullptr");
    }
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::setInitialAttachApn
(
  int32_t serial,
  const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo>& dataProfileInfo
)
{
  ::aidl::android::hardware::radio::data::DataProfileInfo profileInfo;
  rildata::DataProfileInfo_t profile;
  if(dataProfileInfo.has_value()) {
    profile = utils::convertAidlDataProfileInfoToRil(dataProfileInfo.value());
  }

  std::shared_ptr<rildata::SetInitialAttachApnRequestMessage> msg;
  if (!dataProfileInfo.has_value()) {
    msg = std::make_shared<rildata::SetInitialAttachApnRequestMessage>(serial, nullptr);
  } else {
    msg = std::make_shared<rildata::SetInitialAttachApnRequestMessage>(serial, profile, nullptr);
  }

  GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                                               std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
    RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
    if (solicitedMsg && responseDataPtr) {
    Log::getInstance().d( mName +  "[setInitialAttachApn_cb invoked: [msg = " + solicitedMsg->dump() +
                          "] executed. client data = " + "set-ril-service-cb-token status = " + std::to_string((int)status));
    if(status == Message::Callback::Status::SUCCESS) {
      RadioError e = utils::convertMsgToRadioError(status, *responseDataPtr);
      responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
    } else {
      responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::GENERIC_FAILURE};
    }
  }
  this->setInitialAttachApnResponse(responseInfo);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::getDataCallList
(
  int32_t serial
)
{
  Log::getInstance().d( "[" + mName + "] getDataCallList: serial=" + std::to_string(serial));
  using namespace rildata;
  auto msg = std::make_shared<GetRadioDataCallListRequestMessage>();
  GenericCallback<DataCallListResult_t> cb(([this, serial](std::shared_ptr<Message> solicitedMsg,
                  Message::Callback::Status status, std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
    std::ignore = solicitedMsg;
    this->getDataCallListResponse(responseDataPtr, serial, status);
  }));
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::setDataProfile
(
  int32_t serial,
  const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>& in_profiles
)
{
  Log::getInstance().d( "[" + mName + "] setDataProfile: serial=" + std::to_string(serial));
  using namespace utils;
  vector<rildata::DataProfileInfo_t> p;

  for (size_t i = 0; i < in_profiles.size(); i++) {
    /* copy all the profiles */
    rildata::DataProfileInfo_t t = convertAidlDataProfileInfoToRil(in_profiles[i]);
    p.push_back(t);
  }

  auto msg = std::make_shared<rildata::SetDataProfileRequestMessage>(serial, p);
  GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                                                std::shared_ptr<RIL_Errno> responseDataPtr) -> void {
    RadioResponseInfo responseInfo {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::NO_MEMORY};
    if (solicitedMsg && responseDataPtr) {
      Log::getInstance().d( mName +  "[SetDataProfile_1_5 cb invoked: [msg = " + solicitedMsg->dump() +
                            "] executed. client data = " + "set-ril-service-cb-token status = " + std::to_string((int)status));
      if(status == Message::Callback::Status::SUCCESS) {
        RadioError e = convertMsgToRadioError(status, *responseDataPtr);
        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = e};
      } else {
        responseInfo = {.serial = serial, .type = RadioResponseType::SOLICITED, .error = RadioError::GENERIC_FAILURE};
      }
    }
    this->setDataProfileResponse(responseInfo);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::setDataThrottling
(
  int32_t serial,
  ::aidl::android::hardware::radio::data::DataThrottlingAction dataThrottlingAction,
  int64_t completionDurationMillis
)
{
  Log::getInstance().d( "[" + mName + "] setDataThrottling: serial=" + std::to_string(serial));
  using namespace rildata;
  using namespace utils;
  auto msg = std::make_shared<SetDataThrottlingRequestMessage>(convertAidlDataThrottleActionToRil(dataThrottlingAction),completionDurationMillis);
  GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                                      std::shared_ptr<RIL_Errno> resp) -> void {
    std::ignore = msg;
    RIL_Errno errorCode = RIL_Errno::RIL_E_GENERIC_FAILURE;
    if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
      errorCode = RIL_Errno::RIL_E_REQUEST_NOT_SUPPORTED;
    } else if (resp != nullptr && status == Message::Callback::Status::SUCCESS) {
      errorCode = *resp;
    }
    this->sendResponseForSetDataThrottling(serial, errorCode);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::setDataAllowed
(
  int32_t serial,
  bool allow
)
{
  Log::getInstance().d( "[" + mName + "] setDataAllowed: serial=" + std::to_string(serial));
  auto msg = std::make_shared<RilRequestAllowDataMessage>(this->getContext(serial), allow);
  GenericCallback<QcRilRequestMessageCallbackPayload> cb([this, serial](std::shared_ptr<Message>, Message::Callback::Status status,
                                                                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
    RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
    if (status == Message::Callback::Status::SUCCESS && resp) {
      errorCode = resp->errorCode;
    }
    this->sendResponseForSetDataAllowed(serial, errorCode);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::startKeepalive
(
  int32_t serial,
  const ::aidl::android::hardware::radio::data::KeepaliveRequest& in_keepalive
)
{
  Log::getInstance().d( "[" + mName + "] StartKeepAlive: serial=" + std::to_string(serial));
  using namespace rildata;
  switch((KeepaliveType_t)in_keepalive.type) {
    case KeepaliveType_t::NATT_IPV4:
      if (in_keepalive.sourceAddress.size() != 4 || in_keepalive.destinationAddress.size() != 4) {
        Log::getInstance().d( "[" + mName + "] Invalid address for keepalive!");
        RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::INVALID_ARGUMENTS};
        ::aidl::android::hardware::radio::data::KeepaliveStatus ka_status = {};
        this->sendStartKeepAliveResponse(responseInfo, ka_status);
        return ndk::ScopedAStatus::ok();
      }
      break;
    case KeepaliveType_t::NATT_IPV6:
      if (in_keepalive.sourceAddress.size() != 16 || in_keepalive.destinationAddress.size() != 16) {
        Log::getInstance().d( "[" + mName + "] Invalid address for keepalive!");
        RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::INVALID_ARGUMENTS};
        ::aidl::android::hardware::radio::data::KeepaliveStatus ka_status = {};
        this->sendStartKeepAliveResponse(responseInfo, ka_status);
        return ndk::ScopedAStatus::ok();
      }
      break;
    default:
      Log::getInstance().d( "[" + mName + "] Unknown packet keepalive type!");
      RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::INVALID_ARGUMENTS};
      ::aidl::android::hardware::radio::data::KeepaliveStatus ka_status = {};
      this->sendStartKeepAliveResponse(responseInfo, ka_status);
      return ndk::ScopedAStatus::ok();
  }

  KeepaliveRequest_t req;
  req.type = (KeepaliveType_t)in_keepalive.type;
  req.sourceAddress = in_keepalive.sourceAddress;
  req.sourcePort = in_keepalive.sourcePort;
  req.destinationAddress = in_keepalive.destinationAddress;
  req.destinationPort = in_keepalive.destinationPort;
  req.maxKeepaliveIntervalMillis = in_keepalive.maxKeepaliveIntervalMillis;
  req.cid = in_keepalive.cid;
  auto msg = std::make_shared<StartKeepAliveRequestMessage>(serial, req);

  std::stringstream source, dest;
  std::copy(req.sourceAddress.begin(), req.sourceAddress.end(), std::ostream_iterator<int>(source, " "));
  std::copy(req.destinationAddress.begin(), req.destinationAddress.end(), std::ostream_iterator<int>(dest, " "));

  Log::getInstance().d("KeepAlive type " + std::to_string((int)req.type));
  Log::getInstance().d("KeepAlive sourceAddress " + source.str());
  Log::getInstance().d("KeepAlive sourcePort "+ std::to_string(req.sourcePort));
  Log::getInstance().d("KeepAlive destinationAddress " + dest.str());
  Log::getInstance().d("KeepAlive destinationPort " + std::to_string(req.destinationPort));
  Log::getInstance().d("KeepAlive maxKeepaliveIntervalMillis "+ std::to_string(req.maxKeepaliveIntervalMillis));
  Log::getInstance().d("KeepAlive cid "+ std::to_string(req.cid));

  GenericCallback<StartKeepAliveResp_t> cb([this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                                                          std::shared_ptr<StartKeepAliveResp_t> rsp) -> void {
    RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::REQUEST_NOT_SUPPORTED};
    ::aidl::android::hardware::radio::data::KeepaliveStatus ka_status = {};
    if (msg && rsp) {
      Log::getInstance().d( "[" + mName + "] startKeepAlivecb msg & rsp not null");
      RadioError e = RadioError::NONE;
      if ((status != Message::Callback::Status::SUCCESS) || (rsp->error != ResponseError_t::NO_ERROR)) {
        switch(rsp->error) {
          case ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
          default: e = RadioError::NO_RESOURCES; break;
        }
      }
      responseInfo = {.serial = serial, .error = e};
      Log::getInstance().d( "[" + mName + "] startKeepAlivecb cb invoked status "+ std::to_string((int)status) +
                           " respErr " + std::to_string((int)(rsp->error)));
      ka_status.sessionHandle = rsp->handle;
      ka_status.code = static_cast<KeepaliveStatusCode>(rsp->status);
    }
    Log::getInstance().d( "[" + mName + "] startKeepAlivecb here");
    this->sendStartKeepAliveResponse(responseInfo, ka_status);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::stopKeepalive
(
  int32_t serial,
  int32_t sessionHandle
)
{
  Log::getInstance().d( "[" + mName + "] stopKeepalive: serial=" + std::to_string(serial));
  using namespace rildata;
  auto msg = std::make_shared<StopKeepAliveRequestMessage>(serial, sessionHandle);

  GenericCallback<ResponseError_t> cb([this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                                                     std::shared_ptr<ResponseError_t> rsp) -> void {
    RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::REQUEST_NOT_SUPPORTED};
    if (msg && rsp) {
      RadioError e = RadioError::NONE;
      if ((status != Message::Callback::Status::SUCCESS) || (*rsp != ResponseError_t::NO_ERROR)) {
        e = RadioError::INVALID_ARGUMENTS;
      }
      responseInfo = {.serial = serial, .error = e};
      Log::getInstance().d( "[" + mName + "] stopKeepAlivecb cb invoked status " + std::to_string((int)status)
                            + "respErr " + std::to_string((int)(*rsp)));
    }
    Log::getInstance().d( "[" + mName + "] stopKeepAlivecb sending resp");
    this->sendStopKeepAliveResponse(responseInfo);
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::getSlicingConfig
(
  int32_t serial
)
{
  Log::getInstance().d( "[" + mName + "] getSlicingConfig: serial=" + std::to_string(serial));
  using namespace rildata;

  auto msg = std::make_shared<GetSlicingConfigRequestMessage>(
    serial,
    std::make_shared<std::function<void(int32_t)>>(std::bind(&RadioDataServiceImpl::sendAcknowledgeRequest, this, std::placeholders::_1)));
  GenericCallback<GetSlicingConfigResp_t> cb([this, serial](std::shared_ptr<Message>,
                                          Message::Callback::Status status,
                                          std::shared_ptr<GetSlicingConfigResp_t> rsp) -> void {
    RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::REQUEST_NOT_SUPPORTED};
    if (rsp != nullptr) {
      RadioError e = RadioError::NONE;
      if(status != Message::Callback::Status::SUCCESS || (rsp->respErr != ResponseError_t::NO_ERROR)) {
        e = RadioError::INTERNAL_ERR;
      } else {
        responseInfo = {.serial = serial, .error = e};
      }
      Log::getInstance().d( "[" + mName + "] getSlicingConfigRespCb invoked status " + std::to_string((int)status)
                             + "respErr " + std::to_string((int)rsp->respErr));
      this->sendResponseForGetSlicingConfig(responseInfo, rsp->slicingConfig);
    } else {
      Log::getInstance().d( "[" + mName + "] setupDataCall resp is nullptr");
    }
  });
  msg->setCallback(&cb);
  msg->dispatch();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::responseAcknowledgement()
{
  Log::getInstance().d( "[" + mName + "] responseAcknowledgement");
  wakeLockUtils::releaseWakeLock();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::allocatePduSessionId
(
  int32_t in_serial
)
{
  Log::getInstance().d( "[" + mName + "] allocatePduSessionId: serial=" + std::to_string(in_serial));
  this->sendResponseForAllocatePduSessionId(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::releasePduSessionId
(
  int32_t serial,
  int32_t id
)
{
  Log::getInstance().d( "[" + mName + "] releasePduSessionId: serial=" +std::to_string(serial));
  std::ignore = id;
  this->sendResponseForReleasePduSessionId(serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::startHandover
(
  int32_t serial,
  int32_t callId
)
{
  Log::getInstance().d( "[" + mName + "] startHandover: serial=" + std::to_string(serial));
  std::ignore = callId;
  this->sendResponseForStartHandover(serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus RadioDataServiceImpl::cancelHandover
(
  int32_t serial,
  int32_t callId
)
{
  Log::getInstance().d( "[" + mName + "] cancelHandover: serial=" + std::to_string(serial));
  std::ignore = callId;
  this->sendResponseForCancelHandover(serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ndk::ScopedAStatus::ok();
}

#ifdef SET_LOCAL_URSP_CONFIG
::ndk::ScopedAStatus RadioDataServiceImpl::setLocalUrspConfiguration
(
  int32_t serial,
  std::vector<UrspRule> urspRules
)
{
  Log::getInstance().d( "[" + mName + "] RadioDataServiceImpl::setLocalUrspConfiguration: serial="+ std::to_string(serial));
  return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}
#endif

//Response related APIs
void RadioDataServiceImpl::sendResponseForSetupDataCall
(
  std::shared_ptr<rildata::SetupDataCallRadioResponseIndMessage> msg
)
{
  Log::getInstance().d( "[" + mName + "] Sending SetupDataCallRadioResponseIndMessage " + msg->dump());
  auto respCb = this->getResponseCallback();
  if (msg != nullptr && respCb != nullptr) {
    auto rsp = msg->getResponse();
    auto status = msg->getStatus();
    auto serial = msg->getSerial();

    RadioResponseInfo responseInfo {.serial = serial, .error = RadioError::NO_MEMORY};
    SetupDataCallResult dcResult = {};
    dcResult.cause = DataCallFailCause::ERROR_UNSPECIFIED;
    dcResult.suggestedRetryTime = -1;
    RadioError e = RadioError::NONE;
    if (status == Message::Callback::Status::SUCCESS) {
      Log::getInstance().d("setup data call cb invoked status " + std::to_string((int)(status)) +
                           " respErr " + std::to_string((int)(rsp.respErr)));
      Log::getInstance().d("cause = " +std::to_string((int)rsp.call.cause));
      Log::getInstance().d("suggestedRetryTime = " +std::to_string((int)rsp.call.suggestedRetryTime));
      Log::getInstance().d("cid = " +std::to_string(rsp.call.cid));
      Log::getInstance().d("active = " +std::to_string(rsp.call.active));
      Log::getInstance().d("type = " + rsp.call.type);
      Log::getInstance().d("ifname = " + rsp.call.ifname);
      Log::getInstance().d("addresses = " + rsp.call.addresses);
      Log::getInstance().d("dnses = " + rsp.call.dnses);
      Log::getInstance().d("gateways = " + rsp.call.gateways);
      Log::getInstance().d("pcscf = " + rsp.call.pcscf);
      Log::getInstance().d("mtuV4 = " + std::to_string(rsp.call.mtuV4));
      Log::getInstance().d("mtuV6 = " + std::to_string(rsp.call.mtuV6));
      Log::getInstance().d("handoverFailureMode = " + std::to_string((int)rsp.call.handoverFailureMode));
      Log::getInstance().d("tdSize = " +std::to_string(rsp.call.trafficDescriptors.size()));
      dcResult = utils::convertDcResultToAidlDcResult(rsp.call);
    } else {
      switch(rsp.respErr) {
        case rildata::ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
        case rildata::ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
        default: e = RadioError::GENERIC_FAILURE; break;
      }
    }
    responseInfo = {.serial = serial, .error = e};
    wakeLockUtils::grabPartialWakeLock();
    auto ret = respCb->setupDataCallResponse(responseInfo, dcResult);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send indication. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForDeactivateDataCall
(
  std::shared_ptr<rildata::DeactivateDataCallRadioResponseIndMessage> msg
)
{
  Log::getInstance().d( "[" + mName + "] Sending DeactivateDataCallRadioResponseIndMessage " + msg->dump());
  auto respCb = this->getResponseCallback();
  if (msg != nullptr && respCb != nullptr) {
    auto rsp = msg->getResponse();
    auto status = msg->getStatus();
    auto serial = msg->getSerial();
    RadioResponseInfo responseInfo{.serial = serial, .error = RadioError::NO_MEMORY};
    RadioError e = RadioError::NONE;
    if ((status != Message::Callback::Status::SUCCESS) ||
        (rsp != rildata::ResponseError_t::NO_ERROR)) {
      switch (rsp)
      {
        case rildata::ResponseError_t::NOT_SUPPORTED:
             e = RadioError::REQUEST_NOT_SUPPORTED;
             break;
        case rildata::ResponseError_t::INVALID_ARGUMENT:
             e = RadioError::INVALID_ARGUMENTS;
             break;
        case rildata::ResponseError_t::CALL_NOT_AVAILABLE:
             e = RadioError::INVALID_CALL_ID;
             break;
        default:
             e = RadioError::GENERIC_FAILURE;
             break;
        }
      }
      responseInfo = {.serial = serial, .error = e};
      Log::getInstance().d( "[" + mName + "] deactivate data call cb invoked status " + std::to_string((int)(status))
                           + " respErr " + std::to_string((int)(rsp)));
      wakeLockUtils::grabPartialWakeLock();
      auto ret = respCb->deactivateDataCallResponse(responseInfo);
      if (!ret.isOk()) {
        Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
      }
    } else {
        Log::getInstance().d( "[" + mName + "] Message or Response Callback is Nullptr");
    }
}

void RadioDataServiceImpl::getDataCallListResponse
(
  std::shared_ptr<rildata::DataCallListResult_t> responseDataPtr,
  int serial,
  Message::Callback::Status status
)
{
  Log::getInstance().d( "[" + mName + "] GetDataCallListResponse: serial=" +std::to_string(serial));
  using namespace rildata;
  auto respCb = this->getResponseCallback();
  if (respCb) {
    std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> dcResultList;
    RadioResponseInfo responseInfo {.type = RadioResponseType::SOLICITED, .serial = serial, .error = RadioError::NO_MEMORY};
    if (responseDataPtr) {
      RadioError e = RadioError::NONE;
      if ((status == Message::Callback::Status::SUCCESS) &&
          (responseDataPtr->respErr == ResponseError_t::NO_ERROR)) {
        Log::getInstance().d( "[" + mName + "] getDataCallList cb invoked status " + std::to_string((int)(status)) +
                             " respErr " + std::to_string((int)responseDataPtr->respErr));
        Log::getInstance().d( "[" + mName + "] Call list size = " + std::to_string(responseDataPtr->call.size()));

        dcResultList.resize(responseDataPtr->call.size());
        int i=0;
        for (rildata::DataCallResult_t entry: responseDataPtr->call) {
          Log::getInstance().d("cid " + std::to_string(entry.cid));
          Log::getInstance().d("ifname " + entry.ifname);
          Log::getInstance().d("addresses " + entry.addresses);
          dcResultList[i] = utils::convertDcResultToAidlDcResult(entry);
          i++;
        }
      } else {
        switch (responseDataPtr->respErr) {
          case ResponseError_t::NOT_SUPPORTED: e = RadioError::REQUEST_NOT_SUPPORTED; break;
          case ResponseError_t::INVALID_ARGUMENT: e = RadioError::INVALID_ARGUMENTS; break;
          default: e = RadioError::GENERIC_FAILURE; break;
        }
      }
      responseInfo = {.type = RadioResponseType::SOLICITED, .serial = serial, .error = e};
    }//responseDataptr
    auto ret = respCb->getDataCallListResponse(responseInfo, dcResultList);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::setInitialAttachApnResponse
(
  RadioResponseInfo responseInfo
)
{
  Log::getInstance().d( "[" + mName + "] Sending setInitialAttachApnResponse");
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->setInitialAttachApnResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::setDataProfileResponse
(
  RadioResponseInfo responseInfo
)
{
  Log::getInstance().d( "[" + mName + "] sending setDataProfileResponse");
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->setDataProfileResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForSetDataThrottling
(
  int32_t serial,
  RIL_Errno errorCode
)
{
  Log::getInstance().d( "[" + mName + "] Sending setDataThrottlingResponse serial=" + std::to_string(serial));
  RadioResponseInfo responseInfo{ RadioResponseType::SOLICITED, serial, static_cast<RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    Log::getInstance().d( "[" + mName + "] sendResponseForSetDataThrottling: serial=" + std::to_string(serial) + " error=" + std::to_string(errorCode));
    auto ret = respCb->setDataThrottlingResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForSetDataAllowed
(
  int32_t serial,
  RIL_Errno errorCode
)
{
  RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, serial, static_cast<RadioError>(errorCode)};
  auto respCb = this->getResponseCallback();
  if (respCb) {
    Log::getInstance().d( "[" + mName + "] setDataAllowedResponse: serial= "+ std::to_string(serial) + "error= " + std::to_string(errorCode));
    auto ret = respCb->setDataAllowedResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::sendAcknowledgeRequest
(
  int32_t serial
)
{
  auto respCb = this->getResponseCallback();
  if (respCb) {
    Log::getInstance().d( "[" + mName + "] acknowledgeRequest: serial=" + std::to_string(serial));
    auto ret = respCb->acknowledgeRequest(serial);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb or ctx is nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForReleasePduSessionId
(
  int32_t serial,
  RIL_Errno errorCode
)
{
  Log::getInstance().d( "[" + mName + "] sendResponseForReleasePduSessionId: serial=" + std::to_string(serial) + " error=" + std::to_string(errorCode));
  RadioResponseInfo responseInfo{ RadioResponseType::SOLICITED, serial, static_cast<RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->releasePduSessionIdResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] respcb is nullptr");
  }
}

void RadioDataServiceImpl::sendStartKeepAliveResponse
(
  RadioResponseInfo responseInfo,
  KeepaliveStatus result
)
{
  Log::getInstance().d( "[" + mName + "] Sending StartKeepAliveRequestMessage Response");
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->startKeepaliveResponse(responseInfo, result);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
        Log::getInstance().d( "[" + mName + "] Response Callback is Nullptr");
  }
}

void RadioDataServiceImpl::sendStopKeepAliveResponse
(
  RadioResponseInfo responseInfo
)
{
  Log::getInstance().d( "[" + mName + "] Sending StopKeepAliveRequestMessage Response");
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->stopKeepaliveResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] Response Callback is Nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForAllocatePduSessionId
(
  int32_t serial,
  RIL_Errno errorCode
)
{
  Log::getInstance().d( "[" + mName + "] sendResponseForAllocatePduSessionId: serial=" + std::to_string(serial) + " error=" + std::to_string(errorCode));
  RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, serial, static_cast<RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if(respCb) {
    auto ret = respCb->allocatePduSessionIdResponse(responseInfo, 0);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] Response Callback is Nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForCancelHandover
(
  int32_t serial,
  RIL_Errno errorCode
)
{
  Log::getInstance().d( "[" + mName + "] sendResponseForCancelHandover: serial=" + std::to_string(serial) + " error=" + std::to_string(errorCode));
  RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, serial, static_cast<RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->cancelHandoverResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] Response Callback is Nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForGetSlicingConfig
(
  RadioResponseInfo respInfo,
  rildata::SlicingConfig_t slicingConfig
)
{
  Log::getInstance().d( "[" + mName + "] sendResponseForGetSlicingConfig: serial=" + std::to_string(respInfo.serial)
                        + " error=" + std::to_string((int)respInfo.error));
  RadioResponseInfo responseInfo{RadioResponseType::SOLICITED, respInfo.serial, respInfo.error };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    SlicingConfig config = utils::convertToAidlSlicingConfig(slicingConfig);
    wakeLockUtils::grabPartialWakeLock();
    auto ret = respCb->getSlicingConfigResponse(responseInfo, config);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : "+ ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] Response Callback is Nullptr");
  }
}

void RadioDataServiceImpl::sendResponseForStartHandover
(
  int32_t serial,
  RIL_Errno errorCode
)
{
  Log::getInstance().d( "[" + mName + "] sendResponseForStartHandover: serial=" + std::to_string(serial) + " error=" + std::to_string(errorCode));
  RadioResponseInfo responseInfo{ RadioResponseType::SOLICITED, serial, static_cast<RadioError>(errorCode) };
  auto respCb = this->getResponseCallback();
  if (respCb) {
    auto ret = respCb->startHandoverResponse(responseInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send response. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] Response Callback is Nullptr");
  }
}
//Indication APIs
void RadioDataServiceImpl::sendRadioDataCallListChangeInd
(
  std::shared_ptr<rildata::RadioDataCallListChangeIndMessage> msg
)
{
  Log::getInstance().d( "[" + mName + "] Sending RadioDataCallListChangeInd " + msg->dump());
  auto indCb = this->getIndicationCallback();
  if (msg == nullptr) {
    Log::getInstance().d( "[" + mName + "] msg is nullptr");
    return;
  }
  if(indCb) {
    Log::getInstance().d( "[" + mName + "] Handling " + msg->dump());
    std::vector<rildata::DataCallResult_t> dcList = msg->getDCList();
    std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> dcResultList;
    Log::getInstance().d("dcList " + std::to_string(dcList.size()));
    dcResultList.resize(dcList.size());
    int i=0;
    for (rildata::DataCallResult_t entry : dcList)
    {
      dcResultList[i] = utils::convertDcResultToAidlDcResult(entry);
      i++;
    }
    wakeLockUtils::grabPartialWakeLock();
    auto ret = indCb->dataCallListChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, dcResultList);
    if (!ret.isOk()) {
    Log::getInstance().d( "[" + mName + "] Unable to send indication. Exception : " + ret.getDescription());
    }
  }
}

void RadioDataServiceImpl::sendUnthrottleApnMessage
(
  std::shared_ptr<rildata::ThrottledApnTimerExpirationMessage> msg
)
{
  Log::getInstance().d( "[" + mName + "] Sending sendUnthrottleApnMessage " + msg->dump());
  auto indCb = this->getIndicationCallback();
  if (msg == nullptr) {
      Log::getInstance().d( "[" + mName + "] msg is nullptr");
      return;
  }
  if (msg->getSrc() != rildata::RequestSource_t::RADIO) {
      Log::getInstance().d( "[" + mName + "] request is for IWLAN");
      return;
  }
  if(indCb) {
    Log::getInstance().d( "[" + mName + "] UNSOL: unthrottleApn");
    DataProfileInfo profileInfo = utils::convertToAidlDataProfileInfo(msg->getProfileInfo());
    wakeLockUtils::grabPartialWakeLock();
    auto ret = indCb->unthrottleApn(RadioIndicationType::UNSOLICITED_ACK_EXP, profileInfo);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send indication. Exception : " + ret.getDescription());
    }
  }
}

void RadioDataServiceImpl::sendKeepAliveStatusInd
(
  std::shared_ptr<rildata::RadioKeepAliveStatusIndMessage> msg
)
{
  Log::getInstance().d( "[" + mName + "] sending sendKeepAliveStatusInd");
  auto indCb = this->getIndicationCallback();
  if (msg != nullptr && indCb != nullptr) {
    ::aidl::android::hardware::radio::data::KeepaliveStatus status = {};
    status.sessionHandle = msg->getHandle();
    status.code = (int)(msg->getStatusCode());
    wakeLockUtils::grabPartialWakeLock();
    auto ret = indCb->keepaliveStatus(RadioIndicationType::UNSOLICITED_ACK_EXP, status);
    if (!ret.isOk()) {
      Log::getInstance().d( "[" + mName + "] Unable to send indication. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d( "[" + mName + "] Ind cb is nullptr");
  }
}

void RadioDataServiceImpl::sendRilPCODataInd
(
  std::shared_ptr<rildata::RilPCODataMessage> msg
)
{
  Log::getInstance().d( "[" + mName + "] Sending RilPCODataChangeInd " + msg->dump());
  auto indCb = this->getIndicationCallback();
  if(msg != nullptr && indCb != nullptr) {
    rildata::PcoDataInfo_t pcoData = msg->getPcoDataInfo();
    PcoDataInfo pco = {};
    Log::getInstance().d("pcoData.cid " + std::to_string(pcoData.cid));
    Log::getInstance().d("pcoData.bearerProto " + pcoData.bearerProto);
    Log::getInstance().d("pcoData.pcoId " + std::to_string(pcoData.pcoId));
    Log::getInstance().d("pcoData.contents size " + std::to_string(pcoData.contents.size()));
    for (const auto &content : pcoData.contents) {
      Log::getInstance().d("pcoData.content " + std::to_string(content));
    }
    pco.cid = pcoData.cid;
    pco.bearerProto = pcoData.bearerProto;
    pco.pcoId = pcoData.pcoId;
    pco.contents = pcoData.contents;
    wakeLockUtils::grabPartialWakeLock();
    auto ret = indCb->pcoData(RadioIndicationType::UNSOLICITED_ACK_EXP, pco);
    if (!ret.isOk()) {
      Log::getInstance().d("[" + mName + "] Unable to send indication. Exception : " + ret.getDescription());
    }
  } else {
    Log::getInstance().d("[" + mName + "] Indication Callback is Nullptr");
  }
}

void RadioDataServiceImpl::sendSlicingConfigChange
(
  std::shared_ptr<rildata::SlicingConfigChangedIndMessage> msg
)
{
  Log::getInstance().d("[" + mName + "] sending slicingConfigChangeInd " + msg->dump());
  auto indCb = this->getIndicationCallback();
  if(msg != nullptr && indCb != nullptr){
    SlicingConfig config{};
    if(msg->getSlicingConfig().has_value())
      config = utils::convertToAidlSlicingConfig(*(msg->getSlicingConfig()));

    wakeLockUtils::grabPartialWakeLock();
    auto ret  = indCb->slicingConfigChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, config);
    if (!ret.isOk()) {
      Log::getInstance().d("[" + mName + "] Unable to send indication. Exception : "+ ret.getDescription());
    }
  } else {
    Log::getInstance().d("[" + mName + "] Indication Callback is nullptr");
  }
}
