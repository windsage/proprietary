AM_CFLAGS = -Wall -Werror -Wno-unused-value

AM_CXXFLAGS = $(DATA_CFLAGS) \
            $(QMI_FRAMEWORK_CFLAGS) \
            $(DSUTILS_CFLAGS)
if DEBUG
AM_CFLAGS += -DCONFIG_DEBUG -g -O
else
AM_CFLAGS += -O2
endif

AM_CFLAGS += -DICNSS_QMI

AM_CFLAGS += -DCONFIG_DEBUG
AM_CFLAGS += -DCONFIG_NO_PM
AM_CFLAGS += -DCONFIG_WLAN_MSG_SVC

AM_CFLAGS += $(QMI_FRAMEWORK_CFLAGS)
AM_CFLAGS += $(LIBNL3_CFLAGS)
AM_CFLAGS += $(LINUX_QUIC_FLAGS)
AM_CFLAGS += $(SYSTEM_CORE_CFLAGS)

requiredlibs = -lpthread -lcutils -lqmi_cci -lqmi_common_so -lnl-3 -lnl-genl-3 -ldl

if USE_GLIB
AM_CFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
requiredlibs += $(GLIB_LIBS)
endif

if ENABLE_IP_SOLICIT_RETRY
AM_CFLAGS += -DENABLE_IP_SOLICIT_RETRY
endif

if DISABLE_CLD80211_LIB
AM_CFLAGS += -DDISABLE_CLD80211_LIB
else
requiredlibs += -lcld80211
endif

if CONFIG_DEBUG_SYSLOG
AM_CFLAGS += -DCONFIG_DEBUG_SYSLOG
endif

if CONFIG_DEBUG_FILE
AM_CFLAGS += -DCONFIG_DEBUG_FILE
endif

if CONFIG_CNSS_USER
AM_CFLAGS += -DCONFIG_CNSS_USER
endif

if CONFIG_RECORD_DAEMON_QMI_LOG
AM_CFLAGS += -DCONFIG_RECORD_DAEMON_QMI_LOG
endif

if CONFIG_ENABLE_CNSSCLI
AM_CFLAGS += -DCONFIG_ENABLE_CNSSCLI
endif

if IPQ
AM_CFLAGS += -DIPQ
AM_CFLAGS += -DIPQ_LE
else
AM_CFLAGS += -DCONFIG_CNSS_LPASS_SCAN
endif

AM_CXXFLAGS += $(AM_CFLAGS)

c_sources = main.c
c_sources += nl_loop.c
c_sources += debug.c
c_sources += wlan_firmware_service_v01.c
c_sources += wlfw_qmi_client.c
c_sources += cnss_plat.c
c_sources += cnss_genl.c

if !IPQ
c_sources += cnss_qmi_client.c
c_sources += wireless_lan_proxy_service_v01.c
endif

if CONFIG_ENABLE_CNSSCLI
c_cli_sources = cnss_cli.c
endif

if CONFIG_DEBUG_LOGCAT
AM_CFLAGS += -DCONFIG_DEBUG_LOGCAT
requiredlibs += -llog
endif

if ENABLE_INTEROP_ISSUES_AP_DETECTION
AM_CFLAGS += -DENABLE_INTEROP_ISSUES_AP_DETECTION
c_sources += cnss_interop_issues_ap.c
endif

if ENABLE_NETLINK_COMMON
AM_CFLAGS += -DENABLE_NETLINK_COMMON
c_sources += cnss_netlink_common.c
endif

if CONFIG_CNSS_DP
AM_CFLAGS += -DCONFIG_CNSS_DP
c_sources += cnss_dp.c
if ENABLE_SUBNET_DETECTION
AM_CFLAGS += -DENABLE_SUBNET_DETECTION
AM_CFLAGS += -DCONFIG_CNSS_GW_UPDATE
c_sources += cnss_gw_update.c
endif
endif

if CONFIG_CNSS_USER
AM_CFLAGS += -DCONFIG_CNSS_USER
c_sources += cnss_user.c
endif
cnss_daemon_LDADD = ${LIBNL_LIBS}
cnss_daemon_CFLAGS = ${LIBNL_CFLAGS}
cnss_daemon_CFLAGS += ${AM_CFLAGS}

if !IPQ
cnss_daemon_CFLAGS += "-I${CLD80211_LIB}"
endif

cnss_daemon_SOURCES = $(c_sources)
cnss_daemon_LDADD += $(requiredlibs)

cnss_cli_SOURCES = $(c_cli_sources)
cnss_cli_LDADD = ${LIBNL_LIBS}
cnss_cli_CFLAGS = ${LIBNL_CFLAGS}
cnss_cli_CFLAGS += ${AM_CFLAGS}
cnss_cli_LDADD += $(requiredlibs)

sbin_PROGRAMS = cnss_daemon
if CONFIG_ENABLE_CNSSCLI
sbin_PROGRAMS += cnss_cli
endif
init_ddir = ${sysconfdir}/init.d
dist_init_d_SCRIPTS = start_cnss_daemon
