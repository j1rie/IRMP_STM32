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

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include "dfu.h"
#include "stm32mem.h"

#define LOAD_ADDRESS 0x8002000

struct libusb_device * find_dev(void)
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

	// bNumConfigurations = 1, bNumInterfaces = 1, num_altsetting = 1
	if(libusb_get_config_descriptor(dev, 0, &config) != LIBUSB_SUCCESS) {
		printf("couldn't get config descriptor\n");
		return NULL;
	}
	iface = (void*)&config->interface[0].altsetting[0];
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

uint8_t * get_firmware(const char *firmwarefile, int *firmwareSize)
{
	FILE *fpFirmware;
	uint8_t *fw_buf;

	fpFirmware = fopen (firmwarefile, "rb");
	if(fpFirmware == NULL) {
		printf("error opening firmware file: %s\n",firmwarefile);
		return NULL;
	} else {
		printf("opened firmware file %s\n", firmwarefile);
	}

	if((fseek(fpFirmware, 0, SEEK_END) != 0) || ((*firmwareSize = ftell(fpFirmware)) < 0) ||
							(fseek(fpFirmware, 0, SEEK_SET) != 0)) {
		printf("error determining firmware size\n");
		fclose(fpFirmware);
		return NULL;
	}

	fw_buf = malloc(*firmwareSize);
	if (fw_buf == NULL) {
		fclose(fpFirmware);
		printf("error allocating memory\n");
		return NULL;
	}

	if(fread(fw_buf,*firmwareSize,1,fpFirmware) != 1) {
		printf("read firmware error\n");
		fclose(fpFirmware);
		free(fw_buf);
		return NULL;
	} else {
		printf("read %d bytes of firmware\n", *firmwareSize);
	}

	fclose(fpFirmware);
	return fw_buf;
}

int main(int argc, const char **argv)
{
	struct libusb_device *dev;
	libusb_device_handle *handle;
	int state;
	int offset;
	int firmwareSize;
	uint8_t *fw_buf;
	uint16_t wTransferSize;
	int ret;

	puts("\n=== STM32 Firmware Upgrade Utility ===\n");

	if(argc != 2){
		puts("Usage: stm32FWupgrade firmware-file.bin\n");
		return 1;
	}

	if(!(fw_buf = get_firmware(argv[1], &firmwareSize)))
		return -1;

	printf("Waiting for device ...\n");

	ret = libusb_init(NULL);
	if(ret < 0) {
		printf("Error initializing libusb: %s\n", libusb_error_name(ret));
		return -1;
	}
	//libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
	//libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_WARNING);

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

	printf("wTransfer Size = %d\n", wTransferSize);
	fflush(stdout);

	dfu_makeidle(handle, 0);

	for(offset = 0; offset < firmwareSize; offset += wTransferSize) {
		if(firmwareSize - offset > wTransferSize) {
			stm32_mem_write(handle, 0, offset/wTransferSize, (void*)&fw_buf[offset], wTransferSize);
			printf("Progress: %d%%\n", (offset+wTransferSize)*100/firmwareSize);
		} else {
			stm32_mem_write(handle, 0, offset/wTransferSize, (void*)&fw_buf[offset], firmwareSize - offset);
			printf("Progress: 100%%\n");
		}
		fflush(stdout);
	}

	stm32_mem_manifest(handle, 0);

	libusb_release_interface(handle, 0);
	libusb_close(handle);
	libusb_unref_device(dev);
	libusb_exit(NULL);
	free(fw_buf);

	printf("\n=== Firmware Upgrade successful! ===\n");
	fflush(stdout);

	return 0;
}
