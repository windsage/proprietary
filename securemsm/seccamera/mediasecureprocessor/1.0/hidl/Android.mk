LOCAL_PATH := $(call my-dir)

QC_PROP_ROOT ?= vendor/qcom/proprietary
SECUREMSM_PATH := $(TOP)/$(QC_PROP_ROOT)/securemsm

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.hardware.secureprocessor@1.0
LOCAL_INIT_RC := vendor.qti.hardware.secureprocessor@1.0.rc
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := service.cpp \
    common/utils.cpp \
    common/SecureProcessor.cpp \
    SecureProcessorFactory.cpp \
    tee-processor/SecureProcessorQTEEMink.cpp \
    tvm-processor/SecureProcessorTVMMink.cpp \

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
    libdl \
    libbase \
    libutils \
    libhardware \
    libhidlbase \
    libbinder \
    vendor.qti.hardware.secureprocessor.common@1.0 \
    vendor.qti.hardware.secureprocessor.config@1.0 \
    vendor.qti.hardware.secureprocessor.device@1.0 \
    vendor.qti.hardware.secureprocessor.common@1.0-helper \
    libgralloc.qti \
    libgralloctypes \
    libdmabufheap \
    libminksocket_vendor \
    libminkdescriptor

LOCAL_HEADER_LIBRARIES := \
    display_intf_headers \
    vendor_common_inc \
    qtvm_sdk_headers

LOCAL_CFLAGS := -g -O3 -Wno-unused-parameter -Wall -Werror
LOCAL_CFLAGS += -D__ANDROID__

ifeq ($(DISABLE_CSF_VERSION_CHECK),true)
LOCAL_CFLAGS += -DDISABLE_CSF_VERSION_CHECK
endif

include $(BUILD_EXECUTABLE)
