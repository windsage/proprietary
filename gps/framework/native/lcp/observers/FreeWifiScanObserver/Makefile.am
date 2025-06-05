AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     -I./inc \
     -I./inc/internal \
     -I../../utils/inc \
     -I../../utils/inc/internal \
     -D_HAS_LOC_V02_ \
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

libwifiobserver_la_SOURCES = \
    src/FreeWifiScanObserver.cpp

if USE_GLIB
libwifiobserver_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libwifiobserver_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libwifiobserver_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libwifiobserver_la_CFLAGS = $(AM_CFLAGS)
libwifiobserver_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libwifiobserver_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libwifiobserver_la_LIBADD = -lstdc++ $(GPSUTILS_LIBS) $(LOWICLIENT_LIBS) $(LOCBASEUTIL_LIBS) \
                $(LOCCORE_LIBS)

library_include_HEADERS = \
    inc/internal/FreeWifiScanObserver.h \
    inc/IFreeWifiScanObserver.h \
    inc/IFreeWifiScanSubscription.h \
    inc/IWifiScanInitiator.h

#Create and Install libraries
lib_LTLIBRARIES = libwifiobserver.la
library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = wifiobserver.pc
EXTRA_DIST = $(pkgconfig_DATA)
