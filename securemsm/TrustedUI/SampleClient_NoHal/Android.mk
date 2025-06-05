LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

SECUREMSM_SHIP_PATH := $(TOP)/vendor/qcom/proprietary/securemsm

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := src/TAVMInterface.cpp \
                   src/TUIMINKInvoke.cpp \
                   src/CHLOSListenerCBO.cpp \
                   src/TUISampleClient_NoHal.cpp

LOCAL_C_INCLUDES += $(SECUREMSM_SHIP_PATH)/TrustedUI/SampleClient_NoHal/inc \
                    $(SECUREMSM_SHIP_PATH)/sse/SecureIndicator/inc \
                    $(SECUREMSM_SHIP_PATH)/mink/inc/interface

LOCAL_HEADER_LIBRARIES := libhardware_headers smcinvoke_headers vendor_common_inc loadtvmapp_headers qtvm_sdk_headers
LOCAL_STATIC_LIBRARIES += libgtest

LOCAL_SHARED_LIBRARIES := libbinder_ndk \
                          liblog \
                          libhidlbase \
                          libutils \
                          libcutils \
                          libsi \
                          libminksocket_vendor \
                          libvmmem \
                          libqdutils \
                          libhidlmemory \
                          vendor.qti.hardware.trustedui@1.0 \
                          android.hidl.allocator@1.0 \
                          libloadtrusteduiapp \
                          vendor.qti.hardware.display.config-V7-ndk

LOCAL_MODULE := TrustedUISampleTAClient
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
