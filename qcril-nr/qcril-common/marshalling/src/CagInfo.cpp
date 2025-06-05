/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/CagInfo.h>

template <>
Marshal::Result Marshal::write<RIL_CagInfo>(const RIL_CagInfo& arg)
{
  WRITE_AND_CHECK(arg.cagName);
  WRITE_AND_CHECK(arg.cagId);
  WRITE_AND_CHECK(arg.cagOnlyAccess);
  WRITE_AND_CHECK(arg.cagInAllowedList);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CagInfo>(RIL_CagInfo& arg) const
{
  READ_AND_CHECK(arg.cagName);
  READ_AND_CHECK(arg.cagId);
  READ_AND_CHECK(arg.cagOnlyAccess);
  READ_AND_CHECK(arg.cagInAllowedList);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CagInfo>(RIL_CagInfo& arg)
{
  release(arg.cagName);
  release(arg.cagId);
  release(arg.cagOnlyAccess);
  release(arg.cagInAllowedList);
  return Result::SUCCESS;
}
