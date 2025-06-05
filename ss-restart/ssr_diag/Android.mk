################################################################################
# @file ssr_diag/Android.mk
# @brief Makefile for building the ssr diag API on Android.
################################################################################

ifeq ($(call is-vendor-board-platform,QCOM),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	ssr_diag_main.c

commonSharedLibraries :=libdiag

LDLIBS += -lpthread
LOCAL_MODULE:= ssr_diag
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES += $(commonSharedLibraries) libcutils libutils liblog
LOCAL_HEADER_LIBRARIES := vendor_common_inc libdiag_headers

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

endif
