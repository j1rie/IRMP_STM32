/*
 *  Copyright (C) 2014 Joerg Riechardt
 *
 *  based on work by Uwe Becker - http://mikrocontroller.bplaced.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#include "usb_hid.h"

USB_OTG_CORE_HANDLE  USB_OTG_dev;
uint8_t USB_HID_OUT_BUF[HID_OUT_BUFFER_SIZE];  /* PC->STM32 */
uint8_t USB_HID_IN_BUF[HID_IN_BUFFER_SIZE];    /* STM32->PC */


void USB_HID_Init(void)
{
	USB_HID_STATUS=USB_HID_DETACHED;
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_HID_cb, &USR_cb);
}


USB_HID_STATUS_t USB_HID_GetStatus(void)
{
	return(USB_HID_STATUS);
}


/* len: bytes to send (1...HID_IN_BUFFER_SIZE) */
ErrorStatus USB_HID_SendData(uint8_t Report_ID, uint8_t *ptr, uint8_t len)
{
	uint8_t n;

	if(USB_HID_STATUS != USB_HID_CONNECTED) {
		return(ERROR);
	}

	/* Report ID */
	USB_HID_IN_BUF[0] = Report_ID;

	for(n = 1; n < HID_IN_BUFFER_SIZE; n++) {
		if(n <= len) {
			/* after report ID */
			USB_HID_IN_BUF[n] = *ptr;
			ptr++;
		}
		else {
			USB_HID_IN_BUF[n] = 0x00;
		}
	}

	if(USBD_HID_SendReport(&USB_OTG_dev, USB_HID_IN_BUF, HID_IN_BUFFER_SIZE)!=USBD_OK) {
		return(ERROR);
	}

	return(SUCCESS);
}

USB_HID_RXSTATUS_t USB_HID_ReceiveData(uint8_t *ptr)
{
	uint16_t check,n;

	if(USB_HID_STATUS != USB_HID_CONNECTED) {
		return(RX_USB_ERR);
	}

	check = USBD_HID_RecReport();
	if(check == 0) {
		ptr[0] = 0x00;
		return(RX_EMPTY);
	}

	/* USB_HID_OUT_BUF[0] is Report ID */
	for(n = 1; n < HID_OUT_BUFFER_SIZE; n++) {
		if(n <= check) {
			ptr[n-1] = USB_HID_OUT_BUF[n];
		}
		else {
			ptr[n-1] = 0x00;
		}
	}

	return(RX_READY);
}
