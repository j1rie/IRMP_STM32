all: stm32IRalarm

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRalarm.o
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-g -c -Wall
ifeq ($(MSYSTEM),MINGW32)
LIBS= -lsetupapi -static-libgcc
else ifeq ($(MSYSTEM),MINGW64)
LIBS= -lsetupapi
endif
CXXFLAGS=-g -s

stm32IRalarm: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o stm32IRalarm

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32IRalarm.exe

.PHONY: clean
