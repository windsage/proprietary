OBJS=bt_config.o hw_rome.o hci_uart.o
ALL=btconfig

.PHONY: all clean

all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -g3 -Wall $(OBJS) -o btconfig

clean:
	rm -rf *.o btconfig
