/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CallModifyInfo.h>

template <>
Marshal::Result Marshal::write<RIL_CallModifyInfo>(const RIL_CallModifyInfo& arg) {
  WRITE_AND_CHECK(arg.callId);
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.callDomain);
  WRITE_AND_CHECK(arg.hasRttMode);
  WRITE_AND_CHECK(arg.rttMode);
  WRITE_AND_CHECK(arg.hasCallModifyFailCause);
  WRITE_AND_CHECK(arg.callModifyFailCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallModifyInfo>(RIL_CallModifyInfo& arg) const {
  READ_AND_CHECK(arg.callId);
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.callDomain);
  READ_AND_CHECK(arg.hasRttMode);
  READ_AND_CHECK(arg.rttMode);
  READ_AND_CHECK(arg.hasCallModifyFailCause);
  READ_AND_CHECK(arg.callModifyFailCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallModifyInfo>(RIL_CallModifyInfo& arg) {
  release(arg.callId);
  release(arg.callType);
  release(arg.callDomain);
  release(arg.hasRttMode);
  release(arg.rttMode);
  release(arg.hasCallModifyFailCause);
  release(arg.callModifyFailCause);
  return Result::SUCCESS;
}
