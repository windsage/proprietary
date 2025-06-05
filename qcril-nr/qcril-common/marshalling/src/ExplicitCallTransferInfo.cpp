/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/ExplicitCallTransferInfo.h>

template <>
Marshal::Result Marshal::write<RIL_ExplicitCallTransfer>(const RIL_ExplicitCallTransfer& arg)
{
  WRITE_AND_CHECK(arg.isValid);
  WRITE_AND_CHECK(arg.callId);
  WRITE_AND_CHECK(arg.ectType);
  WRITE_AND_CHECK(arg.targetAddress);
  WRITE_AND_CHECK(arg.targetCallId);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_ExplicitCallTransfer>(RIL_ExplicitCallTransfer& arg) const
{
  READ_AND_CHECK(arg.isValid);
  READ_AND_CHECK(arg.callId);
  READ_AND_CHECK(arg.ectType);
  READ_AND_CHECK(arg.targetAddress);
  READ_AND_CHECK(arg.targetCallId);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_ExplicitCallTransfer>(RIL_ExplicitCallTransfer& arg)
{
  release(arg.isValid);
  release(arg.callId);
  release(arg.ectType);
  release(arg.targetAddress);
  release(arg.targetCallId);
  return Result::SUCCESS;
}
