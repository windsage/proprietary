LIMITS_STAT_PRODUCT_LIST += lahaina
LIMITS_STAT_PRODUCT_LIST += taro
LIMITS_STAT_PRODUCT_LIST += kalama
LIMITS_STAT_PRODUCT_LIST += pineapple

ifeq ($(call is-board-platform-in-list,$(LIMITS_STAT_PRODUCT_LIST)),true)
	PRODUCT_PACKAGES_DEBUG += limits_stat.ko
endif
