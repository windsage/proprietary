ifeq ($(ENABLE_TRUSTED_UI_VM_3_0),true)
TOP_LOCAL_PATH:= $(call my-dir)
include $(call all-subdir-makefiles)

include $(CLEAR_VARS)

LOCAL_PATH:= $(TOP_LOCAL_PATH)

LOCAL_MODULE := libTrustedInputUtils

LOCAL_SRC_FILES := TUITouchInputUtils.cpp

LOCAL_CFLAGS := -g -O3 -Wno-unused-parameter -fno-operator-names
LOCAL_CFLAGS += -D_VENDOR_QTI_
LOCAL_CFLAGS += -D__LINUX__


LOCAL_SHARED_LIBRARIES := liblog \
                          libutils \
                          libcutils

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_SHARED_LIBRARY)
endif
