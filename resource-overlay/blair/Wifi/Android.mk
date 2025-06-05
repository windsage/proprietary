LOCAL_PATH:= $(call my-dir)

# WifiResTarget for blair

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_MANIFEST_FILE := AndroidManifest.xml

LOCAL_PACKAGE_NAME := WifiResTarget
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResMainlineTarget for blair

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_MANIFEST_FILE := AndroidGoogleManifest.xml

LOCAL_PACKAGE_NAME := WifiResMainlineTarget
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)
