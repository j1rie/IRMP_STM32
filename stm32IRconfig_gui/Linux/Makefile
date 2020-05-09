all: stm32IRconfig_gui

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig_gui.o icons.o dfu.o stm32mem.o upgrade.o
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-Wall -g -c `fox-config --cflags` -I/usr/include/libusb-1.0
LIBS=-ludev -lrt `fox-config --libs` -lusb-1.0

stm32IRconfig_gui: $(OBJS)
	g++ -Wall -g $^ $(LIBS) -o stm32IRconfig_gui

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm *.o stm32IRconfig_gui

.PHONY: clean
