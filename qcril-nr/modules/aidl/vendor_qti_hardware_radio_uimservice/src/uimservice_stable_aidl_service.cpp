/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "uimservice_stable_aidl_service.h"
#include "framework/Log.h"
#include "interfaces/uim/UimRemoteSimlockRequestMsg.h"
#include "interfaces/uim/UimGBAInitRequestMsg.h"
#include "interfaces/uim/UimGBAGetIMPIRequestMsg.h"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim {
namespace implementation {

IUimImpl::IUimImpl(qcril_instance_id_e_type instance) : mInstanceId(instance)
{
}

IUimImpl::~IUimImpl()
{
}

qcril_instance_id_e_type IUimImpl::getInstanceId()
{
  return mInstanceId;
}

std::shared_ptr<aidlimports::IUimResponse> IUimImpl::getRemoteSimlockResponseCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mRemoteSimlockResponseCb;
}

std::shared_ptr<aidlimports::IUimResponse> IUimImpl::getGBAServiceResponseCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mGBAServiceResponseCb;
}

std::shared_ptr<aidlimports::IUimIndication> IUimImpl::getIndicationCallback()
{
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mIndicationCb;
}

void IUimImpl::clearCallbacks_nolock()
{
  QCRIL_LOG_DEBUG("clearCallbacks_nolock");
  mRemoteSimlockResponseCb = nullptr;
  mIndicationCb = nullptr;
  AIBinder_DeathRecipient_delete(mDeathRecipient);
  mDeathRecipient = nullptr;
}

void IUimImpl::clearGBACallbacks_nolock()
{
  QCRIL_LOG_DEBUG("clearGBACallbacks_nolock");
  mGBAServiceResponseCb = nullptr;
  AIBinder_DeathRecipient_delete(mGBADeathRecipient);
  mGBADeathRecipient = nullptr;
}

void IUimImpl::clearCallbacks()
{
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearCallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void IUimImpl::clearGBACallbacks()
{
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    clearGBACallbacks_nolock();
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

void IUimImpl::deathNotifier(void*)
{
  QCRIL_LOG_DEBUG("IUimImpl::serviceDied: Client died, Cleaning up callbacks");
  clearCallbacks();
}

void IUimImpl::gbaDeathNotifier(void*)
{
  QCRIL_LOG_DEBUG("IUimImpl::serviceDied: GBA Client died, Cleaning up callbacks");
  clearGBACallbacks();
}

static void deathRecpCallback(void* cookie)
{
  IUimImpl* uimImpl = static_cast<IUimImpl*>(cookie);
  if (uimImpl != nullptr) {
    uimImpl->deathNotifier(cookie);
  }
}

static void deathGBARecpCallback(void* cookie)
{
  IUimImpl* uimImpl = static_cast<IUimImpl*>(cookie);
  if (uimImpl != nullptr) {
    uimImpl->gbaDeathNotifier(cookie);
  }
}

void IUimImpl::setRemoteSimlockResponse_nolock(
    const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse,
    const std::shared_ptr<aidlimports::IUimIndication>& in_uimServiceIndication)
{
  QCRIL_LOG_DEBUG("IUimImpl::setResponseFunctions_nolock");
  mRemoteSimlockResponseCb = in_uimServiceRepsonse;
  mIndicationCb = in_uimServiceIndication;
}

void IUimImpl::setGBAServiceResponse_nolock(
    const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse)
{
  QCRIL_LOG_DEBUG("IUimImpl::setIndicationFunctions_nolock");
  mGBAServiceResponseCb = in_uimServiceRepsonse;
}

::ndk::ScopedAStatus IUimImpl::setCallback(
    const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse,
    const std::shared_ptr<aidlimports::IUimIndication>& in_uimServiceIndication)
{
  QCRIL_LOG_DEBUG("IUimImpl::setResposeFuntions");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mRemoteSimlockResponseCb != nullptr) {
    AIBinder_unlinkToDeath(
        mRemoteSimlockResponseCb->asBinder().get(), mDeathRecipient, reinterpret_cast<void*>(this));
  }
  setRemoteSimlockResponse_nolock(in_uimServiceRepsonse, in_uimServiceIndication);
  if (mRemoteSimlockResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(mRemoteSimlockResponseCb->asBinder().get(),
                           mDeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus IUimImpl::setGbaCallback(
    const std::shared_ptr<aidlimports::IUimResponse>& in_uimServiceRepsonse)
{
  QCRIL_LOG_DEBUG("IUimImpl::setResposeFuntions");
  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mGBAServiceResponseCb != nullptr) {
    AIBinder_unlinkToDeath(
        mGBAServiceResponseCb->asBinder().get(), mGBADeathRecipient, reinterpret_cast<void*>(this));
  }
  setGBAServiceResponse_nolock(in_uimServiceRepsonse);
  if (mGBAServiceResponseCb != nullptr) {
    AIBinder_DeathRecipient_delete(mGBADeathRecipient);
    mGBADeathRecipient = AIBinder_DeathRecipient_new(&deathGBARecpCallback);
    if (mGBADeathRecipient) {
      AIBinder_linkToDeath(mGBAServiceResponseCb->asBinder().get(),
                           mGBADeathRecipient,
                           reinterpret_cast<void*>(this));
    }
  }
  return ndk::ScopedAStatus::ok();
}

void IUimImpl::sendResponseForUimRemoteSimlock(int32_t token,
                                               UimRemoteSimlockResponseType response,
                                               UimRemoteSimlockOperationType op_type,
                                               std::vector<uint8_t>& simlock_data,
                                               UimRemoteSimlockVersion version,
                                               UimRemoteSimlockStatus status,
                                               int32_t /*timer_value*/)
{
  auto respCb = getRemoteSimlockResponseCallback();

  if (respCb) {
    QCRIL_LOG_DEBUG(
        "uimRemoteSimlockResponse token = %d, op_type  %d, status = %d", token, op_type, status);

    auto ret = respCb->uimRemoteSimlockResponse(token,
                                                response,
                                                static_cast<aidlimports::UimRemoteSimlockOperationType>(op_type),
                                                simlock_data,
                                                version,
                                                status);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s ", ret.getDescription().c_str());
    }
  }
}

void IUimImpl::sendResponseForuimGbaInit(int32_t token,
                                         aidlimports::UimGbaStatus gbaStatus,
                                         bool isKsNafEncrypted,
                                         std::vector<uint8_t> ksNaf,
                                         std::string bTid,
                                         std::string lifeTime)
{
  auto respCb = getGBAServiceResponseCallback();

  if (respCb) {
    QCRIL_LOG_DEBUG("uimGbaInitResponse: token = %d, gbaStatus = %d", token, gbaStatus);

    auto ret = respCb->uimGbaInitResponse(token, gbaStatus, isKsNafEncrypted, ksNaf, bTid, lifeTime);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception: %s ", ret.getDescription().c_str());
    }
  }
}

void IUimImpl::sendResponseForUimGbaGetImpi(int32_t token,
                                            aidlimports::UimGbaStatus gbaStatus,
                                            bool is_impi_encrypted,
                                            std::vector<uint8_t> impi)
{
  auto respCb = getGBAServiceResponseCallback();

  if (respCb) {
    QCRIL_LOG_INFO("uimGbaGetImpiResponse: token = %d, gbaStatus = %d", token, gbaStatus);
    auto ret = respCb->uimGbaGetImpiResponse(token, gbaStatus, is_impi_encrypted, impi);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception: %s ", ret.getDescription().c_str());
    }
  }
}

ndk::ScopedAStatus IUimImpl::uimRemoteSimlockRequest(
    int32_t token,
    aidlimports::UimRemoteSimlockOperationType simlockOp,
    const std::vector<uint8_t>& simlockData)
{
  auto msg = std::make_shared<UimRemoteSimlockRequestMsg>(
      static_cast<RIL_UIM_RemoteSimlockOperationType>(simlockOp), simlockData);

  QCRIL_LOG_DEBUG("uimRemoteSimlockRequest: token = %d, simlockOp = %d", token, simlockOp);

  if (msg) {
    GenericCallback<RIL_UIM_RemoteSimlockResponse> cb((
        [this, token, simlockOp](std::shared_ptr<Message> solicitedMsg,
                                 Message::Callback::Status ret_status,
                                 std::shared_ptr<RIL_UIM_RemoteSimlockResponse> rsp_ptr) -> void
        {
          aidlimports::UimRemoteSimlockOperationType op_type = simlockOp;
          UimRemoteSimlockResponseType rsp_type =
              UimRemoteSimlockResponseType::UIM_REMOTE_SIMLOCK_RESP_FAILURE;
          std::vector<uint8_t> data = {};
          UimRemoteSimlockVersion version = {};
          UimRemoteSimlockStatus status = {};
          int32_t timer = -1;

          if (solicitedMsg && rsp_ptr && ret_status == Message::Callback::Status::SUCCESS) {
            op_type = static_cast<aidlimports::UimRemoteSimlockOperationType>(rsp_ptr->op_type);
            rsp_type = static_cast<UimRemoteSimlockResponseType>(rsp_ptr->rsp_type);
            data = rsp_ptr->rsp_data;
            version.majorVersion = rsp_ptr->version.majorVersion;
            version.minorVersion = rsp_ptr->version.minorVersion;
            status.status = static_cast<UimRemoteSimlockStatusType>(rsp_ptr->status.state);
            status.unlockTime = rsp_ptr->status.unlockTime;
            timer = rsp_ptr->timer_value;
          }
          sendResponseForUimRemoteSimlock(token, rsp_type, op_type, data, version, status, timer);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    std::vector<uint8_t> data = {};
    UimRemoteSimlockVersion version = {};
    UimRemoteSimlockStatus status = {};

    sendResponseForUimRemoteSimlock(token,
                                    UimRemoteSimlockResponseType::UIM_REMOTE_SIMLOCK_RESP_FAILURE,
                                    simlockOp,
                                    data,
                                    version,
                                    status,
                                    -1);
  }

  return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus IUimImpl::uimGbaInit(int32_t token,
                                        const std::vector<uint8_t>& securityProtocol,
                                        const std::string& nafFullyQualifiedDomainName,
                                        aidlimports::UimApplicationType appType,
                                        bool forceBootStrapping,
                                        const std::string& apn)
{
  QCRIL_LOG_DEBUG("uimGbaInit: token = %d, appType = %d", token, appType);

  RIL_UIM_GBA_InitParams gba_init_params = {};
  gba_init_params.slot_id = mInstanceId;
  gba_init_params.force_bootstrapping = forceBootStrapping;
  gba_init_params.domain_name = nafFullyQualifiedDomainName;
  gba_init_params.protocol = securityProtocol;
  gba_init_params.app_type = static_cast<RIL_UIM_AppType>(appType);

  if (!apn.empty()) {
    gba_init_params.apn = apn;
  }

  auto msg = std::make_shared<UimGBAInitRequestMsg>(gba_init_params, token);

  if (msg) {
    GenericCallback<RIL_UIM_GBA_InitResponse> cb((
        [this](std::shared_ptr<Message> solicitedMsg,
               Message::Callback::Status ret_status,
               std::shared_ptr<RIL_UIM_GBA_InitResponse> rsp_ptr) -> void
        {
          UimGbaStatus gbaStatus = UimGbaStatus::UIM_GBA_GENERIC_FAILURE;
          bool isKsNafEncrypted = false;
          std::vector<uint8_t> ksNaf = {};
          std::string bTid = "";
          std::string lifeTime = "";
          uint32_t rsp_token = 0;
          if (solicitedMsg && rsp_ptr && ret_status == Message::Callback::Status::SUCCESS) {
            if (rsp_ptr->err == RIL_UIM_E_SUCCESS) {
              gbaStatus = UimGbaStatus::UIM_GBA_SUCCESS;
            }
            isKsNafEncrypted = static_cast<bool>(rsp_ptr->naf_type);
            ksNaf = rsp_ptr->naf_response;
            bTid = rsp_ptr->bootstrap_tid;
            lifeTime = rsp_ptr->life_time;
            rsp_token = rsp_ptr->token;
          }
          sendResponseForuimGbaInit(rsp_token, gbaStatus, isKsNafEncrypted, ksNaf, bTid, lifeTime);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    UimGbaStatus gbaStatus = UimGbaStatus::UIM_GBA_GENERIC_FAILURE;
    bool isKsNafEncrypted = false;
    std::vector<uint8_t> ksNaf = {};
    std::string bTid = "";
    std::string lifeTime = "";
    sendResponseForuimGbaInit(token, gbaStatus, isKsNafEncrypted, ksNaf, bTid, lifeTime);
  }

  return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus IUimImpl::uimGetImpi(int32_t token,
                                        aidlimports::UimApplicationType app_type,
                                        bool secure)
{
  QCRIL_LOG_DEBUG("uimGetImpi: token = %d, app_type = %d, secure = %d", token, app_type, secure);
  RIL_UIM_GBA_IMPI_Params gba_get_impi_params = {};

  gba_get_impi_params.slot_id = mInstanceId;
  gba_get_impi_params.app_type = static_cast<RIL_UIM_AppType>(app_type);
  gba_get_impi_params.secure = secure;

  auto msg = std::make_shared<UimGBAGetIMPIRequestMsg>(gba_get_impi_params, token);
  if (msg) {
    GenericCallback<RIL_UIM_GBA_IMPI_Response> cb((
        [this](std::shared_ptr<Message> solicitedMsg,
               Message::Callback::Status ret_status,
               std::shared_ptr<RIL_UIM_GBA_IMPI_Response> rsp_ptr) -> void
        {
          UimGbaStatus gbaStatus = aidlimports::UimGbaStatus::UIM_GBA_GENERIC_FAILURE;
          std::vector<uint8_t> impi = {};
          bool is_impi_encrypted = false;
          uint32_t rsp_token = 0;
          if (solicitedMsg && rsp_ptr && ret_status == Message::Callback::Status::SUCCESS) {
            if (rsp_ptr->err == RIL_UIM_E_SUCCESS) {
              gbaStatus = UimGbaStatus::UIM_GBA_SUCCESS;
            }
            is_impi_encrypted = rsp_ptr->is_impi_encrypted;
            impi = rsp_ptr->impi;
            rsp_token = rsp_ptr->token;
          }

          sendResponseForUimGbaGetImpi(rsp_token, gbaStatus, is_impi_encrypted, impi);
        }));
    msg->setCallback(&cb);
    msg->dispatch();
  } else {
    UimGbaStatus gbaStatus = UimGbaStatus::UIM_GBA_GENERIC_FAILURE;
    std::vector<uint8_t> impi = {};
    bool is_impi_encrypted = false;
    sendResponseForUimGbaGetImpi(token, gbaStatus, is_impi_encrypted, impi);
  }

  return ndk::ScopedAStatus::ok();
}

}  // namespace implementation
}  // namespace uim
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
