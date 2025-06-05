/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.smq.util;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

//public class QtiSendCmdData implements Parcelable{
public class QtiSendCmdData {
    private String _packageName;
    private String _value;
    private String _command;
    private String _appInfo;
    private String _collectorList;
    private int _optInType;
    private byte[] _licenseByte;
    private int _licenseLen = 0;
    private byte[] _isvHash;
    private int _isvHashLen;
    private int _cmdType;

    public QtiSendCmdData(String packageName, String cmd, String value, int cmdType){
        this._packageName = packageName;
        this._command = cmd;
        this._value = value;
        this._cmdType = cmdType;
    }

    public QtiSendCmdData(String packageName, byte[] license, String appInfo, String collectorList,
                          byte[] hash, int hashLen, int optInType, int cmdType){
        this._licenseByte = license;
        this._licenseLen = license.length;
        this._packageName = packageName;
        this._appInfo = appInfo;
        this._collectorList = collectorList;
        this._isvHash = hash;
        this._isvHashLen = hashLen;
        this._optInType = optInType;
        this._cmdType = cmdType;
    }

    public QtiSendCmdData(File pfmFile, String packageName,
                          String appInfo, String collectorList, byte[] hash, int hashLen, int optInType, int cmdType){
        this._licenseByte = new byte[(int)pfmFile.length()];
        try(BufferedInputStream buf = new BufferedInputStream(new FileInputStream(pfmFile)))
        {
            buf.read(this._licenseByte, 0, this._licenseByte.length);
            this._licenseLen = this._licenseByte.length;
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        this._packageName = packageName;
        this._appInfo = appInfo;
        this._collectorList = collectorList;
        this._isvHash = hash;
        this._isvHashLen = hashLen;
        this._optInType = optInType;
        this._cmdType = cmdType;
    }

    //read from assets
    public QtiSendCmdData(Context context, String pfmFile, String packageName,
                          String appInfo, String collectorList, byte[] hash, int hashLen, int optInType, int cmdType){
        AssetManager am = context.getAssets();
        try(InputStream is = am.open(pfmFile))
        {
            this._licenseByte = new byte[is.available()];
            is.read(this._licenseByte, 0, this._licenseByte.length);
            this._licenseLen = this._licenseByte.length;
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        this._packageName = packageName;
        this._appInfo = appInfo;
        this._collectorList = collectorList;
        this._isvHash = hash;
        this._isvHashLen = hashLen;
        this._optInType = optInType;
        this._cmdType = cmdType;
    }

    public String getPkgName(){
        return this._packageName;
    }

    public String getValue(){
        return this._value;
    }

    public String getCommand(){
        return this._command;
    }

    public String getAppInfo(){
        return this._appInfo;
    }

    public String getCollectorList(){
        return this._collectorList;
    }

    public int getOptInType(){
        return this._optInType;
    }

    public int getLicenseLen(){
        return this._licenseLen;
    }

    public int getCmdType(){
        return this._cmdType;
    }

    public byte[] getLicenseByte(){
        return this._licenseByte;
    }

    public int getIsvHashLen() { return this._isvHashLen; }

    public byte[] getIsvHash() { return this._isvHash; }
}
