PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so \
    ro.vendor.perf-hal.ver=3.0

SOONG_CONFIG_NAMESPACES += perf
SOONG_CONFIG_perf += QMAA_ENABLE_PERF_STUB_HALS
ifneq ($(TARGET_BOARD_AUTO),true)
TARGET_ENABLE_PASR = true
endif
ifeq ($(TARGET_ENABLE_PASR),true)
$(call add_soong_config_var_value,perf,pasr_manager,enabled)
endif
PRODUCT_PACKAGES += \
    vendor.qti.hardware.perf2 \
    vendor.qti.hardware.perf2-hal-service.rc \
    vendor.qti.hardware.perf2-hal-service

ifeq ($(TARGET_DISABLE_PERF_OPTIMIZATIONS),true)
SOONG_CONFIG_perf_QMAA_ENABLE_PERF_STUB_HALS := true
PRODUCT_PROPERTY_OVERRIDES +=\
    vendor.disable.perf.hal=1
endif

ifneq ($(TARGET_DISABLE_PERF_OPTIMIZATIONS),true)
SOONG_CONFIG_perf_QMAA_ENABLE_PERF_STUB_HALS := false
# QC_PROP_ROOT is already set when we reach here:
# PATH for reference: QC_PROP_ROOT := vendor/qcom/proprietary
$(call inherit-product-if-exists, $(QC_PROP_ROOT)/perf-core/profiles.mk)

PRODUCT_PACKAGES += \
    libqti-perfd \
    libqti-perfd-client \
    libqti-util \
    libq-perflog \
    android.hardware.tests.libhwbinder@1.0-impl \
    libperfgluelayer \
    libperfconfig

# For Pixel Targets.
# ODM partition will be created and below rc files will go to ODM partition in pixel targets to disable the perf service.
ifeq ($(GENERIC_ODM_IMAGE),true)
PRODUCT_PACKAGES += \
    init.pixel.vendor.qti.hardware.perf-hal-service.rc
endif

# Pre-render feature
PRODUCT_PROPERTY_OVERRIDES += \
ro.vendor.perf.scroll_opt=1

# Perf Adaptive Frame Pacing
ifeq ($(call is-board-platform-in-list,kalama pineapple),true)
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.perf.framepacing.enable=1
endif

PRODUCT_PACKAGES_DEBUG += \
    perflock_native_test \
    perflock_native_stress_test \
    perfunittests \
    boostConfigParser \
    libqti-tests-mod1 \
    libqti-tests-mod2 \
    libqti-tests-mod3 \
    libqti-tests-mod4 \
    libqti-tests-mod5 \
    libqti-tests-mod6 \
    perflock_native_test_server \
    perflock_native_test_client \
    regressionframework \
    libqti-perfd-tests

endif # TARGET_DISABLE_PERF_OPTIMIZATIONS is false
