LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:=  main.cpp

LOCAL_HEADER_LIBRARIES := cne_common_inc
LOCAL_HEADER_LIBRARIES += cnd_src_inc
LOCAL_HEADER_LIBRARIES += libcndinc_headers
LOCAL_HEADER_LIBRARIES += libqmi_headers
LOCAL_HEADER_LIBRARIES += libdataqmiservices_headers
LOCAL_HEADER_LIBRARIES += qti_kernel_headers

LOCAL_C_INCLUDES += $(QC_PROP_ROOT)/data/dsi_netctrl/inc

LOCAL_MODULE:= autod
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SHARED_LIBRARIES :=  libcutils \
                           liblog \
                           libdsi_netctrl \
                           libcne
LOCAL_CLANG := true
include $(BUILD_EXECUTABLE)
