LOCAL_PATH := $(call my-dir)

HIDL_INTERFACE_VERSION = 1.0
INCLUDES = $(LOCAL_PATH)
INCLUDES += external/libnl/include

########################
include $(CLEAR_VARS)
LOCAL_MODULE := wifilearner
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_CPPFLAGS := -Wall -Werror
LOCAL_CFLAGS := -Wall -Werror -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libutils \
    liblog \
    libnl \
    libc

#Below check needs to be updated for all future Android platform versions
ifeq ($(filter $(PLATFORM_VERSION),15 VanillaIceCream),$(PLATFORM_VERSION))
INCLUDES += $(LOCAL_PATH)/aidl
LOCAL_SHARED_LIBRARIES += \
    vendor.qti.hardware.wifi.wifilearner-V1-ndk \
    libbinder_ndk
LOCAL_CFLAGS := -DCONFIG_WIFILEARNER_USE_AIDL
else
INCLUDES += $(LOCAL_PATH)/hidl/$(HIDL_INTERFACE_VERSION)
LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    libhidlmemory \
    android.hidl.memory@1.0 \
    android.hidl.allocator@1.0 \
    vendor.qti.hardware.wifi.wifilearner@1.0
endif

LOCAL_C_INCLUDES := $(INCLUDES)

LOCAL_SRC_FILES := \
    main.c \
    utils.c \
    nl_utils.c

ifeq ($(filter $(PLATFORM_VERSION),15 VanillaIceCream),$(PLATFORM_VERSION))
LOCAL_SRC_FILES += \
    aidl/aidl.cpp \
    aidl/wifistats.cpp
LOCAL_INIT_RC := vendor.qti.hardware.wifi.wifilearner-service.rc
LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.wifi.wifilearner-service.xml
else
LOCAL_SRC_FILES += \
    hidl/$(HIDL_INTERFACE_VERSION)/hidl.cpp \
    hidl/$(HIDL_INTERFACE_VERSION)/wifistats.cpp
LOCAL_INIT_RC := vendor.qti.hardware.wifi.wifilearner@1.0-service.rc
LOCAL_SANITIZE := integer_overflow
endif

include $(BUILD_EXECUTABLE)

