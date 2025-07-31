ifneq ($(TARGET_DISABLE_PERF_OPTIMIZATIONS),true)
SOONG_CONFIG_perf_QMAA_ENABLE_PERF_STUB_HALS := false

#IOP Feature
ifneq ($(call is-board-platform-in-list, msm8937 msm8953 msm8998 qcs605 sdm660 sdm710 ),true)
PRODUCT_PACKAGES += \
    vendor.qti.hardware.iop@2.0 \
    vendor.qti.hardware.iop@2.0-service.rc \
    vendor.qti.hardware.iop@2.0-service \
    libqti-iopd \
    libqti-iopd-client
endif

# QC_PROP_ROOT is already set when we reach here:
# PATH for reference: QC_PROP_ROOT := vendor/qcom/proprietary
$(call inherit-product-if-exists, $(QC_PROP_ROOT)/android-perf/profiles.mk)

#LM Feature
ifneq ($(call is-board-platform-in-list, msm8953 sdm660 sdm845 ),true)
PRODUCT_PACKAGES += \
    liblearningmodule \
    libmeters \
    libreffeature \
    ReferenceFeature.xml \
    AdaptLaunchFeature.xml \
    AppClassifierFeature.xml \
    GameOptimizationFeature.xml
endif

# add for perfconfig submodule by chao.xu5 at Jul 22nd, 2025 start.
ifeq ($(call is-board-platform-in-list, volcano),true)
PRODUCT_PACKAGES += \
    libperfconfigfeature \
    perf_config.xml \
    perfconfig_manager2_test \
    PerfConfigFeature.xml
endif
# add for perfconfig submodule by chao.xu5 at Jul 22nd, 2025 start.

ifeq ($(call is-board-platform-in-list, kalama anorak pineapple sun pitti anorak61),true)
PRODUCT_PACKAGES += \
    libqape_oem_ext
endif

# For Pixel Targets.
# ODM partition will be created and below rc files will go to ODM partition in pixel targets to disable IOP service.
ifeq ($(GENERIC_ODM_IMAGE),true)
PRODUCT_PACKAGES += \
    init.pixel.vendor.qti.hardware.iop@2.0-service.rc
endif

ifeq ($(call is-board-platform-in-list, pitti pitti_32go ),true)
# ZRAM writeback settings
PRODUCT_PROPERTY_OVERRIDES += \
    ro.zram.mark_idle_delay_mins=5    \
    ro.zram.first_wb_delay_mins=10    \
    ro.zram.periodic_wb_delay_hours=1
endif
endif
