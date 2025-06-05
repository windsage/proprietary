/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSSubConfigInfo.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SubConfigInfo>(const RIL_IMS_SubConfigInfo& arg) {
  WRITE_AND_CHECK(arg.simultStackCount);
  WRITE_AND_CHECK(arg.imsStackEnabledLen);
  if (arg.imsStackEnabledLen) {
    if (arg.imsStackEnabled) {
      for (size_t i = 0; i < arg.imsStackEnabledLen; i++) {
        WRITE_AND_CHECK(arg.imsStackEnabled[i]);
      }
    }
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_SubConfigInfo>(RIL_IMS_SubConfigInfo& arg) const {
  size_t imsStackEnabledLen;
  READ_AND_CHECK(arg.simultStackCount);
  RUN_AND_CHECK(readAndAlloc(arg.imsStackEnabled, imsStackEnabledLen));
  arg.imsStackEnabledLen = imsStackEnabledLen;
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_SubConfigInfo>(RIL_IMS_SubConfigInfo& arg) {
  release(arg.simultStackCount);
  if (arg.imsStackEnabledLen && arg.imsStackEnabled) {
    delete []arg.imsStackEnabled;
    arg.imsStackEnabled = nullptr;
  }
  return Result::SUCCESS;
}
