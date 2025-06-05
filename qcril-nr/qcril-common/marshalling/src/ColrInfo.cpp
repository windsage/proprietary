/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ColrInfo.h>

template <>
Marshal::Result Marshal::write<RIL_ColrInfo>(const RIL_ColrInfo& arg) {
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.provisionStatus);
  WRITE_AND_CHECK(arg.presentation);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_ColrInfo>(RIL_ColrInfo& arg) const {
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.provisionStatus);
  READ_AND_CHECK(arg.presentation);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) return Result::FAILURE;
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_ColrInfo>(RIL_ColrInfo& arg) {
  release(arg.status);
  release(arg.provisionStatus);
  release(arg.presentation);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}

