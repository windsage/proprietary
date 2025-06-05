/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _QTI_MARSHALLING_RIL_QosSession
#define _QTI_MARSHALLING_RIL_QosSession

#include <Marshal.h>
#include <telephony/ril.h>
template <>
Marshal::Result Marshal::write<RIL_QosBandwidth>(const RIL_QosBandwidth &arg);
template <>
Marshal::Result Marshal::read<RIL_QosBandwidth>(RIL_QosBandwidth &arg) const;
template <>
Marshal::Result Marshal::release<RIL_QosBandwidth>(RIL_QosBandwidth &arg);
template <>
Marshal::Result Marshal::write<RIL_EpsQos>(const RIL_EpsQos &arg);
template <>
Marshal::Result Marshal::read<RIL_EpsQos>(RIL_EpsQos &arg) const;
template <>
Marshal::Result Marshal::release<RIL_EpsQos>(RIL_EpsQos &arg);
template <>
Marshal::Result Marshal::write<RIL_NrQos>(const RIL_NrQos &arg);
template <>
Marshal::Result Marshal::read<RIL_NrQos>(RIL_NrQos &arg) const;
template <>
Marshal::Result Marshal::release<RIL_NrQos>(RIL_NrQos &arg);
template <>
Marshal::Result Marshal::write<RIL_Qos>(const RIL_Qos &arg);
template <>
Marshal::Result Marshal::read<RIL_Qos>(RIL_Qos &arg) const;
template <>
Marshal::Result Marshal::release<RIL_Qos>(RIL_Qos &arg);
template <>
Marshal::Result Marshal::write<RIL_PortRange>(const RIL_PortRange &arg);
template <>
Marshal::Result Marshal::read<RIL_PortRange>(RIL_PortRange &arg) const;
template <>
Marshal::Result Marshal::release<RIL_PortRange>(RIL_PortRange &arg);
template <>
Marshal::Result Marshal::write<RIL_QosFilter>(const RIL_QosFilter &arg);
template <>
Marshal::Result Marshal::read<RIL_QosFilter>(RIL_QosFilter &arg) const;
template <>
Marshal::Result Marshal::release<RIL_QosFilter>(RIL_QosFilter &arg);
template <>
Marshal::Result Marshal::write<RIL_QosSession>(const RIL_QosSession &arg);
template <>
Marshal::Result Marshal::read<RIL_QosSession>(RIL_QosSession &arg) const;
template <>
Marshal::Result Marshal::release<RIL_QosSession>(RIL_QosSession &arg);

#endif

