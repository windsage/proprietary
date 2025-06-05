libdiagwrapper JAVA library will be delivered as a prebuilt .jar.
If any changes are required to libdiagwrapper then follow these steps for
enabling library compilation.

1) Comment below lines in Android.bp file:
java_import {
    name: "libdiagwrapper",
    jars: ["lib/libdiagwrapper.jar"],
    sdk_version: "current",
    owner: "qti",
    proprietary: true,
}

2) Un-comment below mentioned lines in Android.bp file:
//java_library {
//    name: "libdiagwrapper",
//
//    manifest: "com.qualcomm.qti.diagservice.libdiagwrapper.xml",
//    srcs: ["com/qualcomm/qti/diagservice/*"],
//    installable: true,
//
//    sdk_version: "current",
//    owner: "qti",
//    proprietary: true,
//}

3) The library will not be exported so it must be copied using this command:
cp out/soong/.intermediates/vendor/qcom/proprietary/diag/java/libdiagwrapper/android_common/javac/libdiagwrapper.jar vendor/qcom/proprietary/diag/java/lib/libdiagwrapper.jar

4) For library testing please refer to gerrit Topic "DIAG_JAVA_TEST" for examples

5) Java file compilation will be enabled with the VSDK. At which point the above steps
may no longer be necessary. This will be updated in the future to reflect the VSDK
process.
