#                                               -*- Autoconf -*-

# configure.ac -- Autoconf script for cnss_service.
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the cnss_daemon package version 1.0.0
AC_INIT([cnss_daemon],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

PKG_CHECK_MODULES([QMI_FRAMEWORK], [qmi-framework])
AC_SUBST([QMI_FRAMEWORK_CFLAGS])
AC_SUBST([QMI_FRAMEWORK_LIBS])

AC_ARG_ENABLE([debug],
        [  --enable-debug    Turn on debugging],
        [case "${enableval}" in
            yes) debug=true ;;
            no)  debug=false ;;
            *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
        esac],[debug=false])

AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AM_CONDITIONAL([ENABLE_IP_SOLICIT_RETRY], [test x$ip_solicit_retry = xtrue])
AM_CONDITIONAL([CONFIG_DEBUG_LOGCAT], [test x$debug_logcat = xtrue])
AM_CONDITIONAL([ENABLE_INTEROP_ISSUES_AP_DETECTION], [test x$interop_issues_detection = xtrue])
AM_CONDITIONAL([ENABLE_NETLINK_COMMON], [test x$netlink_common = xtrue])
AM_CONDITIONAL([ENABLE_SUBNET_DETECTION], [test x$subnet_detection = xtrue])
AM_CONDITIONAL([CONFIG_CNSS_DP], [test x$cnss_dp_support = xtrue])
AM_CONDITIONAL([CONFIG_CNSS_USER], [test x$cnss_user_support = xtrue])
AM_CONDITIONAL([CONFIG_DEBUG_SYSLOG], [test x$debug_syslog_support = xtrue])
AM_CONDITIONAL([CONFIG_DEBUG_FILE], [test x$debug_file_support = xtrue])
AM_CONDITIONAL([CONFIG_RECORD_DAEMON_QMI_LOG], [test x$record_daemon_qmi_log_support= xtrue])
AM_CONDITIONAL([IPQ], [test x$ipq_support = xtrue])
AM_CONDITIONAL([CONFIG_ENABLE_CNSSCLI], [test x$enable_cnsscli = xtrue])

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, building HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GLib >= 2.16 is required))
        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi
AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_ARG_VAR(CLD80211_LIB,[Path to cld80211-lib directory])
AS_IF([test "${CLD80211_LIB}" = ""],[AC_MSG_WARN("CLD80211_LIB=path to cld80211-lib not provided. Pass path or provide --disable-cld80211-lib as argument")])

AC_ARG_ENABLE([cld80211_lib],AS_HELP_STRING([--disable-cld80211-lib], [Disable feature cld80211-lib]))
AM_CONDITIONAL(DISABLE_CLD80211_LIB, test "x${enable_cld80211_lib}" = "xno")

has_libnl_ver=0
# libnl-2 provides only libnl-2.0.pc file, so we check for separate libnl-genl-3.0.pc
# pkg-config file just for libnl-3.0 case.
#
PKG_CHECK_MODULES([LIBNL], [libnl-3.0 >= 3.0 libnl-genl-3.0 >= 3.0], [has_libnl_ver=3], [
   PKG_CHECK_MODULES([LIBNL], [libnl-2.0 >= 2.0], [has_libnl_ver=2], [
           PKG_CHECK_MODULES([LIBNL], [libnl-1], [has_libnl_ver=1], [has_libnl_ver=0])])])

if (test "$has_libnl_ver" -eq 0); then
        AC_MSG_ERROR(libnl and libnl-genl are required but were not found)
fi

AC_SUBST([LIBNL_CFLAGS])
AC_SUBST([LIBNL_LIBS])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC

AC_CONFIG_FILES([ \
        Makefile \
        ])
AC_OUTPUT

