LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers
LOCAL_HEADER_LIBRARIES += libqmi_legacy_headers
LOCAL_HEADER_LIBRARIES += libqmi_ext_noship_headers
LOCAL_HEADER_LIBRARIES += vendor_common_inc
LOCAL_SHARED_LIBRARIES := libidl
LOCAL_SHARED_LIBRARIES += libqmiservices

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libqmi_ext_noship_headers

LOCAL_SRC_FILES := services/voice_service_common_v01.c
LOCAL_SRC_FILES += services/network_access_service_ext_v01.c
LOCAL_SRC_FILES += services/voice_service_ims_ext_v02.c

LOCAL_MODULE:= libqmiextservices

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
