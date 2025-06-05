AC_PREREQ(2.61)
AC_INIT([securemsm], 1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign subdir-objects])
AM_MAINTAINER_MODE
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AM_PROG_AS
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_CXX

#LT_INIT

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([SECUREMSMNOSHIP], [securemsm-noship])
AC_SUBST([SECUREMSMNOSHIP_LIBS])
AC_SUBST([SECUREMSMNOSHIP_CFLAGS])

PKG_CHECK_MODULES([LIBVMMEM], [libvmmem])
AC_SUBST([LIBVMMEM_CFLAGS])
AC_SUBST([LIBVMMEM_LIBS])

AC_ARG_WITH(sanitized-headers,
AS_HELP_STRING([--with-sanitized-headers=DIR],
[Specify the location of the sanitized Linux headers]),
[CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([kernel],
AC_HELP_STRING([--with-kernel=@<:@dir@:>@],
[Specify the location of the Linux kernel headers]),
[kerneldir=$withval],
with_kernel=no)

AC_ARG_ENABLE([trustedui],
              AC_HELP_STRING([--enable-trustedui],
                             [Enable trustedui components]),
                             [trustedui="${enableval}"],
                             trustedui=no)

AM_CONDITIONAL([TRUSTED_UI], [test "x$trustedui" = xyes])

AC_ARG_ENABLE([tuioemvm],
              AC_HELP_STRING([--enable-tuioemvm],
                             [Enable tuioemvm components]),
                             [tuioemvm="${enableval}"],
                             tuioemvm=no)

AM_CONDITIONAL([TUI_OEM_VM], [test "x$tuioemvm" = xyes])

AC_ARG_ENABLE([swencrypt],
              AC_HELP_STRING([--enable-swencrypt],
                             [Enable SW encryption]),
                             [swencrypt="${enableval}"],
                             swencrypt=no)

AM_CONDITIONAL([SW_ENCRYPT], [test "x$swencrypt" = xyes])

AC_ARG_ENABLE(hyp,
    AC_HELP_STRING( [--enable-hyp], [Enable hyp]),
                    [hyp_build="${enableval}"], [hyp_build="no"])

AM_CONDITIONAL([ENABLE_HYP],[test "x${hyp_build}" = "xyes"])

if test "x$with_kernel" != "xno"; then
   CFLAGS="${CFLAGS} -I${kerneldir}/include -I${kerneldir}/arch/arm/include"
fi

AC_ARG_ENABLE(cmnlib-load,
    AC_HELP_STRING([--enable-cmnlib-load], [Enable cmnlib loading]),
            [cmnlib_load=${enableval}], [cmnlib_load="no"])

AM_CONDITIONAL([LOAD_CMNLIB],[test "x${cmnlib_load}" = "xyes"])

AS_CASE([$host],
    [arm*], [ARM=yes],
    [ARM=no]
)

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

AC_ARG_WITH([lv-build],
    AC_HELP_STRING([--with-lv-build],
                   [enable LV build, building HLOS systems on LV platform]),
    [with_lv="${withval}"],
    [with_lv="no"])

AM_CONDITIONAL(LV_BUILD, test "x${with_lv}" = "xyes")

AC_ARG_ENABLE([qseecompat],
        AC_HELP_STRING([--enable-qseecompat], [Enable QseeCompat Support]),
        [qseecompat="${enableval}"], [qseecompat="no"])

AM_CONDITIONAL([TARGET_ENABLE_QSEECOMCOMPAT],[test "x${qseecompat}" = "xyes"])

AC_ARG_ENABLE([qti_qseecom],
        AC_HELP_STRING([--enable-qti-qseecom], [Enable QseeCom Support]),
        [qti_qseecom="${enableval}"], [qti_qseecom="no"])

AM_CONDITIONAL([TARGET_ENABLE_QSEECOM],[test "x${qti_qseecom}" = "xyes"])

AC_ARG_ENABLE([qti_mink],
        AC_HELP_STRING([--enable-qti-mink], [Enable mink component]),
        [qti_mink="${enableval}"], [qti_mink="no"])

AM_CONDITIONAL([TARGET_ENABLE_MINK],[test "x${qti_mink}" = "xyes"])

AC_ARG_ENABLE([qti_smciqseecomd],
        AC_HELP_STRING([--enable-qti-smciqseecomd], [Enable smciqseecomd components]),
        [qti_smciqseecomd="${enableval}"], [qti_smciqseecomd="no"])

AM_CONDITIONAL([TARGET_ENABLE_SMCI_SYSLISTENER],[test "x${qti_smciqseecomd}" = "xyes"])

AM_CONDITIONAL(ARM, [test "x$ARM" = "xyes"])
AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([Makefile \
            km_lite/tests/Makefile \
            rpmbClient/Makefile \
            securemsm.pc])

AM_COND_IF([TRUSTED_UI],
           [AC_CONFIG_FILES([ \
           TrustedUI/TrustedVM/Composer/LayoutManager/Makefile \
           TrustedUI/TrustedVM/libTrustedUI/Makefile
        ])])

if (test "x${qti_qseecom}" = "xyes" -o "x${qseecompat}" = "xyes"); then
AC_CONFIG_FILES([ \
    sampleclient/Makefile \
])
fi

AM_COND_IF([LV_BUILD],
           [AC_CONFIG_FILES([ \
            qapi/build/Makefile \
            GPTEE_Sample_client/Makefile \
            ])
            ])

if (test "x${qti_qseecom}" = "xyes" -o "x${qseecompat}" = "xyes" -o "x${qti_smciqseecomd}" = "xyes"); then
AC_CONFIG_FILES([ \
    daemon/Makefile \
])
fi

AC_OUTPUT
