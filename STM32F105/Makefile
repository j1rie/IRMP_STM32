TARGET ?= F105
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
DEFINES = -D$(PLATFORM) -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER

OBJS = src/irmpmain.o src/st_link_leds.o src/usb_hid.o src/main.o\
       irmp/irmp.o irmp/irsnd.o\
       cmsis_boot/startup/startup_stm32f10x_cl.o cmsis_boot/system_stm32f10x.o\
       stm_lib/src/misc.o\
       stm_lib/src/stm32f10x_flash.o stm_lib/src/stm32f10x_gpio.o\
       stm_lib/src/stm32f10x_rcc.o\
       stm_lib/src/stm32f10x_tim.o stm_lib/src/stm32f10x_usart.o\
       stm_lib/src/eeprom.o\
       usb_hid/src/usb_bsp.o usb_hid/src/usb_core.o usb_hid/src/usb_dcd.o\
       usb_hid/src/usb_dcd_int.o usb_hid/src/usbd_core.o\
       usb_hid/src/usbd_desc.o\
       usb_hid/src/usbd_hid_core.o usb_hid/src/usbd_ioreq.o\
       usb_hid/src/usbd_req.o usb_hid/src/usbd_usr.o\

CFLAGS = -Wall -ffunction-sections -fno-builtin $(ARCH) $(COMMON) $(INCLUDES) $(DEFINES)

DATE := $(shell date +"%F_%H-%M")
TARGET := $(DATE)_$(TARGET)

IRMP_VERSION := $(shell grep -e "Version IRMP" irmp/README.txt | awk '{print $$3}')
DEFINES += -DFW_STR="\"$(TARGET)   IRMP-Version:_$(IRMP_VERSION)\""

ifdef Bootloader
DEFINES += -DBootloader
TARGET := $(TARGET)_BL
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link-bootloader.ld $(ARCH) $(COMMON)
else
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--gc-sections,--entry=main,-Tscripts/arm-gcc-link.ld $(ARCH) $(COMMON)
endif
LDFLAGS += --specs=nano.specs

ifdef SimpleCircuit_F105
DEFINES += -DSimpleCircuit_F105
TARGET := $(TARGET)_SC
endif

COMPILED_BY = jrie
TARGET := $(TARGET)_$(COMPILED_BY)

$(TARGET).bin: $(TARGET).elf
	$(OBJCP) -O binary $< $@

$(TARGET).elf: prepare $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

.PHONY: prepare flash clean-objects clean distclean

prepare:
	@./scripts/prepare.sh

flash: $(TARGET).bin
	stm32flash -v -w $(TARGET).bin /dev/ttyUSB0

clean-objects:
	rm -f $(OBJS) $(TARGET).{elf,map}

clean: clean-objects
	rm -rf cmsis cmsis_boot irmp stm_lib usb_hid ext_src/prepared

distclean: clean
	rm -rf ext_src
