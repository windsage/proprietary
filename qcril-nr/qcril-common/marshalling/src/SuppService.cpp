/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SuppService.h>

template <>
Marshal::Result Marshal::write<RIL_CallBarringNumbersInfo>(const RIL_CallBarringNumbersInfo& arg)
{
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallBarringNumbersInfo>(RIL_CallBarringNumbersInfo& arg) const
{
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallBarringNumbersInfo>(RIL_CallBarringNumbersInfo& arg)
{
  release(arg.status);
  release(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CallBarringNumbersListInfo>(
    const RIL_CallBarringNumbersListInfo& arg)
{
  WRITE_AND_CHECK(arg.serviceClass);
  WRITE_AND_CHECK(arg.callBarringNumbersInfo, arg.callBarringNumbersInfoLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallBarringNumbersListInfo>(RIL_CallBarringNumbersListInfo& arg) const
{
  READ_AND_CHECK(arg.serviceClass);
  RUN_AND_CHECK(readAndAlloc(arg.callBarringNumbersInfo, arg.callBarringNumbersInfoLen));
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallBarringNumbersListInfo>(RIL_CallBarringNumbersListInfo& arg)
{
  release(arg.serviceClass);
  release(arg.callBarringNumbersInfo, arg.callBarringNumbersInfoLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SuppSvcRequest>(const RIL_SuppSvcRequest& arg)
{
  WRITE_AND_CHECK(arg.operationType);
  WRITE_AND_CHECK(arg.facilityType);
  WRITE_AND_CHECK(arg.callBarringNumbersListInfo, 1);
  WRITE_AND_CHECK(arg.password);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SuppSvcRequest>(RIL_SuppSvcRequest& arg) const
{
  READ_AND_CHECK(arg.operationType);
  READ_AND_CHECK(arg.facilityType);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.callBarringNumbersListInfo, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  READ_AND_CHECK(arg.password);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SuppSvcRequest>(RIL_SuppSvcRequest& arg)
{
  release(arg.operationType);
  release(arg.facilityType);
  release(arg.callBarringNumbersListInfo, 1);
  release(arg.password);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_SuppSvcResponse>(const RIL_SuppSvcResponse& arg)
{
  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.provisionStatus);
  WRITE_AND_CHECK(arg.facilityType);
  WRITE_AND_CHECK(arg.callBarringNumbersListInfo, arg.callBarringNumbersListInfoLen);
  WRITE_AND_CHECK(arg.errorDetails, 1);
  WRITE_AND_CHECK(arg.isPasswordRequired);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SuppSvcResponse>(RIL_SuppSvcResponse& arg) const
{
  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.provisionStatus);
  READ_AND_CHECK(arg.facilityType);
  RUN_AND_CHECK(readAndAlloc(arg.callBarringNumbersListInfo, arg.callBarringNumbersListInfoLen));
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
  if (sz != 0 && sz != 1) {
    return Result::FAILURE;
  }
  READ_AND_CHECK(arg.isPasswordRequired);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SuppSvcResponse>(RIL_SuppSvcResponse& arg)
{
  release(arg.status);
  release(arg.provisionStatus);
  release(arg.facilityType);
  release(arg.callBarringNumbersListInfo, arg.callBarringNumbersListInfoLen);
  release(arg.errorDetails, 1);
  release(arg.isPasswordRequired);
  return Result::SUCCESS;
}
