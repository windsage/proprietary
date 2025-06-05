ifeq ($(filter $(TARGET_BOARD_PLATFORM), anorak kalama pineapple niobe), $(TARGET_BOARD_PLATFORM))
PRODUCT_PACKAGES += fan_service
endif
