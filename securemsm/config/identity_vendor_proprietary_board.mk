IDENTITY_USE_QTI_SERVICE := true
#Set IDENTITY_USE_QTI_SERVICE false for Gen-3 SP's Hana/Poipu/Talos.
ifeq ($(TARGET_BOARD_AUTO),true)
ifeq ($(filter $(TARGET_BOARD_PLATFORM), sm6150 msmnile),$(TARGET_BOARD_PLATFORM)
IDENTITY_USE_QTI_SERVICE := false
endif
endif
