/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "interfaces/rfrpe/RilRequestSetRfmScenarioMessage.h"
#include "interfaces/rfrpe/RilRequestGetRfmScenarioMessage.h"
#include "interfaces/rfrpe/RilRequestGetProvTableRevMessage.h"


//===========================================================================
// qcril_qmi_nas_get_rfm_scenario_req
//===========================================================================
void qcril_qmi_nas_get_rfm_scenario_req(std::shared_ptr<RilRequestGetRfmScenarioMessage> msg);

//===========================================================================
// qcril_qmi_nas_get_provisioned_table_revision_req
//===========================================================================
void qcril_qmi_nas_get_provisioned_table_revision_req(std::shared_ptr<RilRequestGetProvTableRevMessage> msg);

//===========================================================================
// qcril_qmi_nas_set_rfm_scenario_req
//===========================================================================
void qcril_qmi_nas_set_rfm_scenario_req(std::shared_ptr<RilRequestSetRfmScenarioMessage> msg);
