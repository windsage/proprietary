/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SuppServiceNotification.h>

template <>
Marshal::Result Marshal::write<RIL_SuppSvcNotificationInfo>(const RIL_SuppSvcNotificationInfo& arg)
{
  WRITE_AND_CHECK(arg.notificationType);
  WRITE_AND_CHECK(arg.code);
  WRITE_AND_CHECK(arg.index);
  WRITE_AND_CHECK(arg.type);
  WRITE_AND_CHECK(arg.number);
  WRITE_AND_CHECK(arg.connId);
  WRITE_AND_CHECK(arg.historyInfo);
  WRITE_AND_CHECK(arg.hasHoldTone);
  WRITE_AND_CHECK(arg.holdTone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SuppSvcNotificationInfo>(RIL_SuppSvcNotificationInfo& arg) const
{
  READ_AND_CHECK(arg.notificationType);
  READ_AND_CHECK(arg.code);
  READ_AND_CHECK(arg.index);
  READ_AND_CHECK(arg.type);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.connId);
  READ_AND_CHECK(arg.historyInfo);
  READ_AND_CHECK(arg.hasHoldTone);
  READ_AND_CHECK(arg.holdTone);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SuppSvcNotificationInfo>(RIL_SuppSvcNotificationInfo& arg)
{
  release(arg.notificationType);
  release(arg.code);
  release(arg.index);
  release(arg.type);
  release(arg.number);
  release(arg.connId);
  release(arg.historyInfo);
  release(arg.hasHoldTone);
  release(arg.holdTone);
  return Result::SUCCESS;
}
