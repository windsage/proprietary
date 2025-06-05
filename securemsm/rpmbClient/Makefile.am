AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -I$(WORKSPACE)/security/securemsm/smcinvoke/inc \
        -I$(WORKSPACE)/security/securemsm/mink/inc/interface/

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG \
    -DOE \
    $(SECUREMSMNOSHIP_CFLAGS)

h_sources = SecureUILib.h
rpmbClient_SOURCES = rpmbClient.c $(h_sources)
bin_PROGRAMS = rpmbClient
rpmbClient_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
rpmbClient_CPPFLAGS = $(AM_CPPFLAGS)
rpmbClient_CFLAGS = $(AM_CFLAGS)

rpmbClient_CFLAGS += -I$(WORKSPACE)/system/core/libion/include
rpmbClient_LDFLAGS = -ldl -lc -lcutils -lutils -llog
rpmbClient_LDADD = -lion -lqcbor -ldmabufheap -lminkdescriptor -lqcbor

if TARGET_ENABLE_QSEECOM
rpmbClient_CFLAGS += -DTARGET_ENABLE_QSEECOM
AM_CFLAGS += -I$(WORKSPACE)/security/securemsm/QSEEComAPI/
rpmbClient_LDADD += -lQseeComApi
endif

all-locat: $(bin_PROGRAMS)
