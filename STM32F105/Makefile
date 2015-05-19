TARGET ?= IR
PLATFORM ?= STM32F105RC
#STM32F105RC
#	CL - connectivity line
#	max. 72MHz
#	256 KB (0x40000) of Flash memory
#	 40 KB (0x10000) of SRAM

CROSS_COMPILE ?= arm-none-eabi-
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CC)
OBJCP = $(CROSS_COMPILE)objcopy

ARCH = -mcpu=cortex-m3 -mthumb
COMMON = -g -Os -flto
INCLUDES = -Icmsis -Icmsis_boot -Istm_lib/inc -Iusb_hid/inc -Iirmp -Isrc
DEFINES = -D$(PLATFORM) -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER -DSOF_CALLBACK

DONT_CARE := $(shell ./scripts/prepare.sh)

CRT0 = cmsis_boot/startup/startup_stm32f10x_cl.o
SRCS = $(shell find -type f -name "*.c")
OBJS = $(CRT0) $(SRCS:.c=.o)

CFLAGS = -Wall -ffunction-sections -fno-builtin $(ARCH) $(COMMON) $(INCLUDES) $(DEFINES)
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link.ld $(ARCH) $(COMMON)

ifdef SimpleCircuit
DEFINES += -DSimpleCircuit
endif

$(TARGET).bin: $(TARGET).elf
	$(OBJCP) -O binary $< $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

.PHONY: flash clean distclean
flash: $(TARGET).bin
	stm32flash -v -w $(TARGET).bin /dev/ttyUSB0

clean:
	rm -f $(OBJS) $(TARGET).{bin,elf,map}

distclean: clean
	rm -rf ext
