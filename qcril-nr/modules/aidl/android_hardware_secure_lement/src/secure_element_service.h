/******************************************************************************
#  Copyright (c) 2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "BnSecureElementDefault.h"
#include "QtiMutex.h"
#include "framework/legacy.h"
#include "modules/qmi/UimQmiUimIndicationMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include <binder/IBinder.h>

#undef TAG
#define TAG "RILQ"

class SecureElementAidlModule;

namespace aidlimports {
using namespace aidl::android::hardware::secure_element;
}

typedef enum {

  SE_STATUS_SUCCESS = 0x00,

  SE_STATUS_FAILED = 0x01,

  SE_STATUS_CHNL_NOT_AVAILABLE = 0x02,

  SE_STATUS_NO_SUCH_ELEMENT_ERROR = 0x03,

  SE_STATUS_UNSUPPORTED_OPERATION = 0x04,

  SE_STATUS_IOERROR = 0x05,
} SecureElementStatus;

typedef struct {
  boolean in_use;
  uint16_t total_apdu_len;
  uint32_t token;
  uint16_t rx_len;
  uint8_t *apdu_ptr;
} secure_element_long_apdu_info_type;

class ISecureElementImpl : public aidlimports::BnSecureElementDefault {
private:
  qcril_instance_id_e_type mInstanceId;
  qtimutex::QtiSharedMutex mCallbackLock;
  AIBinder_DeathRecipient *mDeathRecipient = nullptr;
  AIBinder_DeathRecipient *mSecureElementDeathRecipient = nullptr;
  void clearCallbacks_nolock();
  void clearCallbacks();
  SecureElementAidlModule *mModule;
  SecureElementStatus convertQmiErrSecureElementStatus(
      int qmi_err_code, qmi_uim_logical_channel_rsp_type logical_channel_rsp);
  SecureElementStatus
  openChannelRequest(const std::vector<uint8_t> &aid, uint8_t p2,
                     qmi_uim_logical_channel_rsp_type *rsp_data_ptr);

public:
  ISecureElementImpl(SecureElementAidlModule *module);
  ~ISecureElementImpl();

  void deathNotifier(void *cookie);

  void setInstanceId(qcril_instance_id_e_type instanceId);
  int getInstanceId();

  std::vector<uint8_t> ch_id_list;
  qtimutex::QtiSharedMutex ch_id_list_lock;
  std::shared_ptr<aidlimports::ISecureElementCallback> mSecureElementRespCb;
  std::shared_ptr<aidlimports::ISecureElementCallback>
  getSecureElementResponseCallback();
  void handleSecureElementModuleReady(void);

  ::ndk::ScopedAStatus closeChannel(int8_t in_channelNumber) override;

  ::ndk::ScopedAStatus getAtr(std::vector<uint8_t> *response) override;

  ::ndk::ScopedAStatus
  init(const std::shared_ptr<::aidlimports::ISecureElementCallback>
           &in_clientCallback) override;

  ::ndk::ScopedAStatus isCardPresent(bool *_aidl_return) override;

  ::ndk::ScopedAStatus
  openBasicChannel(const std::vector<uint8_t> &in_aid, int8_t in_p2,
                   std::vector<uint8_t> *response) override;

  ::ndk::ScopedAStatus
  openLogicalChannel(const std::vector<uint8_t> &in_aid, int8_t in_p2,
                     ::aidlimports::LogicalChannelResponse *response) override;

  ::ndk::ScopedAStatus reset() override;

  ::ndk::ScopedAStatus transmit(const std::vector<uint8_t> &in_data,
                                std::vector<uint8_t> *response) override;
};
