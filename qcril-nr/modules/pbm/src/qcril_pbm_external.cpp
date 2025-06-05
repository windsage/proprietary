/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qcril_pbm_external.h"
#include "interfaces/nas/QcRilNasGetRilEccMapSyncMessage.h"

void qcril_qmi_pbm_external_get_ril_ecc_map(RIL_EccMapType& map) {
  auto msg = std::make_shared<QcRilNasGetRilEccMapSyncMessage>();
  assert(msg != nullptr);
  RIL_EccMapType eccMap;

  std::shared_ptr<RIL_EccMapType> shared_map;
  auto ret = msg->dispatchSync(shared_map);
  if (ret == Message::Callback::Status::SUCCESS && shared_map != nullptr) {
    eccMap = *shared_map;
  }

  map = std::move(eccMap);
}
