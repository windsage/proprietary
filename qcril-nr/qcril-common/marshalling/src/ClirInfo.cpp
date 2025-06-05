/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ClirInfo.h>

template <>
Marshal::Result Marshal::write<RIL_ClirInfo>(const RIL_ClirInfo& arg)
{
  WRITE_AND_CHECK(arg.action);
  WRITE_AND_CHECK(arg.presentation);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_ClirInfo>(RIL_ClirInfo& arg) const
{
  READ_AND_CHECK(arg.action);
  READ_AND_CHECK(arg.presentation);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_ClirInfo>(RIL_ClirInfo& arg)
{
  release(arg.action);
  release(arg.presentation);
  return Marshal::Result::SUCCESS;
}
