AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(SECUREMSM-NOSHIP_CFLAGS) \
        -I$(WORKSPACE)/security/securemsm/QSEEComAPI/ \
        -I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/cutils

ACLOCAL_AMFLAGS = -I m4

SECUREMSM_NOSHIP_PATH = $(WORKSPACE)/security/securemsm-noship
SECUREMSM_PATH = $(WORKSPACE)/security/securemsm

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG \
    -DOE \
    -I$(SECUREMSM_PATH)/daemon/include \
    -I$(SECUREMSM_PATH)/smcinvoke/inc/ \
    -I$(SECUREMSM_PATH)/smcinvoke/TZCom/inc/ \
    -I$(SECUREMSM_PATH)/smcinvoke/minksocket/include \
    -I$(SECUREMSM_PATH)/smcinvoke/minksocket/sock \
    -I$(SECUREMSM_PATH)/mink/inc \
    -I$(SECUREMSM_PATH)/mink/inc/uid \
    -I$(SECUREMSM_PATH)/mink/inc/interface \
    -I$(SECUREMSM_NOSHIP_PATH)/smcinvoke/QSEECom/inc \
    -std=c++17 \
    $(SECUREMSMNOSHIP_CFLAGS)

#init_ddir = ${sysconfdir}/init.d
#init_d_SCRIPTS = start_qseecomd

bin_PROGRAMS = qseecomd
qseecomd_SOURCES = qseecomd.c
qseecomd_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
qseecomd_CPPFLAGS = $(AM_CPPFLAGS)
qseecomd_CFLAGS = $(AM_CFLAGS)
if ENABLE_HYP
qseecomd_CFLAGS += -DLISTENERS_VIRTUALIZED
endif
if LOAD_CMNLIB
qseecomd_CFLAGS += -DENABLE_CMNLIB_LOAD
endif
if LV_BUILD
qseecomd_CFLAGS += -DENABLE_LV_SECURITY
qseecomd_LDFLAGS = -lgpt
endif
if TARGET_ENABLE_QSEECOM
qseecomd_CFLAGS+= -DTARGET_ENABLE_QSEECOM
endif

qseecomd_CFLAGS += -DCOMPILE_GPT_LISTENER \
                   -DCOMPILE_RPMB_LISTENER

qseecomd_LDFLAGS = -lpthread -ldl -lcutils -lion -lrpmb -lssd -ldrmfs

if TARGET_ENABLE_SMCI_SYSLISTENER
qseecomd_SOURCES += CListenersmci.cpp \
                    listenerMngr.cpp

qseecomd_CFLAGS += -DTARGET_ENABLE_SMCI_SYSLISTENER
qseecomd_LDFLAGS += -lminkdescriptor -ldmabufheap @SECUREMSMNOSHIP_LIBS@
else
qseecomd_CFLAGS += -I$(top_srcdir)/QSEEComAPI
qseecomd_LDFLAGS += -lQseeComApi
endif
