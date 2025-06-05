PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-parrot.sh \
	init.qti.kernel.debug-parrot.sh \
	init.qti.kernel.early_debug-parrot.sh \
	init.kernel.post_boot-ravelin.sh \
	init.qti.kernel.debug-ravelin.sh

TARGET_OUT_INTERMEDIATES := out/target/product/parrot/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
