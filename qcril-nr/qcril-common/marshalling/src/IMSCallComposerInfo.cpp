/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/IMSCallComposerInfo.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_CallComposerInfo>(const RIL_IMS_CallComposerInfo& arg) {
  WRITE_AND_CHECK(arg.priority);
  WRITE_AND_CHECK(arg.subject);
  WRITE_AND_CHECK(arg.location);
  WRITE_AND_CHECK(arg.imageUrl);
  WRITE_AND_CHECK(arg.organization);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_IMS_CallComposerLocation>(const RIL_IMS_CallComposerLocation& arg) {
  WRITE_AND_CHECK(arg.radius);
  WRITE_AND_CHECK(arg.latitude);
  WRITE_AND_CHECK(arg.longitude);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallComposerInfo>(RIL_IMS_CallComposerInfo& arg) const {
  READ_AND_CHECK(arg.priority);
  READ_AND_CHECK(arg.subject);
  READ_AND_CHECK(arg.location);
  READ_AND_CHECK(arg.imageUrl);
  READ_AND_CHECK(arg.organization);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_IMS_CallComposerLocation>(RIL_IMS_CallComposerLocation& arg) const {
  READ_AND_CHECK(arg.radius);
  READ_AND_CHECK(arg.latitude);
  READ_AND_CHECK(arg.longitude);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallComposerInfo>(RIL_IMS_CallComposerInfo& arg) {
  release(arg.priority);
  release(arg.subject);
  release(arg.location);
  release(arg.imageUrl);
  release(arg.organization);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_IMS_CallComposerLocation>(RIL_IMS_CallComposerLocation& arg) {
  release(arg.radius);
  release(arg.latitude);
  release(arg.longitude);
  return Result::SUCCESS;
}
