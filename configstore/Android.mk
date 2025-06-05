LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := configparser.sh
LOCAL_MODULE_OWNER := qti
LOCAL_IS_HOST_MODULE := true

include $(BUILD_HOST_PREBUILT)
