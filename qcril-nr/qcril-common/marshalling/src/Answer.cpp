/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/Answer.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_Answer>(const RIL_Answer& arg)
{
  WRITE_AND_CHECK(arg.isValid);
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.presentation);
  WRITE_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Answer>(RIL_Answer& arg) const
{
  READ_AND_CHECK(arg.isValid);
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.presentation);
  READ_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Answer>(RIL_Answer& arg)
{
  release(arg.isValid);
  release(arg.callType);
  release(arg.presentation);
  release(arg.rttMode);
  return Result::SUCCESS;
}
