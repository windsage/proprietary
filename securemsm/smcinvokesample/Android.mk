QC_PROP_ROOT ?= vendor/qcom/proprietary
SSG_MINK_PROJECT_PATH := $(TOP)/$(QC_PROP_ROOT)/ssg/mink

ifneq ($(wildcard $(SSG_MINK_PROJECT_PATH)/.),)
ifeq ($(call is-board-platform-in-list, msm8998 apq8098_latv sdm660 sdm845 sdm710 msmnile sdmshrike $(MSMSTEPPE) lito atoll bengal $(TRINKET)),true)

include $(call all-subdir-makefiles)

endif # is-board-platform-in-list
endif # SSG_MINK_PROJECT_PATH exists
