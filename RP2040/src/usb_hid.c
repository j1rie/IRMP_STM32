/*
 *  Copyright (C) 2014-2023 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "usb_hid.h"

uint8_t *bufptr; // maybe copying the data into buf would be better? but using this pointer works fine
//uint8_t buf[BUFFER_SIZE]; // for configuration data

volatile uint8_t USB_HID_Data_Received = 0;

void USB_HID_SendData(uint8_t Report_ID, uint8_t *ptr, uint8_t len)
{
	if (!tud_ready())
		return;
	if (Report_ID == REPORT_ID_IR)
	{
		/* Windows needs HID_IN_REPORT_COUNT, for linux SIZEOF_IR + 1 is sufficient */
		uint8_t buf[HID_IN_REPORT_COUNT - 1] = {0};
		memcpy(buf, ptr, SIZEOF_IR);
		tud_hid_report(Report_ID, buf, HID_IN_REPORT_COUNT - 1);

	}
	else if (Report_ID == REPORT_ID_CONFIG_IN)
	{
		/* Windows needs HID_IN_REPORT_COUNT, for linux len is sufficient */
		memset(&ptr[len], 0, HID_IN_REPORT_COUNT - len);
		tud_hid_report(Report_ID, ptr + 1, HID_IN_REPORT_COUNT - 1);
	}
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) // TODO: is buffer always the same?!
{
  // This example doesn't use multiple report and report ID
  (void) itf;
  (void) report_id;
  (void) report_type;

  bufptr = (uint8_t *)buffer;
  //memcpy(buf, buffer, sizeof(buf));

  USB_HID_Data_Received = 1;
}
