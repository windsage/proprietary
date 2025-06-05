/*========================================================================
  Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/

#include <semaphore.h>

#include "CTrustedUIApp.hpp"
#include "ITrustedUIApp.hpp"
#include "object.h"
#include "TUILog.h"

extern "C" {
#include "moduleAPI.h"
}

int32_t CTrustedUIApp_open(Object *objOut);

// ------------------------------------------------------------------------
// Global variable definitions
// ------------------------------------------------------------------------
static sem_t sWaitForever;
static Object tuiAppObject = Object_NULL;

int32_t main()
{
    TUILOGD("%s::%d - TUIApp process enter",  __func__, __LINE__);
    if (sem_init(&sWaitForever, 0, 0) != 0) {
        TUILOGE("Failed to initialize semaphore");
        return -1;
    }

    // Initialize structures or connections before service becomes available to
    // other processes.

    // Decrement (lock) the semaphore. Put to sleep indefinitely.
    if (sem_wait(&sWaitForever) != 0) {
        TUILOGE("Failed to wait on semaphore\n");
        return -1;
    }

    return 0;
}

/**
 * Description: Release any remaining objects before process is killed.
 *
 * In:          void
 * Out:         void
 * Return:      void
 */
void tProcessShutdown(void)
{
    TUILOGE("%s::%d - TUIApp process exit",  __func__, __LINE__);
    if (!Object_isNull(tuiAppObject)) {
        TrustedUIApp* app = new TrustedUIApp(tuiAppObject);
        if (app) {
            TUILOGE("%s:%u Abort current session", __func__, __LINE__);
            app->stopSession();
            app->deleteSession();
        }
    }
    Object_ASSIGN_NULL(tuiAppObject);

    // Increment (unlock) semaphore. Allow main thread to complete.
    sem_post(&sWaitForever);
    TUILOGE("Posted on semaphore. Beginning process exit.\n");
}

/**
 * Description: Open service by providing the services Unique ID as well as the
 *              ICredentials object of the caller, to uniquely identify it.
 *
 * In:          uid:    The unique ID of the requested service.
 *              cred:   The ICredentials object of the caller.
 * Out:         objOut: The service object.
 * Return:      Object_OK on success.
 */
int32_t tProcessOpen(uint32_t uid, Object cred, Object *objOut)
{
    int32_t ret = Object_OK;
    #pragma unused(cred)  //TODO
    switch (uid) {
        case CTrustedUIApp_UID:
            if (Object_isNull(tuiAppObject)) {
                TUILOGE("Create new TUIApp object");
                ret = CTrustedUIApp_open(&tuiAppObject);
                if (ret || Object_isNull(tuiAppObject)) {
                    TUILOGE("Failed to construct TUIApp object");
                    return Object_ERROR_BADOBJ;
                }
            }
            *objOut = tuiAppObject;
            Object_retain(*objOut);  //explicit retain
            TUILOGE("%s: Return TUIApp object", __func__);
            return ret;
        default:
            break;
    }

    TUILOGE("%s: No object found with UID: %d", __func__, uid);
    return Object_ERROR_BADOBJ;
}
