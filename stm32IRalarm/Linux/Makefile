
PROJECTNAME=stm32IRalarm

CC              = g++

CFLAGS          = -O -Wall

TMPDIR = /tmp


all: stm32IRalarm

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32IRalarm: stm32IRalarm.o
	$(CC) -o stm32IRalarm stm32IRalarm.o

clean:
	rm -f stm32IRalarm.o
	rm -f stm32IRalarm
