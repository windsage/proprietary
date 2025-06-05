#Lahaina specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),lahaina)
TARGET_USES_GPQESE := true
endif

#Waipio specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),taro)
TARGET_USES_GPQESE := true
endif

#Kailua specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),kalama)
TARGET_USES_GPQESE := true
# Enable this flag in case strongbox, weaver
# solutions use OMAPI services for APDU exchange
# Also set secure_element_vintf_enabled as true in
# resource-overlay/kalama/SecureElement/res/values/config.xml
ENABLE_OMAPI_VINTF := true
endif

#Mannar specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),holi)
TARGET_USES_GPQESE := true
endif

#Bengal specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),bengal)
TARGET_USES_GPQESE := true
endif
#Strait 2.5 specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),blair)
TARGET_USES_GPQESE := true
# Enable this flag in case strongbox, weaver
# solutions use OMAPI services for APDU exchange
# Also set secure_element_vintf_enabled as true in
# resource-overlay/blair/SecureElement/res/values/config.xml
ENABLE_OMAPI_VINTF := true
#use GPQTEEC by default
TARGET_USES_GPQTEEC :=true
endif

#Lanai specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),pineapple)
TARGET_USES_GPQESE := true
# Enable this flag in case strongbox, weaver
# solutions use OMAPI services for APDU exchange
# Also set secure_element_vintf_enabled as true in
# resource-overlay/pineapple/SecureElement/res/values/config.xml
ENABLE_OMAPI_VINTF := true
endif

#Milos specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),volcano)
TARGET_USES_GPQESE := true
# Enable this flag in case strongbox, weaver
# solutions use OMAPI services for APDU exchange
# Also set secure_element_vintf_enabled as true in
# resource-overlay/pineapple/SecureElement/res/values/config.xml
ENABLE_OMAPI_VINTF := true
endif

#Pakala specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),sun)
TARGET_USES_GPQESE := true
# Enable this flag in case strongbox, weaver
# solutions use OMAPI services for APDU exchange
# Also set secure_element_vintf_enabled as true in
# resource-overlay/pineapple/SecureElement/res/values/config.xml
ENABLE_OMAPI_VINTF := true
endif

#Disable GPQESE for automotive targets
ifeq ($(TARGET_BOARD_AUTO),true)
TARGET_USES_GPQESE := false
endif

#Halliday/anorak61 specific build flag
ifeq ($(TARGET_BOARD_PLATFORM),anorak61)
TARGET_USES_GPQESE := true
# Enable this flag in case strongbox, weaver
# solutions use OMAPI services for APDU exchange
# Also set secure_element_vintf_enabled as true in
# resource-overlay/anorak61/SecureElement/res/values/config.xml
#ENABLE_OMAPI_VINTF := true
ENABLE_ESE_STRONGBOX_PROVISION_LIB := false
endif
