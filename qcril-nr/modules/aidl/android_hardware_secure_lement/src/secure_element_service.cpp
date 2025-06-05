/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "secure_element_service.h"
#include "SecureElementAidlModule.h"
#include "framework/Log.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/secure_element/SecureElementOnStateChangeIndMsg.h"
#include "modules/uim/UimModemEndPoint.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "qcril_legacy_apis.h"
#include "utils_common.h"
#include <mutex>
#include <unistd.h>
#include <vector>
#define QCRIL_UIM_INS_BYTE_SELECT 0xA4
#define QCRIL_UIM_P1_VALUE_SELECT_BY_DF_NAME 0x04
#define QCRIL_UIM_P2_MASK_SELECT_NEXT 0x02

#define UICC_SE_HAL_PROP "persist.vendor.radio.uicc_se_enabled"

using android::sp;
// using aidl::android::hardware::Return;

std::condition_variable_any cond;
qtimutex::QtiRecursiveMutex mutx;
secure_element_long_apdu_info_type long_apdu_info;
boolean is_send_apdu_in_progress = FALSE;

/*===========================================================================

FUNCTION: ISapImpl::clearCallbacks

===========================================================================*/
void ISecureElementImpl::clearCallbacks() {
  QCRIL_LOG_FUNC_ENTRY("enter");
  {
    std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
    mSecureElementRespCb = nullptr;
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = nullptr;
    mModule->mServiceUp = false;
    mModule->mReady = false;
  }
  QCRIL_LOG_FUNC_ENTRY("exit");
}

/*===========================================================================

FUNCTION: ISapImpl::deathNotifier

===========================================================================*/

void ISecureElementImpl::deathNotifier(void *) {
  QCRIL_LOG_DEBUG(
      "ISecureElementImpl::serviceDied: Client died, Cleaning up callbacks");
  clearCallbacks();
}

/*===========================================================================

FUNCTION: deathRecpCallback

===========================================================================*/

static void deathRecpCallback(void *cookie) {
  ISecureElementImpl *SecureElementImpl =
      static_cast<ISecureElementImpl *>(cookie);
  if (SecureElementImpl != nullptr) {
    SecureElementImpl->deathNotifier(cookie);
  }
}

ISecureElementImpl::ISecureElementImpl(SecureElementAidlModule *module) {
  mModule = module;
}

ISecureElementImpl::~ISecureElementImpl() {}

SecureElementStatus ISecureElementImpl::convertQmiErrSecureElementStatus(
    int qmi_err_code, qmi_uim_logical_channel_rsp_type logical_channel_rsp) {
  if (logical_channel_rsp.sw1_sw2_valid == QMI_UIM_FALSE) {
    return SE_STATUS_IOERROR;
  }

  switch (qmi_err_code) {
  case QMI_UIM_SERVICE_ERR_INSUFFICIENT_RESOURCES:
    return SE_STATUS_CHNL_NOT_AVAILABLE;

  case QMI_UIM_SERVICE_ERR_INCOMPATIBLE_STATE:
  case QMI_UIM_SERVICE_ERR_SIM_FILE_NOT_FOUND:
    return SE_STATUS_NO_SUCH_ELEMENT_ERROR;

  case QMI_UIM_SERVICE_ERR_NONE:
    return SE_STATUS_SUCCESS;

  default:
    return SE_STATUS_FAILED;
  }
} /* SecureElementImpl::convertQmiErrSecureElementStatus */

SecureElementStatus ISecureElementImpl::openChannelRequest(
    const std::vector<uint8_t> &aid, uint8_t p2,
    qmi_uim_logical_channel_rsp_type *rsp_data_ptr) {
  qmi_uim_open_logical_channel_params_type open_ch = {};
  qmi_uim_rsp_data_type rsp_data;
  int qmi_err_code = 0;
  SecureElementStatus status;

  QCRIL_LOG_INFO("SecureElementImpl::openChannelRequest");

  if (rsp_data_ptr == NULL) {
    return SE_STATUS_FAILED;
  }

  open_ch.slot = (qmi_uim_slot_type)(getInstanceId() + QMI_UIM_SLOT_1);

  QCRIL_LOG_INFO("%d", aid.size());

  if (aid.size() > 0) {
    open_ch.aid_present = QMI_UIM_TRUE;
    open_ch.aid.data_ptr = new uint8_t[aid.size()];

    if (open_ch.aid.data_ptr == NULL) {
      return SE_STATUS_FAILED;
    }

    memcpy(open_ch.aid.data_ptr, aid.data(), aid.size());
    open_ch.aid.data_len = aid.size();
  }

  open_ch.file_control_information.is_valid = QMI_UIM_TRUE;

  switch (p2) {
  case 0x00:
    open_ch.file_control_information.fci_value = QMI_UIM_FCI_VALUE_FCI;
    break;
  case 0x04:
    open_ch.file_control_information.fci_value = QMI_UIM_FCI_VALUE_FCP;
    break;
  case 0x08:
    open_ch.file_control_information.fci_value = QMI_UIM_FCI_VALUE_FMD;
    break;
  case 0x0C:
    open_ch.file_control_information.fci_value = QMI_UIM_FCI_VALUE_NO_DATA;
    break;
  default:
    open_ch.file_control_information.is_valid = QMI_UIM_FALSE;
    QCRIL_LOG_ERROR("Invalid P2 value: 0x%x", p2);
    return SE_STATUS_UNSUPPORTED_OPERATION;
  }

  memset(&rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  qmi_err_code = mModule->secure_element_send_qmi_sync_msg(
      QCRIL_UIM_REQUEST_OPEN_LOGICAL_CHANNEL, &open_ch, &rsp_data);

  delete[] open_ch.aid.data_ptr;

  if (qmi_err_code != QMI_UIM_SERVICE_ERR_NONE) {
    QCRIL_LOG_INFO("openChannelRequest: Error %d", rsp_data.qmi_err_code);

    return SE_STATUS_FAILED;
  } else {
    status = convertQmiErrSecureElementStatus(
        rsp_data.qmi_err_code, rsp_data.rsp_data.logical_channel_rsp);
  }

  if (status != SE_STATUS_SUCCESS) {
    if (rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr !=
        NULL) {
      qcril_free(
          rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr);
    }
    return status;
  }

  memcpy(rsp_data_ptr, &rsp_data.rsp_data.logical_channel_rsp,
         sizeof(qmi_uim_logical_channel_rsp_type));

  rsp_data_ptr->select_response.data_ptr = new uint8_t
      [rsp_data.rsp_data.logical_channel_rsp.select_response.data_len + 2];

  rsp_data_ptr->select_response.data_len =
      rsp_data.rsp_data.logical_channel_rsp.select_response.data_len + 2;

  if (rsp_data_ptr->select_response.data_ptr != NULL) {
    if (rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr !=
        NULL) {
      memcpy(rsp_data_ptr->select_response.data_ptr,
             rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr,
             rsp_data.rsp_data.logical_channel_rsp.select_response.data_len);
      qcril_free(
          rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr);
    }
    rsp_data_ptr->select_response.data_ptr[rsp_data.rsp_data.logical_channel_rsp
                                               .select_response.data_len] =
        rsp_data.rsp_data.logical_channel_rsp.sw1;
    rsp_data_ptr->select_response.data_ptr
        [rsp_data.rsp_data.logical_channel_rsp.select_response.data_len + 1] =
        rsp_data.rsp_data.logical_channel_rsp.sw2;
  } else {
    rsp_data_ptr->select_response.data_len = 0;
  }

  return status;
} /* SecureElementImpl::openChannelRequest */

void ISecureElementImpl::handleSecureElementModuleReady(void) {
  int qmi_err_code = QMI_UIM_SERVICE_ERR_NONE;
  qmi_uim_rsp_data_type rsp_data;

  std::shared_ptr<aidlimports::ISecureElementCallback> callback =
      getSecureElementResponseCallback();

  QCRIL_LOG_INFO("=DBG= handleSecureElementModuleReady called");

  memset(&rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  qmi_err_code = mModule->secure_element_send_qmi_sync_msg(
      QCRIL_UIM_REQUEST_GET_CARD_STATUS, nullptr, &rsp_data);

  if (qmi_err_code == QMI_UIM_SERVICE_ERR_NONE &&
      rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NONE) {
    uint8_t slot = (uint8_t)getInstanceId();

    if (slot < QMI_UIM_MAX_CARD_COUNT &&
        rsp_data.rsp_data.get_card_status_rsp.card_status_validity
                .card_status_invalid[slot] == QMI_UIM_FALSE &&
        rsp_data.rsp_data.get_card_status_rsp.card_status.card[slot]
                .card_state == QMI_UIM_CARD_STATE_PRESENT) {
      mModule->card_accessible = true;

      /* Delay the card insert time for 3 secs to delay non prov app activation
       * requests */
      sleep(3);

      if (callback != nullptr) {
        QCRIL_LOG_INFO("=DBG= Send card state as present to Clients");
        callback->onStateChange(TRUE, "init");
      }
    }
  }
} /* handleSecureElementModuleReady */

/*===========================================================================

FUNCTION:  SecureElementImpl::init

===========================================================================*/
::ndk::ScopedAStatus ISecureElementImpl::init(
    const std::shared_ptr<::aidlimports::ISecureElementCallback>
        &clientCallback) {
  QCRIL_LOG_INFO("SecureElementImpl::init");

  if (clientCallback == nullptr) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
  }

  std::unique_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  if (mSecureElementRespCb != nullptr) {
    AIBinder_unlinkToDeath(mSecureElementRespCb->asBinder().get(),
                           mDeathRecipient, reinterpret_cast<void *>(this));
  }
  mSecureElementRespCb = clientCallback;
  if (mSecureElementRespCb != nullptr) {
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    mDeathRecipient = AIBinder_DeathRecipient_new(&deathRecpCallback);
    if (mDeathRecipient) {
      AIBinder_linkToDeath(mSecureElementRespCb->asBinder().get(),
                           mDeathRecipient, reinterpret_cast<void *>(this));
    }
  }

  mModule->mServiceUp = true;

  if (mModule->mQmiUimUp) {
    mModule->mReady = true;
  }

  if (mModule->mReady == true) {
    std::shared_ptr<SecureElementOnStateChangeIndMsg> seOnStateChangePtr =
        nullptr;

    /* Send ONStateChange Indication to SE clients */
    seOnStateChangePtr = std::make_shared<SecureElementOnStateChangeIndMsg>();

    if (seOnStateChangePtr) {
      seOnStateChangePtr->broadcast();
    }
  }

  return ndk::ScopedAStatus::ok();
} /* SecureElementImpl::init */

std::shared_ptr<aidlimports::ISecureElementCallback>
ISecureElementImpl::getSecureElementResponseCallback() {
  std::shared_lock<qtimutex::QtiSharedMutex> lock(mCallbackLock);
  return mSecureElementRespCb;
}

/*===========================================================================

FUNCTION:  SecureElementImpl::isCardPresent

===========================================================================*/
::ndk::ScopedAStatus ISecureElementImpl::isCardPresent(bool *_aidl_return) {
  if (mModule != nullptr) {
    *_aidl_return = mModule->card_accessible;
  }

  return ndk::ScopedAStatus::ok();
} /* SecureElementImpl::isCardPresent */

void ISecureElementImpl::setInstanceId(qcril_instance_id_e_type instanceId) {
  mInstanceId = instanceId;
}

int ISecureElementImpl::getInstanceId() { return mInstanceId; }

/*===========================================================================

FUNCTION:  SecureElementImpl::getAtr

===========================================================================*/
::ndk::ScopedAStatus
ISecureElementImpl::getAtr(std::vector<uint8_t> *response) {
  qmi_uim_get_atr_params_type get_atr_params = {};
  qmi_uim_rsp_data_type rsp_data;
  int qmi_err_code = 0;
  std::vector<uint8_t> atr;

  QCRIL_LOG_INFO("SecureElementImpl::getAtr");

  if (response == NULL) {
    return ndk::ScopedAStatus::ok();
  }

  get_atr_params.slot = (qmi_uim_slot_type)((getInstanceId()) + QMI_UIM_SLOT_1);

  /* get atr */
  memset(&rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  qmi_err_code = mModule->secure_element_send_qmi_sync_msg(
      QCRIL_UIM_REQUEST_GET_ATR, &get_atr_params, &rsp_data);

  QCRIL_LOG_INFO("SecureElementImpl::getAtrreponse status is %d", qmi_err_code);

  for (int i = 0; i < rsp_data.rsp_data.get_atr_rsp.atr_response.data_len;
       i++) {
    response->push_back(rsp_data.rsp_data.get_atr_rsp.atr_response.data_ptr[i]);
  }

  if (rsp_data.rsp_data.get_atr_rsp.atr_response.data_ptr != NULL) {
    qcril_free(rsp_data.rsp_data.get_atr_rsp.atr_response.data_ptr);
    rsp_data.rsp_data.get_atr_rsp.atr_response.data_ptr = NULL;
  }

  return ndk::ScopedAStatus::ok();
} /* SecureElementImpl::getAtr */

/*===========================================================================

FUNCTION:  SecureElementImpl::transmit

===========================================================================*/
::ndk::ScopedAStatus
ISecureElementImpl::transmit(const std::vector<uint8_t> &data,
                             std::vector<uint8_t> *response) {
  std::vector<uint8_t> rsp = {};
  qmi_uim_rsp_data_type rsp_data;
  int qmi_err_code = 0;
  bool not_select_next = true;
  uint8_t *select_rsp = NULL;

  QCRIL_LOG_INFO("SecureElementImpl::transmit");

  if (response == NULL) {
    return ndk::ScopedAStatus::ok();
  }

  if (data.size() >= 4 && (data.data()[1] == QCRIL_UIM_INS_BYTE_SELECT) &&
      (data.data()[2] == QCRIL_UIM_P1_VALUE_SELECT_BY_DF_NAME) &&
      ((data.data()[3] & 0x03) == QCRIL_UIM_P2_MASK_SELECT_NEXT)) {
    qmi_uim_reselect_params_type reselect_params{};

    reselect_params.slot =
        (qmi_uim_slot_type)(getInstanceId() + QMI_UIM_SLOT_1);
    reselect_params.channel_id = data.data()[0];
    reselect_params.select_mode = QMI_UIM_SELECT_MODE_NEXT;

    memset(&rsp_data, 0, sizeof(qmi_uim_rsp_data_type));
    qmi_err_code = mModule->secure_element_send_qmi_sync_msg(
        QCRIL_UIM_REQUEST_RESELECT, &reselect_params, &rsp_data);

    if (rsp_data.qmi_err_code != QMI_UIM_SERVICE_ERR_INVALID_QMI_CMD) {
      select_rsp =
          new uint8_t[rsp_data.rsp_data.reselect_rsp.select_response.data_len +
                      2];

      not_select_next = false;

      if (select_rsp != NULL) {
        if (rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NOT_SUPPORTED) {
          response->push_back(0x6F);
          response->push_back(0x00);
        } else {
          if (rsp_data.rsp_data.reselect_rsp.sw1_sw2_valid == QMI_UIM_FALSE ||
              (rsp_data.rsp_data.reselect_rsp.sw1 == 0 &&
               rsp_data.rsp_data.reselect_rsp.sw2 == 0)) {
            response->push_back(0x00);
            response->push_back(0x00);
          } else {
            if (rsp_data.rsp_data.reselect_rsp.select_response.data_ptr !=
                NULL) {
              memcpy(select_rsp,
                     rsp_data.rsp_data.reselect_rsp.select_response.data_ptr,
                     rsp_data.rsp_data.reselect_rsp.select_response.data_len);
              qcril_free(
                  rsp_data.rsp_data.reselect_rsp.select_response.data_ptr);
            }
            select_rsp[rsp_data.rsp_data.reselect_rsp.select_response
                           .data_len] = rsp_data.rsp_data.reselect_rsp.sw1;
            select_rsp[rsp_data.rsp_data.reselect_rsp.select_response.data_len +
                       1] = rsp_data.rsp_data.reselect_rsp.sw2;

            for (int i = 0;
                 i <
                 rsp_data.rsp_data.reselect_rsp.select_response.data_len + 2;
                 i++) {
              response->push_back(select_rsp[i]);
            }
          }
        }
      }
    }
  }

  if (not_select_next) {

    QCRIL_LOG_INFO("Transmitting APdu");
    qmi_uim_send_apdu_params_type apdu_params{};

    apdu_params.slot = (qmi_uim_slot_type)(getInstanceId() + QMI_UIM_SLOT_1);
    apdu_params.apdu.data_ptr = new uint8_t[data.size()];

    if (apdu_params.apdu.data_ptr == NULL) {
      return ndk::ScopedAStatus::ok();
    }

    memcpy(apdu_params.apdu.data_ptr, data.data(), data.size());
    apdu_params.apdu.data_len = data.size();

    is_send_apdu_in_progress = TRUE;
    qmi_err_code = mModule->secure_element_send_qmi_sync_msg(
        QCRIL_UIM_REQUEST_SEND_APDU, &apdu_params, &rsp_data);

    delete[] apdu_params.apdu.data_ptr;

    QCRIL_LOG_DEBUG("qmi_err_code is %d", rsp_data.qmi_err_code);
    /* Special case for long APDUs - we send the response of the long APDU
       stream after we get all the chunks in the corresponding SEND_APDU_INDs */
    if (rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_INSUFFICIENT_RESOURCES) {
      QCRIL_LOG_DEBUG("transmit: total_len=0x%x bytes, token=0x%x",
                      rsp_data.rsp_data.send_apdu_rsp.total_len,
                      rsp_data.rsp_data.send_apdu_rsp.token);

      mutx.lock();

      /* Store only if the Long APDU response TLV is valid. We also need to
         check
         and handle cases where the INDs might have come earlier than this
         response.
         Note that original_request_ptr will be freed when we get SEND_APDU_INDs
         */
      if (long_apdu_info.in_use == TRUE) {
        /* If Indication already came, we need to check incoming info */
        if ((long_apdu_info.token == rsp_data.rsp_data.send_apdu_rsp.token) &&
            (long_apdu_info.total_apdu_len ==
             rsp_data.rsp_data.send_apdu_rsp.total_len)) {
          /* If Indication already came & incoming info matches,
             nothing else to do wait for indication to complete*/
          goto apdu_ind_wait;
        } else {
          /* Error condition - mismatch in data, send error if there was any
             previous
             request & store the current response's token */
          QCRIL_LOG_ERROR(
              "Mismatch with global data, token: 0x%x, total_apdu_len: 0x%x",
              long_apdu_info.token, long_apdu_info.total_apdu_len);
          /* Cleanup and proceed to store current resp info */
          mModule->cleanUpLongApduInfo();
        }
      }

      /* Store response info. We return after successfully storing since
         we expect subsequent INDs */
      if (rsp_data.rsp_data.send_apdu_rsp.total_len > 0) {
        QCRIL_LOG_INFO("Storing long_apdu_info");
        long_apdu_info.in_use = TRUE;
        long_apdu_info.token = rsp_data.rsp_data.send_apdu_rsp.token;
        long_apdu_info.total_apdu_len =
            rsp_data.rsp_data.send_apdu_rsp.total_len;
      }

    apdu_ind_wait:
      /* Wait only when we have still some response data to be received */
      if (long_apdu_info.rx_len < long_apdu_info.total_apdu_len) {
        cond.wait(mutx);

        if (long_apdu_info.rx_len != long_apdu_info.total_apdu_len) {
          response->push_back(0x00);
          response->push_back(0x00);
        } else {
          for (int i = 0; i < long_apdu_info.total_apdu_len; i++) {
            response->push_back(long_apdu_info.apdu_ptr[i]);
          }
        }
      } else {
        for (int i = 0; i < long_apdu_info.total_apdu_len; i++) {
          response->push_back(long_apdu_info.apdu_ptr[i]);
        }
      }
      mModule->cleanUpLongApduInfo();

      mutx.unlock();

      is_send_apdu_in_progress = FALSE;

      return ndk::ScopedAStatus::ok();
    }

    if (rsp_data.qmi_err_code != QMI_UIM_SERVICE_ERR_NONE &&
        rsp_data.qmi_err_code != QMI_UIM_SERVICE_ERR_INSUFFICIENT_RESOURCES) {
      return ndk::ScopedAStatus::fromServiceSpecificError(FAILED);
    }

    for (int i = 0; i < rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_len;
         i++) {
      response->push_back(
          rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr[i]);
    }

    QCRIL_LOG_INFO("Sending the response back to clients");
  }

  is_send_apdu_in_progress = FALSE;

  if (not_select_next) {
    if (rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr != NULL) {
      qcril_free(rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr);
    }
  }
  if (select_rsp != NULL) {
    delete[] select_rsp;
  }

  return ndk::ScopedAStatus::ok();
} /* SecureElementImpl::transmit */

/*===========================================================================

FUNCTION:  SecureElementImpl::openLogicalChannel

===========================================================================*/
::ndk::ScopedAStatus ISecureElementImpl::openLogicalChannel(
    const std::vector<uint8_t> &aid, int8_t p2,
    ::aidlimports::LogicalChannelResponse *response) {

  ::aidlimports::LogicalChannelResponse rsp;
  qmi_uim_logical_channel_rsp_type rsp_data = {};
  SecureElementStatus status;

  QCRIL_LOG_INFO("%d", aid.size());

  if (response == NULL) {
    return ndk::ScopedAStatus::ok();
  }

  status = openChannelRequest(aid, p2, &rsp_data);

  QCRIL_LOG_INFO(
      "SecureElementImpl::openLogicalChannel status : %d, channel_id : %d",
      status, rsp_data.channel_id);

  if (status == SE_STATUS_SUCCESS) {
    std::lock_guard<qtimutex::QtiSharedMutex> lock(ch_id_list_lock);

    response->channelNumber = rsp_data.channel_id;
    for (int i = 0; i < rsp_data.select_response.data_len; i++) {
      response->selectResponse.push_back(rsp_data.select_response.data_ptr[i]);
    }
    /* cache the channeld ID, client may die and service need to close the
     * channels */
    ch_id_list.push_back(rsp_data.channel_id);
  }

  else {
    return ndk::ScopedAStatus::fromServiceSpecificError(status);
  }

  if (rsp_data.select_response.data_ptr != NULL) {
    delete[] rsp_data.select_response.data_ptr;
  }

  return ndk::ScopedAStatus::ok();
} /* SecureElementImpl::openLogicalChannel */

/*===========================================================================

FUNCTION:  SecureElementImpl::openBasicChannel

===========================================================================*/
::ndk::ScopedAStatus
ISecureElementImpl::openBasicChannel(const std::vector<uint8_t> &aid, int8_t p2,
                                     std::vector<uint8_t>* /*response*/) {
  std::vector<uint8_t> rsp = {};

  (void)aid;

  QCRIL_LOG_INFO("SecureElementImpl::openBasicChannel : %d", p2);

  /* Basic channel is channel 0 for telecom app, accessing not support on UICC
   */
  return ndk::ScopedAStatus::fromServiceSpecificError(
      SE_STATUS_CHNL_NOT_AVAILABLE);
} /* SecureElementImpl::openBasicChannel */

/*===========================================================================

FUNCTION:  SecureElementImpl::closeChannel

===========================================================================*/
::ndk::ScopedAStatus ISecureElementImpl::closeChannel(int8_t channelNumber) {
  qmi_uim_rsp_data_type rsp_data;
  int qmi_err_code = 0;
  qmi_uim_logical_channel_params_type close_ch;
  memset(&rsp_data, 0, sizeof(qmi_uim_rsp_data_type));
  memset(&close_ch, 0, sizeof(qmi_uim_logical_channel_params_type));

  QCRIL_LOG_INFO("SecureElementImpl::closeChannel ChannelNumber is %d",
                 channelNumber);

  if (channelNumber == 0) {
    return ndk::ScopedAStatus::fromServiceSpecificError(FAILED);
  }

  close_ch.slot = (qmi_uim_slot_type)(getInstanceId() + QMI_UIM_SLOT_1);
  close_ch.operation_type = QMI_UIM_LOGICAL_CHANNEL_CLOSE;
  close_ch.channel_data.close_channel_info.channel_id = channelNumber;

  qmi_err_code = mModule->secure_element_send_qmi_sync_msg(
      QCRIL_UIM_REQUEST_LOGICAL_CHANNEL, &close_ch, &rsp_data);

  QCRIL_LOG_INFO("SecureElementImpl::closeChannel status : %d", qmi_err_code);

  if (qmi_err_code != QMI_UIM_SERVICE_ERR_NONE &&
      rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NONE) {
    return ndk::ScopedAStatus::fromServiceSpecificError(FAILED);
  } else if (rsp_data.qmi_err_code != QMI_UIM_SERVICE_ERR_NONE) {
    return ndk::ScopedAStatus::fromServiceSpecificError(FAILED);
  } else {
    std::lock_guard<qtimutex::QtiSharedMutex> lock(ch_id_list_lock);

    /* Remove channel id from the cached list */
    if (!ch_id_list.empty()) {
      std::vector<uint8_t>::iterator it = ch_id_list.begin();
      while (it != ch_id_list.end()) {
        if (*it == channelNumber) {
          ch_id_list.erase(it);
          break;
        }
        it++;
      }
    }

    return ndk::ScopedAStatus::ok();
  }
} /* SecureElementImpl::closeChannel */

/*===========================================================================

FUNCTION:  SecureElementImpl::reset

===========================================================================*/
::ndk::ScopedAStatus ISecureElementImpl::reset() {
  qmi_uim_power_down_params_type power_down_params = {};
  qmi_uim_power_up_params_type power_up_params = {};
  qmi_uim_rsp_data_type rsp_data = {};
  power_down_params.slot =
      (qmi_uim_slot_type)(getInstanceId() + QMI_UIM_SLOT_1);

  mModule->secure_element_send_qmi_sync_msg(QCRIL_UIM_REQUEST_POWER_DOWN,
                                            &power_down_params, &rsp_data);

  /*Modem needs sometime to process Powerdown followed by Powerup - 500 msecs*/
  usleep(500 * 1000);

  if (rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NONE) {
    QCRIL_LOG_INFO("UimPowerDown Request Processed Successfully");
    power_up_params.slot =
        (qmi_uim_slot_type)(getInstanceId() + QMI_UIM_SLOT_1);
    power_up_params.mode = QMI_UIM_CARD_MODE_TELECOM_CARD;
    mModule->secure_element_send_qmi_sync_msg(QCRIL_UIM_REQUEST_POWER_UP,
                                              &power_up_params, &rsp_data);
    if (rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NONE) {
      QCRIL_LOG_INFO("UimPowerUp Request Processed Successfully");
    } else if (rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NO_EFFECT) {
      QCRIL_LOG_INFO("UimPowerUp NO_EFFECT treated as SUCCESS");
      return ndk::ScopedAStatus::ok();
    } else {
      QCRIL_LOG_ERROR("UimPowerUp Request Failed with error code %d",
                      rsp_data.qmi_err_code);
      return ndk::ScopedAStatus::ok();
    }
  } else {
    QCRIL_LOG_ERROR("UimPowerDown Request Failed with error code %d",
                    rsp_data.qmi_err_code);
    return ndk::ScopedAStatus::ok();
  }

  return ndk::ScopedAStatus::ok();
} /* SecureElementImpl::reset*/
