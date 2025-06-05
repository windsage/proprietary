ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
ifeq ($(WEAR_POWER_SERVICE),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libgnsswear_power

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    GnssWearPowerHandler.cpp \

LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libhidlbase \
    libcutils \
    libutils \
    libgps.utils \
    liblocation_api \
    libbase \
    vendor.qti.hardware.powerstateservice@1.0 \

include $(BUILD_SHARED_LIBRARY)

endif # ifeq ($(WEAR_POWER_SERVICE),true)
endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
