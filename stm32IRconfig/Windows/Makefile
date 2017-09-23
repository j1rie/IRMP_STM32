all: stm32IRconfig

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig.o
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-g -c -Wall -Os -flto
#LIBS= -mwindows -lsetupapi
LIBS= -lsetupapi
#LDFLAGS+=static

stm32IRconfig: $(OBJS)
	g++ -g $^ $(LIBS) -o stm32IRconfig

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32IRconfig.exe

.PHONY: clean
