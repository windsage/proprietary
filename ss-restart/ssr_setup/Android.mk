ifeq ($(call is-vendor-board-platform,QCOM),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#only compile on targets with libmdmdetect
ifneq ($(wildcard $(LOCAL_PATH)/../../mdm-helper/libmdmdetect),)
LOCAL_HEADER_LIBRARIES := vendor_common_inc
LOCAL_SRC_FILES := ssr_setup.c
LOCAL_MODULE := ssr_setup
LOCAL_SHARED_LIBRARIES += libc libcutils libutils libmdmdetect liblog
LOCAL_MODULE_TAG := optional

ifeq ($(TARGET_USES_REMOTEPROC),true)
	LOCAL_CFLAGS += -DUSES_RPROC=1
endif

LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif
endif
