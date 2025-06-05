PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-blair.sh \
	init.qti.kernel.debug-blair.sh \
	init.qti.kernel.early_debug-blair.sh \
	init.kernel.post_boot-holi.sh \
	init.qti.kernel.debug-holi.sh \
	init.qti.kernel.early_debug-holi.sh

TARGET_OUT_INTERMEDIATES := out/target/product/blair/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
