#                                               -*- Autoconf -*-

# configure.ac -- Autoconf script for diag-router
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([diag-router], 1.0.0)
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])

DIAG_LIBRARY_NAME=diag

#release versioning
DIAG_MAJOR_VERSION=1
DIAG_MINOR_VERSION=0
DIAG_MICRO_VERSION=0

DIAG_LIBRARY_VERSION=1:0:0
AC_SUBST(DIAG_LIBRARY_VERSION)

PACKAGE=$DIAG_LIBRARY_NAME
AC_SUBST(DIAG_LIBRARY_NAME)

DIAG_VERSION=$DIAG_MAJOR_VERSION.$DIAG_MINOR_VERSION.$DIAG_MICRO_VERSION
DIAG_RELEASE=$DIAG_MAJOR_VERSION.$DIAG_MINOR_VERSION
AC_SUBST(DIAG_RELEASE)
AC_SUBST(DIAG_VERSION)

VERSION=$DIAG_VERSION

LT_INIT
AM_PROG_LIBTOOL
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
PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE([target-sdxkuno],
        [AS_HELP_STRING([--enable-target-sdxkuno], [enable sdxkuno features])],
        [sdxkuno_features=${enableval}],
        [sdkuno_features=no]
)

if test "x${sdxkuno_features}" = xyes; then
        CFLAGS="${CFLAGS} -DFEATURE_SDXKUNO_DIAG"
fi

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

AC_ARG_WITH([common_includes],
      AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
         [Specify the location of the common headers]),
      [common_incdir=$withval],
      with_common_includes=no)

AC_ARG_WITH([header_includes],
      AC_HELP_STRING([--with-header-includes=@<:@dir@:>@],
         [Specify the location of the headers]),
      [common_header_incdir=$withval],
      with_header_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${common_incdir}"
fi

if test "x$with_header_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${common_header_incdir}"
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_ARG_WITH([diag-televm],
   AC_HELP_STRING([--with-diag-televm], [Enable diag support on tele-vm]))

if test "x${with_diag_televm}" = xyes; then
   CFLAGS="${CFLAGS} -DDIAG_TELE_VM_ENABLED"
fi

AC_ARG_WITH([diag-fotavm],
   AC_HELP_STRING([--with-diag-fotavm], [Enable diag support on fota-vm]))

if test "x${with_diag_fotavm}" = xyes; then
   CFLAGS="${CFLAGS} -DDIAG_FOTA_VM_ENABLED"
fi

AC_ARG_WITH([diag-id-pkt],
   AC_HELP_STRING([--with-diag-id-pkt], [Enable diag-id based cmd req/resp and async packets]))

if test "x${with_diag_id_pkt}" = xyes; then
   CFLAGS="${CFLAGS} -DDIAG_ID_BASED_PKT_ENABLED"
fi

AC_ARG_ENABLE(target,
  [AS_HELP_STRING([--enable-target=TARGET], [Specify the target product to build])],
  [TARGET=$enableval],
  [TARGET=none]
)

PKG_CHECK_MODULES([QMIFW], [qmi-framework])
AC_SUBST([QMIFW_CFLAGS])
AC_SUBST([QMIFW_LIBS])

AC_SUBST([CFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([     \
	Makefile      \
	router/Makefile	\
	diag-router.pc
	])
AC_OUTPUT

