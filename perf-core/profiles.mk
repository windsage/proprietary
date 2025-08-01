ifeq ($(TARGET_PERF_DIR),)
    PERF_CONFIG_DIR := $(TARGET_BOARD_PLATFORM)
else
    PERF_CONFIG_DIR := $(TARGET_PERF_DIR)
endif

PRODUCT_COPY_FILES += \
		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/*.conf),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

# Exclude encrypted files from wildcard matching, they are handled by Android.mk
PRODUCT_COPY_FILES += \
		  $(foreach file,$(filter-out %perfboostsconfig.xml %perfconfigstore.xml,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/perfboosts*.xml)),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/power*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/perfmap*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/target*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/targetresourceconfigs.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/common/common*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

# Remove the perfconfigstore.xml wildcard copy, handled by Android.mk
# PRODUCT_COPY_FILES += \
#		  $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/$(PERF_CONFIG_DIR)/perfconfigstore.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

# Add the encrypted modules to PRODUCT_PACKAGES to ensure they are built
PRODUCT_PACKAGES += \
    perfboostsconfig.xml \
    perfconfigstore.xml

PRODUCT_COPY_FILES += \
                   $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/test/*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
                   $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/test/$(PERF_CONFIG_DIR)/*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))

PRODUCT_COPY_FILES += \
                   $(foreach file,$(wildcard vendor/qcom/proprietary/perf-core/configs/test/common/*.xml),$(file):$(TARGET_COPY_OUT_VENDOR)/etc/perf/$(notdir $(file)))
