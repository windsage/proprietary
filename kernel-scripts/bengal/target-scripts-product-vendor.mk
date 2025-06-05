PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.kernel.post_boot-bengal.sh \
	init.kernel.post_boot-bengal-iot.sh \
	init.qti.kernel.debug-bengal.sh

TARGET_OUT_INTERMEDIATES := out/target/product/bengal_515/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
