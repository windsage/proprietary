/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SwitchWaitingOrHoldingAndActive.h>

template <>
Marshal::Result Marshal::write<RIL_SwitchWaitingOrHoldingAndActive>(
    const RIL_SwitchWaitingOrHoldingAndActive& arg)
{
  WRITE_AND_CHECK(arg.hasCallType);
  WRITE_AND_CHECK(arg.callType);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SwitchWaitingOrHoldingAndActive>(
    RIL_SwitchWaitingOrHoldingAndActive& arg) const
{
  READ_AND_CHECK(arg.hasCallType);
  READ_AND_CHECK(arg.callType);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SwitchWaitingOrHoldingAndActive>(
    RIL_SwitchWaitingOrHoldingAndActive& arg)
{
  release(arg.hasCallType);
  release(arg.callType);
  return Result::SUCCESS;
}
