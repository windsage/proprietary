PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-pineapple.sh \
	init.kernel.post_boot-pineapple_default_2_3_2_1.sh \
	init.kernel.post_boot-pineapple_2_3_1_1.sh \
	init.kernel.post_boot-pineapple_1_2_2_1.sh \
	init.kernel.post_boot-pineapple_1_3_1_1.sh \
	init.kernel.post_boot-pineapple_2_3_2_0.sh \
	init.kernel.post_boot-pineapple_2_2_2_0.sh \
	init.qti.kernel.debug-pineapple.sh \
	init.qti.kernel.early_debug-pineapple.sh \
	init.kernel.post_boot-cliffs.sh \
	init.kernel.post_boot-cliffs_default_3_4_1.sh \
	init.kernel.post_boot-cliffs_3_3_1.sh \
	init.kernel.post_boot-cliffs_2_3_0.sh \
	init.kernel.post_boot-cliffs_2_2_1.sh \
	init.qti.kernel.debug-cliffs.sh \
	dcc_extension.sh \
	init.kernel.early_boot-memory.sh \
	init.kernel.post_boot-memory.sh

TARGET_OUT_INTERMEDIATES := out/target/product/pineapple/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
