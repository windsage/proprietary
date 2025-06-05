AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I${top_srcdir}/qrtr/lib \
        $(QMI_CFLAGS)

h_sources =

requiredlibs = -lc \
        -lrt \
        -lpthread \
        ../qrtr/libqrtr.la

bin_PROGRAMS = qrtr_test_client
qrtr_test_client_SOURCES = qrtr_test_client.c
qrtr_test_client_LDADD = $(requiredlibs)
qrtr_test_client_CC = @CC@

bin_PROGRAMS += qrtr_test_server
qrtr_test_server_SOURCES = qrtr_test_server.c
qrtr_test_server_LDADD = $(requiredlibs)
qrtr_test_server_CC = @CC@
