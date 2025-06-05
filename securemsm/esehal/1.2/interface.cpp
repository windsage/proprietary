/**
 * Copyright (c) 2018, 2020, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "interface.h"
#include <utils/Log.h>

static android::sp<ISecureElementHalCallback> seCallback = nullptr;
static android::sp<ISecureElementHalCallbackV1_1> seCallbackV1_1 = nullptr;

Return<void> setSeCallBack(const android::sp<ISecureElementHalCallback>& clientCallback) {
  seCallback = clientCallback;
  return Void();
}

Return<void> setSeCallBackV1_1(const android::sp<ISecureElementHalCallbackV1_1>& clientCallback) {
  seCallbackV1_1 = clientCallback;
  return Void();
}

Return<void> initSEService() {

  Return<void> hidlStatus;

  if(!seCallback && !seCallbackV1_1) {
     ALOGE("secure element callback not registered yet, skip initialization");
     return Void();
  }

  TEEC_Result result = gpqese_open();
  if (result == TEEC_SUCCESS) {
    if(seCallbackV1_1)
      hidlStatus = seCallbackV1_1->onStateChange_1_1(true, "GPQeSE open successful");
    else if(seCallback)
      hidlStatus = seCallback->onStateChange(true);
  } else {
    if(seCallbackV1_1)
      hidlStatus = seCallbackV1_1->onStateChange_1_1(false, "GPQeSE open failed");
    else if(seCallback)
      hidlStatus = seCallback->onStateChange(false);
  }
  if(!hidlStatus.isOk()) {
    ALOGE("%s: onStateChange callback Failed !", __func__);
  }

  return Void();
}
