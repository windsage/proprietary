PRODUCT_PACKAGES += \
	init.kernel.post_boot.sh \
	init.kernel.post_boot-sa8155.sh \
	init.kernel.post_boot-sa8195.sh

TARGET_OUT_INTERMEDIATES := out/target/product/$(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX)$(TARGET_BOARD_DERIVATIVE_SUFFIX)/obj
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr:
	mkdir -p $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
