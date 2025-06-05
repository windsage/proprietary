PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-volcano.sh \
	init.qti.kernel.debug-volcano.sh \
	init.kernel.post_boot-volcano_default_4_3_1.sh \
	init.kernel.post_boot-volcano_3_2_1.sh \
	init.kernel.post_boot-volcano_2_2_1.sh \
	init.kernel.post_boot-volcano_4_3_0.sh \
	init.kernel.post_boot-volcano_4_2_1.sh \
        init.kernel.post_boot-volcano_3_3_1.sh \
	init.qti.kernel.early_debug-volcano.sh \
	init.kernel.early_boot-memory.sh \
	init.kernel.post_boot-memory.sh

TARGET_OUT_INTERMEDIATES := out/target/product/volcano/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
