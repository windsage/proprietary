PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.qti.kernel.early_ipd.sh \
	init.kernel.post_boot-anorak61.sh \
	init.kernel.post_boot-anorak61_default_2_4.sh \
	init.qti.kernel.debug-anorak61.sh \
	init.qti.kernel.ipd-calib-anorak61.sh \
	init.qti.kernel.early_debug-anorak61.sh

TARGET_OUT_INTERMEDIATES := out/target/product/anorak61/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
