# Makefile.am - Automake script for gps location-service
#

AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(LOCATIONAPI_CFLAGS) \
     $(IZATCORE_CFLAGS) \
     $(LOCATIONSERVICE_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     -I$(srcdir)/inc\
     -D__func__=__PRETTY_FUNCTION__ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

libizatprovider_la_SOURCES = \
        src/AltitudeReceiverProxy.cpp \
        src/IzatProvider.cpp \
        src/ZlocationResponseHandler.cpp

if USE_GLIB
libizatprovider_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libizatprovider_la_LDFLAGS = -lstdc++ -ldl -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libizatprovider_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libizatprovider_la_CFLAGS = $(AM_CFLAGS)
libizatprovider_la_LDFLAGS = -ldl -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libizatprovider_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libizatprovider_la_LIBADD = -lstdc++ $(GPSUTILS_LIBS) $(LOCATIONAPI_LIBS) $(IZATCORE_LIBS) \
                            $(LOCATIONSERVICE_LIBS) $(LOCCORE_LIBS) $(LOCBASEUTIL_LIBS)

library_include_HEADERS = \
    ./inc/AltitudeReceiverProxy.h  \
    ./inc/IzatProvider.h
library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = libizatprovider.la
