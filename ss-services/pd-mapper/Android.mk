PD_MAPPER_ROOT := $(call my-dir)
LOCAL_PATH := $(PD_MAPPER_ROOT)

ifeq ($(TARGET_PD_SERVICE_ENABLED),true)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := pd-mapper
LOCAL_C_INCLUDES += $(LOCAL_PATH)/pd-mapper-svc \
		    external/connectivity/stlport/stlport
LOCAL_SRC_FILES := pd-mapper-svc/pd-mapper.cpp \
	pd-mapper-idl/service_registry_locator_v01.c
LOCAL_HEADER_LIBRARIES := libcutils_headers \
                          vendor_common_inc \
                          libpdmapper_headers \
                          libqmi_common_headers
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libjson \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec \
			  libqmi_csi
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
endif

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := libpdmapper
ifeq ($(TARGET_PD_SERVICE_ENABLED),true)
	LOCAL_CFLAGS += -DLIBPDMAPPER_SUPPORTED
else
$(warning libpdmapper not supported.Compiling stubbed version)
endif
LOCAL_SRC_FILES := libpdmapper/libpdmapper.c \
	pd-mapper-idl/service_registry_locator_v01.c
LOCAL_HEADER_LIBRARIES := vendor_common_inc \
                          libpdmapper_headers \
                          libqmi_common_headers
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libjson \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec
LOCAL_EXPORT_HEADER_LIBRARY_HEADERS := libpdmapper_headers
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
