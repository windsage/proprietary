# configure.ac -- Autoconf script for gps eng-hub-msg-proto
#
# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the gps lbs-core package version 1.0.0
AC_INIT([eng-hub-msg-proto],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign subdir-objects])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([src/Makefile.am])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([GPSUTILS], [qcom-gps-utils])
AC_SUBST([GPSUTILS_CFLAGS])
AC_SUBST([GPSUTILS_LIBS])

AC_ARG_ENABLE(target,
  [AS_HELP_STRING([--enable-target=TARGET], [Specify the target product to build])],
  [TARGET=$enableval],
  [TARGET=none]
)

AC_ARG_WITH([core_includes],
      AC_HELP_STRING([--with-core-includes=@<:@dir@:>@],
               [Specify the location of the core headers]),
               [core_incdir=$withval],
               with_core_includes=no)

if test "x$with_core_includes" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -I${core_incdir}"
fi

AC_ARG_WITH([locpla_includes],
      AC_HELP_STRING([--with-locpla-includes=@<:@dir@:>@],
        [Specify the path to locpla-includes in loc-pla_git.bb]),
        [locpla_incdir=$withval],
        with_locpla_includes=no)

if test "x${with_locpla_includes}" != "xno"; then
   AC_SUBST(LOCPLA_CFLAGS, "-I${locpla_incdir}")
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

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

## cdfw enabled or not
AC_ARG_ENABLE(cdfw,
    [AS_HELP_STRING([--enable-cdfw=TARGET_CDFW_ENABLED],
            [If CDFW is enabled for this target])],
    [TARGET_CDFW_ENABLED=$enableval],
    [TARGET_CDFW_ENABLED=no]
)

if (test "x$TARGET_CDFW_ENABLED" == "xyes"); then
    ## include cdfw
    AC_MSG_NOTICE([CDFW is enabled for target.])
    PKG_CHECK_MODULES([CDFW], [cdfw])
    AC_SUBST([CDFW_CFLAGS])
    AC_SUBST([CDFW_LIBS])

    AC_ARG_WITH([cdfw_api_includes],
          AC_HELP_STRING([--with-cdfw-api-includes=@<:@dir@:>@],
            [Specify the path to cdfw-api-includes in cfdw_git.bb]),
          [cdfw_api_incdir=$withval],
          with_cdfw_api_includes=no)

    if test "x${with_cdfw_api_includes}" != "xno"; then
       AC_SUBST(CDFW_API_CFLAGS, "-I${cdfw_api_incdir}")
    fi

else
    AC_MSG_NOTICE([CDFW is not enabled for target..])
fi

AM_CONDITIONAL(CDFW_ENABLED, test "x$TARGET_CDFW_ENABLED" == "xyes")

## automotive enabled target.
AC_ARG_WITH([auto_feature],
    AC_HELP_STRING([--with-auto_feature=@<:@dir@:>@],
        [Using Automotive feature]),
    [],
    with_auto_feature=no)

if test "x$with_auto_feature" != "xno"; then
    AC_MSG_NOTICE([Automotive enabled target - Using Automotive feature.])
    CPPFLAGS="${CPPFLAGS} -DFEATURE_AUTOMOTIVE"
fi

AM_CONDITIONAL(USE_FEATURE_AUTOMOTIVE, test "x${with_auto_feature}" = "xyes")

AC_CONFIG_FILES([ \
        Makefile \
        src/Makefile \
        eng-hub-msg-proto.pc
        ])

AC_OUTPUT
