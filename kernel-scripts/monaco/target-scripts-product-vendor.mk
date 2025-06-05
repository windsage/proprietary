PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-monaco.sh \
	init.qti.kernel.debug-monaco.sh \
	init.qti.kernel.early_debug-monaco.sh

TARGET_OUT_INTERMEDIATES := out/target/product/monaco$(TARGET_BOARD_SUFFIX)/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
