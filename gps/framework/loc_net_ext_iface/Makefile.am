AM_CFLAGS = \
     -I./ \
     -I./le \
     $(GPSUTILS_CFLAGS)

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -std=c++11

if USE_TELSDK
AM_CPPFLAGS += \
       $(TELUX_CFLAGS)
endif

if USE_TELSDK
libloc_net_ext_iface_la_SOURCES = \
     le/LocNetExt.cpp \
     le/LocNetTelSdkIface.cpp
endif

if USE_GLIB
libloc_net_ext_iface_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_net_ext_iface_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_net_ext_iface_la_LDFLAGS += -Wl,--export-dynamic
libloc_net_ext_iface_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_net_ext_iface_la_CFLAGS = $(AM_CFLAGS)
libloc_net_ext_iface_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_net_ext_iface_la_LDFLAGS += -Wl,--export-dynamic
libloc_net_ext_iface_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_net_ext_iface_la_LIBADD = $(GPSUTILS_LIBS)
if USE_TELSDK
libloc_net_ext_iface_la_LIBADD += -ltelux_data -ltelux_common -ltelux_tel
endif

if USE_TELSDK
library_include_HEADERS = \
    le/LocNetExtIface.h \
    le/LocNetExt.h \
    le/LocNetTelSdkIface.h \
    le/ExtSecChainValidationIface.h
endif

#Create and Install libraries
lib_LTLIBRARIES = libloc_net_ext_iface.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-net-ext-iface.pc
EXTRA_DIST = $(pkgconfig_DATA)
