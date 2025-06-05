/*========================================================================
  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <inttypes.h>
#include <stdint.h>
#include <mutex>
#include <string>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <glib.h>

#include "CAppClient.h"
#include "IAppClient.h"
#include "IClientEnv.h"
#include "IOpener.h"
#include "IConfirmationToken.h"
#include "CConfirmationToken.h"

#include "object.h"
#include "proxy_base.hpp"
#include "impl_base.hpp"
#include "CTrustedUIApp.hpp"
#include "CTPowerService.hpp"
#include "IHLOSListenerCBO.hpp"
#include "ITPowerService.hpp"
#include "ITUICoreService.hpp"
#include "ITrustedUIApp_invoke.hpp"
#include "ITuiComposer.h"
#include "TUIErrorCodes.h"
#include "TUILog.h"
#include "TUIUtils.h"
#include "TrustedUILib.h"
#include "fdwrapper.h"
#include "memscpy.h"
#include "secure_memset.h"
#include "minkipc.h"

extern Object gTVMEnv;
Object gFontFdObject = Object_NULL;
#define LOG_TAG "VM_CTrustedUIApp"

#define ENABLE_LM

#define MAX_SESSIONS 1
#define NUM_TUI_SD_BUFFERS 3

#define TRUSTEDUIVM_CMD_OFFSET 0x03000000
#define TUI_CMD_SEND_PING      ( 1 | TRUSTEDUIVM_CMD_OFFSET )
#define TUI_CMD_RECV_PING 17
#define TUI_CMD_SEND_MSG 18
#define TUI_CMD_RECV_NO_MSG 22
// ConfirmationUI commands
#define TUI_CMD_SET_PROMPT_TEXT 19
#define TUI_CMD_GET_CONF_RESULT 20
#define TUI_CMD_RECV_SET_PROMPT_TEXT 21
#define TUI_HMAC_DIGEST_SIZE_SHA256 32
#define TUI_MAX_PROMPT_TEXT_LEN 256
// prompt text + "confirmation token"
#define TUI_MAX_TOKEN_LENGTH 274

#define TUI_LOCK_STR "tuiapp"

#define TUI_DISPLAY_BUFFERS 2
#define DEFAULT_WIDTH 1440
#define DEFAULT_HEIGHT 2880
#define BYTES_PER_PIXEL 4
#define DEFAULT_TIMEOUT -1

// membuf doesn't map physical pages without 2mb alignment
#ifdef ENABLE_TUI_OEM_VM
#define SIZE_2MB 0x200000
#endif

//#define ENABLE_LM
#define TIME_STOP \
    do {          \
    } while (0)
#define TIME_START \
    do {           \
    } while (0)

#define ENTER   \
    TIME_START; \
    TUILOGE("%s:++", __func__);
#define EXIT   \
    TIME_STOP; \
    TUILOGE("%s:--", __func__);

#define UNPACK_DATA(data, size, cmd)           \
    itr = 0;                                   \
    while (itr < size) {                       \
        data = data << 8;                      \
        data |= cmd[itr];                      \
        itr++;                                 \
    }

class CTrustedUIApp : public TrustedUIAppImplBase
{
   public:
    enum State : uint32_t {
        UNINITIALIZED,        // TA not loaded
        INITIALIZED,          // TA loaded, Events/resources registered
        HANDLING_INPUT,       // Input handling (processSecureInput) ongoing
        HANDLING_INPUT_DONE,  // Input handling done
        ABORTING              // Session is aborting
    };
    CTrustedUIApp();
    virtual ~CTrustedUIApp()
    {
        TUILOGD("%s: Destructor: destryoing app Object %p ", __func__, this);
        stopSession();
        deleteSession();
    }

    virtual int32_t createSession(uint32_t displayId_val,
                                  uint32_t inputMethod_val, const HLOSListenerCBO &HLOSCbo_ref);
    virtual int32_t startSession(const TUIConfig &TUIconf_ref);
    virtual int32_t stopSession();
    virtual int32_t deleteSession();
    virtual int32_t handleCmd(const void *cmd_ptr, size_t cmd_len,
                              void *response_ptr, size_t response_len,
                              size_t *response_lenout);
    virtual int32_t handleCmdLargeData(const IMemObject &cmd_ref, void* response_ptr,
                                       size_t response_len, size_t* response_lenout);
    static tuiComposerStatus_t tuiComposerCBAllocHelperBuf(
        void *pTUIAppContext, tuiComposerBufferHandle_t *handle);
    static tuiComposerStatus_t tuiComposerCBFreeHelperBuf(
        void *pTUIAppContext, tuiComposerBufferHandle_t *handle);
    static int32_t tuiComposerCBHandleCmd(
        const void* cmd_ptr, size_t cmd_len, int32_t* fontFd,
        void* rsp_ptr, size_t rsp_len, size_t* rsp_lenout);

    //Listener Object from TUI HAL
    static HLOSListenerCBO *mHLOSCBORef;

   private:
    int refs;
    uint32_t mSessionId = UINT32_MAX;
    State mState = UNINITIALIZED;
    // Size of Display Buffers
    uint64_t mSDBufSize;
    TUIDisplayConfig mDispCfg;
    char mPromptText[TUI_MAX_PROMPT_TEXT_LEN];

    ITWakeLock *mWakeLock = nullptr;

    std::mutex mLock;
    std::shared_ptr<std::thread> mTouchThread = nullptr;
    void _touchThreadHandler();
    void _stopTouchThreadIfRunning();

    //Composer defines
    std::string mLayoutName;
    tuiComposerInput_t mInput;
    ITuiComposer_t *mComposer = nullptr;
    ITuiComposerCallback_t *mComposerCB = nullptr;
    int32_t _validateDisplayBuf(tuiComposerDisplayBuffer_t *pDispBuf,
                                TUIDisplayBuffer *pBuf, uint32_t align) const;
    int32_t _cleanUpSession();
    void _cleanUpComposer();

    const char *ToString(State state);
};

HLOSListenerCBO* CTrustedUIApp::mHLOSCBORef = nullptr;

/*=============================================================*/

#ifdef __cplusplus
extern "C" {
#endif

tuiComposerStatus_t CTrustedUIApp::tuiComposerCBAllocHelperBuf(
    void *pTUIAppContext, tuiComposerBufferHandle_t *pHandle)
{
    tuiComposerStatus_t ret = TUI_COMPOSER_STATUS_OK;
    TUIResult res = TUI_OK;
    CTrustedUIApp *pSelf = (CTrustedUIApp *)pTUIAppContext;
    uint32_t align = 1;

#ifdef ENABLE_TUI_OEM_VM
    align = SIZE_2MB;
#endif

    pHandle->size = (pHandle->size + (align -1)) & (~(align - 1));

    TUI_CHECK_COND(
        (res = TUIAllocateAppBuffer(pSelf->mSessionId,
                                    (TUIBufferHandle *)pHandle)) == TUI_OK ||
            pHandle->vAddr != nullptr,
        TUI_COMPOSER_STATUS_NO_MEM);

    TUILOGD("%s: allocated helper buffer: 0x%x for size: 0x%x\n", __func__,
            pHandle->vAddr, pHandle->size);
end:
    return ret;
}

tuiComposerStatus_t CTrustedUIApp::tuiComposerCBFreeHelperBuf(
    void *pTUIAppContext, tuiComposerBufferHandle_t *pHandle)
{
    tuiComposerStatus_t ret = TUI_COMPOSER_STATUS_OK;
    TUIResult res = TUI_OK;
    CTrustedUIApp *pSelf = (CTrustedUIApp *)pTUIAppContext;

    TUI_CHECK_COND((pHandle != nullptr), TUI_COMPOSER_STATUS_BAD_PARAM);
    TUI_CHECK_COND((pHandle->vAddr != nullptr) || (pHandle->size > 0),
                   TUI_COMPOSER_STATUS_BAD_PARAM);
    TUI_CHECK_COND((pHandle->vAddr + pHandle->size >= pHandle->vAddr),
                   TUI_COMPOSER_STATUS_BAD_PARAM);

    TUI_CHECK_COND(
        (res = TUIFreeAppBuffer(pSelf->mSessionId,
                                (TUIBufferHandle *)pHandle)) == TUI_OK,
        TUI_COMPOSER_STATUS_FAILED);

    pHandle->size = 0;
    pHandle->vAddr = nullptr;

    TUILOGD("%s: allocated helper buffer: 0x%x for size: 0x%x\n", __func__,
            pHandle->vAddr, pHandle->size);
end:
    return ret;
}

int32_t CTrustedUIApp::tuiComposerCBHandleCmd(
    const void* cmd_ptr, size_t cmd_len, int32_t* fontFd,
    void* rsp_ptr, size_t rsp_len, size_t* rsp_lenout)
{
    int32_t ret = 0;
    IMemObject fontFdMemObject;

    if (!cmd_ptr || cmd_len == 0
         || !rsp_ptr || rsp_len == 0
         || !rsp_lenout) {
        TUILOGE("%s: Invalid parameter.", __func__);
        ret = -1;
        goto end;
    }

    if (mHLOSCBORef != nullptr) {
        ret = mHLOSCBORef->handleCmd(cmd_ptr, cmd_len, fontFdMemObject, rsp_ptr, rsp_len, rsp_lenout);
        if (ret) {
            TUILOGE("%s: handle command failed in HLOS Listener.", __func__);
            goto end;
        }
        gFontFdObject = fontFdMemObject.extract();
        if (Object_isNull(gFontFdObject)) {
            TUILOGE("%s: handle command doesn't support in HLOS Listener", __func__);
            goto end;
        }
        ret = Object_unwrapFd(gFontFdObject, fontFd);
        if (ret != 0 || *fontFd < 0) {
            TUILOGE("%s: listener returned invalid font Fd", __func__);
            goto end;
        }
    } else {
        TUILOGE("%s: can't call HLOS via HLOSCBO, as mHLOSCBORef is NULL.", __func__);
        ret = -1;
    }

end:
    return ret;
}

#ifdef __cplusplus
}
#endif

/*=============================================================*/
CTrustedUIApp::CTrustedUIApp() : refs(1)
{
    TUILOGD("Creating app Object %p ", this);
}

/*=============================================================*/
int32_t _calculateBufSize(uint32_t height, uint32_t width, uint32_t *bufSize, uint32_t align)
{
    int32_t ret = 0;
    uint32_t tmpSize = 0;
    *bufSize = 0;
    tmpSize = width * height;
    if (tmpSize / width != height) {
        TUILOGE("%s: overflow detected sxh (%d, %d)", __func__,
                width, height);
        ret = -1;
        goto end;
    }
    *bufSize = tmpSize;
    tmpSize *= BYTES_PER_PIXEL; //TODO : Need to compute PixelWidth
    if (tmpSize / 4 != *bufSize) {
        TUILOGE("%s: overflow detected rxd (%d, %d)", __func__,
                *bufSize, BYTES_PER_PIXEL);
        ret = -1;
        goto end;
    }
    *bufSize = (tmpSize + (align - 1)) & (~(align - 1));
    TUILOGD("%s: bufSize: %lu", __func__, bufSize);
end:
    return ret;

}

/*=============================================================*/
int32_t CTrustedUIApp::_validateDisplayBuf(tuiComposerDisplayBuffer_t *pDispBuf,
                                           TUIDisplayBuffer *pBuf, uint32_t align) const
{
    int32_t ret = 0;
    /*
     Display buffer size = stride * height * pixelwidth
     Note: stride is nothing but (width + padding)
     Pixelwidth is the depth of the pixel i.e. number of bits per pixel
    */
    TUI_CHECK_COND((_calculateBufSize(pDispBuf->config.height, pDispBuf->config.stride,
            &pDispBuf->handle.size, align) == 0), -1);

    if (pDispBuf->handle.size != pBuf->buffHandle.size) {
        TUILOGE("%s: size mismatch (%d, %d)", __func__, pDispBuf->handle.size,
                pBuf->buffHandle.size);
        ret = -1;
        goto end;
    }
    TUILOGD("%s: size  (%d, %d)", __func__, pDispBuf->handle.size,
            pBuf->buffHandle.size);

end:
    return ret;
}

/* Clean up Display and TouchInput after a session completes. */
int32_t CTrustedUIApp::_cleanUpSession()
{
    int32_t ret = Object_OK;
    TUIResult res = TUI_OK;
    TUI_CHECK_COND((res = TUIStopSession(mSessionId)) == 0, (int32_t)res);
end:
    return ret;
}

/* Clean up Composer only during stop session. */
void CTrustedUIApp::_cleanUpComposer()
{
    if (mComposer) {
        TUILOGE("%s::%d Free Composer", __func__, __LINE__);
        /* There is no need to call Composer deInit because it is anyways
         * called within destroyTUIComposer. */
        destroyTUIComposer(mComposer);
        mComposer = nullptr;
    }
    if (mComposerCB) {
        TUILOGE("%s::%d Free Composer Callback", __func__, __LINE__);
        delete mComposerCB;
        mComposerCB = nullptr;
    }
}

/*=============================================================*/
int32_t CTrustedUIApp::createSession(uint32_t displayId_val,
                                     uint32_t inputMethod_val,
                                     const HLOSListenerCBO &HLOSCbo_ref)
{
    ENTER;
    int32_t ret = Object_OK;
    uint32_t bufSize = 0;
    int32_t numSDBuffers = TUI_DISPLAY_BUFFERS;
    int32_t useCaseHeapSize = 0;
    TUIResult res = TUI_OK;
    uint32_t align = 1;
    Object wakeLockSvcObj = Object_NULL;
    ITPowerService *wakeLockService = nullptr;

#ifdef ENABLE_TUI_OEM_VM
    align = SIZE_2MB;
#endif

    TUISessionConfig cfg = {displayId_val, numSDBuffers,
                            1 /*colorFormat*/, inputMethod_val, 0,
                            -1 /*No session timeout*/, 1 /* Use system events*/};
    std::lock_guard<std::mutex> l(mLock);
    TUI_CHECK_COND(mState == UNINITIALIZED, TUI_ERR_INVALID_STATE);

    TUI_CHECK_COND(!Object_isNull(HLOSCbo_ref.get()), TUI_ERR_INVALID_PARAM);

    mHLOSCBORef = new HLOSListenerCBO(HLOSCbo_ref);
    TUI_CHECK(mHLOSCBORef != nullptr);

    ret = IOpener_open(gTVMEnv, CTPowerService_UID, &wakeLockSvcObj);
    TUI_CHECK_COND((ret == Object_OK && !Object_isNull(wakeLockSvcObj)), -1);

    wakeLockService = new ITPowerService(wakeLockSvcObj);
    TUI_CHECK_COND(wakeLockService != nullptr, TUI_ERR_OUT_OF_MEMORY);

    mWakeLock = new ITWakeLock();
    TUI_CHECK_COND(mWakeLock != nullptr, TUI_ERR_OUT_OF_MEMORY);

    ret = wakeLockService->acquireWakeLock(*mWakeLock);
    TUI_CHECK_COND(ret == Object_OK && !(mWakeLock->isNull()), -1);

    TUI_CHECK_COND(_calculateBufSize(DEFAULT_HEIGHT, DEFAULT_WIDTH, &bufSize, align) == 0,
            TUI_ERR_GENERIC_FAILURE);
    cfg.useCaseHeapSize = bufSize * (numSDBuffers + 2);
    TUI_CHECK_COND(cfg.useCaseHeapSize > 0, TUI_ERR_GENERIC_FAILURE);

    // Create a session with TUI Core Framework
    TUI_CHECK_COND((res = TUICreateSession(&cfg, &mSessionId)) == TUI_OK,
                   TUI_ERR_CORE_SVC_NOT_READY);

    mState = INITIALIZED;
    TUILOGE("%s::%d State changed to %s", __func__, __LINE__, ToString(mState));
end:
    // clean-up
    if (ret) {
        TUILOGE("%s: Error in creating session", __func__);
        if (mHLOSCBORef) {
            TUILOGE("%s: Delete my reference to listener", __func__);
            delete mHLOSCBORef;
        }

        if (mWakeLock) {
            delete mWakeLock;
            mWakeLock = nullptr;
        }
    }

    if(wakeLockService != nullptr) {
       delete wakeLockService;
       wakeLockService = nullptr;
    }

    EXIT;
    return ret;
}

void CTrustedUIApp::_touchThreadHandler()
{
    int32_t ret = Object_OK;
    int32_t retVal = Object_OK;
    TUIResult res = TUI_OK;
    int32_t touchEvenTimeout = DEFAULT_TIMEOUT;
    TUIInputData inputData;
    TUIDisplayBuffer buffer;
    TUIDisplayConfig *bufferCfg = &(buffer.config);
    tuiComposerDisplayBuffer_t dispBuf;
    uint32_t align = 1;

#ifdef ENABLE_TUI_OEM_VM
    align = SIZE_2MB;
#endif

    // Continue to process touch events
    while ((mInput.status == TUI_COMPOSER_INPUT_STATUS_CONTINUE) &&
           (ret == 0)) {
        TUILOGE("%s: Waiting on input with a timeout of : %d", __func__, touchEvenTimeout);
        TUI_CHECK_COND((res = TUIGetInputData(mSessionId, touchEvenTimeout,
                                              &inputData)) == TUI_OK,
                       (int32_t)res);

        std::lock_guard<std::mutex> l(mLock);
        //Check if the GetInput call exited due to ABORT scenario
        if (mState == ABORTING)
            break;

        buffer.buffHandle.size = mSDBufSize;
        TUILOGD("Requesting  buffer of size : %d", mSDBufSize);
        TUI_CHECK_COND(
            (res = TUIDequeueDisplayBuffer(mSessionId, &buffer)) == TUI_OK,
            (int32_t)res);

        TUILOGD("%s: bufAddr:%p, bufSize:%d", __func__, buffer.buffHandle.vAddr,
                mSDBufSize);

        dispBuf.config.width = mDispCfg.width;
        dispBuf.config.stride = mDispCfg.width;
        dispBuf.config.height = mDispCfg.height;
        dispBuf.config.format = TUI_COMPOSER_FORMAT_RGBA;
        dispBuf.handle.vAddr = buffer.buffHandle.vAddr;

        bufferCfg->width = mDispCfg.width;
        bufferCfg->height = mDispCfg.height;
        bufferCfg->stride = mDispCfg.width;
        bufferCfg->colorFormat = 1;
        bufferCfg->offHeight = 0;
        bufferCfg->offWidth = 0;

        ret = _validateDisplayBuf(&dispBuf, &buffer, align);
        TUI_CHECK_COND(ret == 0, TUI_ERR_GENERIC_FAILURE);

        mInput.cmd = TUI_COMPOSER_INPUT_CMD_DATA;
        TUILOGD("%s : %d - sizeof(inputData.data.touch) -  %d", __func__,
                __LINE__, sizeof(inputData.data.touch));
        secure_memset(&mInput.data.touch, 0, sizeof(TUITouchInput));
        memscpy((void *)&mInput.data.touch, sizeof(TUITouchInput),
                (void *)&inputData.data.touch, sizeof(inputData.data.touch));

        ret = mComposer->compose(mComposer->ctx, &mInput, &dispBuf);
        TUI_CHECK_COND(ret == TUI_COMPOSER_STATUS_OK, ret);

        TUILOGD("%s: mInput.status :%d", __func__, mInput.status);
        if (TUI_COMPOSER_INPUT_STATUS_COMPLETE == mInput.status ) {
            TUILOGD("%s: mInput.status TUI_COMPOSER_INPUT_STATUS_COMPLETE ", __func__);
        } else if ( TUI_COMPOSER_INPUT_STATUS_CANCEL == mInput.status) {
            TUILOGD("%s: mInput.status TUI_COMPOSER_INPUT_STATUS_CANCEL ", __func__);
        } else {
            TUI_CHECK_COND(
                (res = TUIEnqueueDisplayBuffer(mSessionId, &buffer)) == TUI_OK,
                (int32_t)res);
        }
    }

end :
    {
        std::lock_guard<std::mutex> l(mLock);
        if (mState != ABORTING) {
            TUILOGD("Clean Up App Session ..");
            retVal = _cleanUpSession();
            if (retVal) {
            TUILOGE("%s::%d Failed to clean up TUIApp session, retVal - %d",
                    __func__, __LINE__, retVal);
            }
            mState = HANDLING_INPUT_DONE;
        } else {
            TUILOGD("TUI Session is ABORTING");
        }
        TUILOGD("%s: state:%s ret:%d mInput.status:%d", __func__, ToString(mState),
                  ret, mInput.status);
        ret = ((ret == 0) && (mInput.status == TUI_COMPOSER_INPUT_STATUS_COMPLETE));
    }
    TUILOGD("%s: ret:%d Notify HLOS via HLOSCBO ", __func__, ret);
    (ret) ? mHLOSCBORef->onComplete() : mHLOSCBORef->onError(ret);

    TUILOGD("%s--: ret:%d ", __func__, ret);
    return;
}

int32_t CTrustedUIApp::startSession(const TUIConfig &TUIconf_ref)
{
    ENTER;
    int32_t ret = -1;
    int32_t retVal = Object_OK;
    TUIResult res = TUI_OK;
    uint32_t pixelWidth = 4;
    TUIDisplayBuffer buffer = {};
    TUIDisplayConfig *bufferCfg = &(buffer.config);
    tuiComposerParam_t composerParams;
    tuiComposerDisplayBuffer_t dispBuf;
    displayColorModes cm = {0};
    displayColorMode sRGB = {1, 1, 1};
    uint32_t align = 1;

#ifdef ENABLE_TUI_OEM_VM
    align = SIZE_2MB;
#endif

    std::lock_guard<std::mutex> l(mLock);
    TUI_CHECK_COND(mState == INITIALIZED, TUI_ERR_INVALID_STATE);

    mComposerCB = new ITuiComposerCallback_t();
    TUI_CHECK_COND(mComposerCB != nullptr, TUI_ERR_OUT_OF_MEMORY);

    mComposerCB->cbData = this;
    mComposerCB->allocHelperBuff = CTrustedUIApp::tuiComposerCBAllocHelperBuf;
    mComposerCB->freeHelperBuff = CTrustedUIApp::tuiComposerCBFreeHelperBuf;
    mComposerCB->handleCmd = CTrustedUIApp::tuiComposerCBHandleCmd;

    TUI_CHECK_COND(
        (res = TUIGetDisplayProperties(mSessionId, &mDispCfg)) == TUI_OK,
        (int32_t)res);

    mSDBufSize =
        static_cast<uint64_t>(mDispCfg.height * mDispCfg.width * pixelWidth);
    mSDBufSize = (mSDBufSize + (align -1)) & (~(align - 1));
    buffer.buffHandle.size = mSDBufSize;
    TUILOGD("%s: buffer size: %lu",__func__, mSDBufSize);

    TUI_CHECK_COND(
        (res = TUIGetColorModes(mSessionId, &cm)) == TUI_OK,
        (int32_t)res);
    for (int i = 0; i < cm.num_of_modes; i++) {
        if (memcmp(&(cm.modes[i]), &sRGB, sizeof(displayColorMode)) == 0) {
            TUILOGD("%s: setting color mode to sRGB", __func__);
            ret = TUISetColorMode(mSessionId, &sRGB);
            break;
        }
    }
    if (ret != TUI_OK) {
        TUILOGE("%s: unable to set color mode", __func__);
    }

    TUI_CHECK_COND(
        (res = TUIDequeueDisplayBuffer(mSessionId, &buffer)) == TUI_OK,
        (int32_t)res);

    mLayoutName = reinterpret_cast<char *>(
        const_cast<uint8_t *>(TUIconf_ref.layoutName));

    composerParams.layoutId = &mLayoutName[0];
    memscpy(composerParams.promptText, strlen(mPromptText), mPromptText , strlen(mPromptText));
    composerParams.width = mDispCfg.width;
    composerParams.height = mDispCfg.height;
    composerParams.format = TUI_COMPOSER_FORMAT_RGBA;
    composerParams.useSecureIndicator = TUIconf_ref.useSecureIndicator;

    if (mLayoutName.compare("pin") == 0 ||
        mLayoutName.compare("msg_pin") == 0 ||
        mLayoutName.compare("msg_user_confirmation") == 0 ||
        mLayoutName.compare("login") == 0 ||
        mLayoutName.compare("msg_login") == 0) {
        TUILOGD("%s::%d - Selecting Dialog Composer", __func__, __LINE__);
        mComposer = createTUIComposer(TUI_COMPOSER_TYPE_DIALOG);
    } else if (mLayoutName.compare("draw_dot") == 0){
        TUILOGD("%s::%d Selecting Custom Composer", __func__, __LINE__);
        mComposer = createTUIComposer(TUI_COMPOSER_TYPE_CUSTOM);
    }
    TUI_CHECK_COND(mComposer != nullptr, TUI_ERR_OUT_OF_MEMORY);

    ret = mComposer->init(mComposer->ctx, &composerParams, mComposerCB);
    TUI_CHECK_COND(ret == TUI_COMPOSER_STATUS_OK, ret);

    dispBuf.config.width = mDispCfg.width;
    dispBuf.config.stride = mDispCfg.width;
    dispBuf.config.height = mDispCfg.height;
    dispBuf.config.format = TUI_COMPOSER_FORMAT_RGBA;
    dispBuf.handle.vAddr = buffer.buffHandle.vAddr;

    /* TUIStartSession is imperative to start a secure session in the VM. TUICore
     * Service will make sure to switch the display and touch in secure mode */
    bufferCfg->width = mDispCfg.width;
    bufferCfg->height = mDispCfg.height;
    bufferCfg->stride = mDispCfg.width;
    bufferCfg->colorFormat = 1;
    bufferCfg->offHeight = 0;
    bufferCfg->offWidth = 0;

    ret = _validateDisplayBuf(&dispBuf, &buffer, align);
    TUI_CHECK_COND(ret == 0, TUI_ERR_GENERIC_FAILURE);

    mInput.type = TUI_COMPOSER_INPUT_TOUCH;
    mInput.cmd = TUI_COMPOSER_INPUT_CMD_START;

    ret = mComposer->compose(mComposer->ctx, &mInput, &dispBuf);
    TUI_CHECK_COND(ret == TUI_COMPOSER_STATUS_OK, ret);

    TUI_CHECK_COND((res = TUIStartSession(mSessionId, &buffer)) == TUI_OK,
                   (int32_t)res);

    // Start async input handling during session
    mTouchThread = std::make_shared<std::thread>(
        [](CTrustedUIApp *app) { app->_touchThreadHandler(); }, this);

    if (mTouchThread == nullptr) {
        TUILOGE("Failed to start touch thread, abort");
        ret = TUI_ERR_GENERIC_FAILURE;
        goto end;
    }
    mState = HANDLING_INPUT;
    TUILOGE("%s::%d State changed to %s", __func__, __LINE__, ToString(mState));
end:
    if (ret) {
        TUILOGE("%s::%d Start session failed with ret - %d",
                    __func__, __LINE__, ret);
        _cleanUpComposer();
        retVal = _cleanUpSession();
        if (retVal) {
            TUILOGE("%s::%d Failed to clean up TUIApp session, ret - %d",
                    __func__, __LINE__, retVal);
        }
    }
    return ret;
}

void CTrustedUIApp::_stopTouchThreadIfRunning()
{
    if (mTouchThread != nullptr && mTouchThread->joinable()) {
        TUILOGV("%s: waiting for touch thread to join", __func__);
        mTouchThread->join();
        TUILOGV("%s: thread joined ", __func__);
        mTouchThread = nullptr;
        TUILOGV("%s: touch thread is stopped", __func__);
    }
}

int32_t CTrustedUIApp::stopSession()
{
    ENTER;
    int32_t ret = Object_OK;
    int32_t retVal = Object_OK;
    TUIResult res = TUI_OK;
    TUILOGE("%s++: Stop the session: Current State: %s ", __func__, ToString(mState));
    {
      std::lock_guard<std::mutex> l(mLock);
        TUI_CHECK_COND(mState == HANDLING_INPUT ||
                       mState == HANDLING_INPUT_DONE,
                       TUI_ERR_INVALID_STATE);

        if (mState == HANDLING_INPUT) {
            TUILOGE("%s: HLOS - Request sent to abort this session", __func__);
            mState = ABORTING;
        }
        _cleanUpComposer();
        if (mState != HANDLING_INPUT_DONE) {
            TUILOGD("%s++: cleanup session ", __func__);
            retVal = _cleanUpSession();
            if (retVal) {
                TUILOGE("%s::%d Failed to clean up TUIApp session, retVal - %d",
                    __func__, __LINE__, retVal);
            }
        }
    }
    // Notify TUI Framework to stop/abort this session
    // Release lock for touch thread handler to terminate & update state
    TUILOGD("%s: Wait for touch thread to terminate", __func__);
    _stopTouchThreadIfRunning();
    TUILOGD("%s::%d Touch thread terminated", __func__, __LINE__);
    {
      TUILOGD("%s::%d Session Stopped curr state:%s", __func__, __LINE__, ToString(mState));
      std::lock_guard<std::mutex> l(mLock);
      mState = INITIALIZED;
    }
    TUILOGE("%s::%d State changed to %s", __func__, __LINE__, ToString(mState));

end:
    TUILOGD("%s-- ", __func__);
    return ret;
}

int32_t CTrustedUIApp::deleteSession()
{
    int32_t ret = Object_OK;
    TUIResult res = TUI_OK;
    ENTER;

    std::lock_guard<std::mutex> l(mLock);
    TUI_CHECK_COND(mState == INITIALIZED, TUI_ERR_INVALID_STATE);

    TUILOGD("%s:++ ", __func__);
    // Notify TUI Framework to delete this session
    TUI_CHECK_COND((res = TUIDeleteSession(mSessionId)) == TUI_OK, -1);

    // App's clean-up routine
    if (mHLOSCBORef) {
        //We need to release the ref held by proxy object
        TUILOGE("delete my reference to listener ");
        delete mHLOSCBORef;
    }

    if (mWakeLock) {
        delete mWakeLock;
        mWakeLock = nullptr;
    }

    mState = UNINITIALIZED;
    TUILOGE("%s: State changed to %s", __func__, ToString(mState));

end:
    EXIT;
    return ret;
}

int32_t getKeyMasterInfo(uint8_t *tzHmac, char *mPromptText)
{
    int32_t ret = 0;
    Object keyMasterObj = Object_NULL;
    size_t keyLenout = 0;
    size_t hmacLenout = 0;
    uint8_t inputText[TUI_MAX_TOKEN_LENGTH];

    TUILOGD("%s::%d Getting the keymaster app object to get the tzHmac", __func__,
              __LINE__);

    if (Object_isNull(gTVMEnv)) {
        TUILOGE("%s::%d Error: Failed to get global opener object", __func__,
                 __LINE__);
        ret = Object_ERROR;
        goto end;
    }

    ret = IOpener_open(gTVMEnv, CConfirmationToken_UID, &keyMasterObj);
    if (Object_isERROR(ret) || Object_isNull(keyMasterObj)) {
        TUILOGE("%s::%d Failed to get keymaster app object, ret - %d", __func__,
                __LINE__, ret);
        ret = Object_ERROR;
        goto end;
    }

    // clear the values for tzHmac and inputText
    memset(tzHmac, 0, TUI_HMAC_DIGEST_SIZE_SHA256);
    memset(inputText, 0, TUI_MAX_TOKEN_LENGTH);
    snprintf((char *)inputText, TUI_MAX_TOKEN_LENGTH,"confirmation token %s", mPromptText);

    // Get Key and HMAC from authapp
    ret = IConfirmationToken_signConfirmationToken(keyMasterObj, (const void *)inputText,
                          sizeof(inputText), tzHmac, TUI_HMAC_DIGEST_SIZE_SHA256, &hmacLenout);
    TUI_CHECK_ERR(ret == 0, ret);

    TUILOGD("tzHmac = %u and inputText = %s", tzHmac, inputText);
    TUILOGD("sign confirmationToken returned successfully, ret = %d", ret);

end:
    Object_ASSIGN_NULL(keyMasterObj);

    return ret;
}

int32_t CTrustedUIApp::handleCmd(const void *cmd_ptr, size_t cmd_len,
                                 void *response_ptr, size_t response_len,
                                 size_t *response_lenout)
{
    int32_t ret = Object_OK;
    uint32_t cmdId = 0;
    char * cmd = nullptr;
    char * rsp = nullptr;
    char * msg = nullptr;
    int itr = 0;
    int nthCmd = 0;
    uint8_t tzHmac[TUI_HMAC_DIGEST_SIZE_SHA256];

    ENTER;
    std::lock_guard<std::mutex> l(mLock);

    TUI_CHECK_COND(mState != UNINITIALIZED, TUI_ERR_INVALID_STATE);
    TUI_CHECK_COND(cmd_ptr != nullptr &&
                   response_ptr != nullptr &&
                   response_lenout != nullptr &&
                   response_len > 0 &&
                   (cmd_len >= sizeof(uint32_t)), TUI_ERR_INVALID_INPUT);

    cmd = (char *)cmd_ptr;
    rsp = (char *)response_ptr;

    //extract command id(4 bytes) from byte buffer
    UNPACK_DATA(cmdId, sizeof(cmdId), cmd);
    cmd = cmd + sizeof(cmdId);
    TUILOGD("cmdId: %u", cmdId);

    if (cmd_len > sizeof(cmdId) || cmdId == TUI_CMD_SEND_PING) {
        msg = cmd;
    } else {
        rsp[0] = TUI_CMD_RECV_NO_MSG;
        *response_lenout = 1;
        goto end;
    }

    // App's custom cmd handling
    if (cmdId == TUI_CMD_SEND_PING) {
        TUILOGD("ping received, sending response");
        rsp[0] = TUI_CMD_RECV_PING;
        *response_lenout = 1;
    }
    else if (cmdId == TUI_CMD_SEND_MSG)
    {
        TUILOGD("message = %s", msg);
        for (itr = 4; itr < response_len; itr++) {
            rsp[itr] = itr % 256;
        }

        rsp[0] = TUI_CMD_RECV_PING;
        *response_lenout = response_len;
    }
    else if(cmdId == TUI_CMD_SET_PROMPT_TEXT)
    {
        TUILOGD("set prompt text for confirmation");
        TUI_CHECK_COND((cmd_len - nthCmd < 256), TUI_ERR_INVALID_INPUT);
        memset(mPromptText, 0, TUI_MAX_PROMPT_TEXT_LEN);
        // get prompt text
        strlcpy(mPromptText, &cmd[nthCmd], (cmd_len - nthCmd));
        TUILOGD("mPromptText = %s", mPromptText);
        // mark response as recieved
        rsp[0] = TUI_CMD_RECV_SET_PROMPT_TEXT;
        *response_lenout = 1;
    }
    else if (cmdId == TUI_CMD_GET_CONF_RESULT)
    {
        TUILOGD("get confirmation result");
        ret = getKeyMasterInfo(tzHmac, mPromptText);
        TUI_CHECK_ERR(ret == 0, ret);
        // we check to make sure we have enough space to write the HMAC and prompt text
        TUI_CHECK_COND(response_len > (TUI_HMAC_DIGEST_SIZE_SHA256 + TUI_MAX_PROMPT_TEXT_LEN), TUI_ERR_INVALID_INPUT);

        memset(rsp , 0, response_len);
        memscpy(rsp, response_len, (char *)tzHmac, TUI_HMAC_DIGEST_SIZE_SHA256);
        memscpy((rsp+TUI_HMAC_DIGEST_SIZE_SHA256), (response_len - TUI_HMAC_DIGEST_SIZE_SHA256), mPromptText, TUI_MAX_PROMPT_TEXT_LEN);
        *response_lenout = TUI_HMAC_DIGEST_SIZE_SHA256 + TUI_MAX_PROMPT_TEXT_LEN;  // response_lenout indicates how much response buffer we added
    }

end:
    TUILOGD("%s::%d ret = %d", __func__, __LINE__, ret);
    return ret;
}

int32_t CTrustedUIApp::handleCmdLargeData(const IMemObject &cmd_ref, void* response_ptr,
                                          size_t response_len, size_t* response_lenout)
{
    int32_t dataFd = -1, ret = Object_OK, itr = 0, cmdId = 0;
    void *data = nullptr;
    char *msg = nullptr;
    char *rsp = nullptr;
    uint32_t fileSize = 0;
    struct stat fileStats = {};

    ENTER;
    std::lock_guard<std::mutex> l(mLock);

    TUI_CHECK_COND(mState != UNINITIALIZED, TUI_ERR_INVALID_STATE);
    TUI_CHECK_COND(!Object_isNull(cmd_ref.get()) &&
                   response_ptr != nullptr &&
                   response_lenout != nullptr &&
                   response_len > 0, TUI_ERR_INVALID_INPUT);
    rsp = (char *)response_ptr;

    ret = Object_unwrapFd(cmd_ref.get(), &dataFd);
    TUI_CHECK_COND(ret == 0, TUI_ERR_GENERIC_FAILURE);

    ret = fstat(dataFd, &fileStats);
    fileSize = fileStats.st_size;
    TUI_CHECK_COND(fileSize > sizeof(cmdId), TUI_ERR_INVALID_INPUT);

    data = mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, dataFd, 0);
    TUI_CHECK_ERR((data != MAP_FAILED), TUI_ERR_GENERIC_FAILURE);
    msg = (char *)data;

    UNPACK_DATA(cmdId, sizeof(cmdId), msg);
    msg = msg + sizeof(cmdId);
    TUILOGE("cmdId: %u", cmdId);

    if (cmdId == TUI_CMD_SEND_PING) {
        TUILOGD("ping received, sending response");
        rsp[0] = TUI_CMD_RECV_PING;
        *response_lenout = 1;
    }
    else if (cmdId == TUI_CMD_SEND_MSG)
    {
        for (itr = 4; itr < response_len; itr++) {
            rsp[itr] = itr % 256;
        }

        rsp[0] = TUI_CMD_RECV_PING;
        *response_lenout = response_len;
    }

end:
    if (data != nullptr) {
        munmap((void *)data, fileSize);
    }

    if (dataFd > 0) {
        close(dataFd);
    }

    TUILOGD("%s::%d ret = %d", __func__, __LINE__, ret);
    return ret;
}

int32_t CTrustedUIApp_open(Object *objOut)
{
    TUILOGD("%s ++", __func__);
    CTrustedUIApp *me = new CTrustedUIApp();
    if (!me) {
        TUILOGE("Memory allocation for CTrustedUIApp failed!");
        return Object_ERROR_KMEM;
    }

    *objOut = (Object){ImplBase::invoke, me};
    TUILOGD("%s --", __func__);
    return Object_OK;
}

const char *CTrustedUIApp::ToString(State state)
{
    switch (state) {
        case UNINITIALIZED:
            return "UNINITIALIZED";
        case INITIALIZED:
            return "INITIALIZED";
        case HANDLING_INPUT:
            return "HANDLING_INPUT";
        case HANDLING_INPUT_DONE:
            return "HANDLING_INPUT_DONE";
        case ABORTING:
            return "ABORTING";
        default:
            return "INVALID";
    }
}
