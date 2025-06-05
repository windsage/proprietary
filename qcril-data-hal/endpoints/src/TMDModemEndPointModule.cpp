/**
* Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/
#include <string>
#include "TMDModemEndPointModule.h"
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <modules/qmi/QmiIndMessage.h>

using namespace rildata;
extern int global_instance_id;
DEFINE_MSG_ID_INFO(TMDModemEndPoint_QMI_IND)
DEFINE_MSG_ID_INFO(TMDModemEndPoint_ENDPOINT_STATUS_IND)

TMDModemEndPointModule::TMDModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner,
    REG_MSG(TMDModemEndPoint_QMI_IND),
    REG_MSG(TMDModemEndPoint_ENDPOINT_STATUS_IND))
{
    mServiceObject = nullptr;
    mLooper = std::make_unique<ModuleLooper>();

    using std::placeholders::_1;
    mMessageHandler = {
      HANDLER(QmiSetupRequest, TMDModemEndPointModule::handleQmiClientSetup),
      HANDLER(QmiServiceUpIndMessage, TMDModemEndPointModule::handleQmiServiceUp),
    };
}

TMDModemEndPointModule::~TMDModemEndPointModule() { mLooper = nullptr; }

void TMDModemEndPointModule::init() {
    ModemEndPointModule::init();
}

qmi_idl_service_object_type TMDModemEndPointModule::getServiceObject() {
    return tmd_get_service_object_v01();
}

static void qmiUnsolIndCb(qmi_client_type user_handle, unsigned int msg_id,
                          void *ind_buf, unsigned int ind_buf_len,
                          void *ind_cb_data) {
  Log::getInstance().d("[TMDModemEndPointModule]: qmiUnsolIndCb");
  if (ind_cb_data != nullptr) {
    ModemEndPointModule* pModule = static_cast<ModemEndPointModule*>(ind_cb_data);
    if (pModule) {
      pModule->onQmiUnsolIndCb(user_handle, msg_id, ind_buf, ind_buf_len);
    } else {
      Log::getInstance().d("Unexpected: pModule is NULL");
    }
  } else {
    Log::getInstance().d("Unexpected: no right CB data for QMI ind");
  }
}

static void qmiNotifyCb(qmi_client_type user_handle,
                        qmi_idl_service_object_type service_obj,
                        qmi_client_notify_event_type service_event,
                        void *notify_cb_data) {
  Log::getInstance().d("[TMDModemEndPointModule]: qmiNotifyCb");
  if (notify_cb_data != nullptr) {
    ModemEndPointModule *pModule = static_cast<ModemEndPointModule*>(notify_cb_data);
    if (pModule) {
      pModule->onQmiNotifyCb(user_handle, service_obj, service_event);
    } else {
      Log::getInstance().d("Unexpected: pModule is NULL");
    }
  } else {
    Log::getInstance().d("Unexpected: no right CB data for notification");
  }
}

static void qmiNotifyErrorCb(qmi_client_type user_handle,
                             qmi_client_error_type error,
                             void* error_cb_data) {
  (void)user_handle;

  Log::getInstance().d("[TMDModemEndPointModule]: qmiNotifyErrorCb");

  if (error_cb_data != nullptr) {
    ModemEndPointModule *pModule = static_cast<ModemEndPointModule*>(error_cb_data);
    if (pModule) {
      Log::getInstance().d("qmiNotifyErrorCb: module : " + pModule->to_string());
      auto shared_srvDownMsg = std::make_shared<QmiServiceDownIndMessage>(error);
      pModule->dispatch(shared_srvDownMsg);
    }
  }
}

void TMDModemEndPointModule::handleQmiClientSetup(
    std::shared_ptr<QmiSetupRequest> shared_setupMsg) {

  Log::getInstance().d("[TMDModemEndPointModule]: Handling msg = " + shared_setupMsg->dump());
  mInstanceId = shared_setupMsg->getInstanceId();

  // get the service obj if needed
  if (mServiceObject == nullptr) {
    mServiceObject = getServiceObject();
    if (mServiceObject == nullptr) {
      Log::getInstance().d(
          "[TMDModemEndPointModule]:" + mOwner.mName + " : Did not get get_service_object");
      return;
    } else {
      Log::getInstance().d(
          "[TMDModemEndPointModule]:" + mOwner.mName + " : Got get_service_object");
    }
  }
  if (mOwner.getState() == ModemEndPoint::State::OPERATIONAL) {
    Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName + " is already operational");
    // send response immediately
    shared_setupMsg->sendResponse(shared_setupMsg, Message::Callback::Status::SUCCESS, nullptr);
    return;
  }

  // initialize the QMI client
  qmi_client_type qmi_svc_client = nullptr;
  qmi_client_error_type client_err = qmi_client_init_instance(
    mServiceObject, QMI_CLIENT_INSTANCE_MODEM, (qmi_client_ind_cb)qmiUnsolIndCb,
    this, &mOsParams, 4, &qmi_svc_client);
  Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName +
                       " : qmi_client_init_instance return: " + std::to_string(client_err));
  if (client_err == QMI_NO_ERR) {
    // Register for service_down event in case of client init success
    client_err = qmi_client_register_error_cb(qmi_svc_client, qmiNotifyErrorCb, this);
    Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName +
                         " : qmi_client_register_error_cb return: " + std::to_string(client_err));
  }

  // send response immediately
  shared_setupMsg->sendResponse(shared_setupMsg,
      (client_err == QMI_NO_ERR) ? Message::Callback::Status::SUCCESS
      : Message::Callback::Status::FAILURE, nullptr);

  if (client_err != QMI_NO_ERR) {
    Log::getInstance().d("[TMDModemEndPointModule]: Error in client init:" + mOwner.mName);
    // release client
    if (qmi_svc_client) {
      qmi_client_error_type rc = qmi_client_release(qmi_svc_client);
      if (rc != QMI_NO_ERR) {
        Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName + ":Failed to release client");
      } else {
        Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName + ":Release client");
      }
    }
    mQmiSvcClient = nullptr;
    // register for UP event in case of client init failure
    if (mQmiNotifyHandle) {
      Log::getInstance().d("[TMDModemEndPointModule]: Relase QMI notify handle for "
            + mOwner.mName);
      qmi_client_release(mQmiNotifyHandle);
      mQmiNotifyHandle = nullptr;
    }
    qmi_client_error_type rc = qmi_client_notifier_init(
        mServiceObject, &mNotifierOsParams, &mQmiNotifyHandle);
    if (rc == QMI_NO_ERR) {
      rc = qmi_client_register_notify_cb(mQmiNotifyHandle, qmiNotifyCb, this);

      Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName +
          " : qmi_client_register_notify_cb return : " + std::to_string(rc));

      if (rc != QMI_NO_ERR) {
        Log::getInstance().d(
            "[TMDModemEndPointModule]: " + mOwner.mName + " : qmi_client_register_notify_cb failed: " +
            std::to_string(rc));
      }
    } else {
      Log::getInstance().d(
        "[TMDModemEndPointModule]: " + mOwner.mName + " : qmi_client_notifier_init failed: " +
        std::to_string(rc));
    }
  } else {
    Log::getInstance().d("[TMDModemEndPointModule]: No Error in client init : " + mOwner.mName);
    mQmiSvcClient = qmi_svc_client;
    // for single SIM, the stackId is always -1, since no StackIdIndMessage received
    if (mInstanceId >= QCRIL_DEFAULT_INSTANCE_ID &&
        mInstanceId < QCRIL_MAX_INSTANCE_ID) {
      if (mStackId[mInstanceId] != -1) {
        (void)handleQmiBinding(mInstanceId, mStackId[mInstanceId]);
      }
    }
    mOwner.setState(ModemEndPoint::State::OPERATIONAL);
    // Send ModemEndPoint state changed
    auto shared_msg = std::make_shared<EndpointStatusIndMessage>
                        (ModemEndPoint::State::OPERATIONAL, mEndPtStatusIndId);
    Dispatcher::getInstance().dispatch(shared_msg);
  }
}

void TMDModemEndPointModule::handleQmiServiceUp(std::shared_ptr<QmiServiceUpIndMessage> msg) {
  (void)msg;
  Log::getInstance().d("[TMDModemEndPointModule]: " + mOwner.mName + " : handleQmiServiceUp()");

  if (mOwner.getState() != ModemEndPoint::State::OPERATIONAL) {
    qmi_client_error_type rc;
    qmi_service_info info;
    rc = qmi_client_get_service_instance(mServiceObject,
         QMI_CLIENT_INSTANCE_MODEM, &info);
    Log::getInstance().d(
        "[TMDModemEndPointModule]: " + mOwner.mName + " : qmi_client_get_service_instance returns : "
        + std::to_string(rc));
    if (rc == QMI_NO_ERR) {
      if (mQmiNotifyHandle) {
        Log::getInstance().d("[TMDModemEndPointModule]: Relase QMI notify handle for " + mOwner.mName);
        qmi_client_release(mQmiNotifyHandle);
        mQmiNotifyHandle = nullptr;
      }

      GenericCallback<string> cb ([](std::shared_ptr<Message> qmiSetupRequest,
                                             Message::Callback::Status status,
                                             std::shared_ptr<string> rsp) -> void {
        if (qmiSetupRequest && status == Message::Callback::Status::SUCCESS && rsp)
          Log::getInstance().d("TMDModemEndPointModule::QmiSetupRequest: Callback executed.");
        else
          Log::getInstance().d("TMDModemEndPointModule:: QmiSetupRequest callback response error");
      });

      auto shared_setupMsg = std::make_shared<QmiSetupRequest>(
           "tmd-token-client-server", 0, nullptr, mInstanceId, &cb);
      if( shared_setupMsg != NULL ) {
        Log::getInstance().d("[TMDModemEndPointModule]: Dispatching QmiSetupRequest message");
        this->dispatch(shared_setupMsg);
      }
      else
      {
        Log::getInstance().d("[TMDModemEndPointModule]: QmiSetupRequest dispatch error");
      }
    }
  }
}
