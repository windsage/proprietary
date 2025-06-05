cflags = -Wall -fPIC -O2

ifeq ($(OFFTARGET), false)
top = $(WORKSPACE)
cflags += -DOE -D_XOPEN_SOURCE=600
else
top = ../../../..
cflags += -DOFFTARGET -DSTUB
sanc = -fsanitize=address -static-libasan
sanl = -fsanitize=address
endif

SHIP_PATH = $(top)/security/securemsm

LIB_NAME = libminkcredentials
LIB_STATIC = libminkcredentials.a

SRC = $(wildcard src/*.c)

INCLUDES = -I./inc/ \
           -I$(SHIP_PATH)/smcinvoke/QCBOR/inc/ \
           -I$(SHIP_PATH)/smcinvoke/minksocket/include/ \
           -I$(SHIP_PATH)/mink/inc/interface/ \
           -I$(SHIP_PATH)/mink/inc/qtvm/ \
           -I$(SHIP_PATH)/smcinvoke/minksocket/kernel/ \

OBJ = $(SRC:c=o)

all: $(LIB_STATIC)

$(LIB_STATIC): $(OBJ)
	@echo "compiling static library"
	$(AR) crs $@ $^

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(cflags) $(INCLUDES) $(sanc) $<

install:

clean:
	rm -f $(OBJ)
	rm -f $(LIB_STATIC)
