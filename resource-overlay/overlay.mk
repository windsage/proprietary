#include target specific overlays
OVERLAY_PATH_VND := vendor/qcom/proprietary/resource-overlay
-include $(OVERLAY_PATH_VND)/$(TARGET_BOARD_PLATFORM)$(TARGET_BOARD_SUFFIX)/OverlayBoardConfig.mk
