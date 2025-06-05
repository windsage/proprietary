/******************************************************************************
#  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qmi_ril_platform_dep.c
  @brief   Provides interface to functions where conditional platform
           dependency is there.

******************************************************************************/
#include <sys/ioctl.h>

#define TAG "RIL-PlatformDep"

#include "framework/Log.h"
#include "qcrili.h"
#include "qcril_qmi_client.h"
#include "qmi_ril_platform_dep.h"
#include "qcril_memory_management.h"
#ifdef FEATURE_QCRIL_SHM
#ifndef QMI_RIL_UTF
#include "sys_health_mon.h"
#endif
#endif
#include "qcril_legacy_apis.h"

#define NAS_MDM_SHUTDOWN_IOCTL_CMD   (0x4004cc0a)

static const char *modem_node_name[QCRIL_MAX_INSTANCE_ID] = { "/dev/mdm",
                                                              "/dev/mdm1"};

/*===========================================================================

  FUNCTION  qcril_process_mdm_shutdown

===========================================================================*/
/*!
    @brief
    Process modem shutdown request

    @return
    0 on success
*/
/*=========================================================================*/
RIL_Errno qcril_process_mdm_shutdown
(
    void
)
{
    int                                   ioctl_ret_code;
    int                                   ioctl_err_code;
    const char                           *modem_dev_node_name = modem_node_name[QCRIL_DEFAULT_INSTANCE_ID];
    qcril_instance_id_e_type              ril_instance_id;
    RIL_Errno                             ril_req_res = RIL_E_GENERIC_FAILURE;

    ioctl_ret_code = E_SUCCESS;
    ioctl_err_code = E_SUCCESS;

    ril_instance_id = qmi_ril_get_process_instance_id();
    if ((ril_instance_id == QCRIL_SECOND_INSTANCE_ID) &&
         qmi_ril_is_feature_supported( QMI_RIL_FEATURE_DSDA2 ))
    {
        modem_dev_node_name = modem_node_name[ril_instance_id];
    }

#ifdef FEATURE_QCRIL_8064

    fd = open(modem_dev_node_name, O_RDONLY | O_NONBLOCK);
    if ( fd >= RIL_VALID_FILE_HANDLE)
    {
        ioctl_ret_code = ioctl(fd, NAS_MDM_SHUTDOWN_IOCTL_CMD, &ioctl_err_code);
        if ( ioctl_ret_code < NAS_NIL || ioctl_err_code != E_SUCCESS )
        {
            QCRIL_LOG_ERROR("error while issuing ioctl SHUTDOWN_CHARM, ret %d, err %d", ioctl_ret_code, ioctl_err_code);
        }
        else
        {
            ril_req_res = RIL_E_SUCCESS;
        }

        close(fd);
    }
    else
    {
        QCRIL_LOG_ERROR("Could not open device %s, fd %d", modem_dev_node_name, fd);
    }

#else

    ril_req_res = RIL_E_SUCCESS;

#endif

    return ril_req_res;
}

