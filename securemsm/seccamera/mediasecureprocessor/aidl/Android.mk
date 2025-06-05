LOCAL_PATH := $(call my-dir)

QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.hardware.secureprocessor
LOCAL_INIT_RC := vendor.qti.hardware.secureprocessor.rc
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.secureprocessor.xml

LOCAL_SRC_FILES := service.cpp \
    common/utils.cpp \
    common/SecureProcessor.cpp \
    tee-processor/SecureProcessorQTEEMink.cpp \
    tvm-processor/SecureProcessorTVMMink.cpp

LOCAL_C_INCLUDES := $(SECUREMSM_PATH)/smcinvoke/inc \
                    $(SECUREMSM_PATH)/smcinvoke/TZCom/inc \
                    $(SECUREMSM_PATH)/mink/inc/interface/ \
                    $(SECUREMSM_PATH)/mink/inc/qtvm/ \
                    $(SECUREMSM_PATH)/mink/inc/uid/ \
                    $(LOCAL_PATH)/tee-processor \
                    $(LOCAL_PATH)/tvm-processor \
                    $(LOCAL_PATH)/common

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libbase \
    libutils \
    libhardware \
    libbinder_ndk \
    libhidlbase \
    android.hardware.common-V2-ndk \
    vendor.qti.hardware.secureprocessor.common-helper \
    vendor.qti.hardware.secureprocessor.device-V1-ndk \
    vendor.qti.hardware.secureprocessor.common-V1-ndk \
    vendor.qti.hardware.secureprocessor.config-V1-ndk \
    libgralloc.qti \
    libgralloctypes \
    libdmabufheap \
    libminksocket_vendor \
    libminkdescriptor

LOCAL_STATIC_LIBRARIES := libaidlcommonsupport

LOCAL_HEADER_LIBRARIES := \
    display_intf_headers \
    vendor_common_inc \
    qtvm_sdk_headers

LOCAL_CFLAGS := -g -O3 -Wno-unused-parameter -Wall -Werror -Woverloaded-virtual
LOCAL_CFLAGS += -D__ANDROID__

ifeq ($(DISABLE_CSF_VERSION_CHECK),true)
LOCAL_CFLAGS += -DDISABLE_CSF_VERSION_CHECK
endif

include $(BUILD_EXECUTABLE)
