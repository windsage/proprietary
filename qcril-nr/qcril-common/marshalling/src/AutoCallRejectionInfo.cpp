/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/AutoCallRejectionInfo.h>
#include <marshal/CallComposerInfo.h>
#include <marshal/EcnamInfo.h>

template <>
Marshal::Result Marshal::write<RIL_AutoCallRejectionInfo>(const RIL_AutoCallRejectionInfo& arg) {
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.cause);
  WRITE_AND_CHECK(arg.sipErrorCode);
  WRITE_AND_CHECK(arg.number);
  WRITE_AND_CHECK(arg.verificationStatus);
  WRITE_AND_CHECK(arg.ccInfo, 1);
  WRITE_AND_CHECK(arg.ecnamInfo, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_AutoCallRejectionInfo>(RIL_AutoCallRejectionInfo& arg) const {
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.cause);
  READ_AND_CHECK(arg.sipErrorCode);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.verificationStatus);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.ccInfo, sz));
  if (sz != 1 && sz != 0) {
    return Result::FAILURE;
  }
  sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.ecnamInfo, sz));
  if (sz != 1 && sz != 0) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_AutoCallRejectionInfo>(RIL_AutoCallRejectionInfo& arg) {
  release(arg.callType);
  release(arg.cause);
  release(arg.sipErrorCode);
  release(arg.number);
  release(arg.verificationStatus);
  if (arg.ccInfo) {
    release(arg.ccInfo, 1);
  }
  if (arg.ecnamInfo) {
    release(arg.ecnamInfo, 1);
  }
  return Result::SUCCESS;
}
