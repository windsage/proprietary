LOCAL_PATH := $(call my-dir)
PREBUILT_DIR_PATH := $(LOCAL_PATH)

ifeq ($(call is-board-platform,pineapple),true)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_64/Android.mk
-include $(PREBUILT_DIR_PATH)/target/product/pineapple/Android.mk
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM),qssi)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),)
-include $(PREBUILT_DIR_PATH)/target/product/qssi/Android.mk
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM),qssi)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),_32go)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_32go/Android.mk
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM),qssi)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),_64)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_64/Android.mk
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM),qssi)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),_lite)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_lite/Android.mk
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM),qssi)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),_xrl)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_xrl/Android.mk
endif
endif

ifeq ($(TARGET_BOARD_PLATFORM),qssi)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),_xrM)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_xrM/Android.mk
endif
endif

ifeq ($(call is-board-platform,volcano),true)
ifeq ($(strip $(TARGET_BOARD_SUFFIX)),)
-include $(PREBUILT_DIR_PATH)/target/product/qssi_64/Android.mk
-include $(PREBUILT_DIR_PATH)/target/product/volcano/Android.mk
endif
endif

-include $(sort $(wildcard $(PREBUILT_DIR_PATH)/*/*/Android.mk))
