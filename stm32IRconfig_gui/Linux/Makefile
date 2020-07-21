all: stm32IRconfig_gui

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig_gui.o icons.o dfu.o stm32mem.o upgrade.o
OBJS=$(COBJS) $(CPPOBJS)
ifeq ($(OS),Windows_NT)
ifeq ($(MSYSTEM),MINGW32)
CFLAGS=-I/mingw32/include/fox-1.6 -I/mingw64/include/libusb-1.0 -g -c -s -Wall -mno-ms-bitfields -O2
# -static
LIBS= -mwindows -Wl,-Bstatic -lFOX-1.6 -lpng -ljpeg -ltiff -lz -llzma -lws2_32 -lgdi32 -static-libgcc -static-libstdc++ -lkernel32 -lsetupapi -lusb-1.0
else ifeq ($(MSYSTEM),MINGW64)
CFLAGS=-I/mingw64/include/fox-1.6 -I/mingw64/include/libusb-1.0 -g -c -s -Wall -mno-ms-bitfields -O2
# -static
LIBS= -mwindows -Wl,-Bstatic -lFOX-1.6 -lpng -ljpeg -ltiff -lz -lzstd -llzma -lws2_32 -lgdi32 -static-libgcc -static-libstdc++ -lkernel32 -lsetupapi -lusb-1.0
endif
CXXFLAGS=-g -s -Wall  -mno-ms-bitfields -O2
# -static // causes timing problems, libwinpthread-1.dll is needed
else
CFLAGS=-Wall -g -c `fox-config --cflags` -I/usr/include/libusb-1.0 -O2
LIBS=-ludev -lrt `fox-config --libs` -lusb-1.0
CXXFLAGS=-g -Wall -O2
endif

stm32IRconfig_gui: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o stm32IRconfig_gui

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32IRconfig_gui stm32IRconfig_gui.exe

.PHONY: clean
