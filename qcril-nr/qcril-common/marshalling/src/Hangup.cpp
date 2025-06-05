/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/Hangup.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_Hangup>(const RIL_Hangup& arg)
{
  WRITE_AND_CHECK(arg.connIndex);
  WRITE_AND_CHECK(arg.connUri);
  WRITE_AND_CHECK(arg.hasRejectCause);
  WRITE_AND_CHECK(arg.rejectCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Hangup>(RIL_Hangup& arg) const
{
  READ_AND_CHECK(arg.connIndex);
  READ_AND_CHECK(arg.connUri);
  READ_AND_CHECK(arg.hasRejectCause);
  READ_AND_CHECK(arg.rejectCause);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Hangup>(RIL_Hangup& arg)
{
  release(arg.connIndex);
  release(arg.connUri);
  release(arg.hasRejectCause);
  release(arg.rejectCause);
  return Result::SUCCESS;
}

