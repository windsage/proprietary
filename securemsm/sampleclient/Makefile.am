AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(SECUREMSM-NOSHIP_CFLAGS) \
        -I$(WORKSPACE)/security/securemsm/smcinvoke/inc \
        -I$(WORKSPACE)/security/securemsm/mink/inc/interface/ \
        -I$(WORKSPACE)/security/securemsm/QSEEComAPI/

if TARGET_ENABLE_QSEECOM
AM_CFLAGS += -DTARGET_ENABLE_QSEECOM
endif

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG \
    -DOE \
    $(SECUREMSMNOSHIP_CFLAGS)

h_sources = SecureUILib.h
qseecom_sample_client_SOURCES = qseecom_sample_client.c $(h_sources)
bin_PROGRAMS = qseecom_sample_client
qseecom_sample_client_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
qseecom_sample_client_CPPFLAGS = $(AM_CPPFLAGS)
qseecom_sample_client_CFLAGS = $(AM_CFLAGS)
qseecom_sample_client_CFLAGS += -I$(WORKSPACE)/system/core/libion/include
qseecom_sample_client_LDFLAGS = -lpthread -ldl -lion -lQseeComApi -ldrmfs -lrpmb -lssd
qseecom_sample_client_LDADD = -lQseeComApi -lion -ldmabufheap -lqcbor

if TARGET_ENABLE_QSEECOMCOMPAT
qseecom_sample_client_LDADD += -lminkdescriptor
endif
