PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-crow.sh \
	init.qti.kernel.debug-crow.sh \
	init.qti.kernel.early_debug-crow.sh

TARGET_OUT_INTERMEDIATES := out/target/product/crow/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
