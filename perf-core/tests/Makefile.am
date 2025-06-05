AM_CPPFLAGS = -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat
if OPENWRT_PERF
    AM_CPPFLAGS += -Dsigval_t=sigval
endif
AM_CPPFLAGS += -g \
              -Wall \
              -Wno-unused-variable \
              -Wno-unused-parameter \
              -Wno-unused-function \
              -DVENDOR_DIR=\"/etc\" \
              -DQC_DEBUG=0 \
              -I$(top_srcdir)/perf-hal/ \
              -I$(top_srcdir)/perf-util \
              -I$(top_srcdir)/mp-ctl \
              -I$(top_srcdir)/mp-ctl-client \
              -I$(top_srcdir)tests \
              @LIBCUTILS_CFLAGS@ \
              @XML2_CFLAGS@ \
              -std=c++17 \
              -fexceptions \
              -fpermissive \
              -DUSE_GLIB \
              @GLIB_CFLAGS@

AM_CPPFLAGS += -L$(libdir)
requiredlibs = -llog    \
               -lcutils \
               -lutils \
               -lxml2 \
               -lrt \
               -ldl \
               @GLIB_LIBS@ \
               @XML2_LIBS@

#####################################################################
bin_PROGRAMS = regressionframework
regressionframework_CC  = @CC@
regressionframework_SOURCES =  RegressionFramework.cpp
regressionframework_SOURCES += UnitTests/perf-util/EventDataPoolTest.cpp
regressionframework_SOURCES += UnitTests/perf-util/EventQueueTest.cpp
regressionframework_SOURCES += UnitTests/perf-hal/PerfGlueLayerTest.cpp
regressionframework_SOURCES += UnitTests/activity-trigger/ActivityTriggerTest.cpp
regressionframework_SOURCES += UnitTests/mp-ctl/ActiveRequestListTest.cpp
regressionframework_SOURCES += UnitTests/mp-ctl-client/PerfThreadPoolTest.cpp
regressionframework_SOURCES += ModuleTests/mp-ctl/PerfTest.cpp
regressionframework_SOURCES += ModuleTests/learning-module/QGPETests.cpp
regressionframework_SOURCES += ModuleTests/mp-ctl/PerfStressTest.cpp
regressionframework_SOURCES += ModuleTests/mp-ctl/DisplayTests.cpp
regressionframework_SOURCES += ModuleTests/mp-ctl/MemoryDetective.cpp

regressionframework_LDADD = $(requiredlibs)
regressionframework_LDADD += ../perf-util/libqti-util.la
regressionframework_LDADD += ../perf-hal/libperfconfig.la
regressionframework_LDADD += ../perf-hal/libperfgluelayer.la
regressionframework_LDADD += ../mp-ctl/libqti-perfd.la
regressionframework_LDADD += ../mp-ctl-client/libqti-perfd-client.la
regressionframework_CFLAGS = $(AM_CPPFLAGS)
regressionframework_CPPFLAGS = $(AM_CPPFLAGS)
regressionframework_LDFLAGS = $(requiredlibs) -avoid-version -shared -lpthread
