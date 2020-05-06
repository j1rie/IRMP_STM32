all: stm32IRconfig_gui

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig_gui.o icons.o upgrade.o stm32mem.o dfu.o
OBJS=$(COBJS) $(CPPOBJS)
ifeq ($(MSYSTEM),MINGW32)
CFLAGS=-I/mingw32/include/fox-1.6 -I/mingw64/include/libusb-1.0 -g -c -s -Wall -O0 -MD -mno-ms-bitfields
# -static
LIBS= -mwindows -Wl,-Bstatic -lFOX-1.6 -lpng -ljpeg -ltiff -lz -llzma -lws2_32 -lgdi32 -static-libgcc -static-libstdc++ -lkernel32 -lsetupapi -lusb-1.0
else ifeq ($(MSYSTEM),MINGW64)
CFLAGS=-I/mingw64/include/fox-1.6 -I/mingw64/include/libusb-1.0 -g -c -s -Wall -O0 -MD -mno-ms-bitfields
# -static
LIBS= -mwindows -Wl,-Bstatic -lFOX-1.6 -lpng -ljpeg -ltiff -lz -llzma -lws2_32 -lgdi32 -static-libgcc -static-libstdc++ -lkernel32 -lsetupapi -lusb-1.0
endif
CXXFLAGS= -g -s -Wall -O0 -MD -mno-ms-bitfields
# -static // causes timing problems, libwinpthread-1.dll is needed

stm32IRconfig_gui: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o stm32IRconfig_gui

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32IRconfig_gui.exe *.d

.PHONY: clean
