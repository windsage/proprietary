PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-neo61.sh \
	init.kernel.post_boot-neo61_default_4.sh \
	init.qti.kernel.debug-neo61.sh \
	init.qti.kernel.early_debug-neo61.sh

TARGET_OUT_INTERMEDIATES := out/target/product/neo61/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
