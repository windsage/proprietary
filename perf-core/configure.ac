#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for perf_service
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the perf-hal version 2.3
AC_INIT([perf-hal],2.3)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign subdir-objects])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
PKG_CHECK_MODULES([LIBUTILS], [libutils])
AC_SUBST([LIBUTILS_CFLAGS])
AC_SUBST([LIBUTILS_LIBS])

PKG_CHECK_MODULES([LIBCUTILS], [libcutils])
AC_SUBST([LIBCUTILS_CFLAGS])
AC_SUBST([LIBCUTILS_LIBS])

PKG_CHECK_MODULES([XML2], [libxml-2.0])
AC_SUBST([XML2_CFLAGS])
AC_SUBST([XML2_LIBS])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
#AC_SUBST([AM_CPPFLAGS], [--std=c++11])

# Check for programs
AM_PROG_AR

AC_ARG_WITH([core_includes],
    AC_HELP_STRING([--with-core-includes=@<:@dir@:>@],
       [Specify the location of the core headers]),
    [core_incdir=$withval],
    with_core_includes=no)

if test "x$with_core_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${core_incdir}"
fi

AC_ARG_WITH(sanitized-headers,
   AS_HELP_STRING([--with-sanitized-headers=DIR],
       [Specify the location of the sanitized Linux headers]),
   [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH(systemd,
        AS_HELP_STRING([--with-systemd], [enable perf-hal in systemd]),
        [SYSTEMD="yes"],
        [SYSTEMD="no"])
AM_CONDITIONAL([USE_SYSTEMD], [test "x${SYSTEMD}" != "xno"])

AC_ARG_ENABLE([openwrt-perf],
  AC_HELP_STRING([--enable-openwrt-perf],
         [Enable conditional compile for openwrt]),
         [openwrt_perf="${enableval}"],
         openwrt_perf=no)
AM_CONDITIONAL([OPENWRT_PERF], [test "x$openwrt_perf" = xyes])
# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT([dlopen shared])

AC_SUBST([CFLAGS])
AC_SUBST([CC])
AC_DEFINE([LE_CODE], [1], ["LE Code"])
AH_TOP(
[#ifndef __PERF_LE_CONFIG_FILE__
#define __PERF_LE_CONFIG_FILE__
#include <glib.h>
])
AH_BOTTOM([
#endif //__PERF_LE_CONFIG_FILE__
])

AC_CONFIG_FILES([Makefile
        perf-util/Makefile \
        mp-ctl-client/Makefile \
        perf-hal/Makefile \
        ioctl/Makefile \
        mp-ctl/Makefile \
        tests/ModuleTests/Makefile \
        tests/Makefile \
        tests/UnitTests/perf-hal/Makefile \
        tests/UnitTests/Makefile \
        configs/Makefile
    ])
AC_OUTPUT
