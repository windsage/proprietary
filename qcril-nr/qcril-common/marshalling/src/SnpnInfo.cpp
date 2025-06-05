/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <telephony/ril.h>
#include <marshal/SnpnInfo.h>

template <>
Marshal::Result Marshal::write<RIL_SnpnInfo>(const RIL_SnpnInfo& arg)
{
  WRITE_AND_CHECK(arg.nid);
  WRITE_AND_CHECK(arg.mcc);
  WRITE_AND_CHECK(arg.mnc);
  WRITE_AND_CHECK(arg.signalStrength);
  WRITE_AND_CHECK(arg.signalQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::read<RIL_SnpnInfo>(RIL_SnpnInfo& arg) const
{
  READ_AND_CHECK(arg.nid);
  READ_AND_CHECK(arg.mcc);
  READ_AND_CHECK(arg.mnc);
  READ_AND_CHECK(arg.signalStrength);
  READ_AND_CHECK(arg.signalQuality);
  return Result::SUCCESS;
}

template <>
Marshal::Result Marshal::release<RIL_SnpnInfo>(RIL_SnpnInfo& arg)
{
  release(arg.nid);
  release(arg.mcc);
  release(arg.mnc);
  release(arg.signalStrength);
  release(arg.signalQuality);
  return Result::SUCCESS;
}
