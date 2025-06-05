/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _QTI_MARSHALLING_SUPP_SVC_NOTIFICATION
#define _QTI_MARSHALLING_SUPP_SVC_NOTIFICATION

#include <Marshal.h>
#include <telephony/ril_call.h>

template <>
Marshal::Result Marshal::write<RIL_SuppSvcNotificationInfo>(const RIL_SuppSvcNotificationInfo& arg);
template <>
Marshal::Result Marshal::read<RIL_SuppSvcNotificationInfo>(RIL_SuppSvcNotificationInfo& arg) const;
template <>
Marshal::Result Marshal::release<RIL_SuppSvcNotificationInfo>(RIL_SuppSvcNotificationInfo& arg);

#endif
