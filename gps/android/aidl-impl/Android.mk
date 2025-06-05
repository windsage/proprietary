ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.gnss-service

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_VINTF_FRAGMENTS := vendor.qti.gnss-service.xml

LOCAL_SRC_FILES := \
    LocAidlGnss.cpp \
    LocAidlFlpService.cpp \
    LocAidlIzatProvider.cpp \
    LocAidlDebugReportService.cpp \
    LocAidlAGnss.cpp \
    LocAidlGnssNi.cpp \
    LocAidlWiFiDBProvider.cpp \
    LocAidlWiFiDBReceiver.cpp \
    LocAidlWWANDBProvider.cpp \
    LocAidlWWANDBReceiver.cpp \
    LocAidlEsStatusReceiver.cpp \
    LocAidlGnssConfigService.cpp \
    LocAidlRilInfoMonitor.cpp \
    LocAidlGeofenceService.cpp \
    LocAidlIzatConfig.cpp \
    LocAidlIzatSubscription.cpp \
    LocAidlGeocoder.cpp \
    LocAidlQesdkTracking.cpp \
    location_api/LocAidlFlpClient.cpp \
    location_api/LocAidlGeofenceClient.cpp \
    location_api/LocAidlNiClient.cpp \
    location_api/LocAidlUtils.cpp \
    service.cpp

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/location_api \
    $(LOCAL_PATH)/../

LOCAL_HEADER_LIBRARIES := \
    izat_remote_api_headers \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    liblocationservice_glue_headers \
    liblocationservice_headers \
    libdataitems_headers \
    izat_remote_api_prop_headers \
    libxtadapter_headers \
    liblbs_core_headers \
    libizat_core_headers \
    vendor_common_inc \
    libloc_mq_client_headers \
    liblocation_qesdk_headers \
    libloc_base_util_headers \

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libbinder_ndk \
    vendor.qti.gnss-V6-ndk \
    liblog \
    libcutils \
    libutils

LOCAL_SHARED_LIBRARIES += \
    libloc_core \
    libgps.utils \
    libdl \
    liblocation_api \
    liblocationservice_glue \
    liblbs_core \
    libdataitems \
    liblocationservice \
    libxtadapter \
    libizat_core \
    libizat_client_api \
    libloc_base_util \
    libloc_mq_client

LOCAL_CFLAGS += $(GNSS_CFLAGS)

ifeq ($(PRODUCT_ENABLE_QESDK),true)
LOCAL_CFLAGS += -DQESDK_ENABLED
endif

include $(BUILD_SHARED_LIBRARY)

endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
