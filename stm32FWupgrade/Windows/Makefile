OUTFILE = stm32FWupgrade

CC = gcc

ifeq ($(OS),Windows_NT)
CFLAGS = -I/mingw64/include/libusb-1.0 -Wall -Wextra -g -s -mno-ms-bitfields -O2
LDFLAGS = -Wl,-Bstatic -lusb-1.0
else
CFLAGS = -I/usr/include/libusb-1.0 -Wall -Wextra -g -s -O2
LDFLAGS = -lusb-1.0
endif

OBJ = dfu.o stm32mem.o main.o

all: $(OUTFILE)

$(OUTFILE) $(OUTFILE).exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OUTFILE) $(OUTFILE).exe *.o
