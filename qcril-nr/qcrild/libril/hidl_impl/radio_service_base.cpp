/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "framework/Log.h"
#include "hidl_impl/radio_service_base.h"
#include "modules/android/ClientDisconnectedMessage.h"

#undef TAG
#define TAG "RILQ"

/**
 * Handler function for hidl death notification.
 * From hidl_death_recipient
 */
void RadioServiceBase::serviceDied(uint64_t,
                                   const ::android::wp<::android::hidl::base::V1_0::IBase>&) {
  QCRIL_LOG_INFO("serviceDied");
  clearCallbacks();

  /* Telephony died, send Unsol client disconnected indication to RIL */
  auto msg = std::make_shared<ClientDisconnectedMessage>(mInstanceId);
  if (msg) {
    msg->broadcast();
  }
}

/**
 * Generate RadioServiceContext
 */
std::shared_ptr<RadioServiceContext> RadioServiceBase::getContext(uint32_t serial) {
  return std::make_shared<RadioServiceContext>(mInstanceId, serial);
}

/**
 * Set instance id
 */
void RadioServiceBase::setInstanceId(qcril_instance_id_e_type instId) {
  mInstanceId = instId;
}

qcril_instance_id_e_type RadioServiceBase::getInstanceId() {
  return mInstanceId;
}
