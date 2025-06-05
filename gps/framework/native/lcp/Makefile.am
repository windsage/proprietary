# Makefile.am - Automake script for gps location-service
#

AM_CFLAGS = \
     $(GPSUTILS_CFLAGS) \
     $(LOCCORE_CFLAGS) \
     $(LOCATIONAPI_CFLAGS) \
     $(LOWICOMMON_CFLAGS) \
     $(LOWICLIENT_CFLAGS) \
     $(LOCBASEUTIL_CFLAGS) \
     $(IZATCORE_CFLAGS) \
     $(DATAITEMS_CFLAGS) \
     $(QMIFW_CFLAGS) \
     $(QMIF_CFLAGS) \
     $(LOCAPIV02_CFLAGS) \
     $(WIFIOBSERVER_CFLAGS) \
     -I../core \
     -I../service \
     -I../adapter \
     -I./izat_api  \
     -I./observers/IPCHandler/inc \
     -I./observers/IPCHandler/inc/internal \
     -I./observers/IPCClient/inc \
     -I./observers/IPCClient/inc/internal \
     -I./observers/FreeWifiScanObserver/inc \
     -I./observers/FreeWifiScanObserver/inc/internal \
     -I./izat-provider-service/inc \
     -I./izat-provider-service/inc/internal \
     -I./izat-listeners/inc \
     -I./izat-listeners/inc/internal \
     -I./ipc-messaging-proxy/inc \
     -I./ipc-messaging-proxy/inc/internal \
     -I./utils/inc \
     -I./utils/inc/internal \
     -I./wiper/inc \
     -I./wiper/inc/internal \
     -D__func__=__PRETTY_FUNCTION__ \
     -D_HAS_LOC_V02_ \
     -DON_TARGET_TEST \
     -std=c++11

ACLOCAL_AMFLAGS = -I m4

liblocationservice_la_SOURCES = \
        ../adapter/LBSAdapter.cpp \
        observers/IPCClient/src/IPCClient.cpp \
        observers/IPCHandler/src/IPCHandler.cpp \
        izat-provider-service/src/LocationProvider.cpp \
        izat-provider-service/src/IzatManager.cpp \
        izat-provider-service/src/WiFiLocationProvider.cpp \
        izat-provider-service/src/WWanLocationProvider.cpp \
        izat-listeners/src/IzatPassiveLocationListener.cpp \
        wiper/src/Wiper.cpp \
        utils/src/Utils.cpp

if USE_GLIB
liblocationservice_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
liblocationservice_la_LDFLAGS = -lstdc++ -ldl -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
liblocationservice_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
liblocationservice_la_CFLAGS = $(AM_CFLAGS)
liblocationservice_la_LDFLAGS = -ldl -Wl,-z,defs -lpthread -shared -version-info 1:0:0
liblocationservice_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

liblocationservice_la_LIBADD = -lstdc++ $(GPSUTILS_LIBS) $(LOCCORE_LIBS) $(LOCATIONAPI_LIBS) \
        $(LOWICLIENT_LIBS) $(LBSCORE_LIBS) $(LOCBASEUTIL_LIBS) $(LOCMQCLIENT_LIBS) \
        $(DATAITEMS_LIBS) $(WIFIOBSERVER_LIBS) $(IZATCORE_LIBS) $(CUTILS_LIBS)

library_include_HEADERS = \
    ../adapter/LBSAdapter.h \
    ../geofence/GeoFencerProxyMsgs.h \
    izat_api/IIzatManager.h \
    izat_api/IOSFramework.h \
    izat_api/IzatLocation.h \
    izat_api/IOSLocationProvider.h \
    izat_api/IIzatListener.h \
    izat_api/IOSListener.h \
    izat_api/IzatTypes.h \
    izat_api/IzatRequest.h \
    izat_api/IRawCommandInjector.h \
    izat_api/IOSFrameworkCleaner.h \
    utils/inc/IzatDefines.h \
    utils/inc/internal/Utils.h \
    izat_api/IzatNmea.h \
    izat_api/IzatSvInfo.h \
    izat-provider-service/inc/internal/LocationProvider.h \
    izat-provider-service/inc/internal/IzatManager.h \
    izat-provider-service/inc/internal/IzatContext.h \
    izat-provider-service/inc/ILocationProvider.h \
    izat-provider-service/inc/LocationError.h  \
    izat-provider-service/inc/LocationReport.h \
    izat-provider-service/inc/ILocationResponse.h

library_includedir = $(pkgincludedir)

#Create and Install libraries
lib_LTLIBRARIES = liblocationservice.la

