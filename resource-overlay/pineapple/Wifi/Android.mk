LOCAL_PATH:= $(call my-dir)

# WifiResTarget for pineapple, which supports DBS features

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_MANIFEST_FILE := AndroidManifest.xml

LOCAL_PACKAGE_NAME := WifiResTarget
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResMainlineTarget for pineapple, which supports DBS features

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_MANIFEST_FILE := AndroidGoogleManifest.xml

LOCAL_PACKAGE_NAME := WifiResMainlineTarget
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResTarget-spf for common SPF features. ex: pineapple, kalama and taro.
# This is primarily for non-DBS targets.

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/spf/res

LOCAL_MANIFEST_FILE := AndroidManifest-spf.xml

LOCAL_PACKAGE_NAME := WifiResTarget_spf
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResMainlineTarget-spf for common SPF features.
# This is primarily for non-DBS targets.

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/spf/res

LOCAL_MANIFEST_FILE := AndroidGoogleManifest-spf.xml

LOCAL_PACKAGE_NAME := WifiResMainlineTarget_spf
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)
