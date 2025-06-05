ifeq ($(strip $(TARGET_USES_GPQESE)),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libEseUtils
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    EseUtils/EseUtils.cpp

LOCAL_HEADER_LIBRARIES := vendor_common_inc

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \

LOCAL_SANITIZE := cfi integer_overflow

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
QC_PROP_ROOT ?= vendor/qcom/proprietary

SMSM_PATH           := $(QC_PROP_ROOT)/securemsm

COMMON_INCLUDES     :=  $(SMSM_PATH)/GPTEE/inc \
                        $(LOCAL_PATH)/1.2 \
                        $(LOCAL_PATH)/ese-clients/inc \
                        $(SMSM_PATH)/QSEEComAPI

LOCAL_HEADER_LIBRARIES := vendor_common_inc
ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_HEADER_LIBRARIES += libese_client_headers
endif

LOCAL_MODULE := libGPQeSE
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := \
    gpqese/gpqese-be.cpp \
    gpqese/gpqese-apdu.cpp \
    gpqese/gpqese-utils.cpp \
    gpqese/gpqese-jcop.cpp \
    gpqese/gpqese-cold-reset.cpp \

#checking if gpqese client will use GPQTEEC
ifeq ($(TARGET_USES_GPQTEEC),true)
LOCAL_CFLAGS += -DTARGET_USES_GPQTEEC
endif

LOCAL_C_INCLUDES := $(COMMON_INCLUDES)

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libutils \
    liblog \
    libhardware \
    libbase \
    libcutils \
    android.hardware.secure_element@1.0 \
    libGPTEE_vendor \
    libQSEEComAPI \
    libEseUtils

LOCAL_SANITIZE := cfi integer_overflow
#Enable below line during debug
#LOCAL_SANITIZE_DIAG := cfi integer_overflow

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
