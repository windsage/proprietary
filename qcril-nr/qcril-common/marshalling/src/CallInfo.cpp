/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <marshal/CallInfo.h>
#include <Marshal.h>

template <>
Marshal::Result Marshal::write<RIL_CallFailCauseResponse>(const RIL_CallFailCauseResponse &arg) {
    WRITE_AND_CHECK(arg.failCause);
    WRITE_AND_CHECK(arg.extendedFailCause);
    WRITE_AND_CHECK(arg.networkErrorString);
    WRITE_AND_CHECK(arg.errorDetails, 1);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallFailCauseResponse>(RIL_CallFailCauseResponse &arg) const {
    READ_AND_CHECK(arg.failCause);
    READ_AND_CHECK(arg.extendedFailCause);
    READ_AND_CHECK(arg.networkErrorString);
    size_t sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.errorDetails, sz));
    if (sz != 0 && sz != 1) {
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallFailCauseResponse>(RIL_CallFailCauseResponse &arg) {
    release(arg.failCause);
    release(arg.extendedFailCause);
    release(arg.networkErrorString);
    release(arg.errorDetails, 1);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_VerstatInfo>(const RIL_VerstatInfo &arg) {
    WRITE_AND_CHECK(arg.canMarkUnwantedCall);
    WRITE_AND_CHECK(arg.verificationStatus);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_VerstatInfo>(RIL_VerstatInfo &arg) const {
    READ_AND_CHECK(arg.canMarkUnwantedCall);
    READ_AND_CHECK(arg.verificationStatus);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_VerstatInfo>(RIL_VerstatInfo & arg) {
    release(arg.canMarkUnwantedCall);
    release(arg.verificationStatus);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CallProgressInfo>(const RIL_CallProgressInfo &arg) {
    WRITE_AND_CHECK(arg.type);
    WRITE_AND_CHECK(arg.reasonCode);
    WRITE_AND_CHECK(arg.reasonText);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallProgressInfo>(RIL_CallProgressInfo &arg) const {
    READ_AND_CHECK(arg.type);
    READ_AND_CHECK(arg.reasonCode);
    READ_AND_CHECK(arg.reasonText);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallProgressInfo>(RIL_CallProgressInfo &arg) {
    release(arg.type);
    release(arg.reasonCode);
    release(arg.reasonText);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_MsimAdditionalCallInfo>(
    const RIL_MsimAdditionalCallInfo &arg) {
  WRITE_AND_CHECK(arg.additionalCode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_MsimAdditionalCallInfo>(
    RIL_MsimAdditionalCallInfo &arg) const {
  READ_AND_CHECK(arg.additionalCode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_MsimAdditionalCallInfo>(RIL_MsimAdditionalCallInfo &arg) {
  release(arg.additionalCode);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CallAudioQuality>(const RIL_CallAudioQuality& arg) {
  WRITE_AND_CHECK(arg.codec);
  WRITE_AND_CHECK(arg.computedAudioQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallAudioQuality>(RIL_CallAudioQuality& arg) const {
  READ_AND_CHECK(arg.codec);
  READ_AND_CHECK(arg.computedAudioQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallAudioQuality>(RIL_CallAudioQuality& arg) {
  release(arg.codec);
  release(arg.computedAudioQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_CallInfo>(const RIL_CallInfo& arg) {
  WRITE_AND_CHECK(arg.callState);
  WRITE_AND_CHECK(arg.index);
  WRITE_AND_CHECK(arg.toa);
  WRITE_AND_CHECK(arg.isMpty);
  WRITE_AND_CHECK(arg.isMt);
  WRITE_AND_CHECK(arg.als);
  WRITE_AND_CHECK(arg.isVoice);
  WRITE_AND_CHECK(arg.isVoicePrivacy);
  WRITE_AND_CHECK(arg.number);
  WRITE_AND_CHECK(arg.numberPresentation);
  WRITE_AND_CHECK(arg.name);
  WRITE_AND_CHECK(arg.namePresentation);
  WRITE_AND_CHECK(arg.redirNum);
  WRITE_AND_CHECK(arg.redirNumPresentation);
  WRITE_AND_CHECK(arg.callType);
  WRITE_AND_CHECK(arg.callDomain);
  WRITE_AND_CHECK(arg.callSubState);
  WRITE_AND_CHECK(arg.isEncrypted);
  WRITE_AND_CHECK(arg.isCalledPartyRinging);
  WRITE_AND_CHECK(arg.isVideoConfSupported);
  WRITE_AND_CHECK(arg.historyInfo);
  WRITE_AND_CHECK(arg.mediaId);
  WRITE_AND_CHECK(arg.causeCode);
  WRITE_AND_CHECK(arg.rttMode);
  WRITE_AND_CHECK(arg.sipAlternateUri);
  WRITE_AND_CHECK(arg.localAbility, arg.localAbilityLen);
  WRITE_AND_CHECK(arg.peerAbility, arg.peerAbilityLen);
  WRITE_AND_CHECK(arg.callFailCauseResponse, 1);
  WRITE_AND_CHECK(arg.terminatingNumber);
  WRITE_AND_CHECK(arg.isSecondary);
  WRITE_AND_CHECK(arg.verstatInfo, 1);
  WRITE_AND_CHECK(arg.uusInfo, 1);
  WRITE_AND_CHECK(arg.displayText);
  WRITE_AND_CHECK(arg.additionalCallInfo);
  WRITE_AND_CHECK(arg.childNumber);
  WRITE_AND_CHECK(arg.emergencyServiceCategory);
  WRITE_AND_CHECK(arg.tirMode);
  WRITE_AND_CHECK(arg.callProgInfo, 1);
  WRITE_AND_CHECK(arg.msimAdditionalCallInfo, 1);
  WRITE_AND_CHECK(arg.audioQuality, 1);
  WRITE_AND_CHECK(arg.modemCallId);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CallInfo>(RIL_CallInfo &arg) const {
    READ_AND_CHECK(arg.callState);
    READ_AND_CHECK(arg.index);
    READ_AND_CHECK(arg.toa);
    READ_AND_CHECK(arg.isMpty);
    READ_AND_CHECK(arg.isMt);
    READ_AND_CHECK(arg.als);
    READ_AND_CHECK(arg.isVoice);
    READ_AND_CHECK(arg.isVoicePrivacy);
    READ_AND_CHECK(arg.number);
    READ_AND_CHECK(arg.numberPresentation);
    READ_AND_CHECK(arg.name);
    READ_AND_CHECK(arg.namePresentation);
    READ_AND_CHECK(arg.redirNum);
    READ_AND_CHECK(arg.redirNumPresentation);
    READ_AND_CHECK(arg.callType);
    READ_AND_CHECK(arg.callDomain);
    READ_AND_CHECK(arg.callSubState);
    READ_AND_CHECK(arg.isEncrypted);
    READ_AND_CHECK(arg.isCalledPartyRinging);
    READ_AND_CHECK(arg.isVideoConfSupported);
    READ_AND_CHECK(arg.historyInfo);
    READ_AND_CHECK(arg.mediaId);
    READ_AND_CHECK(arg.causeCode);
    READ_AND_CHECK(arg.rttMode);
    READ_AND_CHECK(arg.sipAlternateUri);
    RUN_AND_CHECK(readAndAlloc(arg.localAbility, arg.localAbilityLen));
    RUN_AND_CHECK(readAndAlloc(arg.peerAbility, arg.peerAbilityLen));
    size_t sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.callFailCauseResponse, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    READ_AND_CHECK(arg.terminatingNumber);
    READ_AND_CHECK(arg.isSecondary);
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.verstatInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.uusInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    READ_AND_CHECK(arg.displayText);
    READ_AND_CHECK(arg.additionalCallInfo);
    READ_AND_CHECK(arg.childNumber);
    READ_AND_CHECK(arg.emergencyServiceCategory);
    READ_AND_CHECK(arg.tirMode);
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.callProgInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.msimAdditionalCallInfo, sz));
    if (sz != 1 && sz != 0) {
        return Result::FAILURE;
    }
    sz = 0;
    RUN_AND_CHECK(readAndAlloc(arg.audioQuality, sz));
    if (sz != 1 && sz != 0) {
      return Result::FAILURE;
    }
    READ_AND_CHECK(arg.modemCallId);
    return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_CallInfo>(RIL_CallInfo &arg) {
    release(arg.callState);
    release(arg.index);
    release(arg.toa);
    release(arg.isMpty);
    release(arg.isMt);
    release(arg.als);
    release(arg.isVoice);
    release(arg.isVoicePrivacy);
    release(arg.number);
    release(arg.numberPresentation);
    release(arg.name);
    release(arg.namePresentation);
    release(arg.redirNum);
    release(arg.redirNumPresentation);
    release(arg.callType);
    release(arg.callDomain);
    release(arg.callSubState);
    release(arg.isEncrypted);
    release(arg.isCalledPartyRinging);
    release(arg.isVideoConfSupported);
    release(arg.historyInfo);
    release(arg.mediaId);
    release(arg.causeCode);
    release(arg.rttMode);
    release(arg.sipAlternateUri);
    release(arg.localAbility, arg.localAbilityLen);
    release(arg.peerAbility, arg.peerAbilityLen);
    release(arg.callFailCauseResponse, 1);
    release(arg.terminatingNumber);
    release(arg.isSecondary);
    release(arg.verstatInfo, 1);
    release(arg.uusInfo, 1);
    release(arg.displayText);
    release(arg.additionalCallInfo);
    release(arg.childNumber);
    release(arg.emergencyServiceCategory);
    release(arg.tirMode);
    release(arg.callProgInfo, 1);
    release(arg.msimAdditionalCallInfo, 1);
    release(arg.audioQuality, 1);
    release(arg.modemCallId);
    return Result::SUCCESS;
}
