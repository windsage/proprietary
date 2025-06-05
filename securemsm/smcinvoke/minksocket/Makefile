ifeq ($(OFFTARGET), false)
top = $(WORKSPACE)
else
top = ../../../..
endif
shipPath = $(top)/security/securemsm
libvmmempath = $(top)/vendor/qcom/proprietary/memory-ship/libvmmem

cflags = -fPIC -Wall -Werror \
         -Wno-unused-parameter -Wno-unused-function -Wno-missing-field-initializers

includes = -I ./kernel  \
           -I ./include \
           -I ./sock \
           -I ./sharemm/inc \
           -I $(shipPath)/mink/inc/interface \
           -I $(shipPath)/mink/inc/qtvm

SRCS_ALL = $(wildcard ./sock/*.c) $(wildcard ./ext/*.c)
SRCS_ALL += $(wildcard ./sharemm/src/*.c)

ifeq ($(DEBUG), true)
cflags += -g
endif

ifeq ($(OEM_VM), true)
cflags += -DENABLE_TUI_OEM_VM
endif

ifeq ($(CUSTOMIZED_PLATFORM), true)
cflags += -D_CUSTOMIZED_PLATFORM
SRCS_ALL += vmosal/VmOsalCustomize.c
else
SRCS_ALL += vmosal/VmOsalUnix.c
endif

ifeq ($(OFFTARGET), false)
cflags += -DOE -DVNDR_SMCINVOKE_QRTR -DVNDR_VSOCK -D_XOPEN_SOURCE=600
linkflags += -lvmmem -ldmabufheap -lqrtr -lpthread
SRCS = $(filter-out ./sharemm/src/stub.c, $(SRCS_ALL))
else
sanc = -fsanitize=address -static-libasan
cflags += -DOFFTARGET -DSTUB
includes += -I $(libvmmempath)/include
SRCS = $(SRCS_ALL)
DESTDIR = out
endif

OBJS = $(SRCS:c=o)
OBJS_NOSTUB = $(filter-out ./sharemm/src/stub.o, $(OBJS))

LIB = libminksocket_vendor.so
LIB_STATIC = libminksocket_vendor.a
all: $(LIB) $(LIB_STATIC)

$(LIB): $(OBJS)
	@echo "compiling dynamic library"
	$(AR) crs $@ $^

$(LIB_STATIC): $(OBJS_NOSTUB)
	@echo "compiling static library"
	$(AR) crs $@ $^

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(cflags) $(includes) $(sanc) $<

clean: clean_objs
	rm -f $(LIB) $(LIB_STATIC)
	rm -rf $(DESTDIR)

clean_objs:
	rm -f $(OBJS)

install:
ifeq ($(OFFTARGET), false)
else
	mkdir -p $(DESTDIR)$(prefix)/lib/
	mkdir -p $(DESTDIR)$(prefix)/include/minkipc/
endif
	install -D -m 755 $(LIB) $(DESTDIR)$(prefix)/lib/$(LIB)
	install -D -m 755 $(LIB_STATIC) $(DESTDIR)$(prefix)/lib/$(LIB_STATIC)
	# TO-DO : Automate this
	install -D -m 755 sock/VmOsal.h $(DESTDIR)$(prefix)/include/minkipc/VmOsal.h
	install -D -m 755 sock/minkipc.h $(DESTDIR)$(prefix)/include/minkipc/minkipc.h
	install -D -m 755 sock/fdwrapper.h $(DESTDIR)$(prefix)/include/minkipc/fdwrapper.h
	install -D -m 755 sock/LinkCredentials.h $(DESTDIR)$(prefix)/include/minkipc/LinkCredentials.h
	install -D -m 755 sock/xtzdCredentials.h $(DESTDIR)$(prefix)/include/minkipc/xtzdCredentials.h
	install -D -m 755 sharemm/inc/MSMem.h $(DESTDIR)$(prefix)/include/minkipc/MSMem.h
	install -D -m 755 sharemm/inc/RemoteShareMemory.h $(DESTDIR)$(prefix)/include/minkipc/RemoteShareMemory.h
	install -D -m 755 kernel/qlist.h $(DESTDIR)$(prefix)/include/minkipc/qlist.h
	install -D -m 755 kernel/kdefs.h $(DESTDIR)$(prefix)/include/minkipc/kdefs.h
	install -D -m 755 kernel/bbuf.h $(DESTDIR)$(prefix)/include/minkipc/bbuf.h
	install -D -m 755 include/cdefs.h $(DESTDIR)$(prefix)/include/minkipc/cdefs.h
	install -D -m 755 include/check.h $(DESTDIR)$(prefix)/include/minkipc/check.h
	install -D -m 755 include/vmuuid.h $(DESTDIR)$(prefix)/include/minkipc/vmuuid.h
	install -D -m 755 sock/minksocket.h $(DESTDIR)$(prefix)/include/minkipc/minksocket.h
	install -D -m 755 sock/msforwarder.h $(DESTDIR)$(prefix)/include/minkipc/msforwarder.h
	install -D -m 755 sock/ProtocolVsock.h $(DESTDIR)$(prefix)/include/minkipc/ProtocolVsock.h
	install -D -m 755 sock/ConnEventHandler.h $(DESTDIR)$(prefix)/include/minkipc/ConnEventHandler.h
