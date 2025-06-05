LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH) \

LOCAL_HEADER_LIBRARIES := libfastrpc_vendor_headers \
                          libvmmem_headers
LOCAL_SHARED_LIBRARIES := \
        libc \
        libcdsprpc \
        libvmmem
LOCAL_MODULE := libloadalgo_stub
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := loadalgo_stub.c
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TARGET_ARCHS:= arm64
LOCAL_MULTILIB := 64
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH) \
                    vendor/qcom/proprietary/securemsm/QSEEComAPI \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/loadalgo_TA_headers

LOCAL_HEADER_LIBRARIES := libfastrpc_vendor_headers \
                          vendor_common_inc \
                          libvmmem_headers \
                          mink_headers \
                          libminksocket_vendor \
                          qtvm_sdk_headers

LOCAL_SHARED_LIBRARIES := \
        libc \
        libutils \
        libdl \
        libdmabufheap \
        libloadalgo_stub \
        liblog \
        libvmmem \
        libminksocket_vendor

ifneq ($(TARGET_BOARD_AUTO),true)
LOCAL_SHARED_LIBRARIES += libQSEEComAPI
endif

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk \
                                 $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_MODULE := loadalgo
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := \
        secure_memory.c \
        loadalgo.c \
        loadalgo_qtvm.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)
ifeq ($(TARGET_USES_QMAA),true)
    LOCAL_CFLAGS += -DLOADALGO_QMAA_ENABLED
endif #TARGET_USES_QMAA
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TARGET_ARCHS:= arm64
LOCAL_MULTILIB := 64
include $(BUILD_EXECUTABLE)

