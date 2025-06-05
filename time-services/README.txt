Now TimeService APK will be delivered as prebuilt apk.

If any changes will be required in TimeService.apk then follow these steps for enabling JAVA file compilation in vendor which is required for generating TimeService APK:

Comment below mentioned lines in Android.bp file:
android_app_import {
    name: "TimeService",

    apk: "TimeService.apk",
    presigned: true,

    proprietary: true,
    owner: "qti",

}

Un-comment below mentioned lines in Android.bp file:
//android_app {
//    name: "TimeService",
//
//    optimize: {
//    enabled: false,
//    },
//
//    srcs: ["**/*.java"],
//
//    certificate: "platform",
//    proprietary: true,
//    owner: "qti",
//
//    sdk_version: "system_current",
//}
