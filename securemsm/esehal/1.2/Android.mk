LOCAL_PATH := $(call my-dir)

# set below flag to enable boot time update.
DISABLE_BOOT_TIME_UPDATE := true

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.secure_element@1.2-service
LOCAL_INIT_RC := vendor.qti.secure_element@1.2-service.rc
LOCAL_C_INCLUDES := $(SMSM_PATH)/GPTEE/inc \
                    $(SMSM_PATH)/esepowermanager/1.1 \
                    $(SMSM_PATH)/esehal \
                    $(SMSM_PATH)/esehal/ese-clients/inc \
                    $(SMSM_PATH)/esehal/OsuHal/inc \
                    $(SMSM_PATH)/QSEEComAPI

LOCAL_SRC_FILES := \
    service.cpp \
    interface.cpp \
    ../OsuHal/src/OsuHalExtn.cpp \
    SecureElement.cpp \
    SecureElementHalCallback.cpp

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
# Set ESE_CLIENT_LIB_PRESENT flag in below line, to use OSU or SEMS at boot time.
LOCAL_CFLAGS := -DESE_CLIENT_LIB_PRESENT=true
ifeq ($(DISABLE_BOOT_TIME_UPDATE),true)
LOCAL_HEADER_LIBRARIES := libese_client_headers
LOCAL_SRC_FILES += \
    ../ese-clients/src/eSEClient.cpp
endif
else
LOCAL_CFLAGS := -DESE_CLIENT_LIB_PRESENT=false
endif

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libhardware \
    libQSEEComAPI

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
ifeq ($(DISABLE_BOOT_TIME_UPDATE),true)
LOCAL_SHARED_LIBRARIES += \
    se_nq_extn_client \
    ls_nq_client \
    jcos_nq_client
endif
endif
LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    android.hardware.secure_element@1.0 \
    android.hardware.secure_element@1.1 \
    android.hardware.secure_element@1.2 \
    libGPQeSE \
    libEseUtils

LOCAL_SANITIZE := cfi integer_overflow
#Enable below line during debug
#LOCAL_SANITIZE_DIAG := cfi integer_overflow

include $(BUILD_EXECUTABLE)
