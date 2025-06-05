QC_PROP_ROOT := vendor/qcom/proprietary
PRODUCT_PACKAGES += \
	init.qti.kernel.rc \
	init.qti.kernel.sh \
	vendor_modprobe.sh \
	system_dlkm_modprobe.sh \
	init.qti.kernel.test.rc \
	init.qti.write.sh \
	early_eth.sh
$(call inherit-product, $(QC_PROP_ROOT)/kernel-scripts/$(TARGET_BOARD_PLATFORM)/target-scripts-product-vendor.mk)
