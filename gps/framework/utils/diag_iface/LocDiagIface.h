/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018, 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __LOC_DIAG_IFACE__
#define __LOC_DIAG_IFACE__

#include <stdlib.h>
#include <LocDiagIfaceApi.h>
#include <loc_pla.h>
#include <log.h>
#include <log_codes.h>
#include <msgcfg.h>
#include <msg.h>
#include "event.h"
#include <diag_lsm.h>
#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"
#include "diag.h"

using namespace std;

#ifdef MSG_BUILD_MASK_MSG_SSID_GNSS_HLOS
#undef MSG_BUILD_MASK_MSG_SSID_GNSS_HLOS
#endif

#define MSG_BUILD_MASK_MSG_SSID_GNSS_HLOS \
        (MSG_LVL_LOW | MSG_LVL_MED | MSG_LVL_HIGH | MSG_LVL_ERROR | MSG_LVL_FATAL)

struct DiagCommitMsg {
    void  *mCommitPtr;
    unsigned int mDiagId;
    diagBuffSrc mBufferSrc;
    size_t mSize;
    inline DiagCommitMsg(void *pLogCommitPtr, unsigned int msgId,
            diagBuffSrc src, size_t datasize)
    {
        mCommitPtr = pLogCommitPtr;
        mBufferSrc = src;
        mDiagId = msgId;
        mSize = datasize;
    }
    /**Free up malloc pointers in destructor log_alloc pointers are cleared in log_commit*/
    inline ~DiagCommitMsg() {
        if (mBufferSrc == BUFFER_FROM_MALLOC) {
            if (mCommitPtr != nullptr) {
                free(mCommitPtr);
            }
        }
    }
};

typedef enum {
    DIAG_BUFFERING_NOT_STARTED,
    DIAG_BUFFERING_STARTED,
    DIAG_BUFFERING_STOPPED
} DiagBufferState;

class LocDiagIface {
public:
    // singleton instance
    static LocDiagIface* getInstance();
    void destroy();
    void* logAlloc(uint32_t diagId, size_t size, diagBuffSrc *bufferSrc);
    void logCommit(void *pData, diagBuffSrc bufferSrc, uint32_t diagId, size_t size);
    inline bool logStatus(uint32_t diagId) {
        return checkInit() && log_status(diagId);
    }
    bool getDiagInitStatus(const DIAG_SERVICE_TYPE type);
    void eventReportPayload(event_id_enum_type eventId, uint8 length, void *pPayload);
    void logMsgStr(const uint32_t level, char *buf);
private:
    static LocDiagIface *mInstance;
    DiagBufferState     mDiagBufferState;
    bool                mIsDiagInitialized;
    void*               mQ;
    int64_t             mLastDiagInitFailedBoottime;
    int                 mLastDiagInitCount;
    uint32_t            mIsDiagBufferingEnabled;

    bool checkInit();
    void flushBuffer();
    LocDiagIface();
    ~LocDiagIface();
};
#endif /**__LOC_DIAG_IFACE__*/
