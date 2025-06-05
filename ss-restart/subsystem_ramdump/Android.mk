LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := subsystem_ramdump.c

LOCAL_CFLAGS := -DANDROID_BUILD

LOCAL_SHARED_LIBRARIES += libmdmdetect libcutils libutils liblog

LOCAL_MODULE := subsystem_ramdump

LOCAL_MODULE_TAGS := optional

ifeq ($(call is-vendor-board-platform,QCOM),true)
  LOCAL_HEADER_LIBRARIES := vendor_common_inc
endif

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
