library_includedir = $(includedir)/mp-ctl $(top_srcdir)/perf-util $(top_srcdir)/mp-ctl-client

AM_CPPFLAGS = -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat
if OPENWRT_PERF
     WORKSPACE = $(TOPDIR)/src
     AM_CPPFLAGS += -Dsigval_t=sigval
endif
AM_CPPFLAGS += -DSERVER \
               -DCLIENT \
               -DPERFD \
               -g \
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
               -I${WORKSPACE}/OTA/device/qcom/common/power/ \
               -I${WORKSPACE}/OTA/device/qcom/common/thermal-engine \
               @LIBUTILS_CFLAGS@ \
               @LIBCUTILS_CFLAGS@ \
               -std=c++17 \
               -I${WORKSPACE}/OTA/system/core/include/ \
               @XML2_CFLAGS@ \
               -fexceptions \
               -DUSE_GLIB \
               -fPIC \
               -fpic \
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

lib_LTLIBRARIES  = libqti-perfd.la
libqti_perfd_la_CC = @CC@
libqti_perfd_la_SOURCES = PerfController.cpp \
                          ActiveRequestList.cpp \
                          ResourceQueues.cpp \
                          OptsHandlerExtnStub.cpp \
                          OptsHandler.cpp \
                          RestoreHandler.cpp \
                          OptsData.cpp \
                          ResourceInfo.cpp \
                          Target.cpp \
                          TargetInit.cpp \
                          MpctlUtils.cpp \
                          Request.cpp \
                          BoostConfigReader.cpp \
                          HintExtHandler.cpp \
                          VendorIBoostConfig.cpp \
                          VendorIPerf.cpp

libqti_perfd_la_CFLAGS = $(AM_CPPFLAGS)
libqti_perfd_la_CPPFLAGS = $(AM_CPPFLAGS)
libqti_perfd_la_LIBADD = $(requiredlibs)

libqti_perfd_la_LIBADD += ../perf-util/libqti-util.la
libqti_perfd_la_LIBADD += ../perf-util/libq-perflog.la
libqti_perfd_la_LIBADD += ../perf-hal/libperfconfig.la
libqti_perfd_la_LIBADD += ../mp-ctl-client/libqti-perfd-client.la
libqti_perfd_la_LIBADD += ../ioctl/libperfioctl.la

libqti_perfd_la_LDFLAGS = $(requiredlibs) -shared -avoid-version -lpthread
