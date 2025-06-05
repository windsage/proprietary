LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

## Build header library
include $(CLEAR_VARS)
LOCAL_MODULE               := qcrilNrSmsHeaders
LOCAL_VENDOR_MODULE        := true
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/src
include $(BUILD_HEADER_LIBRARY)

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

LOCAL_MODULE               := libqcrilNrSmsModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_SHARED_LIBRARIES     := qcrilInterfaces

LOCAL_SHARED_LIBRARIES     += libqmi_cci
LOCAL_SHARED_LIBRARIES     += libqmi_encdec
LOCAL_SHARED_LIBRARIES     += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES     += libdiag
LOCAL_SHARED_LIBRARIES     += libhardware_legacy
LOCAL_SHARED_LIBRARIES     += libqmiservices
LOCAL_SHARED_LIBRARIES     += librilqmimiscservices
LOCAL_SHARED_LIBRARIES     += libqmi_client_helper
LOCAL_SHARED_LIBRARIES     += libcutils
LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libutils
LOCAL_SHARED_LIBRARIES     += libbase

LOCAL_SHARED_LIBRARIES     += libsqlite
LOCAL_SHARED_LIBRARIES     += qtiril-utils
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES     += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES     += qcrilNrQmiModule
LOCAL_SHARED_LIBRARIES     += libril-db
LOCAL_SHARED_LIBRARIES     += libQtiRilLoadable
LOCAL_SHARED_LIBRARIES     += libril-legacy

LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += libcutils_headers
LOCAL_HEADER_LIBRARIES     += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES     += libril-db-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNr-cmas-headers
LOCAL_HEADER_LIBRARIES     += libril-legacy-headers
LOCAL_HEADER_LIBRARIES 	   += libril-qc-qmi-services-headers

LOCAL_WHOLE_STATIC_LIBRARIES += libqcrilNr-cmas

include $(BUILD_SHARED_LIBRARY)
