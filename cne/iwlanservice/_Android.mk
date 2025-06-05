TOP_LOCAL_PATH:= $(call my-dir)

include $(call all-subdir-makefiles)

LOCAL_PATH:= $(TOP_LOCAL_PATH)

ifneq ($(TARGET_BOARD_AUTO),true)


include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_STATIC_JAVA_LIBRARIES := vendor.qti.hardware.data.iwlandata-V1-java
LOCAL_STATIC_JAVA_LIBRARIES += android.hardware.radio.data-V2-java
LOCAL_STATIC_JAVA_LIBRARIES += android.hardware.radio.network-V2-java

LOCAL_JNI_SHARED_LIBRARIES := libWlanServiceJni

LOCAL_MODULE_OWNER := qti
LOCAL_PACKAGE_NAME := IWlanService

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform
LOCAL_SDK_VERSION := system_current
include $(BUILD_PACKAGE)
endif
