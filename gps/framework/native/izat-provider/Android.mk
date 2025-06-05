LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libizatprovider
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := src/IzatProvider.cpp
LOCAL_SRC_FILES += src/AltitudeReceiverProxy.cpp
LOCAL_SRC_FILES += src/ZlocationResponseHandler.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libgps.utils \
    liblocation_api \
    liblocationservice \
    libloc_base_util \
    libdataitems \
    libloc_core

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_HEADER_LIBRARIES)
LOCAL_HEADER_LIBRARIES := \
    vendor_common_inc \
    libutils_headers \
    libgps.utils_headers \
    liblocation_api_headers \
    liblocationservice_headers \
    izat_remote_api_prop_headers \
    izat_remote_api_headers \
    libloc_core_headers \
    liblbs_core_headers \
    libloc_base_util_headers \
    libloc_mq_client_headers \
    libdataitems_headers \
    libloc_pla_headers \

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
