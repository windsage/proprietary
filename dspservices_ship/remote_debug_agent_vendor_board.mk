ifneq ($(TARGET_BOARD_AUTO),true)
ifeq ($(call is-board-platform-in-list,sdm710 sdm845 sm6150 trinket msmnile lito atoll),true)
    BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_MODULES_OUT)/rdbg.ko
endif
endif
