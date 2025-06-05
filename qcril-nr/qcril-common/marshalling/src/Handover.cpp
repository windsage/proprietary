/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/Handover.h>

template <>
Marshal::Result Marshal::write<RIL_HoExtra>(const RIL_HoExtra& arg)
{
  WRITE_AND_CHECK(arg.type);
  WRITE_AND_CHECK(arg.extraInfo);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_HoExtra>(RIL_HoExtra& arg) const
{
  READ_AND_CHECK(arg.type);
  READ_AND_CHECK(arg.extraInfo);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_HoExtra>(RIL_HoExtra& arg)
{
  release(arg.type);
  release(arg.extraInfo);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_HandoverInfo>(const RIL_HandoverInfo& arg)
{
  WRITE_AND_CHECK(arg.type);
  WRITE_AND_CHECK(arg.srcTech);
  WRITE_AND_CHECK(arg.targetTech);
  WRITE_AND_CHECK(arg.hoExtra, 1);
  WRITE_AND_CHECK(arg.errorCode);
  WRITE_AND_CHECK(arg.errorMessage);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_HandoverInfo>(RIL_HandoverInfo& arg) const
{
  READ_AND_CHECK(arg.type);
  READ_AND_CHECK(arg.srcTech);
  READ_AND_CHECK(arg.targetTech);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.hoExtra, sz));
  if (sz != 1 && sz != 0) {
    return Result::FAILURE;
  }
  READ_AND_CHECK(arg.errorCode);
  READ_AND_CHECK(arg.errorMessage);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_HandoverInfo>(RIL_HandoverInfo& arg)
{
  release(arg.type);
  release(arg.srcTech);
  release(arg.targetTech);
  release(arg.hoExtra, 1);
  release(arg.errorCode);
  release(arg.errorMessage);
  return Result::SUCCESS;
}
