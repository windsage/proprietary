LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Logging Features. Enable only one at any time
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_STDERR
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_SYSLOG
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM

LOCAL_SRC_FILES := qdi.c \
                   qdi_netlink.c \
                   qdi_qmi_wds.c

LOCAL_SHARED_LIBRARIES := libdiag
LOCAL_SHARED_LIBRARIES += libdsutils
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libqmi
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../inc

LOCAL_HEADER_LIBRARIES := libqmi_common_headers \
		          libqmi_cci_headers \
                          libdiag_headers \
                          libqmi_headers \
                          libdataqmiservices_headers \
                          vendor_common_inc \
                          libutils_headers \
                          libdsi_netctrl_headers \
                          libdsutils_headers

LOCAL_MODULE := libqdi
LOCAL_SANITIZE:=integer_overflow
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_CLANG := true

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
