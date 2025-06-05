PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-niobe.sh \
	init.kernel.post_boot-niobe_default_2_4.sh \
	init.kernel.post_boot-niobe_2_3.sh \
	init.qti.kernel.debug-niobe.sh \
	init.qti.kernel.early_debug-niobe.sh\
	dcc_extension.sh \
	init.kernel.early_boot-memory.sh \
	init.kernel.post_boot-memory.sh

TARGET_OUT_INTERMEDIATES := out/target/product/niobe/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
