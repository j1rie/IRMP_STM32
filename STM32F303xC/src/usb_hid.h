/*
 *  Copyright (C) 2014-2022 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef __USB_HID_H
#define __USB_HID_H

#include "stm32f30x.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include <string.h>

#define HID_IN_REPORT_COUNT	17 /* STM32->PC */
#define HID_OUT_REPORT_COUNT	17 /* PC->STM32 */
#define BUFFER_SIZE (HID_IN_REPORT_COUNT < HID_OUT_REPORT_COUNT ? HID_OUT_REPORT_COUNT : HID_IN_REPORT_COUNT)

#define HID_IN_INTERVAL		1
#define HID_OUT_INTERVAL	1

#define REPORT_ID_IR		0x01
#define REPORT_ID_CONFIG_IN	0x02
#define REPORT_ID_CONFIG_OUT	0x03

extern uint8_t buf[BUFFER_SIZE];
extern volatile uint8_t USB_HID_Data_Received;
extern __IO uint8_t PrevXferComplete;

void USB_HID_Init(void);
extern void USB_HID_SendData(uint8_t Report_ID, uint8_t *ptr, uint8_t len);

#endif /* __USB_HID_H */
