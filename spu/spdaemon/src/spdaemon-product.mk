ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list, lahaina taro),true)

SECUREMSM += init.spdaemon.rc

PRODUCT_PACKAGES += $(SECUREMSM)

endif
endif
