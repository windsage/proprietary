/******************************************************************************
#  Copyright (c) 2021 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <telephony/ril.h>
#include <marshal/LpaUserReq.h>

template <>
Marshal::Result Marshal::write<UimLpaSrvAddrOpReq>(const UimLpaSrvAddrOpReq& arg) {
  write(static_cast<uint32_t>(arg.opCode));
  if (arg.smdpAddress) {
    write<char>(arg.smdpAddress);
  } else {
    write((int32_t)-1);
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaSrvAddrOpReq>(UimLpaSrvAddrOpReq& arg) const {
  read(*reinterpret_cast<uint32_t*>(&arg.opCode));
  size_t length = 0;
  readAndAlloc(arg.smdpAddress, length);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaUserReq>(const UimLpaUserReq& arg) {

  WRITE_AND_CHECK(arg.event);
  WRITE_AND_CHECK(arg.activationCode);
  WRITE_AND_CHECK(arg.confirmationCode);
  WRITE_AND_CHECK(arg.nickname);
  WRITE_AND_CHECK(arg.iccid);
  WRITE_AND_CHECK(arg.resetMask);
  WRITE_AND_CHECK(arg.userOk);
  WRITE_AND_CHECK(arg.nok_reason);
  WRITE_AND_CHECK((arg.srvOpReq));

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaUserReq>(UimLpaUserReq& arg) const {

  READ_AND_CHECK(arg.event);
  READ_AND_CHECK(arg.activationCode);
  READ_AND_CHECK(arg.confirmationCode);
  READ_AND_CHECK(arg.nickname);
  READ_AND_CHECK(arg.iccid);
  READ_AND_CHECK(arg.resetMask);
  READ_AND_CHECK(arg.userOk);
  READ_AND_CHECK(arg.nok_reason);
  READ_AND_CHECK((arg.srvOpReq));
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaProfileInfo>(UimLpaProfileInfo& arg) const {

  READ_AND_CHECK(arg.state);
  size_t sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.iccid, sz));
  arg.iccid_len = static_cast<int32_t>(sz);
  READ_AND_CHECK(arg.profileName);
  READ_AND_CHECK(arg.nickName);
  READ_AND_CHECK(arg.spName);
  READ_AND_CHECK(arg.iconType);
  sz = 0;
  RUN_AND_CHECK(readAndAlloc(arg.icon, sz));
  arg.icon_len = static_cast<int32_t>(sz);
  READ_AND_CHECK(arg.profileClass);
  READ_AND_CHECK(arg.profilePolicy);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaSrvAddrOpResp>(UimLpaSrvAddrOpResp &arg) const{

  READ_AND_CHECK(arg.smdpAddress);
  READ_AND_CHECK(arg.smdsAddress);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaUserResponse>(UimLpaUserResponse& arg) const {

  READ_AND_CHECK(arg.event);
  READ_AND_CHECK(arg.result);
  size_t sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.eid, sz));
  arg.eid_len = static_cast<int32_t>(sz);
  READ_AND_CHECK(arg.no_of_profiles);
  sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.profiles,sz));
  READ_AND_CHECK(arg.srvAddr);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaProfileInfo>(const UimLpaProfileInfo& arg) {

  WRITE_AND_CHECK(arg.state);
  WRITE_AND_CHECK(arg.iccid, static_cast<size_t> (arg.iccid_len));
  WRITE_AND_CHECK(arg.profileName);
  WRITE_AND_CHECK(arg.nickName);
  WRITE_AND_CHECK(arg.spName);
  WRITE_AND_CHECK(arg.iconType);
  WRITE_AND_CHECK(arg.icon, static_cast<size_t> (arg.icon_len));
  WRITE_AND_CHECK(arg.profileClass);
  WRITE_AND_CHECK(arg.profilePolicy);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaSrvAddrOpResp>(const UimLpaSrvAddrOpResp &arg) {

  WRITE_AND_CHECK(arg.smdpAddress);
  WRITE_AND_CHECK(arg.smdsAddress);
  return Result::SUCCESS;
}
template <>
Marshal::Result Marshal::write<UimLpaUserResponse>(const UimLpaUserResponse& arg) {

  WRITE_AND_CHECK(arg.event);
  WRITE_AND_CHECK(arg.result);
  WRITE_AND_CHECK(arg.eid, static_cast<size_t> (arg.eid_len));
  WRITE_AND_CHECK(arg.no_of_profiles);
  WRITE_AND_CHECK(arg.profiles,arg.no_of_profiles);
  WRITE_AND_CHECK(arg.srvAddr);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaHttpCustomHeader>(UimLpaHttpCustomHeader& arg) const {
  READ_AND_CHECK(arg.headerName);
  READ_AND_CHECK(arg.headerValue);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaHttpCustomHeader>(const UimLpaHttpCustomHeader& arg) {

  WRITE_AND_CHECK(arg.headerName);
  WRITE_AND_CHECK(arg.headerValue);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaHttpTrnsReq>(const UimLpaHttpTrnsReq& arg) {

  WRITE_AND_CHECK(arg.tokenId);
  WRITE_AND_CHECK(arg.result);
  WRITE_AND_CHECK(arg.payload,static_cast<size_t>(arg.payload_len));
  WRITE_AND_CHECK(arg.no_of_headers);
  WRITE_AND_CHECK(arg.headers,arg.no_of_headers);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaHttpTrnsReq>(UimLpaHttpTrnsReq& arg) const {

  READ_AND_CHECK(arg.tokenId);
  READ_AND_CHECK(arg.result);
  size_t sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.payload, sz));
  arg.payload_len = static_cast<int32_t>(sz);
  READ_AND_CHECK(arg.no_of_headers);
  sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.headers,sz));
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaAddProfProgressInd>(UimLpaAddProfProgressInd& arg) const {

  READ_AND_CHECK(arg.status);
  READ_AND_CHECK(arg.cause);
  READ_AND_CHECK(arg.progress);
  READ_AND_CHECK(arg.policyMask);
  READ_AND_CHECK(arg.userConsentRequired);
  READ_AND_CHECK(arg.profileName);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaAddProfProgressInd>(const UimLpaAddProfProgressInd& arg) {

  WRITE_AND_CHECK(arg.status);
  WRITE_AND_CHECK(arg.cause);
  WRITE_AND_CHECK(arg.progress);
  WRITE_AND_CHECK(arg.policyMask);
  WRITE_AND_CHECK(arg.userConsentRequired);
  WRITE_AND_CHECK(arg.profileName);
  return Result::SUCCESS;
}


template <>
Marshal::Result Marshal::read<UimLpaHttpCustomHdrType>(UimLpaHttpCustomHdrType& arg) const {
   READ_AND_CHECK(arg.headerName);
   READ_AND_CHECK(arg.headerValue);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaHttpCustomHdrType>(const UimLpaHttpCustomHdrType& arg) {
   WRITE_AND_CHECK(arg.headerName);
   WRITE_AND_CHECK(arg.headerValue);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<UimLpaHttpTxnIndype>(UimLpaHttpTxnIndype& arg) const {

  READ_AND_CHECK(arg.tokenId);
  size_t sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.payload, sz));
  arg.payload_len = static_cast<int32_t>(sz);
  READ_AND_CHECK(arg.contentType);
  READ_AND_CHECK(arg.no_of_headers);
  sz=0;
  RUN_AND_CHECK(readAndAlloc(arg.customHeaders,sz));
  READ_AND_CHECK(arg.url);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<UimLpaHttpTxnIndype>(const UimLpaHttpTxnIndype& arg) {

  WRITE_AND_CHECK(arg.tokenId);
  WRITE_AND_CHECK(arg.payload, static_cast<size_t> (arg.payload_len));
  WRITE_AND_CHECK(arg.contentType);
  WRITE_AND_CHECK(arg.no_of_headers);
  WRITE_AND_CHECK(arg.customHeaders,arg.no_of_headers);
  WRITE_AND_CHECK(arg.url);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaUserReq>(UimLpaUserReq& arg) {
  release(arg.event);
  release(arg.activationCode);
  release(arg.confirmationCode);
  release(arg.nickname);
  release(arg.iccid);
  release(arg.resetMask);
  release(arg.userOk);
  release(arg.nok_reason);
  release((arg.srvOpReq));

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaSrvAddrOpReq>(UimLpaSrvAddrOpReq& arg) {
  release(arg.smdpAddress);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaUserResponse>(UimLpaUserResponse& arg) {
  release(arg.event);
  release(arg.result);
  release(arg.eid);
  release(arg.no_of_profiles);
  release(arg.profiles, arg.no_of_profiles);
  release(arg.srvAddr);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaSrvAddrOpResp>(UimLpaSrvAddrOpResp& arg) {
  release(arg.smdpAddress);
  release(arg.smdsAddress);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaHttpTrnsReq>(UimLpaHttpTrnsReq& arg) {
  release(arg.tokenId);
  release(arg.result);
  release(arg.payload);
  release(arg.no_of_headers);
  release(arg.headers, arg.no_of_headers);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaHttpCustomHeader>(UimLpaHttpCustomHeader& arg) {
  release(arg.headerName);
  release(arg.headerValue);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaProfileInfo>(UimLpaProfileInfo& arg) {
  release(arg.state);
  release(arg.iccid);
  release(arg.profileName);
  release(arg.nickName);
  release(arg.spName);
  release(arg.icon);
  release(arg.profileClass);
  release(arg.profilePolicy);

  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaHttpTxnIndype>(UimLpaHttpTxnIndype& arg) {
  release(arg.tokenId);
  release(arg.payload);
  release(arg.customHeaders, arg.no_of_headers);
  release(arg.url);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaHttpCustomHdrType>(UimLpaHttpCustomHdrType& arg) {
  release(arg.headerName);
  release(arg.headerValue);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<UimLpaAddProfProgressInd>(UimLpaAddProfProgressInd& arg) {

  release(arg.status);
  release(arg.cause);
  release(arg.progress);
  release(arg.policyMask);
  release(arg.userConsentRequired);
  release(arg.profileName);
  return Result::SUCCESS;
}
