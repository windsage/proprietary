ifneq ($(TARGET_IS_HEADLESS), true)
ifeq ($(ENABLE_TRUSTED_UI_AIDL), true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_SHIP_PATH   := $(TOP)/$(QC_PROP_ROOT)/securemsm

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_C_INCLUDES := $(TOP)/hardware/qcom/display/libqdutils \
                    $(TOP)/hardware/libhardware/include \
                    $(SECUREMSM_SHIP_PATH)/sse/SecureIndicator/inc \
                    $(SECUREMSM_SHIP_PATH)/smcinvoke/TZCom/inc

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        liblog \
        libqdutils

LOCAL_CFLAGS := -O3

ifeq ($(ENABLE_TRUSTED_UI_VM_3_0), true)
LOCAL_CFLAGS += -DENABLE_TRUSTED_UI_VM_3_0
endif

LOCAL_SHARED_LIBRARIES += libsi \
        libbinder_ndk \
        libbase \
        vendor.qti.hardware.trustedui-V1-ndk \
        vendor.qti.hardware.display.config-V7-ndk

LOCAL_HEADER_LIBRARIES := vendor_common_inc libbinder_ndk_headers libbinder_ndk_helper_headers
LOCAL_STATIC_LIBRARIES += libgtest

LOCAL_MODULE := TrustedUISampleTestAIDL
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := TrustedUISampleClient.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_SANITIZE := cfi integer_overflow
LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)
endif
endif
