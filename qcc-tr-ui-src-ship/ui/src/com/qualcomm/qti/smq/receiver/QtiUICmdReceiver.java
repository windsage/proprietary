/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.smq.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.text.TextUtils;
import android.util.Log;

import com.qualcomm.qti.smq.app.QtiUIApplicationManager;

import com.qualcomm.qti.smq.util.OptInOutHandler;
import com.qualcomm.qti.smq.util.OptInOutProductConstants;
import com.qualcomm.qti.smq.util.QtiSendCmdData;
import com.qualcomm.qti.smq.util.QtiUIConstants;

import java.io.File;
import java.io.FileOutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Formatter;
import java.util.List;

public class QtiUICmdReceiver extends BroadcastReceiver {
    private static final String LOG_TAG = "QCC-UI-CMD";
    private static final int _OPT_OUT = 1;
    private static final int _OPT_IN = 2;

    @Override
    public void onReceive(Context context, Intent intent) {
        String strAction = intent.getAction();
        if (strAction == null) {
            return;
        }
        Log.d(LOG_TAG, "Received action=" + strAction);
        QtiUIApplicationManager mAppmgr = QtiUIApplicationManager.getInstance();
        QtiSendCmdData cmdData;
        String isv_name = intent.getStringExtra("ISV_NAME");
        String opt_type = intent.getStringExtra("OPTIN_TYPE");
        String start = intent.getStringExtra("START_CMD");
        String isv_id = intent.getStringExtra("ISV_ID");
        String collectorList = intent.getStringExtra("COLLECT_LIST");
        String collectorIds = intent.getStringExtra("COLLECT_ID");
        String licenseFileName = intent.getStringExtra("FILE_NAME");
        switch (strAction) {
            case QtiUIConstants.INTENT_SMQUI_COLLECTOR_START:
                if((isv_id == null)&&(start == null)){
                    return;
                }
                Log.d(LOG_TAG, "COLLECTOR_START - isv_id: " + isv_id+", start: "+start);
                cmdData = new QtiSendCmdData(isv_id, QtiUIConstants.COLLECTOR_START_CMD,
                        start, QtiUIConstants.SEND_DB_CMD);
                mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_DB_CMD);
                break;
            case QtiUIConstants.INTENT_SMQUI_OPTOUT:
                if((isv_name == null) || (collectorList == null))
                    return;
                Log.d(LOG_TAG, "OPTOUT - isv_name: " + isv_name+", collectorList: "+collectorList);
                cmdData = new QtiSendCmdData(isv_name, QtiUIConstants.OPTOUT_CMD,
                        collectorList, QtiUIConstants.SEND_DB_CMD);
                mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_DB_CMD);
                break;
            case QtiUIConstants.INTENT_SMQUI_LICENSE_TEST:
                int nOptIn, nOptInType;
                byte[] sigHash = null;
                Log.d(LOG_TAG, "isv_name: " + isv_name+"licenseFileName: " + licenseFileName);
                if(licenseFileName == null){
                    Log.e(LOG_TAG, "licenseFileName: " + licenseFileName+" not present");
                    return;
                }

                if((opt_type != null)&&(opt_type.matches("\\d+"))){
                    nOptInType = Integer.parseInt(opt_type);
                }else{
                    nOptInType = 1;
                }

                String packageName = context.getPackageName();
                Log.i(LOG_TAG,"sendLicenseData, packageName : "+packageName+" , list : "+collectorList);
                sigHash =mAppmgr.getSignatureSha256(context, packageName);

                if(mAppmgr.existDefaultLicense(licenseFileName)&&(sigHash != null)){
                    cmdData = new QtiSendCmdData(context, licenseFileName, isv_name, "appInfo",
                            collectorList, sigHash, sigHash.length, 1, QtiUIConstants.SEND_LICENSE_CMD);
                    if(cmdData.getLicenseLen() > 0){
                        mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_LICENSE_CMD);
                    }else{
                        Log.e(LOG_TAG, "licenseFile read error ");
                    }
                }else{
                    String QccUIFileDir = mAppmgr.getFilesDir().getAbsolutePath();
                    File pfmLicenseFile = new File(QtiUIApplicationManager.getContext().getFilesDir().getAbsolutePath(), licenseFileName);
                    if(pfmLicenseFile.exists()&&(sigHash != null)){
                        cmdData = new QtiSendCmdData(pfmLicenseFile, isv_name, "appInfo", collectorList,
                                sigHash, sigHash.length, nOptInType, QtiUIConstants.SEND_LICENSE_CMD);
                        if(cmdData.getLicenseLen() > 0){
                            mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_LICENSE_CMD);
                        }else{
                            Log.e(LOG_TAG, "licenseFile read error ");
                        }
                    }
                }
                break;

            case QtiUIConstants.INTENT_SMQUI_KILL_ALLCOLLECTORS:
                if(isv_name == null)
                    return;
                Log.d(LOG_TAG, "kill all Collectors - isv_name: " + isv_name);
                cmdData = new QtiSendCmdData(isv_name, QtiUIConstants.KILL_ALL_CMD,
                        null, QtiUIConstants.SEND_DB_CMD);
                mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_DB_CMD);
                break;

            case QtiUIConstants.INTENT_SMQUI_KILL_COLLECTOR:
                if((isv_name == null) || (collectorList == null))
                    return;
                Log.i(LOG_TAG,"kill collector, isv_name : "+isv_name+" , collector : "+collectorList);
                cmdData = new QtiSendCmdData(isv_name, QtiUIConstants.KILL_COLLECTOR_CMD,
                        collectorList, QtiUIConstants.SEND_DB_CMD);
                mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_DB_CMD);
                break;

            case QtiUIConstants.INTENT_SMQUI_TURNON_COLLECTOR:
                if((isv_name == null) || (collectorList == null))
                    return;
                Log.i(LOG_TAG,"turnon collector, isv_name : "+isv_name+" , collector : "+collectorList);
                cmdData = new QtiSendCmdData(isv_name, QtiUIConstants.TURNON_COLLECTOR_CMD,
                        collectorList, QtiUIConstants.SEND_DB_CMD);
                mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_DB_CMD);
                break;

            case QtiUIConstants.INTENT_SNQUI_COLLECTOR_INFO:
                Log.i(LOG_TAG,"Collector info: "+collectorIds);
                cmdData = new QtiSendCmdData(isv_name, QtiUIConstants.COLLECTOR_INFO_CMD,
                        collectorIds, QtiUIConstants.SEND_DB_CMD);
                mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_DB_CMD);
                break;

            case QtiUIConstants.INTENT_SNQUI_GLB_OPT:
                Log.i(LOG_TAG,"GLB_OPT: "+opt_type);
                if((opt_type != null)&&(opt_type.matches("\\d+"))){
                    int optType = Integer.parseInt(opt_type);
                    if(optType == 2){
                        //global optin
                        OptInOutHandler dbHandler = new OptInOutHandler(QtiUIApplicationManager.getContext());
                        dbHandler.changeSettingsAsync(OptInOutProductConstants.QTR_KEY_DATA_PREFERENCE, true);
                    }else if (optType == 1){
                        //global optout
                        OptInOutHandler dbHandler = new OptInOutHandler(QtiUIApplicationManager.getContext());
                        dbHandler.changeSettingsAsync(OptInOutProductConstants.QTR_KEY_DATA_PREFERENCE, false);
                    }else{
                        Log.d(LOG_TAG,"GLB_OPT - invalid optValue");
                    }
                }else{
                    Log.d(LOG_TAG,"GLB_OPT - invalid optedType");
                }
                break;

            default:
                break;
        }
    }
}
