/******************************************************************************
#  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qmi_ril_platform_dep.h
  @brief   Provides interface to functions where conditional platform
           dependency is there.

******************************************************************************/

#ifndef QMI_RIL_PLATFORM_DEP_H
#define QMI_RIL_PLATFORM_DEP_H

#include "qcril_qmi_client.h"

#ifdef __cplusplus
extern "C" {
#endif

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
);

#ifdef __cplusplus
}
#endif
#endif /* QCRIL_DB_H */
