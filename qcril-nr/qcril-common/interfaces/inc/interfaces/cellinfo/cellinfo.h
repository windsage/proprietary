/******************************************************************************
#  Copyright (c) 2020 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "interfaces/common.h"

#include <vector>

namespace qcril {
namespace interfaces {

struct RilGetCellInfoListResult_t : public qcril::interfaces::BasePayload {
  std::vector<RIL_CellInfo_v12> cellInfos;

  RilGetCellInfoListResult_t() {
  }
  RilGetCellInfoListResult_t(RIL_CellInfo_v12 *cellInfos, size_t size) :
    cellInfos(cellInfos, cellInfos + size) {
  }
};

}  // namespace interfaces
}  // namespace qcril
