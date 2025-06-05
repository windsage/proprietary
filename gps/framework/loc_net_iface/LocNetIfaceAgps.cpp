/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#include <LocNetIfaceAgps.h>
#include <loc_pla.h>
#include <log_util.h>

#define LOG_TAG "LocSvc_LocNetIfaceHolder"

/* LocNetIfaceAgps members */
LocNetIfaceBase* LocNetIfaceAgps::sLocNetIfaceAgps = NULL;
LocNetAgpsState LocNetIfaceAgps::sAgpsState = LOC_NET_AGPS_STATE_CLOSED;
LocAgpsOpenResultCb LocNetIfaceAgps::sAgpsOpenResultCb = NULL;
LocAgpsCloseResultCb LocNetIfaceAgps::sAgpsCloseResultCb = NULL;
void* LocNetIfaceAgps::sUserDataPtr = NULL;
AgpsCbInfo LocNetIfaceAgps::sAgpsCbInfo = {};

/* Method accessed from HAL */
AgpsCbInfo& LocNetIfaceAgps_getAgpsCbInfo(
        LocAgpsOpenResultCb openResultCb,
        LocAgpsCloseResultCb closeResultCb, void* userDataPtr) {

    ENTRY_LOG();

    /* Save callbacks and userDataPtr */
    LocNetIfaceAgps::sAgpsOpenResultCb = openResultCb;
    LocNetIfaceAgps::sAgpsCloseResultCb = closeResultCb;
    LocNetIfaceAgps::sUserDataPtr = userDataPtr;

    /* Create LocNetIface instances */
    if (LocNetIfaceAgps::sLocNetIfaceAgps == NULL) {
        LocNetIfaceAgps::sLocNetIfaceAgps = LocNetIfaceBase::getLocNetIfaceImpl();
        LocNetIfaceAgps::sLocNetIfaceAgps->registerWwanCallStatusCallback(
                LocNetIfaceAgps::wwanStatusCallback);
    } else {
        LOC_LOGe("sLocNetIfaceAgps not NULL");
    }

    /* Return our callback */
    LocNetIfaceAgps::sAgpsCbInfo.statusV4Cb = LocNetIfaceAgps::agpsStatusCb;
    LocNetIfaceAgps::sAgpsCbInfo.atlType = AGPS_ATL_TYPE_SUPL | AGPS_ATL_TYPE_SUPL_ES;
    LocNetIfaceAgps::sAgpsCbInfo.cbPriority = AGPS_CB_PRIORITY_HIGH;
    return LocNetIfaceAgps::sAgpsCbInfo;
}

void LocNetIfaceAgps::agpsStatusCb(AGnssExtStatusIpV4 status) {
    uint32_t subId = LOC_PRIMARY_SUB;
    uint32_t ipType = LOC_APN_IP_IPV4;
    string apn;
    string clientName;
    char apnName[LOC_MAX_PARAM_STRING];

    ENTRY_LOG();

    /* Validate */
    if (sLocNetIfaceAgps == NULL) {
        LOC_LOGe("Not init'd");
        return;
    }

    memset(apnName, 0, LOC_MAX_PARAM_STRING);
    LocNetConnType locNetConnType;
    LOC_LOGd("status.type=%d status.subId=%d status.status=%d",
             status.type, status.subId, status.status);

    if (LOC_AGPS_TYPE_SUPL == status.type) {
        LOC_LOGe("REQUEST LOC_AGPS_TYPE_SUPL");
        if (LOC_SECONDARY_SUB == status.subId) {
            loc_param_s_type confItemsToFetchArray[] = {
                { "SUPL_APN_SUB2",     &apnName, NULL, 's' },
                { "SUPL_IP_TYPE_SUB2", &ipType,  NULL, 'n' } };
            UTIL_READ_CONF(LOC_PATH_GPS_CONF, confItemsToFetchArray);
        } else {
            loc_param_s_type confItemsToFetchArray[] = {
                { "SUPL_APN",     &apnName, NULL, 's' },
                { "SUPL_IP_TYPE", &ipType,  NULL, 'n' } };
            UTIL_READ_CONF(LOC_PATH_GPS_CONF, confItemsToFetchArray);
        }
        apn = apnName;
        subId = status.subId;
        clientName = LOCNETIFACE_SUPL_CLIENT;
        locNetConnType = LOC_NET_CONN_TYPE_WWAN_SUPL;
    } else if (LOC_AGPS_TYPE_WWAN_ANY == status.type) {
        LOC_LOGd("REQUEST LOC_AGPS_TYPE_WWAN_ANY");
        loc_param_s_type confItemsToFetchArray[] = {
            { "INTERNET_APN",     &apnName, NULL, 's' },
            { "INTERNET_IP_TYPE", &ipType,  NULL, 'n' },
            { "INTERNET_SUB_ID",  &subId,   NULL, 'n' }};
        UTIL_READ_CONF(LOC_PATH_GPS_CONF, confItemsToFetchArray);
        apn = apnName;
        subId = status.subId;
        clientName = LOCNETIFACE_AGPS_CLIENT;
        locNetConnType = LOC_NET_CONN_TYPE_WWAN_INTERNET;
    } else if (LOC_AGPS_TYPE_SUPL_ES == status.type) {
        LOC_LOGd("REQUEST LOC_AGPS_TYPE_SUPL_ES");
        apn = string("");
        ipType = LOC_APN_IP_IPV4V6;
        subId = status.subId;
        clientName = LOCNETIFACE_SUPL_ES_CLIENT;
        locNetConnType = LOC_NET_CONN_TYPE_WWAN_EMERGENCY;
    } else {
        LOC_LOGe("Unsupported AGPS type %d", status.type);
        return;
    }

    LOC_LOGd("clientName=%s subId=%d apn=%s ipType=%d locNetConnType=%d",
             clientName.c_str(), subId, apn.c_str(), ipType, locNetConnType);

    if (status.status == LOC_GPS_REQUEST_AGPS_DATA_CONN) {
        sAgpsState = LOC_NET_AGPS_STATE_OPEN_PENDING;
        if (!sLocNetIfaceAgps->connectBackhaul(clientName, false, subId, apn, ipType)) {
            LOC_LOGe("Connect Backhaul for SUPL failed");
            wwanStatusCallback(LOC_NET_WWAN_CALL_EVT_OPEN_FAILED, locNetConnType,
                    apn.c_str(), ipType);
            sAgpsState = LOC_NET_AGPS_STATE_CLOSE_PENDING;
            // remove supl-client from connection list
            sLocNetIfaceAgps->disconnectBackhaul(clientName, false, subId, apn, ipType);
            sAgpsState = LOC_NET_AGPS_STATE_CLOSED;
        }
    }
    else if (status.status == LOC_GPS_RELEASE_AGPS_DATA_CONN) {
        sAgpsState = LOC_NET_AGPS_STATE_CLOSE_PENDING;
        if (!sLocNetIfaceAgps->disconnectBackhaul(clientName, false, subId, apn, ipType)) {
            LOC_LOGe("Disconnect backhaul failed !");
            wwanStatusCallback(LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED, locNetConnType,
                    apn.c_str(), ipType);
            sAgpsState = LOC_NET_AGPS_STATE_CLOSED;
        }
    }
}

void LocNetIfaceAgps::wwanStatusCallback(LocNetWwanCallEvent event,
                                         LocNetConnType locNetConnType,
                                         const char* apn,
                                         LocApnIpType apnIpType) {

    ENTRY_LOG();
    LOC_LOGd("event: %d, locNetConnType: %d apn: %s apnIpType: %d",
             event, locNetConnType, apn, apnIpType);

    /* Derive bearer type */
    AGpsBearerType bearerType = AGPS_APN_BEARER_INVALID;
    switch (apnIpType) {
        case LOC_APN_IP_IPV4:
            bearerType = AGPS_APN_BEARER_IPV4;
            break;
        case LOC_APN_IP_IPV6:
            bearerType = AGPS_APN_BEARER_IPV6;
            break;
        case LOC_APN_IP_IPV4V6:
            bearerType = AGPS_APN_BEARER_IPV4V6;
            break;
        default:
            LOC_LOGe("Invalid APN IP type %d", apnIpType);
    }

    /* Derive AGPS type */
    AGpsExtType agpsType = LOC_AGPS_TYPE_INVALID;
    string clientName;
    switch (locNetConnType) {
        case LOC_NET_CONN_TYPE_WWAN_SUPL:
            agpsType = LOC_AGPS_TYPE_SUPL;
            clientName = LOCNETIFACE_SUPL_CLIENT;
            break;
        case LOC_NET_CONN_TYPE_WWAN_INTERNET:
            agpsType = LOC_AGPS_TYPE_WWAN_ANY;
            clientName = LOCNETIFACE_AGPS_CLIENT;
            break;
        case LOC_NET_CONN_TYPE_WWAN_EMERGENCY:
            agpsType = LOC_AGPS_TYPE_SUPL_ES;
            clientName = LOCNETIFACE_SUPL_ES_CLIENT;
            break;
        default:
            LOC_LOGe("Invalid locNetConnType %d", locNetConnType);
    }

    /* Complete AGPS call flow */
    if (event == LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS &&
        sAgpsState == LOC_NET_AGPS_STATE_OPEN_PENDING) {
        LOC_LOGd("LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS agpsType=%d bearerType=%d",
                 agpsType, bearerType);
        sAgpsOpenResultCb(true, agpsType, apn, bearerType, sUserDataPtr);
        sAgpsState = LOC_NET_AGPS_STATE_OPENED;
    }
    else if (event == LOC_NET_WWAN_CALL_EVT_OPEN_FAILED &&
        sAgpsState == LOC_NET_AGPS_STATE_OPEN_PENDING) {
        LOC_LOGe("LOC_NET_WWAN_CALL_EVT_OPEN_FAILED agpsType=%d bearerType=%d",
                 agpsType, bearerType);
        // send request to disconnect first and then send open result CB
        string apnName = apn;
        if (!sLocNetIfaceAgps->disconnectBackhaul(clientName, false, LOC_DEFAULT_SUB, apnName,
                apnIpType)) {
            LOC_LOGe("Disconnect backhaul failed !");
        }
        sAgpsOpenResultCb(false, agpsType, apn, bearerType, sUserDataPtr);
        sAgpsState = LOC_NET_AGPS_STATE_CLOSED;
    }
    else if (event == LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS &&
        sAgpsState == LOC_NET_AGPS_STATE_CLOSE_PENDING) {
        LOC_LOGd("LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS agpsType=%d", agpsType);
        sAgpsCloseResultCb(true, agpsType, sUserDataPtr);
        sAgpsState = LOC_NET_AGPS_STATE_CLOSED;
    }
    else if (event == LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED &&
        sAgpsState == LOC_NET_AGPS_STATE_CLOSE_PENDING) {
        LOC_LOGe("LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED agpsType=%d", agpsType);
        sAgpsCloseResultCb(false, agpsType, sUserDataPtr);
        sAgpsState = LOC_NET_AGPS_STATE_CLOSED;
    }
    else {
        LOC_LOGe("Unsupported event %d, type %d, state %d",
                 event, agpsType, sAgpsState);
        // Ignore unsupported cases.
    }
}
