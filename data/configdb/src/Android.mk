# sources and intermediate files are separated

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

# Logging Features. Enable only one at any time
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_STDERR
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_SYSLOG
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM

LOCAL_CFLAGS += -DFEATURE_XMLLIB

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := configdb.c
LOCAL_SRC_FILES += configdb_xml.c
LOCAL_SRC_FILES += configdb_util.c

LOCAL_HEADER_LIBRARIES := libdiag_headers \
                          vendor_common_inc \
                          libconfigdb_headers \
                          libxml_headers \
                          libdsutils_headers

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libconfigdb_headers

LOCAL_MODULE := libconfigdb
LOCAL_SANITIZE:=integer_overflow

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libxml
LOCAL_SHARED_LIBRARIES += libdsutils
LOCAL_SHARED_LIBRARIES += libdiag

LOCAL_CLANG := true

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

LOCAL_HEADER_LIBRARIES += libcutils_headers
ifdef BOARD_VNDK_VERSION
LOCAL_SHARED_LIBRARIES += liblog
endif
include $(BUILD_SHARED_LIBRARY)
