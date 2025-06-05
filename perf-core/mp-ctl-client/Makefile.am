library_includedir = $(includedir))/mp-ctl-client

AM_CPPFLAGS = -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat
AM_CPPFLAGS += -g \
               -Wall \
               -Wno-unused-variable \
               -Wno-unused-parameter \
               -Wno-unused-function \
               -DQC_DEBUG=0 \
               @LIBUTILS_CFLAGS@ \
               @LIBCUTILS_CFLAGS@ \
               -I$(top_srcdir)/mp-ctl \
               -I$(top_srcdir)/mp-ctl-client \
               -I$(top_srcdir)/perf-util \
               -std=c++17 \
               -DUSE_GLIB \
               -fPIC \
               -fpic \
               @GLIB_CFLAGS@ \
               -fexceptions


AM_CPPFLAGS += -L$(libdir)

requiredlibs = -llog    \
               -lcutils \
               -lutils \
               -lrt \
               @GLIB_LIBS@

lib_LTLIBRARIES  = libqti-perfd-client.la
libqti_perfd_client_la_CC = @CC@
libqti_perfd_client_la_SOURCES = client_sock.cpp PerfOffloadHelper.cpp PerfThreadPool.cpp

libqti_perfd_client_la_CFLAGS = $(AM_CPPFLAGS)
libqti_perfd_client_la_CPPFLAGS = $(AM_CPPFLAGS)
libqti_perfd_client_la_LIBADD = $(requiredlibs)

libqti_perfd_client_la_LIBADD += ../perf-util/libqti-util.la
libqti_perfd_client_la_LIBADD += ../perf-util/libq-perflog.la

libqti_perfd_client_la_LDFLAGS = $(requiredlibs) -avoid-version  -shared -lpthread

perf_incdir = "$(includedir)/perf"
perf_inc_HEADERS = client.h
