ifeq ($(KMGK_USE_QTI_SERVICE),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
                   InstallKeybox.cpp \
                   DeviceIdProvision.cpp \
                   VerifyAttestation.cpp \
                   device_management_service_v01.c

QC_PROP_ROOT ?= vendor/qcom/proprietary

LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/inc                \
                    $(LOCAL_PATH)/../../inc          \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/QSEEComAPI \
                    $(TOP)/$(QC_PROP_ROOT)/securemsm/keymaster_utils \
                    $(TOP)/external/boringssl/include \
                    $(TOP)/hardware/libhardware/include/hardware \
                    $(TOP)/vendor/qcom/proprietary/qmi-framework/inc \
                    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_HEADER_LIBRARIES := jni_headers libhardware_headers vendor_common_inc \
                          libqmi_common_headers libdiag_headers

LOCAL_SHARED_LIBRARIES := \
                          liblog                \
                          libcutils \
                          libcrypto             \
                          libkeymasterprovision \
                          libqmi_cci \
                          libqmi_common_so \
                          libkeymasterutils \
                          libkeymasterdeviceutils \
                          libqtikeymaster4

LOCAL_MODULE := KmInstallKeybox
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -Wall -Werror -Wunused -DKEYMASTER_NAME_TAGS
LOCAL_CLANG := true
LOCAL_CLANG_CFLAGS += -fno-sanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers

include $(BUILD_EXECUTABLE)

endif #ifeq ($(KMGK_USE_QTI_SERVICE),true)
