/**
 * Copyright (c) 2018, 2020-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "SecureElement.h"
#include "eSEClient.h"
#include "EseUtils/EseUtils.h"
#include <utils/Log.h>

namespace android {
namespace hardware {
namespace secure_element {
namespace V1_2 {
namespace implementation {

static std::mutex m;
static bool cardPresent = false;

sp<ISecureElementHalCallback> SecureElement::clientCB = NULL;
sp<ISecureElementHalCallbackV1_1> SecureElement::clientCBV1_1 = NULL;

// Methods from ::android::hardware::secure_element::V1_0::ISecureElement follow.
Return<void> SecureElement::init(const
    sp<ISecureElementHalCallback> &clientCallback) {
  Return<void> hidlStatus;

  if(clientCallback == NULL) {
    ALOGE("%s: Received NULL client callback handle", __func__);
    return Void();
  }

  if(ese_update != ESE_UPDATE_COMPLETED) {
    ALOGE("%s: JCOP or Loader service download is in progress", __func__);
    clientCB = clientCallback;
    hidlStatus = clientCB->onStateChange(false);
    if(!hidlStatus.isOk()) {
      ALOGE("%s: onStateChange callback Failed !", __func__);
      return Void();
    }
    cardPresent = false;
    setSeCallBack(clientCallback);
    return Void();
  }

  clientCallback->linkToDeath(this, 0 /*cookie*/);

  TEEC_Result result = ese_init();
  if (result == TEEC_SUCCESS) {
    clientCB = clientCallback;
    hidlStatus = clientCB->onStateChange(true);
    if(!hidlStatus.isOk()) {
      ALOGE("%s: onStateChange callback Failed !", __func__);
      return Void();
    }
    cardPresent = true;
    return Void();
  }

  ALOGE("%s: eSE initialisation FAILED", __func__);
  return Void();
}

Return<void> SecureElement::getAtr(getAtr_cb _hidl_cb) {
  uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
  hidl_vec<uint8_t> response;
  uint32_t rspLen = 0;
  TEEC_Result result;
  TEEC_Result res_gpese_open;

  res_gpese_open = gpqese_open();
  if (res_gpese_open != TEEC_SUCCESS) {
    ALOGE("%s: gpqese_open failed", __func__);
    goto ret_handle;
  }

  result = gpqese_getATR(rApdu, &rspLen);
  if (result == TEEC_SUCCESS) {
    response.resize(rspLen);
    memscpy(&response[0], rspLen, rApdu, rspLen);
    goto ret_handle;
  } else {
    ALOGE("%s: getATR Failed !", __func__);
  }

ret_handle:
  if (gpqese_close(res_gpese_open) != TEEC_SUCCESS) {
      ALOGE("%s: gpqese_close failed", __func__);
  }
  _hidl_cb(response);
  return Void();
}

Return<bool> SecureElement::isCardPresent() {
    return cardPresent;
}

Return<void> SecureElement::transmit(const hidl_vec<uint8_t> &data,
                                     transmit_cb _hidl_cb) {
  std::lock_guard<std::mutex> lock(m);

  uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
  uint8_t cApdu[GPQESE_MAX_CAPDU_SIZE] = {};
  int rLen = 0;
  hidl_vec<uint8_t> respApdu;
  hidl_vec<uint8_t> cmdApdu;
  TEEC_Result result;
  uint32_t cmdLen;

  OsuHalExtn::OsuApduMode mode = IS_OSU_MODE(
      data, OsuHalExtn::getInstance().TRANSMIT, &cmdLen, cApdu);

  if (mode == OsuHalExtn::getInstance().OSU_BLOCKED_MODE) {
    // Non-OSU APDU request while update in progress
    ALOGE("%s: Not allowed, OSU update in progress", __func__);
    _hidl_cb(respApdu);
    return Void();
  } else if ((mode == OsuHalExtn::getInstance().OSU_RST_MODE) ||
             (mode == OsuHalExtn::getInstance().OSU_ACK_SELECT_MODE)) {
    // Request for hard reset or
    // select request with basic channel and aid NULL from OSU jar
    // return dummy success response as this command not sent further
    uint8_t sw[2] = {0x90, 0x00};
    respApdu.resize(sizeof(sw));
    memscpy(&respApdu[0], sizeof(sw), sw, sizeof(sw));
    _hidl_cb(respApdu);
    return Void();
  } else if (mode == OsuHalExtn::getInstance().OSU_PROP_MODE) {
    // Prop APDU - not compliant with GP
    cmdApdu.resize(cmdLen);
    memscpy(&cmdApdu[0], cmdLen, cApdu, cmdLen);
    result = transmitApdu(cmdApdu, true, BASIC_CHANNEL_NUM, rApdu, &rLen);
  } else {
    // GP compliant APDU
    result = transmitApdu(data, false, INVALID_CHANNEL_NUM, rApdu, &rLen);
  }

  if (result == TEEC_SUCCESS) {
    respApdu.resize(rLen);
    memscpy(&respApdu[0], rLen, rApdu, rLen);
    print_text("Received :", &respApdu[0], rLen);
  } else {
      ALOGE("%s: Transmit FAILED !", __func__);
  }

  _hidl_cb(respApdu);
  return Void();
}

static Return<SecureElementStatus> doCloseChannel(uint8_t channelNumber) {
  TEEC_Result result = closeGPChannel(channelNumber);
  if (result == TEEC_SUCCESS) {
    return SecureElementStatus::SUCCESS;
  }

  ALOGE("%s: Close channel Failed for %d", __func__, channelNumber);
  return SecureElementStatus::FAILED;
}

Return<void> SecureElement::openLogicalChannel(const hidl_vec<uint8_t> &aid,
    uint8_t p2, openLogicalChannel_cb _hidl_cb) {
  std::lock_guard<std::mutex> lock(m);

  uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
  SecureElementStatus status = SecureElementStatus::FAILED;
  LogicalChannelResponse resApduBuff;
  memset(&resApduBuff, 0x00, sizeof(resApduBuff));
  resApduBuff.channelNumber = 0xff;
  int rLen = 0;
  int cNumber = 0;

  if (IS_OSU_MODE(OsuHalExtn::getInstance().OPENLOGICAL)) {
    ALOGE("%s: Not allowed, OSU update in progress", __func__);
    _hidl_cb(resApduBuff, SecureElementStatus::IOERROR);
    return Void();
  }

  TEEC_Result result = openChannel(aid, p2, &cNumber, rApdu, &rLen, false);
  if ((result == TEEC_SUCCESS) && (rLen > 1)) {
    resApduBuff.channelNumber = cNumber;
    resApduBuff.selectResponse.resize(rLen);
    memscpy(&resApduBuff.selectResponse[0], rLen, rApdu, rLen);
    print_text("OpenLogical Success : Select Response :",
               &resApduBuff.selectResponse[0], rLen);
    status = SecureElementStatus::SUCCESS;
  } else if (result == TEEC_ERROR_OUT_OF_MEMORY) {
    ALOGE("%s: - No more channel available", __func__);
    resApduBuff.channelNumber = 0xff;
    status = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
  } else if (result == TEEC_ERROR_ITEM_NOT_FOUND) {
    ALOGE("%s: - Applet not found", __func__);
    resApduBuff.channelNumber = cNumber;
    status = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
    doCloseChannel(resApduBuff.channelNumber);
  }

  _hidl_cb(resApduBuff, status);
  return Void();
}

Return<void> SecureElement::openBasicChannel(const hidl_vec<uint8_t> &aid,
    uint8_t p2, openBasicChannel_cb _hidl_cb) {
  std::lock_guard<std::mutex> lock(m);

  uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};
  SecureElementStatus status = SecureElementStatus::FAILED;
  hidl_vec<uint8_t> resApduBuff;
  int rLen = 0;
  int cNumber = 0;
  OsuHalExtn::OsuApduMode mode;
  TEEC_Result result;
  const hidl_vec<uint8_t> dummy_aid;

  mode = IS_OSU_MODE(aid, OsuHalExtn::getInstance().OPENBASIC, NULL);
  if (mode == OsuHalExtn::OSU_PROP_MODE) {
    // start of JCOP OSU update
    // enable cold reset protection
    if (do_cold_reset_protection(true))
    {
       ALOGE("%s: cold reset protection failed", __func__);
       status = SecureElementStatus::IOERROR;
       IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
      _hidl_cb(resApduBuff, status);
      return Void();
    }
    // aid coming from OSU framework is not supported by JCOP
    result = openChannel(dummy_aid, p2, &cNumber, rApdu, &rLen, true);
  } else if (mode == OsuHalExtn::OSU_BLOCKED_MODE) {
    ALOGE("%s: Not allowed, OSU update in progress", __func__);
    _hidl_cb(resApduBuff, SecureElementStatus::IOERROR);
    return Void();
  } else {
    result = openChannel(aid, p2, &cNumber, rApdu, &rLen, true);
  }

  if (result == TEEC_SUCCESS) {
    resApduBuff.resize(rLen);
    memscpy(&resApduBuff[0], rLen, rApdu, rLen);
    print_text("OpenBasic Success : Select Response :", &resApduBuff[0], rLen);
    status = SecureElementStatus::SUCCESS;
    if (mode == OsuHalExtn::OSU_PROP_MODE) {
      // update TEE that OSU update started
      if (updateOSUState(OSU_STATE_STARTED)) {
        ALOGE("%s: Unable to notify TEE", __func__);
        IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
        doCloseChannel(0);
        status = SecureElementStatus::IOERROR;
        _hidl_cb(resApduBuff, status);
        return Void();
      }

      ALOGD("%s: send HARD reset, getATR and IFSC", __func__);
      if (resetJcopUpdate()) {
        ALOGE("%s: JCOP reset failed", __func__);
        IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
        doCloseChannel(0);
        status = SecureElementStatus::IOERROR;
      }
    }
  } else if (result == TEEC_ERROR_OUT_OF_MEMORY) {
    ALOGE("%s: - No more channel available", __func__);
    status = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
  } else if (result == TEEC_ERROR_ITEM_NOT_FOUND) {
    ALOGE("%s: - Applet not found", __func__);
    status = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
    doCloseChannel(0);
  } else {
    ALOGE("%s: - unexpected error", __func__);
    status = SecureElementStatus::IOERROR;
  }

  _hidl_cb(resApduBuff, status);
  return Void();
}

Return<SecureElementStatus> SecureElement::closeChannel(uint8_t channelNumber) {
  std::lock_guard<std::mutex> lock(m);

  // update OSU state
  IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, channelNumber);
  return doCloseChannel(channelNumber);
}

// Methods from ::android::hardware::secure_element::V1_1::ISecureElement follow.
Return<void> SecureElement::init_1_1(const
    sp<ISecureElementHalCallbackV1_1> &clientCallback) {
  Return<void> hidlStatus;

  if(clientCallback == NULL) {
    ALOGE("%s: Received NULL client callback handle", __func__);
    return Void();
  }

  if(ese_update != ESE_UPDATE_COMPLETED) {
    ALOGE("%s: JCOP or Loader service download is in progress", __func__);
    clientCBV1_1 = clientCallback;
    hidlStatus = clientCBV1_1->onStateChange_1_1(false, "eSE Not Ready yet");
    if(!hidlStatus.isOk()) {
      ALOGE("%s: onStateChange_1_1 callback Failed !", __func__);
      return Void();
    }
    cardPresent = false;
    setSeCallBackV1_1(clientCallback);
    return Void();
  }

  clientCallback->linkToDeath(this, 0 /*cookie*/);

  TEEC_Result result = ese_init();
  if (result == TEEC_SUCCESS) {
    IS_OSU_MODE(OsuHalExtn::getInstance().INIT, 0);
    clientCBV1_1 = clientCallback;
    hidlStatus = clientCBV1_1->onStateChange_1_1(true, "eSE has been initialised");
    if(!hidlStatus.isOk()) {
      ALOGE("%s: onStateChange_1_1 callback Failed !", __func__);
      return Void();
    }
    cardPresent = true;
    return Void();
  }

  ALOGE("%s: eSE initialisation FAILED", __func__);
  return Void();
}

// Methods from ::android::hardware::secure_element::V1_2::ISecureElement follow.
Return<SecureElementStatus> SecureElement::reset() {
  TEEC_Result result = TEEC_SUCCESS;
  Return<void> hidlStatus;

  hidlStatus = clientCBV1_1->onStateChange_1_1(false, "re-initilaise eSE");
  if(!hidlStatus.isOk()) {
    ALOGE("%s: onStateChange_1_1 callback Failed !", __func__);
    return SecureElementStatus::FAILED;
  }
  cardPresent = false;
  result = gpqese_close(result);
  if(result == TEEC_SUCCESS) {
    if(ese_init() == TEEC_SUCCESS) {
      hidlStatus = clientCBV1_1->onStateChange_1_1(true, "eSE re-initialised");
      if(!hidlStatus.isOk()) {
        ALOGE("%s: onStateChange_1_1 callback Failed !", __func__);
        return SecureElementStatus::FAILED;
      }
      cardPresent = true;
      return SecureElementStatus::SUCCESS;
    }
  }

  ALOGE("%s: eSE re-initialisation FAILED", __func__);
  return SecureElementStatus::FAILED;
}

void SecureElement::serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) {
  TEEC_Result result = TEEC_SUCCESS;
  ALOGE("%s: SE client service died!", __func__);

  if (eseGetVendorId() == ESE_VENDOR_NXP) {
    // service died while OSU in progress ?
    // disable reset protection and notify OSU state to TEE
    IS_OSU_MODE(OsuHalExtn::getInstance().CLOSE, 0);
  }

  // close session
  if (gpqese_close(result) != TEEC_SUCCESS) {
    ALOGE("%s: session close request failed", __func__);
  }
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace secure_element
}  // namespace hardware
}  // namespace android
