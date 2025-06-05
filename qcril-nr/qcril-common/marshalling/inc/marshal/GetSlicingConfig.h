/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <telephony/ril.h>
#include <iostream>
#include <Marshal.h>

#ifndef _QTI_MARSHALLING_GETSLICINGCONFIG
#define _QTI_MARSHALLING_GETSLICINGCONFIG

template <>
Marshal::Result Marshal::write<rilDnn>(const rilDnn &arg);
template <>
Marshal::Result Marshal::read<rilDnn>(rilDnn &arg) const;
template <>
Marshal::Result Marshal::release<rilDnn>(rilDnn &arg);
template <>
Marshal::Result Marshal::write<rilRouteSelectorDescriptor>(const rilRouteSelectorDescriptor &arg);
template <>
Marshal::Result Marshal::read<rilRouteSelectorDescriptor>(rilRouteSelectorDescriptor &arg) const;
template <>
Marshal::Result Marshal::release<rilRouteSelectorDescriptor>(rilRouteSelectorDescriptor &arg);
template <>
Marshal::Result Marshal::write<rilUrspRule>(const rilUrspRule &arg);
template <>
Marshal::Result Marshal::read<rilUrspRule>(rilUrspRule &arg) const;
template <>
Marshal::Result Marshal::release<rilUrspRule>(rilUrspRule &arg);
template <>
Marshal::Result Marshal::write<rilSlicingConfig>(const rilSlicingConfig &arg);
template <>
Marshal::Result Marshal::read<rilSlicingConfig>(rilSlicingConfig &arg) const;
template <>
Marshal::Result Marshal::release<rilSlicingConfig>(rilSlicingConfig &arg);

#endif