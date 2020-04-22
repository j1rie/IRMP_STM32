/*
 * Copyright (C) 2020 Joerg Riechardt
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#   include <C:/msys64/mingw64/include/libusb-compat/usb.h>
#else
#   include <usb.h>
#endif

#include "dfu.h"
#include "stm32mem.h"
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define LOAD_ADDRESS 0x8002000

struct usb_device * find_dev(void)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	struct usb_dev_handle *handle;
	char man[40];
	char prod[40];

	usb_find_busses();
	usb_find_devices();

	for(bus = usb_get_busses(); bus; bus = bus->next) {
		for(dev = bus->devices; dev; dev = dev->next) {
			/* Check for vendor ID */
			if (dev->descriptor.idVendor != 0x1209)
				continue;

			handle = usb_open(dev);
			usb_get_string_simple(handle, dev->descriptor.iManufacturer, man,
						sizeof(man));
			usb_get_string_simple(handle, dev->descriptor.iProduct, prod,
						sizeof(prod));
#if 0
			printf("%s:%s [%04X:%04X] %s : %s\n", bus->dirname, dev->filename,
				dev->descriptor.idVendor, dev->descriptor.idProduct, man, prod);
#endif
			usb_close(handle);

			/* Check for product ID */
			if ((dev->descriptor.idProduct == 0x4443) &&
			   !strcmp(prod, "STM32 Bootloader"))
				return dev;
		}
	}
	return NULL;
}

usb_dev_handle * get_dfu_interface(struct usb_device *dev, uint16_t *interface)
{
	int i, j, k;
	struct usb_config_descriptor *config;
	struct usb_interface_descriptor *iface;

	usb_dev_handle *handle;

	for(i = 0; i < dev->descriptor.bNumConfigurations; i++) {
		config = &dev->config[i];

		for(j = 0; j < config->bNumInterfaces; j++) {
			for(k = 0; k < config->interface[j].num_altsetting; k++) {
				iface = &config->interface[j].altsetting[k];
				if((iface->bInterfaceClass == 0xFE) &&
				   (iface->bInterfaceSubClass = 0x01)) {
					handle = usb_open(dev);
					//usb_set_configuration(handle, i);
					usb_claim_interface(handle, j);
					//usb_set_altinterface(handle, k);
					//*interface = j;
					*interface = iface->bInterfaceNumber;
					return handle;
				}
			}
		}
	}
	return NULL;
}

int upgrade(const char* firmwarefile, int TransferSize)
{
	struct usb_device *dev;
	usb_dev_handle *handle;
	uint16_t iface;
	int state;
	int offset;
	FILE *fpFirmware;
  	int firmwareSize;
	uint8_t *fw_buf;

    fpFirmware = fopen (firmwarefile, "rb");
    if(fpFirmware == NULL) {
        printf("error opening firmware file: %s\n",firmwarefile);
        return 0;
    } else {
	  printf("opened firmware file %s\n", firmwarefile);
    }

    if((fseek(fpFirmware, 0, SEEK_END) != 0) ||
	((firmwareSize = ftell(fpFirmware)) < 0) ||
	(fseek(fpFirmware, 0, SEEK_SET) != 0)) {
	    printf("error determining firmware size\n");
	    return 0;
    }

//    uint8_t fw_buf[firmwareSize];

    fw_buf = malloc(firmwareSize);
    if (fw_buf == NULL) {
	  fclose(fpFirmware);
	  printf("error allocating memory\n");
	  return 0;
    }


    if(fread(fw_buf,firmwareSize,1,fpFirmware) != 1) {
	  puts("read firmware error\n");
    } else {
	  printf("read %d bytes of firmware\n", firmwareSize);
    }

    fclose(fpFirmware);

	usb_init();

retry:
	if(!(dev = find_dev()) || !(handle = get_dfu_interface(dev, &iface))) {
		printf("\rDevice not yet found ...");

#ifdef WIN32
		sleep(3);
#else
		usleep(3000);
#endif
		goto retry;
	}

	state = dfu_getstate(handle, iface);
	if((state < 0) || (state == STATE_APP_IDLE)) {
		puts("Resetting device in firmware upgrade mode...");
		dfu_detach(handle, iface, 1000);
		usb_release_interface(handle, iface);
		usb_close(handle);
#ifdef WIN32
		Sleep(5000);
#else
		sleep(5);
#endif
		goto retry;
	}
	printf("\nDevice found at %s:%s\n", dev->bus->dirname, dev->filename);
	printf("Transfer Size:%d\n", TransferSize);

	dfu_makeidle(handle, iface);

	for(offset = 0; offset < firmwareSize; offset += TransferSize) {
		printf("Progress: %d%%\n", (offset*100)/firmwareSize);
		fflush(stdout);
		if(firmwareSize - offset > TransferSize)
		    stm32_mem_write(handle, iface, offset/TransferSize, (void*)&fw_buf[offset], TransferSize);
		else
		    stm32_mem_write(handle, iface, offset/TransferSize, (void*)&fw_buf[offset], firmwareSize - offset);
	}
	puts("Progress: 100%");
	
	stm32_mem_manifest(handle, iface);

	usb_release_interface(handle, iface);
	usb_close(handle);
	free(fw_buf);

	printf("=== Firmware Upgrade successful! ===\n");

	return 1;
}
