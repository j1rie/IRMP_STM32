/*
 *  Copyright (C) 2014-2023 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef __USB_HID_H
#define __USB_HID_H

#include <string.h>
#include "stdint.h"
#include "tusb.h"
#include "config.h"

#define HID_IN_REPORT_COUNT	64 /* STM32->PC */
#define HID_OUT_REPORT_COUNT	64 /* PC->STM32 */
#define BUFFER_SIZE (HID_IN_REPORT_COUNT < HID_OUT_REPORT_COUNT ? HID_OUT_REPORT_COUNT : HID_IN_REPORT_COUNT)

#define HID_IN_INTERVAL		1
#define HID_OUT_INTERVAL	1

#define REPORT_ID_IR		0x01
#define REPORT_ID_CONFIG_IN	0x02
#define REPORT_ID_CONFIG_OUT	0x03

//extern uint8_t buf[BUFFER_SIZE];
extern uint8_t *bufptr;
extern volatile uint8_t USB_HID_Data_Received;
extern volatile uint8_t PrevXferComplete;

extern void USB_HID_SendData(uint8_t Report_ID, uint8_t *ptr, uint8_t len);
extern void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize);
extern uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen);

#endif /* __USB_HID_H */
