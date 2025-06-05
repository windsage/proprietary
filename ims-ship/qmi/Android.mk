LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES :=  libimsqmiservices_headers
LOCAL_HEADER_LIBRARIES +=  libqmi_headers
LOCAL_HEADER_LIBRARIES +=  vendor_common_inc
LOCAL_HEADER_LIBRARIES +=  libqmi_common_headers

LOCAL_EXPORT_HEADER_LIBRARY_HEADERS :=  libimsqmiservices_headers

LOCAL_SHARED_LIBRARIES := libidl

LOCAL_SRC_FILES += qmi_ims_vt_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_video_telephony_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_presence_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_settings_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_application_v01.c
LOCAL_SRC_FILES += ip_multimedia_subsystem_ims_common_v01.c

LOCAL_MODULE:= libimsqmiservices
LOCAL_SANITIZE:=integer_overflow

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_HOST_STATIC_LIBRARY)
