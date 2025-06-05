AM_CFLAGS = \
     -I./ \
     -D__STDC_FORMAT_MACROS \
     $(GPSUTILS_CFLAGS) \
     $(LOCPLA_CFLAGS) \
     -Dstrlcpy=g_strlcpy \
     -Dstrlcat=g_strlcat \
     -std=c++14

ACLOCAL_AMFLAGS = -I m4

if USE_FEATURE_AUTOMOTIVE
  AM_CFLAGS += -DFEATURE_AUTOMOTIVE \
    -DPROTOBUF_ENABLED
endif

if CDFW_ENABLED
  AM_CFLAGS += \
    -DFEATURE_CDFW \
     $(CDFW_CFLAGS) \
     $(CDFW_API_CFLAGS)
endif

libeng_hub_msg_proto_la_SOURCES = \
    LocationDataTypes.pb.cc \
    EHMsg.pb.cc

if USE_FEATURE_AUTOMOTIVE
libeng_hub_msg_proto_la_SOURCES += \
    EHMsgUtils.cpp \
    EngineHubMsgConverter.cpp \
    EnginePluginProtoMsgConverter.cpp
endif

if USE_GLIB
libeng_hub_msg_proto_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@ -include glib.h
libeng_hub_msg_proto_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libeng_hub_msg_proto_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@ -include glib.h
else
libeng_hub_msg_proto_la_CFLAGS = $(AM_CFLAGS)
libeng_hub_msg_proto_la_LDFLAGS = -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libeng_hub_msg_proto_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

required_libs = \
    $(LOCPLA_LIBS) \
    $(GPSUTILS_LIBS)

if CDFW_ENABLED
  required_libs += \
    $(CDFW_LIBS)
endif

libeng_hub_msg_proto_la_LIBADD = -ldl -lstdc++ -lprotobuf-lite $(required_libs)

library_include_HEADERS = \
    LocationDataTypes.pb.h \
    EHMsg.pb.h

if USE_FEATURE_AUTOMOTIVE
library_include_HEADERS += \
    EHMsgUtils.h \
    EngineHubMsgConverter.h \
    EnginePluginProtoMsgConverter.h
endif

library_includedir = $(pkgincludedir)
#Create and Install libraries
lib_LTLIBRARIES = libeng_hub_msg_proto.la
