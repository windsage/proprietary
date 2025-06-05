PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-pitti.sh \
	init.kernel.post_boot-pitti_default_6_2.sh \
	init.kernel.post_boot-pitti_4_1.sh \
	init.kernel.post_boot-pitti_4_0.sh \
	init.qti.kernel.debug-pitti.sh \
	init.qti.kernel.early_debug-pitti.sh \
	dcc_extension.sh \
	init.kernel.early_boot-memory.sh \
	init.kernel.post_boot-memory.sh


TARGET_OUT_INTERMEDIATES := out/target/product/$(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX)/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
