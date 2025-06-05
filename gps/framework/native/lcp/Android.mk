#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module - common
#
# GENERAL DESCRIPTION
#   Common location service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblocationservice

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := ../adapter/LBSAdapter.cpp
LOCAL_SRC_FILES += observers/IPCClient/src/IPCClient.cpp
LOCAL_SRC_FILES += observers/IPCHandler/src/IPCHandler.cpp
LOCAL_SRC_FILES += izat-provider-service/src/LocationProvider.cpp
LOCAL_SRC_FILES += izat-provider-service/src/IzatManager.cpp
LOCAL_SRC_FILES += izat-provider-service/src/WiFiLocationProvider.cpp
LOCAL_SRC_FILES += izat-provider-service/src/WWanLocationProvider.cpp
LOCAL_SRC_FILES += izat-listeners/src/IzatPassiveLocationListener.cpp
LOCAL_SRC_FILES += wiper/src/Wiper.cpp
LOCAL_SRC_FILES += utils/src/Utils.cpp

LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libloc_core
LOCAL_SHARED_LIBRARIES += libgps.utils
LOCAL_SHARED_LIBRARIES += liblbs_core
LOCAL_SHARED_LIBRARIES += libdataitems
LOCAL_SHARED_LIBRARIES += liblowi_client
LOCAL_SHARED_LIBRARIES += libizat_core
LOCAL_SHARED_LIBRARIES += liblocation_api
LOCAL_SHARED_LIBRARIES += libloc_base_util
LOCAL_SHARED_LIBRARIES += libloc_mq_client
LOCAL_SHARED_LIBRARIES += liblowi_common
LOCAL_SHARED_LIBRARIES += libwifiobserver

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../service
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../adapter
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat_api
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCHandler/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCHandler/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCClient/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCClient/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/FreeWifiScanObserver/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/FreeWifiScanObserver/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-provider-service/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-provider-service/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-listeners/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-listeners/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utils/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/wiper/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/wiper/inc/internal

LOCAL_HEADER_LIBRARIES := \
    vendor_common_inc \
    libutils_headers \
    libgps.utils_headers \
    libloc_api_v02_headers \
    libloc_core_headers \
    liblbs_core_headers \
    libloc_base_util_headers \
    libloc_mq_client_headers \
    libdataitems_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    libizat_core_headers \
    liblowi_common_headers \
    liblocation_qesdk_headers \
    libqmi_common_headers \
    libqcc_sdk_headers \
    izat_remote_api_headers \
    izat_remote_api_prop_headers

LOCAL_CFLAGS += $(GPS_FEATURES)
LOCAL_CFLAGS += -D_ANDROID_
LOCAL_CFLAGS += -DON_TARGET_TEST
LOCAL_CFLAGS += -D_HAS_LOC_V02_

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libwifiobserver

# activate the following line for debug purposes only, comment out for production
#LOCAL_SANITIZE_DIAG += $(GNSS_SANITIZE_DIAG)
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES = observers/FreeWifiScanObserver/src/FreeWifiScanObserver.cpp

LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libloc_core
LOCAL_SHARED_LIBRARIES += libgps.utils
LOCAL_SHARED_LIBRARIES += libdataitems
LOCAL_SHARED_LIBRARIES += liblowi_client
LOCAL_SHARED_LIBRARIES += liblocation_api
LOCAL_SHARED_LIBRARIES += libloc_base_util
LOCAL_SHARED_LIBRARIES += liblowi_common

LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/FreeWifiScanObserver/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/FreeWifiScanObserver/inc/internal

LOCAL_HEADER_LIBRARIES := \
    vendor_common_inc \
    libutils_headers \
    libgps.utils_headers \
    libloc_api_v02_headers \
    libloc_core_headers \
    liblbs_core_headers \
    libloc_base_util_headers \
    libloc_mq_client_headers \
    libdataitems_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    libizat_core_headers \
    liblowi_common_headers \
    liblowi_client_headers \
    izat_remote_api_headers \
    izat_remote_api_prop_headers

LOCAL_CFLAGS += $(GPS_FEATURES)
LOCAL_CFLAGS += -D_ANDROID_
LOCAL_CFLAGS += -DON_TARGET_TEST

ifneq ($(GNSS_HIDL_VERSION),)
    LOCAL_CFLAGS += -DGNSS_HIDL_VERSION='"$(GNSS_HIDL_VERSION)"'
endif

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)
