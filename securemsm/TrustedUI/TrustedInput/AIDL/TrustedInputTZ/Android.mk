# =============================================================================
#
# Module: Secure TouchInput Android Driver Interface
#
# =============================================================================

ifneq ($(ENABLE_TRUSTED_UI_VM_3_0), true)
LOCAL_PATH          := $(call my-dir)

include $(CLEAR_VARS)

QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm

LOCAL_MODULE        := libTrustedInputAIDL

LOCAL_MODULE_TAGS   := optional

LOCAL_C_INCLUDES   := $(LOCAL_PATH)/inc \
                      $(SECUREMSM_PATH)/TrustedUI/aidl-service \

LOCAL_CPPFLAGS      := $(ST_TARGET_CFLAGS) -fdiagnostics-show-option

LOCAL_SRC_FILES     := src/TrustedInput.cpp \
                       src/TrustedInputFactory.cpp

LOCAL_HEADER_LIBRARIES := vendor_common_inc libbinder_ndk_headers

LOCAL_SHARED_LIBRARIES := liblog \
                          libutils \
                          libbinder_ndk \
                          vendor.qti.hardware.trustedui-V1-ndk \

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
endif
