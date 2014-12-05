/*
 *  IR receiver, sender, USB wakeup, motherboard switch wakeup, wakeup timer,
 *  USB HID device, eeprom emulation
 *
 *  Copyright (C) 2014 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "stm32f10x.h"
#include "usb_hid.h"
#include "irmpmain.h"
#include "irsndmain.h"
#include "eeprom.h"

#define SND_MAX 2

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
USB_HID_RXSTATUS_t check=RX_EMPTY;
uint8_t buf[HID_OUT_BUFFER_SIZE-1];
uint32_t AlarmValue = 0xFFFFFFFF;
volatile unsigned int systicks = 0;

void delay_ms(unsigned int msec)
{
	systicks = 0;
	while (systicks <= msec);
}

void LED_Switch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* PB12 (blue LED) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Config PB7 (mb switch pin) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* wakeup reset pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* start with LED on */
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
}

void toggle_LED(void)
{
	GPIOB->ODR ^= GPIO_Pin_12;
}

/* buf[0 ... 5] -> eeprom[virt_addr ... virt_addr + 2] */
/* buffer: 012345 -> arguments for Write: (10)(32)(54) -> eeprom: 01,23,45 */
void eeprom_store(uint8_t *buf, uint8_t virt_addr)
{
	EE_WriteVariable(virt_addr, (buf[1] << 8) | buf[0]);
	EE_WriteVariable(virt_addr + 1, (buf[3] << 8) | buf[2]);
	EE_WriteVariable(virt_addr + 2, (buf[5] << 8) | buf[4]);
}

/* eeprom[virt_addr ... virt_addr + 2] -> buf[0-5] */
/* eeprom: 01,23,45 -> Read results: (10)(32)(54) -> memcpy 01|23|45 -> buffer: 012345 */
void eeprom_restore(uint8_t *buf, uint8_t virt_addr)
{
	uint8_t i;
	uint16_t EE_Data;

	for(i=0; i<3; i++) {
		if (EE_ReadVariable(virt_addr + i, &EE_Data)) {
			/* the variable was not found or no valid page was found */
			EE_Data = 0;
			/* TODO: notify about an error */
		}
		memcpy(&buf[2*i], &EE_Data, 2);
	}
}

void Systick_Init(void)
{
	/* 1ms */
	SysTick_Config((SysCtlClockGet()/1000));
}

void SysTick_Handler(void)
{
	static uint16_t i = 0;
	systicks++;
	if (i == 1000) {
		if (AlarmValue)
			AlarmValue--;
		i = 0;
	} else {
		i++;
	}
}

void Wakeup(void)
{
	AlarmValue = 0xFFFFFFFF;
	/* USB wakeup */
	USB_OTG_ActiveRemoteWakeup(&USB_OTG_dev);
	/* motherboard switch: PB7 short high */
	GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
	delay_ms(500);
	GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
}

/* put wakeup IRData into buf and wakeup eeprom */
void store_new_wakeup(void)
{
	IRMP_DATA wakeup_IRData;
	toggle_LED();
	systicks = 0;
	/* 5 seconds to press button on remote */
	delay_ms(5000);
	if (irmp_get_data(&wakeup_IRData)) {
		/* wakeup_IRData -> buf[0-5] */
		memcpy(buf, &wakeup_IRData, sizeof(wakeup_IRData)-1);
		/* set flags to 0 */
		buf[5] = 0;
		/* buf[0-5] -> eeprom[0-2] */
		eeprom_store(buf, 0);
		toggle_LED();
	}
}

int main(void)
{
	uint8_t k, wakeup_buf[6], trigger_send_buf[6], send_buf[SND_MAX][6];
	IRMP_DATA myIRData, sendIRData, lastIRData = { 0, 0, 0, 0};

	LED_Switch_init();
	USB_HID_Init();
	IRMP_Init();
	IRSND_Init();
	FLASH_Unlock();
	EE_Init();
	Systick_Init();

	/* read wakeup IR-data from eeprom: eeprom[0-2] -> wakeup_buf */
	eeprom_restore(wakeup_buf, 0);

	/* read trigger IR-data from eeprom: eeprom[3-5] -> trigger_send_buf */
	eeprom_restore(trigger_send_buf, 3);

	/* read IR-data to send from eeprom: eeprom[6-8] -> send_buf[0], eeprom[9-11] -> send_buf[1], etc */
	for (k = 0; k < SND_MAX; k++) {
		eeprom_restore(send_buf[k], 6 + k * 3);
	}

	while (1) {
		if (!AlarmValue)
			Wakeup();

		/* wakeup reset pin pulled low? */
		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) {
			/* put wakeup IRData into buf and wakeup eeprom */
			store_new_wakeup();
			memcpy(wakeup_buf, buf, sizeof(wakeup_buf));
		}

		/* test if USB is connected to PC and data is received */
		if (USB_HID_GetStatus() == USB_HID_CONNECTED && USB_HID_ReceiveData(buf) == RX_READY) {

			switch (buf[0]) {
			/* set wakeup IRData */
			case 0xFF:
				/* buf[1-6] -> eeprom[0-2] */
				eeprom_store(&buf[1], 0);
				eeprom_restore(wakeup_buf, 0);
				break;

			/* set trigger_send IRData */
			case 0xFE:
				/* buf[1-6] -> eeprom[3-5] */
				eeprom_store(&buf[1], 3);
				eeprom_restore(trigger_send_buf, 3);
				break;

			/* set send[0] IRData */
			case 0xFD:
				/* buf[1-6] -> eeprom[6-8] */
				eeprom_store(&buf[1], 6);
				eeprom_restore(send_buf[0], 6);
				break;

			/* set send[1] IRData */
			case 0xFC:
				/* buf[1-6] -> eeprom[9-11] */
				eeprom_store(&buf[1], 9);
				eeprom_restore(send_buf[1], 9);
				break;

			/* get wakeup IRData */
			case 0xFB:
				memcpy(buf, wakeup_buf, sizeof(wakeup_buf));
				break;

			/* get trigger_send IRData */
			case 0xFA:
				memcpy(buf, trigger_send_buf, sizeof(trigger_send_buf));
				break;

			/* get send[0] IRData */
			case 0xF9:
				memcpy(buf, send_buf[0], sizeof(send_buf[0]));
				break;

			/* get send[1] IRData */
			case 0xF8:
				memcpy(buf, send_buf[1], sizeof(send_buf[1]));
				break;

			/* IR send command */
			case 0xF4:
				/* buf[1-6] -> sendIRData */
				memcpy(&sendIRData, &buf[1], sizeof(sendIRData));
				/* 0|1: don't|do wait until send finished */
				irsnd_send_data(&sendIRData, 1);
				break;

			/* set systick alarm */
			case 0xF3:
				/* buf[1-4] -> AlarmValue */
				memcpy(&AlarmValue, &buf[1], sizeof(AlarmValue));
				break;

			/* get systick alarm */
			case 0xF2:
				/* AlarmValue -> buf[0-3] */
				memcpy(buf, &AlarmValue, sizeof(AlarmValue));
				break;

			default:
				break;
			}

			/* send data */
			USB_HID_SendData(0x02, buf, 6);
			toggle_LED();
		}

		/* poll IR-data */
		if (irmp_get_data(&myIRData)) {
			/* new IR-Data? */
			/* omit flags */
			if (memcmp(&myIRData, &lastIRData, sizeof(myIRData)-1)) {
				toggle_LED();
				memcpy(&lastIRData, &myIRData, sizeof(myIRData)-1);
			}

			/* wakeup IR-data? */
			if (!memcmp(wakeup_buf, &myIRData, sizeof(myIRData)))
				Wakeup();

			/* trigger send IR-data? */
			if (!memcmp(trigger_send_buf, &myIRData, sizeof(myIRData))) {
				for (k=0; k < SND_MAX; k++) {
					/* ?? 100 too small, 125 ok, RC5 is 114ms */
					delay_ms(115);
					/* send_buf[k] -> sendIRData */
					memcpy(send_buf[k], &sendIRData, sizeof(sendIRData));
					/* 0|1: don't|do wait until send finished */
					irsnd_send_data(&sendIRData, 1);
					/* ?? */
					delay_ms(300);
				}
			}

			/* send IR-data via USB-HID */
			/* myIRData -> buf[0-5] */
			memcpy(buf, &myIRData, sizeof(myIRData));
			USB_HID_SendData(0x01, buf, 6);
		}
	}
}
