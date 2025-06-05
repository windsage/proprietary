AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(POWERSTATESERVICE_CFLAGS) \
     $(LOCATIONAPI_CFLAGS) \
     -I./ \
     -D__STDC_FORMAT_MACROS \
     -Dstrlcpy=g_strlcpy \
     -Dstrlcat=g_strlcat \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

libgnsspowerhandler_h_sources = GnssPowerHandler.h
libgnsspowerhandler_c_sources = GnssPowerHandler.cpp

#le-auto
if USE_FEATURE_AUTOMOTIVE
AM_CFLAGS += $(POWERMNGR_CFLAGS) -DFEATURE_AUTOMOTIVE
endif

libgnsspowerhandler_la_SOURCES = \
    $(libgnsspowerhandler_h_sources) \
    $(libgnsspowerhandler_c_sources)

if USE_GLIB
libgnsspowerhandler_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@ -include glib.h
libgnsspowerhandler_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libgnsspowerhandler_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@ -include glib.h
else
libgnsspowerhandler_la_CFLAGS = $(AM_CFLAGS)
libgnsspowerhandler_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libgnsspowerhandler_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libgnsspowerhandler_la_LIBADD = -ldl -lstdc++ $(GPSUTILS_LIBS) $(LOCATIONAPI_LIBS)

#openwrt - libs
if USE_OPENWRT
libgnsspowerhandler_la_LIBADD += -lpowerstateclient
endif

#le-auto - libs
if USE_FEATURE_AUTOMOTIVE
libgnsspowerhandler_la_LIBADD += $(POWERMNGR_LIBS)
endif

library_include_HEADERS = GnssPowerHandler.h

#Create and Install libraries
lib_LTLIBRARIES = libgnsspowerhandler.la
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(libgnsspowerhandler_h_sources)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnss-powerhandler.pc
EXTRA_DIST = $(pkgconfig_DATA)

