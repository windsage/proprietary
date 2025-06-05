/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2021, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LOC_DIAG_IFACE_API_H
#define LOC_DIAG_IFACE_API_H

#include <stdint.h>
#include <loc_cfg.h>
#include <loc_misc_utils.h>

/**This is to indicate if the buffer is allocated from Diag or malloc.
* This is needed for cases where Diag service is not up
*/
typedef enum {
    BUFFER_INVALID = 0,
    BUFFER_FROM_MALLOC, /**The diag buffer is allocated from heap using malloc.
                        * Used for diag messages that are generated prio to DIag
                        * susbsystem being initialised*/
    BUFFER_FROM_DIAG    /**The diag buffer is allocated from diag heap using log_alloc.*/
} diagBuffSrc;

typedef enum {
    /** To log DIAG report Packets */
    DIAG_REPORT_SERVICE_TYPE,
    /** To log DIAG events */
    DIAG_EVENT_SERVICE_TYPE,
    /** To log Message strings (AKA F3's) to DIAG */
    DIAG_MSG_STR_SERVICE_TYPE
} DIAG_SERVICE_TYPE;

struct LocDiagInterface {
    size_t size;
    void* (*logAlloc)(uint32_t diagId, size_t size, diagBuffSrc *bufferSrc);
    void (*logCommit)(void *pData, diagBuffSrc bufferSrc, uint32_t diagId, size_t size);
    void (*eventReportPayload)(uint32_t eventId, uint8_t length, void *pPayload);
    bool (*getDiagInitStatus)(const DIAG_SERVICE_TYPE type);
    void (*logDiagMsgStr)(const uint32_t level, char *buf);
};

// Entry point to the library
typedef const LocDiagInterface* (getLocDiagIfaceFunc)();

static inline LocDiagInterface* loadLocDiagIfaceInterface() {
    static LocDiagInterface *diagIface = nullptr;

    if (nullptr == diagIface) {
        int loadDiagIfaceLib = 1;
        const loc_param_s_type gps_conf_params[] = {
            {"LOC_DIAGIFACE_ENABLED", &loadDiagIfaceLib, nullptr, 'n'}
        };
        UTIL_READ_CONF(LOC_PATH_GPS_CONF, gps_conf_params);
        if (0 != loadDiagIfaceLib) {
            void* libHandle = nullptr;
            getLocDiagIfaceFunc* getter = (getLocDiagIfaceFunc*)dlGetSymFromLib(
                    libHandle, "liblocdiagiface.so", "getLocDiagIface");
            if (nullptr != getter) {
                diagIface = (LocDiagInterface*)(*getter)();
            }
        }
    }
    return diagIface;
}

#endif /**LOC_DIAG_IFACE_API_H*/
