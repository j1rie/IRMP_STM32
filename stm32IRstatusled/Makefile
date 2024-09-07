
PROJECTNAME=stm32IRstatusled

CC              = g++

CFLAGS          = -O -Wall

all: stm32IRstatusled

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32IRstatusled: stm32IRstatusled.o
	$(CC) -o stm32IRstatusled stm32IRstatusled.o

clean:
	rm -f stm32IRstatusled.o
	rm -f stm32IRstatusled
