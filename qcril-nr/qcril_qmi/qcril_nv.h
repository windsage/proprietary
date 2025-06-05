/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifndef __QCRIL_NV_H_
#define __QCRIL_NV_H_

#include "interfaces/nv/RilRequestNvReadMessage.h"
#include "interfaces/nv/RilRequestNvWriteMessage.h"

/*=========================================================================

  FUNCTION:  qcril_nv_request_oem_hook_nv_read

===========================================================================*/
/*!
    @brief
    Reads the request NAM parameter from NV.

    @return
    If NV read is success then the corresponding NV item value is returned
    void
*/
/*=========================================================================*/
void qcril_nv_request_oem_hook_nv_read(std::shared_ptr<RilRequestNvReadMessage> msg);

/*=========================================================================

  FUNCTION:  qcril_nv_request_oem_hook_nv_write

===========================================================================*/
/*!
    @brief
    Writes the requested NAM parameter to NV item after validating the data.

    @return
    Void
    NV Write status is returned back in nv_write_status parameter.
*/
/*=========================================================================*/
void qcril_nv_request_oem_hook_nv_write(std::shared_ptr<RilRequestNvWriteMessage> msg);

#endif /* __QCRIL_NV_H_ */
