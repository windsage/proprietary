PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-sun.sh \
	init.qti.kernel.debug-sun.sh \
	init.qti.kernel.early_debug-sun.sh

TARGET_OUT_INTERMEDIATES := out/target/product/sun/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
