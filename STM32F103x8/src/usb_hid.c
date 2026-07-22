/*
 *  Copyright (C) 2014-2026 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "usb_hid.h"
#include "irmpmain.h"

uint8_t buf[BUFFER_SIZE];
volatile uint8_t USB_HID_Data_Received = 0;
__IO uint8_t PrevXferComplete = 1;
volatile uint8_t suspended = 0;

void USB_HID_Init(void)
{
	Set_System();
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
}

void USB_HID_SendData(uint8_t Report_ID, uint8_t *ptr, uint8_t len)
{
	if (!USB_Ready())
		return;
	if (Report_ID == REPORT_ID_IR)
	{
		/* Windows needs HID_IN_REPORT_COUNT, for linux SIZEOF_IR + 1 is sufficient */
		uint8_t buf[HID_IN_REPORT_COUNT] = {0};
		buf[0] = Report_ID;
		memcpy(&buf[1], ptr, SIZEOF_IR);
		//buf[63] = ;
		//buf[62] = ;
		buf[61] = timeout;
		buf[60] = (keep_same_key && !timeout);
		//buf[59] = ;
		buf[58] = delta >> 8;
		buf[57] = delta & 0xFF;
		buf[56] = INV_F_INT_US;
		//buf[55] = ;
		buf[54] = keep_same_key;
		buf[53] = min_delta >> 8;
		buf[52] = min_delta & 0xFF;
		buf[51] = upper_border >> 8;
		buf[50] = upper_border & 0xFF;
		buf[49] = max_delta >> 8;
		buf[48] = max_delta & 0xFF;
		USB_SIL_Write(EP1_IN, buf, HID_IN_REPORT_COUNT);
	}
	else if (Report_ID == REPORT_ID_CONFIG_IN)
	{
		/* Windows needs HID_IN_REPORT_COUNT, for linux len is sufficient */
		ptr[0] = Report_ID;
		memset(&ptr[len], 0, HID_IN_REPORT_COUNT - len);
		USB_SIL_Write(EP1_IN, ptr, HID_IN_REPORT_COUNT);
	}
	SetEPTxValid(ENDP1);
	PrevXferComplete = 0;
}

uint8_t USB_Ready(void)
{
	return ((bDeviceState == CONFIGURED) && !suspended);
}
