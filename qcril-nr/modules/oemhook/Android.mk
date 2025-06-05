LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

ifeq ($(TARGET_HAS_LOW_RAM),true)
qcril_cflags += -DRIL_FOR_LOW_RAM
qcril_cppflags += -DRIL_FOR_LOW_RAM
endif

LOCAL_CFLAGS               +=  $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_SRC_FILES            := src/OemHookStableAidlModule.cpp
LOCAL_SRC_FILES            += src/stable_aidl_impl/qti_oem_hook_stable_aidl_service.cpp


ifneq ($(qcril_sanitize_diag),)
LOCAL_SANITIZE_DIAG := $(qcril_sanitize_diag)
endif

ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif

LOCAL_MODULE               := qcrilNrOemHookModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     := libqcrilNr-headers \
                              libqcrilNrQtiMutex-headers \
                              qtiril-utils-headers \
                              libril-legacy-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES     += qtiRilHalUtil-headers
LOCAL_HEADER_LIBRARIES     += libdiag_headers
LOCAL_HEADER_LIBRARIES     += libqcrilnr-nanopb-headers
LOCAL_HEADER_LIBRARIES 	   += libril-qc-qmi-services-headers
LOCAL_HEADER_LIBRARIES     += libimsqmiservices_headers

LOCAL_SHARED_LIBRARIES     := libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += qtiril-utils
LOCAL_SHARED_LIBRARIES     += libril-legacy
LOCAL_SHARED_LIBRARIES     += librilqmimiscservices


LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.qcrilhook-V1-ndk
LOCAL_SHARED_LIBRARIES     += libbinder_ndk
LOCAL_SHARED_LIBRARIES     += libbase

LOCAL_C_INCLUDES           += $(LOCAL_PATH)/src/

include $(BUILD_STATIC_LIBRARY)
