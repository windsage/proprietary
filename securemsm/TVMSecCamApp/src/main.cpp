/*========================================================================
  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  =========================================================================*/
/******************************************************************************
 *                   Header Inclusions
 *****************************************************************************/
#include <semaphore.h>
#include "CSecureCamera2App.hpp"

extern "C" {
#include "seccam.h"
}

int32_t CSecureCamera2App_open(Object *obj);

// ------------------------------------------------------------------------
// Global variable definitions
// ------------------------------------------------------------------------
static sem_t gWaitForever;
static Object gSecCamAppObj = Object_NULL;

int32_t main()
{
    if (sem_init(&gWaitForever, 0, 0) != 0) {
        LOG_MSG("Failed to initialize semaphore");
        return -1;
    }

    // Initialize structures or connections before service becomes available to
    // other processes.

    // Decrement (lock) the semaphore. Put to sleep indefinitely.
    if (sem_wait(&gWaitForever) != 0) {
        LOG_MSG("Failed to wait on semaphore");
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
    // Increment (unlock) semaphore. Allow main thread to complete.
    sem_post(&gWaitForever);
    LOG_MSG("Posted on semaphore. Beginning process exit");
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

    (void)cred;
    switch (uid) {
        case CSecureCamera2App_UID:
            if (!Object_isNull(gSecCamAppObj)) {
                LOG_MSG("SecCam App object has been created");
                *objOut = gSecCamAppObj;
                return ret;
            }

            LOG_MSG("Create new SecCam app object");
            ret = CSecureCamera2App_open(&gSecCamAppObj);
            if (ret || Object_isNull(gSecCamAppObj)) {
                LOG_MSG("Failed to construct SecCam App object");
                return Object_ERROR;
            }

            *objOut = gSecCamAppObj;
            return ret;
        default:
            break;
    }

    LOG_MSG("No object found with UID: %d", uid);

    return Object_ERROR;
}
