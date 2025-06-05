/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_IMS_SRTP_ENCRYPTION_INFO
#define _QTI_MARSHALLING_IMS_SRTP_ENCRYPTION_INFO

#include <Marshal.h>
#include <telephony/ril_ims.h>

template <>
Marshal::Result Marshal::write<RIL_IMS_SrtpEncryptionStatus>(const RIL_IMS_SrtpEncryptionStatus& arg);
template <>
Marshal::Result Marshal::read<RIL_IMS_SrtpEncryptionStatus>(RIL_IMS_SrtpEncryptionStatus& arg) const;
template <>
Marshal::Result Marshal::release<RIL_IMS_SrtpEncryptionStatus>(RIL_IMS_SrtpEncryptionStatus& arg);

#endif
