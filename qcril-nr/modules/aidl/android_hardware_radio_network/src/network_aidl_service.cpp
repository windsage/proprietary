/******************************************************************************
  @file    network_aidl_service.cpp
  @brief   network_aidl_service

  DESCRIPTION
    Implements the server side of the IRadioNetwork interface. Handles RIL
    requests and responses to be received and sent to client respectively

  ---------------------------------------------------------------------------
  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <inttypes.h>
#include "aidl/android/hardware/radio/network/IndicationFilter.h"
#include "framework/Log.h"
#include "network_aidl_service.h"
#include "wake_lock_utils.h"
#include "network_aidl_service_utils.h"
#include "interfaces/nas/RilRequestGetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestQueryAvailBandModeMessage.h"
#include "interfaces/nas/RilRequestQueryAvailNetworkMessage.h"
#include "interfaces/nas/RilRequestSetPrefNetworkTypeMessage.h"
#include "interfaces/nas/RilRequestSetBandModeMessage.h"
#include "interfaces/nas/RilRequestSetSysSelChannelsMessage.h"
#include "interfaces/nas/RilRequestStartNetworkScanMessage.h"
#include "interfaces/nas/RilRequestStopNetworkScanMessage.h"
#include "interfaces/voice/QcRilRequestSetCallBarringPasswordMessage.h"
#include "interfaces/voice/QcRilRequestSetSuppSvcNotificationMessage.h"
#include "interfaces/uim/UimEnterDePersoRequestMsg.h"
#include "interfaces/cellinfo/RilRequestGetCellInfoListMessage.h"
#include "interfaces/nas/RilRequestGetBarringInfoMessage.h"
#include "interfaces/nas/RilRequestQueryCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestGetDataRegistrationMessage.h"
#include "interfaces/nas/RilRequestGetSignalStrengthMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRadioTechMessage.h"
#include "interfaces/nas/RilRequestGetVoiceRegistrationMessage.h"
#include "interfaces/nas/RilRequestSetCdmaRoamingPrefMessage.h"
#include "interfaces/nas/RilRequestSetUnsolCellInfoListRateMessage.h"
#include "interfaces/nas/NasSetSignalStrengthCriteriaAllRans.h"
#include "interfaces/nas/RilRequestGetUsageSettingMessage.h"
#include "interfaces/nas/RilRequestSetUsageSettingMessage.h"
#include "interfaces/nas/RilRequestOperatorMessage.h"
#include "interfaces/nas/RilRequestQueryNetworkSelectModeMessage.h"
#include "interfaces/sms/RilRequestGetImsRegistrationMessage.h"
#include "interfaces/nas/RilRequestGetSysSelChannelsMessage.h"
#include "interfaces/nas/RilRequestSetUnsolRespFilterMessage.h"
#include "interfaces/nas/NasEnablePhysChanConfigReporting.h"
#include "interfaces/nas/NasPhysChanConfigMessage.h"
#include "request/SetLinkCapRptCriteriaMessage.h"
#include "UnSolMessages/DataRegistrationFailureCauseMessage.h"
#include "utils/SystemClock.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionManualMessage.h"
#include "interfaces/nas/RilRequestSetNetworkSelectionAutoMessage.h"
#include "interfaces/nas/RilRequestSetLocationUpdateMessage.h"

using namespace qti::aidl::android::hardware::radio::network::utils;

#undef TAG
#define TAG "RILQ"

/* For sending the NITZ update, if the NAS clinet connected late */
static std::shared_ptr<RilUnsolNitzTimeReceivedMessage> s_lastNITZTimeMsg = nullptr;

void IRadioNetworkImpl::setResponseFunctions_nolock(
    const std::shared_ptr<aidlnetwork::IRadioNetworkResponse>& radioNetworkResponse,
    const std::shared_ptr<aidlnetwork::IRadioNetworkIndication>& radioNetworkIndication) {
  mIRadioNetworkResponse = radioNetworkResponse;
  mIRadioNetworkIndication = radioNetworkIndication;
}

void IRadioNetworkImpl::deathNotifier(void* /*cookie*/) {
  QCRIL_LOG_DEBUG("IRadioNetworkImpl: Client died, cleaning up callbacks");
  clearCallbacks();
}

static void deathRecpCallback(void* cookie) {
  IRadioNetworkImpl* impl = static_cast<IRadioNetworkImpl*>(cookie);
  if (impl != nullptr) {
    impl->deathNotifier(cookie);
  }
}

static void resendLastNITZTimeData() {
  if (s_lastNITZTimeMsg) {
    Dispatcher::getInstance().dispatchSync(s_lastNITZTimeMsg);
    s_lastNITZTimeMsg = nullptr;
  }
}

/*
 *   @brief
 *   Registers the callback for IRadioNetwork using the
 *   IRadioNetworkResponse and IRadioNetworkIndication objects
 *   being passed in by the client as parameters
 *
 */
::ndk::ScopedAStatus IRadioNetworkImpl::setResponseFunctions(
    const std::shared_ptr<aidlnetwork::IRadioNetworkResponse>& in_radioNetworkResponse,
    const std::shared_ptr<aidlnetwork::IRadioNetworkIndication>& in_radioNetworkIndication) {
  QCRIL_LOG_INFO("IRadioNetworkImpl::setResponseFunctions: Set client callback");

  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);

    if (mIRadioNetworkResponse != nullptr) {
      AIBinder_unlinkToDeath(mIRadioNetworkResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
    }

    setResponseFunctions_nolock(in_radioNetworkResponse, in_radioNetworkIndication);

    if (mIRadioNetworkResponse != nullptr) {
      AIBinder_DeathRecipient_delete(mDeathRecipient);
      mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
      if (mDeathRecipient != nullptr) {
        AIBinder_linkToDeath(mIRadioNetworkResponse->asBinder().get(), mDeathRecipient,
                             reinterpret_cast<void*>(this));
      }
    }
  }

  if(in_radioNetworkIndication != nullptr) {
    resendLastNITZTimeData();
  }
  return ndk::ScopedAStatus::ok();
}

IRadioNetworkImpl::IRadioNetworkImpl(qcril_instance_id_e_type instance) : mInstanceId(instance) {
}

IRadioNetworkImpl::~IRadioNetworkImpl() {
}

void IRadioNetworkImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mIRadioNetworkResponse = nullptr;
    mIRadioNetworkIndication = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}
// sendIndication Functions

void IRadioNetworkImpl::sendCdmaPrlChanged(std::shared_ptr<RilUnsolCdmaPrlChangedMessage> msg) {
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }
  qti::ril::utils::grabPartialWakeLock();
  if (msg) {
    int32_t version = msg->getPrl();
    QCRIL_LOG_DEBUG("UNSOL: cdmaPrlChanged, version= %d", version);
    auto ret = indCb->cdmaPrlChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, version);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioNetworkImpl::sendImsNetworkStateChanged(std::shared_ptr<RilUnsolImsNetworkStateChangedMessage> msg) {
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }
  qti::ril::utils::grabPartialWakeLock();

  if (msg) {
    QCRIL_LOG_DEBUG("UNSOL: imsNetworkStateChanged");
    auto ret = indCb->imsNetworkStateChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioNetworkImpl::sendNetworkScanResult(std::shared_ptr<RilUnsolNetworkScanResultMessage> msg) {
  if(!mIsScanRequested){
      return;
  }
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }

  qti::ril::utils::grabPartialWakeLock();

  if (msg) {
    aidlnetwork::NetworkScanResult aidl_result{};
    aidl_result.status = static_cast<int32_t>(msg->getStatus());
    aidl_result.error = static_cast<aidlradio::RadioError>(msg->getError());
    convert(msg->getNetworkInfo(),aidl_result.networkInfos);
    auto ret = indCb->networkScanResult(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, aidl_result);
    if(aidl_result.status == aidlnetwork::NetworkScanResult::SCAN_STATUS_COMPLETE){
        mIsScanRequested = false;
    }
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioNetworkImpl::sendNetworkStateChanged(std::shared_ptr<RilUnsolNetworkStateChangedMessage> msg) {
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }

  qti::ril::utils::grabPartialWakeLock();

  if (msg) {
    QCRIL_LOG_DEBUG("UNSOL: sendNetworkStateChanged");
    auto ret = indCb->networkStateChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioNetworkImpl::sendSuppSvcNotify(std::shared_ptr<QcRilUnsolSuppSvcNotificationMessage> msg) {
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }

  qti::ril::utils::grabPartialWakeLock();

  if (msg) {
    aidlnetwork::SuppSvcNotification aidl_result{};
    aidl_result = convertRILSuppSvcNotificationToAidl(msg);
    QCRIL_LOG_DEBUG("UNSOL: sendSuppSvcNotify, %s", aidl_result.toString().c_str());
    auto ret = indCb->suppSvcNotify(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, aidl_result);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}

void IRadioNetworkImpl::sendLinkCapInd(std::shared_ptr<rildata::LinkCapIndMessage> msg) {
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto ind = getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind == nullptr) {
    QCRIL_LOG_ERROR("Indcb is nullptr");
    return;
  }
  qti::ril::utils::grabPartialWakeLock();
  rildata::LinkCapEstimate_t &ilce = msg->getParams();
  aidlnetwork::LinkCapacityEstimate lce = {
      .downlinkCapacityKbps = static_cast<int32_t>(ilce.downlinkCapacityKbps),
      .uplinkCapacityKbps = static_cast<int32_t>(ilce.uplinkCapacityKbps) };
  QCRIL_LOG_DEBUG("Sending CURRENT_LINK_CAPACITY_ESTIMATE dl: %d ul: %d",
                    ilce.downlinkCapacityKbps, ilce.uplinkCapacityKbps );
  QCRIL_LOG_DEBUG("UNSOL CurrentLinkCapacityEstimate");
  auto ret = ind->currentLinkCapacityEstimate(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, lce);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    return;
  }
}

void IRadioNetworkImpl::sendPhysicalConfigStructUpdateInd(std::shared_ptr<rildata::PhysicalConfigStructUpdateMessage> msg)
{
  if (msg == nullptr) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }
  auto ind = getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (ind == nullptr) {
    QCRIL_LOG_ERROR("Indcb is nullptr");
    return;
  }
  qti::ril::utils::grabPartialWakeLock();
  std::vector<aidlnetwork::PhysicalChannelConfig> physList;
  auto dataPhysList = msg->getPhysicalConfigStructInfo();
  for (int i = 0; i < dataPhysList.size(); i++) {
    aidlnetwork::PhysicalChannelConfig phys = {};
    phys.status = static_cast<aidlnetwork::CellConnectionStatus>(dataPhysList[i].status);
    phys.cellBandwidthDownlinkKhz = dataPhysList[i].cellBandwidthDownlink;
    phys.rat = convertRadioAccessFamilyTypeToRadioTechnology(dataPhysList[i].rat);
    for (auto cid: dataPhysList[i].contextIds) {
        phys.contextIds.push_back(cid);
    }
    phys.physicalCellId = dataPhysList[i].physicalCellId;
    physList.push_back(phys);
  }
  QCRIL_LOG_DEBUG("UNSOL currentPhysicalChannelConfigs");
  auto ret = ind->currentPhysicalChannelConfigs(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, physList);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    return;
  }
}

void IRadioNetworkImpl::sendNwRegistrationReject(
    std::shared_ptr<RilUnsolNwRegistrationRejectMessage> msg) {
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }

  if (msg) {
    qti::ril::utils::grabPartialWakeLock();

    aidlnetwork::CellIdentity cellIdentity {};
    auto rejInfo = msg->getNwRegistrationRejectInfo();
    convert(rejInfo.rilCellIdentity, cellIdentity);
    std::string plmn(rejInfo.choosenPlmn);

    QCRIL_LOG_DEBUG("UNSOL: sendNwRegistrationReject");
    auto ret =
      indCb->registrationFailed(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, cellIdentity,
                                       plmn, static_cast<int32_t>(rejInfo.domain),
                                       rejInfo.causeCode, rejInfo.additionalCauseCode);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioNetworkImpl::sendRestrictedStateChanged(
    std::shared_ptr<RilUnsolRestrictedStateChangedMessage> msg) {
  auto indCb = getIndicationCallback();
  if (indCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getIndicationCallback Failed");
    return;
  }

  if (msg) {

    // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
    qti::ril::utils::grabPartialWakeLock();

    aidlnetwork::PhoneRestrictedState state =
      static_cast<aidlnetwork::PhoneRestrictedState>(msg->getState());

    QCRIL_LOG_DEBUG("UNSOL: restrictedStateChanged");
    auto ret =
      indCb->restrictedStateChanged(aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, state);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
  }
}

//sendResponse Functions

void IRadioNetworkImpl::sendResponseForGetAllowedNetworkTypesBitmap(int32_t serial,
    RIL_Errno errorCode, std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }


  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  int32_t networkTypeBitmap{1}; // default value = 1 = UNKNOWN RAT

  if (errorCode == RIL_E_SUCCESS && prefResult) {
    networkTypeBitmap = prefResult->pref;
  }

  QCRIL_LOG_DEBUG("getAllowedNetworkTypesBitmapResponse: serial=%d, error=%d, networkTypeBitmap= %d",
      serial, errorCode, networkTypeBitmap);
  auto ret = respCb->getAllowedNetworkTypesBitmapResponse(resp, networkTypeBitmap);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForGetAvailableBandModes(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilQueryAvailBandsResult_t> rilBandMode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }


  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  std::vector<aidlnetwork::RadioBandMode> aidlBandMode{};

  if (errorCode == RIL_E_SUCCESS && rilBandMode) {
    aidlBandMode.resize(rilBandMode->bandList.size());
    for (unsigned int i = 0; i < rilBandMode->bandList.size(); i++) {
      aidlBandMode[i] = static_cast<aidlnetwork::RadioBandMode>(rilBandMode->bandList[i]);
      QCRIL_LOG_DEBUG("aidlBandMode[%d] = %s", i,
          aidlnetwork::toString(aidlBandMode[i]).c_str());
    }
  }

  QCRIL_LOG_DEBUG("getAvailableBandModesResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getAvailableBandModesResponse(resp, aidlBandMode);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForGetAvailableNetworks(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetAvailNetworkResult_t> rilNetworkResult) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }


  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  std::vector<aidlnetwork::OperatorInfo> aidlNetworkInfos{};

  if (errorCode == RIL_E_SUCCESS && rilNetworkResult) {
    aidlNetworkInfos.resize(rilNetworkResult->info.size());
    for (unsigned int i = 0; i < rilNetworkResult->info.size(); i++) {
      aidlNetworkInfos[i] = convertRilNetworkInfotoAidl(rilNetworkResult->info.at(i));
      QCRIL_LOG_DEBUG("aidlNetworkInfos[%d] = %s", i,
          aidlNetworkInfos[i].toString().c_str());
    }
  }

  QCRIL_LOG_DEBUG("getAvailableNetworksResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->getAvailableNetworksResponse(resp, aidlNetworkInfos);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetAllowedNetworkTypesBitmap(int32_t serial,
  RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("SetAllowedNetworkTypesBitmapResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setAllowedNetworkTypesBitmapResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetBandMode(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("SetBandModeResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setBandModeResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetBarringPassword(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("SetBarringPasswordResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setBarringPasswordResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetSuppServiceNotifications(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("sendResponseForSetSuppServiceNotifications: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setSuppServiceNotificationsResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetSystemSelectionChannels(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  if(errorCode == RIL_E_REQUEST_NOT_SUPPORTED) {
    QCRIL_LOG_DEBUG("Convert error code from %d to RIL_E_INTERNAL_ERR", errorCode);
    errorCode = RIL_E_INTERNAL_ERR;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("SetSystemSelectionChannelsResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setSystemSelectionChannelsResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForStartNetworkScan(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("startNetworkScanResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->startNetworkScanResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForStopNetworkScan(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("stopNetworkScanResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->stopNetworkScanResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSupplyNetworkDepersonalization(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }


  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  int32_t remainingRetries{-1};

  if (errorCode == RIL_E_SUCCESS && responseDataPtr) {
    remainingRetries = responseDataPtr->no_of_retries;
  }

  QCRIL_LOG_DEBUG("supplyNetworkDepersonalizationResponse: serial=%d, error=%d, remainingRetries= %d",
      serial, errorCode, remainingRetries);
  auto ret = respCb->supplyNetworkDepersonalizationResponse(resp, remainingRetries);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetSignalStrengthReportingCriteria(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  QCRIL_LOG_DEBUG("setSignalStrengthReportingCriteriaResponse: serial=%d, error=%d",
    serial, errorCode);

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  auto ret = respCb->setSignalStrengthReportingCriteriaResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetUsageSetting(int32_t serial, RIL_Errno errorCode) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }

  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("setUsageSettingResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setUsageSettingResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForGetUsageSetting(int32_t serial,
    RIL_Errno errorCode, std::shared_ptr<qcril::interfaces::RilGetUsageSettingResult_t> usageSettingResult) {
  auto respCb = getResponseCallback();
  if (respCb == nullptr) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo resp {
      aidlradio::RadioResponseType::SOLICITED, serial,
      static_cast<aidlradio::RadioError>(errorCode)};

  aidlnetwork::UsageSetting aidl_usageSetting{};

  if (errorCode == RIL_E_SUCCESS && usageSettingResult) {
    RIL_Errno res = convertRilUsageSettingtoAidlUsageSetting(usageSettingResult->mode, aidl_usageSetting);
    resp = {aidlradio::RadioResponseType::SOLICITED, serial, static_cast<aidlradio::RadioError>(res)};
  }

  QCRIL_LOG_DEBUG("getUsageSettingResponse: serial=%d, error=%d, usageSetting= %s",
      serial, errorCode, aidlnetwork::toString(aidl_usageSetting).c_str());
  auto ret = respCb->getUsageSettingResponse(resp, aidl_usageSetting);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

// Request APIs

::ndk::ScopedAStatus IRadioNetworkImpl::getAllowedNetworkTypesBitmap(int32_t serial) {
  QCRIL_LOG_DEBUG("IRadioNetworkImpl::getAllowedNetworkTypeBitmap: serial=%d", serial);
  auto msg = std::make_shared<RilRequestGetPrefNetworkTypeMessage>(getContext(serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilGetModePrefResult_t> prefResult;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            prefResult =
                std::static_pointer_cast<qcril::interfaces::RilGetModePrefResult_t>(resp->data);
          }
          sendResponseForGetAllowedNetworkTypesBitmap(serial, errorCode, prefResult);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
      sendResponseForGetAllowedNetworkTypesBitmap(serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getAvailableBandModes(int32_t serial) {
  QCRIL_LOG_DEBUG("getAvailableBandModes: serial=%d", serial);
    auto msg = std::make_shared<RilRequestQueryAvailBandModeMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<qcril::interfaces::RilQueryAvailBandsResult_t> rilBandResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilBandResult =
                  std::static_pointer_cast<qcril::interfaces::RilQueryAvailBandsResult_t>(
                      resp->data);
            }
            this->sendResponseForGetAvailableBandModes(serial, errorCode, rilBandResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetAvailableBandModes(serial, RIL_E_NO_MEMORY, nullptr);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getAvailableNetworks(int32_t serial) {
  QCRIL_LOG_DEBUG("getAvailableNetworks: serial=%d", serial);
    auto msg = std::make_shared<RilRequestQueryAvailNetworkMessage>(this->getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<qcril::interfaces::RilGetAvailNetworkResult_t> rilNetworkResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilNetworkResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetAvailNetworkResult_t>(
                      resp->data);
            }
            this->sendResponseForGetAvailableNetworks(serial, errorCode, rilNetworkResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      this->sendResponseForGetAvailableNetworks(serial, RIL_E_NO_MEMORY, nullptr);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setAllowedNetworkTypesBitmap(int32_t serial, int32_t in_networkTypeBitmap) {
  QCRIL_LOG_DEBUG("setAllowedNetworkTypeBitmap: serial=%d, bitMap= %d",
      serial, in_networkTypeBitmap);
  auto msg = std::make_shared<RilRequestSetPrefNetworkTypeMessage>(
    getContext(serial), in_networkTypeBitmap);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            sendResponseForSetAllowedNetworkTypesBitmap(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForSetAllowedNetworkTypesBitmap(serial, RIL_E_NO_MEMORY);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setBandMode(int32_t serial, aidlnetwork::RadioBandMode in_mode) {
  QCRIL_LOG_DEBUG("setBandMode: serial=%d, aidlBandMode= %s",
      serial, aidlnetwork::toString(in_mode).c_str());
  auto msg = std::make_shared<RilRequestSetBandModeMessage>(getContext(serial),
    static_cast<int32_t>(in_mode));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponseForSetBandMode(serial, errorCode);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
      sendResponseForSetBandMode(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setBarringPassword(int32_t serial, const std::string& in_facility, const std::string& in_oldPassword, const std::string& in_newPassword) {
  QCRIL_LOG_DEBUG("setBarringPassword: serial=%d, in_facility=%s", serial, in_facility.c_str());
  if (in_facility.empty()) {
    QCRIL_LOG_ERROR("invalid parameter: in_facility");
    sendResponseForSetBarringPassword(serial, RIL_E_INVALID_ARGUMENTS);
  }
  auto msg = std::make_shared<QcRilRequestSetCallBarringPasswordMessage>(getContext(serial));
  if (msg != nullptr) {
    // Set parameters
    msg->setFacilityType(convertFacilityType(in_facility));
    if (!in_oldPassword.empty()) {
      msg->setOldPassword(in_oldPassword.c_str());
    }
    if (!in_newPassword.empty()) {
      msg->setNewPassword(in_newPassword.c_str());
    }
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponseForSetBarringPassword(serial, errorCode);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
      sendResponseForSetBarringPassword(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setSignalStrengthReportingCriteria(int32_t serial,
    const std::vector<aidlnetwork::SignalThresholdInfo>& signalThresholdInfos) {
  QCRIL_LOG_DEBUG("setSignalStrengthReportingCriteria: serial=%d", serial);
  int size = signalThresholdInfos.size();
  for (int i=0; i < size ; i++) {
    QCRIL_LOG_DEBUG("aidlSigCriteria[%d]= %s", i, signalThresholdInfos[i].toString().c_str());
  }
  RIL_Errno ret = RIL_E_SUCCESS;
  std::vector<qcril::interfaces::SignalStrengthCriteriaEntry> args;
  ret = sanityCheckSignalStrengthCriteriaParams(signalThresholdInfos);
  if (ret == RIL_E_SUCCESS) {
    args = fillInSignalStrengthCriteria(signalThresholdInfos);
    auto msg = std::make_shared<NasSetSignalStrengthCriteriaAllRans>(getContext(serial), args);
    if (msg != nullptr) {
      GenericCallback<RIL_Errno> cb([this, serial](std::shared_ptr<Message> /*msg*/,
                                                   Message::Callback::Status status,
                                                   std::shared_ptr<RIL_Errno> rsp) -> void {
        RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
        if (status == Message::Callback::Status::SUCCESS && rsp) {
          errorCode = *rsp;
        }
        sendResponseForSetSignalStrengthReportingCriteria(
            serial, errorCode);
      });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      ret = RIL_E_NO_MEMORY;
    }
  } else {
    ret = RIL_E_INVALID_ARGUMENTS;
  }
  if (ret != RIL_E_SUCCESS) {
    sendResponseForSetSignalStrengthReportingCriteria(serial, ret);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setSuppServiceNotifications(int32_t serial, bool in_enable) {
  QCRIL_LOG_DEBUG("setSuppServiceNotifications: serial=%d, in_enable= %s",
      serial, in_enable ? "true":"false");
  auto msg = std::make_shared<QcRilRequestSetSuppSvcNotificationMessage>(getContext(serial));
  if (msg !=nullptr) {
    // Set parameters
    msg->setStatus(in_enable);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
          }
          sendResponseForSetSuppServiceNotifications(serial, errorCode);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
      sendResponseForSetSuppServiceNotifications(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setSystemSelectionChannels(int32_t serial, bool in_specifyChannels, const std::vector<aidlnetwork::RadioAccessSpecifier>& in_specifiers) {
  QCRIL_LOG_DEBUG("setSystemSelectionChannels: serial=%d, channel info valid = %d", serial, in_specifyChannels);
  RIL_SysSelChannels ril_info{};
  memset(&ril_info, 0, sizeof(ril_info));
  RIL_Errno res = RIL_E_SUCCESS;
  // If specifyChannels is true, it only scans bands specified in specifiers.
  // If specifyChannels is false, it scans all bands.
  if (in_specifyChannels == true) {
    for (size_t i = 0; i < in_specifiers.size(); ++i) {
      QCRIL_LOG_DEBUG( "RAS %d: %s", i, in_specifiers[i].toString().c_str());
    }
    res = fillSetSystemSelectionChannelRequest(in_specifiers, ril_info);
  }

  if (res != RIL_E_SUCCESS) {
    sendResponseForSetSystemSelectionChannels(serial, res);
    return ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<RilRequestSetSysSelChannelsMessage>(getContext(serial), ril_info);

  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
      [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponseForSetSystemSelectionChannels(serial, errorCode);
    });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForSetSystemSelectionChannels(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::startNetworkScan(int32_t serial, const aidlnetwork::NetworkScanRequest& request) {
  QCRIL_LOG_DEBUG("startNetworkScan: serial=%d, %s",
      serial, request.toString().c_str());

  mIsScanRequested = true;
  RIL_NetworkScanRequest scanRequest{};
  RIL_Errno res = fillNetworkScanRequest(request, scanRequest);

  if (res != RIL_E_SUCCESS) {
    sendResponseForStartNetworkScan(serial, res);
    return ndk::ScopedAStatus::ok();
  }

  auto msg = std::make_shared<RilRequestStartNetworkScanMessage>(getContext(serial),scanRequest);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          sendResponseForStartNetworkScan(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForStartNetworkScan(serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::stopNetworkScan(int32_t serial) {
  QCRIL_LOG_DEBUG("stopNetworkScan: serial=%d", serial);
    auto msg = std::make_shared<RilRequestStopNetworkScanMessage>(getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
              errorCode = resp->errorCode;
            }
            mIsScanRequested = false;
            sendResponseForStopNetworkScan(serial, errorCode);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForStopNetworkScan(serial, RIL_E_NO_MEMORY);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::supplyNetworkDepersonalization(int32_t serial, const std::string& netPin) {
  QCRIL_LOG_DEBUG("supplyNetworkDepersonalization: serial=%d, netPin=%s",
      serial, PII(netPin.c_str()));
    auto msg =
        std::make_shared<UimEnterDePersoRequestMsg>(netPin, RIL_UIM_PERSOSUBSTATE_SIM_NETWORK);
    if (msg) {
      GenericCallback<RIL_UIM_PersoResponse> cb(
          ([this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                          std::shared_ptr<RIL_UIM_PersoResponse> responseDataPtr) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && responseDataPtr != nullptr) {
              errorCode = static_cast<RIL_Errno>(responseDataPtr->err);
            }
            sendResponseForSupplyNetworkDepersonalization(serial, errorCode, responseDataPtr);
          }));
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForSupplyNetworkDepersonalization(serial, RIL_E_NO_MEMORY, nullptr);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getUsageSetting(int32_t serial) {
  QCRIL_LOG_DEBUG("getUsageSetting: serial=%d", serial);
    auto msg = std::make_shared<RilRequestGetUsageSettingMessage>(getContext(serial));
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
          [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                         std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            std::shared_ptr<qcril::interfaces::RilGetUsageSettingResult_t> rilUsageSettingResult{};
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
              rilUsageSettingResult =
                  std::static_pointer_cast<qcril::interfaces::RilGetUsageSettingResult_t>(
                      resp->data);
            }
            sendResponseForGetUsageSetting(serial, errorCode, rilUsageSettingResult);
          });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
      sendResponseForGetUsageSetting(serial, RIL_E_NO_MEMORY, nullptr);
    }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setUsageSetting(int32_t serial,
    aidlnetwork::UsageSetting aidlMode) {
  QCRIL_LOG_DEBUG("setUsageSetting: serial=%d, usageSetting= %s", serial,
      aidlnetwork::toString(aidlMode).c_str());
  RIL_UsageSettingMode rilMode;
  RIL_Errno result = RIL_E_SUCCESS;
  result = convertAidlUsageSettingtoRil(aidlMode, rilMode);
  if (result == RIL_E_SUCCESS) {
    auto msg = std::make_shared<RilRequestSetUsageSettingMessage>(getContext(serial), rilMode);
    if (msg != nullptr) {
      GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
          std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
            RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
            if (status == Message::Callback::Status::SUCCESS && resp) {
              errorCode = resp->errorCode;
            }
            sendResponseForSetUsageSetting(serial, errorCode);
      });
      msg->setCallback(&cb);
      msg->dispatch();
    } else {
        sendResponseForSetUsageSetting(serial, RIL_E_NO_MEMORY);
    }
  } else {
      sendResponseForSetUsageSetting(serial, result);
  }
  return ndk::ScopedAStatus::ok();
}


::ndk::ScopedAStatus IRadioNetworkImpl::responseAcknowledgement(){
  QCRIL_LOG_DEBUG("IRadioNetwork: responseAcknowledgement");
  qti::ril::utils::releaseWakeLock();
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getBarringInfo(int32_t in_serial) {
  QCRIL_LOG_INFO("getBarringInfo: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetBarringInfoMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilCellBarringInfo_t> rilCellBarring {};
          if (status == Message::Callback::Status::SUCCESS) {
            if (resp) {
              errorCode = resp->errorCode;
              rilCellBarring =
                  std::static_pointer_cast<qcril::interfaces::RilCellBarringInfo_t>(resp->data);
            } else {
              errorCode = RIL_E_NO_MEMORY;
            }
          }
          this->sendResponseForGetBarringInfo(in_serial, errorCode, rilCellBarring);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetBarringInfo(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetBarringInfo(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilCellBarringInfo_t> rilCellBarring) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlnetwork::CellIdentity cellIdentity {};
  std::vector<aidlnetwork::BarringInfo> barringInfo {};

  if (errorCode == RIL_E_SUCCESS) {
    if (rilCellBarring) {
      convert(rilCellBarring->rilCellIdentity, cellIdentity);
      convert(rilCellBarring->barring_info, barringInfo);
    } else {
      errorCode = RIL_E_NO_MEMORY;
    }
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getBarringInfoResponse: serial=%d, error=%d", serial, errorCode);
  QCRIL_LOG_DEBUG("getBarringInfoResponse: cellIdentity = %s", cellIdentity.toString().c_str());

  for (size_t i = 0; i < barringInfo.size(); i++) {
    QCRIL_LOG_DEBUG("getBarringInfoResponse: barringInfo[%zu] = %s",
        i, barringInfo[i].toString().c_str());
  }

  auto ret = respCb->getBarringInfoResponse(responseInfo, cellIdentity, barringInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getCdmaRoamingPreference(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getCdmaRoamingPreference: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestQueryCdmaRoamingPrefMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t> roamPref {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            roamPref =
                std::static_pointer_cast<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t>(
                    resp->data);
          }
          this->sendResponseForGetCdmaRoamingPreference(in_serial, errorCode, roamPref);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetCdmaRoamingPreference(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetCdmaRoamingPreference(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilQueryCdmaRoamingPrefResult_t> roamPref) {
  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  aidlnetwork::CdmaRoamingType pref = aidlnetwork::CdmaRoamingType::HOME_NETWORK;
  if (errorCode == RIL_E_SUCCESS && roamPref) {
    pref = static_cast<aidlnetwork::CdmaRoamingType>(roamPref->mPrefType);
  }

  auto respCb = this->getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getCdmaRoamingPreferenceResponse: serial=%d, error=%d, pref=%d",
        serial, errorCode, pref);

    auto ret = respCb->getCdmaRoamingPreferenceResponse(responseInfo, pref);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getCellInfoList(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getCellInfoList: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetCellInfoListMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            cellInfoListResult =
                std::static_pointer_cast<qcril::interfaces::RilGetCellInfoListResult_t>(
                    resp->data);
          }
          this->sendResponseForGetCellInfoList(in_serial, errorCode, cellInfoListResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetCellInfoList(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetCellInfoList(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetCellInfoListResult_t> cellInfoListResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  std::vector<aidlnetwork::CellInfo> cellInfos {};
  if (errorCode == RIL_E_SUCCESS && cellInfoListResult) {
    convert(cellInfoListResult->cellInfos, cellInfos);
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getCellInfoListResponse: serial=%d, error=%d", serial, errorCode);

  for (size_t i = 0; i < cellInfos.size(); i++) {
    QCRIL_LOG_DEBUG("getCellInfoListResponse: cellInfos[%zu] = %s",
        i, cellInfos[i].toString().c_str());
  }

  auto ret = respCb->getCellInfoListResponse(responseInfo, cellInfos);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getDataRegistrationState(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getDataRegistrationState: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetDataRegistrationMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult {};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilRegResult =
                std::static_pointer_cast<qcril::interfaces::RilGetDataRegResult_t>(resp->data);
          }
          this->sendResponseForGetDataRegistrationState(in_serial, errorCode, rilRegResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetDataRegistrationState(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetDataRegistrationState(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetDataRegResult_t> rilRegResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlnetwork::RegStateResult regResponse {};
  if (errorCode == RIL_E_SUCCESS && rilRegResult) {
    convert(rilRegResult->respData, regResponse);
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getDataRegistrationStateResponse: serial=%d, error=%d", serial, errorCode);
  QCRIL_LOG_DEBUG("getDataRegistrationStateResponse: regResponse = %s",
      regResponse.toString().c_str());

  auto ret = respCb->getDataRegistrationStateResponse(responseInfo, regResponse);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getSignalStrength(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getSignalStrength: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetSignalStrengthMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult {};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilSigResult =
                std::static_pointer_cast<qcril::interfaces::RilGetSignalStrengthResult_t>(
                    resp->data);
          } else if (status == Message::Callback::Status::NO_HANDLER_FOUND) {
            errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
          }
          this->sendResponseForGetSignalStrength(in_serial, errorCode, rilSigResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetSignalStrength(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetSignalStrength(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetSignalStrengthResult_t> rilSigResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  aidlnetwork::SignalStrength signalStrength {};
  if (errorCode == RIL_E_SUCCESS && rilSigResult) {
    convert(rilSigResult->respData, signalStrength);
  }

  QCRIL_LOG_DEBUG("getSignalStrengthResponse: serial=%d, error=%d", serial, errorCode);
  QCRIL_LOG_DEBUG("getSignalStrengthResponse: signalStrength = %s",
      signalStrength.toString().c_str());

  auto ret = respCb->getSignalStrengthResponse(responseInfo, signalStrength);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getVoiceRadioTechnology(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getVoiceRadioTechnology: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetVoiceRadioTechMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetVoiceTechResult_t> ratResult {};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            ratResult =
                std::static_pointer_cast<qcril::interfaces::RilGetVoiceTechResult_t>(resp->data);
          }
          this->sendResponseForGetVoiceRadioTechnology(in_serial, errorCode, ratResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetVoiceRadioTechnology(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetVoiceRadioTechnology(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetVoiceTechResult_t> ratResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  aidlradio::RadioTechnology rat = aidlradio::RadioTechnology::UNKNOWN;
  if (errorCode == RIL_E_SUCCESS && ratResult) {
    rat = static_cast<aidlradio::RadioTechnology>(ratResult->rat);
  }

  QCRIL_LOG_DEBUG("getVoiceRadioTechnologyResponse: serial=%d, error=%d, rat=%d",
      serial, errorCode, rat);

  auto ret = respCb->getVoiceRadioTechnologyResponse(responseInfo, rat);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getVoiceRegistrationState(int32_t in_serial) {
  QCRIL_LOG_DEBUG("getVoiceRegistrationState: serial=%d", in_serial);

  auto msg = std::make_shared<RilRequestGetVoiceRegistrationMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult {};
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
            rilRegResult =
                std::static_pointer_cast<qcril::interfaces::RilGetVoiceRegResult_t>(resp->data);
          }
          this->sendResponseForGetVoiceRegistrationState(in_serial, errorCode, rilRegResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForGetVoiceRegistrationState(in_serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForGetVoiceRegistrationState(int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetVoiceRegResult_t> rilRegResult) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlnetwork::RegStateResult regResponse {};
  if (errorCode == RIL_E_SUCCESS) {
    if (rilRegResult) {
      convert(rilRegResult->respData, regResponse);
    } else {
      errorCode = RIL_E_NO_MEMORY;
    }
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("getVoiceRegistrationStateResponse: serial=%d, error=%d", serial, errorCode);
  QCRIL_LOG_DEBUG("getVoiceRegistrationStateResponse: regResponse = %s",
      regResponse.toString().c_str());

  auto ret = respCb->getVoiceRegistrationStateResponse(responseInfo, regResponse);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::isNrDualConnectivityEnabled(int32_t in_serial) {
  QCRIL_LOG_DEBUG("isNrDualConnectivityEnabled: serial=%d", in_serial);
  this->sendResponseForIsNrDualConnectivityEnabled(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);
  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForIsNrDualConnectivityEnabled(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("isNrDualConnectivityEnabledResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->isNrDualConnectivityEnabledResponse(responseInfo, false);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::setCdmaRoamingPreference(int32_t in_serial,
    aidlnetwork::CdmaRoamingType in_type) {
  QCRIL_LOG_DEBUG("setCdmaRoamingPreference: serial=%d, roaming_pref=%d",
      in_serial, in_type);

  auto msg = std::make_shared<RilRequestSetCdmaRoamingPrefMessage>(
      this->getContext(in_serial), static_cast<int>(in_type));

  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCdmaRoamingPreference(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetCdmaRoamingPreference(in_serial, RIL_E_NO_MEMORY);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForSetCdmaRoamingPreference(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("setCdmaRoamingPreferenceResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setCdmaRoamingPreferenceResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::setCellInfoListRate(int32_t in_serial, int32_t in_rate) {
  QCRIL_LOG_DEBUG("setCellInfoListRate: serial=%d, rate = %d", in_serial, in_rate);

  auto msg = std::make_shared<RilRequestSetUnsolCellInfoListRateMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    msg->setRate(in_rate);
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial] (std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_GENERIC_FAILURE;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetCellInfoListRate(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetCellInfoListRate(in_serial, RIL_E_NO_MEMORY);
  }

  return ::ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::sendResponseForSetCellInfoListRate(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    return;
  }

  aidlradio::RadioResponseInfo responseInfo {};
  responseInfo.type = aidlradio::RadioResponseType::SOLICITED;
  responseInfo.serial = serial;
  responseInfo.error = static_cast<aidlradio::RadioError>(errorCode);

  QCRIL_LOG_DEBUG("setCellInfoListRateResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setCellInfoListRateResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendCellBarring(std::shared_ptr<RilUnsolCellBarringMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    return;
  }

  auto rilBarringInfo = msg->getCellBarringInfo();

  aidlnetwork::CellIdentity aidlCellIdentity {};
  convert(rilBarringInfo.rilCellIdentity, aidlCellIdentity);

  std::vector<aidlnetwork::BarringInfo> aidlBarringInfos {};
  convert(rilBarringInfo.barring_info, aidlBarringInfos);

  QCRIL_LOG_DEBUG("UNSOL: barringInfoChanged: cellIdentity = %s",
      aidlCellIdentity.toString().c_str());

  for (size_t i = 0; i < aidlBarringInfos.size(); i++) {
    QCRIL_LOG_DEBUG("UNSOL: barringInfoChanged: barringInfos[%zu] = %s",
        i, aidlBarringInfos[i].toString().c_str());
  }

  auto ret = ind->barringInfoChanged(aidlradio::RadioIndicationType::UNSOLICITED,
      aidlCellIdentity, aidlBarringInfos);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendCellInfoList(std::shared_ptr<RilUnsolCellInfoListMessage> msg) {
    if (!msg) {
      QCRIL_LOG_ERROR("msg is nullptr");
      return;
    }

    auto ind = this->getIndicationCallback();
    QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
    if (!ind) {
      return;
    }

    std::vector<aidlnetwork::CellInfo> aidlCellInfos {};
    convert(msg->getCellInfoList(), aidlCellInfos);

    QCRIL_LOG_DEBUG("UNSOL: cellInfoList");
    for (size_t i = 0; i < aidlCellInfos.size(); i++) {
      QCRIL_LOG_DEBUG("UNSOL: cellInfoList: cellInfos[%zu] = %s",
          i, aidlCellInfos[i].toString().c_str());
    }

    auto ret = ind->cellInfoList(aidlradio::RadioIndicationType::UNSOLICITED,
        aidlCellInfos);

    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
    }
}

void IRadioNetworkImpl::sendSignalStrength(std::shared_ptr<RilUnsolSignalStrengthMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    return;
  }

  aidlnetwork::SignalStrength aidlSignalStrength {};
  convert(msg->getSignalStrength(), aidlSignalStrength);

  QCRIL_LOG_DEBUG("UNSOL: currentSignalStrength: signalStrength = %s",
      aidlSignalStrength.toString().c_str());

  auto ret = ind->currentSignalStrength(
      aidlradio::RadioIndicationType::UNSOLICITED, aidlSignalStrength);

  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendVoiceRadioTechChanged(
    std::shared_ptr<RilUnsolVoiceRadioTechChangedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    return;
  }

  // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
  qti::ril::utils::grabPartialWakeLock();

  QCRIL_LOG_DEBUG("UNSOL: voiceRadioTechChanged: rat = %d", msg->getVoiceRat());
  auto ret = ind->voiceRadioTechChanged(
      aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP,
      static_cast<aidlradio::RadioTechnology>(msg->getVoiceRat()));

  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendNitzTimeReceived(
    std::shared_ptr<RilUnsolNitzTimeReceivedMessage> msg) {
  if (!msg) {
    QCRIL_LOG_ERROR("msg is nullptr");
    return;
  }

  auto ind = this->getIndicationCallback();
  QCRIL_LOG_INFO("ind null: %s", ind ? "false" : "true");
  if (!ind) {
    s_lastNITZTimeMsg = msg;
    return;
  }

  auto& time = msg->getNitzTime();
  int64_t timeReceived = android::elapsedRealtime();
  QCRIL_LOG_DEBUG("nitzTimeReceivedInd: nitzTime %s receivedTime %" PRId64,
      time.c_str(), timeReceived);

  // The ATEL will acknowldge this UNSOL, which leads to release this wakelock
  qti::ril::utils::grabPartialWakeLock();

  QCRIL_LOG_DEBUG("UNSOL: nitzTimeReceived");
  auto ret = ind->nitzTimeReceived(
      aidlradio::RadioIndicationType::UNSOLICITED_ACK_EXP, time, timeReceived, 0);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForGetOperator(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetOperatorResult_t> rilOperatorResult) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  std::string longName{};
  std::string shortName{};
  std::string numeric{};
  if (errorCode == RIL_E_SUCCESS && rilOperatorResult) {
    longName = rilOperatorResult->longName;
    shortName = rilOperatorResult->shortName;
    numeric = rilOperatorResult->numeric;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getOperatorResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getOperatorResponse(responseInfo, longName, shortName, numeric);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioNetworkImpl::sendResponseForGetImsRegistrationState(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<RilGetImsRegistrationResult_t> regStateResp) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  bool isRegistered{ false };
  aidlradio::RadioTechnologyFamily radioTechFamily{ aidlradio::RadioTechnologyFamily::THREE_GPP };
  if (errorCode == RIL_E_SUCCESS && regStateResp) {
    isRegistered = regStateResp->isRegistered;
    radioTechFamily = regStateResp->ratFamily == RADIO_TECH_3GPP
                          ? aidlradio::RadioTechnologyFamily::THREE_GPP
                          : aidlradio::RadioTechnologyFamily::THREE_GPP2;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getImsRegistrationStateResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getImsRegistrationStateResponse(responseInfo, isRegistered, radioTechFamily);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioNetworkImpl::sendResponseForGetNetworkSelectionMode(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  bool manual = false;
  if (errorCode == RIL_E_SUCCESS && rilModeResult) {
    manual = rilModeResult->bManual;
  }
  auto respCb = getResponseCallback();
  if (respCb) {
    QCRIL_LOG_DEBUG("getNetworkSelectionModeResponse: serial=%d, error=%d", serial, errorCode);
    auto ret = respCb->getNetworkSelectionModeResponse(responseInfo, manual);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  }
}

void IRadioNetworkImpl::sendResponseForGetSystemSelectionChannels(
    int32_t serial, RIL_Errno errorCode,
    std::shared_ptr<qcril::interfaces::RilGetSysSelResult_t> getSysSelResult) {
  auto respCb = getResponseCallback();
  if (!respCb) {
    return;
  }
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };

  std::vector<aidlnetwork::RadioAccessSpecifier> specifiers{};
  if (errorCode == RIL_E_SUCCESS && getSysSelResult) {
    convertRadioAccessSpecifiersToAidl(specifiers, getSysSelResult->respData);
  }
  QCRIL_LOG_DEBUG("getSystemSelectionChannelsResponse: serial=%d, error=%d", serial, errorCode);
  for (size_t i = 0; i < specifiers.size(); ++i) {
    QCRIL_LOG_DEBUG( "RAS %d: %s", i, specifiers[i].toString().c_str());
  }
  auto ret = respCb->getSystemSelectionChannelsResponse(responseInfo, specifiers);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetIndicationFilter(int32_t serial, RIL_Errno errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             static_cast<aidlradio::RadioError>(errorCode) };
  auto respCb = getResponseCallback();
  if (!respCb) {
    return;
  }
  QCRIL_LOG_DEBUG("setIndicationFilterResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setIndicationFilterResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}
void IRadioNetworkImpl::sendResponseForSetLinkCapacityReportingCriteria(
    int32_t serial, aidlradio::RadioError errorCode) {
  aidlradio::RadioResponseInfo responseInfo{ aidlradio::RadioResponseType::SOLICITED, serial,
                                             errorCode };
  auto respCb = getResponseCallback();
  if (!respCb) {
    return;
  }
  QCRIL_LOG_DEBUG("setLinkCapacityReportingCriteriaResponse: serial=%d, error=%d", serial,
                  errorCode);
  auto ret = respCb->setLinkCapacityReportingCriteriaResponse(responseInfo);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetLocationUpdates(int32_t serial, RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, serial,
       static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("setLocationUpdatesResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setLocationUpdatesResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetNrDualConnectivityState(int32_t serial,
    RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, serial,
       static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("setNrDualConnectivityStateResponse: serial=%d, error=%d", serial, errorCode);
  auto ret = respCb->setNrDualConnectivityStateResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetNetworkSelectionModeManual(int32_t serial,
                                                                RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, serial,
       static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("sendResponseForSetNetworkSelectionModeManual: serial=%d, error=%d", serial,
                  errorCode);
  auto ret = respCb->setNetworkSelectionModeManualResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

void IRadioNetworkImpl::sendResponseForSetNetworkSelectionModeAutomatic(int32_t serial,
    RIL_Errno errorCode) {
  auto respCb = this->getResponseCallback();
  if (!respCb) {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
    return;
  }
  aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, serial,
       static_cast<aidlradio::RadioError>(errorCode)};

  QCRIL_LOG_DEBUG("setNetworkSelectionModeAutomaticResponse: serial=%d, error=%d", serial,
                   errorCode);
  auto ret = respCb->setNetworkSelectionModeAutomaticResponse(resp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::getImsRegistrationState(int32_t serial) {
  QCRIL_LOG_DEBUG("getImsRegistrationState: serial=%d", serial);
  auto msg = std::make_shared<RilRequestGetImsRegistrationMessage>(getContext(serial));
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        ([this, serial]([[maybe_unused]] std::shared_ptr<Message> /*msg*/,
                        Message::Callback::Status status,
                        std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<RilGetImsRegistrationResult_t> regStateResp{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            regStateResp = std::static_pointer_cast<RilGetImsRegistrationResult_t>(resp->data);
          }
          sendResponseForGetImsRegistrationState(serial, errorCode, regStateResp);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetImsRegistrationState(serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getNetworkSelectionMode(int32_t serial) {
  QCRIL_LOG_DEBUG("getNetworkSelectionMode: serial=%d", serial);
  auto msg = std::make_shared<RilRequestQueryNetworkSelectModeMessage>(getContext(serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilGetSelectModeResult_t> rilModeResult{};
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
            rilModeResult =
                std::static_pointer_cast<qcril::interfaces::RilGetSelectModeResult_t>(resp->data);
          }
          sendResponseForGetNetworkSelectionMode(serial, errorCode, rilModeResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetNetworkSelectionMode(serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getOperator(int32_t serial) {
  QCRIL_LOG_DEBUG("getOperator: serial=%d", serial);
  auto msg = std::make_shared<RilRequestOperatorMessage>(getContext(serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilGetOperatorResult_t> rilOperatorResult{};
          if (status == Message::Callback::Status::SUCCESS && resp)
          {
            errorCode = resp->errorCode;
            rilOperatorResult = std::static_pointer_cast<qcril::interfaces::RilGetOperatorResult_t>(resp->data);
          }
          sendResponseForGetOperator(serial, errorCode, rilOperatorResult);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetOperator(serial, RIL_E_NO_MEMORY, nullptr);
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::getSystemSelectionChannels(int32_t serial) {
  QCRIL_LOG_DEBUG("getSystemSelectionChannels: serial=%d", serial);
  auto msg = std::make_shared<RilRequestGetSysSelChannelsMessage>(getContext(serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          std::shared_ptr<qcril::interfaces::RilGetSysSelResult_t> result{};
          if (status == Message::Callback::Status::SUCCESS && resp)
          {
            errorCode = resp->errorCode;
            result = std::static_pointer_cast<qcril::interfaces::RilGetSysSelResult_t>(resp->data);
          }
          sendResponseForGetSystemSelectionChannels(serial, errorCode, result);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForGetSystemSelectionChannels(serial, RIL_E_NO_MEMORY, nullptr);
  }
  return ndk::ScopedAStatus::ok();
}

void IRadioNetworkImpl::setUnsolRespFilter(int32_t serial, int32_t indicationFilter) {
  auto msg =
      std::make_shared<RilRequestSetUnsolRespFilterMessage>(getContext(serial), indicationFilter);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          sendResponseForSetIndicationFilter(serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForSetIndicationFilter(serial, RIL_E_NO_MEMORY);
  }
}

void IRadioNetworkImpl::setPhysChanConfigReportingFilter(int32_t serial, int32_t indicationFilter) {
  auto msg = std::make_shared<NasEnablePhysChanConfigReporting>(
      (indicationFilter & (int32_t)aidlnetwork::IndicationFilter::PHYSICAL_CHANNEL_CONFIG) != 0,
      getContext(serial),
      [this, serial, indicationFilter](
          std::shared_ptr<Message> msg, Message::Callback::Status status,
          std::shared_ptr<qcril::interfaces::NasSettingResultCode> rc) -> void {
        QCRIL_LOG_DEBUG("Callback for NasEnablePhysChanConfigReporting. rc: %d",
                        rc ? static_cast<int>(*rc) : -1);
        aidlradio::RadioError err =
            convertMsgToRadioError(status, rc ? static_cast<RIL_Errno>(*rc) : RIL_E_INTERNAL_ERR);
        if (err != aidlradio::RadioError::NONE) {
          QCRIL_LOG_ERROR("Error enabling ChanConfigReporting: %d", static_cast<int>(err));
        } else {
          auto phyChanConfigReportingStatusMsg = std::make_shared<NasPhysChanConfigReportingStatus>(
              indicationFilter & (int32_t)aidlnetwork::IndicationFilter::PHYSICAL_CHANNEL_CONFIG);
          if (phyChanConfigReportingStatusMsg) {
            phyChanConfigReportingStatusMsg->broadcast();
          } else {
            QCRIL_LOG_ERROR("Failed to create message NasPhysChanConfigReportingStatus.");
          }
        }

        int32_t int32Filter =
            indicationFilter & (RIL_UR_SIGNAL_STRENGTH | RIL_UR_FULL_NETWORK_STATE |
                                RIL_UR_DATA_CALL_DORMANCY_CHANGED | RIL_UR_LINK_CAPACITY_ESTIMATE);
        setUnsolRespFilter(serial, int32Filter);
      });
  if (msg) {
    msg->dispatch();
  } else {
    sendResponseForSetIndicationFilter(serial, RIL_E_NO_MEMORY);
  }
}

::ndk::ScopedAStatus IRadioNetworkImpl::setIndicationFilter(int32_t serial, int32_t indicationFilter) {
  QCRIL_LOG_DEBUG("setIndicationFilter: serial=%d, indicationFilter=%d", serial, indicationFilter);
#ifndef QMI_RIL_UTF
  auto regFailureReportingStatusMsg =
      std::make_shared<rildata::RegistrationFailureReportingStatusMessage>(
          indicationFilter &
          (int32_t)aidlnetwork::IndicationFilter::REGISTRATION_FAILURE);
  if (regFailureReportingStatusMsg) {
    regFailureReportingStatusMsg->broadcast();
  } else {
    QCRIL_LOG_ERROR("Failed to create message RegistrationFailureReportingStatusMessage.");
  }
#endif

  bool is_barring = false;
  if (indicationFilter & (int32_t)aidlnetwork::IndicationFilter::BARRING_INFO) {
    is_barring = true;
  }
  auto msg = std::make_shared<RilRequestSetUnsolBarringFilterMessage>(getContext(serial),
                                                                      is_barring, indicationFilter);
  if (msg) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, serial, indicationFilter](
            std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
            std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          setPhysChanConfigReportingFilter(serial, indicationFilter);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    sendResponseForSetIndicationFilter(serial, RIL_E_NO_MEMORY);
  }

  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setLinkCapacityReportingCriteria(
    int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps, int32_t hysteresisUlKbps,
    const std::vector<int32_t>& thresholdsDownlinkKbps,
    const std::vector<int32_t>& thresholdsUplinkKbps,
    ::aidl::android::hardware::radio::AccessNetwork accessNetwork) {
  QCRIL_LOG_DEBUG(
      "setLinkCapacityReportingCriteria: serial: %d, hysteresisMs: %d, hysteresisDlKbps: %d, "
      "hysteresisUlKbps: %d, accessNetwork: %d",
      serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps, accessNetwork);
  QCRIL_LOG_DEBUG("thresholdsDownlinkKbps: {");
  for (int thrD : thresholdsDownlinkKbps) {
    QCRIL_LOG_DEBUG("| %d", thrD);
  }
  QCRIL_LOG_DEBUG("}");
#ifndef QMI_RIL_UTF
  rildata::LinkCapCriteria_t c = {
    .hysteresisMs = hysteresisMs,
    .hysteresisUlKbps = hysteresisUlKbps,
    .hysteresisDlKbps = hysteresisDlKbps,
    .thresholdsUplinkKbps = thresholdsUplinkKbps,
    .thresholdsDownlinkKbps = thresholdsDownlinkKbps,
  };
  c.ran = convertHidlAccessNetworkToDataAccessNetwork(accessNetwork);
  auto msg = std::make_shared<rildata::SetLinkCapRptCriteriaMessage>(c);
  if (msg) {
    GenericCallback<rildata::LinkCapCriteriaResult_t> cb(
        ([this, serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                        std::shared_ptr<rildata::LinkCapCriteriaResult_t> resp) -> void {
          if (msg && resp) {
            RIL_Errno re = convertLcResultToRilError(*resp);
            aidlradio::RadioError e = convertMsgToRadioError(status, re);
            sendResponseForSetLinkCapacityReportingCriteria(serial, e);
          }
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  }
#endif
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setLocationUpdates(int32_t in_serial, bool in_enable) {
  QCRIL_LOG_DEBUG("setLocationUpdates: serial=%d", in_serial);
  auto msg =
      std::make_shared<RilRequestSetLocationUpdateMessage>(this->getContext(in_serial), in_enable);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetLocationUpdates(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetLocationUpdates(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setNetworkSelectionModeAutomatic(int32_t in_serial) {
  QCRIL_LOG_DEBUG("setNetworkSelectionModeAutomatic: serial=%d", in_serial);
  auto msg =
    std::make_shared<RilRequestSetNetworkSelectionAutoMessage>(this->getContext(in_serial));
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> /*msg*/, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetNetworkSelectionModeAutomatic(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetNetworkSelectionModeAutomatic(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setNetworkSelectionModeManual(int32_t in_serial,
    const std::string& in_operatorNumeric, aidlradio::AccessNetwork in_ran) {
  QCRIL_LOG_INFO("setNetworkSelectionModeManual: serial=%d", in_serial);
  RIL_RadioTechnology rat = getRilRadioTechnologyFromRan(in_ran);
  auto msg = std::make_shared<RilRequestSetNetworkSelectionManualMessage>(
      this->getContext(in_serial), in_operatorNumeric, rat);
  if (msg != nullptr) {
    GenericCallback<QcRilRequestMessageCallbackPayload> cb(
        [this, in_serial](std::shared_ptr<Message> msg, Message::Callback::Status status,
                       std::shared_ptr<QcRilRequestMessageCallbackPayload> resp) -> void {
          RIL_Errno errorCode = RIL_E_INTERNAL_ERR;
          if (status == Message::Callback::Status::SUCCESS && resp != nullptr) {
            errorCode = resp->errorCode;
          }
          this->sendResponseForSetNetworkSelectionModeManual(in_serial, errorCode);
        });
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    this->sendResponseForSetNetworkSelectionModeManual(in_serial, RIL_E_NO_MEMORY);
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setNrDualConnectivityState(int32_t in_serial,
    aidlnetwork::NrDualConnectivityState in_nrDualConnectivityState) {
  QCRIL_LOG_DEBUG("setNrDualConnectivityState: serial=%d", in_serial);
  // TODO: Need to add support
  this->sendResponseForSetNrDualConnectivityState(in_serial, RIL_E_REQUEST_NOT_SUPPORTED);

  return ndk::ScopedAStatus::ok();
}

#ifdef QMI_RIL_UTF
void IRadioNetworkImpl::cleanup()
{
  mIsScanRequested = false;
}
#endif

// V2 unsupported requests

::ndk::ScopedAStatus IRadioNetworkImpl::setEmergencyMode(int32_t in_serial, ::aidl::android::hardware::radio::network::EmergencyMode in_emcModeType) {
  QCRIL_LOG_DEBUG("setEmergencyMode: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("setEmergencyModeResponse: serial=%d", in_serial);
    aidlnetwork::EmergencyRegResult regState{};
    auto ret = respCb->setEmergencyModeResponse(resp, regState);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::triggerEmergencyNetworkScan(
      int32_t in_serial, const ::aidl::android::hardware::radio::network::EmergencyNetworkScanTrigger& in_request) {
  QCRIL_LOG_DEBUG("triggerEmergencyNetworkScan: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("triggerEmergencyNetworkScanResponse: serial=%d", in_serial);
    auto ret = respCb->triggerEmergencyNetworkScanResponse(resp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::cancelEmergencyNetworkScan(int32_t in_serial, bool in_resetScan) {
  QCRIL_LOG_DEBUG("cancelEmergencyNetworkScan: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("cancelEmergencyNetworkScanResponse: serial=%d", in_serial);
    auto ret = respCb->cancelEmergencyNetworkScanResponse(resp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::exitEmergencyMode(int32_t in_serial) {
  QCRIL_LOG_DEBUG("exitEmergencyMode: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("exitEmergencyModeResponse: serial=%d", in_serial);
    auto ret = respCb->exitEmergencyModeResponse(resp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setNullCipherAndIntegrityEnabled(int32_t in_serial, bool in_enabled) {
  QCRIL_LOG_DEBUG("setNullCipherAndIntegrityEnabled: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("setNullCipherAndIntegrityEnabledResponse: serial=%d", in_serial);
    auto ret = respCb->setNullCipherAndIntegrityEnabledResponse(resp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::isNullCipherAndIntegrityEnabled(int32_t in_serial) {
  QCRIL_LOG_DEBUG("isNullCipherAndIntegrityEnabled: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("isNullCipherAndIntegrityEnabledResponse: serial=%d", in_serial);
    auto ret = respCb->isNullCipherAndIntegrityEnabledResponse(resp, false);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::isN1ModeEnabled(int32_t in_serial) {
  QCRIL_LOG_DEBUG("isN1ModeEnabled: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("isN1ModeEnabledResponse: serial=%d", in_serial);
    auto ret = respCb->isN1ModeEnabledResponse(resp, false);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IRadioNetworkImpl::setN1ModeEnabled(int32_t in_serial, bool in_enable) {
  QCRIL_LOG_DEBUG("setN1ModeEnabled: serial=%d", in_serial);
  auto respCb = this->getResponseCallback();
  if (respCb) {
    aidlradio::RadioResponseInfo resp {
       aidlradio::RadioResponseType::SOLICITED, in_serial,
       static_cast<aidlradio::RadioError>(RIL_E_REQUEST_NOT_SUPPORTED)};

    QCRIL_LOG_DEBUG("setN1ModeEnabledResponse: serial=%d", in_serial);
    auto ret = respCb->setN1ModeEnabledResponse(resp);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.getDescription().c_str());
    }
  } else {
    QCRIL_LOG_DEBUG("IRadioNetwork: getResponseCallback Failed");
  }
  return ndk::ScopedAStatus::ok();
}
