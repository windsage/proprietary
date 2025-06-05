ifneq ($(TARGET_NO_TELEPHONY), true)

LOCAL_DIR := $(call my-dir)
QCRIL_NR_DIR := $(LOCAL_DIR)

qcril_cflags := -Oz -g -Wall -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-writable-strings -Wno-error=\#warnings
qcril_cppflags := -Oz -g -Wall -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-writable-strings -Wno-error=\#warnings
qcril_ldflags += -Wl,--no-allow-shlib-undefined,--unresolved-symbols=report-all

ifeq ($(TARGET_HAS_LOW_RAM),true)
qcril_cflags += -DRIL_FOR_LOW_RAM
qcril_cppflags += -DRIL_FOR_LOW_RAM
endif

ifeq ($(TARGET_SUPPORTS_WEAR_AON), true)
qcril_cflags +=  -DRIL_WEARABLES_TARGET_AON
qcril_cppflags +=  -DRIL_WEARABLES_TARGET_AON
endif

ifeq ($(TARGET_SUPPORTS_WEARABLES), true)
qcril_cflags +=  -DRIL_WEARABLES_TARGET
qcril_cppflags +=  -DRIL_WEARABLES_TARGET
endif
# Uncomment the following line to debug CFI crashes
# qcril_sanitize_diag = cfi
#

# Uncomment the following line to enable ASAN
# QCRIL_BUILD_WITH_ASAN=true
# (And Uncomment "//defaults: ["qcril_asan_defaults"]" in Android.bp)
ifeq ($(QCRIL_BUILD_WITH_ASAN),true)
qcril_sanitize := address
qcril_sanitize_diag := address
qcril_cflags += -DRIL_ASAN_ENABLED
qcril_cppflags += -DRIL_ASAN_ENABLED
endif

ifeq ($(ATEL_ENABLE_LLVM_SA),true)
    LLVM_SA_OUTPUT_DIR := $(PRODUCT_OUT)/atel-llvm-sa-results/qcril-nr
    LLVM_SA_FLAG := --compile-and-analyze $(LLVM_SA_OUTPUT_DIR)
    qcril_cflags   += $(LLVM_SA_FLAG)
    qcril_cppflags += $(LLVM_SA_FLAG)
endif

include $(call all-makefiles-under,$(LOCAL_DIR))
endif
