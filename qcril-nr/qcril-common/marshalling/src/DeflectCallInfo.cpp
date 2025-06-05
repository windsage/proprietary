/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/DeflectCallInfo.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_DeflectCallInfo>(const RIL_DeflectCallInfo& arg)
{
  WRITE_AND_CHECK(arg.connIndex);
  WRITE_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_DeflectCallInfo>(RIL_DeflectCallInfo& arg) const
{
  READ_AND_CHECK(arg.connIndex);
  READ_AND_CHECK(arg.number);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_DeflectCallInfo>(RIL_DeflectCallInfo& arg)
{
  release(arg.connIndex);
  release(arg.number);
  return Result::SUCCESS;
}
