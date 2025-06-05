/******************************************************************************
#  Copyright (c) 2021-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "IWLAN"
#include "IWlanServiceBase.h"
#include "RadioServiceUtils.h"
#include "MessageCommon.h"
#include "QtiMutex.h"
#include "DataModule.h"

#include "GetAllQualifiedNetworkRequestMessage.h"
#include "GetIWlanDataRegistrationStateRequestMessage.h"
#include "GetIWlanDataCallListRequestMessage.h"
#include "DeactivateDataCallRequestMessage.h"
#include "IWLANCapabilityHandshake.h"
#include "request/SetupDataCallRequestMessage.h"


#define IWLAN_WAKE_LOCK_NSECS 200000000

using namespace ::aidl::vendor::qti::hardware::data::iwlandata;
using namespace ::aidl::android::hardware::radio::data;
using namespace ::aidl::android::hardware::radio;
using ::aidl::android::hardware::radio::network::RegState;

std::mutex IWlanServiceBase::wakelockMutex;
uint8_t IWlanServiceBase::wakelockRefCount = 0;
timer_t IWlanServiceBase::wakelockTimerId = NULL;
std::string IWlanServiceBase::iwlanSvcWakelock = "";
AIBinder_DeathRecipient* IWlanServiceBase::mDeathRecipient = nullptr;

void IWlanServiceBase::deathNotifier(){
    clearResponseFunctions();
}

void convertQualifiedNetworksToAIDL(std::vector<rildata::QualifiedNetwork_t> &qnList,
    std::vector<::aidl::vendor::qti::hardware::data::iwlandata::QualifiedNetworks>& qNetworks) {
    qNetworks.resize(qnList.size());
    int i=0;
    for (rildata::QualifiedNetwork_t entry: qnList) {
        qNetworks[i].apnType = static_cast<ApnTypes>(entry.apnType);
        qNetworks[i].networks = entry.network;
        i++;
    }
}

static void deathRecpCallback(void* cookie)
{
  IWlanServiceBase* impl = static_cast<IWlanServiceBase*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier();
  }
}

void IWlanServiceBase::acquireWakeLock() {
#ifndef QMI_RIL_UTF
    wakelockMutex.lock();
    if (wakelockRefCount == 0) {
        if (acquire_wake_lock(PARTIAL_WAKE_LOCK, iwlanSvcWakelock.c_str()) < 0)
        {
            QCRIL_LOG_ERROR("%s: failed to acquire wake lock [%d:%s]",
                             __func__, errno, strerror(errno));
            wakelockMutex.unlock();
            return;

        }
    }
    //Create a new timer if required

    struct sigevent sigev;
    struct itimerspec itimers;

    memset(&sigev, 0, sizeof(struct sigevent));
    memset(&itimers, 0,sizeof(struct itimerspec));

    sigev.sigev_notify            = SIGEV_THREAD;
    sigev.sigev_notify_attributes = NULL;
    sigev.sigev_value.sival_ptr   = NULL;
    sigev.sigev_notify_function   = wakeLockTimeoutHdlr;
    if (wakelockTimerId == NULL) {
        if (-1 == timer_create(CLOCK_MONOTONIC, &sigev, &wakelockTimerId) )
        {
            QCRIL_LOG_ERROR( "failed to create wakelock timer ");
            wakelockMutex.unlock();
            return;
        }
        else
        {
            QCRIL_LOG_DEBUG( "wakelock timer creation success:");
        }
    }

    itimers.it_value.tv_sec     = 0;
    itimers.it_value.tv_nsec    = IWLAN_WAKE_LOCK_NSECS;
    itimers.it_interval.tv_sec  = 0;
    itimers.it_interval.tv_nsec = 0;

    // Start the timer, value gets overwritten if timer had already been started
    if (-1 == timer_settime(wakelockTimerId, 0, &itimers, NULL))
    {
        QCRIL_LOG_ERROR( "failed to start timer for timer_id , deleting... ");
        timer_delete(wakelockTimerId);
        wakelockTimerId = NULL;
        wakelockMutex.unlock();
        return;
    }
    else
    {
        wakelockRefCount++;
        wakelockMutex.unlock();
    }
#endif
}

void IWlanServiceBase::releaseWakeLock() {
#ifndef QMI_RIL_UTF
    wakelockMutex.lock();
    if (wakelockRefCount > 0) {
        wakelockRefCount--;
        if (wakelockRefCount == 0) {
            QCRIL_LOG_DEBUG( "ref count is 0, releasing wakelock");
            timer_delete(wakelockTimerId);
            wakelockTimerId = NULL;
            if (release_wake_lock(iwlanSvcWakelock.c_str()) < 0)
            {
                QCRIL_LOG_ERROR("%s: release wakelock %s failed. ",__func__,iwlanSvcWakelock.c_str() );
            }
        }
    }
    wakelockMutex.unlock();
#endif
}

void IWlanServiceBase::wakeLockTimeoutHdlr(union sigval sval) {
    std::ignore = sval;
    QCRIL_LOG_DEBUG( "wakelock timeout called, releasing wakelock");
    IWlanServiceBase::resetWakeLock();

}

void IWlanServiceBase::resetWakeLock() {
    wakelockMutex.lock();
    if (wakelockRefCount > 0) {
        wakelockRefCount = 0;
        timer_delete(wakelockTimerId);
        wakelockTimerId = NULL;
        if (release_wake_lock(iwlanSvcWakelock.c_str()) < 0)
        {
            QCRIL_LOG_ERROR("%s: release wakelock %s failed. ",__func__,iwlanSvcWakelock.c_str() );
        }
    }
    wakelockMutex.unlock();
}

void IWlanServiceBase::clearResponseFunctions() {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mIWlanResponse = NULL;
    mIWlanIndication = NULL;
    IWlanServiceBase::resetWakeLock();
}

std::shared_ptr<IIWlanResponse> IWlanServiceBase::getIWlanResponse() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mIWlanResponse;
}

std::shared_ptr<IIWlanIndication> IWlanServiceBase::getIWlanIndication() {
    std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    return mIWlanIndication;
}

void IWlanServiceBase::acknowledgeRequest(int32_t serial) {
    QCRIL_LOG_ERROR("acknowledgeRequest %d", serial);
    auto responseCb = getIWlanResponse();
    if (responseCb != NULL) {
        acquireWakeLock();
        auto retStatus = responseCb->acknowledgeRequest(serial);
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send acknowledgeRequest");
        }
    }
    else {
        QCRIL_LOG_ERROR("Response cb ptr is NULL");
    }
}

void IWlanServiceBase::onDeactivateDataCallIWlanResponseIndMessage(ResponseError_t rsp, int32_t serial, Message::Callback::Status status) {
    QCRIL_LOG_DEBUG("IWLAN DeactivateDataCall response indication");
    auto responseCb = getIWlanResponse();
    if (responseCb != NULL) {
        acquireWakeLock();
        IWlanResponseInfo responseInfo = {};
        RadioError e = utils::convertMsgToRadioError(status, rsp);
        responseInfo = {.serial = serial, .error = e};
        QCRIL_LOG_DEBUG("IWLAN deactivate data call cb invoked serial %d error %d", responseInfo.serial, responseInfo.error);
        auto retStatus =
            responseCb->deactivateDataCallResponse(responseInfo);
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send deactivateDataCall response.");
        }
    }else {
        QCRIL_LOG_ERROR("%s: IWlanService response cb is NULL", __FUNCTION__);
    }
}

void IWlanServiceBase::onSetupDataCallIWlanResponseIndMessage(SetupDataCallResponse_t rsp,
                                          int32_t serial, Message::Callback::Status status) {
    QCRIL_LOG_DEBUG("IWLAN SetupDataCall response indication");
    auto responseCb = getIWlanResponse();
    if (responseCb != NULL) {
        acquireWakeLock();
        SetupDataCallResult dcResult = {};
        RadioResponseInfo responseInfo = {};
        RadioError e = utils::convertMsgToRadioError(status, rsp.respErr);
        QCRIL_LOG_DEBUG("status %d respErr %d", status, rsp.respErr);
        QCRIL_LOG_DEBUG("cause = %d", rsp.call.cause);
        QCRIL_LOG_DEBUG("suggestedRetryTime = %d", rsp.call.suggestedRetryTime);
        QCRIL_LOG_DEBUG("cid = %d", rsp.call.cid);
        QCRIL_LOG_DEBUG("active = %d", rsp.call.active);
        QCRIL_LOG_DEBUG("type = %s", rsp.call.type.c_str());
        QCRIL_LOG_DEBUG("ifname = %s", rsp.call.ifname.c_str());
        QCRIL_LOG_DEBUG("addresses = %s", rsp.call.addresses.c_str());
        QCRIL_LOG_DEBUG("dnses = %s", rsp.call.dnses.c_str());
        QCRIL_LOG_DEBUG("gateways = %s", rsp.call.gateways.c_str());
        QCRIL_LOG_DEBUG("pcscf = %s", rsp.call.pcscf.c_str());
        QCRIL_LOG_DEBUG("mtuV4 = %d", rsp.call.mtuV4);
        QCRIL_LOG_DEBUG("mtuV6 = %d", rsp.call.mtuV6);
        dcResult = utils::convertDcResultToAidlDcResult(rsp.call);
        responseInfo = {.serial = serial, .error = e};
        QCRIL_LOG_DEBUG("setup data call cb invoked serial %d error %d",
                                  responseInfo.serial, responseInfo.error);
        auto retStatus =
                responseCb->setupDataCallResponse(responseInfo, dcResult);
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send setupDataCall response");
        }
    }else {
        QCRIL_LOG_ERROR("%s: IWlanService response cb is NULL", __FUNCTION__);
    }
}

void IWlanServiceBase::onQualifiedNetworksChange(std::vector<QualifiedNetwork_t> qnList) {
    QCRIL_LOG_DEBUG("IWLAN Qualified Networks Change indication");
    auto indicationCb = getIWlanIndication();
    if (indicationCb != NULL) {
        acquireWakeLock();
        std::vector<QualifiedNetworks> qNetworks;
        convertQualifiedNetworksToAIDL(qnList, qNetworks);
        auto retStatus =
             indicationCb->qualifiedNetworksChangeIndication(qNetworks);
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send QualifiedNetworksChange ind");
        }
    }else {
        QCRIL_LOG_ERROR("%s: IWlanService ind cb is NULL", __FUNCTION__);
    }
}

void IWlanServiceBase::onUnthrottleApn(rildata::DataProfileInfo_t profile) {
    QCRIL_LOG_DEBUG("IWLAN unthrottleApn indication");
    auto indicationCb = getIWlanIndication();
    if (indicationCb != NULL) {
        acquireWakeLock();
        DataProfileInfo profileInfo = utils::convertToAidlDataProfileInfo(profile);
        auto retStatus = indicationCb->unthrottleApn(RadioIndicationType::UNSOLICITED_ACK_EXP, profileInfo);
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send unthrottleApn ind. Exception");
        }
    }else {
        QCRIL_LOG_ERROR("%s: IWlanService ind cb is NULL", __FUNCTION__);
    }
}

void IWlanServiceBase::onDataCallListChange(std::vector<DataCallResult_t> dcList) {
    QCRIL_LOG_DEBUG("IWLAN Data Call List Change indication");
    auto indicationCb = getIWlanIndication();
    if (indicationCb != NULL) {
        acquireWakeLock();
        std::vector<SetupDataCallResult> dcResultList;
        dcResultList.resize(dcList.size());
        int i=0;
        for (DataCallResult_t entry: dcList) {
            dcResultList[i++] = utils::convertDcResultToAidlDcResult(entry);
        }
        auto retStatus =
             indicationCb->dataCallListChanged(RadioIndicationType::UNSOLICITED_ACK_EXP, dcResultList);
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send dataCallListChange ind");
        }
    } else {
        QCRIL_LOG_ERROR("%s: IWlanService ind cb is NULL", __FUNCTION__);
    }
}

void IWlanServiceBase::onDataRegistrationStateChange() {
    QCRIL_LOG_DEBUG("IWLAN Data Registration State Change indication");
    auto indicationCb = getIWlanIndication();
    if (indicationCb != NULL) {
        acquireWakeLock();
        auto retStatus =
             indicationCb->dataRegistrationStateChangeIndication();
        if (!retStatus.isOk()) {
            QCRIL_LOG_ERROR("Unable to send dataRegistrationStateChange ind");
        }
    } else {
        QCRIL_LOG_ERROR("%s: IWlanService ind cb is NULL", __FUNCTION__);
    }
}

::ndk::ScopedAStatus IWlanServiceBase::iwlanDisabled() {
    auto msg = std::make_shared<IWLANCapabilityHandshake>(false);
    msg->dispatch();
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::responseAcknowledgement() {
    releaseWakeLock();
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::getAllQualifiedNetworks(int32_t serial) {
    auto msg = std::make_shared<GetAllQualifiedNetworkRequestMessage>();
    GenericCallback<QualifiedNetworkResult_t> cb(
        ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<QualifiedNetworkResult_t> responseDataPtr) -> void {
            if (solicitedMsg && responseDataPtr) {
                RadioError e = utils::convertMsgToRadioError(status, responseDataPtr->respErr);
                IWlanResponseInfo responseInfo = {
                    .serial = serial, .error = e};
                QCRIL_LOG_DEBUG("getAllQualifiedNetworks cb invoked status %d respErr %d", status, responseDataPtr->respErr);

                std::vector<QualifiedNetworks> qNetworks;
                convertQualifiedNetworksToAIDL(responseDataPtr->qualifiedNetwork, qNetworks);

                auto responseCb = getIWlanResponse();
                if (responseCb != NULL) {
                    auto retStatus =
                        responseCb->getAllQualifiedNetworksResponse(responseInfo, qNetworks);
                    if (!retStatus.isOk()) {
                        QCRIL_LOG_ERROR("Unable to send qualifiedNetworks response");
                    }
                } else {
                    QCRIL_LOG_ERROR("%s: IWlanService resp cb is NULL", __FUNCTION__);
                }

            }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::getDataRegistrationState(int32_t serial) {
    auto msg = std::make_shared<GetIWlanDataRegistrationStateRequestMessage>();
    GenericCallback<IWlanDataRegistrationStateResult_t> cb(
        ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<IWlanDataRegistrationStateResult_t> responseDataPtr) -> void {
            if (solicitedMsg && responseDataPtr) {
                RadioError e = utils::convertMsgToRadioError(status, responseDataPtr->respErr);
                IWlanResponseInfo responseInfo = {
                    .serial = serial, .error = e};
                QCRIL_LOG_DEBUG("getDataRegistrationState cb invoked status %d respErr %d", status, responseDataPtr->respErr);

                IWlanDataRegStateResult regResult = {
                    .regState = static_cast<RegState>(responseDataPtr->regState),
                    .reasonForDenial = responseDataPtr->reasonForDenial};

                auto responseCb = getIWlanResponse();
                if (responseCb != NULL) {
                    auto retStatus =
                        responseCb->getDataRegistrationStateResponse(responseInfo, regResult);
                    if (!retStatus.isOk()) {
                        QCRIL_LOG_ERROR("Unable to send data reg state response");
                    }
                } else {
                    QCRIL_LOG_ERROR("%s: IWlanService resp cb is NULL", __FUNCTION__);
                }
            }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::getDataCallList(int32_t serial) {

    auto msg = std::make_shared<GetIWlanDataCallListRequestMessage>();

    GenericCallback<DataCallListResult_t> cb(
        ([this, serial](std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
                        std::shared_ptr<DataCallListResult_t> responseDataPtr) -> void {
            if (solicitedMsg && responseDataPtr) {
                RadioError e = utils::convertMsgToRadioError(status, responseDataPtr->respErr);
                RadioResponseInfo responseInfo = {
                    .serial = serial, .error = e};
                QCRIL_LOG_DEBUG("getIWlanDataCallList cb invoked status %d respErr %d", status, responseDataPtr->respErr);
                QCRIL_LOG_DEBUG("Call list size = %d", responseDataPtr->call.size());
                for(size_t i=0;i<responseDataPtr->call.size();i++) {
                    DataCallResult_t *callinst = &(responseDataPtr->call[i]);
                    QCRIL_LOG_DEBUG("cid %d", callinst->cid);
                    QCRIL_LOG_DEBUG("cause %d", callinst->cause);
                    QCRIL_LOG_DEBUG("suggestedRetryTime %ld", callinst->suggestedRetryTime);
                    QCRIL_LOG_DEBUG("type %s", callinst->type.c_str());
                    QCRIL_LOG_DEBUG("ifname %s", callinst->ifname.c_str());
                    QCRIL_LOG_DEBUG("addresses %s", callinst->addresses.c_str());
                    QCRIL_LOG_DEBUG("dnses %s", callinst->dnses.c_str());
                    QCRIL_LOG_DEBUG("gateways %s", callinst->gateways.c_str());
                    QCRIL_LOG_DEBUG("pcscf %s", callinst->pcscf.c_str());
                    QCRIL_LOG_DEBUG("mtuV4 %d", callinst->mtuV4);
                    QCRIL_LOG_DEBUG("mtuV6 %d", callinst->mtuV6);
                    QCRIL_LOG_DEBUG("tdSize %d", callinst->trafficDescriptors.size());
                }

                std::vector<SetupDataCallResult> dcResultList;
                dcResultList.resize(responseDataPtr->call.size());
                int i=0;
                for (DataCallResult_t entry: responseDataPtr->call) {
                    dcResultList[i++] = utils::convertDcResultToAidlDcResult(entry);
                }

                auto responseCb = getIWlanResponse();
                if (responseCb != NULL) {
                    auto retStatus =
                        responseCb->getDataCallListResponse(responseInfo, dcResultList);
                    if (!retStatus.isOk()) {
                        QCRIL_LOG_ERROR("Unable to send getDataCallList response");
                    }
                } else {
                    QCRIL_LOG_ERROR("%s: IWlanService resp cb is NULL", __FUNCTION__);
                }

            }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::deactivateDataCall(int32_t serial, int32_t cid, DataRequestReason reason) {
   auto msg =
        std::make_shared<DeactivateDataCallRequestMessage>(
            serial,
            cid,
            static_cast<DataRequestReason_t>(reason),
            std::make_shared<std::function<void(int32_t)>>(std::bind(&IWlanServiceBase::acknowledgeRequest, this, std::placeholders::_1)));
    if (msg) {
        GenericCallback<ResponseError_t> cb([serial](std::shared_ptr<Message>,
                                                     Message::Callback::Status status,
                                                     std::shared_ptr<ResponseError_t> rsp) -> void {
            if (rsp != nullptr) {
                auto indMsg = std::make_shared<DeactivateDataCallIWlanResponseIndMessage>(*rsp, serial, status);
                indMsg->broadcast();
            } else {
                QCRIL_LOG_ERROR("IWLAN deactivateDataCall resp is nullptr");
            }
        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::setupDataCall(int32_t serial,
                                   AccessNetwork /*accessNetwork*/,
                                   const DataProfileInfo& dataProfileInfo,
                                   bool roamingAllowed,
                                   DataRequestReason reason,
                                   const std::vector<LinkAddress>& addresses,
                                   const std::vector<std::string>& dnses,
                                   int32_t /*in_pduSessionId*/,
                                   const std::optional<SliceInfo>& /*in_sliceInfo*/,
                                   bool matchAllRuleAllowed) {
    QCRIL_LOG_DEBUG("IWLAN setupDataCall");
    std::vector<std::string> iwlanAddresses;
    std::vector<std::string> iwlanDnses(dnses);
    rildata::DataProfileInfo_t profileInfo = utils::convertAidlDataProfileInfoToRil(dataProfileInfo);

    for (size_t i = 0 ; i < addresses.size(); i++ ) {
      iwlanAddresses.push_back(addresses[i].address);
    }
    auto msg = std::make_shared<SetupDataCallRequestMessage_1_6>(
        serial,
        RequestSource_t::IWLAN,
        AccessNetwork_t::IWLAN,
        profileInfo,
        roamingAllowed,
        static_cast<DataRequestReason_t>(reason),
        iwlanAddresses,
        iwlanDnses,
        matchAllRuleAllowed,
        std::make_shared<std::function<void(int32_t)>>(std::bind(&IWlanServiceBase::acknowledgeRequest, this, std::placeholders::_1)));

    if (msg) {
      if (profileInfo.trafficDescriptor.has_value()) {
        msg->setOptionalTrafficDescriptor(*profileInfo.trafficDescriptor);
      }

      GenericCallback<SetupDataCallResponse_t> cb([serial](std::shared_ptr<Message>,
                                                          Message::Callback::Status status,
                                                          std::shared_ptr<SetupDataCallResponse_t> rsp) -> void {
          if (rsp != nullptr) {
              auto indMsg = std::make_shared<SetupDataCallIWlanResponseIndMessage>(*rsp, serial, status);
              indMsg->broadcast();
          } else {
              QCRIL_LOG_ERROR("setupDataCall resp is nullptr");
          }
      });
      msg->setCallback(&cb);
      msg->dispatch();
    }
    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IWlanServiceBase::setResponseFunctions
                (const std::shared_ptr<IIWlanResponse>& iwlanResponse,
                 const std::shared_ptr<IIWlanIndication>& iwlanIndication) {

    QCRIL_LOG_INFO("IWlanServiceBase::setResponseFunctions");
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    #ifndef QMI_RIL_UTF
    if (mIWlanResponse) {
      AIBinder_unlinkToDeath(mIWlanResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      mIWlanResponse = nullptr;
    }
    #endif

    QCRIL_LOG_DEBUG("IWlanServiceBase::setResponseFunctions. iwlanResponseParam: %s. iwlanIndicationParam: %s",
            iwlanResponse ? "<not null>" : "<null>",
            iwlanIndication ? "<not null>" : "<null>");
    mIWlanResponse = iwlanResponse;
    mIWlanIndication = iwlanIndication;
    #ifndef QMI_RIL_UTF
    if (mIWlanResponse != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mIWlanResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
    #endif

    //Since at least one of the services has been created, which means QTI
    //IWLAN is being used and we need to tell modem to turn on AP assist mode
    //This is for initial handshake

    auto msg =
        std::make_shared<IWLANCapabilityHandshake>(true);

    if (msg) {
          GenericCallback<ModemIWLANCapability_t> cb([this](std::shared_ptr<Message> msg,
                                                       Message::Callback::Status status,
                                                       std::shared_ptr<ModemIWLANCapability_t> rsp) -> void {
              if (msg && rsp) {

                  QCRIL_LOG_DEBUG("IWLANCapabilityHandshake cb invoked status %d ", status);

                  auto indicationCb = getIWlanIndication();
                  if (indicationCb != NULL) {
                      if ((status != Message::Callback::Status::SUCCESS) ||
                          (*rsp == ModemIWLANCapability_t::not_present)) {
                          auto retStatus = indicationCb->modemSupportNotPresent();
                          if (!retStatus.isOk()) {
                              QCRIL_LOG_ERROR("Unable to send support notification ind. Exception");
                          }
                      }
                  } else {
                      QCRIL_LOG_ERROR("%s: IWlanService ind cb is NULL", __FUNCTION__);
                  }
              }

        });
        msg->setCallback(&cb);
        msg->dispatch();
    }
    return ndk::ScopedAStatus::ok();
}
