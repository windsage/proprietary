/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved
=============================================================================*/
#include <thread>
#include "LocNetIface.h"
#include <QCMAP_Client.h>
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include <loc_pla.h>
#include <loc_cfg.h>
#include <log_util.h>
#include <unistd.h>

#define LOG_TAG "LocSvc_LocNetIface"

using namespace loc_core;

#define CONNECT_BACKHAUL_RETRY_COUNT (20)
#define DELAY_BW_CONNECT_RETRIES_MICROSECS (500000)

#if ((QCMAP_MSGR_V01_IDL_MAJOR_VERS == (0x01)) && (QCMAP_MSGR_V01_IDL_MINOR_VERS <= (0x51)))
    // qcmap_msgr_subscription_enum_v01 was introduced with MINOR_VERS 0x52
    typedef enum {
        /**< To force a 32 bit signed enum.  Do not change or use*/
        QCMAP_MSGR_SUBSCRIPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647,
        QCMAP_MSGR_DEFAULT_SUBS_V01 = 0, /**<  Default Subscription \n  */
        QCMAP_MSGR_PRIMARY_SUBS_V01 = 1, /**<  Primary Subscription \n  */
        QCMAP_MSGR_SECONDARY_SUBS_V01 = 2, /**<  Secondary Subscription \n  */
        /**< To force a 32 bit signed enum.  Do not change or use*/
        QCMAP_MSGR_SUBSCRIPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647
    }qcmap_msgr_subscription_enum_v01;
#endif

void LocNetIface::subscribe(
        const std::unordered_set<DataItemId>& itemSetToSubscribe) {

    ENTRY_LOG();

    /* Add items to subscribed unordered_set */
    bool anyUpdatesToSubscriptionList =
            updateSubscribedItemSet(itemSetToSubscribe, true);

    // Initialize QCMAP instance
    subscribeWithQcmap();

    /* If either of network info items is in subscription unordered_set,
     * subscribe with QCMAP */
    if (anyUpdatesToSubscriptionList) {
        if (isItemSubscribed(NETWORKINFO_DATA_ITEM_ID)) {
            notifyCurrentNetworkInfo(true);
        }
        if (isItemSubscribed(WIFIHARDWARESTATE_DATA_ITEM_ID)) {
            notifyCurrentWifiHardwareState(true);
        }
    }

    loc_param_s_type setRoamingTable[] = {
        { "SET_ROAMING", &mSetRoaming,  NULL, 'n' } };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, setRoamingTable);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void LocNetIface::unsubscribe(
        const std::unordered_set<DataItemId>& itemListToUnsubscribe) {

    ENTRY_LOG();

    /* Remove items from subscribed list */
    updateSubscribedItemSet(itemListToUnsubscribe, false);

    unsubscribeWithQcmap();
}

void LocNetIface::unsubscribeAll() {

    ENTRY_LOG();

    unsubscribeWithQcmap();

    /* Clear subscription list */
    mSubscribedItemList.clear();
}

void LocNetIface::requestData(
        const std::unordered_set<DataItemId>& itemListToRequestData) {

    ENTRY_LOG();

    /* NO-OP for LE platform
     * We don't support any data item to fetch data for */
}

void qcmapClientCallback(qmi_client_type user_handle, /**< QMI user handle. */
    unsigned int msg_id, /**< Indicator message ID. */
    void *ind_buf, /**< Raw indication data. */
    unsigned int ind_buf_len, /**< Raw data length. */
    void *ind_cb_data /**< User callback handle. */) {
    LocNetIface* pLocNetIFace;

    pLocNetIFace = (LocNetIface* )LocNetIfaceBase::getLocNetIfaceImpl();
    if (nullptr == pLocNetIFace) {
        LOC_LOGe("pLocNetIFace is nullptr!");
        return;
    }

    pLocNetIFace->qcmapClientCallbackInternal(user_handle, msg_id,
                                              ind_buf, ind_buf_len, ind_cb_data);
}

void LocNetIface::subscribeWithQcmap() {

    ENTRY_LOG();

    qmi_error_type_v01 qcmapErr = QMI_ERR_NONE_V01;

    /* Are we already subscribed */
    if (mQcmapClientPtr != NULL) {
        LOC_LOGw("Already subscribed !");
        return;
    }

    /* Create a QCMAP Client instance */
    mQcmapClientPtr = new QCMAP_Client(qcmapClientCallback);
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("Failed to allocate QCMAP instance !");
        return;
    }
    LOC_LOGd("Created QCMAP_Client instance %p", mQcmapClientPtr);

    /* Cache Internet APN, IPType and SubId configuration */
    char apnName[LOC_MAX_PARAM_STRING];
    memset(apnName, 0, LOC_MAX_PARAM_STRING);
    loc_param_s_type confItemsToFetchArray[] = {
        { "INTERNET_APN",     &apnName, NULL, 's' },
        { "INTERNET_IP_TYPE", &mInternetIpType,  NULL, 'n' },
        { "INTERNET_SUB_ID",  &mInternetSubId,   NULL, 'n' },
        { "CID_DEFAULT_PROFILE", &mCidDefaultProfile,   NULL, 'n' }};
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, confItemsToFetchArray);
    mInternetApnName = apnName;

    LOC_LOGi("Internet config - subId=%d apn=%s ipType=%d mCidDefaultProfile=%d",
            mInternetSubId, mInternetApnName.c_str(), mInternetIpType, mCidDefaultProfile);

    // If RegisterForIndications api is available, we need not call EnableMobileAP in
    // constructor and/or bootup, as it is required only to be invoked before initiating
    // a data call (before ConnectBackhaul). We should not unnecessarily EnableMobileAP
    // at bootup.
    /* Need to RegisterForIndications to get station mode status indications */
    uint64_t reg_mask = WWAN_ROAMING_STATUS_IND|BACKHAUL_STATUS_IND|WWAN_STATUS_IND| \
            MOBILE_AP_STATUS_IND|STATION_MODE_STATUS_IND|CRADLE_MODE_STATUS_IND| \
            ETHERNET_MODE_STATUS_IND|BT_TETHERING_STATUS_IND|BT_TETHERING_WAN_IND| \
            WLAN_STATUS_IND|PACKET_STATS_STATUS_IND;
#ifdef QMI_QCMAP_MSGR_MODEM_STATUS_IND_V01
    reg_mask |= MODEM_STATUS_IND;
#endif
    bool ret  = false;
    //Register with QCMAP for any BACKHAUL/network availability
    ret = mQcmapClientPtr->RegisterForIndications(&qcmapErr, reg_mask);
    LOC_LOGi("RegisterForIndications - qmi_error %d status %d\n", qcmapErr, ret);
    if (QMI_ERR_NONE_V01 != qcmapErr)
    {
        LOC_LOGe("Backhaul registration failed error value: %d", qcmapErr);
    }
}

void LocNetIface::unsubscribeWithQcmap() {

    ENTRY_LOG();

    // Simply deleting the qcmap client instance is enough
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance to unsubscribe from");
        return;
    }

    delete mQcmapClientPtr;
    mQcmapClientPtr = NULL;
}

void LocNetIface::qcmapClientCallbackInternal(
        qmi_client_type user_handle, /**< QMI user handle. */
        unsigned int msg_id, /**< Indicator message ID. */
        void *ind_buf, /**< Raw indication data. */
        unsigned int ind_buf_len, /**< Raw data length. */
        void *ind_cb_data /**< User callback handle. */ ) {

    ENTRY_LOG();

    qmi_client_error_type qmi_error;

    // Check the message type
    // msg_id  = QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01
    // ind_buf = qcmap_msgr_wlan_status_ind_msg_v01
    switch (msg_id) {

    case QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01: {
        LOC_LOGD("Received QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01");

        qcmap_msgr_wlan_status_ind_msg_v01 wlanStatusIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &wlanStatusIndData,
                sizeof(qcmap_msgr_wlan_status_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        handleQcmapCallbackWlanStatusInd(wlanStatusIndData);
        break;
    }

    case QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01: {
        LOC_LOGD("Received QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01");

        qcmap_msgr_bring_up_wwan_ind_msg_v01 bringUpWwanIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &bringUpWwanIndData,
                sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        handleQcmapCallbackBringupWwanInd(bringUpWwanIndData);
        break;
    }

    case QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01: {
        LOC_LOGD("Received QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01");

        qcmap_msgr_tear_down_wwan_ind_msg_v01 teardownWwanIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &teardownWwanIndData,
                sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        handleQcmapCallbackTeardownInd(teardownWwanIndData);
        break;
    }

    case QMI_QCMAP_MSGR_BACKHAUL_STATUS_IND_V01:
    {
        qcmap_msgr_backhaul_status_ind_msg_v01 backhaulStatusData;

        qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &backhaulStatusData,
                           sizeof(qcmap_msgr_backhaul_status_ind_msg_v01));
        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        handleQcmapCallbackBackhaulStatusInd(backhaulStatusData);
        break;
     }

    case QMI_QCMAP_MSGR_WWAN_ROAMING_STATUS_IND_V01:
    {
        qcmap_msgr_wwan_roaming_status_ind_msg_v01 roamingStatusData;

        qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &roamingStatusData,
                           sizeof(qcmap_msgr_wwan_roaming_status_ind_msg_v01));
        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        handleQcmapCallbackRoamStatusInd(roamingStatusData);
        break;
    }

    case QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01: {
        LOC_LOGd("Received QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01");

        qcmap_msgr_wwan_status_ind_msg_v01 wwanStatusIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &wwanStatusIndData,
                sizeof(qcmap_msgr_wwan_status_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        handleQcmapCallbackWwanStatusInd(wwanStatusIndData);
        break;
    }

#ifdef QMI_QCMAP_MSGR_MODEM_STATUS_IND_V01
    case QMI_QCMAP_MSGR_MODEM_STATUS_IND_V01: {
        LOC_LOGd("Received QMI_QCMAP_MSGR_MODEM_STATUS_IND_V01");

        qcmap_msgr_modem_status_ind_msg_v01 modemStatInd;
        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &modemStatInd, sizeof(modemStatInd));
        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }
        LOC_LOGd("MODEM_STATUS_IND Srv status: %d", modemStatInd.service_status);
        break;
    }
#endif

    default:
        LOC_LOGE("Ignoring QCMAP indication: %d", msg_id);
    }
}

void LocNetIface::handleQcmapCallbackWlanStatusInd(
        qcmap_msgr_wlan_status_ind_msg_v01 &wlanStatusIndData) {

    ENTRY_LOG();

    LOC_LOGD("WLAN Status (enabled=1, disabled=2): %d",
            wlanStatusIndData.wlan_status);

    LOC_LOGD("WLAN Mode (AP=1, ... STA=6): %d",
            wlanStatusIndData.wlan_mode);

    /* Notify observers */
    if (wlanStatusIndData.wlan_status == QCMAP_MSGR_WLAN_ENABLED_V01) {
        mLocNetWlanState = LOC_NET_CONN_STATE_ENABLED;
        notifyObserverForWlanStatus(true);
    } else if (wlanStatusIndData.wlan_status == QCMAP_MSGR_WLAN_DISABLED_V01) {
        mLocNetWlanState = LOC_NET_CONN_STATE_DISABLED;
        notifyObserverForWlanStatus(false);
    } else {
        LOC_LOGE("Invalid wlan status %d", wlanStatusIndData.wlan_status);
    }
}

void LocNetIface::handleQcmapCallbackBackhaulStatusInd(
            qcmap_msgr_backhaul_status_ind_msg_v01 &backhaulStatusIndData){
    ENTRY_LOG();

    if (true == backhaulStatusIndData.backhaul_type_valid)
    {
        boolean isIpv4Avail = ((backhaulStatusIndData.backhaul_v4_status_valid
                    && backhaulStatusIndData.backhaul_v4_status));
        boolean isIpv6Avail = ((backhaulStatusIndData.backhaul_v6_status_valid
                    && backhaulStatusIndData.backhaul_v6_status));
        setCurrentBackHaulStatus(backhaulStatusIndData.backhaul_type,
                                 isIpv4Avail, isIpv6Avail);
        notifyCurrentNetworkInfo(false);
    }
    else {
        LOC_LOGE("Backhaul type is not valid : %d", backhaulStatusIndData.backhaul_type_valid);
        mLocNetBackHaulState = LOC_NET_CONN_STATE_INVALID;
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_INVALID;
    }
}

void LocNetIface::handleQcmapCallbackWwanStatusInd(
        qcmap_msgr_wwan_status_ind_msg_v01 &wwanStatusIndData) {
    ENTRY_LOG();

    uint32_t profileHdl = 0;
    uint32_t subsId = 0;
    bool isIpv6 = false;
    bool isIpv4 = false;
    profileHdl = (TRUE == wwanStatusIndData.profile_handle_valid) ?
            (uint32_t)wwanStatusIndData.profile_handle: 1;
#ifdef FEATURE_DSDA_ENABLED
    subsId  = (TRUE == wwanStatusIndData.subs_id_valid) ?
            (uint32_t)wwanStatusIndData.subs_id: 0xFFFF;
#endif

    if ((QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 == wwanStatusIndData.wwan_status) ||
            (QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01 == wwanStatusIndData.wwan_status)) {
        if (QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01 == wwanStatusIndData.wwan_status) {
            isIpv6 = true;
        } else {
            isIpv4 = true;
        }
        LOC_LOGd("WWAN Status (Connected_v4,v6=3,9): %d, isIpv6: %d, isIpv4: %d",
                wwanStatusIndData.wwan_status, isIpv6, isIpv4);
    } else {
        LOC_LOGd("WWAN Status (Connecting_v4,v6=1,7) (ConnectingFail_v4,v6=2,8)");
        LOC_LOGd("WWAN Status (Disonnecting_v4,v6=4,10) (DisonnectingFail_v4,v6=5,11)");
        LOC_LOGd("WWAN Status (Disonnected_v4,v6=6,12) (Reconfig_v4,v6=13,14)");
        LOC_LOGd("WWAN Status: %d", wwanStatusIndData.wwan_status);
        LOC_LOGd("wwan_status_ind_msg_v01: PDN=%d (Subs_Id=%d) WAN CallendType=%d CallendCode=%d",
                profileHdl, subsId,
                wwanStatusIndData.wwan_call_end_reason.wwan_call_end_reason_type,
                wwanStatusIndData.wwan_call_end_reason.wwan_call_end_reason_code);
    }
}

void LocNetIface::handleQcmapCallbackBringupWwanInd (
        qcmap_msgr_bring_up_wwan_ind_msg_v01 &bringUpWwanIndData) {

    ENTRY_LOG();

    uint32_t profileHdl;
    uint32_t subsId = 0;
    profileHdl = (bringUpWwanIndData.profile_handle_valid) ?
            (uint32_t)bringUpWwanIndData.profile_handle: 0xFFFF;
#ifdef FEATURE_DSDA_ENABLED
    subsId  = (bringUpWwanIndData.subs_id_valid) ? (uint32_t)bringUpWwanIndData.subs_id: 0xFFFF;
#endif
    LOC_LOGd("WWAN Bringup MobileApHdl:%d, ProfileHdl:%d, Sub Id: %d",
            bringUpWwanIndData.mobile_ap_handle, profileHdl, subsId);
    LOC_LOGd("WWAN Bringup status (Connected_v4,v6=3,9, connecting fail_v4,v6=2,8): %d",
            bringUpWwanIndData.conn_status);

    /* In case ipType is LOC_APN_IP_IPV4V6 this will be called twice per connection,
       we need to report as follows:
       1. First call is SUCCESS = report this, ignore the second
       2. First call is FAILURE = do not report, but wait for the second one and report
          the status indicated by that (SUCCESS or FAILURE) */
    /* Notify observers */
    if (bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 ||
            bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01) {
        // We update state and type in backhaul status CB only
        if (mWwanCallStatusCb != NULL && (mSuplClientCnt > 0)) {
            // Send wwan status callback to HAL for SUPL calls.
            LOC_LOGd("LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS");
            // If we get success for IPv4v6 for first callback, ignore the second CB.
            if (LOC_APN_IP_IPV4V6 == getSuplIpType()) {
                LOC_LOGd("IPV4V6 Current Connect Cb State: %d", mWwanSuplConnectCbState);
                if (LOC_NET_WWAN_SUPL_CB_STATE_DEFAULT == mWwanSuplConnectCbState) {
                    mWwanSuplConnectCbState = LOC_NET_WWAN_SUPL_CB_STATE_FIRST_CB_SUCCESS;
                } else if (LOC_NET_WWAN_SUPL_CB_STATE_FIRST_CB_SUCCESS == mWwanSuplConnectCbState) {
                    LOC_LOGd("Connect: Already got success callback for IPv4v6. So ignoring.");
                    return;
                }
            }
            mWwanCallStatusCb(LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS, LOC_NET_CONN_TYPE_WWAN_SUPL,
                    getSuplApnName(), getSuplIpType());
        }
      } else if (bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
               bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01) {

        if (mWwanCallStatusCb != NULL) {
            LOC_LOGd("LOC_NET_WWAN_CALL_EVT_OPEN_FAILED");
            if (LOC_APN_IP_IPV4V6 == getSuplIpType()) {
                LOC_LOGd("IPV4V6 Current Connect Cb State: %d", mWwanSuplConnectCbState);
                if (LOC_NET_WWAN_SUPL_CB_STATE_DEFAULT == mWwanSuplConnectCbState) {
                    mWwanSuplConnectCbState = LOC_NET_WWAN_SUPL_CB_STATE_FIRST_CB_FAILURE;
                    LOC_LOGd("Connect: First CB is failure. Waiting for the second callback");
                    return;
                }
            }
            if (mSuplClientCnt > 0) {
                // Send wwan status callback to HAL for SUPL calls.
                mWwanCallStatusCb(LOC_NET_WWAN_CALL_EVT_OPEN_FAILED, LOC_NET_CONN_TYPE_WWAN_SUPL,
                        getSuplApnName(), getSuplIpType());
            }
        }
    } else {
        LOC_LOGw("Unsupported wwan status %d",
                bringUpWwanIndData.conn_status);
    }
    // Clear disconnect map upon bringup.
    uint32_t numBackHaulClients = mBackHaulDisconnReqCache.size();
    if (numBackHaulClients > 0) {
        // Invoke inside a thread, as GetWwanStatus qcmap api might block and therefore
        // will cause delay in qcmap notifications.
        std::thread t([this] {
            // For each client, invoke disconnectbackhaul.
            for (auto clientContext : mBackHaulDisconnReqCache) {
                LOC_LOGd("Invoke disconnectBackhaul for client: %s Sub: %d Apn: %s IpType: %d",
                        clientContext.second.clientName.c_str(), clientContext.second.prefSub,
                        clientContext.second.prefApn.c_str(), clientContext.second.prefIpType);
                disconnectBackhaul(clientContext.second.clientName, false,
                        (LocSubId)clientContext.second.prefSub, clientContext.second.prefApn,
                        (LocApnIpType)clientContext.second.prefIpType);
            }
            // Clear the map
            mBackHaulDisconnReqCache.clear();
        });
        t.detach();
    }
}

void LocNetIface::handleQcmapCallbackTeardownInd(
        qcmap_msgr_tear_down_wwan_ind_msg_v01 &teardownWwanIndData) {

    ENTRY_LOG();

    uint32_t profileHdl;
    uint32_t subsId = 0;
    profileHdl = (teardownWwanIndData.profile_handle_valid) ?
            (uint32_t)teardownWwanIndData.profile_handle: 0xFFFF;
#ifdef FEATURE_DSDA_ENABLED
    subsId  = (teardownWwanIndData.subs_id_valid) ? (uint32_t)teardownWwanIndData.subs_id: 0xFFFF;
#endif
    LOC_LOGd("WWAN teardown MobileApHdl:%d, ProfileHdl:%d, Sub Id: %d",
            teardownWwanIndData.mobile_ap_handle, profileHdl, subsId);
    LOC_LOGd("WWAN teardown status (Disconnected_v4,v6=6,12) (Disconnecting fail_v4,v6=5,11): %d",
            teardownWwanIndData.conn_status);

    /* In case ipType is LOC_APN_IP_IPV4V6 this will be called twice per connection,
       we need to report as follows:
       1. First call is SUCCESS = report this, ignore the second
       2. First call is FAILURE = do not report, but wait for the second one and report
       the status indicated by that (SUCCESS or FAILURE) */
    /* Notify observers */
    if (teardownWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
        teardownWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01) {
        //We update state and type in backhaul status CB only
        if (mWwanCallStatusCb != NULL) {
            // If we get success for IPv4v6 for first callback, ignore the second CB.
            if (LOC_APN_IP_IPV4V6 == getSuplIpType()) {
                LOC_LOGd("IPV4V6 Current Disconnect Cb State: %d", mWwanSuplDiscnctCbState);
                if (LOC_NET_WWAN_SUPL_CB_STATE_DEFAULT == mWwanSuplDiscnctCbState) {
                    mWwanSuplDiscnctCbState = LOC_NET_WWAN_SUPL_CB_STATE_FIRST_CB_SUCCESS;
                } else if (LOC_NET_WWAN_SUPL_CB_STATE_FIRST_CB_SUCCESS == mWwanSuplDiscnctCbState) {
                    LOC_LOGd("Disconnect: Already got success callback for IPv4v6. So ignoring.");
                    return;
                }
            }
        }
    } else if (teardownWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01 ||
                    teardownWwanIndData.conn_status ==
                        QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01) {
        if (mWwanCallStatusCb != NULL) {
            if (LOC_APN_IP_IPV4V6 == getSuplIpType()) {
                LOC_LOGd("IPV4V6 Current Disconnect Cb State: %d", mWwanSuplDiscnctCbState);
                if (LOC_NET_WWAN_SUPL_CB_STATE_DEFAULT == mWwanSuplDiscnctCbState) {
                    mWwanSuplDiscnctCbState = LOC_NET_WWAN_SUPL_CB_STATE_FIRST_CB_FAILURE;
                    LOC_LOGd("Disconnect: First CB is failure. Waiting for the second callback");
                    return;
                }
            }
        }
    } else {
        LOC_LOGw("Unsupported wwan status %d", teardownWwanIndData.conn_status);
    }
    // Clear connect map upon diconnect teardown.
    uint32_t numBackHaulClients = mBackHaulConnReqCache.size();
    if (numBackHaulClients > 0) {
        // Invoke inside a thread, as GetWwanStatus qcmap api might block and therefore
        // will cause delay in qcmap notifications.
        std::thread t([this] {
            // For each client, invoke connectbackhaul.
            for (auto clientContext : mBackHaulConnReqCache) {
                LOC_LOGd("Invoke ConnectBackhaul for client: %s Sub: %d Apn: %s IpType: %d",
                        clientContext.second.clientName.c_str(), clientContext.second.prefSub,
                        clientContext.second.prefApn.c_str(), clientContext.second.prefIpType);
                connectBackhaul(clientContext.second.clientName, false,
                        (LocSubId)clientContext.second.prefSub, clientContext.second.prefApn,
                        (LocApnIpType)clientContext.second.prefIpType);
            }
            // Clear the map
            mBackHaulConnReqCache.clear();
        });
        t.detach();
    }
}

void LocNetIface::handleQcmapCallbackRoamStatusInd(
        qcmap_msgr_wwan_roaming_status_ind_msg_v01 &roamingStatusIndData) {

    ENTRY_LOG();

    mIsRoaming = (roamingStatusIndData.wwan_roaming_status != 0);
    LOC_LOGd("Roaming status(OFF:0x00, ON:0x01-0x0C): %x, Roaming: %d",
             roamingStatusIndData.wwan_roaming_status, mIsRoaming);
}

void LocNetIface::notifyCurrentNetworkInfo(bool queryQcmap, LocNetConnType connType) {

    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance !");
        return;
    }

    /* Check saved state if queryQcmap disabled */
    if (!queryQcmap) {
        if (LOC_NET_CONN_TYPE_INVALID != mLocNetBackHaulType) {
            LOC_LOGd("notifyObserverForNetworkInfo backhaultype :%d", mLocNetBackHaulType);
            notifyObserverForNetworkInfo(
                    (LOC_NET_CONN_STATE_CONNECTED == mLocNetBackHaulState),
                    mIsRoaming,
                    mLocNetBackHaulType);
        }
        else {
            LOC_LOGe("Invalid connection type:%d , State:%d",
                     mLocNetBackHaulType, mLocNetBackHaulState);
        }
        return;
    }

    /* Fetch connectivity status from qcmap and notify observers */
    /* Check if any network interface backhaul is connected */
    isAnyBackHaulConnected();
    if (LOC_NET_CONN_TYPE_WWAN_INTERNET == mLocNetBackHaulType) {
        /* Check the roaming status if backhaul type is WWAN */
        mIsRoaming = isWwanRoaming();
    }
    if (LOC_NET_CONN_TYPE_INVALID != mLocNetBackHaulType) {
        notifyObserverForNetworkInfo(
                (LOC_NET_CONN_STATE_CONNECTED == mLocNetBackHaulState),
                mIsRoaming,
                mLocNetBackHaulType);
    }
}

void LocNetIface::notifyCurrentWifiHardwareState(bool queryQcmap) {

    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance !");
        return;
    }

    /* Check saved state if queryQcmap disabled */
    if (!queryQcmap) {
        notifyObserverForWlanStatus((LOC_NET_CONN_STATE_ENABLED == mLocNetWlanState));
        return;
    }

    /* Fetch WLAN status */
    qcmap_msgr_wlan_mode_enum_v01 wlan_mode =
            QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;

    if (!mQcmapClientPtr->GetWLANStatus(&wlan_mode, &qmi_err_num)) {
        LOC_LOGe("Failed to fetch wlan status, err %d", qmi_err_num);
        return;
    }

    if (wlan_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01) {
        mLocNetWlanState = LOC_NET_CONN_STATE_DISABLED;
        notifyObserverForWlanStatus(false);
    } else if (wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
               wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
               wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
               wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01 ||
               wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_BRIDGE_V01 ||
               wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_BRIDGE_V01) {
        mLocNetWlanState =LOC_NET_CONN_STATE_ENABLED;
        notifyObserverForWlanStatus(true);
    }
}

bool LocNetIface::isNonMeteredBackHaulTypeConnected() {
    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance !");
        return false;
    }

    /* Update backhaul status */
    isAnyBackHaulConnected();
    /* if Current backhaul - Is not WWAN && Is not an Invalid type*/
    return ((LOC_NET_CONN_TYPE_WWAN_INTERNET != mLocNetBackHaulType) &&
            (LOC_NET_CONN_TYPE_INVALID != mLocNetBackHaulType));
}

bool LocNetIface::isWwanRoaming() {
    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance !");
        return false;
    }

    /* fetch roaming status */
    uint8_t roamStatus = 0;
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;
    if (!mQcmapClientPtr->GetWWANRoamStatus(&roamStatus, &qmi_err_num)) {
        LOC_LOGe("Failed to fetch roaming status, err %d", qmi_err_num);
        return false;
    }
    // update internal roaming variable
    LOC_LOGd("Roaming status(OFF:0x00, ON:0x01-0x0C): %x", roamStatus);
    return (roamStatus != 0);
}

bool LocNetIface::isAnyBackHaulConnected() {

    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance !");
        return false;
    }

    /* Fetch backhaul status */
    qcmap_backhaul_status_info_type backhaulStatus =
            {false, false, QCMAP_MSGR_BACKHAUL_TYPE_ENUM_MIN_ENUM_VAL_V01};
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;

    if (!mQcmapClientPtr->GetBackhaulStatus(&backhaulStatus, &qmi_err_num)) {
        LOC_LOGe("Failed to fetch backhaul status, err %d", qmi_err_num);
        return false;
    }

    setCurrentBackHaulStatus(backhaulStatus.backhaul_type,
                             backhaulStatus.backhaul_v4_available,
                             backhaulStatus.backhaul_v6_available);
    return (LOC_NET_CONN_STATE_CONNECTED == mLocNetBackHaulState);
}

void LocNetIface::setCurrentBackHaulStatus(
                qcmap_msgr_backhaul_type_enum_v01 backhaulType,
                boolean backhaulIPv4Available,
                boolean backhaulIPv6Available) {
    LOC_LOGi("Type:  1-WWAN, 2-USB Cradle, 3-WLAN , 4-Ethernet, 5-BT");
    LOC_LOGi("BackhaulStatus Type: %d, IPv4 avail:%d, IPv6 avail:%d",
             backhaulType, backhaulIPv4Available, backhaulIPv6Available);
    switch (backhaulType)
    {
      case QCMAP_MSGR_WWAN_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_WWAN_INTERNET;
        break;
      case QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_USB_CRADLE;
        break;
      case QCMAP_MSGR_WLAN_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_WLAN;
        break;
      case QCMAP_MSGR_ETHERNET_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_ETHERNET;
        break;
      case QCMAP_MSGR_BT_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_BLUETOOTH;
        break;
      default:
        LOC_LOGE("Invalid backhaul type : %d", backhaulType);
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_INVALID;
        break;
    }
    if (backhaulType != QCMAP_MSGR_WWAN_BACKHAUL_V01) {
        // set this to false for backhaul type other than wwan
        mIsRoaming = false;
    }
    if ((false == backhaulIPv4Available) && (false == backhaulIPv6Available)) {
        mLocNetBackHaulState = LOC_NET_CONN_STATE_DISCONNECTED;
    }
    else {
        mLocNetBackHaulState = LOC_NET_CONN_STATE_CONNECTED;
    }
}

/* isWwanConnected is used mainly from external clients (eg:XtraClient) */
bool LocNetIface::isWwanConnected() {

    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance !");
        return false;
    }

    /* Fetch backhaul status */
    qcmap_backhaul_status_info_type backhaulStatus =
            {false, false, QCMAP_MSGR_BACKHAUL_TYPE_ENUM_MIN_ENUM_VAL_V01};
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;

    if (!mQcmapClientPtr->GetBackhaulStatus(&backhaulStatus, &qmi_err_num)) {
        LOC_LOGe("Failed to fetch backhaul status, err %d", qmi_err_num);
        return false;
    }

    if ((QCMAP_MSGR_WWAN_BACKHAUL_V01 == backhaulStatus.backhaul_type) &&
            (backhaulStatus.backhaul_v4_available || backhaulStatus.backhaul_v6_available)) {
        // If WWAN is current backhaul type and either IPv4 or IPv6 connection available ?
        LOC_LOGd("WWAN is connected.");
        return true;
    } else {
        LOC_LOGd("WWAN is disconnected.");
        return false;
    }

    return false;
}

#ifndef NO_MODEM_TARGET
#ifdef FEATURE_DSDA_ENABLED
bool LocNetIface::getHandleForSuplEs(qcmap_msgr_subscription_enum_v01 qcmapSubId,
                                    profile_handle_type_v01& profile_handle,
                                    uint8_t esProfileIndex,
                                    bool& bProfileIsInTheList, const char* apnStr)
{
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    bool retValue = true;
    qcmap_wwan_policy_list_info wwan_policy_list = {};

    if (mQcmapClientPtr->GetWWANPolicyListEx(&wwan_policy_list, &qmi_err_num)) {
        if (wwan_policy_list.wwan_policy_valid) {
            for (uint32_t i = 0;
                i < wwan_policy_list.wwan_policy_len && i < QCMAP_MAX_NUM_BACKHAULS_V01;
                i++) {
                if (qcmapSubId == wwan_policy_list.wwan_policy[i].subscription_id &&
                    (esProfileIndex == wwan_policy_list.wwan_policy[i].profile_id_3gpp ||
                    (0 == strncmp(apnStr, wwan_policy_list.wwan_policy[i].apn_name,
                                     QCMAP_MAX_APN_NAME_LEN_V01)))){
                    bProfileIsInTheList = true;
                    profile_handle = wwan_policy_list.wwan_policy[i].profile_handle;
                    LOC_LOGd("GetWWANPolicyListEx returns profile_handle=%d",
                             profile_handle);
                    // save the APN and ipType of the emrgency profile
                    switch (wwan_policy_list.wwan_policy[i].ip_family) {
                    case QCMAP_MSGR_IP_FAMILY_V4_V01:
                        setSuplIpType(LOC_APN_IP_IPV4);
                        break;
                    case QCMAP_MSGR_IP_FAMILY_V6_V01:
                        setSuplIpType(LOC_APN_IP_IPV6);
                        break;
                    case QCMAP_MSGR_IP_FAMILY_V4V6_V01:
                        setSuplIpType(LOC_APN_IP_IPV4V6);
                        break;
                    default:
                        // dont set, we have already set based on iptype config
                        break;
                    }
                    LOC_LOGd("Set ipType=%d, apn=%s", getSuplIpType(), getSuplApnName());
                    break;
                }
            }
        } else {
            LOC_LOGe("wwan_policy_list.wwan_policy_valid is false");
            retValue = false;
        }
    } else {
        LOC_LOGe("Failed to Get WWAN policy. Error 0x%x", qmi_err_num);
        retValue = false;
    }
    return retValue;
}
#endif // FEATURE_DSDA_ENABLED

bool LocNetIface::getProfileHandleForApn(const LocSubId subId, const string& apn,
        bool isSuplEs, uint32_t& profileHandle)
{
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    bool retValue = true;
    /* Do the following before calling mQcmapClientPtr->ConnectBackHaul:
        1. call mQcmapClientPtr->GetWWANPolicyListEx to get profile_handle
        2. call mQcmapClientPtr->CreateWWANPolicyEx to get profile_handle
        3. call mQcmapClientPtr->SetWWANProfileHandlePreference
    */
    // 1 - default profile for 3GPP
    profile_handle_type_v01 profile_handle = 1;
    qcmap_msgr_subscription_enum_v01 qcmapSubId;
    char* apnstr = new char[QCMAP_MAX_APN_NAME_LEN_V01];
    strlcpy(apnstr, apn.c_str(), QCMAP_MAX_APN_NAME_LEN_V01);
    uint8_t esProfileIndex = 0;

    do {
        switch (subId) {
        case LOC_PRIMARY_SUB: qcmapSubId = QCMAP_MSGR_PRIMARY_SUBS_V01; break;
        case LOC_SECONDARY_SUB: qcmapSubId = QCMAP_MSGR_SECONDARY_SUBS_V01; break;
        case LOC_TERTIARY_SUB:
        case LOC_DEFAULT_SUB:
        default: qcmapSubId = QCMAP_MSGR_DEFAULT_SUBS_V01; break;
        }
        if (isSuplEs) {
            string apnName = mLocQmiWds->queryEsProfileIndex(qcmapSubId, esProfileIndex);

            if (apnName.length() == 0) {
                apnName = mLocQmiWds->queryEsProfileIndexLite(qcmapSubId, esProfileIndex);
                // If still apname is null from WDSLite
                if (apnName.length() == 0) {
                    LOC_LOGe("queryEsProfileIndexLite apnName is null");
                }
            }

            string str("");
            if (str.compare(apnName) == 0) {
                LOC_LOGe("SuplEs is true, but apnName is null");
                retValue = false;
                break;
            } else {
                setSuplApnName(const_cast<char*>(apnName.c_str()));
                strlcpy(apnstr, apnName.c_str(), QCMAP_MAX_APN_NAME_LEN_V01);
            }
        }

        LOC_LOGd("qcmapSubId=%d, apnstr=%s isSuplEs=%d esProfileIndex=%d",
                 qcmapSubId, apnstr, isSuplEs, esProfileIndex);

        if (apn.empty() && (qcmapSubId != QCMAP_MSGR_PRIMARY_SUBS_V01) && !isSuplEs) {
            LOC_LOGw("Trying to establish a connection on LOC_PRIMARY_SUB");
            qcmapSubId = QCMAP_MSGR_PRIMARY_SUBS_V01;
        }

#ifdef FEATURE_DSDA_ENABLED
        qcmap_wwan_policy_list_info wwan_policy_list = {};
        qcmap_net_policy_info net_policy = {};
        bool bProfileIsInTheList = false;

        if (!(apn.empty()) || (qcmapSubId != QCMAP_MSGR_PRIMARY_SUBS_V01) || isSuplEs) {
            if (isSuplEs) {
                retValue = getHandleForSuplEs(qcmapSubId,
                                              profile_handle,
                                              esProfileIndex,
                                              bProfileIsInTheList, apnstr);
                if (!retValue) {
                    LOC_LOGe("getHandleForSuplEs fails");
                    break;
                }
            } else {
                if (mQcmapClientPtr->GetWWANPolicyListEx(&wwan_policy_list, &qmi_err_num)) {
                    if (wwan_policy_list.wwan_policy_valid) {
                        for (uint32_t i = 0;
                            i < wwan_policy_list.wwan_policy_len && i < QCMAP_MAX_NUM_BACKHAULS_V01;
                            i++) {
                            if ((qcmapSubId == wwan_policy_list.wwan_policy[i].subscription_id) &&
                                (0 == strncmp(apnstr,
                                    wwan_policy_list.wwan_policy[i].apn_name,
                                    QCMAP_MAX_APN_NAME_LEN_V01))) {
                                bProfileIsInTheList = true;
                                profile_handle = wwan_policy_list.wwan_policy[i].profile_handle;
                                LOC_LOGd("GetWWANPolicyListEx returns profile_handle=%d",
                                         profile_handle);
                                break;
                            }
                        }
                    } else {
                        LOC_LOGe("wwan_policy_list.wwan_policy_valid is false");
                        retValue = false;
                        break;
                    }
                } else {
                    LOC_LOGe("Failed to Get WWAN policy. Error 0x%x", qmi_err_num);
                    retValue = false;
                    break;
                }
            }

            if (!bProfileIsInTheList) {
                memset(&net_policy, 0, sizeof(net_policy));
                net_policy.tech_pref = 0; //ANY
                net_policy.subscription_id = qcmapSubId;
                if (isSuplEs) {
                    net_policy.profile_id_3gpp = esProfileIndex;
                    strlcpy(net_policy.apn_name, getSuplApnName(), QCMAP_MAX_APN_NAME_LEN_V01);
                } else {
                    net_policy.profile_id_3gpp = (uint8_t) mCidDefaultProfile;
                    strlcpy(net_policy.apn_name, apnstr, QCMAP_MAX_APN_NAME_LEN_V01);
                }

                LOC_LOGd("subscription_id=%d, apn_name=%s isSuplEs=%d profile_id_3gpp=%d",
                         net_policy.subscription_id, net_policy.apn_name,
                         isSuplEs, net_policy.profile_id_3gpp);
                if (mQcmapClientPtr->CreateWWANPolicyEx(net_policy,
                                                        &profile_handle, &qmi_err_num)) {
                    LOC_LOGd("Create WWAN policy, profile_handle=%d", profile_handle);
                    if (isSuplEs) {
                        // we need to call again GetWWANPolicyListEx to set apn and ipType
                        retValue = getHandleForSuplEs(qcmapSubId,
                                                      profile_handle,
                                                      esProfileIndex,
                                                      bProfileIsInTheList, apnstr);
                        LOC_LOGd("getHandleForSuplEs returned profile_handle=%d", profile_handle);
                        if (!retValue) {
                            LOC_LOGe("getHandleForSuplEs fails");
                            break;
                        }
                    }
                } else {
                    retValue = false;
                    if (QMI_ERR_NO_EFFECT_V01 == qmi_err_num) {
                        // Not an error
                        LOC_LOGd("CreateWWANPolicyEx ret QMI_ERR_NO_EFFECT_V01. profile_handle=%d",
                                profile_handle);
                        // Get the profile handle from GetWWANPolicyListEx if CreateWWANPolicyEx
                        // is failing.
                        // we need to call again GetWWANPolicyListEx to set apn and ipType
                        retValue = getHandleForSuplEs(qcmapSubId,
                                                      profile_handle,
                                                      mCidDefaultProfile,
                                                      bProfileIsInTheList, apnstr);
                        LOC_LOGd("getHandleForSuplEs returned profile_handle=%d", profile_handle);
                        if (!retValue) {
                            LOC_LOGe("getHandleForSuplEs fails");
                        }
                    } else if (QMI_ERR_NO_FREE_PROFILE_V01 == qmi_err_num) {
                        LOC_LOGe("Max Profiles reached, Error 0x%x", qmi_err_num);
                    } else if (QMI_ERR_INVALID_PROFILE_V01 == qmi_err_num) {
                        LOC_LOGe("Invalid/Duplicate Profile request, Error 0x%x", qmi_err_num);
                    } else {
                        LOC_LOGe("Failed to Create WWAN policy. Error 0x%x", qmi_err_num);
                    }
                    break;
                }
            } else {
                LOC_LOGd("Profile is not in the WWANPolicy list");
            }
        }
#endif
    } while (0);
    profileHandle = (uint32_t)profile_handle;
    delete[] apnstr;
    return retValue;
}
#endif // ifndef NO_MODEM_TARGET

bool LocNetIface::connectBackhaul(const string& clientName,
                                  bool async,
                                  const LocSubId subId,
                                  const string& apn,
                                  const LocApnIpType ipType) {

#ifdef NO_MODEM_TARGET
    LOC_LOGd("This target does not supports modem");
    return true;
#else
    /* QCMAP client instance must have been created.
     * Happens when some client subscribes. */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance");
        return false;
    }

    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    qcmap_msgr_wwan_status_enum_v01 v4_status, v6_status;
    if (mQcmapClientPtr->GetWWANStatus(
        &v4_status, &v6_status, &qmi_err_num) == false) {
        LOC_LOGd("Failed to get wwan status, err 0x%x", qmi_err_num);
    } else {
        LOC_LOGd("v4_status: %d, v6_status: %d", v4_status, v6_status);
        if ((QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01 == v4_status)  ||
                (QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01 == v6_status)) {
            LOC_LOGw("Previous disconnect request is in progress.");
            // add client name to map.
            BackhaulContext ctx;
            ctx.clientName = clientName;
            ctx.prefSub = (uint16_t) subId;
            ctx.prefApn = apn;
            ctx.prefIpType = (uint16_t) ipType;
            LOC_LOGd("Adding client %s to connect req cache", clientName.c_str());
            mBackHaulConnReqCache[ctx.clientName] = ctx;
            return true;
        }
    }

    // Check if request from client is already there.
    // Not expecting multiple request from same client at same time.
    auto mapIter {mClientToProfileHdl.find(clientName)};
    if (mapIter != mClientToProfileHdl.end()) {
        LOC_LOGd("LocNetIfClient:%s already found for ProfileHdl: %d",
                mapIter->first.c_str(), mapIter->second);
        return true;
    }

    LocSubId newSubId = subId;
    LocApnIpType newIpType = ipType;
    string   newApnName = apn;
    if ((0 != clientName.compare(LOCNETIFACE_SUPL_CLIENT)) &&
                (0 != clientName.compare(LOCNETIFACE_SUPL_ES_CLIENT))) {
        // For SUPL data call, correct configurations are provided by
        // LocNetIfaceAgps
        if ((LOC_APN_IP_INVALID == ipType) || (0 == apn.length()) ||
                (subId > LOC_TERTIARY_SUB) ) {
            // For internet WWAN call, if IP type is invalid or null APN string etc
            // then we need to set the default value from config.
            LOC_LOGd("Setting the configs for Internet WWAN call");
            newSubId = (LocSubId)mInternetSubId;
            newIpType = (LocApnIpType)mInternetIpType;
            newApnName = mInternetApnName;
        }
        LOC_LOGd("For clientName=%s connect NewSubId=%d NewApn=%s NewIpType=%d",
             clientName.c_str(), newSubId, newApnName.c_str(), newIpType);
    }

    if (!async) return connectBackhaulInternal(clientName, newSubId, newApnName, newIpType);

    std::thread t([this, clientName, newSubId, newApnName, newIpType] {
            connectBackhaulInternal(clientName, newSubId, newApnName, newIpType);
        });
    t.detach();
    return true;
#endif // #ifndef NO_MODEM_TARGET
}

#ifndef NO_MODEM_TARGET
void LocNetIface::printProfileListInformation() {
    LOC_LOGd("ProfileHdlToBackhaulCtxt: %d, ClientToProfileHdl: %d",
            mProfileHdlToBackhaulCtxt.size(), mClientToProfileHdl.size());

    for (auto iter = mProfileHdlToBackhaulCtxt.begin();
            iter != mProfileHdlToBackhaulCtxt.end(); iter++) {
        LOC_LOGd("LocNetIfClient list for ProfileHdl: %d", iter->first);
        ClientBackhaulRequest& backhaulCtxSet = iter->second;
        ClientBackhaulRequest::const_iterator it = backhaulCtxSet.begin();
        for (; it != backhaulCtxSet.end(); it++) {
            LOC_LOGd("LocNetIfClient: %s", (*it).clientName.c_str());
        }
    }

    LOC_LOGd("LocNetIfClient to Profile Id Information");
    for (auto iter = mClientToProfileHdl.begin();
            iter != mClientToProfileHdl.end(); iter++) {
        LOC_LOGd("LocNetIfClient:%s, ProfileHdl: %d", iter->first.c_str(), iter->second);
    }
}

void LocNetIface::removeClientFromInternalClientCache(string clientName,
        uint32_t& profileHdl, bool& profileHdlCleared) {
    // update supl client count
    if ((0 == clientName.compare(LOCNETIFACE_SUPL_CLIENT)) ||
            (0 == clientName.compare(LOCNETIFACE_SUPL_ES_CLIENT))) {
        LOC_LOGd("Num Supl clients %d", mSuplClientCnt);
        if (mSuplClientCnt > 0) {
            mSuplClientCnt--;
        } else {
            // Should never happend
            LOC_LOGe("mSuplClientCnt is 0");
        }
    }

    profileHdlCleared = false;
    auto mapIter {mClientToProfileHdl.find(clientName)};

    if (mapIter != mClientToProfileHdl.end()) {
        profileHdl = mapIter->second;
        LOC_LOGd("LocNetIfClient:%s found for ProfileHdl: %d",
                mapIter->first.c_str(), profileHdl);
        // remove from mClientToProfileHdl list
        mClientToProfileHdl.erase(mapIter);
        // also update mProfileHdlToBackhaulCtxt set.
        auto iter {mProfileHdlToBackhaulCtxt.find(profileHdl)};
        if (iter != mProfileHdlToBackhaulCtxt.end()) {
            ClientBackhaulRequest &backhaulCtxtSet = iter->second;
            BackhaulContext ctxt{clientName, 0, "", 0};
            ClientBackhaulRequest::const_iterator it = backhaulCtxtSet.find(ctxt);
            if (it != backhaulCtxtSet.end()) {
                // client found, remove from set.
                LOC_LOGd("Removing client %s from ClientBackhaulRequest Set",
                        clientName.c_str());
                backhaulCtxtSet.erase(it);
            } else {
                LOC_LOGe("Client: %s not found in map for profileHdl: %d ",
                        clientName.c_str(), profileHdl);
            }

            // Send disconnect request to QCMAP. Check if any more clients are there for the
            // same profile-hdl. If no more clients are there, then disconnect this profile
            if (0 == backhaulCtxtSet.size()) {
                // No more clients for profile-hdl. Invoke disconnect
                LOC_LOGd("No more clients for ProfileHdl: %d", profileHdl);
                mProfileHdlToBackhaulCtxt.erase(iter);
                profileHdlCleared = true;
            }
        } else {
            LOC_LOGw("Profile-Hdl %d not found in ProfileHdl map", profileHdl);
        }
    } else {
        LOC_LOGw("Client %s has not requested for connection. Not found in clientmap",
                clientName.c_str());
    }
}

void LocNetIface::cleanUpForConnectApiCallfailure(string clientName) {
    bool invokeDisconnect = false;
    uint32_t profileHandle = 0;
    removeClientFromInternalClientCache(clientName, profileHandle, invokeDisconnect);
    // If we have no more clients, invoke disable mobileap
    uint32_t numBackHaulClients = mClientToProfileHdl.size();
    LOC_LOGi("Num backhaul clients %d, Num Supl clients %d", numBackHaulClients, mSuplClientCnt);
    if (numBackHaulClients <= 0) {
        disableMobileApInternal();
    }
}

bool LocNetIface::connectBackhaulInternal(const string& clientName,
                                          LocSubId subId,
                                          const string& apn,
                                          const LocApnIpType ipType) {
    ENTRY_LOG();
    lock_guard<recursive_mutex> guard(mMutex);

    LOC_LOGd("clientName=%s subId=%d apn=%s ipType=%d",
             clientName.c_str(), subId, apn.c_str(), ipType);

    IF_LOC_LOGD {
        LOC_LOGd("Connect: List of client requested for backhaul");
        printProfileListInformation();
    }
    // reset states
    mWwanSuplConnectCbState = LOC_NET_WWAN_SUPL_CB_STATE_DEFAULT;

    /* Track each connection attempt by new clients,
     * by increasing connect request recvd counter before notifying
     * and returning success. */
    if ((0 == clientName.compare(LOCNETIFACE_SUPL_CLIENT)) ||
            (0 == clientName.compare(LOCNETIFACE_SUPL_ES_CLIENT))) {
        mSuplClientCnt++;
        LOC_LOGd("Num Supl clients %d", mSuplClientCnt);
    }

    bool isSuplEs = false;
    LocNetConnType cLocNetConnType;
    if (0 == clientName.compare(LOCNETIFACE_SUPL_CLIENT)) {
        cLocNetConnType = LOC_NET_CONN_TYPE_WWAN_SUPL;
    } else if (0 == clientName.compare(LOCNETIFACE_SUPL_ES_CLIENT)) {
        isSuplEs = true;
        cLocNetConnType = LOC_NET_CONN_TYPE_WWAN_EMERGENCY;
    } else {
        cLocNetConnType = LOC_NET_CONN_TYPE_WWAN_INTERNET;
    }

    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    if (!mIsMobileApEnabled) {
        LOC_LOGi("Enabling MobileAP..");
         /* Need to enable MobileAP to invoke backhaul functions */
        bool ret = mQcmapClientPtr->EnableMobileAP(&qmi_err_num);
        if (!ret) {
            if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                LOC_LOGe("Failed to enable mobileap, qcmapErr %d", qmi_err_num);
                return false;
            } else {
                LOC_LOGe("Enable mobileap: consider success. Err 0x%x (QMI_ERR_NO_EFFECT_V01)",
                        qmi_err_num);
            }
        }
        mIsMobileApEnabled = true;
    }
    setLocNetConnType(cLocNetConnType);

    char* apnstr = new char[apn.length() + 1];
    strlcpy(apnstr, apn.c_str(), apn.length()+1);
    if ((LOC_NET_CONN_TYPE_WWAN_EMERGENCY == cLocNetConnType) ||
            (LOC_NET_CONN_TYPE_WWAN_SUPL == cLocNetConnType)) {
        setSuplApnName(apnstr);
        setSuplIpType(ipType);
    } else {
        // Internet data
        setInternetApnName(apnstr);
        setInternetIpType(ipType);
    }
    delete[] apnstr;

    uint32_t profile_handle = 0;
    /* Send connect request to QCMAP */
    if (!getProfileHandleForApn(subId, apn, isSuplEs, profile_handle)) {
        LOC_LOGe("getProfileHandleForApn failed");
        cleanUpForConnectApiCallfailure(clientName);
        return false;
    }
    LOC_LOGi("getProfileHandleForApn returns Profile handle: %d", profile_handle);
    // For SUPL ES, we get the APN name from WDS profile. Fill up the value
    string newApn = isSuplEs ? getSuplApnName() : apn;

    // Check if call already raised with same ProfileHdl, if so add client
    // to that ProfileHdl2Client map.
    bool sameProfileFound = false;
    BackhaulContext ctx = { clientName, (uint16_t) subId, newApn, (uint16_t) ipType };
    auto mapIter {mProfileHdlToBackhaulCtxt.find(profile_handle)};
    if (mapIter != mProfileHdlToBackhaulCtxt.end()) {
        LOC_LOGd("Connection already requested for profile handle:%d with Sub=%d, APN=%s",
                profile_handle, subId, newApn.c_str());
        // Add this new client to ProfileHdl2Client map, backhaulCtxSet
        mapIter->second.emplace(ctx);
        sameProfileFound = true;
    } else {
        LOC_LOGd("New profile handle:%d with Sub=%d, APN=%s", profile_handle, subId,
                newApn.c_str());
        ClientBackhaulRequest backhaulCtxtSet;
        backhaulCtxtSet.emplace(ctx);
        mProfileHdlToBackhaulCtxt[profile_handle] = backhaulCtxtSet;
    }
    mClientToProfileHdl[clientName] = profile_handle;

    IF_LOC_LOGD {
        LOC_LOGd("Connect: Updated list of client requested for backhaul");
        printProfileListInformation();
    }

    /* Check if backhaul is already connected */
    qmi_err_num = QMI_ERR_NONE_V01;
    bool needReconnectAgain = true;
    qcmap_msgr_wwan_status_enum_v01 v4_status, v6_status;
    if (mQcmapClientPtr->GetWWANStatus(
        &v4_status, &v6_status, &qmi_err_num) == false) {
        LOC_LOGe("Failed to get wwan status, err 0x%x", qmi_err_num);
    }
    LOC_LOGd("v4_status: %d, v6_status: %d", v4_status, v6_status);

    LocNetWwanCallEvent callEvent = LOC_NET_WWAN_CALL_EVT_OPEN_FAILED;
    if (v4_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 ||
        v6_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01) {
        LOC_LOGd("WWAN Backhaul already connected");
        callEvent = LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS;
        // No need of reconnect as already connected
        needReconnectAgain = false;
    } else if (v4_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
        v6_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01) {
        LOC_LOGd("WWAN Backhaul connecting failed");
    } else if (v4_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
        v6_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01) {
        LOC_LOGd("WWAN Backhaul disconnected");
    } else if (v4_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01 ||
        v6_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01) {
        LOC_LOGd("WWAN Backhaul disconnecting failed");
    }

    LOC_LOGd("Need reconnect :%d ?", needReconnectAgain);
    if (!needReconnectAgain) {
        if (!sameProfileFound) {
            // Already connect, but same profile not found - case for automotive targets
            // where connection is already up via xtra-daemon via telsdk-data route. So
            // here (for supl calls), we need to increase ref_count at QCMAP side to keep
            // connection active
            qmi_err_num = QMI_ERR_NONE_V01;
            LocApnIpType dataCallIpType = LOC_APN_IP_INVALID;
            if ((LOC_NET_CONN_TYPE_WWAN_EMERGENCY == cLocNetConnType) ||
                    (LOC_NET_CONN_TYPE_WWAN_SUPL == cLocNetConnType)) {
                // Supl
                dataCallIpType = getSuplIpType();
            } else {
                // Internet data
                dataCallIpType = getInternetIpType();
            }
            qcmap_msgr_wwan_call_type_v01 wwan_call_type = getWwanCallType(dataCallIpType);
            LOC_LOGi("Sending ConnectBackhaul request (sameProfileFound) wwan_call_type=%d",
                    wwan_call_type);

            if (mQcmapClientPtr->ConnectBackHaul(wwan_call_type, &qmi_err_num) == false) {
                if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                    LOC_LOGe("Connect backhaul failed, err 0x%x", qmi_err_num);
                } else {
                    LOC_LOGd("Connect backhaul:consider success.Err 0x%x (QMI_ERR_NO_EFFECT_V01)",
                            qmi_err_num);
                }
            } else {
                LOC_LOGd("Connect Backhaul request is send");
            }
        }
        // Notify clients
        if (LOC_NET_CONN_TYPE_WWAN_INTERNET == cLocNetConnType) {
            // Internet data
            notifyCurrentNetworkInfo(true);
        } else {
            // SUPL data call
            mWwanCallStatusCb(callEvent, LOC_NET_CONN_TYPE_WWAN_SUPL, getSuplApnName(),
                    getSuplIpType());
        }
        return true;
    }

    uint32_t connectRetryCnt = 0;
    bool connectReqStatus = false;
    LOC_LOGd("Invoke SetWWANProfileHandle for profile handle: %d", profile_handle);
    do
    {
        // Set client preference/profile handle preference
        profile_handle_type_v01 profileHdl = (profile_handle_type_v01)profile_handle;
        if (mQcmapClientPtr->SetWWANProfileHandlePreference(profileHdl, &qmi_err_num)) {
            LOC_LOGd("SetWWANProfileHandlePreference succeeded");
            connectReqStatus = true;
            break;
        } else {
            if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                connectRetryCnt++;
                LOC_LOGd("SetWWANProfileHandlePreference failed. Error 0x%x.  Retrying %d",
                        qmi_err_num, connectRetryCnt);
                usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
            } else {
                LOC_LOGd("SetWWANProfileHandlePreference: consider success. Err 0x%x",
                        qmi_err_num);
                connectReqStatus = true;
                break;
            }
        }
    } while (connectRetryCnt < CONNECT_BACKHAUL_RETRY_COUNT);

    if (!connectReqStatus) {
        LOC_LOGe("Unable to send WWANProfileHandlePreference even after retrying %d times",
                connectRetryCnt);
    }

    /* Enable roaming */
    qmi_err_num = QMI_ERR_NONE_V01;
    if (mSetRoaming) {
        LOC_LOGi("Calling SetRoaming enable");
        if (false == mQcmapClientPtr->SetRoaming(true, &qmi_err_num)) {
            LOC_LOGe("SetRoaming failed, err 0x%x", qmi_err_num);
        }
    } else {
        LOC_LOGi("SET_ROAMING is set to false in gps.conf, don't call SetRoaming");
    }

    qmi_err_num = QMI_ERR_NONE_V01;
    LocApnIpType dataCallIpType = LOC_APN_IP_INVALID;
    if ((LOC_NET_CONN_TYPE_WWAN_EMERGENCY == cLocNetConnType) ||
            (LOC_NET_CONN_TYPE_WWAN_SUPL == cLocNetConnType)) {
        // Supl
        dataCallIpType = getSuplIpType();
    } else {
        // Internet data
        dataCallIpType = getInternetIpType();
    }
    qcmap_msgr_wwan_call_type_v01 wwan_call_type = getWwanCallType(dataCallIpType);

    LOC_LOGi("Sending ConnectBackhaul request wwan_call_type=%d", wwan_call_type);

    connectRetryCnt = 0;
    connectReqStatus = false;
    do
    {
        if (mQcmapClientPtr->ConnectBackHaul(wwan_call_type, &qmi_err_num) == false) {
            if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                connectRetryCnt++;
                LOC_LOGe("Connect backhaul failed, err 0x%x. Retrying %d",
                        qmi_err_num, connectRetryCnt);
                usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
            } else {
                LOC_LOGd("Connect backhaul: consider success. Err 0x%x (QMI_ERR_NO_EFFECT_V01)",
                        qmi_err_num);
                connectReqStatus = true;
                break;
            }
        } else {
            LOC_LOGd("Connect Backhaul request is send");
            connectReqStatus = true;
            break;
        }
    } while (connectRetryCnt < CONNECT_BACKHAUL_RETRY_COUNT);

    if (!connectReqStatus) {
        // Do not Disable mobile AP if connect backhaul fails, as we
        // should not unnecessarily enable/disable MobileAp. MobileAP
        // will be disabled in Disconnect Backhaul after retries are
        // done.
        LOC_LOGe("Unable to send Connect Backhaul request even after retrying %d times",
                connectRetryCnt);
        cleanUpForConnectApiCallfailure(clientName);
        return false;
    }

    return true;
}
#endif //#ifndef NO_MODEM_TARGET

qcmap_msgr_wwan_call_type_v01 LocNetIface::getWwanCallType(const LocApnIpType ipType) {
    qcmap_msgr_wwan_call_type_v01 qIpType;

    switch (ipType) {
    case LOC_APN_IP_IPV4:
        qIpType = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
        break;
    case LOC_APN_IP_IPV6:
        qIpType = QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01;
        break;
    case LOC_APN_IP_IPV4V6:
    default:
        qIpType = QCMAP_MSGR_WWAN_CALL_TYPE_V4V6_V01;
        break;
    }
    return qIpType;
}

bool LocNetIface::disconnectBackhaul(const string& clientName,
                                     bool async,
                                     const LocSubId subId,
                                     const string& apn,
                                     const LocApnIpType ipType) {

#ifdef NO_MODEM_TARGET
    LOC_LOGd("This target does not supports modem");
    return true;
#else
    /* QCMAP client instance must have been created.
    * Happens when some client subscribes. */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGe("No QCMAP instance");
        return false;
    }

    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    qcmap_msgr_wwan_status_enum_v01 v4_status, v6_status;
    if (mQcmapClientPtr->GetWWANStatus(
        &v4_status, &v6_status, &qmi_err_num) == false) {
        LOC_LOGd("Failed to get wwan status, err 0x%x", qmi_err_num);
    } else {
        LOC_LOGd("v4_status: %d, v6_status: %d", v4_status, v6_status);
        if ((QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01 == v4_status)  ||
                (QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01 == v6_status)) {
            LOC_LOGw("Connection is in progress. Wait for sometime before disconnecting");
            // add client name to map.
            BackhaulContext ctx;
            ctx.clientName = clientName;
            ctx.prefSub = (uint16_t) subId;
            ctx.prefApn = apn;
            ctx.prefIpType = (uint16_t) ipType;
            LOC_LOGd("Adding client %s to disconnect req cache", clientName.c_str());
            mBackHaulDisconnReqCache[ctx.clientName] = ctx;
            return true;
        }
    }

    // Check if a request from client is there.
    // Not expecting multiple request from same client at same time.
    auto mapIter {mClientToProfileHdl.find(clientName)};
    if (mapIter == mClientToProfileHdl.end()) {
        LOC_LOGd("Client %s has not requested for connection. Not found in clientmap",
                clientName.c_str());
        return true;
    }

    LocSubId newSubId = subId;
    LocApnIpType newIpType = ipType;
    string   newApnName = apn;
    if (LOC_APN_IP_INVALID == ipType &&
            ((0 != clientName.compare(LOCNETIFACE_SUPL_CLIENT)) &&
                (0 != clientName.compare(LOCNETIFACE_SUPL_ES_CLIENT)))
        ) {
        // If IP type is invalid for internet WWAN call.
        // For SUPL data call, correct configurations are provided by
        // LocNetIfaceAgps.
        newSubId = (LocSubId)mInternetSubId;
        newIpType = (LocApnIpType)mInternetIpType;
        newApnName = mInternetApnName;
        LOC_LOGd("For clientName=%s disonnect NewSubId=%d NewApn=%s NewIpType=%d",
             clientName.c_str(), newSubId, newApnName.c_str(), newIpType);
    }

    if (!async) return disconnectBackhaulInternal(clientName, newSubId, newApnName, newIpType);

    std::thread t([this, clientName, newSubId, newApnName, newIpType] {
        disconnectBackhaulInternal(clientName, newSubId, newApnName, newIpType);
    });
    t.detach();
    return true;
#endif // #ifndef NO_MODEM_TARGET
}

#ifndef NO_MODEM_TARGET
bool LocNetIface::disconnectBackhaulInternal(const string& clientName,
    LocSubId subId,
    const string& apn,
    const LocApnIpType ipType) {
    ENTRY_LOG();
    lock_guard<recursive_mutex> guard(mMutex);

    // check how many clients are there.
    uint32_t numBackHaulClients = mClientToProfileHdl.size();
    if (numBackHaulClients <= 0) {
        LOC_LOGe("Invalid number of clients for backhaul %d", numBackHaulClients);
        return false;
    }

    IF_LOC_LOGD {
        LOC_LOGd("Disconnect: List of client requests for backhaul");
        printProfileListInformation();
    }

    // Check if client has requested for backhaul connection.
    uint32_t profileHdl = 0;
    bool invokeDisconnect = false;
    LOC_LOGd("Disconnect: Removing client %s from backhaul req list", clientName.c_str());
    removeClientFromInternalClientCache(clientName, profileHdl, invokeDisconnect);

    IF_LOC_LOGD {
        LOC_LOGd("Disconnect: Update list of client requests for backhaul");
        printProfileListInformation();
    }

    bool isSuplEs = false;
    bool isSuplCall = false;
    char* apnstr = new char[apn.length() + 1];
    strlcpy(apnstr, apn.c_str(), apn.length() + 1);
    if (0 == clientName.compare(LOCNETIFACE_SUPL_CLIENT)) {
        isSuplCall = true;
        setLocNetConnType(LOC_NET_CONN_TYPE_WWAN_SUPL);
        setSuplApnName(apnstr);
        setSuplIpType(ipType);
    } else if (0 == clientName.compare(LOCNETIFACE_SUPL_ES_CLIENT)) {
        isSuplEs = true;
        isSuplCall = true;
        setLocNetConnType(LOC_NET_CONN_TYPE_WWAN_EMERGENCY);
        setSuplApnName(apnstr);
        setSuplIpType(ipType);
    } else {
        setLocNetConnType(LOC_NET_CONN_TYPE_WWAN_INTERNET);
        setInternetApnName(apnstr);
        setInternetIpType(ipType);
    }
    delete[] apnstr;
    LOC_LOGd("isSuplEs:%d, isSuplCall:%d", isSuplEs, isSuplCall);


#ifdef OPENWRT_BUILD
    // Sent wwan status if it is supl call
    // If internet and SUPL data call uses same profile handle, then ATL close request
    // could not be send due to Disconnect backhaul is not getting invoked
    if (isSuplCall) {
        LOC_LOGd("Sending wwan status callback for SUPL calls.");
        mWwanCallStatusCb(LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS, LOC_NET_CONN_TYPE_WWAN_SUPL,
                    getSuplApnName(), getSuplIpType());
    }
#endif

    // If still not yet ready to disconnect for this profile-hdl
    if (!invokeDisconnect) {
        return true;
    }

    uint32_t disconnectRetryCnt = 0;
    bool disconnectReqStatus = false;
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;

    // Teardown indication will be sent out to client if there is active call and we
    // disconnect call. Client will get notification from server for same.
    qcmap_msgr_wwan_status_enum_v01 v4_status, v6_status;
    if (mQcmapClientPtr->GetWWANStatus(
        &v4_status, &v6_status, &qmi_err_num) == false) {
        LOC_LOGd("Failed to get wwan status, err 0x%x", qmi_err_num);
    } else {
        LOC_LOGd("v4_status: %d, v6_status: %d", v4_status, v6_status);
    }

    LOC_LOGd("Invoke SetWWANProfileHandle for profile handle: %d", profileHdl);
    do
    {
        qmi_err_num = QMI_ERR_NONE_V01;
        // Set client preference/profile handle preference
        profile_handle_type_v01 profile_handle = (profile_handle_type_v01)profileHdl;
        if (mQcmapClientPtr->SetWWANProfileHandlePreference(profile_handle, &qmi_err_num)) {
            LOC_LOGd("SetWWANProfileHandlePreference succeeded");
            disconnectReqStatus = true;
            break;
        } else {
            if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                disconnectRetryCnt++;
                LOC_LOGd("SetWWANProfileHandlePreference failed. Error 0x%x.  Retrying %d",
                        qmi_err_num, disconnectRetryCnt);
                usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
            } else {
                LOC_LOGd("SetWWANProfileHandlePreference: consider success. Err 0x%x",
                        qmi_err_num);
                disconnectReqStatus = true;
                break;
            }
        }
    } while (disconnectRetryCnt < CONNECT_BACKHAUL_RETRY_COUNT);

    if (!disconnectReqStatus) {
        LOC_LOGe("Unable to send WWANProfileHandlePreference even after retrying %d times",
                disconnectRetryCnt);
    }

    LocNetConnType connType = getLocNetConnType();
    LocApnIpType dataCallIpType = LOC_APN_IP_INVALID;
    if ((LOC_NET_CONN_TYPE_WWAN_EMERGENCY == connType) ||
            (LOC_NET_CONN_TYPE_WWAN_SUPL == connType)) {
        // Supl
        dataCallIpType = getSuplIpType();
    } else {
        // Internet data
        dataCallIpType = getInternetIpType();
    }
    qcmap_msgr_wwan_call_type_v01 wwan_call_type = getWwanCallType(dataCallIpType);

    LOC_LOGi("Sending DisconnectBackhaul wwan_call_type=%d", wwan_call_type);
    disconnectRetryCnt = 0;
    disconnectReqStatus = false;
    do
    {
        qmi_err_num = QMI_ERR_NONE_V01;
        // Set client preference/profile handle preference
        profile_handle_type_v01 profile_handle = (profile_handle_type_v01)profileHdl;
        // Set state to default before invoking disconnect backhaul. In teardown callback,
        // we need to check for the states.
        mWwanSuplDiscnctCbState = LOC_NET_WWAN_SUPL_CB_STATE_DEFAULT;
        if (mIsMobileApEnabled && mQcmapClientPtr->DisconnectBackHaul(
            wwan_call_type, &qmi_err_num) == false) {
            if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                disconnectRetryCnt++;
                LOC_LOGd("Disconnect backhaul failed, err 0x%x. Retrying %d",
                        qmi_err_num, disconnectRetryCnt);
                usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
            } else {
                LOC_LOGd("Disconnect backhaul: consider success. Err 0x%x (QMI_ERR_NO_EFFECT_V01)",
                        qmi_err_num);
                disconnectReqStatus = true;
                break;
            }
        } else {
            disconnectReqStatus = true;
            break;
        }
    } while (disconnectRetryCnt < CONNECT_BACKHAUL_RETRY_COUNT);

    if (!disconnectReqStatus) {
        LOC_LOGe("Disconnect backhaul failed even after retrying %d times",
                disconnectRetryCnt);
        // Even if DisconnectBackHaul fails, do not return, we need to
        // DisableMobileAP in any case.
    }

    // Send wwan status callback to HAL for SUPL calls.
    // For supl calls, we have to notify SUPL clients of Session stop/ATLClose
    LOC_LOGd("isSuplCall:%d", isSuplCall);
    if (isSuplCall) {
        LOC_LOGd("Sending WWAN_CALL_EVT_CLOSE wwan status callback for SUPL calls.");
        mWwanCallStatusCb(LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS, LOC_NET_CONN_TYPE_WWAN_SUPL,
                    getSuplApnName(), getSuplIpType());
    }

    // If we still have clients, dont invoke disable mobileap
    numBackHaulClients = mClientToProfileHdl.size();
    LOC_LOGi("Num backhaul clients %d, Num Supl clients %d", numBackHaulClients, mSuplClientCnt);
    if (numBackHaulClients > 0) {
        LOC_LOGd("LocNetIface clients still remaining");
        return true;
    }

    disableMobileApInternal();
    mIsMobileApEnabled = false;
    return true;
}

bool LocNetIface::disableMobileApInternal() {
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    qcmap_msgr_bootup_flag_v01 mobileap_enable, wlan_enable;
    bool bRetval = false;

    if (mQcmapClientPtr->GetQCMAPBootupCfg(&mobileap_enable, &wlan_enable, &qmi_err_num)) {
        LOC_LOGd("MobileAP bootup cfg: %d, WLAN bootup cfg: %d", mobileap_enable, wlan_enable);
    } else {
        // assume bootup config default values are disabled.
        mobileap_enable = QCMAP_MSGR_DISABLE_ON_BOOT_V01;
        wlan_enable = QCMAP_MSGR_DISABLE_ON_BOOT_V01;
    }
    if ((QCMAP_MSGR_ENABLE_ON_BOOT_V01 == mobileap_enable) ||
            (QCMAP_MSGR_ENABLE_ON_BOOT_V01 == wlan_enable)) {
        LOC_LOGi("Not Disabling MobileAp since bootup cfg for mobileap or wlan is enabled..");
        bRetval = true;
    } else {
        LOC_LOGi("Disabling MobileAp..");
        bRetval = mQcmapClientPtr->DisableMobileAP(&qmi_err_num);
        if (!bRetval) {
            if ((QMI_ERR_NO_EFFECT_V01 != qmi_err_num) && (QMI_ERR_NONE_V01 != qmi_err_num)) {
                LOC_LOGe("Failed to disable mobileap, qcmapErr %d", qmi_err_num);
            } else {
                LOC_LOGe("Disable mobileap: consider success. Err 0x%x (QMI_ERR_NO_EFFECT_V01)",
                        qmi_err_num);
                bRetval = true;
            }
        }
        mIsMobileApEnabled = false;
    }
    return bRetval;
}
#endif // #ifndef NO_MODEM_TARGET
