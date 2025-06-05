header_path=${WORKSPACE}/coretech-config-vendor/diag/include
src_path=../src

AM_CFLAGS = -Wall \
		-Wundef \
		-Wstrict-prototypes \
		-Wno-trigraphs \
		-Werror	\
		-DUSE_LOGCAT_LOGGING

AM_CPPFLAGS = -D__packed__= \
		-DIMAGE_APPS_PROC \
		-DFEATURE_Q_SINGLE_LINK \
		-DFEATURE_Q_NO_SELF_QPTR \
		-DFEATURE_LINUX \
		-DFEATURE_NATIVELINUX \
		-DFEATURE_DSM_DUP_ITEMS \
		-DFEATURE_LE_DIAG \
		-I$(header_path) \
		-I$(src_path) \
		$(DIAG_CFLAGS)

bin_PROGRAMS = diag_klog

diag_klog_SOURCES = diag_klog.c
diag_klog_LDADD = $(srcdir)/../src/.libs/libdiag.so
diag_klog_LDFLAGS = -lpthread -llog $(DIAG_LIBS)
