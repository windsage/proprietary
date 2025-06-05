/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef QCRIL_QMI_NAS_CSG_H
#define QCRIL_QMI_NAS_CSG_H

#include "network_access_service_v01.h"
#include "telephony/ril.h"
#include "interfaces/nas/RilRequestCsgPerformNetworkScanMessage.h"
#include "interfaces/nas/RilRequestCsgSetSysSelPrefMessage.h"

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_get_csg_id
======================================================================================*/
RIL_Errno qcril_qmi_nas_get_csg_id(uint32_t& csg_id);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_csg_perform_network_scan
======================================================================================*/
void qcril_qmi_nas_csg_perform_network_scan(
    std::shared_ptr<RilRequestCsgPerformNetworkScanMessage> msg);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_csg_set_system_selection_preference
======================================================================================*/
void qcril_qmi_nas_csg_set_system_selection_preference(
    std::shared_ptr<RilRequestCsgSetSysSelPrefMessage> msg);

#endif /* QCRIL_QMI_NAS_CSG_H */
