ifneq ($(TARGET_BUILD_VARIANT), user)
LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE := wifimyftm

LOCAL_SRC_FILES:= \
    wifimyftm.cpp \
    service.cpp

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libbase \
    liblog \
    libutils \
    libnl \
    libc

#Below check needs to be updated for all future Android platform versions
ifeq ($(filter $(PLATFORM_VERSION),14 UpsideDownCake 15 VanillaIceCream),$(PLATFORM_VERSION))
LOCAL_SHARED_LIBRARIES += \
    vendor.qti.hardware.wifi.wifimyftm-V1-ndk \
    libbinder_ndk
LOCAL_CFLAGS := -DCONFIG_MYFTM_USE_AIDL
else
LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    libhidlmemory \
    android.hidl.memory@1.0 \
    android.hidl.allocator@1.0 \
    vendor.qti.hardware.wifi.wifimyftm@1.0
endif

LOCAL_MODULE_OWNER := qti
LOCAL_SANITIZE := integer_overflow
ifeq ($(filter $(PLATFORM_VERSION),14 UpsideDownCake 15 VanillaIceCream),$(PLATFORM_VERSION))
LOCAL_INIT_RC := vendor.qti.hardware.wifi.wifimyftm-service.rc
LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.wifi.wifimyftm-service.xml
else
LOCAL_INIT_RC := vendor.qti.hardware.wifi.wifimyftm@1.0-service.rc
LOCAL_VINTF_FRAGMENTS := vendor.qti.hardware.wifi.wifimyftm@1.0-service.xml
endif

include $(BUILD_EXECUTABLE)

endif
