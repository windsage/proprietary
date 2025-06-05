/*
 * Copyright (c) 2018-2019, 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package vendor.qti.iwlan;

import android.hardware.radio.data.SetupDataCallResult;
import android.os.SystemProperties;
import android.util.Log;

import java.util.ArrayList;

import vendor.qti.hardware.data.iwlandata.IIWlanIndication;
import vendor.qti.hardware.data.iwlandata.QualifiedNetworks;

import android.hardware.radio.data.DataProfileInfo;

public class IWlanIndication
{
    private static final boolean DBG = true;
    private static final String TAG = IWlanIndication.class.getSimpleName();

    IWlanProxy mProxy;

    IWlanIndication(IWlanProxy proxy) {
        mProxy = proxy;
    }

    static String qualifiedNetworksListToString(ArrayList<QualifiedNetworks> qualifiedNetworksList) {
        StringBuilder b = new StringBuilder();
        for (QualifiedNetworks qNetwork : qualifiedNetworksList) {
            //Handle case when there are no available networks for an apn type
            if ((qNetwork.networks == null) || (qNetwork.networks.length == 0)) {
                b.append("No available networks for apnType  " + qNetwork.apnType);
            }
            else {
                int prefNw = qNetwork.networks[0];
                b.append("Pref network for apnType  " + qNetwork.apnType + " is " + prefNw);
            }
        }
        return b.toString();
    }

    /**
     * Indicates when either voice or data network state changed
     */
    public void dataCallListChanged(ArrayList<SetupDataCallResult> dcList) {
        mProxy.iwlanLog.d(IWlanDataService.TAG, "dataCallListChanged");
        mProxy.sendAck(IWlanDataService.TAG);
        mProxy.mDataCallListChangedRegistrants.notifyRegistrants(new AsyncResult(null, dcList, null));
    }

    /**
     * Indicates when data network state changed
     */
    public void dataRegistrationStateChangeIndication() {
        mProxy.iwlanLog.d(IWlanNetworkService.TAG, "networkRegistrationStateChangeIndication");
        mProxy.sendAck(IWlanNetworkService.TAG);
        mProxy.mDataRegistrationStateChangedRegistrants.notifyRegistrants();
    }

    /**
     * Indicates when either voice or data network state changed
     */
    public void qualifiedNetworksChangeIndication(ArrayList<QualifiedNetworks> qnlist) {
        mProxy.iwlanLog.d(QualifiedNetworksServiceImpl.TAG, "qualifiedNetworksChangeIndication" + qualifiedNetworksListToString(qnlist));
        mProxy.sendAck(QualifiedNetworksServiceImpl.TAG);
        mProxy.mQualifiedNetworksChangedRegistrants.notifyRegistrants(new AsyncResult(null, qnlist, null));
    }

    /**
     * Indicates that modem supports iWlan function
     */
    public void modemSupportNotPresent() {
        mProxy.iwlanLog.d(TAG, "modemSupportNotPresent");

        mProxy.modemSupportNotPresent();
    }

    /**
     * Indicates that apn is unthrottled
     */
    public void unthrottleApn(DataProfileInfo info) {
        mProxy.iwlanLog.d(IWlanDataService.TAG, "unthrottleApn " + info.apn);
        mProxy.sendAck(IWlanDataService.TAG);
        mProxy.mApnUnthrottledRegistrants.notifyRegistrants(new AsyncResult(null, info.apn, null));
    }
}
