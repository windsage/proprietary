AM_CFLAGS = \
     -I./ \
     -I./le \
     $(LOCPLA_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(GPSUTILS_CFLAGS) \
     $(DATAITEMS_CFLAGS)

# Qcmap related Cflags
if USE_QCMAP
    AM_CFLAGS += $(QMIF_CFLAGS) \
     $(QMI_CFLAGS) \
     $(DATA_CFLAGS) \
     $(DSUTILS_CFLAGS) \
     -DUSE_QCMAP
endif

# WpaClient related Cflags
if USE_WPACLIENT
    AM_CFLAGS += -DUSE_WPACLIENT \
        -I./wpaclient \
        -I$(WORKSPACE)/external/wpa_supplicant_8/src/common/ \
        -I$(WORKSPACE)/external/wpa_supplicant_8/src/drivers \
        -I$(WORKSPACE)/external/wpa_supplicant_8/src/utils
endif

if HAS_MODEM_SUPPORT
    AM_CFLAGS += $(LOCWDS_CFLAGS)
endif

ACLOCAL_AMFLAGS = -I m4

if USE_DSDA
    AM_CFLAGS += -DFEATURE_DSDA_ENABLED
endif

AM_CPPFLAGS = -std=c++11

# Sources
libloc_net_iface_la_SOURCES = \
     LocNetIfaceBase.cpp \
     LocNetIfaceAgps.cpp

if USE_QCMAP
libloc_net_iface_la_SOURCES += le/LocNetIface.cpp
endif

if USE_WPACLIENT
libloc_net_iface_la_SOURCES += wpaclient/LocNetIfaceWpaClient.cpp
endif

if USE_GLIB
libloc_net_iface_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_net_iface_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_net_iface_la_LDFLAGS += -Wl,--export-dynamic
libloc_net_iface_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_net_iface_la_CFLAGS = $(AM_CFLAGS)
libloc_net_iface_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_net_iface_la_LDFLAGS += -Wl,--export-dynamic
libloc_net_iface_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

# Lib add's
libloc_net_iface_la_LIBADD = -lstdc++ $(GPSUTILS_LIBS) $(DATAITEMS_LIBS) $(LOCCORE_LIBS)

if USE_QCMAP
libloc_net_iface_la_LIBADD += $(QMIF_LIBS) $(DATA_LIBS) -lqmiservices -lqcmap_client
endif

if USE_WPACLIENT
libloc_net_iface_la_LIBADD += -lwpa_client
endif

if HAS_MODEM_SUPPORT
libloc_net_iface_la_LIBADD += $(LOCWDS_LIBS)
endif

# Include headers
library_include_HEADERS = \
    LocNetIfaceBase.h \
    LocNetIfaceAgps.h

if USE_QCMAP
library_include_HEADERS += le/LocNetIface.h
endif

if USE_WPACLIENT
library_include_HEADERS += wpaclient/LocNetIfaceWpaClient.h
endif

#Create and Install libraries
lib_LTLIBRARIES = libloc_net_iface.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-net-iface.pc
EXTRA_DIST = $(pkgconfig_DATA)
