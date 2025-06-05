#AM_CPPFLAGS = -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat
#AM_CPPFLAGS += -g \
#              -Wall \
#              -Wno-unused-variable \
#              -Wno-unused-parameter \
#              -Wno-unused-function \
#              -DQC_DEBUG=0 \
#              -I$(top_srcdir)/perf-hal \
#              -I$(top_srcdir)/perf-util \
#              -I$(top_srcdir)/tests \
#              -I$(top_srcdir)/mp-ctl \
#              -I$(top_srcdir)/mp-ctl-client \
#              @LIBCUTILS_CFLAGS@ \
#              @XML2_CFLAGS@ \
#              -std=c++17 \
#              -fexceptions \
#              -fpermissive \
#              -DUSE_GLIB \
#              @GLIB_CFLAGS@
#
#AM_CPPFLAGS += -L$(libdir)
#requiredlibs = -llog    \
#               -lcutils \
#               -lutils \
#               -lxml2 \
#               -lrt \
#               -ldl \
#               @GLIB_LIBS@ \
#               @XML2_LIBS@
#
######################################################################
#bin_PROGRAMS = perfunittests
#perfunittests_CC  = @CC@
#perfunittests_SOURCES = ttf.cpp \
#                        mp-ctl-client/PerfThreadPoolTest.cpp \
#                        perf-util/EventDataPoolTest.cpp \
#                        perf-util/EventQueueTest.cpp \
#                        perf-hal/PerfGlueLayerTest.cpp \
#                        activity-trigger/ActivityTriggerTest.cpp \
#                        mp-ctl/ActiveRequestListTest.cpp
#
#perfunittests_CFLAGS = $(AM_CPPFLAGS)
#perfunittests_CPPFLAGS = $(AM_CPPFLAGS)
#perfunittests_LDADD = $(requiredlibs)
#perfunittests_LDADD += ../../perf-util/libqti-util.la
#perfunittests_LDADD += ../../perf-hal/libperfgluelayer.la
#perfunittests_LDADD += ../../mp-ctl/libqti-perfd.la
#perfunittests_LDADD += ../../mp-ctl-client/libqti-perfd-client.la
#perfunittests_LDFLAGS = $(requiredlibs) -g -avoid-version -shared -lpthread
