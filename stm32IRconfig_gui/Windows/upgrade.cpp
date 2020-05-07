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
#include <stdlib.h>
#include <string.h>
#include "dfu.h"
#include "stm32mem.h"
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "upgrade.h"
#include <algorithm>

#define LOAD_ADDRESS 0x8002000

int upgrade(const char* firmwarefile, char* print, char* pprintcollect, FXGUISignal* guisignal);

Upgrade::Upgrade(const char* pfirmwarefile, char* pprint, char* pprintcollect, FXGUISignal* pguisignal)
: firmwarefile{pfirmwarefile}, print{pprint}, printcollect{pprintcollect}, guisignal{pguisignal} {}

Upgrade::Upgrade() {}

void Upgrade::set_firmwarefile(const char* pfirmwarefile) {
	firmwarefile = pfirmwarefile;
}

void Upgrade::set_print(char* pprint) {
	print = pprint;
}

void Upgrade::set_printcollect(char* pprintcollect) {
	printcollect = pprintcollect;
}

void Upgrade::set_signal(FXGUISignal* pguisignal) {
	guisignal = pguisignal;
}

FXint Upgrade::run() {
	upgrade(firmwarefile, print, printcollect, guisignal);
  return 0;
}

Upgrade::~Upgrade(){}

struct libusb_device * find_dev(void)
{
	struct libusb_device *dev, **devs;
	libusb_get_device_list(NULL, &devs);
	for (int i=0; (dev=devs[i]) != NULL; i++) {
		struct libusb_device_descriptor desc;
		libusb_get_device_descriptor(dev, &desc);
		/* Check for vendor ID */
			if (desc.idVendor != 0x1209)
				continue;
			/* Check for product ID */
			if (desc.idProduct == 0x4443)
				return dev;
	}
	return NULL;
}

struct usb_dfu_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bmAttributes;
	uint16_t wDetachTimeout;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;
} __attribute__ ((__packed__)) ;

libusb_device_handle * get_dfu_interface(struct libusb_device *dev, uint16_t *interface, uint16_t *wTransferSize)
{
	int i, j, k;
	struct libusb_device_descriptor desc;
	struct libusb_interface_descriptor *iface;
	libusb_device_handle *handle;
	libusb_get_device_descriptor(dev, &desc);
	for(i = 0; i < desc.bNumConfigurations; i++) {
		struct libusb_config_descriptor *config;
		libusb_get_config_descriptor(dev, i, &config);
		for(j = 0; j < config->bNumInterfaces; j++) {
			for(k = 0; k < config->interface[j].num_altsetting; k++) {
				iface = (libusb_interface_descriptor*)&config->interface[j].altsetting[k];
				if((iface->bInterfaceClass == 0xFE) &&
				   (iface->bInterfaceSubClass = 0x01)) {
					libusb_open(dev, &handle); //
					struct usb_dfu_descriptor *dfu_function = (struct usb_dfu_descriptor*)iface->extra;
					*wTransferSize = dfu_function->wTransferSize;
					libusb_claim_interface(handle, j);
					*interface = iface->bInterfaceNumber;
					return handle;
				}
			}
		}
	}
	return NULL;
}

int upgrade(const char* firmwarefile, char* print, char* printcollect, FXGUISignal* guisignal)
{
	struct libusb_device *dev;
	libusb_device_handle *handle;
	uint16_t iface;
	int state;
	int offset;
	FILE *fpFirmware;
  	int firmwareSize;
	uint8_t *fw_buf;
	uint16_t wTransferSize;

	char* printbuf;
	printbuf = (char*)malloc(80);
	if (printbuf == NULL) {
		printf("printbuf: error allocating memory\n");
		return 0;
	}

	fpFirmware = fopen (firmwarefile, "rb");
	if(fpFirmware == NULL) {
		printf("error opening firmware file: %s\n",firmwarefile);
		sprintf(print, "error opening firmware file: %s\n",firmwarefile);
		guisignal->signal();
		return 0;
	} else {
		printf("opened firmware file %s\n", firmwarefile);
		sprintf(print, "opened firmware file %s\n", firmwarefile);
	}

	if((fseek(fpFirmware, 0, SEEK_END) != 0) ||
			((firmwareSize = ftell(fpFirmware)) < 0) ||
			(fseek(fpFirmware, 0, SEEK_SET) != 0)) {
		printf("error determining firmware size\n");
		sprintf(printbuf, "error determining firmware size\n");
		strcat(print, printbuf);
		guisignal->signal();
		return 0;
	}

	fw_buf = (uint8_t*)malloc(firmwareSize);
	if (fw_buf == NULL) {
		fclose(fpFirmware);
		printf("error allocating memory\n");
		sprintf(printbuf, "error allocating memory\n");
		strcat(print, printbuf);
		guisignal->signal();
		return 0;
	}

	if(fread(fw_buf,firmwareSize,1,fpFirmware) != 1) {
		printf("read firmware error\n");
		sprintf(printbuf, "read firmware error\n");
		strcat(print, printbuf);
		guisignal->signal();
	} else {
		printf("read %d bytes of firmware\n", firmwareSize);
		sprintf(printbuf, "read %d bytes of firmware\n", firmwareSize);
		strcat(print, printbuf);
	}

	fclose(fpFirmware);

	libusb_init(NULL);

	printf("Waiting for device ...\n");
	sprintf(printbuf, "Waiting for device ...\n");
	strcat(print, printbuf);

retry:
	if(!(dev = find_dev()) || !(handle = get_dfu_interface(dev, &iface, &wTransferSize))) {

#ifdef WIN32
		Sleep(3);
#else
		usleep(3000);
#endif
		goto retry;
	}

	state = dfu_getstate(handle, iface);
	if((state < 0) || (state == STATE_APP_IDLE)) {
		printf("Resetting device in firmware upgrade mode...\n");
		sprintf(print, "Resetting device in firmware upgrade mode...\n");
		guisignal->signal();
		dfu_detach(handle, iface, 1000);
		libusb_release_interface(handle, iface);
		libusb_close(handle);
#ifdef WIN32
		Sleep(5000);
#else
		sleep(5);
#endif
		goto retry;
	}

	printf("Found device at %d:%d\n", libusb_get_bus_number(dev), libusb_get_device_address(dev));
	sprintf(printbuf, "Found device at %d:%d\n", libusb_get_bus_number(dev), libusb_get_device_address(dev));
	strcat(print, printbuf);

	printf("wTransfer Size = %d\n", wTransferSize);
	fflush(stdout);
	sprintf(printbuf, "wTransfer Size = %d\n", wTransferSize);
	strcat(print, printbuf);
	strcat(printcollect, print);
	guisignal->signal();

	dfu_makeidle(handle, iface);

	for(offset = 0; offset < firmwareSize; offset += wTransferSize) {

		if(firmwareSize - offset > wTransferSize)
		    stm32_mem_write(handle, iface, offset/wTransferSize, (void*)&fw_buf[offset], wTransferSize);
		else
		    stm32_mem_write(handle, iface, offset/wTransferSize, (void*)&fw_buf[offset], firmwareSize - offset);
		printf("Progress: %d%%\n", (std::max(100,(offset+wTransferSize)*100)/firmwareSize));
		fflush(stdout);
		sprintf(print, "Progress: %d%%\n", (std::max(100,(offset+wTransferSize)*100)/firmwareSize));
		strcat(printcollect, print);
		guisignal->signal();
	}
	
	stm32_mem_manifest(handle, iface);

	libusb_release_interface(handle, iface);
	libusb_close(handle);
	free(fw_buf);

	printf("=== Firmware Upgrade successful! ===\n");
	fflush(stdout);
	sprintf(print, "=== Firmware Upgrade successful! ===\n");
	strcat(printcollect, print);
	guisignal->signal();

	return 1;
}
