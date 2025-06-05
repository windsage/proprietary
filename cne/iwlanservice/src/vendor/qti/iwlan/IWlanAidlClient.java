/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package vendor.qti.iwlan;

import android.os.IBinder;
import android.os.Parcelable;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import java.util.Arrays;
import android.os.Parcel;

import android.hardware.radio.data.DataProfileInfo;
import android.hardware.radio.data.SetupDataCallResult;
import android.hardware.radio.RadioResponseInfo;
import vendor.qti.hardware.data.iwlandata.QualifiedNetworks;
import vendor.qti.hardware.data.iwlandata.IWlanResponseInfo;
import vendor.qti.hardware.data.iwlandata.IWlanDataRegStateResult;

public class IWlanAidlClient{
    public static Handler[] mConnectionHandler = {null, null};
    private static final String LOGTAG = "IWlanAidlClient";
    private static IWlanAidlClient aidlClient = null;
    private IWlanAidlClient() {}

    public static IWlanAidlClient getInstance() {
        if(aidlClient == null)
        {
            aidlClient = new IWlanAidlClient();
        }
        return aidlClient;
    }

    public void acknowledgeRequest(int serial, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "acknowledgeRequest called");
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.ACKNOWLEDGE_REQUEST;
        android.os.Bundle b = new android.os.Bundle();
        b.putInt("SERIAL", serial);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void dataRegistrationStateChangeIndication(int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "dataRegistrationStateChangeIndication called");
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.DATA_REG_STATE_CHANGE_IND;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void setupDataCallResponse(Parcel info, Parcel dcResult, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "setupDataCallResponse called");
        RadioResponseInfo responseInfo = new RadioResponseInfo();
        responseInfo.readFromParcel(info);
        SetupDataCallResult setupRes = new SetupDataCallResult();
        setupRes.readFromParcel(dcResult);
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.SETUP_DATA_CALL_RESP;
        android.os.Bundle b = new android.os.Bundle();
        b.putParcelable("RADIO_Parcelable", responseInfo);
        b.putParcelable("SetupResult_Parcelable", setupRes);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void getDataCallListResponse(Parcel info, Parcel[] dcResult, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "getDataCallListResponse called");
        RadioResponseInfo resInfo = new RadioResponseInfo();
        resInfo.readFromParcel(info);
        ArrayList<SetupDataCallResult> res = new ArrayList<>();
        for(int i=0; i<dcResult.length; i++)
        {
            SetupDataCallResult dataCallRes = new SetupDataCallResult();
            dataCallRes.readFromParcel(dcResult[i]);
            res.add(dataCallRes);
        }
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.GET_DATA_CALL_LIST_RESP;
        android.os.Bundle b = new android.os.Bundle();
        b.putParcelable("RADIO_Parcelable", resInfo);
        b.putParcelableArrayList("Parcelable_List", res);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void dataCallListChanged(int type, Parcel[] dcResult, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "dataCallListChanged called");
        ArrayList<SetupDataCallResult> res = new ArrayList<>();
        for(int i=0; i<dcResult.length; i++)
        {
            SetupDataCallResult dataCallRes = new SetupDataCallResult();
            dataCallRes.readFromParcel(dcResult[i]);
            res.add(dataCallRes);
        }
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.DATA_CALL_LIST_CHANGED;
        android.os.Bundle b = new android.os.Bundle();
        b.putInt("RADIO_TYPE", type);
        b.putParcelableArrayList("Parcelable_List", res);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void getAllQualifiedNetworksResponse(Parcel info, Parcel[] in_qualifiedNetworksList, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "getAllQualifiedNetworksResponse called");
        IWlanResponseInfo respInfo = new IWlanResponseInfo();
        respInfo.readFromParcel(info);
        ArrayList<QualifiedNetworks> res = new ArrayList<>();
        for(int i=0; i<in_qualifiedNetworksList.length; i++)
        {
            QualifiedNetworks qualNetwork = new QualifiedNetworks();
            qualNetwork.readFromParcel(in_qualifiedNetworksList[i]);
            res.add(qualNetwork);
        }
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.GET_ALL_QUAL_NET_RESP;
        android.os.Bundle b = new android.os.Bundle();
        b.putParcelable("RADIO_Parcelable", respInfo);
        b.putParcelableArrayList("Parcelable_List", res);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void qualifiedNetworksChangeIndication(Parcel[] in_qualifiedNetworksList, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "qualifiedNetworksChangeIndication called");
        ArrayList<QualifiedNetworks> res = new ArrayList<>();
        for(int i=0; i<in_qualifiedNetworksList.length; i++)
        {
            QualifiedNetworks qualNetwork = new QualifiedNetworks();
            qualNetwork.readFromParcel(in_qualifiedNetworksList[i]);
            res.add(qualNetwork);
        }
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.QUALIFIED_NET_CHANGE_IND;
        android.os.Bundle b = new android.os.Bundle();
        b.putParcelableArrayList("Parcelable_List", res);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void unthrottleApn(int type, Parcel in_dataProfileInfo, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "unthrottleApn called");
        DataProfileInfo dpi = new DataProfileInfo();
        dpi.readFromParcel(in_dataProfileInfo);
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.UNTHROTTLE_APN;
        android.os.Bundle b = new android.os.Bundle();
        b.putInt("RADIO_TYPE", type);
        b.putParcelable("ParcelableDataProf", dpi);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void modemSupportNotPresent(int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "modemSupportNotPresent called");
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.MODEM_SUPPORT_NOT_PRESENT;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void deactivateDataCallResponse(Parcel respParcel, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        IWlanResponseInfo responseInfo = new IWlanResponseInfo();
        responseInfo.readFromParcel(respParcel);
        Log.i(LOGTAG, "deactivateDataCallResponse called");
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.DEACTIVATE_DATA_CALL_RESP;
        android.os.Bundle b = new android.os.Bundle();
        b.putParcelable("ParcelableIWlanProf", responseInfo);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void getDataRegistrationStateResponse(Parcel responseInfo, Parcel regStateResult, int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "getDataRegistrationStateResponse called");
        IWlanResponseInfo respInfo = new IWlanResponseInfo();
        respInfo.readFromParcel(responseInfo);
        IWlanDataRegStateResult regState = new IWlanDataRegStateResult();
        regState.readFromParcel(regStateResult);
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.GET_DATA_REG_STATE_RESP;
        android.os.Bundle b = new android.os.Bundle();
        b.putParcelable("ParcelableIWlanProf", respInfo);
        b.putParcelable("ParcelableIWlanReg", regState);
        msg.obj = b;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void aidlServiceDied(int slotId)
    {
        if(mConnectionHandler[slotId] == null)
        {
            Log.e(LOGTAG, "handler is NULL");
            return;
        }
        Log.i(LOGTAG, "aidlserviceDied called");
        Message msg = mConnectionHandler[slotId].obtainMessage();
        msg.what = IWlanProxy.EVENT_PROXY_DEAD;
        mConnectionHandler[slotId].sendMessage(msg);
    }

    public void setHandler(Handler h, int slotId) {
        mConnectionHandler[slotId] = h;
    }
}
