ifeq ($(TARGET_PD_SERVICE_ENABLED),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_MODULE := pd-api-test
LOCAL_SRC_FILES := pd-api-test.c
LOCAL_HEADER_LIBRARIES := vendor_common_inc \
                          libqmi_common_headers
LOCAL_SHARED_LIBRARIES += libcutils \
			  libutils \
			  liblog \
			  libpdmapper \
			  libqmi_cci \
			  libqmi_common_so \
			  libqmi_encdec \
			  libpdnotifier
LOCAL_MODULE_TAG := tests
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA_NATIVE_TESTS)
include $(BUILD_EXECUTABLE)
endif
