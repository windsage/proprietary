ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,pineapple niobe),true)

LOCAL_PATH:= $(call my-dir)
SPU_SHIP_PATH := vendor/qcom/proprietary/spu
COMMON_SHIP_PATH := vendor/qcom/proprietary/common
SECUREMSM_SHIP_COMMONSYS_PATH := vendor/qcom/proprietary/commonsys/securemsm
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(SECUREMSM_SHIP_COMMONSYS_PATH)/QSEEComAPI \
                    $(SPU_SHIP_PATH)/sp_license_sample \
                    $(COMMON_SHIP_PATH)/inc
LOCAL_HEADER_LIBRARIES := vendor_common_inc


LOCAL_SHARED_LIBRARIES := \
        libc \
        libcutils \
        libutils \
        liblog \
        libQSEEComAPI \
        libdl \
        libdmabufheap

LOCAL_MODULE := sp_license_sample
LOCAL_SRC_FILES := sp_license_sample.c
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

endif
endif
