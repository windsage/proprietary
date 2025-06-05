ifeq ($(TARGET_PERF_DIR),)
    PERF_CONFIG_DIR := $(TARGET_BOARD_PLATFORM)
else
    PERF_CONFIG_DIR := $(TARGET_PERF_DIR)
endif

PRODUCT_COPY_FILES += \
					  $(foreach file,$(wildcard vendor/qcom/proprietary/android-perf/configs/$(PERF_CONFIG_DIR)/lm/*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/lm/$(notdir $(file)))
