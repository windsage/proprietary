LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES :=
LOCAL_SHARED_LIBRARIES :=

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../smcinvoke/inc \
                    $(LOCAL_PATH)/../mink/inc/interface

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES += \
        libc \
        libcutils \
        libutils \
        liblog \
        libdl \
        libdmabufheap \
        libminkdescriptor

LOCAL_MODULE := rpmbClient
LOCAL_SRC_FILES := rpmbClient.c
LOCAL_MODULE_TAGS := optional
ifeq ($(TARGET_ENABLE_QSEECOM),true)
LOCAL_CFLAGS := -DTARGET_ENABLE_QSEECOM
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../QSEEComAPI
LOCAL_SHARED_LIBRARIES += libQSEEComAPI
endif
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
