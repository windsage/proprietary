ACLOCAL_AMFLAGS = -I m4

SECUREMSM_SHIP_PATH := $(WORKSPACE)/security/securemsm
SECUREMSM_PATH := $(WORKSPACE)/security/securemsm-noship

AM_CPPFLAGS = -I$(SECUREMSM_SHIP_PATH)/TrustedUI/TrustedVM/libTrustedUI/inc \
              -I$(SECUREMSM_PATH)/CommonLib/inc \
              -I$(SECUREMSM_PATH)/qtvm_sdk/libs/libmodule/inc \
              -I$(SECUREMSM_SHIP_PATH)/smcinvoke/minksocket/include \
              -I$(SECUREMSM_SHIP_PATH)/smcinvoke/minksocket/sock \
              -I$(SECUREMSM_SHIP_PATH)/mink/inc/interface \
              -I${PKG_CONFIG_SYSROOT_DIR}/usr/lib/sdk/ext/inc \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/securemsm-noship \
              -I${PKG_CONFIG_SYSROOT_DIR}/${includedir}/commonsys-intf-securemsm \
              -fno-gnu-unique

C_SOURCES = src/TrustedUILib.cpp \
            src/TUIInputCallback.cpp \
            src/MINKHelper.cpp

lib_LTLIBRARIES := libTrustedUILib.la

libTrustedUILib_la_SOURCES = $(C_SOURCES)
libTrustedUILib_la_CFLAGS = $(AM_CFLAGS)
libTrustedUILib_la_CPPFLAGS = $(AM_CPPFLAGS)
libTrustedUILib_la_LIBADD = -llog -lcommonlib
pkgconfigdir = $(libdir)/pkgconfig
