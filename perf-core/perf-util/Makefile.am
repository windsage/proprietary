AM_CPPFLAGS = -g \
              -Wall \
              -Wno-unused-variable \
              -Wno-unused-parameter \
              -Wno-unused-function \
              -I$(top_srcdir)/perf-util \
              -DQC_DEBUG=0 \
              -fexceptions \
              -std=c++17 \
              @LIBUTILS_CFLAGS@ \
              @LIBCUTILS_CFLAGS@ \
              $(GLIB_CFLAGS) \
              @XML2_CFLAGS@

AM_CPPFLAGS += -L$(libdir)

requiredlibs = -llog \
               -lutils \
               -lcutils \
               -lxml2 \
               $(GLIB_LIBS) \
               ${LIBXML_LIB}

lib_LTLIBRARIES  = libqti-util.la
libqti_util_la_CC = @CC@
libqti_util_la_SOURCES = XmlParser.cpp \
                         EventQueue.cpp \
                         EventDataPool.cpp \
                         SecureOperations.cpp

libqti_util_la_CFLAGS = $(AM_CPPFLAGS)
libqti_util_la_CPPFLAGS = $(AM_CPPFLAGS)
libqti_util_la_LIBADD = $(requiredlibs)
libqti_util_la_LDFLAGS = $(requiredlibs) -avoid-version -lpthread -shared

lib_LTLIBRARIES  += libq-perflog.la
libq-perflog_la_CC = @CC@
libq-perflog_la_SOURCES = PerfLog.cpp

libq-perflog_la_CFLAGS = $(AM_CPPFLAGS)
libq-perflog_la_CPPFLAGS = $(AM_CPPFLAGS)
libq-perflog_la_LIBADD = $(requiredlibs)
libq-perflog_la_LDFLAGS = $(requiredlibs) -avoid-version -shared
