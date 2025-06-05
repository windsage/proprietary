PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-kona.sh \
        init.kernel.post_boot-qcs7230.sh \
        init.kernel.post_boot-qrb3165.sh \
        init.qti.kernel.debug-kona.sh \
	init.qti.kernel.early_debug-kona.sh

TARGET_OUT_INTERMEDIATES := out/target/product/kona$(TARGET_BOARD_SUFFIX)/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
