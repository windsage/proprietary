#Strait 2.5 specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),blair)
TARGET_ENABLE_QSEECOM := true
TARGET_ENABLE_QSEECOMCOMPAT := false
endif

#Lanai specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),pineapple)
TARGET_ENABLE_QSEECOM := false
TARGET_ENABLE_QSEECOMCOMPAT := true
endif

#Halliday.3.0 specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),anorak61)
TARGET_ENABLE_QSEECOM := false
TARGET_ENABLE_QSEECOMCOMPAT := false
endif

#Aurora.la.3.0 specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),neo61)
TARGET_ENABLE_QSEECOM := false
TARGET_ENABLE_QSEECOMCOMPAT := false
endif

#Milos specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),volcano)
TARGET_ENABLE_QSEECOM := false
TARGET_ENABLE_QSEECOMCOMPAT := true
endif

#pakala specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),sun)
TARGET_ENABLE_QSEECOM := false
TARGET_ENABLE_QSEECOMCOMPAT := true
endif

ifeq ($(TARGET_BOARD_PLATFORM),niobe)
TARGET_ENABLE_QSEECOM := false
TARGET_ENABLE_QSEECOMCOMPAT := true
endif

#Hana gen3 specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),msmnile)
TARGET_ENABLE_QSEECOM := true
TARGET_ENABLE_QSEECOMCOMPAT := false
endif

#Talos AU specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),$(MSMSTEPPE))
TARGET_ENABLE_QSEECOM := true
TARGET_ENABLE_QSEECOMCOMPAT := false
endif
