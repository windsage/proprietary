header_path=${WORKSPACE}/coretech-config-vendor/diag/include
src_path=../src

AM_CFLAGS = -Wall \
		-Wundef \
		-Wstrict-prototypes \
		-Wno-trigraphs \
		-DUSE_LOGCAT_LOGGING \
		$(DIAG_CFLAGS)

AM_CPPFLAGS = -DIMAGE_APPS_PROC \
		-DFEATURE_Q_SINGLE_LINK \
		-DFEATURE_Q_NO_SELF_QPTR \
		-DFEATURE_LINUX \
		-DFEATURE_NATIVELINUX \
		-DFEATURE_DSM_DUP_ITEMS \
		-DFEATURE_LE_DIAG \
		-I$(header_path) \
		-I$(src_path) \
		$(DIAG_CFLAGS)

bin_PROGRAMS = test_diag

test_diag_SOURCES = test_diag.c
test_diag_LDADD = $(srcdir)/../src/.libs/libdiag.so
test_diag_LDFLAGS = -lpthread -llog $(DIAG_LIBS)

bin_PROGRAMS += debug-diag
debug_diag_SOURCES = debug.c
debug_diag_LDADD = $(srcdir)/../src/.libs/libdiag.so
debug_diag_LDFLAGS = -lpthread -llog $(DIAG_LIBS)
