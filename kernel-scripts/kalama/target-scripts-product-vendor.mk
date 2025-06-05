PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-kalama.sh \
	init.qti.kernel.debug-kalama.sh \
	init.qti.kernel.early_debug-kalama.sh

TARGET_OUT_INTERMEDIATES := out/target/product/kalama$(TARGET_BOARD_SUFFIX)/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
