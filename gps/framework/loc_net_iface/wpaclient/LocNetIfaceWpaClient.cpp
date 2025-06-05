/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <thread>
#include <loc_pla.h>
#include <loc_cfg.h>
#include <log_util.h>
#include <unistd.h>
#include "LocNetIfaceWpaClient.h"

#define LOG_TAG "LocSvc_LocNetIfaceWpaClient"

#define DELAY_BW_CONNECT_RETRIES_MICROSECS (500000)

using namespace loc_core;

LocNetIfaceWpaClient::LocNetIfaceWpaClient() :
    LocNetIfaceBase(),
    mCtrlConn(NULL), mMonitorConn(NULL),
    strCtrlIface(WPA_CONFIG_CTRL_IFACE),
    mCtrlConnOpenTimer("LocNetWpa_CtrlOpen"),
    mMutex() {

    TimerCallback subscribeWpaClientRunable = [this] {
        LOC_LOGd("subscribeWpaClientRunable invoked");
        // Initialize WPA Client instance
        subscribeWithWpaClient();
    };

    LOC_LOGi("Starting bootup subscribeWpaClientRunable");
    mCtrlConnOpenTimer.set(5, subscribeWpaClientRunable);
    mCtrlConnOpenTimer.start();
}

void LocNetIfaceWpaClient::subscribe(
        const std::unordered_set<DataItemId>& itemSetToSubscribe) {
    ENTRY_LOG();
    /* Add items to subscribed unordered_set */
    bool anyUpdatesToSubscriptionList =
            updateSubscribedItemSet(itemSetToSubscribe, true);
    if (anyUpdatesToSubscriptionList) {
        if (isItemSubscribed(NETWORKINFO_DATA_ITEM_ID)) {
            notifyObserverForWlanStatus(true);
        }
        if (isItemSubscribed(WIFIHARDWARESTATE_DATA_ITEM_ID)) {
            notifyObserverForNetworkInfo(false, false, LOC_NET_CONN_TYPE_WLAN);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void LocNetIfaceWpaClient::unsubscribe(
        const std::unordered_set<DataItemId>& itemListToUnsubscribe) {
    ENTRY_LOG();
    /* Remove items from subscribed list */
    updateSubscribedItemSet(itemListToUnsubscribe, false);
}

void LocNetIfaceWpaClient::unsubscribeAll() {
    ENTRY_LOG();
    /* Clear subscription list */
    mSubscribedItemList.clear();
}

void LocNetIfaceWpaClient::requestData(
        const std::unordered_set<DataItemId>& itemListToRequestData) {
    ENTRY_LOG();
    /* NO-OP for LE platform
     * We don't support any data item to fetch data for */
}

void LocNetIfaceWpaClient::subscribeWithWpaClient() {
    ENTRY_LOG();
    std::thread t([this] {
        lock_guard<recursive_mutex> guard(mMutex);
        int counter=0;
        bool wpaInitDone = false;

        if (mCtrlConn) {
            wpa_ctrl_close(mCtrlConn);
            mCtrlConn = NULL;
        }
        if (mMonitorConn) {
            wpa_ctrl_detach(mMonitorConn);
            wpa_ctrl_close(mMonitorConn);
            mMonitorConn = NULL;
        }

        while (counter < 10) {
            LOC_LOGd("Trying to connect to mCtrlConn %s (cnt:%d)", strCtrlIface.c_str(), counter);
            mCtrlConn = wpa_ctrl_open(strCtrlIface.c_str());
            if (NULL != mCtrlConn) {
                LOC_LOGi("wpa_ctrl_open mCtrlConn completed for %s", strCtrlIface.c_str());
                wpaInitDone = true;
                break;
            }
            counter++;
            usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
        }
        if (!wpaInitDone) {
            LOC_LOGe("Wpa init is not complete - mCtrlConn is NULL");
            // clean up to restart wpaclient check
            unsubscribeWithWpaClient();
            // restart ctrl connection timer
            LOC_LOGd("Starting mCtrlConnOpenTimer in subscribeWithWpaClient");
            mCtrlConnOpenTimer.start();
            return;
        }

        counter=0;
        wpaInitDone = false;
        // If ctrl connection already opened, monitor connection open should go fine, so retry for
        // lesser count.
        while (counter < 5) {
            LOC_LOGd("Trying connect to mMonitorConn %s (cnt:%d)", strCtrlIface.c_str(), counter);
            mMonitorConn = wpa_ctrl_open(strCtrlIface.c_str());
            if (NULL != mMonitorConn) {
                LOC_LOGi("wpa_ctrl_open mMonitorConn completed for %s", strCtrlIface.c_str());
                wpaInitDone = true;
                break;
            }
            counter++;
            usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
        }

        if (!wpaInitDone) {
            LOC_LOGe("Wpa init is not complete - mMonitorConn is NULL");
            // clean up to restart wpaclient check
            unsubscribeWithWpaClient();
            // restart ctrl connection timer
            LOC_LOGd("Starting mCtrlConnOpenTimer in subscribeWithWpaClient");
            mCtrlConnOpenTimer.start();
        } else {
            // If we reach here, that means wpainit is complete
            LOC_LOGi("LocNetIfaceWpaClient init complete");
            startAsyncMonitorConnThread();
        }
    });
    t.detach();

    return;
}

bool LocNetIfaceWpaClient::getWpaStatus(const char *field, char *outBuf, size_t outBufSize) {
    bool retVal = false;

    if (NULL == mCtrlConn) {
        LOC_LOGe("mCtrlConn is null");
        return retVal;
    }
    char buf[1024];
    char *pos, *end;
    size_t len, flen;
    int counter=0;
    int iRet = 0;

    while (counter < 5) {
        iRet = wpa_ctrl_request(mCtrlConn, "STATUS-NO_EVENTS", 16, buf, &len, NULL);
        if ((iRet < 0) || (0 == len)) {
            LOC_LOGe("wpa_ctrl_request failure");
            len = 0;
            usleep(DELAY_BW_CONNECT_RETRIES_MICROSECS);
            counter++;
            continue;
        }
        buf[len] = '\0';
        LOC_LOGv("Wpa req buf(%d) is %s", len, buf);
        break;
    }
    if (0 == len) {
        LOC_LOGv("Wpa req failed after multiple retries");
    }

    flen = strlen(field);
    pos = buf;
    while (pos + flen < buf + len) {
        if (pos > buf) {
            if (*pos != '\n') {
                pos++;
                continue;
            }
            pos++;
        }
        if (strncmp(pos, field, flen) != 0 || pos[flen] != '=') {
            pos++;
            continue;
        }
        pos += flen + 1;
        end = strchr(pos, '\n');
        if (NULL == end) {
            LOC_LOGe("getWpaStatus end is nullptr");
            break;
        }
        *end++ = '\0';
        if ((end - pos) > (int) outBufSize) {
            LOC_LOGe("Len %d, greather than outbufsize: %d", (end - pos), (int) outBufSize);
            break;
        }
        memcpy(outBuf, pos, end - pos);
        retVal = true;
    }
    return retVal;
}

bool LocNetIfaceWpaClient::checkIfWlanAlreadyConnected() {
    char state[100];
    bool isConnected = false;
    if (getWpaStatus("wpa_state", state, sizeof(state))) {
        LOC_LOGd("Wpa State is: %s", state);
        if (0 == strcmp(state, "COMPLETED")) {
            isConnected = true;
        }
    }
    return isConnected;
}

void LocNetIfaceWpaClient::startAsyncMonitorConnThread() {
    ENTRY_LOG();

    std::thread t([this] {
        LOC_LOGi("MonitorConnThread starts on interface %s", strCtrlIface.c_str());
        fd_set rfd;
        int fd, ret;
        char buf[256];

        bool monitorInit = false;
        do {
            if (NULL == mMonitorConn) {
                LOC_LOGe("Error !!! mMonitorConn is NULL. MontiorConnThread loop stopped");
                break;
            }

            if (wpa_ctrl_attach(mMonitorConn) < 0) {
                LOC_LOGe("mMonitorConn - Failed to attach to wpa_supplicant");
                break;
            }

            fd = wpa_ctrl_get_fd(mMonitorConn);
            if (fd < 0) {
                LOC_LOGe("wpa_ctrl_get_fd failed. MonitorConnThread loop stopped");
                break;
            }
            monitorInit = true;
        } while (0);

        if (!monitorInit) {
            // Clean up since we have exit the event loop.
            unsubscribeWithWpaClient();

            // restart ctrl connection timer
            LOC_LOGi("Starting mCtrlConnOpenTimer in startAsyncMonitorConnThread");
            mCtrlConnOpenTimer.start();
            return;
        }

        // By this time, may be connection would be already established. So try to check the
        // connection status.
        if (checkIfWlanAlreadyConnected()) {
            LOC_LOGi("WLAN is already connected");
            notifyObserverForNetworkInfo(true, false, LOC_NET_CONN_TYPE_WLAN);
        } else {
            LOC_LOGi("WLAN is not yet connected");
        }
        LOC_LOGi("Start receiving wpa_supplicant events");
        // event handling.
        while (1) {
            size_t len;

            FD_ZERO(&rfd);
            FD_SET(fd, &rfd);

            ret = select(fd + 1, &rfd, NULL, NULL, NULL);
            if (ret < 0) {
                LOC_LOGe("select error: %s", strerror(errno));
                break;
            }

            if (mMonitorConn && wpa_ctrl_pending(mMonitorConn) > 0) {
                len = sizeof(buf) - 1;
                if (wpa_ctrl_recv(mMonitorConn, buf, &len) == 0) {
                    buf[len] = '\0';
                    char *pos = buf, *pos2;
                    int priority = 2;

                    if (*pos == '<') {
                        // skip priority
                        pos++;
                        priority = atoi(pos);
                        pos = strchr(pos, '>');
                        if (pos) {
                            pos++;
                        } else {
                            pos = buf;
                        }
                    }
                    if (isInterestedWpaEvent(pos)){
                        LOC_LOGd("WPA event:%s", pos);
                    }
                    if (stringMatch(pos, WPA_EVENT_CONNECTED)) {
                        LOC_LOGi("WPA event:WPA_EVENT_CONNECTED");
                        notifyObserverForNetworkInfo(true, false, LOC_NET_CONN_TYPE_WLAN);
                    } else if (stringMatch(pos, WPA_EVENT_DISCONNECTED)) {
                        LOC_LOGi("WPA event:WPA_EVENT_DISCONNECTED");
                        notifyObserverForNetworkInfo(false, false, LOC_NET_CONN_TYPE_WLAN);
                    } else if (stringMatch(pos, WPA_EVENT_TERMINATING)) {
                        // Wpa supplicant service is terminating. exit out
                        LOC_LOGi("WPA event:WPA_EVENT_TERMINATING");
                        break;
                    }
                } else {
                    LOC_LOGe("wpa_ctrl_recv failed");
                    break;
                }
            }
        }
        LOC_LOGi("Exit WPA event handling loop");
        // Clean up since we have exit the event loop.
        unsubscribeWithWpaClient();

        // restart ctrl connection timer
        LOC_LOGi("Starting mCtrlConnOpenTimer in startAsyncMonitorConnThread");
        mCtrlConnOpenTimer.start();
    });
    t.detach();

}

void LocNetIfaceWpaClient::unsubscribeWithWpaClient() {
    ENTRY_LOG();
    if (mMonitorConn) {
        wpa_ctrl_detach(mMonitorConn);
        wpa_ctrl_close(mMonitorConn);
        mMonitorConn = NULL;
    }
    if (mCtrlConn) {
        wpa_ctrl_close(mCtrlConn);
        mCtrlConn = NULL;
    }
}
