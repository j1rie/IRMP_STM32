
PROJECTNAME=stm32IRconfig

CC              = g++

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

all: stm32IRconfig

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32IRconfig: stm32IRconfig.o
	$(CC) -o stm32IRconfig stm32IRconfig.o

#$(LIBS)

clean:
	rm -f stm32IRconfig.o
	rm -f stm32IRconfig
