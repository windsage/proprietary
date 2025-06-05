ifeq ($(ENABLE_TRUSTED_UI_AIDL),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_SHIP_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := vendor.qti.hardware.trustedui-aidl-service-qti
LOCAL_INIT_RC := vendor.qti.hardware.trustedui-aidl-service-qti.rc
LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.trustedui-aidl-service.xml
LOCAL_SRC_FILES := service.cpp

LOCAL_C_INCLUDES := $(SECUREMSM_SHIP_PATH)/mink/inc/interface

LOCAL_HEADER_LIBRARIES := libbinder_ndk_headers

LOCAL_SHARED_LIBRARIES := liblog \
                          libbase \
                          libutils \
                          libbinder_ndk \
                          vendor.qti.hardware.trustedui-V1-ndk \
                          libTrustedUIAIDL \
                          libTrustedInputAIDL

LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif
