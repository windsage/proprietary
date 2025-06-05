/*
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/Call.h>
#include <marshal/UUS_Info.h>
#include <telephony/ril.h>

template <>
Marshal::Result Marshal::write<RIL_Call>(const RIL_Call& arg) {
  WRITE_AND_CHECK(arg.state);
  WRITE_AND_CHECK(arg.index);
  WRITE_AND_CHECK(arg.toa);
  WRITE_AND_CHECK(arg.isMpty);
  WRITE_AND_CHECK(arg.isMT);
  WRITE_AND_CHECK(arg.als);
  WRITE_AND_CHECK(arg.isVoice);
  WRITE_AND_CHECK(arg.isVoicePrivacy);
  if (arg.number) {
    RUN_AND_CHECK(write<char>(arg.number));
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.numberPresentation);
  if (arg.name) {
    RUN_AND_CHECK(write<char>(arg.name));
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.namePresentation);
  int32_t uusPresent = 0;
  if (arg.uusInfo) {
    uusPresent = 1;
    WRITE_AND_CHECK(uusPresent);
    WRITE_AND_CHECK(*(arg.uusInfo));
  } else {
    WRITE_AND_CHECK(uusPresent);
  }
  WRITE_AND_CHECK(arg.audioQuality);
  if (arg.redirNum) {
    RUN_AND_CHECK(write<char>(arg.redirNum));
  } else {
    WRITE_AND_CHECK((int32_t)-1);
  }
  WRITE_AND_CHECK(arg.redirNumPresentation);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_Call>(RIL_Call& arg) const {
  READ_AND_CHECK(arg.state);
  READ_AND_CHECK(arg.index);
  READ_AND_CHECK(arg.toa);
  READ_AND_CHECK(arg.isMpty);
  READ_AND_CHECK(arg.isMT);
  READ_AND_CHECK(arg.als);
  READ_AND_CHECK(arg.isVoice);
  READ_AND_CHECK(arg.isVoicePrivacy);
  READ_AND_CHECK(arg.number);
  READ_AND_CHECK(arg.numberPresentation);
  READ_AND_CHECK(arg.name);
  READ_AND_CHECK(arg.namePresentation);
  int32_t uusPresent = 0;
  READ_AND_CHECK(uusPresent);
  if (uusPresent) {
    arg.uusInfo = new RIL_UUS_Info();
    if (arg.uusInfo) {
      READ_AND_CHECK(*(arg.uusInfo));
    }
  }
  READ_AND_CHECK(arg.audioQuality);
  READ_AND_CHECK(arg.redirNum);
  READ_AND_CHECK(arg.redirNumPresentation);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_Call>(RIL_Call& arg) {
  release(arg.state);
  release(arg.index);
  release(arg.toa);
  release(arg.isMpty);
  release(arg.isMT);
  release(arg.als);
  release(arg.isVoice);
  release(arg.isVoicePrivacy);
  release(arg.number);
  release(arg.numberPresentation);
  release(arg.name);
  release(arg.namePresentation);
  if (arg.uusInfo) {
    release(*arg.uusInfo);
    delete arg.uusInfo;
    arg.uusInfo = nullptr;
  }
  release(arg.audioQuality);
  release(arg.redirNum);
  release(arg.redirNumPresentation);
  return Result::SUCCESS;
}
