AC_PREREQ(2.61)
AC_INIT([mink],1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Check for programs
AM_PROG_AS
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

AC_ARG_WITH([lv-build],
    AC_HELP_STRING([--with-lv-build],
                   [enable LV build, building HLOS systems on LV platform]),
    [with_lv="${withval}"],
    [with_lv="no"])

AM_CONDITIONAL(LV_BUILD, test "x${with_lv}" = "xyes")

AM_COND_IF([LV_BUILD],
           [AC_CONFIG_FILES([ \
            Makefile \
            ])
            ])

AC_OUTPUT

