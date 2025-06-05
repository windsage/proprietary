ifneq ($(QMAA_DISABLES_SPU),true)
# verify it isn't a simulator build
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,pineapple niobe),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := spdaemon.c
LOCAL_INIT_RC := init.spdaemon.rc

LOCAL_C_INCLUDES += $(TOP)/vendor/qcom/proprietary/spu/spcomlib/inc
LOCAL_C_INCLUDES += $(TOP)/vendor/qcom/proprietary/spu/sputils/inc
LOCAL_HEADER_LIBRARIES := vendor_common_inc qti_spu_drivers_kernel_headers
LOCAL_HEADER_LIBRARIES += libmdmdetect_headers
LOCAL_HEADER_LIBRARIES += libperipheralclient_headers


LOCAL_SHARED_LIBRARIES := libspcom libutils libcutils liblog libmdmdetect libhardware_legacy

LOCAL_CFLAGS += -DFEATURE_IAR -fno-exceptions -fno-short-enums -DANDROID

ifeq ($(TARGET_BUILD_VARIANT),user)
    LOCAL_CFLAGS+= -DUSER_BUILD
endif

ifeq ($(call is-board-platform-in-list,pineapple),true)
    LOCAL_CFLAGS += -DSPSS_TARGET=8650
endif

ifeq ($(call is-board-platform-in-list,niobe),true)
    LOCAL_CFLAGS += -DSPSS_TARGET=sxr2330p
endif

LOCAL_MODULE := spdaemon
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti

ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
LOCAL_PROPRIETARY_MODULE := true
endif

include $(BUILD_EXECUTABLE)

endif
endif
endif
