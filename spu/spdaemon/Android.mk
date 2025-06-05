ifneq ($(QMAA_DISABLES_SPU),true)
ifneq ($(ENABLE_HYP),true)
include $(call all-subdir-makefiles)
endif
endif