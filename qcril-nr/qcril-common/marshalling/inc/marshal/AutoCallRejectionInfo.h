/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QTI_MARSHALLING_AUTO_CALL_REJECTION_INFO_H_
#define __QTI_MARSHALLING_AUTO_CALL_REJECTION_INFO_H_

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_AutoCallRejectionInfo>(const RIL_AutoCallRejectionInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_AutoCallRejectionInfo>(RIL_AutoCallRejectionInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_AutoCallRejectionInfo>(RIL_AutoCallRejectionInfo& arg);

#endif
