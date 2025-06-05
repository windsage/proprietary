AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(QMI_CFLAGS)

AM_CPPFLAGS = \
        -I../inc

h_sources =

requiredlibs = -lc \
        -lrt

bin_PROGRAMS = vsock_server
vsock_server_SOURCES = vsock_server.c
vsock_server_LDADD = $(requiredlibs)
vsock_server_CC = @CC@

bin_PROGRAMS += vsock_client
vsock_client_SOURCES = vsock_client.c
vsock_client_LDADD = $(requiredlibs)
vsock_client_CC = @CC@
