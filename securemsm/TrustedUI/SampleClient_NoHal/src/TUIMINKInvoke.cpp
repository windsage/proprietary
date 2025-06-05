/*
 * Copyright (c) 2022-2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "thread"
#include "object.h"
#include "memscpy.h"
#include "impl_base.hpp"
#include "proxy_base.hpp"
#include "ITrustedUIApp.hpp"
#include "LoadTVMAppWrapper.h"
#include "TUIMINKInvoke.h"
#include "TUIUtils.h"
#include "IHLOSListenerCBO.hpp"
#include "CHLOSListenerCBO_open.h"
#include <utils/Log.h>

#define LOG_TAG "MultiVMsTUIClient:MINKInvoke"

using android::sp;

int32_t TUIMINKInvoke::_loadApp(const uint32_t uid, std::string appName,
                                const hidl_memory& appBin, bool isTUIAppinOEMVM)
{
    int32_t ret = -1;
    ret = _loadTUIApp(&mTUIAppHandle, uid, appName, appBin, isTUIAppinOEMVM);
    if (ret || (mTUIAppHandle == nullptr)) {
        ALOGE("%s: App load failed ret:%d", __func__, ret);
        ret = -1;
    }
    return ret;
}

int32_t TUIMINKInvoke::_unloadApp()
{
    int32_t ret = -1;
    ret = _unloadTUIApp(mTUIAppHandle);
    if (ret) {
        ALOGE("%s: App Unload failed ret:%d", __func__, ret);
        goto end;
    }
    mTUIAppHandle = nullptr;
end:
    return ret;
}

int32_t TUIMINKInvoke::createSession(std::string appName, int32_t dpyIdx,
                      uint32_t inputType, sem_t *sem, trusted_vm_event_t* vmStatus,
                      uint32_t *sessionId, const uint32_t uid, const hidl_memory& appBin, bool isTUIAppinOEMVM)
{
    int32_t ret = 0;
    Object listenerObj = Object_NULL;
    HLOSListenerCBO *hlosListenerCB = nullptr;

    mAppName = appName;
    mSessionNotification = sem;

    TUI_CHECK_ERR((0 == _loadApp(uid, mAppName, appBin, isTUIAppinOEMVM)), -1);
    ALOGD("%s: Load App succeded handle : 0x%x", __func__, mTUIAppHandle);

    //Create CBO for VM Notifications
    ret = CHLOSListenerCBO_open(&listenerObj, mSessionNotification, vmStatus);
    if (Object_isERROR(ret) || Object_isNull(listenerObj)) {
        ALOGE("CHostListenerCBO_open failed with error: %d!", ret);
        ret = -1;
        goto end;
    }

    ALOGD("%s: call createSession on app ++", __func__);
    hlosListenerCB = new HLOSListenerCBO(listenerObj);
    ret = mTUIAppHandle->createSession(dpyIdx, inputType, *hlosListenerCB);

    TUI_CHECK(ret == 0);

    //Assign a unique sessionId to this session
    *sessionId = (uint64_t)mTUIAppHandle;
    mTEESessionId = *sessionId;
    ALOGI("%s:Session created successfully sessionId:%u", __func__, *sessionId);
end:

    ALOGI("%s: Release Lisetener Obj ++", __func__);
    if (hlosListenerCB != nullptr) {
        delete hlosListenerCB;
        hlosListenerCB = nullptr;
    }

    if (ret) {
        int32_t ret2 = -1;
        ret2 = _unloadApp();
        ALOGE("%s: _unloadApp ret:%d", __func__, ret2);
    }
    return ret;
}

int32_t TUIMINKInvoke::startSession(uint32_t sessionId, vmSessionConfig cfg)
{
    int32_t ret = 0;
    TUIConfig tuiCfg;

    TUI_CHECK(mTUIAppHandle != nullptr);
    TUI_CHECK(cfg.layoutName.size() <= sizeof(tuiCfg.layoutName));
    memscpy(tuiCfg.layoutName, sizeof(tuiCfg.layoutName), (void*)cfg.layoutName.c_str(), cfg.layoutName.size());
    tuiCfg.useSecureIndicator = cfg.useSecureIndicator;
    tuiCfg.enableSessionAuth = cfg.enableFrameAuth;
    ALOGD("%s::%d Layout name - %s, useSecureIndicator - %d, enableSessionAuth - %d", __func__, __LINE__,
          (char *)tuiCfg.layoutName, tuiCfg.useSecureIndicator, tuiCfg.enableSessionAuth);
    ret = mTUIAppHandle->startSession(tuiCfg);
end:
    return ret;
}

int32_t TUIMINKInvoke::stopSession(uint32_t sessionId)
{
    int32_t ret = -1;
    ALOGD("%s++", __func__);
    TUI_CHECK(mTUIAppHandle != nullptr);
    TUI_CHECK_ERR((ret = mTUIAppHandle->stopSession()) == 0, ret);
end:
    return ret;
}

int32_t TUIMINKInvoke::deleteSession(uint32_t sessionId)
{
    int32_t ret = -1;
    ALOGD("%s++", __func__);
    TUI_CHECK(mTUIAppHandle != nullptr);
    if ((ret = mTUIAppHandle->deleteSession()) != 0) {
        ALOGE("%s deleteSession in VM failed with ret : %d", __func__, ret);
    }
    ALOGD("%s: unloadApp ++", __func__);
    _unloadApp();
end:
    return ret;
}

int32_t TUIMINKInvoke::sendCmd(uint32_t sessionId, uint32_t commandId,
        const std::vector<uint8_t> &commandData, std::vector<uint8_t> &responseData)
{
    int32_t ret = -1, i = 0;
    ALOGD("%s++", __func__);
    size_t respLen = 0, cmdSize = 0;
    uint32_t cmd_id = commandId;
    std::vector<uint8_t> cmdData = commandData;
    TUI_CHECK(mTUIAppHandle != nullptr);
    TUI_CHECK(responseData.size() >= sizeof(uint32_t));

    //append cmdId to commandData
    ALOGD("sending cmd: %d", commandId);
    for (i = 0; i < 4 ; i++) {
        auto it = cmdData.begin();
        it = cmdData.insert(it, cmd_id & 0xFF);
        cmd_id = cmd_id >> 8;
    }

    //If commandData is empty, an extra 1 byte is required to fit the command of large data from App.
    if (commandData.empty()) {
        cmdSize = cmdData.size() + sizeof(uint8_t);
    } else {
        cmdSize = cmdData.size();
    }

    ret = mTUIAppHandle->handleCmd((void*)cmdData.data(), cmdSize,
            (void*)responseData.data(), responseData.size(), &respLen);

    TUI_CHECK(ret == 0);
end:
    return ret;
}
