/*===================================================================================
  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <semaphore.h>

#include "CTouchInput.hpp"
#include "TUILog.h"
#include "TUIUtils.h"
#include "TouchInput.h"
extern "C" {
#include "moduleAPI.h"
}
using namespace std;

/******************************************************************************
 *       Constant Definitions And Local Variables
*****************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TUIVMInput::TrustedInputMain"

static sem_t sigTerm;

/* Description : This API creates a CTouchInput class object pointer
* as the object context and TouchInput mink interface as the invoke.
*
* Out : objOut : Returns the object with invoke and context.
*
* Return : Object_ERROR_KMEM,
*          ITrustedInput::SUCCESS
*/
int32_t CTouchInput_open(Object *objOut)
{
    int32_t ret = ITrustedInput::SUCCESS;
    CTouchInput *me = new CTouchInput();
    if (me == nullptr) {
        TUILOGE("%s::%d - Error: Creating TouchInput pointer object Failed!", __func__, __LINE__);
        ret = Object_ERROR_KMEM;
        goto errorExit;
    }
    *objOut = (Object){ImplBase::invoke, me};
errorExit:
    return ret;
}

int32_t tProcessOpen(uint32_t uid, Object cred, Object *objOut) {
    TUILOGD("%s: Open touch input object: %d", __func__, uid);
    int32_t ret = ITrustedInput::SUCCESS;

    if (uid != CTouchInput_UID) {
        TUILOGE("%s: No object found with UID: %d", __func__, uid);
        ret = Object_ERROR_BADOBJ;
        goto errorExit;
    }

    TUILOGE("%s: Openining new touch input object: %d", __func__, uid);
    ret = CTouchInput_open(objOut);
    if ((ret != 0) || Object_isNull(*objOut)) {
        TUILOGE("Failed to create touch input object");
        Object_ASSIGN_NULL(*objOut);
        ret = Object_ERROR_BADOBJ;
        goto errorExit;
    }

errorExit:
    return ret;
}

void tProcessShutdown(void) {
    TUILOGE("%s::%d - TrustedInputService process died", __func__, __LINE__);
    sem_post(&sigTerm);
}
/******************************************************************************
 *       Main Entry to TouchInput
*****************************************************************************/

int main()
{
    int32_t ret = ITrustedInput::SUCCESS;

    sem_init(&sigTerm, 0, 0);
    sem_wait(&sigTerm);

    return ret;
}
