/*
 * Copyright (c) 2020, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti;

import android.util.Log;
import java.util.Arrays;
import java.util.List;

public class LibsocHelper {
    private static final String TAG = "LibsocHelper";
    private static int msmCpu = -1, cpuVariant = -1;

    static {
        try {
            System.loadLibrary("soc_helper_jni");
        } catch (Exception ex) {
            Log.d(TAG, "Cannot load libsoc_helper_jni shared library");
            ex.printStackTrace();
        }
    }

    public LibsocHelper() {
    }

    public synchronized List<Object> getSocInfo() {
        if (msmCpu == -1) {
            native_getSocInfo();
        }
        return Arrays.asList(msmCpu, cpuVariant);
    }

    private native void native_getSocInfo();
}
