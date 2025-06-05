AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
	-fpie -pie \
	$(QMIFW_CFLAGS) \
        -I.

AM_CPPFLAGS = -D__packed__= \
    -DHAS_LIBQRTR=1 \
    -DUSE_GLIB=1 \
    -DFEATURE_LE_DIAG

c_sources  = app_cmds.c \
	circ_buf.c \
	common_cmds.c \
	diag.c \
	diag_cntl.c \
	dm.c \
	hdlc.c \
	masks.c \
	mbuf.c \
	peripheral.c \
	router.c \
	socket.c \
	uart.c \
	unix.c \
	usb.c \
	util.c \
	watch.c \
	peripheral-qrtr.c \
	diag_mhi.c \
	diag_mux.c \
	diag_dci.c \
	pcie.c \
	peripheral-glinkpkt.c \
	vm.c \
	sys_time.c

requiredlibs = $(QMIFW_LIBS)

library_includedir = $(pkgincludedir)
bin_PROGRAMS = diag-router
if USE_GLIB
AM_CFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
endif
diag_router_SOURCES = $(c_sources)
diag_router_LDADD = $(requiredlibs)
diag_router_CC = @CC@
diag_router_LDFLAGS = -llog -lpthread @GLIB_LIBS@
