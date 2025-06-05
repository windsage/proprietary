/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/SetNetworkSelectionManualParam.h>

template <>
Marshal::Result Marshal::write<RIL_SetNetworkSelectionManualParam>(
    const RIL_SetNetworkSelectionManualParam& arg)
{
  WRITE_AND_CHECK(arg.operatorNumeric);
  WRITE_AND_CHECK(arg.ran);
  WRITE_AND_CHECK(arg.accessMode);
  WRITE_AND_CHECK(arg.cagIdValid);
  WRITE_AND_CHECK(arg.cagId);
  WRITE_AND_CHECK(arg.snpnNidLength);
  WRITE_AND_CHECK(arg.snpnNid, arg.snpnNidLength);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SetNetworkSelectionManualParam>(
    RIL_SetNetworkSelectionManualParam& arg) const
{
  READ_AND_CHECK(arg.operatorNumeric);
  READ_AND_CHECK(arg.ran);
  READ_AND_CHECK(arg.accessMode);
  READ_AND_CHECK(arg.cagIdValid);
  READ_AND_CHECK(arg.cagId);
  READ_AND_CHECK(arg.snpnNidLength);
  READ_AND_CHECK(arg.snpnNid);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SetNetworkSelectionManualParam>(
    RIL_SetNetworkSelectionManualParam& arg)
{
  release(arg.operatorNumeric);
  release(arg.ran);
  release(arg.accessMode);
  release(arg.cagIdValid);
  release(arg.cagId);
  release(arg.snpnNidLength);
  release(arg.snpnNid);
  return Result::SUCCESS;
}
