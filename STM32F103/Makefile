TARGET ?= IR
PLATFORM ?= STM32F103C8
#STM32F103C8
#	MD PL - medium density performance line
#	max. 72MHz
#	64 KB (0x10000) of Flash memory
#	20 KB (0x05000) of SRAM

CROSS_COMPILE ?= arm-none-eabi-
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
OBJCP = $(CROSS_COMPILE)objcopy

ARCH = -mcpu=cortex-m3 -mthumb
COMMON = -g -Os -flto
INCLUDES = -Iext/stm32lib-4.0.0 -Iext/irmp-2.6.7 -Isrc
DEFINES = -D$(PLATFORM) -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DIRMP_LOGGING

DONT_CARE := $(shell ./scripts/prepare.sh)

CRT0 = ext/stm32lib-4.0.0/startup_stm32f10x_md.o
SRCS = $(shell find -type f -name "*.c")
OBJS = $(CRT0) $(SRCS:.c=.o)

CFLAGS = -Wall -ffunction-sections -fno-builtin $(ARCH) $(COMMON) $(INCLUDES) $(DEFINES)
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link.ld $(ARCH) $(COMMON)

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
