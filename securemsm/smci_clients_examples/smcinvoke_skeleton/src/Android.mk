LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES :=
LOCAL_SHARED_LIBRARIES :=

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../../../smcinvoke/inc \
                    $(LOCAL_PATH)/../../../mink/inc/interface \
                    $(LOCAL_PATH)/../../../mink/inc/uid \
                    $(LOCAL_PATH)/../inc \
                    $(LOCAL_PATH)/../../smcinvoke_example/inc \
                    $(LOCAL_PATH)/../../smcinvoke_example/src

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

LOCAL_MODULE := smcinvoke_skeleton
LOCAL_SRC_FILES := smci_ca_main.c \
        smci_skeleton.c
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
