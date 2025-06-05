AM_CPPFLAGS = -g \
              -Wall \
              -Wno-unused-variable \
              -Wno-unused-parameter \
              -Wno-unused-function \
              -I$(top_srcdir)/perf-util \
              -I$(top_srcdir)/mp-ctl \
              -DQC_DEBUG=0 \
              -fexceptions \
              -std=c++17 \
              @LIBUTILS_CFLAGS@ \
              @LIBCUTILS_CFLAGS@

AM_CPPFLAGS += -L$(libdir)

requiredlibs = -llog \
               -lutils \
               -lcutils

lib_LTLIBRARIES  = libperfioctl.la
libperfioctl_la_CC = @CC@
libperfioctl_la_SOURCES = ioctl_fallthrough.cpp

libperfioctl_la_CFLAGS = $(AM_CPPFLAGS)
libperfioctl_la_CPPFLAGS = $(AM_CPPFLAGS)
libperfioctl_la_LIBADD = $(requiredlibs)
libperfioctl_la_LIBADD += ../perf-util/libq-perflog.la
libperfioctl_la_LDFLAGS = $(requiredlibs) -avoid-version -lpthread -shared
