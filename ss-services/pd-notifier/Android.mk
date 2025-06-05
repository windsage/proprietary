PD_NOTIFIER_ROOT := $(call my-dir)
LOCAL_PATH := $(PD_NOTIFIER_ROOT)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := libpdnotifier
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_SRC_FILES := libpdnotifier/libpdnotifier.c \
	pd-notifier-idl/service_registry_notifier_v01.c
LOCAL_HEADER_LIBRARIES := vendor_common_inc \
                          libpdnotifier_headers \
                          libqmi_common_headers
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libpdnotifier_headers
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
ifeq ($(TARGET_PD_SERVICE_ENABLED), true)
	LOCAL_CFLAGS += -DLIBPDNOTIFIER_ENABLED
endif
include $(BUILD_SHARED_LIBRARY)
