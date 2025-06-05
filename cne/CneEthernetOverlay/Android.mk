LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/overlay

LOCAL_MANIFEST_FILE := AndroidManifest.xml

LOCAL_PACKAGE_NAME := CneEthernetOverlay
LOCAL_SDK_VERSION := current

LOCAL_CERTIFICATE := platform
LOCAL_MODULE_OWNER := qti
LOCAL_VENDOR_MODULE := true


include $(BUILD_RRO_PACKAGE)