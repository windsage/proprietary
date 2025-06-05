LOCAL_PATH := $(call my-dir)

#Build qcrilDataModule

###################### Target ########################################
include $(CLEAR_VARS)

LOCAL_MODULE               := libqcrilDataModule
LOCAL_SANITIZE             := integer_overflow
LOCAL_MODULE_OWNER         := qti
LOCAL_PROPRIETARY_MODULE   := true
LOCAL_MODULE_TAGS          := optional

LOCAL_CFLAGS               += -Wall -Werror -Wno-macro-redefined -Wno-unused-parameter -Wno-reorder-ctor -Wno-unused-result -DFEATURE_DATA_LOG_QXDM -DFEATURE_QDP_LINUX_ANDROID
LOCAL_CXXFLAGS             += -std=c++17 -Wno-unused-parameter -Wno-reorder-ctor

LOCAL_VINTF_FRAGMENTS += android.hardware.radio.data.xml


# Build with ASAN and unresolved reference flags enabled
ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
LOCAL_LDFLAGS              += -Wl,--no-allow-shlib-undefined,--unresolved-symbols=report-all
LOCAL_SANITIZE             :=address
endif

LOCAL_SRC_FILES            := $(call all-cpp-files-under, datamodule/module/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, endpoints/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, authmanager/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, profilehandler/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, lcehandler/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, networkservicehandler/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, util/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, callmanager/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, tmdmanager/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, aidl/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, KeepAliveManager/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, iwlanservice/src)
LOCAL_SRC_FILES            += $(call all-cpp-files-under, networkavailabilityhandler/src)

LOCAL_VINTF_FRAGMENTS      += vendor.qti.hardware.data.iwlandata.xml
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.data.iwlandata-V1-ndk

ifeq ($(TARGET_HAS_LOW_RAM),true)
LOCAL_CFLAGS               += -DRIL_FOR_LOW_RAM
LOCAL_CXXFLAGS             += -DRIL_FOR_LOW_RAM
else
LOCAL_SRC_FILES            += $(call all-cpp-files-under, dataconnectionservice/src)
LOCAL_SHARED_LIBRARIES     += android.hardware.radio@1.6
LOCAL_SHARED_LIBRARIES     += vendor.qti.hardware.data.connectionaidl-V1-ndk
LOCAL_VINTF_FRAGMENTS      += vendor.qti.hardware.data.connectionaidl.xml
LOCAL_VINTF_FRAGMENTS      += dataconnection-saidl.xml
endif


#Dont compile DsiWrapperLE.cpp for Android Build
LOCAL_SRC_FILES := $(filter-out %DsiWrapperLE.cpp,$(LOCAL_SRC_FILES))

# Local
LOCAL_CFLAGS += -DFEATURE_DATA_EMBMS
LOCAL_CFLAGS += -DFEATURE_DATA_LQE

LOCAL_STATIC_LIBRARIES += libqcrilNRDataInternal
LOCAL_STATIC_LIBRARIES += libqcrilNRDataInterfaces
LOCAL_STATIC_LIBRARIES += qcrilIntermodulemsgs

LOCAL_SHARED_LIBRARIES += libqdpr
LOCAL_SHARED_LIBRARIES += libdsi_netctrl
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libbase
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libbinder_ndk
LOCAL_SHARED_LIBRARIES += libqcrilNrFramework
LOCAL_SHARED_LIBRARIES += libqcrilNrLogger
LOCAL_SHARED_LIBRARIES += libril-db
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiBus
LOCAL_SHARED_LIBRARIES += libqcrilNrQtiMutex
LOCAL_SHARED_LIBRARIES += qcrilNrQmiModule
LOCAL_SHARED_LIBRARIES += qtiril-utils
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += libdsutils
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += qtiwakelock
LOCAL_SHARED_LIBRARIES += android.hardware.radio-V2-ndk
LOCAL_SHARED_LIBRARIES += android.hardware.radio.data-V2-ndk

LOCAL_HEADER_LIBRARIES += qcrilDataModule_headers
LOCAL_HEADER_LIBRARIES += qcrilDataInternal_headers
LOCAL_HEADER_LIBRARIES += qcrilDataInterfaces_headers
LOCAL_HEADER_LIBRARIES += qcrilIntermodulemsgs-headers
LOCAL_HEADER_LIBRARIES += qcrilInterfaces-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrQtiMutex-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrLogger-headers
LOCAL_HEADER_LIBRARIES += libqcrilNrFramework-headers
LOCAL_HEADER_LIBRARIES += qtiril-utils-headers
LOCAL_HEADER_LIBRARIES += libdsi_netctrl_headers
LOCAL_HEADER_LIBRARIES += libqcrilNrQtiBus-headers
LOCAL_HEADER_LIBRARIES += libqmi_common_headers
LOCAL_HEADER_LIBRARIES += libqmi_headers
LOCAL_HEADER_LIBRARIES += libdiag_headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc
LOCAL_HEADER_LIBRARIES += libdataqmiservices_headers
LOCAL_HEADER_LIBRARIES += libdataqmiservices_headers_ext
LOCAL_HEADER_LIBRARIES += libqmi_cci_headers
LOCAL_HEADER_LIBRARIES += libdsutils_headers
LOCAL_HEADER_LIBRARIES += libqdpr_headers
LOCAL_HEADER_LIBRARIES += libdatactl_headers
LOCAL_HEADER_LIBRARIES += qtiwakelock-headers
LOCAL_HEADER_LIBRARIES += mink_headers
LOCAL_HEADER_LIBRARIES += peripheralstate_headers

$(info Done building qcrilDataModule for target...)

include $(BUILD_SHARED_LIBRARY)
