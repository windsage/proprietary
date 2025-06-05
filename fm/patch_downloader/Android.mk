ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),arm arm64))
ifeq ($(BOARD_HAVE_QCOM_FM),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(call is-board-platform,msm8660),true)
  LOCAL_CFLAGS += -DBOARD_MSM8x60
endif

LOCAL_CFLAGS += -Werror -Wall -Wextra

LOCAL_MODULE := fm_qsoc_patches
LOCAL_CLANG := true

ifeq ($(PLATFORM_VERSION), P)
LOCAL_HEADER_LIBRARIES += libril-qc-qmi-services-headers
endif

LOCAL_SHARED_LIBRARIES := libqmi libqmi_cci libqmiservices libcutils
LOCAL_SRC_FILES := fm_qsoc_patches.c
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qcom
LOCAL_MODULE_PATH :=  $(TARGET_OUT_VENDOR_EXECUTABLES)
include $(BUILD_EXECUTABLE)

endif

endif
