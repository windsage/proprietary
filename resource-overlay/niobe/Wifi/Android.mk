LOCAL_PATH:= $(call my-dir)

# WifiResTarget-spf for common SPF features. ex: niobe, kalama and taro.

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/spf/res

LOCAL_MANIFEST_FILE := AndroidManifest-spf.xml

LOCAL_PACKAGE_NAME := WifiResTarget_spf
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)

# WifiResMainlineTarget-spf for common SPF features.

include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/spf/res

LOCAL_MANIFEST_FILE := AndroidGoogleManifest-spf.xml

LOCAL_PACKAGE_NAME := WifiResMainlineTarget_spf
LOCAL_SDK_VERSION := current

include $(BUILD_RRO_PACKAGE)
