LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES += vendor_common_inc \
                          libdiag_headers

LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(LOCAL_PATH)/../mink/inc/interface \
                    $(LOCAL_PATH)/../mink/inc/uid \
                    $(LOCAL_PATH)/../smcinvoke/inc \

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libdiag \
        libminkdescriptor \

LOCAL_MODULE := diagcommd

LOCAL_SRC_FILES := diagcommd.c
LOCAL_SRC_FILES += diag_dispatch.c
LOCAL_INIT_RC := diagcommd.rc
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(DIAG_CFLAGS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
