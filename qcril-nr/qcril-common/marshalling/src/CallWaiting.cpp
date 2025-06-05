/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CallWaiting.h>

template <>
Marshal::Result Marshal::write<RIL_CallWaitingSettings>(const RIL_CallWaitingSettings& arg)
{
  WRITE_AND_CHECK(arg.enabled);
  WRITE_AND_CHECK(arg.serviceClass);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallWaitingSettings>(RIL_CallWaitingSettings& arg) const
{
  READ_AND_CHECK(arg.enabled);
  READ_AND_CHECK(arg.serviceClass);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallWaitingSettings>(RIL_CallWaitingSettings& arg)
{
  release(arg.enabled);
  release(arg.serviceClass);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_QueryCallWaitingResponse>(const RIL_QueryCallWaitingResponse& arg)
{
  WRITE_AND_CHECK(arg.callWaitingSettings);
  WRITE_AND_CHECK(arg.sipError);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_QueryCallWaitingResponse>(RIL_QueryCallWaitingResponse& arg) const
{
  READ_AND_CHECK(arg.callWaitingSettings);
  READ_AND_CHECK(arg.sipError);
  return Marshal::Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_QueryCallWaitingResponse>(RIL_QueryCallWaitingResponse& arg)
{
  release(arg.callWaitingSettings);
  release(arg.sipError);
  return Marshal::Result::SUCCESS;
}
