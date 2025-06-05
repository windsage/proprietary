/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/PreAlertingCallInfo.h>
#include <marshal/CallComposerInfo.h>
#include <marshal/EcnamInfo.h>

template <>
Marshal::Result Marshal::read<RIL_PreAlertingCallInfo>(RIL_PreAlertingCallInfo& arg) const
{
  READ_AND_CHECK(arg.callId);
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
Marshal::Result Marshal::write<RIL_PreAlertingCallInfo>(const RIL_PreAlertingCallInfo& arg)
{
  WRITE_AND_CHECK(arg.callId);
  WRITE_AND_CHECK(arg.ccInfo, 1);
  WRITE_AND_CHECK(arg.ecnamInfo, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_PreAlertingCallInfo>(RIL_PreAlertingCallInfo& arg)
{
  if (arg.ccInfo) {
    release(arg.ccInfo, 1);
  }
  if (arg.ecnamInfo) {
    release(arg.ecnamInfo, 1);
  }
  return Result::SUCCESS;
}
