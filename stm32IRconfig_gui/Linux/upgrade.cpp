/*
 * Copyright (C) 2020 Joerg Riechardt
 *
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Originally written by Gareth McMullin <gareth@blacksphere.co.nz>
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

#ifdef WIN32
#   include <stdio.h>
#else
#   include <unistd.h>
#endif
#include "dfu.h"
#include "stm32mem.h"
#include "upgrade.h"
#include <algorithm>

#define LOAD_ADDRESS 0x8002000

int upgrade(const char* firmwarefile, char* print, char* printcollect, FXGUISignal* guisignal);

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

struct libusb_device * find_dev()
{
	struct libusb_device *dev, **devs, *found = NULL;
	int ret;
	ret = libusb_get_device_list(NULL, &devs);
	if(ret < 0) {
		printf("error getting device list(): %s\n", libusb_error_name(ret));
		return NULL;
	}

	for (int i=0; (dev=devs[i]) != NULL; i++) {
		struct libusb_device_descriptor desc;
		if(libusb_get_device_descriptor(dev, &desc) < 0) {
			printf("couldn't get device descriptor\n");
			continue;
		}

		/* Check for vendor ID */
		if (desc.idVendor != 0x1209)
			continue;

		/* Check for product ID */
		if (desc.idProduct == 0x4443) {
			libusb_ref_device(dev);
			found = dev;
			break;
		}
	}

	libusb_free_device_list(devs, 1);
	return found;
}

struct usb_dfu_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bmAttributes;
	uint16_t wDetachTimeout;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;
} __attribute__ ((__packed__)) ;

libusb_device_handle * get_dfu_interface(struct libusb_device *dev, uint16_t *wTransferSize)
{
	int ret;
	struct libusb_interface_descriptor *iface;
	libusb_device_handle *handle = NULL;
	struct libusb_config_descriptor *config;

	if(libusb_get_config_descriptor(dev, 0, &config) != LIBUSB_SUCCESS) {
		printf("couldn't get config descriptor\n");
		return NULL;
	}
	// bNumConfigurations = 1, bNumInterfaces = 1, num_altsetting = 1
	iface = (libusb_interface_descriptor*)&config->interface[0].altsetting[0];
	if((iface->bInterfaceClass == 0xFE) && (iface->bInterfaceSubClass = 1)) { // for safety only
		struct usb_dfu_descriptor *dfu_function = (struct usb_dfu_descriptor*)iface->extra;
		*wTransferSize = dfu_function->wTransferSize;
		ret = libusb_open(dev, &handle);
		if(ret < 0) {
			printf("error opening device: %s\n", libusb_error_name(ret));
			libusb_unref_device(dev);
			goto error;
		}
		ret = libusb_claim_interface(handle, 0);
		if (ret != LIBUSB_SUCCESS) {
			printf("error claiming interface: %s\n", libusb_error_name(ret));
			libusb_close(handle);
			handle = NULL;
			goto error;
		}
	}
error:
	libusb_free_config_descriptor(config);
	return handle;
}

int upgrade(const char* firmwarefile, char* print, char* printcollect, FXGUISignal* guisignal)
{
	struct libusb_device *dev;
	libusb_device_handle *handle;
	int state;
	int offset;
	FILE *fpFirmware;
	int firmwareSize;
	uint8_t *fw_buf;
	uint16_t wTransferSize;
	int ret;

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

	printf("Waiting for device ...\n");
	sprintf(printbuf, "Waiting for device ...\n");
	strcat(print, printbuf);

	ret = libusb_init(NULL);
	if(ret < 0) {
		printf("Error initializing libusb: %s\n", libusb_error_name(ret));
		return -1;
	}
	//libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
	libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_WARNING);

retry:
	if(!(dev = find_dev()) || !(handle = get_dfu_interface(dev, &wTransferSize))) {

#ifdef WIN32
		Sleep(20);
#else
		usleep(20000);
#endif
		goto retry;
	}

	state = dfu_getstate(handle, 0);
	if((state < 0) || (state == STATE_APP_IDLE)) {
		printf("Resetting device in firmware upgrade mode...\n");
		sprintf(print, "Resetting device in firmware upgrade mode...\n");
		guisignal->signal();
		dfu_detach(handle, 0, 1000);
		libusb_release_interface(handle, 0);
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

	dfu_makeidle(handle, 0);

	for(offset = 0; offset < firmwareSize; offset += wTransferSize) {
		if(firmwareSize - offset > wTransferSize)
		    stm32_mem_write(handle, 0, offset/wTransferSize, (void*)&fw_buf[offset], wTransferSize);
		else
		    stm32_mem_write(handle, 0, offset/wTransferSize, (void*)&fw_buf[offset], firmwareSize - offset);
		printf("Progress: %d%%\n", std::min(100, (offset+wTransferSize)*100/firmwareSize));
		fflush(stdout);
		sprintf(print, "Progress: %d%%\n", std::min(100, (offset+wTransferSize)*100/firmwareSize));
		strcat(printcollect, print);
		guisignal->signal();
	}
	
	stm32_mem_manifest(handle, 0);

	libusb_release_interface(handle, 0);
	libusb_close(handle);
	libusb_unref_device(dev);
	libusb_exit(NULL);
	free(fw_buf);
	free(printbuf);

	printf("=== Firmware Upgrade successful! ===\n");
	fflush(stdout);
	sprintf(print, "=== Firmware Upgrade successful! ===\n");
	strcat(printcollect, print);
	guisignal->signal();

	return 1;
}
