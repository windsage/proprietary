LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             := $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)

LOCAL_SRC_FILES            += $(call all-cpp-files-under, src)

LOCAL_MODULE               := libqcrilNrVoiceModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_C_INCLUDES           += $(LOCAL_PATH)/src/

LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += libril-db-headers
LOCAL_HEADER_LIBRARIES     += libril-legacy-headers
LOCAL_HEADER_LIBRARIES     += libimsqmiservices_headers
LOCAL_HEADER_LIBRARIES     += qtiwakelock-headers

LOCAL_SHARED_LIBRARIES     := libcutils
LOCAL_SHARED_LIBRARIES     += libsqlite
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES     += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES     += libril-db
LOCAL_SHARED_LIBRARIES     += libril-legacy
LOCAL_SHARED_LIBRARIES     += qtiril-utils
LOCAL_SHARED_LIBRARIES     += libqcrilNrQtiBus
LOCAL_SHARED_LIBRARIES     += qtiwakelock
LOCAL_SHARED_LIBRARIES     += qcrilNrQmiModule
LOCAL_SHARED_LIBRARIES     += libQtiRilLoadable
LOCAL_SHARED_LIBRARIES     += qcrilIntermodulemsgs

LOCAL_SHARED_LIBRARIES += qcrilInterfaces

include $(BUILD_SHARED_LIBRARY)
