# =============================================================================

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

QC_PROP_ROOT ?= vendor/qcom/proprietary

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(QC_PROP_ROOT)/securemsm/QSEEComAPI

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libutils \
        libQSEEComAPI \
        liblog \

LOCAL_CFLAGS += -DLOG_TAG=\"TbaseLoader\" \
                $(QSEECOM_CFLAGS)

LOCAL_SRC_FILES += loadtbase.c

LOCAL_MODULE := tbaseLoader
ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
