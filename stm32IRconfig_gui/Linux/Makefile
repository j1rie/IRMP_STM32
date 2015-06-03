all: stm32IRconfig_gui

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32IRconfig_gui.o icons.o
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-Wall -g -c `fox-config --cflags` `pkg-config libusb-1.0 --cflags`
LIBS=-ludev -lrt -lpthread `fox-config --libs` `pkg-config libusb-1.0 --libs`

stm32IRconfig_gui: $(OBJS)
	g++ -Wall -g $^ $(LIBS) -o stm32IRconfig_gui

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm *.o stm32IRconfig_gui

.PHONY: clean
