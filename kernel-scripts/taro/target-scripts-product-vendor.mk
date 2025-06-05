PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.qti.kernel.debug.sh \
	init.qti.kernel.early_debug.sh \
	init.kernel.post_boot-taro.sh \
	init.qti.kernel.debug-taro.sh \
	init.qti.kernel.early_debug-taro.sh \
	init.kernel.post_boot-diwali.sh \
	init.qti.kernel.debug-diwali.sh \
	init.kernel.post_boot-cape.sh \
	init.qti.kernel.debug-cape.sh

TARGET_OUT_INTERMEDIATES := out/target/product/taro/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
