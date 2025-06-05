ifneq ($(TARGET_BOARD_AUTO),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE        := IWlanService
LOCAL_JAVA_LIBRARIES := android.hardware.radio.data-V1-java \
                        android.hardware.radio.network-V1-java \
LOCAL_STATIC_JAVA_LIBRARIES := vendor.qti.hardware.data.iwlandata-V1-java \

LOCAL_MODULE_OWNER  := qti
LOCAL_MODULE_TAGS   := optional
LOCAL_MODULE_CLASS  := APPS
LOCAL_CERTIFICATE   := platform
LOCAL_MODULE_SUFFIX := .apk
LOCAL_SRC_FILES     := IWlanService.apk
LOCAL_MODULE_PATH   := $(TARGET_OUT_VENDOR)/app/
include $(BUILD_PREBUILT)

endif
