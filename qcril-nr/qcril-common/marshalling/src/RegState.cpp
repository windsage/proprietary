/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_REG_STATE
#define _QTI_MARSHALLING_REG_STATE

#include <Marshal.h>
#include <telephony/ril.h>
#include <marshal/CellIdentity.h>
#include <marshal/RegState.h>

template <>
Marshal::Result Marshal::write<RIL_CellIdentity_v16>(const RIL_CellIdentity_v16& arg) {
  auto data = static_cast<uint32_t>(arg.cellInfoType);
  if (data > RIL_CELL_INFO_TYPE_NR) data = static_cast<uint32_t>(RIL_CELL_INFO_TYPE_NONE);
  write(data);
  switch (arg.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
      write<RIL_CellIdentityGsm_v12>(arg.cellIdentityGsm);
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      write<RIL_CellIdentityCdma>(arg.cellIdentityCdma);
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      write<RIL_CellIdentityLte_v12>(arg.cellIdentityLte);
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      write<RIL_CellIdentityWcdma_v12>(arg.cellIdentityWcdma);
      break;
#if 0
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        write<RIL_CellInfoTdscdma>(arg.cellIdentityTdscdma);
        break;
#endif
    case RIL_CELL_INFO_TYPE_NR:
      write<RIL_CellIdentityNr>(arg.cellIdentityNr);
      break;
    default:
      break;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_CellIdentity_v16>(RIL_CellIdentity_v16& arg) const {
  uint32_t data;
  read(data);
  if (data > RIL_CELL_INFO_TYPE_NR) data = static_cast<uint32_t>(RIL_CELL_INFO_TYPE_NONE);
  arg.cellInfoType = static_cast<RIL_CellInfoType>(data);
  switch (arg.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
      read<RIL_CellIdentityGsm_v12>(arg.cellIdentityGsm);
      break;
    case RIL_CELL_INFO_TYPE_CDMA:
      read<RIL_CellIdentityCdma>(arg.cellIdentityCdma);
      break;
    case RIL_CELL_INFO_TYPE_LTE:
      read<RIL_CellIdentityLte_v12>(arg.cellIdentityLte);
      break;
    case RIL_CELL_INFO_TYPE_WCDMA:
      read<RIL_CellIdentityWcdma_v12>(arg.cellIdentityWcdma);
      break;
#if 0
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        read<RIL_CellInfoTdscdma>(arg.cellIdentityTdscdma);
        break;
#endif
    case RIL_CELL_INFO_TYPE_NR:
      read<RIL_CellIdentityNr>(arg.cellIdentityNr);
      break;
    default:
      break;
  }
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<LteVopsInfo>(const LteVopsInfo& arg) {
  WRITE_AND_CHECK(arg.isVopsSupported);
  WRITE_AND_CHECK(arg.isEmcBearerSupported);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<LteVopsInfo>(LteVopsInfo& arg) const {
  READ_AND_CHECK(arg.isVopsSupported);
  READ_AND_CHECK(arg.isEmcBearerSupported);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<NrIndicators>(const NrIndicators& arg) {
  WRITE_AND_CHECK(arg.isEndcAvailable);
  WRITE_AND_CHECK(arg.isDcNrRestricted);
  WRITE_AND_CHECK(arg.plmnInfoListR15Available);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<NrIndicators>(NrIndicators& arg) const {
  READ_AND_CHECK(arg.isEndcAvailable);
  READ_AND_CHECK(arg.isDcNrRestricted);
  READ_AND_CHECK(arg.plmnInfoListR15Available);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<NrVopsInfo>(const NrVopsInfo& arg) {
  WRITE_AND_CHECK(arg.isVopsSupported);
  WRITE_AND_CHECK(arg.isEmcSupported);
  WRITE_AND_CHECK(arg.isEmfSupported);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<NrVopsInfo>(NrVopsInfo& arg) const {
  READ_AND_CHECK(arg.isVopsSupported);
  READ_AND_CHECK(arg.isEmcSupported);
  READ_AND_CHECK(arg.isEmfSupported);
  return Result::SUCCESS;
}


template <>
Marshal::Result Marshal::write<RIL_VoiceRegistrationStateResponse>(
    const RIL_VoiceRegistrationStateResponse& arg) {
  WRITE_AND_CHECK(static_cast<uint16_t>(arg.regState));
  WRITE_AND_CHECK(static_cast<uint16_t>(arg.rat));
  WRITE_AND_CHECK(arg.cssSupported);
  WRITE_AND_CHECK(arg.roamingIndicator);
  WRITE_AND_CHECK(arg.systemIsInPrl);
  WRITE_AND_CHECK(arg.defaultRoamingIndicator);
  WRITE_AND_CHECK(arg.reasonForDenial);
  write<RIL_CellIdentity_v16>(arg.cellIdentity);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_VoiceRegistrationStateResponse>(
    RIL_VoiceRegistrationStateResponse& arg) const {
  uint16_t data;
  read(data);
  arg.regState = static_cast<RIL_RegState>(data);
  read(data);
  arg.rat = static_cast<RIL_RadioTechnology>(data);
  READ_AND_CHECK(arg.cssSupported);
  READ_AND_CHECK(arg.roamingIndicator);
  READ_AND_CHECK(arg.systemIsInPrl);
  READ_AND_CHECK(arg.defaultRoamingIndicator);
  READ_AND_CHECK(arg.reasonForDenial);
  read<RIL_CellIdentity_v16>(arg.cellIdentity);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::write<RIL_DataRegistrationStateResponse>(
    const RIL_DataRegistrationStateResponse& arg) {
  write(static_cast<uint16_t>(arg.regState));
  WRITE_AND_CHECK(static_cast<uint16_t>(arg.rat));
  WRITE_AND_CHECK(arg.reasonDataDenied);
  WRITE_AND_CHECK(arg.maxDataCalls);
  write<RIL_CellIdentity_v16>(arg.cellIdentity);
  WRITE_AND_CHECK_OPTIONAL(arg.lteVopsInfoValid, arg.lteVopsInfo);
  WRITE_AND_CHECK_OPTIONAL(arg.nrIndicatorsValid, arg.nrIndicators);
  WRITE_AND_CHECK_OPTIONAL(arg.nrVopsInfoValid, arg.nrVopsInfo);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_DataRegistrationStateResponse>(
    RIL_DataRegistrationStateResponse& arg) const {
  uint16_t data;
  read(data);
  arg.regState = static_cast<RIL_RegState>(data);
  read(data);
  arg.rat = static_cast<RIL_RadioTechnology>(data);
  READ_AND_CHECK(arg.reasonDataDenied);
  READ_AND_CHECK(arg.maxDataCalls);
  read<RIL_CellIdentity_v16>(arg.cellIdentity);
  READ_AND_CHECK_OPTIONAL(arg.lteVopsInfoValid, arg.lteVopsInfo);
  READ_AND_CHECK_OPTIONAL(arg.nrIndicatorsValid, arg.nrIndicators);
  READ_AND_CHECK_OPTIONAL(arg.nrVopsInfoValid, arg.nrVopsInfo);
  return Result::SUCCESS;
}

#endif
