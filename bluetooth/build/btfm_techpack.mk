include vendor/qcom/proprietary/bluetooth/build/bt-vendor-proprietary-product.mk
include vendor/qcom/opensource/bt-kernel/bt_kernel_vendor_board.mk
.PHONY: btfm_tp btfm_tp_package btfm_tp_dlkm
btfm_tp: btfm_tp_package btfm_tp_dlkm
btfm_tp_dlkm: $(BT_KERNEL_DRIVER)
btfm_tp_package: $(BTFM_PACKAGES) $(BTFM_PACKAGES_DEBUG)
$(warning "BTFM Techpack configuration TARGET_USES_QMAA_OVERRIDE_DISPLAY  = $(btfm_tp_package)")
