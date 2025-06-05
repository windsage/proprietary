ifeq ($(strip $(ENABLE_PERIPHERAL_STATE_UTILS)), true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES := smcinvoke_headers smcinvoke_no_ship_headers mink_headers peripheralstate_headers

LOCAL_SHARED_LIBRARIES := libminksocket_vendor \
                          liblog \
                          libutils \
                          libcutils \
                          libbase \
                          libminkdescriptor

LOCAL_SRC_FILES := src/peripheralStateUtils.c

LOCAL_MODULE := libPeripheralStateUtils
LOCAL_CFLAGS := -O3
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)
endif
