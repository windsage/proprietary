PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-seraph.sh \
	init.qti.kernel.debug-seraph.sh \
	init.qti.kernel.early_debug-seraph.sh\

TARGET_OUT_INTERMEDIATES := out/target/product/seraph/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
