/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020, 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#ifndef LOC_NET_IFACE_AGPS_H
#define LOC_NET_IFACE_AGPS_H

#include <LocNetIfaceBase.h>
#include <gps_extended_c.h>

/* AGPS state Enum */
typedef enum {
    LOC_NET_AGPS_STATE_INVALID,
    LOC_NET_AGPS_STATE_OPEN_PENDING,
    LOC_NET_AGPS_STATE_OPENED,
    LOC_NET_AGPS_STATE_CLOSE_PENDING,
    LOC_NET_AGPS_STATE_CLOSED,
    LOC_NET_AGPS_STATE_MAX
} LocNetAgpsState;

/*--------------------------------------------------------------------
 * CLASS LocNetIfaceAgps
 *
 * Functionality:
 * This class holds reference to LocNetIface instances for AGPS
 *-------------------------------------------------------------------*/
class LocNetIfaceAgps {

public:
    /* status method registered as part of AGPS Extended callbacks */
    static void agpsStatusCb(AGnssExtStatusIpV4 status);

    /* Callbacks registered with LocNetIface instance */
    static void wwanStatusCallback(LocNetWwanCallEvent event, LocNetConnType locNetConnType,
                                   const char* apn, LocApnIpType apnIpType);

    /* LocNetIface instances for different clients */
    static LocNetIfaceBase* sLocNetIfaceAgps;

    /* AGPS state */
    static LocNetAgpsState sAgpsState;

    /* AGPS interface methods to be invoked on call setup/failure */
    static LocAgpsOpenResultCb sAgpsOpenResultCb;
    static LocAgpsCloseResultCb sAgpsCloseResultCb;
    static void* sUserDataPtr;
    static AgpsCbInfo sAgpsCbInfo;
};

/* Global method accessed from HAL to fetch AGPS status cb */
extern "C" AgpsCbInfo& LocNetIfaceAgps_getAgpsCbInfo(
        LocAgpsOpenResultCb openResultCb,
        LocAgpsCloseResultCb closeResultCb, void* userDataPtr);

#endif /* #ifndef LOC_NET_IFACE_AGPS_H */
