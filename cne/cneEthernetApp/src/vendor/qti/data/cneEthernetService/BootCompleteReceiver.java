/**
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package vendor.qti.data.cneEthernetService;

import android.content.Intent;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.util.Log;
import android.os.UserManager;
import android.content.pm.PackageManager;
import vendor.qti.data.cneEthernetService.EthernetRequestHandler;

public class BootCompleteReceiver extends BroadcastReceiver  {
  private static boolean isServiceStarted = false;
  private static final String LOGTAG = "cneEthernetService:BootCompleteReceiver";

  @Override
  public void onReceive(Context context, Intent intent) {
    if (!context.getSystemService(UserManager.class).isSystemUser()){
      ComponentName targetName = new ComponentName(context, BootCompleteReceiver.class);
      context.getPackageManager().setComponentEnabledSetting(targetName,
      PackageManager.COMPONENT_ENABLED_STATE_DISABLED, 0);
      return;
    }
    if (intent == null) {
      Log.e(LOGTAG, "Null Intent received");
      return;
    }

    final String intentActionStr = intent.getAction();
    if (intentActionStr == null) {
      Log.e(LOGTAG, "Null intent action received");
      return;
    }

    if (intentActionStr.equals(Intent.ACTION_LOCKED_BOOT_COMPLETED) ||
        intentActionStr.equals(Intent.ACTION_BOOT_COMPLETED)) {
      Log.i(LOGTAG, "ACTION_LOCKED_BOOT_COMPLETED or ACTION_BOOT_COMPLETED received");

      if(isServiceStarted == false){
        Intent startEthernetRequestHandlerIntent =
                new Intent(context, EthernetRequestHandler.class);
        Log.d(LOGTAG, "START EthernetRequestHandler");
        ComponentName comp = new ComponentName(context.getPackageName(),
                                                EthernetRequestHandler.class.getName());
        ComponentName service = context.startService(new Intent().setComponent(comp));
        if (service == null) {
          Log.e(LOGTAG, "EthernetRequestHandler couldn't start service " + comp.toString());
        } else {
          Log.d(LOGTAG, "EthernetRequestHandler started successfully");
          isServiceStarted = true;
        }
      } else {
          Log.d(LOGTAG, "EthernetRequestHandler already started");
      }
    } else {
        Log.e(LOGTAG, "cneEthernetService received unknown action: " + intentActionStr);
    }
  }
}
