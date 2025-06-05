/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "SecureElementAidlModule.h"
#include "framework/Log.h"
#include "interfaces/uim/UimQmiUimRequestMsg.h"
#include "interfaces/uim/qcril_uim_types.h"
#include "qcril_legacy_apis.h"
#include "utils_common.h"
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <cutils/properties.h>
#include <framework/ModuleLooper.h>

static load_module<SecureElementAidlModule> sSecureElementAidlModule;

typedef struct {
  qmi_uim_indication_id_type ind_id;
  qmi_uim_indication_data_type ind_data;
  uint8_t payload;
} secure_element_indication_params_type;

extern std::condition_variable_any cond;
extern qtimutex::QtiRecursiveMutex mutx;
extern secure_element_long_apdu_info_type long_apdu_info;
extern boolean is_send_apdu_in_progress;

SecureElementAidlModule::SecureElementAidlModule() {
  mName = "SecureElementAidlModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);
  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(QcrilInitMessage, SecureElementAidlModule::handleQcrilInit),
      HANDLER(UimQmiUimIndicationMsg,
              SecureElementAidlModule::handleQmiUimIndiaction),
      HANDLER(SecureElementOnStateChangeIndMsg,
              SecureElementAidlModule::handleSecureElementOnStateChangeInd)};
}

SecureElementAidlModule::~SecureElementAidlModule() {

  mLooper = nullptr;

}

void SecureElementAidlModule::init() {
  Module::init();

  // Initializations complete.
  mReady = true;
}

void SecureElementAidlModule::handleQcrilInit(
    std::shared_ptr<QcrilInitMessage> msg) {
  if (msg == nullptr) {
    Log::getInstance().d("[" + mName + "]: Handling invalid msg");
    return;
  }
  Log::getInstance().d("[" + mName + "]: Handling msg" + msg->dump());

  qcril_instance_id_e_type instance_id = msg->get_instance_id();
  Log::getInstance().d("[" + mName + "]: get_instance_id = " +
                       std::to_string(instance_id));

  if (mISecureElementImpl == nullptr) {
    mISecureElementImpl = ndk::SharedRefBase::make<ISecureElementImpl>(this);
    std::string serviceInstance = std::string(ISecureElementImpl::descriptor) +
                                  "/SIM" + std::to_string(instance_id + 1);

    mISecureElementImpl->setInstanceId(instance_id);
    binder_status_t status = AServiceManager_addService(
        mISecureElementImpl->asBinder().get(), serviceInstance.c_str());

    QCRIL_LOG_INFO("SecureElement stable aidl addService %s, status = %d",
                   serviceInstance.c_str(), status);
    if (status != STATUS_OK) {
      mISecureElementImpl = nullptr;
      QCRIL_LOG_INFO("Error registering service %s", serviceInstance.c_str());
    }
  }
}

void SecureElementAidlModule::cleanUpLongApduInfo(void) {
  is_send_apdu_in_progress = FALSE;
  if (long_apdu_info.apdu_ptr != NULL) {
    qcril_free(long_apdu_info.apdu_ptr);
    long_apdu_info.apdu_ptr = NULL;
  }
  memset(&long_apdu_info, 0x00, sizeof(long_apdu_info));
} /* cleanUpLongApduInfo */

void SecureElementAidlModule::handleSecureElementOnStateChangeInd(
    std::shared_ptr<SecureElementOnStateChangeIndMsg> msg) {
  if (msg == NULL) {
    QCRIL_LOG_ERROR("SecureElementOnStateChangeIndMsg is NULL");
  }

  /* Msg should never be NULL, call handleSecureElementModuleReady anyways */
  mISecureElementImpl->handleSecureElementModuleReady();

} /* SecureElementModule::handleSecureElementOnStateChangeInd */

/*=========================================================================

  FUNCTION:  secure_element_send_qmi_sync_msg

===========================================================================*/
int SecureElementAidlModule::secure_element_send_qmi_sync_msg(
    uint32_t msg_id, const void *params, qmi_uim_rsp_data_type *rsp_data_ptr) {
  int ret = -1;
  auto req_msg_ptr = std::make_shared<UimQmiUimRequestMsg>(
      msg_id, params, (void *)rsp_data_ptr, nullptr);

  if (req_msg_ptr != nullptr) {
    std::shared_ptr<int> respPtr = nullptr;

    if ((req_msg_ptr->dispatchSync(respPtr) ==
         Message::Callback::Status::SUCCESS) &&
        (respPtr != nullptr)) {
      ret = *respPtr;
    }
  }

  return ret;
} /* secure_element_send_qmi_sync_msg */

void SecureElementAidlModule::handleQmiUimIndiaction(
    std::shared_ptr<UimQmiUimIndicationMsg> uim_qmi_ind_msg_ptr) {

  std::shared_ptr<aidlimports::ISecureElementCallback> callback = nullptr;

  if (mISecureElementImpl != nullptr) {
    callback = mISecureElementImpl->getSecureElementResponseCallback();
  }

  QCRIL_LOG_INFO("Received qmi_uim indication is %d\n",
                 uim_qmi_ind_msg_ptr->get_ind_id());

  if (uim_qmi_ind_msg_ptr != NULL) {
    switch (uim_qmi_ind_msg_ptr->get_ind_id()) {
    case QMI_UIM_SRVC_UP_IND_MSG:
      mQmiUimUp = true;

      if (mServiceUp) {
        mReady = true;
        mISecureElementImpl->handleSecureElementModuleReady();
      }
      break;

    case QMI_UIM_SRVC_DOWN_IND_MSG:
      mReady = false;
      mQmiUimUp = false;
      card_accessible = false;

      if (mISecureElementImpl != nullptr && callback != nullptr) {
        QCRIL_LOG_INFO("Securelement has not been initialised");
        auto ret = callback->onStateChange(
            false, "SecureElement has not been initialised ");
        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
                          ret.getDescription().c_str());
        }
      }
      if (mISecureElementImpl != nullptr) {
        std::lock_guard<qtimutex::QtiSharedMutex> lock(
            mISecureElementImpl->ch_id_list_lock);
        if (!mISecureElementImpl->ch_id_list.empty()) {
          mISecureElementImpl->ch_id_list.clear();
        }
      }

      /* Long APDU transaction might be in progress send signal to send response
       */
      mutx.lock();
      if (long_apdu_info.in_use == TRUE) {
        cleanUpLongApduInfo();
        cond.notify_one();
      }
      mutx.unlock();
      break;

    case QMI_UIM_SRVC_STATUS_CHANGE_IND_MSG: {
      secure_element_indication_params_type *param_ptr =
          (secure_element_indication_params_type *)
              uim_qmi_ind_msg_ptr->get_message();
      uint8_t slot = 0;
      if (mISecureElementImpl != nullptr) {
         slot = (uint8_t)mISecureElementImpl->getInstanceId();
      }

      if (slot < QMI_UIM_MAX_CARD_COUNT && param_ptr != nullptr &&
          param_ptr->ind_data.status_change_ind.card_status_validity
                  .card_status_invalid[slot] == QMI_UIM_FALSE &&
          param_ptr->ind_data.status_change_ind.card_status.card[slot]
                  .card_state == QMI_UIM_CARD_STATE_PRESENT) {
        if (mISecureElementImpl != nullptr && callback != nullptr &&
            !card_accessible) {
          /* Delay the card insert time for 3 secs to delay non prov app
           * activation requests */

          QCRIL_LOG_INFO(" ==DBG== Updating card state update to clients\n");

          sleep(3);
          QCRIL_LOG_INFO("Status change indication to modem");
          auto ret = callback->onStateChange(
              true, "Uim Card Status change Indication ");

          if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
                            ret.getDescription().c_str());
          }
        }
        card_accessible = true;
      } else {
        if (mISecureElementImpl != nullptr && callback != nullptr &&
            card_accessible) {

          QCRIL_LOG_INFO(" ==DBG== Updating card state as absent to clients\n");

          auto ret = callback->onStateChange(false, "Uim Card State absent");

          if (!ret.isOk()) {
            QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
                            ret.getDescription().c_str());
          }
          if (mISecureElementImpl != nullptr) {
            std::lock_guard<qtimutex::QtiSharedMutex> lock(
                mISecureElementImpl->ch_id_list_lock);
            if (!mISecureElementImpl->ch_id_list.empty()) {
              mISecureElementImpl->ch_id_list.clear();
            }
          }
        }
        card_accessible = false;

        /* Long APDU transaction might be in progress send signal to send
         * response */
        mutx.lock();
        if (long_apdu_info.in_use == TRUE) {
          cleanUpLongApduInfo();
          cond.notify_one();
        }
        mutx.unlock();
      }
    } break;

    case QMI_UIM_SRVC_RECOVERY_IND_MSG:
      /* Long APDU transaction might be in progress send signal to send response
       */
      mutx.lock();
      if (long_apdu_info.in_use == TRUE) {
        cleanUpLongApduInfo();
        cond.notify_one();
      }
      mutx.unlock();

      if (mISecureElementImpl != nullptr && callback != nullptr) {
        /* Clear cached channel ids and send state change to false and true for
           recovery
           So that app can clear its data and can connect again */
        QCRIL_LOG_INFO("Service Recovery indication");
        auto ret =
            callback->onStateChange(false, "Uim Service recovery Indication");

        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
                          ret.getDescription().c_str());
        }
        if (mISecureElementImpl != nullptr) {
          std::lock_guard<qtimutex::QtiSharedMutex> lock(
              mISecureElementImpl->ch_id_list_lock);
          if (!mISecureElementImpl->ch_id_list.empty()) {
            secureElementClearChannelIdList(mISecureElementImpl->ch_id_list);
          }
        }

        /* Delay the card insert time for 3 secs to delay non prov app
         * activation requests */
        sleep(3);

        QCRIL_LOG_INFO("Status change indication followed by recovery");
        ret = callback->onStateChange(
            true, "Card Status Indication Folowed by recovery");

        if (!ret.isOk()) {
          QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
                          ret.getDescription().c_str());
        }
      }
      break;

    case QMI_UIM_SRVC_REFRESH_IND_MSG:
      if (callback != nullptr) {
        secure_element_indication_params_type *param_ptr =
            (secure_element_indication_params_type *)
                uim_qmi_ind_msg_ptr->get_message();

        /* Clear cached channel ids and send state change to false for refresh
           reset
           so that app can cleanup its data */
        if (param_ptr != nullptr &&
            param_ptr->ind_data.refresh_ind.refresh_event.refresh_stage ==
                QMI_UIM_REFRESH_STAGE_START &&
            param_ptr->ind_data.refresh_ind.refresh_event.refresh_mode ==
                QMI_UIM_REFRESH_MODE_RESET) {
          if (mISecureElementImpl != nullptr && card_accessible) {
            QCRIL_LOG_INFO("Securelement Rfresh reset indication");
            auto ret =
                callback->onStateChange(false, "Uim Refresh reset indication");

            if (!ret.isOk()) {
              QCRIL_LOG_ERROR("Unable to send indication. Exception : %s",
                              ret.getDescription().c_str());
            }
            if (mISecureElementImpl != nullptr) {
              std::lock_guard<qtimutex::QtiSharedMutex> lock(
                  mISecureElementImpl->ch_id_list_lock);
              if (!mISecureElementImpl->ch_id_list.empty()) {
                mISecureElementImpl->ch_id_list.clear();
              }
            }
          }
          card_accessible = false;

          /* Long APDU transaction might be in progress send signal to send
           * response */
          mutx.lock();
          if (long_apdu_info.in_use == TRUE) {
            cleanUpLongApduInfo();
            cond.notify_one();
          }
          mutx.unlock();
        }
      }
      break;

    case QMI_UIM_SRVC_SEND_APDU_IND_MSG:
      if (is_send_apdu_in_progress) {
        secure_element_indication_params_type *param_ptr =
            (secure_element_indication_params_type *)
                uim_qmi_ind_msg_ptr->get_message();
        qmi_uim_send_apdu_ind_type *apdu_ind_ptr = nullptr;
        uint16_t remaining_len = 0;
        uint16_t stored_len = 0;

        if (param_ptr == nullptr) {
          break;
        }

        mutx.lock();

        apdu_ind_ptr =
            (qmi_uim_send_apdu_ind_type *)&param_ptr->ind_data.send_apdu_ind;
        QCRIL_LOG_INFO("Send APDU Indication - token: 0x%x, total_len: 0x%x, "
                       "offset: 0x%x, data_len: 0x%x",
                       apdu_ind_ptr->token, apdu_ind_ptr->total_len,
                       apdu_ind_ptr->offset, apdu_ind_ptr->apdu.data_len);

        if (long_apdu_info.in_use == TRUE) {
          /* If Response already came, we need to check incoming info */
          if ((long_apdu_info.token != apdu_ind_ptr->token) ||
              (long_apdu_info.total_apdu_len != apdu_ind_ptr->total_len)) {
            /* Error condition - mismatch in data, discrd the response */
            QCRIL_LOG_ERROR(
                "Mismatch with global data, token: 0x%x, total_apdu_len: 0x%x",
                long_apdu_info.token, long_apdu_info.total_apdu_len);
            cleanUpLongApduInfo();
            cond.notify_one();
            mutx.unlock();
            break;
          }
        } else {
          /* Response hasn't come yet, we can still store IND info */
          QCRIL_LOG_INFO("long_apdu_info.in_use is FALSE, storing info");
          long_apdu_info.in_use = TRUE;
          long_apdu_info.token = apdu_ind_ptr->token;
          long_apdu_info.total_apdu_len = apdu_ind_ptr->total_len;
        }

        /* If this is the first chunk, allocate the buffer. This buffer will
           only be freed at the end of the receiving all the INDs */
        if (long_apdu_info.apdu_ptr == NULL) {
          long_apdu_info.rx_len = 0;
          long_apdu_info.apdu_ptr =
              (uint8_t *)qcril_malloc(apdu_ind_ptr->total_len);
          if (long_apdu_info.apdu_ptr == NULL) {
            QCRIL_LOG_ERROR("%s", "Couldnt allocate apdu_ptr pointer !");
            long_apdu_info.in_use = FALSE;
            cond.notify_one();
            mutx.unlock();
            break;
          }
        }

        /* Find out the remaining APDU buffer length */
        stored_len = long_apdu_info.rx_len;
        remaining_len = long_apdu_info.total_apdu_len - stored_len;

        /* If this chunk cannot fit in our global buffer, discard the IND */
        if ((apdu_ind_ptr->apdu.data_len > remaining_len) ||
            (apdu_ind_ptr->offset >= long_apdu_info.total_apdu_len) ||
            ((apdu_ind_ptr->offset + apdu_ind_ptr->apdu.data_len) >
             long_apdu_info.total_apdu_len)) {
          QCRIL_LOG_ERROR("Mismatch with global data, total_apdu_len: 0x%x "
                          "stored_len: 0x%x, remaining_len: 0x%x",
                          long_apdu_info.total_apdu_len, stored_len,
                          remaining_len);
          long_apdu_info.in_use = FALSE;
          cond.notify_one();
          mutx.unlock();
          break;
        }

        /* Save the data & update the data length */
        memcpy(long_apdu_info.apdu_ptr + apdu_ind_ptr->offset,
               apdu_ind_ptr->apdu.data_ptr, apdu_ind_ptr->apdu.data_len);
        long_apdu_info.rx_len += apdu_ind_ptr->apdu.data_len;

        /* If it is the last one, send the response back & clean up global
         * buffer */
        if (long_apdu_info.total_apdu_len == long_apdu_info.rx_len) {
          cond.notify_one();
        }
        mutx.unlock();
      }
      break;
    }
  }
} /* SecureElementModule::handleQmiUimIndiaction */

void SecureElementAidlModule::secureElementClearChannelIdList(
    std::vector<uint8_t> &ch_id_list) {
  for (auto ch_id : ch_id_list) {
    /* Send close channel, modem might re-open if there is no
       transmit APDU on that channel in case of recovery */
    qmi_uim_logical_channel_params_type close_ch;
    qmi_uim_rsp_data_type rsp_data;
    memset(&close_ch, 0, sizeof(qmi_uim_logical_channel_params_type));
    memset(&rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

    if (ch_id == 0) {
      continue;
    }

    close_ch.slot = (qmi_uim_slot_type)(mISecureElementImpl->getInstanceId() +
                                        QMI_UIM_SLOT_1);
    close_ch.operation_type = QMI_UIM_LOGICAL_CHANNEL_CLOSE;
    close_ch.channel_data.close_channel_info.channel_id = ch_id;

    (void)secure_element_send_qmi_sync_msg(QCRIL_UIM_REQUEST_LOGICAL_CHANNEL,
                                           &close_ch, &rsp_data);
  }
  mISecureElementImpl->ch_id_list.clear();
} /* secureElementClearChannelIdList */
