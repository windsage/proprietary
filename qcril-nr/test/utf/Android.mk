#ifeq ($(QCRIL_NR_ENABLE_UTF), true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

QCRIL_NR_DIR := ../../../qcril-nr
QCRIL_DATA_DIR := ../../../qcril-data-hal
DATA_DIR := ../../../data
QMI_DIR := ../../../qmi
QMI_FWK_DIR := ../../../qmi-framework
QCRIL_QMI_DIR := ../../../qcril-qmi-services
IMS_SHIP_DIR := ../../../ims-ship
COMMON_DIR := ../../../common
QCRIL_QMI_SVC_HEADERS_DIR := ../../../qcril-qmi-services-headers

build_for_android := true

LOCAL_CPP_EXTENSION := .cc
include $(LOCAL_PATH)/hal_master_make
LOCAL_SRC_FILES := $(c_sources) $(cc_sources)

LOCAL_CFLAGS                := $(FEATURES) -ferror-limit=0 -g  -fno-omit-frame-pointer -O0
LOCAL_CXXFLAGS              := $(FEATURES) -g  -fno-omit-frame-pointer -std=c++17 -O0
LOCAL_CPPFLAGS              := $(FEATURES) -g  -fno-omit-frame-pointer -fexceptions -std=c++17 -O0
LOCAL_LDFLAGS               := -lrt -g  -ldl

ifeq ($(UTF_TEST_COVERAGE), true)
LOCAL_CFLAGS                += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CXXFLAGS              += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CPPFLAGS              += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_LDFLAGS               += -fprofile-arcs -ftest-coverage --coverage
endif

LOCAL_C_INCLUDES            += $(LOCAL_PATH)/../../qcril-common/interfaces/inc

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES            += system/libhidl/base/include
LOCAL_C_INCLUDES            += system/libfmq/base/
LOCAL_C_INCLUDES            += system/unwinding/libbacktrace/include/
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES      += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES      += qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES      += qcrilIntermodulemsgs-headers
LOCAL_HEADER_LIBRARIES      += libcutils_headers
LOCAL_HEADER_LIBRARIES      += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES      += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES      += libril-db-headers
LOCAL_HEADER_LIBRARIES      += libril-legacy-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNr-cmas-headers
LOCAL_HEADER_LIBRARIES      += qtiRilHalUtil-headers
LOCAL_HEADER_LIBRARIES      += vendor_common_inc
LOCAL_HEADER_LIBRARIES      += libtime_genoff_headers
LOCAL_HEADER_LIBRARIES      += libdataqmiservices_headers_ext
LOCAL_HEADER_LIBRARIES += libcutils_headers

utf_inc := $(patsubst -I%, $(LOCAL_PATH)/%, $(INC))
LOCAL_C_INCLUDES            += $(utf_inc)

# if not coverage
ifneq ($(UTF_TEST_COVERAGE), true)
LOCAL_SANITIZE_DIAG := address cfi
LOCAL_SANITIZE      := address cfi
endif

LOCAL_MODULE                := libqcrilNr
LOCAL_MODULE_OWNER          := qti
LOCAL_PROPRIETARY_MODULE    := true
LOCAL_MODULE_TAGS           := optional

LOCAL_STATIC_LIBRARIES      += qcrilNrQtiRadio
LOCAL_SHARED_LIBRARIES      += libsqlite
LOCAL_SHARED_LIBRARIES      += libQtiRilLoadable
LOCAL_SHARED_LIBRARIES      += libbinder
LOCAL_SHARED_LIBRARIES      += libbinder_ndk
LOCAL_SHARED_LIBRARIES      += librilqmimiscservices
LOCAL_SHARED_LIBRARIES      += libqcrilNrLogger

include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)
build_for_android := true
include $(LOCAL_PATH)/hal_master_make
LOCAL_SRC_FILES := $(cpp_sources)
LOCAL_CPP_EXTENSION := .cpp

LOCAL_CFLAGS    := $(FEATURES) -ferror-limit=0 -g -fno-omit-frame-pointer -O0
LOCAL_CXXFLAGS  := $(FEATURES) -g -fno-omit-frame-pointer -std=c++17 -O0
LOCAL_CPPFLAGS  := $(FEATURES) -g -fno-omit-frame-pointer -std=c++17 -O0

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

ifeq ($(UTF_TEST_COVERAGE), true)
LOCAL_CFLAGS          += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CXXFLAGS        += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CPPFLAGS        += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_LDFLAGS         += -fprofile-arcs -ftest-coverage --coverage
endif

#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES            += system/libhidl/base/include
LOCAL_C_INCLUDES            += system/libfmq/base/
LOCAL_C_INCLUDES            += system/unwinding/libbacktrace/include/
LOCAL_C_INCLUDES            += frameworks/native/libs/binder/ndk

LOCAL_HEADER_LIBRARIES      += libcutils_headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES      += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES      += qcrilIntermodulemsgs-headers
LOCAL_HEADER_LIBRARIES      += libcutils_headers
LOCAL_HEADER_LIBRARIES      += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES      += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES      += libril-db-headers
LOCAL_HEADER_LIBRARIES      += qtiRilHalUtil-headers
LOCAL_HEADER_LIBRARIES      += vendor_common_inc
LOCAL_HEADER_LIBRARIES      += libtime_genoff_headers
LOCAL_HEADER_LIBRARIES      += libdataqmiservices_headers_ext

utf_inc := $(patsubst -I%, $(LOCAL_PATH)/%, $(INC))
LOCAL_C_INCLUDES            += $(utf_inc)

LOCAL_SHARED_LIBRARIES      := libqmiservices
LOCAL_SHARED_LIBRARIES      += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES      += libqmi_encdec
LOCAL_SHARED_LIBRARIES      += libsqlite
LOCAL_SHARED_LIBRARIES      += libqmi_common_so
LOCAL_SHARED_LIBRARIES      += libcutils
LOCAL_SHARED_LIBRARIES      += libutils
LOCAL_SHARED_LIBRARIES      += libbase
LOCAL_SHARED_LIBRARIES      += liblog
LOCAL_SHARED_LIBRARIES      += libfakeservicemanager
LOCAL_SHARED_LIBRARIES      += libbinder
LOCAL_SHARED_LIBRARIES      += libbinder_ndk
LOCAL_SHARED_LIBRARIES      += librilqmimiscservices
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.ims-V16-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim_remote_client-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.am-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qcrilhook-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.voice-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.sim-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.network-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.modem-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.messaging-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.data-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.config-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio-V2-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qtiradio-V9-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qtiradioconfig-V6-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim_remote_server-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.lpa-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.sap-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.secure_element-V1-ndk

LOCAL_CFLAGS += "-D__INTRODUCED_IN(n)="
LOCAL_CFLAGS += "-D__assert(a,b,c)="
LOCAL_CFLAGS += "-D__ANDROID_API__=10000"

LOCAL_MODULE                := libqcril-nr-utf-fwk
LOCAL_MODULE_OWNER          := qti
LOCAL_PROPRIETARY_MODULE    := true
LOCAL_MODULE_TAGS           := optional

# if not coverage
ifneq ($(UTF_TEST_COVERAGE), true)
LOCAL_SANITIZE_DIAG := address cfi
LOCAL_SANITIZE      := address cfi
endif

include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)
build_for_android := true
include $(LOCAL_PATH)/hal_master_make
LOCAL_SRC_FILES := $(cpp_sources)
LOCAL_CPP_EXTENSION := .cpp

LOCAL_CFLAGS    := $(FEATURES) -ferror-limit=0 -g -fno-omit-frame-pointer -O0
LOCAL_CXXFLAGS  := $(FEATURES) -g -fno-omit-frame-pointer -std=c++17 -O0
LOCAL_CPPFLAGS  := $(FEATURES) -g -fno-omit-frame-pointer -std=c++17 -O0

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

ifeq ($(UTF_TEST_COVERAGE), true)
LOCAL_CFLAGS          += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CXXFLAGS        += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CPPFLAGS        += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_LDFLAGS         += -fprofile-arcs -ftest-coverage --coverage
endif

#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES            += system/libhidl/base/include
LOCAL_C_INCLUDES            += system/libfmq/base/
LOCAL_C_INCLUDES            += system/unwinding/libbacktrace/include/
LOCAL_C_INCLUDES            += frameworks/native/libs/binder/ndk
LOCAL_C_INCLUDES            += frameworks/native/libs/fakeservicemanager

LOCAL_HEADER_LIBRARIES      += libcutils_headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES      += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES      += qcrilIntermodulemsgs-headers
LOCAL_HEADER_LIBRARIES      += libcutils_headers
LOCAL_HEADER_LIBRARIES      += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES      += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES      += libril-db-headers
LOCAL_HEADER_LIBRARIES      += qtiRilHalUtil-headers
LOCAL_HEADER_LIBRARIES      += vendor_common_inc
LOCAL_HEADER_LIBRARIES      += libtime_genoff_headers
LOCAL_HEADER_LIBRARIES      += libdataqmiservices_headers_ext

utf_inc := $(patsubst -I%, $(LOCAL_PATH)/%, $(INC))
LOCAL_C_INCLUDES            += $(utf_inc)

LOCAL_SHARED_LIBRARIES      := libqmiservices
LOCAL_SHARED_LIBRARIES      += libqmi_encdec
LOCAL_SHARED_LIBRARIES      += libsqlite
LOCAL_SHARED_LIBRARIES      += libqmi_common_so
LOCAL_SHARED_LIBRARIES      += libcutils
LOCAL_SHARED_LIBRARIES      += libutils
LOCAL_SHARED_LIBRARIES      += libbase
LOCAL_SHARED_LIBRARIES      += liblog
LOCAL_SHARED_LIBRARIES      += libfakeservicemanager
LOCAL_SHARED_LIBRARIES      += libbinder
LOCAL_SHARED_LIBRARIES      += libbinder_ndk
LOCAL_SHARED_LIBRARIES      += librilqmimiscservices
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.ims-V16-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim_remote_client-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.am-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qcrilhook-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.voice-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.sim-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.network-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.modem-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.messaging-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.data-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.config-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio-V2-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qtiradio-V9-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qtiradioconfig-V6-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim_remote_server-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.lpa-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.sap-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.secure_element-V1-ndk

LOCAL_CFLAGS += "-D__INTRODUCED_IN(n)="
LOCAL_CFLAGS += "-D__assert(a,b,c)="
LOCAL_CFLAGS += "-D__ANDROID_API__=10000"
LOCAL_CFLAGS += -DRIL_DATA_UTF

LOCAL_MODULE                := libqcril-data-utf-fwk
LOCAL_MODULE_OWNER          := qti
LOCAL_PROPRIETARY_MODULE    := true
LOCAL_MODULE_TAGS           := optional

ifneq ($(UTF_TEST_COVERAGE), true)
LOCAL_SANITIZE_DIAG := address cfi
LOCAL_SANITIZE      := address cfi
endif

include $(BUILD_HOST_STATIC_LIBRARY)


include $(CLEAR_VARS)

build_for_android := true

LOCAL_CPP_EXTENSION := .cpp
include $(LOCAL_PATH)/hal_master_make
LOCAL_SRC_FILES := $(qcrilnr_cpp_source) $(qcril_data_sources) $(cpp_test_source)



LOCAL_CFLAGS                := $(FEATURES) -ferror-limit=0 -g  -fno-omit-frame-pointer -O0
LOCAL_CXXFLAGS              := $(FEATURES) -g  -fno-omit-frame-pointer -std=c++17 -O0
LOCAL_CPPFLAGS              := $(FEATURES) -g  -fno-omit-frame-pointer -fexceptions -std=c++17 -O0
LOCAL_LDFLAGS               := -lrt -g  -ldl

ifeq ($(UTF_TEST_COVERAGE), true)
LOCAL_CFLAGS                += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CXXFLAGS              += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_CPPFLAGS              += -fprofile-arcs -ftest-coverage --coverage -DUTF_TEST_COVERAGE=true
LOCAL_LDFLAGS               += -fprofile-arcs -ftest-coverage --coverage
endif

LOCAL_C_INCLUDES            += $(LOCAL_PATH)/../../qcril-common/interfaces/inc
LOCAL_C_INCLUDES            += $(LOCAL_PATH)/../../qcril-common/marshalling/inc
LOCAL_C_INCLUDES            += $(LOCAL_PATH)/../../qcril-common/marshalling/platform/linux/inc

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CPPFLAGS += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS += -DRIL_FOR_LOW_RAM
endif

#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
#LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES            += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES            += system/libhidl/base/include
LOCAL_C_INCLUDES            += system/libfmq/base/
LOCAL_C_INCLUDES            += system/unwinding/libbacktrace/include/
LOCAL_C_INCLUDES            += frameworks/native/libs/binder/ndk

LOCAL_HEADER_LIBRARIES      += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES      += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES      += qcrilIntermodulemsgs-headers
LOCAL_HEADER_LIBRARIES      += libcutils_headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES      += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES      += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES      += libril-db-headers
LOCAL_HEADER_LIBRARIES      += libqcrilNr-cmas-headers
LOCAL_HEADER_LIBRARIES      += qtiRilHalUtil-headers
LOCAL_HEADER_LIBRARIES      += vendor_common_inc
LOCAL_HEADER_LIBRARIES      += libtime_genoff_headers
LOCAL_HEADER_LIBRARIES      += libdataqmiservices_headers_ext

utf_inc := $(patsubst -I%, $(LOCAL_PATH)/%, $(INC))
LOCAL_C_INCLUDES            += $(utf_inc)
#LOCAL_ADDITIONAL_DEPENDENCIES += $(HOST_OUT)/bin/QtiMarshallingTest
#LOCAL_ADDITIONAL_DEPENDENCIES += $(HOST_OUT)/bin/QtiRilFwkTest
#LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_VENDOR)/etc/qcril_database/qcrilNr.db

LOCAL_REQUIRED_MODULES += QtiMarshallingTest
LOCAL_REQUIRED_MODULES += QtiRilFwkTest
LOCAL_REQUIRED_MODULES += qcrilNrDb_host

ifneq ($(UTF_TEST_COVERAGE), true)
LOCAL_SANITIZE_DIAG := address cfi
LOCAL_SANITIZE      := address cfi
endif

LOCAL_MODULE                := qcrilnr_utf_test.bin
LOCAL_MODULE_OWNER          := qti
LOCAL_PROPRIETARY_MODULE    := true
LOCAL_MODULE_TAGS           := optional

LOCAL_WHOLE_STATIC_LIBRARIES += libqcril-nr-utf-fwk
LOCAL_WHOLE_STATIC_LIBRARIES += qcrilNrQtiRadio
LOCAL_STATIC_LIBRARIES      := libqcrilNr
LOCAL_STATIC_LIBRARIES      += libimsqmiservices
LOCAL_STATIC_LIBRARIES      += libdataqmiservices
LOCAL_STATIC_LIBRARIES      += librilqmiservices
LOCAL_STATIC_LIBRARIES      += libqcrilNrQtiBus
LOCAL_STATIC_LIBRARIES      += qtiril-utils

LOCAL_SHARED_LIBRARIES      += qtiwakelock
LOCAL_SHARED_LIBRARIES      += qcrilMarshal
LOCAL_SHARED_LIBRARIES      += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES      += libqmiservices
LOCAL_SHARED_LIBRARIES      += libqmi_encdec
LOCAL_SHARED_LIBRARIES      += libsqlite
LOCAL_SHARED_LIBRARIES      += libQtiRilLoadable
LOCAL_SHARED_LIBRARIES      += libqmi_common_so
LOCAL_SHARED_LIBRARIES      += libcutils
LOCAL_SHARED_LIBRARIES      += libutils
LOCAL_SHARED_LIBRARIES      += libbase
LOCAL_SHARED_LIBRARIES      += liblog
LOCAL_SHARED_LIBRARIES      += libfakeservicemanager
LOCAL_SHARED_LIBRARIES      += libbinder
LOCAL_SHARED_LIBRARIES      += libbinder_ndk
LOCAL_SHARED_LIBRARIES      += librilqmimiscservices
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.ims-V16-ndk
LOCAL_SHARED_LIBRARIES      += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim_remote_client-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.am-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qcrilhook-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.voice-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.sim-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.network-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.modem-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.messaging-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.data-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.config-V2-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio-V2-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qtiradio-V9-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.qtiradioconfig-V6-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim_remote_server-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.lpa-V1-ndk
LOCAL_SHARED_LIBRARIES      += vendor.qti.hardware.radio.uim-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.radio.sap-V1-ndk
LOCAL_SHARED_LIBRARIES      += android.hardware.secure_element-V1-ndk

LOCAL_CFLAGS += "-D__INTRODUCED_IN(n)="
LOCAL_CFLAGS += "-D__assert(a,b,c)="
LOCAL_CFLAGS += "-D__ANDROID_API__=10000"


include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_PREBUILT_EXECUTABLES := qcrilnr_utf_test
LOCAL_MODULE_OWNER := qti
LOCAL_IS_HOST_MODULE := true
include $(BUILD_HOST_PREBUILT)

#endif
