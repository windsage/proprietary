AM_CFLAGS = -Wall \
            $(DIAG_CFLAGS) \
	    $(LIBNL_CFLAGS)

if DEBUG
AM_CFLAGS += -g
else
AM_CFLAGS += -O2
endif

if IPQ_COMMON_TOOL
AM_CFLAGS += -DCNSS_DIAG_PLATFORM_WIN
AM_CFLAGS += -DCNSS_DIAG_PLATFORM_WIN_LE
else
AM_CFLAGS += -DCONFIG_CLD80211_LIB
endif

if IPQ_COMMON_TOOL
requiredlibs = -lnl-3 -lnl-genl-3 -lrt
else
requiredlibs = -lnl-3 -lnl-genl-3 -lrt -lcld80211
endif

if USE_GLIB
AM_CFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
requiredlibs += $(GLIB_LIBS)
endif

if CLD80211
AM_CFLAGS += -DCONFIG_CLD80211_LIB
requiredlibs += -lcld80211
AM_CFLAGS += -I${CLD80211_PATH}
endif

cnss_diag_SOURCES = cld-fwlog-netlink.c parser.c cld-diag-parser.c nan-parser.c
cnss_diag_LDADD = $(DIAG_LIBS) $(requiredlibs)
cnss_diag_LDADD += ${LIBNL_LIBS}
cnss_diag_CFLAGS = ${LIBNL_CFLAGS}
cnss_diag_CFLAGS += ${AM_CFLAGS}
bin_PROGRAMS = cnss_diag

