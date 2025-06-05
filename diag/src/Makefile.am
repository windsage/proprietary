# sources and intermediate files are separated
header_path=@HEADER_PATH@
src_path=../src
AM_CFLAGS = -Wall \
				-Wundef \
				-Wstrict-prototypes \
				-Wno-trigraphs \
				-DUSE_LOGCAT_LOGGING

AM_CPPFLAGS = -DIMAGE_APPS_PROC \
				  -DFEATURE_Q_SINGLE_LINK \
				  -DFEATURE_Q_NO_SELF_QPTR \
				  -DFEATURE_LINUX \
				  -DFEATURE_NATIVELINUX \
				  -DFEATURE_DSM_DUP_ITEMS \
				  -DFEATURE_LE_DIAG \
				  -Iinclude \
				  -I$(header_path) \
				  -I$(src_path)

h_sources  =$(header_path)/diag.h
h_sources +=$(header_path)/diag_lsm.h
h_sources +=$(header_path)/diag_lsm_dci.h
h_sources +=$(header_path)/diagpkt.h
h_sources +=$(header_path)/event_defs.h
h_sources +=$(header_path)/event.h
h_sources +=$(header_path)/log_codes.h
h_sources +=$(header_path)/log.h
h_sources +=$(src_path)/msg_arrays_i.h
h_sources +=$(header_path)/msgcfg.h
h_sources +=$(header_path)/msg.h
h_sources +=$(header_path)/msg_pkt_defs.h
h_sources +=$(header_path)/msg_qsr.h
h_sources +=$(header_path)/msgtgt.h
h_sources +=$(header_path)/diagcmd.h
h_sources +=$(src_path)/diagi.h
h_sources +=$(src_path)/diaglogi.h
h_sources +=$(header_path)/diagdiag.h
h_sources +=$(src_path)/diag_lsmi.h
h_sources +=$(src_path)/diag_shared_i.h
h_sources +=$(header_path)/ts_linux.h
h_sources +=$(header_path)/diag_logger.h
h_sources +=$(src_path)/diag_lsm_dci_i.h

c_sources  =diag_lsm.c \
				ts_linux.c \
				diag_lsm_dci.c \
				diag_lsm_event.c \
				diag_lsm_log.c \
				diag_lsm_msg.c \
				diag_lsm_pkt.c \
				diagsvc_malloc.c \
				msg_arrays_i.c \
				diag_logger.c \
				diag_lsm_qdss.c \
				diag_lsm_adpl.c \
				diag_lsm_diag_id.c \
				diag_lsm_comm.c \
				diag_qshrink4_db_parser.c \
				diag_lsm_key_info.c

requiredlibs = $(TGENOFF_LIBS)

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libdiag.la
libdiag_la_CC = @CC@
libdiag_la_SOURCES = $(c_sources)
libdiag_la_CPPFLAGS =$(AM_CPPFLAGS)
libdiag_la_LIBADD = $(requiredlibs)
if USE_GLIB
libdiag_la_CFLAGS = $(AM_CFLAGS) -fPIC -DUSE_GLIB @GLIB_CFLAGS@
libdiag_la_LDFLAGS = -lpthread @GLIB_LIBS@ -shared -version-info $(DIAG_LIBRARY_VERSION) -llog
else
libdiag_la_CFLAGS = $(AM_CFLAGS) -fPIC
libdiag_la_LDFLAGS = -lcutils -shared -version-info $(DIAG_LIBRARY_VERSION) -llog
endif
