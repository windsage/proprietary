/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/RILImeiInfo.h>
#include <iostream>

template <>
Marshal::Result Marshal::write<RIL_ImeiInfo>(const RIL_ImeiInfo& arg)
{
  WRITE_AND_CHECK(static_cast<uint8_t>(arg.imeiType));
  WRITE_AND_CHECK(arg.imei);
  WRITE_AND_CHECK(arg.imeiLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_ImeiInfo>(RIL_ImeiInfo& arg) const
{
  uint8_t val;
  READ_AND_CHECK(val);
  arg.imeiType = static_cast<RIL_ImeiType>(val);
  memset(arg.imei, 0x0, IMEI_MAX_BUFFER_LEN);
  READ_AND_CHECK(arg.imei);
  READ_AND_CHECK(arg.imeiLen);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_ImeiInfo>(RIL_ImeiInfo& arg)
{
  release(arg.imeiType);
  release(arg.imei);
  release(arg.imeiLen);
  return Result::SUCCESS;
}