all: stm32IRconfig_gui

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig_gui.o icons.o
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-I../hidapi -I/mingw64/include/fox-1.6 -g -c
LIBS= -mwindows -lsetupapi -L/mingw64/lib -Wl,-Bstatic -lFOX-1.6 -lpng -ljpeg -ltiff -lz -llzma -lws2_32 -Wl,-Bdynamic -lgdi32 -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32


stm32IRconfig_gui: $(OBJS)
	g++ -g $^ $(LIBS) -o stm32IRconfig_gui

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32IRconfig_gui.exe

.PHONY: clean
