LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblbs_core

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    LBSAdapterBase.cpp \
    LBSApiBase.cpp \
    LBSProxy.cpp \
    LocApiProxy.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    libizat_core \
    libloc_core \
    libgps.utils \
    libcrypto \
    libdl \
    libsqlite \
    libloc_base_util

LOCAL_HEADER_LIBRARIES := \
    vendor_common_inc \
    libgps.utils_headers \
    libloc_core_headers \
    izat_remote_api_headers \
    libloc_pla_headers \
    liblocation_api_headers \
    libloc_api_v02_headers \
    libqmi_common_headers \
    libqmi_cci_headers \
    libdiag_headers \
    libdataqmiservices_headers \
    libizat_core_headers

LOCAL_SRC_FILES += \
    LBSApiV02.cpp

LOCAL_SHARED_LIBRARIES += \
    libloc_api_v02

LOCAL_CFLAGS+= \
    -D_HAS_LOC_V02_

ifneq ($(FEATURE_MODEM_POWER_VOTE),false)
$(call print-vars, $(TARGET_DEVICE))
ifneq ($(FEATURE_PDMAPPER),false)
LOCAL_SHARED_LIBRARIES += \
    libmdmdetect \
    libperipheral_client \
    libpdmapper

LOCAL_HEADER_LIBRARIES += \
    libmdmdetect_headers \
    libperipheralclient_headers

LOCAL_CFLAGS += \
    -DPDMAPPER_AVAILABLE

else #FEATURE_PDMAPPER
LOCAL_SHARED_LIBRARIES += \
    libmdmdetect \
    libperipheral_client

LOCAL_HEADER_LIBRARIES += \
    libmdmdetect_headers \
    libperipheralclient_headers

endif #FEATURE_PDMAPPER

LOCAL_CFLAGS += \
    -DMODEM_POWER_VOTE
endif

LOCAL_CFLAGS+=$(GPS_FEATURES) \
    -D_ANDROID_ \
    -DON_TARGET_TEST

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)
