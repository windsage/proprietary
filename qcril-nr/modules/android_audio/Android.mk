LOCAL_PATH := $(call my-dir)
QCRIL_NR_DIR := ${LOCAL_PATH}/../..

include $(CLEAR_VARS)

LOCAL_CFLAGS               += $(qcril_cflags)
LOCAL_CXXFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_CPPFLAGS             += -std=c++17 $(qcril_cppflags)
LOCAL_LDFLAGS              += $(qcril_ldflags)
ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_SANITIZE             += $(qcril_sanitize)
LOCAL_NOSANITIZE           := cfi
endif
LOCAL_SRC_FILES            := src/AndroidAudioModuleStableAidl.cpp
LOCAL_SRC_FILES            += src/stable_aidl_impl/qti_audio_stable_aidl_service.cpp

LOCAL_MODULE               := qcrilNrAndroidAudioModule
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional
LOCAL_HEADER_LIBRARIES     += libqcrilNr-headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES     += libutils_headers
LOCAL_HEADER_LIBRARIES     += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES     += qtiRilHalUtil-headers
LOCAL_HEADER_LIBRARIES     += libril-legacy-headers
LOCAL_HEADER_LIBRARIES 	   += libril-qc-qmi-services-headers
LOCAL_SHARED_LIBRARIES     := libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.radio.am-V1-ndk
LOCAL_SHARED_LIBRARIES     += libbinder_ndk
LOCAL_SHARED_LIBRARIES     += libbase
LOCAL_SHARED_LIBRARIES     += liblog
LOCAL_SHARED_LIBRARIES     += libutils
LOCAL_SHARED_LIBRARIES     += libril-legacy

LOCAL_C_INCLUDES           += $(LOCAL_PATH)/src/

include $(BUILD_STATIC_LIBRARY)
