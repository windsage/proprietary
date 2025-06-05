/**
 * Copyright (c) 2018, 2020, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "interface.h"
#include <utils/Log.h>

static android::sp<ISecureElementHalCallback> seCallback;

Return<void> setSeCallBack(const android::sp<ISecureElementHalCallback>& clientCallback) {
  seCallback = clientCallback;
  return Void();
}

Return<void> initSEService() {

  Return<void> hidlStatus;

  if(!seCallback) {
     ALOGE("secure element callback not registered yet, skip initialization");
     return Void();
  }

  TEEC_Result result = gpqese_open();
  if (result == TEEC_SUCCESS) {
      hidlStatus = seCallback->onStateChange(true);
  } else {
      hidlStatus = seCallback->onStateChange(false);
  }
  if(!hidlStatus.isOk()) {
    ALOGE("%s: onStateChange callback Failed !", __func__);
  }

  return Void();
}
