# Makefile.am - Automake script for minkipc

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = TZCom credentials

if !TARGET_ENABLE_MINK
SUBDIRS += minksocket
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = minkipc.pc
EXTRA_DIST = $(pkgconfig_DATA)

