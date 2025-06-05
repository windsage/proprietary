LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := libril-legacy
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

ifeq ($(TARGET_PD_SERVICE_ENABLED), true)
LOCAL_SHARED_LIBRARIES     += libpdmapper
LOCAL_SHARED_LIBRARIES     += libpdnotifier
LOCAL_CFLAGS += -DQCRIL_USE_PD_NOTIFIER
endif
LOCAL_SHARED_LIBRARIES     += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += qtiPeripheralMgr
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += qtiril-utils

LOCAL_HEADER_LIBRARIES     += libril-legacy-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES     += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES     += vendor_common_inc
LOCAL_HEADER_LIBRARIES     += qtiPeripheralMgr-headers
LOCAL_HEADER_LIBRARIES     += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES     += libqmi_common_headers

LOCAL_EXPORT_HEADER_LIBRARIES += libril-legacy-headers

include $(BUILD_SHARED_LIBRARY)
