/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.smq.util;

public class QtiUIConstants {
    /*
    adb shell am broadcast -a com.qualcomm.qti.smq.intent.action.OPT_CMD --es ISV_NAME ISV-A --es OPTIN 1  // 1 : optin, 0: optout
    adb shell am broadcast -a com.qualcomm.qti.smq.intent.action.OPT_CMD --es ISV_NAME ISV-A --es OPTIN 0
     */
    public static final String INTENT_SMQUI_OPTOUT = "com.qualcomm.qti.smq.intent.action.OPTOUT";

    /*
    adb shell am broadcast -a com.qualcomm.qti.smq.intent.action.COLLECTOR_START --es ISV_ID 101 --es START_CMD 1  // 1 : start, 0: stop
    adb shell am broadcast -a com.qualcomm.qti.smq.intent.action.COLLECTOR_START --es ISV_ID 101 --es START_CMD 0  // 1 : start, 0: stop
     */
    public static final String INTENT_SMQUI_COLLECTOR_START = "com.qualcomm.qti.smq.intent.action.COLLECTOR_START";

    /*
    adb shell am broadcast -a com.qualcomm.qti.smq.intent.action.LICENSE_TEST
     */
    public static final String INTENT_SMQUI_LICENSE_TEST = "com.qualcomm.qti.smq.intent.action.LICENSE_TEST";

    public static final String INTENT_SMQUI_KILL_ALLCOLLECTORS = "com.qualcomm.qti.smq.intent.action.KILLALL";

    public static final String INTENT_SMQUI_KILL_COLLECTOR = "com.qualcomm.qti.smq.intent.action.KILL";

    public static final String INTENT_SMQUI_TURNON_COLLECTOR = "com.qualcomm.qti.smq.intent.action.TURNON";

    public static final String INTENT_SNQUI_COLLECTOR_INFO = "com.qualcomm.qti.smq.intent.action.COLLECTOR_INFO";

    public static final String INTENT_SNQUI_GLB_OPT= "com.qualcomm.qti.smq.intent.action.GLB_OPT";

    public static final String INTENT_PERMISSION = "com.qualcomm.qti.smq.qcc.permission.QCCTRUI";

    public static final String OPTOUT_CMD = "OPTOUT_CMD";

    public static final String COLLECTOR_START_CMD = "START_CMD";

    public static final String KILL_ALL_CMD = "KILL_ALL_CMD";

    public static final String KILL_COLLECTOR_CMD = "KILL_COLLECTOR_CMD";

    public static final String TURNON_COLLECTOR_CMD = "TURNON_COLLECTOR_CMD";

    public static final String COLLECTOR_INFO_CMD = "COLLECTOR_INFO_CMD";

    public static final int SEND_DB_CMD = 1;
    public static final int SEND_LICENSE_CMD = 2;
}
