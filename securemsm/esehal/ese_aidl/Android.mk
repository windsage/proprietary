ifeq ($(strip $(TARGET_USES_GPQESE)),true)
LOCAL_PATH := $(call my-dir)
# set below flag to enable boot time update.
DISABLE_BOOT_TIME_UPDATE := true

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := android.hardware.secure_element-service.qti
LOCAL_INIT_RC := android.hardware.secure_element-service.qti.rc
LOCAL_C_INCLUDES := $(SMSM_PATH)/GPTEE/inc \
                    $(SMSM_PATH)/esehal \
                    $(SMSM_PATH)/esehal/ese_aidl \
                    $(SMSM_PATH)/esehal/EseUtils \
                    $(SMSM_PATH)/QSEEComAPI \
                    $(SMSM_PATH)/esehal/gpqese \
                    $(SMSM_PATH)/esehal/OsuHal_aidl/inc

LOCAL_SRC_FILES := \
    main.cpp \
    OsuHelper.cpp \
    ../OsuHal_aidl/src/OsuHalExtn.cpp

LOCAL_SHARED_LIBRARIES := \
        libQSEEComAPI \
        libEseUtils \
        libGPQeSE \
        libbase \
        liblog \
        libutils \
        libbinder_ndk \
        android.hardware.secure_element-V1-ndk \

LOCAL_SANITIZE := cfi integer_overflow

include $(BUILD_EXECUTABLE)
endif
