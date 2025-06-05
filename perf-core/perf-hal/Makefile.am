AM_CPPFLAGS = -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat
AM_CPPFLAGS += -g \
              -Wall \
              -Wno-unused-variable \
              -Wno-unused-parameter \
              -Wno-unused-function \
              -I$(top_srcdir)/perf-hal \
              -I$(top_srcdir)/mp-ctl-client \
              -I$(top_srcdir)/perf-util \
              -I$(top_srcdir)/mp-ctl \
              @LIBCUTILS_CFLAGS@ \
              @XML2_CFLAGS@ \
              -DVENDOR_DIR=\"/etc\" \
              -DQC_DEBUG=0 \
              -std=c++17 \
              -fexceptions \
              -DUSE_GLIB \
              @GLIB_CFLAGS@

requiredlibs = -llog    \
               -lcutils \
               -lutils \
               -lxml2 \
               -lrt \
               @GLIB_LIBS@ \
               @XML2_LIBS@

AM_CPPFLAGS += -L$(libdir)

lib_LTLIBRARIES = libperfconfig.la
libperfconfig_CC  = @CC@
libperfconfig_la_SOURCES = TargetConfig.cpp PerfConfig.cpp DivergentConfig.cpp
libperfconfig_la_CFLAGS = $(AM_CPPFLAGS)
libperfconfig_la_CPPFLAGS = $(AM_CPPFLAGS)
libperfconfig_la_LIBADD = $(requiredlibs)
libperfconfig_la_LIBADD += ../perf-util/libqti-util.la
libperfconfig_la_LIBADD += ../perf-util/libq-perflog.la
libperfconfig_la_LDFLAGS = $(requiredlibs) -shared -avoid-version -lpthread
#################################################################################
lib_LTLIBRARIES += libperfgluelayer.la
libperfgluelayer_CC  = @CC@
libperfgluelayer_la_SOURCES = PerfGlueLayer.cpp VendorIGlue.cpp
libperfgluelayer_la_LIBADD = $(requiredlibs)
libperfgluelayer_la_LIBADD += ../mp-ctl-client/libqti-perfd-client.la
libperfgluelayer_la_LIBADD += libperfconfig.la
libperfgluelayer_la_CFLAGS = $(AM_CPPFLAGS)
libperfgluelayer_la_CPPFLAGS = $(AM_CPPFLAGS)
libperfgluelayer_la_LDFLAGS = $(requiredlibs) -avoid-version -lpthread -shared
#################################################################################
bin_PROGRAMS = perf-hal-service
perf_hal_service_CC  = @CC@
perf_hal_service_SOURCES = service_sock.cpp Perf_sock.cpp
perf_hal_service_LDADD = $(requiredlibs)
perf_hal_service_LDADD += libperfgluelayer.la
perf_hal_service_LDADD += libperfconfig.la
perf_hal_service_CFLAGS = $(AM_CPPFLAGS)
perf_hal_service_CPPFLAGS = $(AM_CPPFLAGS)
perf_hal_service_LDFLAGS = $(requiredlibs) -g -avoid-version -lpthread

if USE_SYSTEMD
root_libdir = "/lib"
perf_hal_systemddir       = $(root_libdir)/systemd/system
perf_hal_systemd_DATA     = perf-hal.service
endif
