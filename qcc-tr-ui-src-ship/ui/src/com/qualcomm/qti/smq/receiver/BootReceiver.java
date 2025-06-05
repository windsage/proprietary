/*
 * Copyright (c) 2018,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.smq.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Build;
import android.util.Log;
import com.qualcomm.qti.smq.app.QtiUIApplicationManager;
import com.qualcomm.qti.smq.util.QtiSendCmdData;
import com.qualcomm.qti.smq.util.QtiUIConstants;
import com.qualcomm.qti.smq.ui.util.SMQUiUtils;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.Formatter;


public class BootReceiver extends BroadcastReceiver {
    /** The Constant LOG_TAG. */
    private static final String LOG_TAG = "QCC-TR-UI";
    private QtiNotificationReceiver mQtiNotificationReceiver;
    private final String defaultCollectorList = "MICRODUMP";
    private static String mSharedPreferences = "SharedPreferences";
    private static final String checkFirstBoot = "firstBoot";

    @Override
    public void onReceive(Context context, Intent intent) {

        if (! SMQUiUtils.isVendorEnhancedFramework())
            return;

        Log.d(LOG_TAG,"BootReceiver");
        if(intent == null || intent.getAction() == null) {
            return;
        }
        String action = intent.getAction();
        if( Intent.ACTION_BOOT_COMPLETED.equals(action)
                || Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(action))
        {
            Log.d(LOG_TAG, action + " intent received");

            //registerReceiver
            if(Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(action)){
                QtiUIApplicationManager mAppmgr = QtiUIApplicationManager.getInstance();
                String manufacturer = Build.MANUFACTURER;
                String model = Build.MODEL;
                SharedPreferences firstBootCheck = context.createDeviceProtectedStorageContext()
                        .getSharedPreferences(mSharedPreferences, Context.MODE_PRIVATE );
                boolean firstBoot = firstBootCheck.getBoolean(checkFirstBoot, true);
                Log.d(LOG_TAG, "bootReceiver : registerReceiver for QtiNotificationRecv");
                mQtiNotificationReceiver = new QtiNotificationReceiver();
                context.getApplicationContext().registerReceiver(mQtiNotificationReceiver, new IntentFilter(QtiNotificationReceiver.QCC_UI_NOTI_ACTION), Context.RECEIVER_EXPORTED);
                Log.d(LOG_TAG,"QtiFeedbackActivity manufacturer: "+manufacturer+", model: "+model+", firstBoot: "+firstBoot);
                if(firstBoot){
                    if(manufacturer!=null && manufacturer.contains(QtiUIApplicationManager.sManufacturer)
                            && model != null && model.contains(QtiUIApplicationManager.sModel)) {
                        //enable default uploads and install license
                        mAppmgr.sendLicenseData(QtiUIApplicationManager.getContext(), defaultCollectorList);
                    }else{
                        //send registration only
                        mAppmgr.sendLicenseData(QtiUIApplicationManager.getContext(), null);
                    }
                    SharedPreferences.Editor editor = firstBootCheck.edit();
                    editor.putBoolean(checkFirstBoot, false);
                    boolean ret = editor.commit();
                    Log.i(LOG_TAG, "stored current checkFisrtBoot - done commit: "+ret);
                }
            }
        }
    }
}
