#DIAG
DIAG := libdiag
DIAG += diag_qshrink4_daemon
DIAG += libdiagjni

ifneq ($(TARGET_BOARD_AUTO),true)
DIAG += diag_callback_sample
DIAG += diag_dci_sample
DIAG += diag_klog
DIAG += diag_mdlog
DIAG += diag_socket_log
DIAG += diag_uart_log
DIAG += PktRspTest
DIAG += test_diag
else
DIAG_DBG += diag_callback_sample
DIAG_DBG += diag_dci_sample
DIAG_DBG += diag_klog
DIAG_DBG += diag_mdlog
DIAG_DBG += diag_socket_log
DIAG_DBG += diag_uart_log
DIAG_DBG += PktRspTest
DIAG_DBG += test_diag
endif

ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
DIAG += vendor.qti.diag_userdebug.rc
else
DIAG += vendor.qti.diag.rc
endif

ifeq ($(TARGET_HAS_DIAG_ROUTER),true)
DIAG += diag-router
DIAG += vendor.qti.diaghal@1.0.vendor
DIAG += debug-diag
endif

PRODUCT_PACKAGES += $(DIAG)

#DIAG_DBG
DIAG_DBG := diag_buffering_test
DIAG_DBG += diag_callback_client
DIAG_DBG += diag_dci_client

PRODUCT_PACKAGES_DEBUG += $(DIAG_DBG)
