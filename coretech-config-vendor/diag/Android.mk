
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
LOCAL_MODULE := vendor.qti.diag_userdebug.rc
LOCAL_SRC_FILES := vendor.qti.diag_userdebug.rc
else
LOCAL_MODULE := vendor.qti.diag.rc
LOCAL_SRC_FILES := vendor.qti.diag.rc
endif

LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/init
include $(BUILD_PREBUILT)






