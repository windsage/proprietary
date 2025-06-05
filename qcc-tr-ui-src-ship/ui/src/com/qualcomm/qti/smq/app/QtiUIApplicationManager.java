/*************************************************************************
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *************************************************************************/

package com.qualcomm.qti.smq.app;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;

import com.qualcomm.qti.qdma.authmgr.isvInterface.IIsvStateCallback;
import com.qualcomm.qti.qdma.authmgrcmdlib.CmdDeliverImpl;
import com.qualcomm.qti.qdma.authmgrcmdlib.cmdinterface.CmdDeliver;
import com.qualcomm.qti.qdma.authmgrcmdlib.cmdinterface.onQccAuthMgrServiceConnectedCB;
import com.qualcomm.qti.smq.receiver.QtiUICmdReceiver;
import com.qualcomm.qti.smq.util.QtiSendCmdData;
import com.qualcomm.qti.smq.util.QtiUIConstants;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.Formatter;

public class QtiUIApplicationManager extends Application {
    private static final String LOG_TAG = "qcc:QccUiApp";

    /** The application context. */
    private static Context mApplicationContext = null;
    private static QtiUICmdReceiver uiCmdReceiver = null;
    private static CmdDeliver deliverInstance = null;
    private static QtiSendCmdData cmdData;
    private static final String OPTIN_CMD = "OPTIN_CMD";
    private static final String OPTOUT_CMD = "OPTOUT_CMD";
    private static final String QCC_COMPONMENT = "com.qti.qcc";
    private static final String QCC_SERVICE_PATH = "com.qualcomm.qti.qdma.authmgr.services.IsvService";
    public static final int CMD_OK = 0;
    public static final int CMD_LICENSE_CHECK_FAILED = -1;
    private final byte[] dummyLicense ={0x64, 0x75, 0x6D, 0x6D, 0x79, 0x4C, 0x69, 0x63, 0x65, 0x6E, 0x73, 0x65}; //dummyLicense
    //private qtiAppHandler appHandler;
    private static boolean isCmdReceiverRegistered;
    public static final String sManufacturer = "QUALCOMM";
    public static final String sModel = "for arm";
    public static final boolean IS_DEBUG_BUILD =
            Build.TYPE.equals("eng") || Build.TYPE.equals("userdebug");

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(LOG_TAG,"onCreate()");
        mApplicationContext = getApplicationContext();
        String manufacturer = Build.MANUFACTURER;
        String model = Build.MODEL;
        if(manufacturer!=null && manufacturer.contains(sManufacturer)
                && model != null && model.contains(sModel)) {
            Log.i(LOG_TAG,"debug mode");
            prepareUICmd(mApplicationContext);
            startQtiUICmdReceiver();
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        if(deliverInstance != null)
        {
            deliverInstance.release();
        }
    }

    public static QtiUIApplicationManager getInstance()
    {
        return (QtiUIApplicationManager) mApplicationContext.getApplicationContext();
    }

    public static Context getContext()
    {
        return mApplicationContext;
    }

    public void sendCmd(Object obj, int type){
        Log.d(LOG_TAG,"sendCmd");
        String packageName = getInstance().getPackageName();
        cmdData = (QtiSendCmdData)obj;
        if(deliverInstance != null)
        {
            deliverInstance.release();
        }

        deliverInstance = CmdDeliverImpl.getInstance();
        deliverInstance.init(mApplicationContext, type, packageName, QCC_COMPONMENT,
                QCC_SERVICE_PATH, new onQccAuthMgrServiceConnectedCB(){
            @Override
            public void onServiceConnected(boolean b, int type) {
                Log.d(LOG_TAG,"onServiceConnected :"+b);
                if(b){
                    Log.d(LOG_TAG,"onServiceConnected type : "+ type);
                    if(type == QtiUIConstants.SEND_LICENSE_CMD){
                        deliverInstance.setLicense(cmdData.getPkgName(), cmdData.getAppInfo(), cmdData.getCollectorList(),
                                cmdData.getLicenseByte(), cmdData.getLicenseLen(),
                                cmdData.getIsvHash(), cmdData.getIsvHashLen(), cmdData.getOptInType());
                    } else  if(type == QtiUIConstants.SEND_DB_CMD){
                        deliverInstance.setMessage(cmdData.getPkgName(), cmdData.getCommand(), cmdData.getValue());
                    }
                }
            }
        });
        deliverInstance.setStateCallback(new IIsvStateCallback.Stub() {
            @Override
            public void onIsvStateChanged(String cmdType, int status) throws RemoteException {
                Log.d(LOG_TAG,"onIsvStateChanged cmdType: "+cmdType+", status: "+status);
				deliverInstance.release();
                deliverInstance = null;
                if(cmdType.equals(OPTIN_CMD)){
                    if(status == CMD_OK){
                        Log.d(LOG_TAG,"license validation done, opted-in done successfully");
                    }else{
                        Log.d(LOG_TAG,"license validation failed");
                    }
                }else if(cmdType.equals(OPTOUT_CMD)){
                    if(status == CMD_OK){
                        Log.d(LOG_TAG,"opted-out done successfully");
                    }else{
                        Log.d(LOG_TAG,"opted-out cmd failed");
                    }
                }else if(cmdType.equals(QtiUIConstants.KILL_ALL_CMD)){
                    if(status == CMD_OK){
                        Log.d(LOG_TAG,"killed all collectors done successfully");
                    }else{
                        Log.d(LOG_TAG,"killed all collectors failed");
                    }
                }else if(cmdType.equals(QtiUIConstants.KILL_COLLECTOR_CMD)){
                    if(status == CMD_OK){
                        Log.d(LOG_TAG,"killed collector done successfully");
                    }else{
                        Log.d(LOG_TAG,"killed collector failed");
                    }
                }else if(cmdType.equals(QtiUIConstants.TURNON_COLLECTOR_CMD)){
                    if(status == CMD_OK){
                        Log.d(LOG_TAG,"turnon collector done successfully");
                    }else{
                        Log.d(LOG_TAG,"turnon collector failed");
                    }
                }else{
                    Log.e(LOG_TAG,"undefined cmd done");
                }
            }
        });
    }

    public byte[] getSignatureSha256(Context context, String packageName){
        byte[] signature = null;
        try{
            //int i =0;
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(packageName, PackageManager.GET_SIGNING_CERTIFICATES);
            Signature[] signatures = packageInfo.signingInfo.getApkContentsSigners();
            MessageDigest sigDigest = MessageDigest.getInstance("SHA256");
            sigDigest.update(signatures[0].toByteArray());
            signature = sigDigest.digest();
            Formatter formatter = new Formatter(); // for debugging
            for(byte b : signature){
                formatter.format("%02x", b);
            }
            Log.i(LOG_TAG,"signature-sha256 digest : "+formatter.toString());
        }catch (PackageManager.NameNotFoundException e){
            e.printStackTrace();
        }catch (NoSuchAlgorithmException e){
            e.printStackTrace();
        }
        return signature;
    }

    public void sendLicenseData(Context context, String enableList){
        QtiSendCmdData cmdData;
        byte[] sigHash = null;
        QtiUIApplicationManager mAppmgr = QtiUIApplicationManager.getInstance();
        String packageName = context.getPackageName();
        sigHash = getSignatureSha256(context, packageName);
        Log.i(LOG_TAG,"QtiFeedbackActivity, sendLicenseData, packageName : "+packageName);
        cmdData = new QtiSendCmdData(packageName, dummyLicense, "appInfo",
                enableList, sigHash, sigHash.length, 1, QtiUIConstants.SEND_LICENSE_CMD);
        if(cmdData.getLicenseLen() > 0){
            mAppmgr.sendCmd(cmdData, QtiUIConstants.SEND_LICENSE_CMD);
        }else{
            Log.e(LOG_TAG, "QtiSendCmdData cmd failed ");
        }
    }

    public boolean existDefaultLicense(String pfmFile){
        boolean ret = false;
        Log.i(LOG_TAG,"existDefaultLicense, pfmFile : "+pfmFile);
        try{
            ret = Arrays.asList(QtiUIApplicationManager.getContext().
                    getResources().getAssets().list("")).contains(pfmFile);
        }catch (Exception e)
        {
            e.printStackTrace();
        }
        Log.i(LOG_TAG,"existDefaultLicense, ret : "+ret);
        return ret;
    }

    private void startQtiUICmdReceiver()
    {
        Log.d(LOG_TAG,"startQtiUICmdReceiver");
        if(isCmdReceiverRegistered){
            return;
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(QtiUIConstants.INTENT_SMQUI_COLLECTOR_START);
        filter.addAction(QtiUIConstants.INTENT_SMQUI_OPTOUT);
        filter.addAction(QtiUIConstants.INTENT_SMQUI_LICENSE_TEST);
        filter.addAction(QtiUIConstants.INTENT_SMQUI_KILL_ALLCOLLECTORS);
        filter.addAction(QtiUIConstants.INTENT_SMQUI_KILL_COLLECTOR);
        filter.addAction(QtiUIConstants.INTENT_SMQUI_TURNON_COLLECTOR);
        filter.addAction(QtiUIConstants.INTENT_SNQUI_COLLECTOR_INFO);
		filter.addAction(QtiUIConstants.INTENT_SNQUI_GLB_OPT);

        uiCmdReceiver = new QtiUICmdReceiver();
        mApplicationContext.registerReceiver(uiCmdReceiver, filter, QtiUIConstants.INTENT_PERMISSION, null, Context.RECEIVER_EXPORTED);
        isCmdReceiverRegistered = true;
    }

    private void prepareUICmd(Context context){
        String QccUIFileDir = context.getFilesDir().getAbsolutePath();
        File testTxt = new File(context.getFilesDir().getAbsolutePath(), "test.txt");
        Log.d(LOG_TAG,"testTxt path : "+testTxt.getAbsolutePath());
        Log.d(LOG_TAG,"testTxt name : "+testTxt.getName());
        try{
            if(!testTxt.exists()){
                FileOutputStream fos = this.openFileOutput(testTxt.getName(), Context.MODE_PRIVATE);
                Writer out = new OutputStreamWriter(fos);
                out.write("test");
                out.close();
            }
        }catch (IOException e){
            e.printStackTrace();
        }
    }

}
