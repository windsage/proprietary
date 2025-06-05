/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package vendor.qti.iwlan;

import android.os.IBinder;
import android.os.Parcel;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import java.util.List;
import java.util.ArrayList;


public class JniHelper {
  private static final String LOGTAG = "IWlanProxy:JniHelper";
  public static boolean isJniLibLoaded = false;
  private static JniHelper jnihelper = null;

  private JniHelper(){
    //private constructor
  }

  public static JniHelper getInstance() {
    if(jnihelper == null)
    {
      jnihelper = new JniHelper();
    }
    return jnihelper;
  }

  static {
      if(isJniLibLoaded == false) {
        try {
            System.loadLibrary("WlanServiceJni");
            Log.d(LOGTAG, "done loading jnihelper");
            isJniLibLoaded = true;
        } catch(UnsatisfiedLinkError e) {
            Log.e(LOGTAG,"Failed to load native library");
            e.printStackTrace();
        }
      }
  }

  public static native void initialize(int slotId);

  public static native void setupDataCall(int solotId, int serial, int accessNetwork,
                                          Parcel dataProfileInfo, boolean roamingAllowed,
                                          int reason, Parcel addresses[], String dnses[],
                                          int pduSessionId, Parcel sliceInfo,
                                          boolean matchAllRuleAllowed);

  public static native void deactivateDataCall(int solotId, int serial, int cid, int reason);

  public static native void getDataCallList(int solotId, int serial);

  public static native void getDataRegistrationState(int solotId, int serial);

  public static native void getAllQualifiedNetworks(int solotId, int serial);

  public static native void responseAcknowledgement(int solotId);

};