ifeq ($(OFFTARGET), false)
top = $(WORKSPACE)
else
top = ../../../..
endif
shipPath = $(top)/security/securemsm
examplehubPath = $(shipPath)/smcinvoke/examplehub
credentialsPath = $(shipPath)/smcinvoke/credentials
minksocketPath = $(shipPath)/smcinvoke/minksocket

cflags = -fPIC -Wno-unused-parameter -Wno-unused-function -D_FORTIFY_SOURCE=2 -O3

includes = -I $(shipPath)/mink/inc/interface \
		-I $(shipPath)/mink/inc/uid \
		-I $(shipPath)/mink/inc/qtvm \
		-I $(shipPath)/smcinvoke/inc \
		-I $(shipPath)/smcinvoke/QCBOR/inc/ \
		-I $(minksocketPath)/sock \
		-I $(minksocketPath)/sock/sim_port \
		-I $(minksocketPath)/include \
		-I $(minksocketPath)/kernel \
		-I $(minksocketPath)/sharemm/inc \
		-I $(examplehubPath)/inc \
		-I $(credentialsPath)/inc/ \
		-I $(top)/vendor/qcom/proprietary/memory-ship/libvmmem/include/

MANAGER_SRC = $(examplehubPath)/src/ServiceManager.cpp \
			$(examplehubPath)/src/ServiceOpener.cpp \
			$(examplehubPath)/src/CRegisterModule.cpp \
			$(examplehubPath)/src/VMCredentials.cpp \
			$(examplehubPath)/src/MinkHub.cpp \
			$(examplehubPath)/src/ServiceModuleOpener.cpp
MANAGER_OBJ = $(MANAGER_SRC:cpp=o)
HUB_SRC = $(examplehubPath)/src/ExampleHub.cpp
HUB_OBJ = $(HUB_SRC:cpp=o)
HUBLEGACY_SRC = $(examplehubPath)/src/ExampleHubLegacy.cpp
HUBLEGACY_OBJ = $(HUBLEGACY_SRC:cpp=o)

ifeq ($(OFFTARGET), false)
cflags += -DVNDR_SMCINVOKE_QRTR -DVNDR_VSOCK -D_XOPEN_SOURCE=600
linkflags = -ldmabufheap -lvmmem -lqrtr -lminksocket_vendor
else
sanc = -fsanitize=address -static-libasan
sanl = -fsanitize=address
cflags += -DOFFTARGET
TRANSPORT_LIB = $(minksocketPath)/libminksocket_vendor.so
endif

CRED_LIB = $(credentialsPath)/libminkcredentials.a

TARGET = exampleHub
TARGET_LEGACY = exampleHubLegacy

all: $(TARGET) $(TARGET_LEGACY)

.PHONY: $(TARGET) $(TARGET_LEGACY)

$(TARGET): $(HUB_OBJ) $(MANAGER_OBJ) $(TRANSPORT_LIB) $(CRED_LIB)
	$(CXX) -o $@ $^ $(sanl) $(linkflags) -pthread -lcrypto -lssl -lrt

$(TARGET_LEGACY): $(HUBLEGACY_OBJ) $(MANAGER_OBJ) $(TRANSPORT_LIB) $(CRED_LIB)
	$(CXX) -o $@ $^ $(sanl) $(linkflags) -pthread -lcrypto -lssl -lrt

%.o: %.cpp
	$(CXX) -o $@ -c $(cflags) $(includes) $(includes_gtest) $<

%.o: %.c
	$(CC) -o $@ -c $(cflags) $(includes) $<

$(TRANSPORT_LIB):
	$(MAKE) -C $(minksocketPath)

$(CRED_LIB):
	$(MAKE) -C $(credentialsPath)

clean:
	rm -f exampleHub_PVM_sockUnix exampleHub_PVM_sockQRTR
	rm -f exampleHub_QTVM_sockUnix exampleHub_QTVM_sockQRTR exampleHub_QTVM_sockVSOCK
	rm -f exampleHub_OEMVM_sockUnix exampleHub_OEMVM_sockVsock
	rm -f $(TARGET) $(TARGET_LEGACY)
	rm -f $(HUB_OBJ) $(HUBLEGACY_OBJ) $(MANAGER_OBJ)
	$(MAKE) -C $(minksocketPath) clean
	$(MAKE) -C $(credentialsPath) clean

#do installation in securemsm.bb and securemsm-oemvm.bb
install:

