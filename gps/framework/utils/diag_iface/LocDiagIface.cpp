/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018, 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <log_util.h>
#include "msg_q.h"
#include "LocDiagIface.h"
#include <loc_cfg.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LOC_DIAG_IFACE"

#define DIAG_MSG_BUFFER_WAIT_TIMEOUT (30*1000)

LocDiagIface *LocDiagIface::mInstance = nullptr;

LocDiagIface* LocDiagIface::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new LocDiagIface();
    }
    return mInstance;
}

LocDiagIface::LocDiagIface(): mQ((void*)msg_q_init2())
{
    mDiagBufferState = DIAG_BUFFERING_NOT_STARTED;
    mIsDiagBufferingEnabled = 0;
    mLastDiagInitFailedBoottime = 0;
    mLastDiagInitCount = 0;
    mIsDiagInitialized = Diag_LSM_Init(NULL);
    if (false == mIsDiagInitialized) {
        mLastDiagInitFailedBoottime = uptimeMillis();
        mLastDiagInitCount++;
        LOC_LOGE("%s] Failed to initialize DIAG!", __func__);
    }
    static loc_param_s_type buffer_conf_param_table[] =
    {
        {"BUFFER_DIAG_LOGGING", &mIsDiagBufferingEnabled, NULL, 'n'}
    };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, buffer_conf_param_table);
    LOC_LOGV("%s] BUFFER_DIAG_LOGGING: %d", __func__, mIsDiagBufferingEnabled);
}

void LocDiagIface::destroy()
{
    LOC_LOGD("%s]", __func__);
    if (true == mIsDiagInitialized) {
        Diag_LSM_DeInit();
    }


    mDiagBufferState = DIAG_BUFFERING_NOT_STARTED;
    msg_q_flush((void*)mQ);
    msg_q_destroy((void**)&mQ);
    if (nullptr != mInstance) {
        delete mInstance;
        mInstance = nullptr;
    }
}

LocDiagIface::~LocDiagIface()
{
    LOC_LOGD("%s]", __func__);
    destroy();
}

void LocDiagIface::flushBuffer()
{
    DiagCommitMsg *pCommitMsg = nullptr;
    void *msg = nullptr;
    msq_q_err_type result = eMSG_Q_SUCCESS;
    LOC_LOGV("%s --> \n", __func__);
    do
    {
        result = msg_q_rmv(mQ, (void **)&msg);
        if (eMSG_Q_SUCCESS != result) {
            LOC_LOGE("%s fail to receive msg! result: %d\n", __func__, result);
            break;
        }
        pCommitMsg = (DiagCommitMsg *)msg;
        if (nullptr != pCommitMsg) {
            void *pMsg;
            LOC_LOGV("%s Diag ID: 0x%X buffer source:%d \n",
                    __func__, pCommitMsg->mDiagId, pCommitMsg->mBufferSrc);
            if ((BUFFER_FROM_MALLOC == pCommitMsg->mBufferSrc) &&
                    (pCommitMsg->mSize > sizeof(log_hdr_type))) {
                /*Allocate diag buffer and copy payload from malloc */
                pMsg = log_alloc(pCommitMsg->mDiagId, pCommitMsg->mSize);
                if (pMsg != nullptr) {
                    /**log_alloc will populate the log header, Copy remaining payload */
                    memcpy((void *) ((char *)pMsg + sizeof(log_hdr_type)),
                           ((char *)pCommitMsg->mCommitPtr + sizeof(log_hdr_type)),
                           pCommitMsg->mSize - sizeof(log_hdr_type));
                    log_commit(pMsg);
                } else {
                    LOC_LOGw("log_alloc return nullptr for diag ID 0x%X ", pCommitMsg->mDiagId);
                }
            }
            delete pCommitMsg;
            pCommitMsg = nullptr;
        }
    } while(1);
}

/*!
 * @function: logCommit()
 * @brief Function for push diag data to queue and process queued diag messages
 * Here two types of Diag pointers are pushed to queue.
 * 1) malloc is done for initial messages when diag service is not up.
 * 2) Once Diag service is up log_alloc pointers are sent to the message queue
 */
void LocDiagIface::logCommit(void *pData, diagBuffSrc bufferSrc,
        uint32_t diagId, size_t size)
{
    LOC_LOGV("[%s] diagId: 0x%X bufferSrc: %d",
             __func__, diagId, bufferSrc);
    if (BUFFER_FROM_DIAG == bufferSrc) {
        if (DIAG_BUFFERING_STARTED == mDiagBufferState) {
            flushBuffer();
            mDiagBufferState = DIAG_BUFFERING_STOPPED;
        }
        log_commit(pData);
    } else if (BUFFER_FROM_MALLOC == bufferSrc) {
        /**Post the message to queue*/
        if (mQ) {
            msg_q_snd(mQ, new DiagCommitMsg(pData, diagId, bufferSrc, size), nullptr);
        } else {
            LOC_LOGE("[%s] Error msg thread is not created", __func__);
        }
    }
}

/*!
 * @function: LocGetDiagBuffer()
 * @brief  Allocate Buffer from Diag log_alloc or malloc based on Diag service availablity
 */
void* LocDiagIface::logAlloc(uint32_t diagId, size_t size, diagBuffSrc *bufferSrc)
{
    void *pData = nullptr;
    *bufferSrc = BUFFER_INVALID;

    if (checkInit()) {
        pData = log_alloc(diagId, size);
    }
    /**If pData is not nullptr then diag service is up/message mask is enabled
    * so stop msg buffering */
    if (pData != nullptr) {
        *bufferSrc = BUFFER_FROM_DIAG;
    } else if ((DIAG_BUFFERING_STOPPED != mDiagBufferState) && (1 == mIsDiagBufferingEnabled)) {
        if (uptimeMillis() < DIAG_MSG_BUFFER_WAIT_TIMEOUT) {
            if (pData == nullptr) {
                /** malloc is done only when buffering is active i.e. first 30 seconds
                 *  or Diag service is failing to alloc memory*/
                LOC_LOGw("Allocating memory using malloc diagID 0x%x uptime %" PRIi64 " TimeOut %u",
                        diagId, uptimeMillis(), DIAG_MSG_BUFFER_WAIT_TIMEOUT);
                pData =  malloc(size);
                *bufferSrc = BUFFER_FROM_MALLOC;
                mDiagBufferState = DIAG_BUFFERING_STARTED;
            }
        } else {
            LOC_LOGe("not allocating memory for diagID: 0x%x uptime > DIAG_MSG_BUFFER_WAIT_TIMEOUT",
                     diagId);
            /** After max timeout(upTime > mTimeOut), not allocating memory using malloc().
            * flush or free out buffered packets if diag allocation fails after 30 seconds */
            flushBuffer();
            mDiagBufferState = DIAG_BUFFERING_STOPPED;
        }
    }
    return(pData);
}

bool LocDiagIface::checkInit() {
    // we allow at least five retry and until the buffered time out
    // each retry will be at least 1000 milli-seconds apart
    if (!mIsDiagInitialized) {
        int64_t upTime = uptimeMillis();
        if ((upTime - mLastDiagInitFailedBoottime) > 1000 &&
            ((upTime < DIAG_MSG_BUFFER_WAIT_TIMEOUT) || (mLastDiagInitCount < 5))) {
            mIsDiagInitialized = Diag_LSM_Init(NULL);
            if (mIsDiagInitialized == false) {
                mLastDiagInitFailedBoottime = uptimeMillis();
                mLastDiagInitCount++;
            }
        }
    }

    return mIsDiagInitialized;
}

bool LocDiagIface::getDiagInitStatus(const DIAG_SERVICE_TYPE type) {
    bool retVal = false;
    switch (type) {
        case DIAG_MSG_STR_SERVICE_TYPE:
        case DIAG_REPORT_SERVICE_TYPE:
        case DIAG_EVENT_SERVICE_TYPE:
            retVal = mIsDiagInitialized;
            break;
        default:
            LOC_LOGd("Unknown type. Ignore");
            break;
    }
    return retVal;
}

void LocDiagIface::eventReportPayload(event_id_enum_type eventId, uint8 length, void *pPayload) {
    if (!mIsDiagInitialized) {
        LOC_LOGe("Diag event service not initialized");
        return;
    }
    event_report_payload(eventId, length, pPayload);
}

void LocDiagIface::logMsgStr(const uint32_t level, char *buf) {
    switch (level) {
    case MSG_QXDM_LOW:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_LOW, "%s", buf);
        break;
    case MSG_QXDM_MED:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_MED, "%s", buf);
        break;
    case MSG_QXDM_HIGH:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_HIGH, "%s", buf);
        break;
    case MSG_QXDM_ERROR:
        MSG_SPRINTF_1(MSG_SSID_GNSS_HLOS, MSG_LEGACY_ERROR, "%s", buf);
        break;
    default:
        break;
    }
}

static LocDiagIface* gLocDiagIfaceImpl = NULL;

static void* logAlloc(uint32_t diagId, size_t size, diagBuffSrc *bufferSrc);
static void logCommit(void *pData, diagBuffSrc bufferSrc, uint32_t diagId, size_t size);
static bool getDiagInitStatus(const DIAG_SERVICE_TYPE type);
static void eventReportPayload(uint32_t eventId, uint8 length, void *pPayload);
static void logDiagMsgStr(const uint32_t level, char *buf);

static const LocDiagInterface gLocDiagIface = {
    sizeof(LocDiagInterface),
    logAlloc,
    logCommit,
    eventReportPayload,
    getDiagInitStatus,
    logDiagMsgStr
};

#ifndef DEBUG_X86
extern "C" const LocDiagInterface* getLocDiagIface()
#else
const LocDiagInterface* getLocDiagIface()
#endif // DEBUG_X86
{
    if (nullptr == gLocDiagIfaceImpl) {
        LOC_LOGd("GetInstance of LocDiagIface");
        gLocDiagIfaceImpl = LocDiagIface::getInstance();
    }
    return &gLocDiagIface;
}

static void* logAlloc(uint32_t diagId, size_t size, diagBuffSrc *bufferSrc) {
    if (nullptr != gLocDiagIfaceImpl) {
        return gLocDiagIfaceImpl->logAlloc(diagId, size, bufferSrc);
    } else {
        LOC_LOGe("gLocDiagIfaceImpl is nullptr");
        return nullptr;
    }
}

static void logCommit(void *pData, diagBuffSrc bufferSrc, uint32_t diagId, size_t size) {
    if (nullptr != gLocDiagIfaceImpl) {
        gLocDiagIfaceImpl->logCommit(pData, bufferSrc, diagId, size);
    } else {
        LOC_LOGe("gLocDiagIfaceImpl is nullptr");
    }
}

static bool getDiagInitStatus(const DIAG_SERVICE_TYPE type) {
    if (nullptr != gLocDiagIfaceImpl) {
        return gLocDiagIfaceImpl->getDiagInitStatus(type);
    } else {
        LOC_LOGe("gLocDiagIfaceImpl is nullptr");
        return false;
    }
}

static void eventReportPayload(uint32_t eventId, uint8 length, void *pPayload) {
    if (nullptr != gLocDiagIfaceImpl) {
        return gLocDiagIfaceImpl->eventReportPayload(
            static_cast<event_id_enum_type>(eventId), length, pPayload);
    } else {
        LOC_LOGe("gLocDiagIfaceImpl is nullptr");
    }
}

static void logDiagMsgStr(const uint32_t level, char *buf) {
    if (nullptr != gLocDiagIfaceImpl) {
        return gLocDiagIfaceImpl->logMsgStr(level, buf);
    } else {
        LOC_LOGe("gLocDiagIfaceImpl is nullptr");
    }
}
