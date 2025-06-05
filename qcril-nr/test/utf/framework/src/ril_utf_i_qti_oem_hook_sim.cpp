/******************************************************************************
  @file    ril_utf_i_qti_oem_hook_sim.cpp
  @brief   RIL UTF OEMHOOK AIDL implementation

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <android/binder_status.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include "ibinder_internal.h"

#include <vector>

#include "aidl/vendor/qti/hardware/radio/qcrilhook/IQtiOemHook.h"
#include "aidl/vendor/qti/hardware/radio/qcrilhook/IQtiOemHookResponse.h"
#include "aidl/vendor/qti/hardware/radio/qcrilhook/IQtiOemHookIndication.h"

#ifdef QMI_RIL_HAL_UTF
#include "ril_utf_core.h"
#endif

#include "ril_utf_oem_sim.h"

namespace qtioemhookaidl {
  using namespace aidl::vendor::qti::hardware::radio::qcrilhook;
}

static std::shared_ptr<qtioemhookaidl::IQtiOemHook> utfQtiOemHookAidl = nullptr;

void send_i_oemhook_enqueue_oem_expect(int size, void *payload, int msg_type, int msg_token, uint32_t oem_error) {
  int my_expect_slot;

  // acquire mutex
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: OEM recv thread locking exec_q_access in %s", __func__);
  EXEC_Q_LOCK();

  // Grab next available slot in expect table
  for (int i = 0; i < MAX_EXPECTS; ++i)
  {
    if (expect_table[i].in_use == 0)
    {
      my_expect_slot = i;
      expect_table[my_expect_slot].in_use = 1;
      break;
    }
  }

  // verify that we are allowed to proceed
  while ( expect_table[my_expect_slot].expect_ready != 1 )
  {
    struct timeval e_timeout;
    struct timespec thread_timeout;
    e_timeout.tv_sec = 0;
    e_timeout.tv_usec = EXPECT_TIMEOUT;
    calc_timeout(&thread_timeout, &e_timeout);
    pthread_cond_timedwait(&expect_ready_sig, &exec_q_access, &thread_timeout);
  }

  // Decode message and enqueue
  handle_oem_msg(size, (uint8_t *)payload, msg_type, msg_token, oem_error);

  // signal test thread.
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: Setting expect_received to true @ %s %s %d ",__FILE__, __func__, __LINE__);
  expect_table[my_expect_slot].expect_received = 1;
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: Signalling Expectation");
  pthread_cond_signal(&signal_exptectation);

  // release mutex
  EXEC_Q_UNLOCK();
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: %s. released exec_q_mutex.", __func__);
}

class IQtiOemHookAidlResponseClientImpl : public qtioemhookaidl::IQtiOemHookResponse {
  public:
    ::ndk::ScopedAStatus oemHookRawResponse(int32_t in_serial, ::qtioemhookaidl::RadioError in_error,
        const std::vector<uint8_t>& in_data) {
      RIL_UTF_DEBUG("\n QMI_RIL_UTL: oemHookRawResponse");
      uint8_t *payload = new uint8_t[in_data.size()];
      for (int i = 0; i < in_data.size(); i++) {
        payload[i] = in_data[i];
      }
      send_i_oemhook_enqueue_oem_expect(in_data.size(), (void *)payload, 0, in_serial,
          (uint32_t)in_error);
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) {
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) {
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::SpAIBinder asBinder() {
      return ::ndk::SpAIBinder();
    }

    bool isRemote() {
      return false;
    }
};

class IQtiOemHookAidlIndicationClientImpl : public qtioemhookaidl::IQtiOemHookIndication {
  public:
    ::ndk::ScopedAStatus oemHookRawIndication(const std::vector<uint8_t>& in_data) {
      RIL_UTF_DEBUG("\n QMI_RIL_UTL: oemHookRawIndication");
      uint8_t *payload = new uint8_t[in_data.size()];
      RIL_UTF_DEBUG("\n QMI_RIL_UTL: in_data.size = %d", in_data.size());

      for (int i = 0; i < in_data.size(); i++) {
          payload[i] = in_data[i];
      }
      send_i_oemhook_enqueue_oem_expect(in_data.size(), (void *)payload, 1, 0, 0);
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) {
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) {
      return ndk::ScopedAStatus::ok();
    }

    ::ndk::SpAIBinder asBinder() {
      return ::ndk::SpAIBinder();
    }

    bool isRemote() {
      return false;
    }
};

static std::shared_ptr<IQtiOemHookAidlResponseClientImpl> mOemHookRespClient;
static std::shared_ptr<IQtiOemHookAidlIndicationClientImpl> mOemHookIndClient;

void reset_i_qti_oem_hook() {
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: reset_i_qti_oem_hook");

  if (utfQtiOemHookAidl!= nullptr) {
    RIL_UTF_DEBUG("\n QMI_RIL_UTL: calling clear callback");
    utfQtiOemHookAidl->setCallback(nullptr, nullptr);
    utfQtiOemHookAidl = nullptr;
  }
}

void connect_to_i_qti_oem_hook() {
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: connect_to_i_qti_oem_hook");
  while (utfQtiOemHookAidl == nullptr) {
    auto qtiOemHookBinder = ::ndk::SpAIBinder(AServiceManager_getService(
          "vendor.qti.hardware.radio.qcrilhook.IQtiOemHook/oemhook0"));
    utfQtiOemHookAidl = qtioemhookaidl::IQtiOemHook::fromBinder(qtiOemHookBinder);
    if (utfQtiOemHookAidl != nullptr) {
      RIL_UTF_DEBUG("\n QMI_RIL_UTF: calling setCallbacks for QtiOemHookAidl");
      mOemHookRespClient = ndk::SharedRefBase::make<IQtiOemHookAidlResponseClientImpl>();
      mOemHookIndClient = ndk::SharedRefBase::make<IQtiOemHookAidlIndicationClientImpl>();
      utfQtiOemHookAidl->setCallback(mOemHookRespClient, mOemHookIndClient);
      RIL_UTF_DEBUG("\n QMI_RIL_UTF: get(IQtiOemHookAidl) registration success");
      break;
    }
    RIL_UTF_DEBUG("\n QMI_RIL_UTF: get(IQtiOemHookAidl) returned null");
    usleep(1);
  }
}

void send_i_oemhook_request(int serial, char *msg, int len) {
  RIL_UTF_DEBUG("\n QMI_RIL_UTL: send_i_oemhook_request");
  if (utfQtiOemHookAidl != nullptr) {
    std::vector<uint8_t> data = {};
    for (int i = 0; i < len; i++) {
      data.push_back(msg[i]);
    }
    utfQtiOemHookAidl->oemHookRawRequest(serial, data);
  }
}
