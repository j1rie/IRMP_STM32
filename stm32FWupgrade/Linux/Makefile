OUTFILE = stm32FWupgrade

CC = gcc

CFLAGS = -I/usr/include/libusb-1.0 -Wall -Wextra -g -s
LDFLAGS = -lusb-1.0

OBJ = dfu.o stm32mem.o main.o

all: $(OUTFILE)

$(OUTFILE) $(OUTFILE).exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OUTFILE) $(OUTFILE).exe *.o
