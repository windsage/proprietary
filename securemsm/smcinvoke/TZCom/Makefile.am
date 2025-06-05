
AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs

ACLOCAL_AMFLAGS = -I m4
SECUREMSM_PATH = $(WORKSPACE)/security/securemsm

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG  \
    -DOE \
    -I$(SECUREMSM_PATH)/smcinvoke/minksocket/kernel \
    -I$(SECUREMSM_PATH)/smcinvoke/minksocket/include

h_sources = IClientEnv.h \
            InvokeUtils.h \
            MinkDescriptor.h \
            object.h \
            TZCom.h \
            TimedRetryForwarder.h

requiredlibs =
library_includedir = $(pkgincludedir)
library_include_HEADERS = inc/memscpy.h

cpp_sources = src/TZCom.cpp \
              src/MinkDescriptor.cpp \
              src/CIO.cpp \
              src/fdwrapper.c \
              src/TimedRetryForwarder.cpp

lib_LTLIBRARIES := libminkdescriptor.la
llibminkdescriptor_la_CC = @CC@
libminkdescriptor_la_SOURCES = $(cpp_sources) $(h_sources)
libminkdescriptor_la_CFLAGS = $(AM_CFLAGS) -fPIC
libminkdescriptor_la_CPPFLAGS = \
		$(AM_CPPFLAGS) \
		$(DATA_INCLUDES) \
		-Iinc \
		-I../inc/

if TARGET_ENABLE_QTVM_LOGGING
libminkdescriptor_la_CPPFLAGS += -DENABLE_QTVM_PRINT_LOGGING
endif

libminkdescriptor_la_LDFLAGS = -shared -version-info 1:0:0
libminkdescriptor_la_LDFLAGS += -llog -lcutils -lpthread
libminkdescriptor_la_LIBADD = $(requiredlibs)
