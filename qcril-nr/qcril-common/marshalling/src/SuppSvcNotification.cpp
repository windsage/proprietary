/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/SuppSvcNotification.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_SuppSvcNotification>(const RIL_SuppSvcNotification& arg) {
  WRITE_AND_CHECK(arg.notificationType);
  WRITE_AND_CHECK(arg.code);
  WRITE_AND_CHECK(arg.index);
  WRITE_AND_CHECK(arg.type);
  if (arg.number) {
    WRITE_AND_CHECK(arg.number);
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.connId);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SuppSvcNotification>(RIL_SuppSvcNotification& arg) const {
  READ_AND_CHECK(arg.notificationType);
  READ_AND_CHECK(arg.code);
  READ_AND_CHECK(arg.index);
  READ_AND_CHECK(arg.type);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.connId);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SuppSvcNotification>(RIL_SuppSvcNotification& arg) {
  release(arg.notificationType);
  release(arg.code);
  release(arg.index);
  release(arg.type);
  release(arg.number);
  return Result::SUCCESS;
}

