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

#define LOAD_ADDRESS 0x8002000

int upgrade(const char* firmwarefile, char* print, FXGUISignal* guisignal);

Upgrade::Upgrade(const char* pfirmwarefile, char* pprint, FXGUISignal* pguisignal)
: firmwarefile{pfirmwarefile}, print{pprint}, guisignal{pguisignal} {}

Upgrade::Upgrade() {}

void Upgrade::set_firmwarefile(const char* pfirmwarefile) {
	firmwarefile = pfirmwarefile;
}

void Upgrade::set_print(char* pprint) {
	print = pprint;
}

void Upgrade::set_signal(FXGUISignal* pguisignal) {
	guisignal = pguisignal;
}

FXint Upgrade::run() {
	upgrade(firmwarefile, print, guisignal);
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

int upgrade(const char* firmwarefile, char* print, FXGUISignal* guisignal)
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

	fpFirmware = fopen (firmwarefile, "rb");
	if(fpFirmware == NULL) {
		sprintf(print, "error opening firmware file: %s\n",firmwarefile);
		guisignal->signal();
		return 0;
	} else {
		sprintf(print, "opened firmware file %s\n", firmwarefile);
		guisignal->signal();
	}

	if((fseek(fpFirmware, 0, SEEK_END) != 0) ||
			((firmwareSize = ftell(fpFirmware)) < 0) ||
			(fseek(fpFirmware, 0, SEEK_SET) != 0)) {
		sprintf(print, "error determining firmware size\n");
		guisignal->signal();
		return 0;
	}

	fw_buf = (uint8_t*)malloc(firmwareSize);
	if (fw_buf == NULL) {
		fclose(fpFirmware);
		sprintf(print, "error allocating memory\n");
		guisignal->signal();
		return 0;
	}

	if(fread(fw_buf,firmwareSize,1,fpFirmware) != 1) {
		sprintf(print, "read firmware error\n");
		guisignal->signal();
	} else {
		sprintf(print, "read %d bytes of firmware\n", firmwareSize);
		guisignal->signal();
	}

	fclose(fpFirmware);

	libusb_init(NULL);

	sprintf(print, "Waiting for device ...\n");
	guisignal->signal();

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

	sprintf(print, "Found device at %d:%d\n", libusb_get_bus_number(dev), libusb_get_device_address(dev));
	guisignal->signal();

	sprintf(print, "wTransfer Size = %d\n", wTransferSize);
	guisignal->signal();

	dfu_makeidle(handle, iface);

	for(offset = 0; offset < firmwareSize; offset += wTransferSize) {
		sprintf(print, "Progress: %d%%\n", (offset*100)/firmwareSize);
		guisignal->signal();
		fflush(stdout);
		if(firmwareSize - offset > wTransferSize)
		    stm32_mem_write(handle, iface, offset/wTransferSize, (void*)&fw_buf[offset], wTransferSize);
		else
		    stm32_mem_write(handle, iface, offset/wTransferSize, (void*)&fw_buf[offset], firmwareSize - offset);
	}

	sprintf(print, "Progress: 100%%\n");
	guisignal->signal();
	
	stm32_mem_manifest(handle, iface);

	libusb_release_interface(handle, iface);
	libusb_close(handle);
	free(fw_buf);

	sprintf(print, "=== Firmware Upgrade successful! ===\n");
	guisignal->signal();

	return 1;
}
