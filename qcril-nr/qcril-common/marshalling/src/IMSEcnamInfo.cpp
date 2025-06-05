/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSEcnamInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_EcnamInfo>(const RIL_IMS_EcnamInfo& arg) {
  WRITE_AND_CHECK(arg.name);
  WRITE_AND_CHECK(arg.iconUrl);
  WRITE_AND_CHECK(arg.infoUrl);
  WRITE_AND_CHECK(arg.cardUrl);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_EcnamInfo>(RIL_IMS_EcnamInfo& arg) const {
  READ_AND_CHECK(arg.name);
  READ_AND_CHECK(arg.iconUrl);
  READ_AND_CHECK(arg.infoUrl);
  READ_AND_CHECK(arg.cardUrl);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_EcnamInfo>(RIL_IMS_EcnamInfo& arg) {
  release(arg.name);
  release(arg.iconUrl);
  release(arg.infoUrl);
  release(arg.cardUrl);
  return Result::SUCCESS;
}
