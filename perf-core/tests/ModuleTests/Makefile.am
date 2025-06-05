AM_CPPFLAGS = -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat
AM_CPPFLAGS += -g \
              -Wall \
              -Wno-unused-variable \
              -Wno-unused-parameter \
              -Wno-unused-function \
              -DQC_DEBUG=0 \
              -DVENDOR_DIR=\"/etc\" \
              -I$(top_srcdir) \
              -I$(top_srcdir)/perf-hal \
              -I$(top_srcdir)/perf-util \
              -I$(top_srcdir)/mp-ctl \
              -I$(top_srcdir)/tests \
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
bin_PROGRAMS = perflock_native_test
perflock_native_test_CC  = @CC@
perflock_native_test_SOURCES = mp-ctl/perflock_native_test.cpp
perflock_native_test_LDADD = $(requiredlibs)
perflock_native_test_CFLAGS = $(AM_CPPFLAGS)
perflock_native_test_CPPFLAGS = $(AM_CPPFLAGS)
perflock_native_test_LDFLAGS = $(requiredlibs) -avoid-version -shared -lpthread
#####################################################################
bin_PROGRAMS += perflock_native_stress_test
perflock_native_stress_test_CC  = @CC@
perflock_native_stress_test_SOURCES = mp-ctl/perflock_native_stress_test.cpp
perflock_native_stress_test_LDADD = $(requiredlibs)
perflock_native_stress_test_CFLAGS = $(AM_CPPFLAGS)
perflock_native_stress_test_CPPFLAGS = $(AM_CPPFLAGS)
perflock_native_stress_test_LDFLAGS = $(requiredlibs) -avoid-version -shared -lpthread
#####################################################################
bin_PROGRAMS += perflock_native_test_server
perflock_native_test_server_CC  = @CC@
perflock_native_test_server_SOURCES = mp-ctl/pseudoclient/perflock_native_test_server.cpp
perflock_native_test_server_LDADD = $(requiredlibs)
perflock_native_test_server_CFLAGS = $(AM_CPPFLAGS)
perflock_native_test_server_CPPFLAGS = $(AM_CPPFLAGS)
perflock_native_test_server_LDFLAGS = $(requiredlibs) -avoid-version -shared -lpthread
#####################################################################
bin_PROGRAMS += perflock_native_test_client
perflock_native_test_CC  = @CC@
perflock_native_test_client_SOURCES = mp-ctl/pseudoclient/perflock_native_test_client.cpp
perflock_native_test_client_LDADD = $(requiredlibs)
perflock_native_test_client_CFLAGS = $(AM_CPPFLAGS)
perflock_native_test_client_CPPFLAGS = $(AM_CPPFLAGS)
perflock_native_test_client_LDFLAGS = $(requiredlibs) -avoid-version -shared -lpthread
#####################################################################
lib_LTLIBRARIES  = libqti-perfd-tests.la
libqti_perfd_tests_la_CC = @CC@
libqti_perfd_tests_la_SOURCES = mp-ctl/PerfSysNodeTest.cpp
libqti_perfd_tests_la_CFLAGS = $(AM_CPPFLAGS)
libqti_perfd_tests_la_CPPFLAGS = $(AM_CPPFLAGS)
libqti_perfd_tests_la_LIBADD = $(requiredlibs)

libqti_perfd_tests_la_LIBADD += ../../mp-ctl/libqti-perfd.la
libqti_perfd_tests_la_LIBADD += ../../perf-util/libqti-util.la

libqti_perfd_tests_la_LDFLAGS = $(requiredlibs) -shared -avoid-version -lpthread
