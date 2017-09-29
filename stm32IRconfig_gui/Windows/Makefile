all: stm32IRconfig_gui

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig_gui.o icons.o
OBJS=$(COBJS) $(CPPOBJS)
ifeq ($(MSYSTEM),MINGW32)
CFLAGS=-I/mingw32/include/fox-1.6 -g -c -static -s -Wall
LIBS= -mwindows -L/mingw32/lib -L/mingw32/i686-w64-mingw32/lib -Wl,-Bstatic -lwinpthread -lFOX-1.6 -lpng -ljpeg -ltiff -lz -llzma -lws2_32 -lgdi32 -static-libgcc -static-libstdc++ -lkernel32 -lsetupapi
else ifeq ($(MSYSTEM),MINGW64)
CFLAGS=-I/mingw64/include/fox-1.6 -g -c -static -s -Wall
LIBS= -mwindows -L/mingw64/lib -L/mingw64/x86_64-w64-mingw32/lib -Wl,-Bstatic -lwinpthread -lFOX-1.6 -lpng -ljpeg -ltiff -lz -llzma -lws2_32 -lgdi32 -static-libgcc -static-libstdc++ -lkernel32 -lsetupapi
endif
CXXFLAGS= -g -static -s

stm32IRconfig_gui: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o stm32IRconfig_gui

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32IRconfig_gui.exe

.PHONY: clean
