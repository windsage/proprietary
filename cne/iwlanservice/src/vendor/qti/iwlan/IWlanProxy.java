/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2006 The Android Open Source Project
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
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.RemoteException;
import android.os.SystemClock;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.TrafficDescriptor;
import android.telephony.data.NetworkSliceInfo;
import android.text.TextUtils;
import android.util.Log;
import android.util.SparseArray;

import android.net.LinkProperties;
import android.net.LinkAddress;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicLong;
import java.util.Random;

import vendor.qti.hardware.data.iwlandata.IIWlan;
import vendor.qti.hardware.data.iwlandata.IWlanResponseInfo;

import android.hardware.radio.RadioResponseInfo;
import android.hardware.radio.data.DataProfileInfo;
import android.hardware.radio.data.SliceInfo;
import android.hardware.radio.data.SetupDataCallResult;
import android.hardware.radio.data.OsAppId;

import android.os.HandlerThread;
import android.os.Handler;

import android.os.Parcel;
import android.os.Parcelable;

import vendor.qti.hardware.data.iwlandata.IWlanDataRegStateResult;
import vendor.qti.hardware.data.iwlandata.QualifiedNetworks;

import java.lang.Thread;

public class IWlanProxy {
    public static final String TAG = IWlanProxy.class.getSimpleName();

    private static final String WAKELOCK_TAG = "IWLAN_PROXY_WL";
    private static final String RESP_WAKELOCK_TAG = "IWLAN_PROXY_RESP_WL";

    public IWlanLog iwlanLog;
    JniHelper jniAidl;

    static final String[] HIDL_SERVICE_NAME = {"slot1", "slot2", "slot3"};
    private static IWlanProxy instances[] = new IWlanProxy[HIDL_SERVICE_NAME.length];
    private static int instanceRefCount[] = new int[HIDL_SERVICE_NAME.length];
    volatile IIWlan mService;
    IWlanIndication mIndication;
    IWlanResponse mResponse;
    IWlanAidlClient client;

    /* Modem defaults to supporting iwlan */
    static boolean iwlanModemSupport = true;

    final Handler mHandler;
    final Looper mLooper;
    final AtomicLong mServiceCookie = new AtomicLong(0);

    final Integer mSlotId;
    SparseArray<IWlanRequest> mRequestList = new SparseArray<IWlanRequest>();

    /**
     * Wake lock timeout should be longer than the longest timeout in
     * the vendor ril.
     */
    private static final int WAKE_LOCK_TIMEOUT_MS = 60000;
    private static final int RESP_WAKE_LOCK_TIMEOUT_MS = 200;

    // Variables used to identify releasing of WL on wakelock timeouts
    volatile int mWlSequenceNum = 0;
    volatile int mRespWlSequenceNum = 0;

    // Variables used to differentiate ack messages from request while calling clearWakeLock()
    public static final int INVALID_WAKELOCK = -1;
    public static final int FOR_WAKELOCK = 0;
    public static final int FOR_RESP_WAKELOCK = 1;

    final WakeLock mWakeLock;           // Wake lock associated with request/response
    final WakeLock mRespWakeLock;      // Wake lock associated with response up stack
    // The number of wakelock requests currently active.  Don't release the lock until dec'd to 0
    int mWakeLockCount;

    //***** Request Constants
    static final int REQUEST_SETUP_DATA_CALL         = 1;
    static final int REQUEST_DEACTIVATE_DATA_CALL    = 2;
    static final int REQUEST_DATA_CALL_LIST          = 3;
    static final int REQUEST_GET_DATA_REG_STATE      = 4;
    static final int REQUEST_GET_QUALIFIED_NETWORKS  = 5;

    //***** Response Constants
    static final int RESPONSE_ACKNOWLEDGEMENT        = 50;

    //***** Events
    static final int EVENT_WAKE_LOCK_TIMEOUT          = 2;
    static final int EVENT_RESP_WAKE_LOCK_TIMEOUT     = 4;
    static final int EVENT_PROXY_DEAD                 = 5;
    static final int MODEM_SUPPORT_NOT_PRESENT        = 6;
    static final int DATA_REG_STATE_CHANGE_IND        = 7;
    static final int DATA_CALL_LIST_CHANGED           = 8;
    static final int QUALIFIED_NET_CHANGE_IND         = 9;
    static final int UNTHROTTLE_APN                   = 10;
    static final int SETUP_DATA_CALL_RESP             = 11;
    static final int DEACTIVATE_DATA_CALL_RESP        = 12;
    static final int GET_DATA_CALL_LIST_RESP          = 13;
    static final int GET_DATA_REG_STATE_RESP          = 14;
    static final int GET_ALL_QUAL_NET_RESP            = 15;
    static final int ACKNOWLEDGE_REQUEST              = 16;


    public static IWlanProxy getInstanceBySlotId(Context context, int slotId) {
        synchronized (instances) {
            if (instances[slotId] == null) {
                instances[slotId] = new IWlanProxy(context, slotId);
            }
            ++instanceRefCount[slotId];
            Log.d(TAG, "instanceRefCount =" + instanceRefCount[slotId]+ "for SlotId" +slotId);
        }

        return instances[slotId];
    }

    Handler.Callback handlerCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
            case MODEM_SUPPORT_NOT_PRESENT:
                android.os.Bundle modemSupport = (android.os.Bundle)msg.obj;
                mIndication.modemSupportNotPresent();
                break;

            case DATA_REG_STATE_CHANGE_IND:
                android.os.Bundle dataRegStateChange = (android.os.Bundle)msg.obj;
                mIndication.dataRegistrationStateChangeIndication();
                break;

            case DATA_CALL_LIST_CHANGED:
                android.os.Bundle dataCallListChange = (android.os.Bundle)msg.obj;
                int radioType = dataCallListChange.getInt("RADIO_TYPE");
                ArrayList<SetupDataCallResult> setupList = dataCallListChange.getParcelableArrayList("Parcelable_List");
                mIndication.dataCallListChanged(setupList);
                break;

            case QUALIFIED_NET_CHANGE_IND:
                android.os.Bundle qualNetChange = (android.os.Bundle)msg.obj;
                ArrayList<QualifiedNetworks> qualList = qualNetChange.getParcelableArrayList("Parcelable_List");
                mIndication.qualifiedNetworksChangeIndication(qualList);
                break;

            case UNTHROTTLE_APN:
                android.os.Bundle unthrottleApn = (android.os.Bundle)msg.obj;
                int radioTypeUnt = unthrottleApn.getInt("RADIO_TYPE");
                DataProfileInfo dataProf = unthrottleApn.getParcelable("ParcelableDataProf");
                mIndication.unthrottleApn(dataProf);
                break;

            case SETUP_DATA_CALL_RESP:
                android.os.Bundle setupDataCallResp = (android.os.Bundle)msg.obj;
                RadioResponseInfo dataProfSetup = setupDataCallResp.getParcelable("RADIO_Parcelable");
                SetupDataCallResult setupListSetup = setupDataCallResp.getParcelable("SetupResult_Parcelable");
                mResponse.setupDataCallResponse(dataProfSetup, setupListSetup);
                break;

            case DEACTIVATE_DATA_CALL_RESP:
                android.os.Bundle deacDataCallRes = (android.os.Bundle)msg.obj;
                IWlanResponseInfo iwlanProf = deacDataCallRes.getParcelable("ParcelableIWlanProf");
                mResponse.deactivateDataCallResponse(iwlanProf);
                break;

            case GET_DATA_CALL_LIST_RESP:
                android.os.Bundle getDataCallListResp = (android.os.Bundle)msg.obj;
                RadioResponseInfo dataProfDataCall = getDataCallListResp.getParcelable("RADIO_Parcelable");
                ArrayList<SetupDataCallResult> setupListDataCall = getDataCallListResp.getParcelableArrayList("Parcelable_List");
                mResponse.getDataCallListResponse(dataProfDataCall, setupListDataCall);
                break;

            case GET_DATA_REG_STATE_RESP:
                android.os.Bundle getDataRegStateRes = (android.os.Bundle)msg.obj;
                IWlanResponseInfo iwlanProfDataReg = getDataRegStateRes.getParcelable("ParcelableIWlanProf");
                IWlanDataRegStateResult iwlanReg = getDataRegStateRes.getParcelable("ParcelableIWlanReg");
                mResponse.getDataRegistrationStateResponse(iwlanProfDataReg, iwlanReg);
                break;

            case GET_ALL_QUAL_NET_RESP:
                android.os.Bundle getAllQualNetResp = (android.os.Bundle)msg.obj;
                IWlanResponseInfo iwlanProfQualNet = getAllQualNetResp.getParcelable("RADIO_Parcelable");
                ArrayList<QualifiedNetworks> qualListQualNet = getAllQualNetResp.getParcelableArrayList("Parcelable_List");
                mResponse.getAllQualifiedNetworksResponse(iwlanProfQualNet, qualListQualNet);
                break;

            case ACKNOWLEDGE_REQUEST:
                android.os.Bundle ackReq = (android.os.Bundle)msg.obj;
                int serial = ackReq.getInt("SERIAL");
                mResponse.acknowledgeRequest(serial);
                break;

            case EVENT_WAKE_LOCK_TIMEOUT:
                // Haven't heard back from the last request.  Assume we're
                // not getting a response and  release the wake lock.

                // The timer of WAKE_LOCK_TIMEOUT is reset with each
                // new send request. So when WAKE_LOCK_TIMEOUT occurs
                // all requests in mRequestList already waited at
                // least WAKE_LOCK_TIMEOUT_MS but no response.
                //
                // Note: Keep mRequestList so that delayed response
                // can still be handled when response finally comes.

                synchronized (mRequestList) {
                    if (msg.arg1 == mWlSequenceNum && clearWakeLock(FOR_WAKELOCK)) {
                        int count = mRequestList.size();
                        iwlanLog.w(TAG, "WAKE_LOCK_TIMEOUT " + " mRequestList = " + count);
                        for (int i = 0; i < count; i++) {
                            IWlanRequest req = mRequestList.valueAt(i);
                            iwlanLog.w(TAG, i + ": [" + req.mSerial + "] " + requestToString(req.mRequest));
                        }
                    }
                }
                break;

            case EVENT_RESP_WAKE_LOCK_TIMEOUT:
                if (msg.arg1 == mRespWlSequenceNum && clearWakeLock(FOR_RESP_WAKELOCK)) {
                    iwlanLog.w(TAG, "RESP_WAKE_LOCK_TIMEOUT");
                }
                break;

            case EVENT_PROXY_DEAD:
                iwlanLog.w(TAG, "handleMessage: EVENT_PROXY_DEAD for slotId : "+mSlotId);
                resetProxyAndRequestList();
                break;
            default:
                return false;
            }
            return true;
        }
    };

    public IWlanProxy(Context context, Integer slotId) {
        mSlotId = slotId;

        iwlanLog = new IWlanLog();
        jniAidl = JniHelper.getInstance();
        mIndication = new IWlanIndication(this);
        mResponse = new IWlanResponse(this);
        HandlerThread thread = new HandlerThread(IWlanProxy.class.getSimpleName());
        thread.start();
        mLooper = thread.getLooper();
        mHandler = new Handler(mLooper, handlerCallback);
        client = IWlanAidlClient.getInstance();
        client.setHandler(mHandler, slotId);

        if(jniAidl.isJniLibLoaded) {
            jniAidl.initialize(mSlotId);
        }


        PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, WAKELOCK_TAG);
        mWakeLock.setReferenceCounted(false);
        mRespWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, RESP_WAKELOCK_TAG);
        mRespWakeLock.setReferenceCounted(false);
        mWakeLockCount = 0;

        iwlanLog.w(TAG, "new IWlan Proxy on slot " + mSlotId);
    }

    private void resetProxyAndRequestList() {
        mService = null;
        iwlanLog.w(TAG, "reset Proxy, IIWLAN service set to null");

        // increment the cookie so that death notification can be ignored
        mServiceCookie.incrementAndGet();

        IWlanRequest req;
        synchronized (mRequestList) {
            int count = mRequestList.size();
            iwlanLog.w(TAG, "mWakeLockCount=" + mWakeLockCount + ", mRequestList = " + count);

            // Clear all wakelocks in the request list
            for (int i = 0; i < count; i++) {
                req = mRequestList.valueAt(i);
                iwlanLog.w(TAG, i + ": [" + req.mSerial + "] " + requestToString(req.mRequest));

                if (req.mResult != null) {
                    iwlanLog.w(TAG, "Sending RADIO_NOT_AVAILABLE AsyncResult");
                    AsyncResult.forMessage(req.mResult, null, new CommandException(CommandException.Error.RADIO_NOT_AVAILABLE));
                    req.mResult.sendToTarget();
                }

                decrementWakeLock(req);
            }
            mRequestList.clear();
            iwlanLog.e(TAG, "post empty DataCallListChanged indication to telephony on serviceDied");
            ArrayList<SetupDataCallResult> dcList = new ArrayList<SetupDataCallResult>();
            mDataCallListChangedRegistrants.notifyRegistrants(new AsyncResult(null, dcList, null));
        }
    }

    private String convertDpiToString(DataProfileInfo dpi) {
        StringBuilder b = new StringBuilder();
        b.append("Profile ID = " + dpi.profileId);
        b.append(", APN = " + dpi.apn);
        b.append(", Protocol = " + dpi.protocol);
        b.append(", Roaming Protocol = " + dpi.roamingProtocol);
        b.append(", Auth Type = " + dpi.authType);
        b.append(", User = " + dpi.user);
        b.append(", Passwrod = " + dpi.password);
        b.append(", Type = " + dpi.type);
        b.append(", MaxConnsTime = " + dpi.maxConnsTime);
        b.append(", MaxConns = " + dpi.maxConns);
        b.append(", WaitTime = " + dpi.waitTime);
        b.append(", Enabled = " + dpi.enabled);
        b.append(", Supported Apn Types bitmap = " + dpi.supportedApnTypesBitmap);
        b.append(", BearerBitmap = " + dpi.bearerBitmap);
        b.append(", MTUV4 = " + dpi.mtuV4);
        b.append(", MTUV6 = " + dpi.mtuV6);
        b.append(", Preferred = " + dpi.preferred);
        b.append(", Persistent = " + dpi.persistent);
        b.append(", AlwaysOn = " + dpi.alwaysOn);
        b.append(", TrafficDescriptor = " + dpi.trafficDescriptor);
        b.append(", ID_DEFAULT = " + dpi.ID_DEFAULT);
        b.append(", ID_TETHERED = " + dpi.ID_TETHERED);
        b.append(", ID_IMS = " + dpi.ID_IMS);
        b.append(", ID_FOTA = " + dpi.ID_FOTA);
        b.append(", ID_CBS = " + dpi.ID_CBS);
        b.append(", ID_OEM_BASE = " + dpi.ID_OEM_BASE);
        b.append(", ID_INVALID = " + dpi.ID_INVALID);
        b.append(", TYPE_COMMON = " + dpi.TYPE_COMMON);
        b.append(", TYPE_3GPP = " + dpi.TYPE_3GPP);
        b.append(", TYPE_3GPP2 = " + dpi.TYPE_3GPP2);
        return b.toString();
    }

    public static ArrayList<Byte> primitiveArrayToArrayList(byte[] arr) {
        ArrayList<Byte> arrayList = new ArrayList<>(arr.length);
        for (byte b : arr) {
            arrayList.add(b);
        }
        return arrayList;
    }

    private static SliceInfo convertToSliceInfo(NetworkSliceInfo sliceInfo)
    {
        SliceInfo slice = new SliceInfo();
        slice.sliceServiceType = (byte)sliceInfo.getSliceServiceType();
        slice.sliceDifferentiator = sliceInfo.getSliceDifferentiator();
        slice.mappedHplmnSst = (byte)sliceInfo.getMappedHplmnSliceServiceType();
        slice.mappedHplmnSd = sliceInfo.getMappedHplmnSliceDifferentiator();
        slice.status = (byte)sliceInfo.getStatus();
        return slice;
    }

    private static android.hardware.radio.data.TrafficDescriptor convertToAidlTrafficDescriptor(TrafficDescriptor td)
    {
        android.hardware.radio.data.TrafficDescriptor dataProfileTd = new android.hardware.radio.data.TrafficDescriptor();
        if (td.getDataNetworkName() != null)
            dataProfileTd.dnn = td.getDataNetworkName();
        if (td.getOsAppId() != null){
            dataProfileTd.osAppId = new OsAppId();
            dataProfileTd.osAppId.osAppId = td.getOsAppId();
        }
        return dataProfileTd;
    }

    public void setupDataCall(int accessNetwork, DataProfile dataProfile, boolean isRoaming,
                                boolean allowRoaming, int reason, LinkProperties linkProperties,
                                int pduId, NetworkSliceInfo sliceInfo, TrafficDescriptor trafficDescriptor,
                                boolean matchAllRuleAllowed, Message result) {

        IWlanRequest req = obtainRequest(REQUEST_SETUP_DATA_CALL, result);

        // Convert to AIDL data profile
        android.hardware.radio.data.DataProfileInfo dpi = convertToAidlDataProfile(dataProfile);
        ArrayList<android.hardware.radio.data.LinkAddress> linkAddresses = new ArrayList<>();
        ArrayList<String> dnses = new ArrayList<>();
        if (linkProperties != null) {
            for (LinkAddress address : linkProperties.getLinkAddresses()) {
                android.hardware.radio.data.LinkAddress la = new android.hardware.radio.data.LinkAddress();
                la.address = address.getAddress().getHostAddress();
                la.addressProperties = 0;
                la.deprecationTime = -1;
                la.expirationTime = -1;
                linkAddresses.add(la);
            }
            for (InetAddress dns : linkProperties.getDnsServers()) {
                dnses.add(dns.getHostAddress());
            }
        }
        SliceInfo slice = null;
        if (sliceInfo != null) {
            slice = convertToSliceInfo(sliceInfo);
        }
        dpi.trafficDescriptor = new android.hardware.radio.data.TrafficDescriptor();
        if (trafficDescriptor != null) {
            dpi.trafficDescriptor = convertToAidlTrafficDescriptor(trafficDescriptor);
        }
        android.hardware.radio.data.LinkAddress[] passAddress = linkAddresses.toArray(new android.hardware.radio.data.LinkAddress[linkAddresses.size()]);
        String[] passDns = dnses.toArray(new String[dnses.size()]);

        iwlanLog.v(IWlanDataService.TAG, req.mSerial + " > " + requestToString(req.mRequest) + ", reason = " + reason +
                    ", dpi: " + convertDpiToString(dpi) + ", addresses = [" + TextUtils.join(",", linkAddresses) + "]" +
                    ", dnses = [" + TextUtils.join(",", dnses) + "], traffic descriptor = " + trafficDescriptor.toString());

        if (jniAidl != null) {
            Parcel parcelData = Parcel.obtain();
            dpi.writeToParcel(parcelData, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
            parcelData.setDataPosition(0);
            Parcel parcelSlice = Parcel.obtain();
            if (slice != null) {
                slice.writeToParcel(parcelSlice, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
                parcelSlice.setDataPosition(0);
            }
            Parcel[] parcelAdd = new Parcel[passAddress.length];
            for(int i =0; i< passAddress.length; i++) {
                android.os.Parcel parcel= Parcel.obtain();
                passAddress[i].writeToParcel(parcel, Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
                parcelAdd[i] = parcel;
                parcelAdd[i].setDataPosition(0);
            }
            jniAidl.setupDataCall(mSlotId, req.mSerial, accessNetwork, parcelData, allowRoaming, reason, parcelAdd, passDns, pduId,parcelSlice, matchAllRuleAllowed);
        }
    }

    public void deactivateDataCall(int cid, int reason, Message result)
    {
        IWlanRequest req = obtainRequest(REQUEST_DEACTIVATE_DATA_CALL, result);
        if (jniAidl != null) {
            jniAidl.deactivateDataCall(mSlotId,req.mSerial, cid, reason);
        }
    }

    public void getDataCallList(Message result)
    {
        IWlanRequest req = obtainRequest(REQUEST_DATA_CALL_LIST, result);
        if (jniAidl != null) {
            jniAidl.getDataCallList(mSlotId, req.mSerial);
        }
    }

    public void getDataRegistrationState(Message result)
    {
        IWlanRequest req = obtainRequest(REQUEST_GET_DATA_REG_STATE, result);
        if (jniAidl != null) {
            jniAidl.getDataRegistrationState(mSlotId, req.mSerial);
        }
    }

    public void getAllQualifiedNetworks(Message result)
    {
        IWlanRequest req = obtainRequest(REQUEST_GET_QUALIFIED_NETWORKS, result);
        if (jniAidl != null) {
            jniAidl.getAllQualifiedNetworks(mSlotId, req.mSerial);
        }
    }

    private IWlanRequest obtainRequest(int request, Message result) {
        IWlanRequest req = new IWlanRequest(request, result);

        acquireWakeLock(req, FOR_WAKELOCK);
        synchronized (mRequestList) {
            mRequestList.append(req.mSerial, req);
        }

        return req;
    }

    private IWlanRequest findAndRemoveRequestFromList(int serial) {
        IWlanRequest req = null;
        synchronized (mRequestList) {
            req = mRequestList.get(serial);
            if (req != null) {
                mRequestList.remove(serial);
            }
        }

        return req;
    }

    private boolean responseAcknowledgement(){
        boolean ret = false;
        if (jniAidl != null) {
            jniAidl.responseAcknowledgement(mSlotId);
            ret = true;
        }
        return ret;
    }

    /**
     * Function to send ack and acquire related wakelock
     */
    public void sendAck(String tag) {
        IWlanRequest temp_req = new IWlanRequest(RESPONSE_ACKNOWLEDGEMENT, null);
        acquireWakeLock(temp_req, FOR_RESP_WAKELOCK);
        responseAcknowledgement();
    }

    /**
     * Sends explicit message to modem that iWlan should be disabled.
     * Disables IWlan when the first service exits.
     */
    public static void disableIWlan(IWlanProxy proxy) {
        int slotId = proxy.mSlotId;
        synchronized (instances) {
            if (instances[slotId] != null) {

                --instanceRefCount[slotId];
                if (instanceRefCount[slotId] == 0 ) {
                    Log.d(TAG, "instanceRefCount is 0");
                    // If the service for this slot doesn't exist we don't need to disable
                    if (proxy.mService != null) {
                        try{
                            proxy.mService.iwlanDisabled();
                        }catch(RemoteException | RuntimeException e){
                            Log.e(TAG, "disableIWlan: " + e);
                        }
                    }

                    instances[slotId] = null;
                } else if ( instanceRefCount[slotId] < 0 ) {
                    Log.e(TAG, "Error: instanceRefCount =" + instanceRefCount[slotId]+ " < 0 for SlotId" +slotId);
                }
            } else {
                instanceRefCount[slotId] = 0;
                Log.w(TAG, "instances["+slotId+"] is null, setting instanceRefCount to zero");
            }
        }
    }

    /**
     * This is a helper function to be called when a IWlanServiceResponse callback is called.
     * It takes care of acks, wakelocks, and finds and returns IWlanRequest corresponding to the
     * response if one is found.
     * @param responseInfo IWlanResponseInfo received in response callback
     * @return IWlanRequest corresponding to the response
     */

    public IWlanRequest processResponse(IWlanResponseInfo responseInfo, boolean sendAck, String tag) {
        int serial = responseInfo.serial;
        int error = responseInfo.error;
        IWlanRequest req = findAndRemoveRequestFromList(serial);
        if (req == null) {
            iwlanLog.e(tag, serial + " > processResponse: Unexpected response! error: " + error);
            return null;
        } else {
            iwlanLog.w(tag, req.mSerial + " > Response Processed");
            if(sendAck) {
                iwlanLog.w(tag, req.mSerial + " > send ACK for serial");
                sendAck(tag);
            }
        }
        return req;
    }

    void processRequestAck(int serial) {
        IWlanRequest req;
        synchronized (mRequestList) {
            req = mRequestList.get(serial);
        }
        if (req == null) {
            iwlanLog.e(TAG, "processRequestAck: request not found for serial = " + serial);
        } else {
            decrementWakeLock(req);
            iwlanLog.w(TAG, req.mSerial + " Recieved ACK for serial = " + serial);
        }
    }

    /**
     * Holds a PARTIAL_WAKE_LOCK whenever
     * a) There is outstanding RIL request sent to RIL deamon and no replied
     * b) There is a request pending to be sent out.
     *
     * There is a WAKE_LOCK_TIMEOUT to release the lock, though it shouldn't
     * happen often.
     */
    private void acquireWakeLock(IWlanRequest req, int wakeLockType) {
        synchronized (req) {
            if (req.mWakeLockType != INVALID_WAKELOCK) {
                iwlanLog.w(TAG, "Failed to aquire wakelock for " + req.mSerial);
                return;
            }

            switch(wakeLockType) {
                case FOR_WAKELOCK:
                    synchronized (mWakeLock) {
                        mWakeLock.acquire();
                        mWakeLockCount++;
                        mWlSequenceNum++;

                        try {
                            Message msg = mHandler.obtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
                            msg.arg1 = mWlSequenceNum;
                            mHandler.sendMessageDelayed(msg, WAKE_LOCK_TIMEOUT_MS);
                        } catch (IllegalStateException e) {
                            /*
                            *  Handler thread might not exist if bound service crashed.
                            *  If Unable to send msg, release the wakelock
                            */
                            mWakeLock.release();
                            mWakeLockCount--;
                            mWlSequenceNum--;
                        }
                    }
                    break;
                case FOR_RESP_WAKELOCK:
                    synchronized (mRespWakeLock) {
                        mRespWakeLock.acquire();
                        mRespWlSequenceNum++;

                        try {
                            Message msg = mHandler.obtainMessage(EVENT_RESP_WAKE_LOCK_TIMEOUT);
                            msg.arg1 = mRespWlSequenceNum;
                            mHandler.sendMessageDelayed(msg, RESP_WAKE_LOCK_TIMEOUT_MS);
                        } catch (IllegalStateException e) {
                            /*
                            *  Handler thread might not exist if bound service crashed.
                            *  If Unable to send msg, release the wakelock
                            */
                            mWakeLock.release();
                            mRespWlSequenceNum--;
                        }
                    }
                    break;
                default:
                    iwlanLog.w(TAG, "Acquiring Invalid Wakelock type " + wakeLockType);
                    return;
            }
            req.mWakeLockType = wakeLockType;
        }
    }

    public void decrementWakeLock(IWlanRequest req) {
        synchronized (req) {
            switch(req.mWakeLockType) {
                case FOR_WAKELOCK:
                    synchronized (mWakeLock) {
                        if (mWakeLockCount > 1) {
                            mWakeLockCount--;
                        } else {
                            mWakeLockCount = 0;
                            mWakeLock.release();
                        }
                    }
                    break;
                case FOR_RESP_WAKELOCK:
                    //We do not decrement the ACK wakelock, we always wait for timeout
                    break;
                case INVALID_WAKELOCK:
                    break;
                default:
                    iwlanLog.w(TAG, "Decrementing Invalid Wakelock type " + req.mWakeLockType);
            }
            req.mWakeLockType = INVALID_WAKELOCK;
        }
    }

    private boolean clearWakeLock(int wakeLockType) {
        if (wakeLockType == FOR_WAKELOCK) {
            synchronized (mWakeLock) {
                if (mWakeLockCount == 0 && !mWakeLock.isHeld()) return false;
                iwlanLog.w(TAG, "NOTE: mWakeLockCount is " + mWakeLockCount + "at time of clearing");
                mWakeLockCount = 0;
                mWakeLock.release();
                return true;
            }
        } else {
            synchronized (mRespWakeLock) {
                if (!mRespWakeLock.isHeld()) return false;
                mRespWakeLock.release();
                return true;
            }
        }
    }

    /**
     * Convert to DataProfileInfo defined in types.hal
     * @param dp Data profile
     * @return A converted data profile
     */
    private static DataProfileInfo convertToAidlDataProfile(DataProfile dp) {
        DataProfileInfo dpi = new DataProfileInfo();

        dpi.profileId = dp.getProfileId();
        dpi.apn = dp.getApn();
        dpi.protocol = dp.getProtocolType();
        dpi.roamingProtocol = dp.getRoamingProtocolType();
        dpi.authType = dp.getAuthType();
        dpi.user = dp.getUserName() != null ? dp.getUserName() : "";
        dpi.password = dp.getPassword() != null ? dp.getPassword() : "";
        dpi.type = dp.getType();
        //dpi.maxConnsTime = dp.getMaxConnectionsTime();
        //dpi.maxConns = dp.getMaxConnections();
        //dpi.waitTime = dp.getWaitTime();
        dpi.enabled = dp.isEnabled();
        dpi.supportedApnTypesBitmap = dp.getSupportedApnTypesBitmask();
        dpi.bearerBitmap = dp.getBearerBitmask();
        dpi.mtuV4 = dp.getMtuV4();
        dpi.mtuV6 = dp.getMtuV6();
        dpi.preferred = dp.isPreferred();
        dpi.persistent = dp.isPersistent();
        return dpi;
    }

    static String requestToString(int request) {
        switch(request) {
            case REQUEST_SETUP_DATA_CALL:
                return "REQUEST_SETUP_DATA_CALL";
            case REQUEST_DEACTIVATE_DATA_CALL:
                return "REQUEST_DEACTIVATE_DATA_CALL";
            case REQUEST_DATA_CALL_LIST:
                return "REQUEST_DATA_CALL_LIST";
            case REQUEST_GET_DATA_REG_STATE:
                return "REQUEST_GET_DATA_REG_STATE";
            case REQUEST_GET_QUALIFIED_NETWORKS:
                return "REQUEST_GET_QUALIFIED_NETWORKS";
            default: return "<unknown request>";
        }
    }

    /********** INDICATION FUNCTIONS ****************/
    protected RegistrantList mDataCallListChangedRegistrants = new RegistrantList();
    protected RegistrantList mDataRegistrationStateChangedRegistrants = new RegistrantList();
    protected RegistrantList mQualifiedNetworksChangedRegistrants = new RegistrantList();
    protected RegistrantList mApnUnthrottledRegistrants = new RegistrantList();

    public void modemSupportNotPresent() {
        //Currently not used
        iwlanModemSupport = false;
    }

    public void registerForDataCallListChanged(Handler h, int what) {
        if (h != null) {
            mDataCallListChangedRegistrants.addUnique(h, what, null);
        }
    }

    public void unregisterForDataCallListChanged(Handler h) {
        if (h != null) {
            mDataCallListChangedRegistrants.remove(h);
        }
    }

    public void registerForDataRegistrationStateChanged(Handler h, int what) {
        if (h != null) {
            mDataRegistrationStateChangedRegistrants.addUnique(h, what, null);
        }
    }

    public void unregisterForDataRegistrationStateChanged(Handler h) {
        if (h != null) {
            mDataRegistrationStateChangedRegistrants.remove(h);
        }
    }

    public void registerForQualifiedNetworksChanged(Handler h, int what) {
        if (h != null) {
            mQualifiedNetworksChangedRegistrants.addUnique(h, what, null);
        }
    }

    public void unregisterForQualifiedNetworksChanged(Handler h) {
        if (h != null) {
            mQualifiedNetworksChangedRegistrants.remove(h);
        }
    }

    public void registerForApnUnthrottled(Handler h, int what) {
        if (h != null) {
            mApnUnthrottledRegistrants.addUnique(h, what, null);
        }
    }

    public void unregisterForApnUnthrottled(Handler h) {
        if (h != null) {
            mApnUnthrottledRegistrants.remove(h);
        }
    }
}
