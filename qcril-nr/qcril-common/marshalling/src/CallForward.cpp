/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CallForward.h>

template <>
Marshal::Result Marshal::write<RIL_CallFwdTimerInfo>(const RIL_CallFwdTimerInfo& arg)
{
  WRITE_AND_CHECK(arg.year);
  WRITE_AND_CHECK(arg.month);
  WRITE_AND_CHECK(arg.day);
  WRITE_AND_CHECK(arg.hour);
  WRITE_AND_CHECK(arg.minute);
  WRITE_AND_CHECK(arg.second);
  WRITE_AND_CHECK(arg.timezone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallFwdTimerInfo>(RIL_CallFwdTimerInfo& arg) const
{
  READ_AND_CHECK(arg.year);
  READ_AND_CHECK(arg.month);
  READ_AND_CHECK(arg.day);
  READ_AND_CHECK(arg.hour);
  READ_AND_CHECK(arg.minute);
  READ_AND_CHECK(arg.second);
  READ_AND_CHECK(arg.timezone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallFwdTimerInfo>(RIL_CallFwdTimerInfo& arg)
{
  release(arg.year);
  release(arg.month);
  release(arg.day);
  release(arg.hour);
  release(arg.minute);
  release(arg.second);
  release(arg.timezone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CallForwardParams>(const RIL_CallForwardParams& arg)
{
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.reason);
  WRITE_AND_CHECK(arg.serviceClass);
  WRITE_AND_CHECK(arg.toa);
  WRITE_AND_CHECK(arg.number);
  WRITE_AND_CHECK(arg.timeSeconds);
  WRITE_AND_CHECK(arg.callFwdTimerStart, 1);
  WRITE_AND_CHECK(arg.callFwdTimerEnd, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallForwardParams>(RIL_CallForwardParams& arg) const
{
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.reason);
  READ_AND_CHECK(arg.serviceClass);
  READ_AND_CHECK(arg.toa);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.timeSeconds);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.callFwdTimerStart, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  RUN_AND_CHECK(readAndAlloc(arg.callFwdTimerEnd, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallForwardParams>(RIL_CallForwardParams& arg)
{
  release(arg.status);
  release(arg.reason);
  release(arg.serviceClass);
  release(arg.toa);
  release(arg.number);
  release(arg.timeSeconds);
  release(arg.callFwdTimerStart, 1);
  release(arg.callFwdTimerEnd, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_QueryCallForwardStatusInfo>(
    const RIL_QueryCallForwardStatusInfo& arg)
{
  WRITE_AND_CHECK(arg.callForwardInfo, arg.callForwardInfoLen);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_QueryCallForwardStatusInfo>(RIL_QueryCallForwardStatusInfo& arg) const
{
  RUN_AND_CHECK(readAndAlloc(arg.callForwardInfo, arg.callForwardInfoLen));
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_QueryCallForwardStatusInfo>(RIL_QueryCallForwardStatusInfo& arg)
{
  release(arg.callForwardInfo, arg.callForwardInfoLen);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SetCallForwardStatus>(const RIL_SetCallForwardStatus& arg)
{
  WRITE_AND_CHECK(arg.reason);
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SetCallForwardStatus>(RIL_SetCallForwardStatus& arg) const
{
  READ_AND_CHECK(arg.reason);
  READ_AND_CHECK(arg.status);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SetCallForwardStatus>(RIL_SetCallForwardStatus& arg)
{
  release(arg.reason);
  release(arg.status);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SetCallForwardStatusInfo>(const RIL_SetCallForwardStatusInfo& arg)
{
  WRITE_AND_CHECK(arg.setCallForwardStatus, arg.setCallForwardStatusLen);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SetCallForwardStatusInfo>(RIL_SetCallForwardStatusInfo& arg) const
{
  RUN_AND_CHECK(readAndAlloc(arg.setCallForwardStatus, arg.setCallForwardStatusLen));
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SetCallForwardStatusInfo>(RIL_SetCallForwardStatusInfo& arg)
{
  release(arg.setCallForwardStatus, arg.setCallForwardStatusLen);
  release(arg.errorDetails, 1);
  return Result::SUCCESS;
}
