/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/DialParams.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_CallDetails>(const RIL_CallDetails& arg)
{
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.callDomain);
  WRITE_AND_CHECK(arg.extrasLength);
  for (int i = 0; i < arg.extrasLength; i++) {
    WRITE_AND_CHECK(arg.extras[i]);
  }
  WRITE_AND_CHECK(arg.rttMode);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallDetails>(RIL_CallDetails& arg) const
{
  size_t extrasLength;
  READ_AND_CHECK(arg.callType);
  READ_AND_CHECK(arg.callDomain);
  RUN_AND_CHECK(readAndAlloc(arg.extras, extrasLength));
  arg.extrasLength = extrasLength;
  READ_AND_CHECK(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallDetails>(RIL_CallDetails& arg)
{
  release(arg.callType);
  release(arg.callDomain);
  if (arg.extrasLength && arg.extras) {
    for (int i = 0; i < arg.extrasLength; i++) {
      release(arg.extras[i]);
    }
    delete[] arg.extras;
    arg.extras = nullptr;
  }
  release(arg.extrasLength);
  release(arg.rttMode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_MultiIdentityLineInfo>(const RIL_MultiIdentityLineInfo& arg)
{
  WRITE_AND_CHECK(arg.msisdn);
  WRITE_AND_CHECK(arg.registrationStatus);
  WRITE_AND_CHECK(arg.lineType);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_MultiIdentityLineInfo>(RIL_MultiIdentityLineInfo& arg) const
{
  READ_AND_CHECK(arg.msisdn);
  READ_AND_CHECK(arg.registrationStatus);
  READ_AND_CHECK(arg.lineType);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_MultiIdentityLineInfo>(RIL_MultiIdentityLineInfo& arg)
{
  release(arg.msisdn);
  release(arg.registrationStatus);
  release(arg.lineType);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_DialParams>(const RIL_DialParams& arg)
{
  WRITE_AND_CHECK(arg.address);
  WRITE_AND_CHECK(arg.clirMode);
  WRITE_AND_CHECK(arg.uusInfo, 1);
  WRITE_AND_CHECK(arg.presentation);
  WRITE_AND_CHECK((unsigned char)arg.hasCallDetails);
  if (arg.hasCallDetails) WRITE_AND_CHECK(arg.callDetails);
  WRITE_AND_CHECK((unsigned char)arg.hasIsConferenceUri);
  if (arg.hasIsConferenceUri) WRITE_AND_CHECK((unsigned char)arg.isConferenceUri);
  WRITE_AND_CHECK((unsigned char)arg.hasIsCallPull);
  if (arg.hasIsCallPull) WRITE_AND_CHECK((unsigned char)arg.isCallPull);
  WRITE_AND_CHECK((unsigned char)arg.hasIsEncrypted);
  if (arg.hasIsEncrypted) WRITE_AND_CHECK((unsigned char)arg.isEncrypted);
  WRITE_AND_CHECK(arg.multiLineInfo);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_DialParams>(RIL_DialParams& arg) const
{
  READ_AND_CHECK(arg.address);
  READ_AND_CHECK(arg.clirMode);
  size_t sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.uusInfo, sz));
  if (sz != 1 && sz != 0) {
    return Result::FAILURE;
  }
  READ_AND_CHECK(arg.presentation);

  unsigned char rdVal;
  READ_AND_CHECK(rdVal);
  arg.hasCallDetails = static_cast<bool>(rdVal);
  if (arg.hasCallDetails) READ_AND_CHECK(arg.callDetails);

  READ_AND_CHECK(rdVal);
  arg.hasIsConferenceUri = static_cast<bool>(rdVal);
  if (arg.hasIsConferenceUri) {
    READ_AND_CHECK(rdVal);
    arg.isConferenceUri = static_cast<bool>(rdVal);
  }

  READ_AND_CHECK(rdVal);
  arg.hasIsCallPull = static_cast<bool>(rdVal);
  if (arg.hasIsCallPull) {
    READ_AND_CHECK(rdVal);
    arg.isCallPull = static_cast<bool>(rdVal);
  }

  READ_AND_CHECK(rdVal);
  arg.hasIsEncrypted = static_cast<bool>(rdVal);
  if (arg.hasIsEncrypted) {
    READ_AND_CHECK(rdVal);
    arg.isEncrypted = static_cast<bool>(rdVal);
  }

  READ_AND_CHECK(arg.multiLineInfo);
  return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::release<RIL_DialParams>(RIL_DialParams& arg)
{
  release(arg.address);
  release(arg.clirMode);
  release(arg.uusInfo, 1);
  release(arg.presentation);
  release(arg.callDetails);
  release(arg.multiLineInfo);
  return Result::SUCCESS;
}
