/* 
 * Copyright (c) 2014, 2017,2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.smq.ui.util;

import android.annotation.SuppressLint;
//import android.os.SystemProperties;

import android.content.ComponentName;
import android.content.Context;
import android.content.pm.PackageManager;
import android.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class SMQUiUtils {
    private static final String LOG_TAG = "QCC-TR-UI";
    /**
     * @param c
     * @return
     */
    /**
     * Utility method to disable a component.
     * 
     * @param c
     *            the class of the component.
     * @param context
     *            the app context.
     * @return true if successful, false otherwise.
     */
    public static boolean disableComponent(final Class<?> c,
            final Context context) {
        final PackageManager pm = context.getPackageManager();
        boolean returnStatus = false;

        try {
            pm.setComponentEnabledSetting(new ComponentName(context, c),
                    PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                    PackageManager.DONT_KILL_APP);
            returnStatus = true;
        } catch (final IllegalArgumentException ie) {
			ie.printStackTrace();
        }

        return returnStatus;

    }

    /**
     * Utility methpod to enable a component.
     * 
     * @param c
     *            the class of the component.
     * @param context
     *            the app context.
     * @return true if successful, false otherwise.
     */
    public static boolean enableComponent(final Class<?> c,
            final Context context) {
        final PackageManager pm = context.getPackageManager();
        boolean returnStatus = false;


        try {
            pm.setComponentEnabledSetting(new ComponentName(context, c),
                    PackageManager.COMPONENT_ENABLED_STATE_ENABLED,
                    PackageManager.DONT_KILL_APP);
            returnStatus = true;

        } catch (final IllegalArgumentException ie) {
			ie.printStackTrace();
        }

        return returnStatus;

    }
    /**
     * Enable all receivers registered in the manifest.
     * @param context the app context.
     */
    public static void enbleReceivers(final Context context) {
        //If shutdown is due to non-acceptance of agreement, then disable the boot receiver.
    }
    
    /**
     * Disable all receivers registered in the manifest.
     * @param context the app context.
     */
    public static void disableReceivers(final Context context) {
        //If shutdown is due to non-acceptance of agreement, then disable the boot receiver.
    }

    public static boolean isVendorEnhancedFramework() {
        /*
        try {
            String strProperty = SystemProperties.get("ro.vendor.qti.va_aosp.support", "1");
            if(strProperty != null && !strProperty.equals("1")) {
                return false;
            }
        } catch (Exception e) {
            android.util.Log.e("SMQUiUtils", e.toString(), e);
            return true;
        }
        */
        String strProperty = SystemProperties_get("ro.vendor.qti.va_aosp.support");
        if(strProperty != null && !strProperty.equals("1")) {
            return false;
        }
        return true;
    }

    private static String SystemProperties_get(String key){
        String ret;
        try{
            @SuppressLint("PrivateApi")
            Class<?> SystemProperties = Class.forName("android.os.SystemProperties");

            @SuppressWarnings("rawtypes")
            Class[] paramTypes = { String.class };
            Method get = SystemProperties.getMethod("get", paramTypes);

            Object[] params = { key };
            ret = (String) get.invoke(SystemProperties, params);
        } catch (NoSuchMethodException
                | SecurityException
                | IllegalAccessException
                | IllegalArgumentException
                | InvocationTargetException
                | ClassNotFoundException e){
            e.printStackTrace();
            ret = "0";
        }
        Log.i(LOG_TAG, "SystemProperties_get: "+ret);
        return ret;
    }
}
