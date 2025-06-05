# QMAA mode configuration
ifeq ($(TARGET_USES_QMAA),true)
ifneq ($(TARGET_USES_QMAA_OVERRIDE_KMGK),true)
KMGK_USE_QTI_SERVICE := false
ENABLE_KM_4_0 := false
KMGK_USE_QTI_STUBS := false
ENABLE_STRONGBOX_KM := false
else
ifneq ($(TARGET_USES_GY),true)
KMGK_USE_QTI_SERVICE := true
else
KMGK_USE_QTI_SERVICE := false
endif
ENABLE_KEYMINT := true
ENABLE_KM_SPLIT_MANIFEST := true
ENABLE_STRONGBOX_KM := false
ENABLE_SPU_GK := false
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
endif #TARGET_USES_QMAA_OVERRIDE_KMGK
else # Non-QMAA mode configuration

# Strait2.5/blair specific defines
ifeq ($(TARGET_BOARD_PLATFORM),blair)
KMGK_USE_QTI_SERVICE := true
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := false
# GATEKEEPER_IS_SECURITY_LEVEL_SPU := true
endif

# lanai/pineapple specific defines
ifeq ($(TARGET_BOARD_PLATFORM),pineapple)
KMGK_USE_QTI_SERVICE := true
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := false
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
ENABLE_SPU_GK := true
endif

# Halliday.3.0 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),anorak61)
KMGK_USE_QTI_SERVICE := false
ENABLE_KEYMINT := false
ENABLE_STRONGBOX_KM := false
endif

# Milos/volcano specific defines
ifeq ($(TARGET_BOARD_PLATFORM),volcano)
KMGK_USE_QTI_SERVICE := true
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := false
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
ENABLE_SPU_GK := false
endif

ifeq ($(TARGET_BOARD_PLATFORM),pitti)
KMGK_USE_QTI_SERVICE := true
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := false
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
ENABLE_SPU_GK := false
endif

# kailua/kalama specific defines
ifeq ($(TARGET_BOARD_PLATFORM),kalama)
KMGK_USE_QTI_SERVICE := true
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := true
endif

# waipio/taro specific defines
ifeq ($(TARGET_BOARD_PLATFORM),taro)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KEYMINT := true
ENABLE_STRONGBOX_KM := true
endif

# lahaina/sm8350 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),lahaina)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_STRONGBOX_KM := true
METADATA_ENC_FORMAT_VERSION_2 := true
ENABLE_AUTH_SECRET := true
endif

ifeq ($(TARGET_BOARD_PLATFORM),holi)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
METADATA_ENC_FORMAT_VERSION_2 := true
endif

# kona/sm8250 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),kona)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_STRONGBOX_KM := true
ADOPTABLE_STORAGE_SUPPORTED := true
METADATA_ENC_FORMAT_VERSION_2 := true
ENABLE_AUTH_SECRET := true
endif

# lito specific defines
ifeq ($(TARGET_BOARD_PLATFORM),lito)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
METADATA_ENC_FORMAT_VERSION_2 := true
endif

# atoll specific defines
ifeq ($(TARGET_BOARD_PLATFORM),atoll)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
endif

# Bengal specific defines
ifeq ($(TARGET_BOARD_PLATFORM),bengal)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
ENABLE_KEYMINT := true
#METADATA_ENC_FORMAT_VERSION_2 := true
endif

# msmnile/sm8150 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msmnile)
ifneq ($(TARGET_USES_GY),true)
KMGK_USE_QTI_SERVICE := true
else
KMGK_USE_QTI_SERVICE := false
endif
ifeq ($(TARGET_USES_KM40),true)
ENABLE_KM_4_0 := true
else
ENABLE_KEYMINT := true
endif
ifneq ($(ENABLE_HYP), true)
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
endif #ENABLE_HYP
ifneq ($(TARGET_BOARD_AUTO), true)
ENABLE_STRONGBOX_KM := true
endif #TARGET_BOARD_AUTO
endif

# sdmshrike specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdmshrike)
KMGK_USE_QTI_SERVICE := true
ifneq ($(ENABLE_HYP), true)
ENABLE_KEYMINT := true
ENABLE_KM_4_1 := true
else
ENABLE_KM_4_0 := true
endif #ENABLE_HYP
ifneq ($(TARGET_BOARD_AUTO), true)
ENABLE_STRONGBOX_KM := true
endif #TARGET_BOARD_AUTO
endif

# sdm845 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdm845)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
endif

# trinket specific defines
ifeq ($(TARGET_BOARD_PLATFORM),$(TRINKET))
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_1 := true
ENABLE_KM_SPLIT_MANIFEST := true
endif

# talos specific defines
ifeq ($(TARGET_BOARD_PLATFORM),$(MSMSTEPPE))
KMGK_USE_QTI_SERVICE := true
ifneq ($(ENABLE_HYP), true)
ENABLE_KEYMINT := true
GATEKEEPER_IS_SECURITY_LEVEL_SPU := false
#ENABLE_KM_4_1 := true
#ENABLE_KM_SPLIT_MANIFEST := true
else
ENABLE_KM_4_0 := true
endif #ENABLE_HYP
endif

# sdm710 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdm710)
KMGK_USE_QTI_SERVICE := true
ENABLE_KM_4_0 := true
ADOPTABLE_STORAGE_SUPPORTED := true
endif

# sdm660 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),sdm660)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ENABLE_KM_4_0 := true
endif

# msm8998 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8998)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ENABLE_KM_4_0 := true
endif

# msm8996 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8996)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ifeq ($(strip $(TARGET_KERNEL_VERSION)), 4.4)
ENABLE_KM_4_0 := true
endif
endif

# msm8937 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8937)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ifeq ($(strip $(TARGET_KERNEL_VERSION)), 4.9)
ENABLE_KM_4_0 := true
endif
endif

# msm8953 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8953)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
ifeq ($(strip $(TARGET_KERNEL_VERSION)), 4.9)
ENABLE_KM_4_0 := true
endif
endif

# msm8909 specific defines
ifeq ($(TARGET_BOARD_PLATFORM),msm8909)
ifeq ($(ENABLE_VENDOR_IMAGE),true)
KMGK_USE_QTI_SERVICE := true
endif
endif

 # Monaco specific defines
ifeq ($(TARGET_BOARD_PLATFORM),monaco)
KMGK_USE_QTI_SERVICE := false
ENABLE_KM_4_1 := false
ENABLE_KEYMINT := false
ENABLE_KM_SPLIT_MANIFEST := false
METADATA_ENC_FORMAT_VERSION_2 := true
ENABLE_QB_HANDLER := false
ADOPTABLE_STORAGE_SUPPORTED := true
endif

# Aurora.la.3.0 defines
ifeq ($(TARGET_BOARD_PLATFORM),neo61)
KMGK_USE_QTI_SERVICE := true
ENABLE_KEYMINT := true
ENABLE_KEYMINT_LOGGING := false
endif

endif #TARGET_USES_QMAA
