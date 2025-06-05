/*
 * Copyright (c) 2018-2019, 2021-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2018 The Android Open Source Project
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

import android.content.Context;
import android.hardware.radio.data.SetupDataCallResult;

import android.net.InetAddresses;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.telephony.CellIdentity;
import android.telephony.CellIdentityCdma;
import android.telephony.CellIdentityGsm;
import android.telephony.CellIdentityLte;
import android.telephony.CellIdentityTdscdma;
import android.telephony.CellIdentityWcdma;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.DataService;
import android.telephony.data.DataServiceCallback;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.NetworkSliceInfo;
import android.telephony.data.TrafficDescriptor;
import android.util.Log;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.ConcurrentHashMap;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.HashSet;
import java.util.stream.Collectors;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;

/**
 * This class represents iwlan service which handles wlan data requests and response
 * from the cellular modem.
 */
public class IWlanDataService extends DataService {
    public static final String TAG = IWlanDataService.class.getSimpleName();

    private static final int SETUP_DATA_CALL_COMPLETE               = 1;
    private static final int DEACTIVATE_DATA_CALL_COMPLETE          = 2;
    private static final int REQUEST_DATA_CALL_LIST_COMPLETE        = 3;
    private static final int DATA_CALL_LIST_CHANGED                 = 4;
    private static final int APN_UNTHROTTLED                        = 5;

    HashSet<IWlanDataServiceProvider> providerSet = new HashSet<IWlanDataServiceProvider>();

    private class IWlanDataServiceProvider extends DataService.DataServiceProvider {

        private final ConcurrentHashMap<Message, DataServiceCallback> mCallbackMap = new ConcurrentHashMap<>();

        private final Looper mLooper;

        private final Handler mHandler;

        private final IWlanProxy proxy;

        private boolean isActive = true;

        private IWlanDataServiceProvider(int slotIndex) {
            super(slotIndex);

            proxy = IWlanProxy.getInstanceBySlotId(getApplicationContext(), slotIndex);

            HandlerThread thread = new HandlerThread(IWlanDataService.class.getSimpleName());
            thread.start();
            mLooper = thread.getLooper();
            mHandler = new Handler(mLooper) {
                @Override
                public void handleMessage(Message message) {
                    DataServiceCallback callback = mCallbackMap.remove(message);

                    AsyncResult ar = (AsyncResult) message.obj;
                    switch (message.what) {
                        case SETUP_DATA_CALL_COMPLETE:
                            SetupDataCallResult result = (SetupDataCallResult) ar.result;
                            callback.onSetupDataCallComplete(ar.exception != null
                                    ? DataServiceCallback.RESULT_ERROR_ILLEGAL_STATE
                                    : DataServiceCallback.RESULT_SUCCESS,
                                    convertDataCallResult(result));
                            break;
                        case DEACTIVATE_DATA_CALL_COMPLETE:
                            callback.onDeactivateDataCallComplete(ar.exception != null
                                    ? DataServiceCallback.RESULT_ERROR_ILLEGAL_STATE
                                    : DataServiceCallback.RESULT_SUCCESS);
                            break;
                        case REQUEST_DATA_CALL_LIST_COMPLETE:
                            callback.onRequestDataCallListComplete(
                                    ar.exception != null
                                            ? DataServiceCallback.RESULT_ERROR_ILLEGAL_STATE
                                            : DataServiceCallback.RESULT_SUCCESS,
                                    ar.exception != null
                                            ? null
                                            : convertDataCallList((List<SetupDataCallResult>) ar.result)
                                    );
                            break;
                        case DATA_CALL_LIST_CHANGED:
                            notifyDataCallListChanged(convertDataCallList((List<SetupDataCallResult>) ar.result));
                            break;
                        case APN_UNTHROTTLED:
                            notifyApnUnthrottled((String)ar.result);
                            break;
                        default:
                            proxy.iwlanLog.e(TAG, "Unexpected event: " + message.what);
                            return;
                    }
                }
            };

            proxy.iwlanLog.d(TAG, "Register for unsolicited indications.");
            proxy.registerForDataCallListChanged(mHandler, DATA_CALL_LIST_CHANGED);
            proxy.registerForApnUnthrottled(mHandler, APN_UNTHROTTLED);
        }

        private List<DataCallResponse> convertDataCallList(List<android.hardware.radio.data.SetupDataCallResult> dcList) {
            List<DataCallResponse> dcResponseList = new ArrayList<>();
            for (android.hardware.radio.data.SetupDataCallResult dcResult : dcList) {
                dcResponseList.add(convertDataCallResult(dcResult));
            }
            return dcResponseList;
        }

        @Override
        public void setupDataCall(int accessNetwork, DataProfile dataProfile, boolean isRoaming,
                                  boolean allowRoaming, int reason, LinkProperties linkProperties,
                                  DataServiceCallback callback) {
            proxy.iwlanLog.d(TAG, "setupDataCall " + getSlotIndex());
            int pduSessionId = 0;
            boolean matchAllRuleAllowed = false;
            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, SETUP_DATA_CALL_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            proxy.setupDataCall(accessNetwork, dataProfile, isRoaming, allowRoaming, reason, linkProperties, pduSessionId, null, null, matchAllRuleAllowed, message);
        }

        @Override
        public void setupDataCall(int accessNetworkType, DataProfile dataProfile,
                                  boolean isRoaming, boolean allowRoaming, int reason,
                                  LinkProperties linkProperties, int pduSessionId,
                                  NetworkSliceInfo sliceInfo, TrafficDescriptor trafficDescriptor,
                                  boolean matchAllRuleAllowed, DataServiceCallback callback) {
            /* Call the old version since the new version isn't supported */
            proxy.iwlanLog.d(TAG, "setupDataCall " + getSlotIndex());
            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, SETUP_DATA_CALL_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            proxy.setupDataCall(accessNetworkType, dataProfile, isRoaming, allowRoaming, reason, linkProperties, pduSessionId, sliceInfo, trafficDescriptor, matchAllRuleAllowed, message);
        }

        @Override
        public void deactivateDataCall(int cid, int reason, DataServiceCallback callback) {
            proxy.iwlanLog.d(TAG, "deactivateDataCall " + getSlotIndex());
            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, DEACTIVATE_DATA_CALL_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            proxy.deactivateDataCall(cid, reason, message);
        }

        @Override
        public void requestDataCallList(DataServiceCallback callback) {
            proxy.iwlanLog.d(TAG, "requestDataCallList " + getSlotIndex());
            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, REQUEST_DATA_CALL_LIST_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            proxy.getDataCallList(message);
        }

        @Override
        public void close() {
            Log.d(TAG, "IWlanDataService Closing");
            if(isActive) {
                Log.d(TAG, "IWlanDataService unregister with proxy");
                proxy.unregisterForDataCallListChanged(mHandler);
                proxy.unregisterForApnUnthrottled(mHandler);
                IWlanProxy.disableIWlan(proxy);
                isActive = false;

                if(providerSet.contains(this)) {
                    providerSet.remove(this);
                    Log.d(TAG, "remove entry from providerSet");
                }
            }
        }

        public void dump(PrintWriter pw) {
            pw.println("IWlanDataServiceProvider:");
            proxy.iwlanLog.dataLogBuffer.dump(pw);
        }
    }

    @Override
    public DataServiceProvider onCreateDataServiceProvider(int slotId) {
        Log.d(TAG, "IWlan data service created for slot " + slotId);
        //print the package version info
        getPackageVersionInfo();
        TelephonyManager tm = (TelephonyManager) getApplicationContext().getSystemService(Context.TELEPHONY_SERVICE);
        if (slotId < 0 || slotId > tm.getPhoneCount()) {
            Log.e(TAG, "Tried to create iwlan data service with invalid slotId " + slotId);
            return null;
        }
        IWlanDataServiceProvider provider = new IWlanDataServiceProvider(slotId);
        providerSet.add(provider);
        return provider;
    }

    @Override
    protected void dump(FileDescriptor fd, PrintWriter writer, String[] args){
        writer.println("IWlanDataService:");
        for(IWlanDataServiceProvider p: providerSet) {
            if(p != null)
                p.dump(writer);
        }
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "IWlanDataService destroy called");
        super.onDestroy();
        HashSet<IWlanDataServiceProvider> tempSet = new HashSet<>();
        tempSet.addAll(providerSet);
        for(IWlanDataServiceProvider p: tempSet) {
            p.close();
        }
        providerSet.clear();
    }

    /**
     * Convert SetupDataCallResult defined in types.hal into DataCallResponse
     * @param dcResult setup data call result
     * @return converted DataCallResponse object
     */

    public DataCallResponse convertDataCallResult(SetupDataCallResult dcResult) {
        if (dcResult == null) return null;

        List<LinkAddress> laList = Arrays.stream(dcResult.addresses).map(la ->
                createLinkAddressFromString(la.address, la.addressProperties, la.deprecationTime,
                la.expirationTime)).collect(Collectors.toList());

        // Process dns
        String[] dnses = Arrays.copyOf(dcResult.dnses, dcResult.dnses.length);
        List<InetAddress> dnsList = new ArrayList<>();
        if (dnses != null) {
            for (String dns : dnses) {
                dns = dns.trim();
                InetAddress ia;
                try {
                    ia = InetAddress.getByName(dns);
                    dnsList.add(ia);
                } catch (UnknownHostException e) {
                    Log.e(TAG, "Unknown dns: " + dns + ", exception = " + e);
                }
            }
        }

        // Process gateway
        String[] gateways = Arrays.copyOf(dcResult.gateways, dcResult.gateways.length);
        List<InetAddress> gatewayList = new ArrayList<>();
        if (gateways != null) {
            for (String gateway : gateways) {
                gateway = gateway.trim();
                InetAddress ia;
                try {
                    ia = InetAddress.getByName(gateway);
                    gatewayList.add(ia);
                } catch (UnknownHostException e) {
                    Log.e(TAG, "Unknown gateway: " + gateway + ", exception = " + e);
                }
            }
        }

        // Process pcscf
        String[] pcscfs = Arrays.copyOf(dcResult.pcscf, dcResult.pcscf.length);
        List<InetAddress> pcscfList = new ArrayList<>();
        if (pcscfs != null) {
            for (String pcscf : pcscfs) {
                pcscf = pcscf.trim();
                InetAddress ia;
                try {
                    ia = InetAddress.getByName(pcscf);
                    pcscfList.add(ia);
                } catch (UnknownHostException e) {
                    Log.e(TAG, "Unknown pcscf: " + pcscf + ", exception = " + e);
                }
            }
        }

        // Process trafficDescriptors
        List<TrafficDescriptor> trafficDescriptors = Arrays.stream(dcResult.trafficDescriptors).map(td ->
                    convertToTrafficDescriptor(td)).collect(Collectors.toList());

        return new DataCallResponse.Builder()
                .setCause(dcResult.cause)
                .setRetryDurationMillis(dcResult.suggestedRetryTime)
                .setId(dcResult.cid)
                .setLinkStatus(dcResult.active)
                .setProtocolType(dcResult.type)
                .setInterfaceName(dcResult.ifname)
                .setAddresses(laList)
                .setDnsAddresses(dnsList)
                .setGatewayAddresses(gatewayList)
                .setPcscfAddresses(pcscfList)
                .setMtuV4(dcResult.mtuV4)
                .setMtuV6(dcResult.mtuV6)
                .setMtu(Math.max(dcResult.mtuV4, dcResult.mtuV6))
                .setHandoverFailureMode(dcResult.handoverFailureMode)
                .setTrafficDescriptors(trafficDescriptors)
                .build();
    }

    /** Convert an ArrayList of Bytes to an exactly-sized primitive array */
    private static byte[] arrayListToPrimitiveArray(ArrayList<Byte> bytes) {
        byte[] ret = new byte[bytes.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = bytes.get(i);
        }
        return ret;
    }

    private TrafficDescriptor convertToTrafficDescriptor(
            android.hardware.radio.data.TrafficDescriptor td) {
        String dnn = td.dnn != null ? td.dnn: null;
        byte[] osAppId = td.osAppId != null ? td.osAppId.osAppId : null;
        TrafficDescriptor.Builder builder = new TrafficDescriptor.Builder();
        if (dnn != null) {
            builder.setDataNetworkName(dnn);
        }
        if (osAppId != null) {
            builder.setOsAppId(osAppId);
        }
        return builder.build();
    }

    private void getPackageVersionInfo(){
        try {
            PackageInfo pInfo = this.getPackageManager().
                            getPackageInfo(this.getPackageName(),0);
            Log.d(TAG, "IWlanDataService running with versionCode "+
                  pInfo.versionCode + " and versionName "+ pInfo.versionName );
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

    }

    private static LinkAddress createLinkAddressFromString(String address, int properties,
        long deprecationTime, long expirationTime) {
        List<LinkAddress> laList = new ArrayList<>();
        address = address.trim();
        InetAddress inetAddress = null;
        int prefixLength = -1;

        try {
            String[] pieces = address.split("/", 2);
            inetAddress = InetAddresses.parseNumericAddress(pieces[0]);
            if (pieces.length == 1) {
                prefixLength = (inetAddress instanceof Inet4Address) ? 32 : 128;
            } else if (pieces.length == 2) {
                prefixLength = Integer.parseInt(pieces[1]);
            }
        } catch (NullPointerException e) {            // Null string.
        } catch (ArrayIndexOutOfBoundsException e) {  // No prefix length.
        } catch (NumberFormatException e) {           // Non-numeric prefix.
        } catch (IllegalArgumentException e) {        // Invalid IP address.
        }

        if (inetAddress == null || prefixLength == -1) {
            throw new IllegalArgumentException("Invalid link address " + address);
        }

        return new LinkAddress(inetAddress, prefixLength, properties, 0,
                deprecationTime, expirationTime);
    }
}
