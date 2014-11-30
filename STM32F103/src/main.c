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

/* uncomment this, if you use a ST-Link */ /* atm you can't use WAKEUP_RESET & ST_Link */
//#define ST_Link

/*
 * only if you want to use CLK and DIO on the blue ST-Link Emulator with mistakenly connected Pins
 * WARNING: further firmware updates will become difficult!
 * better use TMS and TCK instead, and leave this commented out
 */
//#define BlueLink_Remap

/* for use of wakeup reset pin */
#define WAKEUP_RESET

#ifdef BlueLink_Remap
#define OUT_PORT	GPIOA
#define LED_PIN		GPIO_Pin_14
#define WAKEUP_PIN	GPIO_Pin_13
#ifdef WAKEUP_RESET
#define WAKEUP_RESET_PIN	GPIO_Pin_6 /* TODO damit es möglich ist für remap einen anderen zu nehmen ?! */
#endif /* WAKEUP_RESET */
#else
#define OUT_PORT	GPIOB
#define LED_PIN		GPIO_Pin_13
#define WAKEUP_PIN	GPIO_Pin_14
#ifdef WAKEUP_RESET
#ifndef ST_Link
#define WAKEUP_RESET_PIN GPIO_Pin_12 /* TODO IRsnd raus definen für WAKEUP_RESET & ST_Link */
#else
#define WAKEUP_RESET_PIN GPIO_Pin_6
#endif /* ST_Link */
#endif /* WAKEUP_RESET */
#endif /* BlueLink_Remap */

#define SND_MAX 2

__IO uint8_t PrevXferComplete = 1;
uint8_t PA9_state = 0;
uint8_t buf[HID_OUT_BUFFER_SIZE-1];
uint32_t timestamp = 0;
uint32_t AlarmValue = 0xFFFFFFFF;
volatile unsigned int systicks = 0;
#ifdef ST_Link
volatile unsigned int systicks2 = 0;
#endif /* ST_Link */

void delay_ms(unsigned int msec)
{
	systicks = 0;
	while (systicks <= msec);
}

void LED_Switch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef BlueLink_Remap
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	/* disable SWD, so pins are available */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#endif /* BlueLink_Remap */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OUT_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(OUT_PORT, &GPIO_InitStructure);
	/* wakeup reset pin */
#ifdef	WAKEUP_RESET
	GPIO_InitStructure.GPIO_Pin = WAKEUP_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(OUT_PORT, &GPIO_InitStructure);
#endif /* WAKEUP_RESET */
	/* start with LED on */
	GPIO_WriteBit(OUT_PORT, LED_PIN, Bit_SET);
}

#ifdef ST_Link
void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* PA9 (red + yellow LED on ST-Link Emus) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LED_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* PA9 (red + yellow LED) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* red + yellow fast toggle */
void fast_toggle(void)
{
	LED_init();
	systicks2 = 0;
	while (systicks2 <= 500) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(50);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(50);
	}
	/* off */
	LED_deinit();
	PA9_state = 0;
	/* red on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
}

/* red + yellow both on */
void both_on(void)
{
	LED_init();
	systicks2 = 0;
	while (systicks2 <= 500) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(1);
	}
	/* red on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
	/* off */
	LED_deinit();
}

/* yellow on */
void yellow_on(void)
{
	LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
	delay_ms(100);
	/* red on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
	/* off */
	LED_deinit();
}
#else
void LED_init(void) {}
void LED_deinit(void) {}
void fast_toggle(void) {}
void both_on(void) {}
void yellow_on(void) {}
#endif /* ST_Link */

/* toggle red [/ yellow] and external LED */
void toggle_LED(void)
{
#ifdef ST_Link
	if (!PA9_state) {
		LED_init();
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		PA9_state = 1;
	} else {
		LED_deinit();
		PA9_state = 0;
	}
	/*GPIOA->ODR ^= GPIO_Pin_9;*/
#endif /* ST_Link */
	OUT_PORT->ODR ^= LED_PIN;
}

/*
 * eeprom: after writing 0xABCD into virtual address 0x6666, you have CDAB 6666 in flash
 * 2 halfwords in reverse order, little endian
 */

/* buf[0 ... 5] -> eeprom[virt_addr ... virt_addr + 2] */
void eeprom_store(uint8_t *buf, uint8_t virt_addr)
{
	EE_WriteVariable(virt_addr, (buf[1] << 8) | buf[0]);
	EE_WriteVariable(virt_addr + 1, (buf[3] << 8) | buf[2]);
	EE_WriteVariable(virt_addr + 2, (buf[5] << 8) | buf[4]);
}

/* eeprom[virt_addr ... virt_addr + 2] -> buf[0-5] */
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

/*
 * IRData -> buf[0-5]
 * irmplircd expects dummy as first byte (Report ID),
 * so start with buf[0], adapt endianness for irmplircd
 */
void IRData_to_buf(IRMP_DATA *IRData)
{
	buf[0] = IRData->protocol;
	buf[2] = ((IRData->address) >> 8) & 0xFF;
	buf[1] = (IRData->address) & 0xFF;
	buf[4] = ((IRData->command) >> 8) & 0xFF;
	buf[3] = (IRData->command) & 0xFF;
	buf[5] = IRData->flags;
}

/* buf[BufIdx...(BufIdx+5)] -> IRData */
void buf_to_IRData(uint8_t buf[6], uint8_t BufIdx, IRMP_DATA *IRData)
{
	IRData->protocol = buf[BufIdx];
	IRData->address = ((buf[(BufIdx + 1)] << 8) | (buf[(BufIdx + 2)]));
	IRData->command = ((buf[(BufIdx + 3)] << 8) | (buf[(BufIdx + 4)]));
	IRData->flags = buf[(BufIdx + 5)];
}

/* buf[0-5] <-> IRData */
uint8_t cmp_buf_IRData(uint8_t buf[6], IRMP_DATA *IRData)
{
	return	IRData->protocol == buf[0] && \
		IRData->address == ((buf[1] << 8) | buf[2]) && \
		IRData->command == ((buf[3] << 8) | buf[4]) && \
		IRData->flags == buf[5];
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
#ifdef ST_Link
	systicks2++;
#endif /* ST_Link */
	timestamp++;
	if (i == 1000) {
		if (AlarmValue)
			AlarmValue--;
		i = 0;
	} else {
		i++;
	}
}

/* Val -> buf[BufIdx...BufIdx+3] */
void uint32_to_buf(uint32_t Val, uint8_t BufIdx)
{
	buf[BufIdx] = ((Val) >> 24) & 0xFF;
	buf[BufIdx+1] = ((Val) >> 16) & 0xFF;
	buf[BufIdx+2] = ((Val) >> 8) & 0xFF;
	buf[BufIdx+3] = Val & 0xFF;
}

void Wakeup(void)
{
	AlarmValue = 0xFFFFFFFF;
	/* USB wakeup */
	Resume(RESUME_START);
	/* motherboard switch: WAKEUP_PIN short high */
	GPIO_WriteBit(OUT_PORT, WAKEUP_PIN, Bit_SET);
	delay_ms(500);
	GPIO_WriteBit(OUT_PORT, WAKEUP_PIN, Bit_RESET);
	/* both_on(); */
	fast_toggle();
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
		IRData_to_buf(&wakeup_IRData);
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
	IRMP_DATA myIRData, loopIRData, sendIRData, lastIRData = { 0, 0, 0, 0};

#ifdef BlueLink_Remap
	/* wait 5 seconds before disabling SWD, to make firmware update more easy */ /* TODO test this */
	delay_ms(5000);
#endif /* BlueLink_Remap */
	LED_Switch_init();
	LED_init();
	/* red LED on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
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

#ifdef	WAKEUP_RESET
		/* wakeup reset pin pulled low? */
		if (!GPIO_ReadInputDataBit(OUT_PORT, WAKEUP_RESET_PIN)) {
			/* put wakeup IRData into buf and wakeup eeprom */
			store_new_wakeup();
			memcpy(wakeup_buf, buf, sizeof(wakeup_buf));
		}
#endif /* WAKEUP_RESET */

		/* test if USB is connected to PC, sendtransfer is complete and data is received */
		if (USB_HID_GetStatus() == CONFIGURED && PrevXferComplete && USB_HID_ReceiveData(buf) == RX_READY) {

			switch (buf[0]) {
			/* set wakeup IRData */
			case 0xFF:
				/* buf[1-6] -> eeprom[0-2] */
				eeprom_store(&buf[1], 0);
				memset(buf, 0, sizeof(buf));
				/* eeprom[0-2] -> buf[0-5] */
				eeprom_restore(wakeup_buf, 0);
				break;

			/* set trigger_send IRData */
			case 0xFE:
				/* buf[1-6] -> eeprom[3-5] */
				eeprom_store(&buf[1], 3);
				memset(buf, 0, sizeof(buf));
				/* eeprom[3-5] -> buf[0-5] */
				eeprom_restore(trigger_send_buf, 3);
				break;

			/* set send[0] IRData */
			case 0xFD:
				/* buf[1-6] -> eeprom[6-8] */
				eeprom_store(&buf[1], 6);
				memset(buf, 0, sizeof(buf));
				/* eeprom[6-8] -> buf[0-5] */
				eeprom_restore(send_buf[0], 6);
				break;

			/* set send[1] IRData */
			case 0xFC:
				/* buf[1-6] -> eeprom[9-11] */
				eeprom_store(&buf[1], 9);
				memset(buf, 0, sizeof(buf));
				/* eeprom[9-11] -> buf[0-5] */
				eeprom_restore(send_buf[1], 9);
				break;

			/* get wakeup IRData */
			case 0xFB:
				memset(buf, 0, sizeof(buf));
				memcpy(buf, wakeup_buf, sizeof(wakeup_buf));
				break;

			/* get trigger_send IRData */
			case 0xFA:
				memset(buf, 0, sizeof(buf));
				memcpy(buf, trigger_send_buf, sizeof(trigger_send_buf));
				break;

			/* get send[0] IRData */
			case 0xF9:
				memset(buf, 0, sizeof(buf));
				memcpy(buf, send_buf[0], sizeof(send_buf[0]));
				break;

			/* get send[1] IRData */
			case 0xF8:
				memset(buf, 0, sizeof(buf));
				memcpy(buf, send_buf[1], sizeof(send_buf[1]));
				break;

			/* IR send command */
			case 0xF4:
				/* buf[1-6] -> sendIRData */
				buf_to_IRData(buf, 1, &sendIRData);
				/* 0|1: don't|do wait until send finished */
				irsnd_send_data(&sendIRData, 1);
				yellow_on();
				memset(buf, 0, sizeof(buf));
				/* sendIRData -> buf[0-5] */
				IRData_to_buf(&sendIRData);
				/* timestamp -> buf[6-9] */
				uint32_to_buf(timestamp, 6);
				break;

			/* 4 halfwords in reverse order, little endian */
			/* set systick alarm */
			case 0xF3:
				/* buf[1-4] -> AlarmValue */
				AlarmValue = (buf[1]<<24)|(buf[2]<<16)|(buf[3]<<8)|buf[4];
				memset(buf, 0, sizeof(buf));
				/* AlarmValue -> buf[0-5] */
				uint32_to_buf(AlarmValue, 0);
				break;

			/* get systick alarm */
			case 0xF2:
				memset(buf, 0, sizeof(buf));
				/* AlarmValue -> buf[0-5] */
				uint32_to_buf(AlarmValue, 0);
				break;

			default:
				break;
			}

			/* send (modified) data (for verify) */
			USB_HID_SendData(0x02, buf, 10);
			toggle_LED();
		}

		/* poll IR-data */
		if (irmp_get_data(&myIRData)) {
			/* new IR-Data? */
			/* omit flags */
			if (!(	myIRData.protocol == lastIRData.protocol && \
				myIRData.address == lastIRData.address && \
				myIRData.command == lastIRData.command )) {

				toggle_LED();
				lastIRData.protocol = myIRData.protocol;
				lastIRData.address = myIRData.address;
				lastIRData.command = myIRData.command;
			}

			/* wakeup IR-data? */
			if (cmp_buf_IRData(wakeup_buf, &myIRData))
				Wakeup();

			/* trigger send IR-data? */
			if (cmp_buf_IRData(trigger_send_buf, &myIRData)) {
				for (k=0; k < SND_MAX; k++) {
					/* ?? 100 too small, 125 ok, RC5 is 114ms */
					delay_ms(115);
					/* send_buf[k] -> sendIRData */
					buf_to_IRData(send_buf[k], 0, &sendIRData);
					/* 0|1: don't|do wait until send finished */
					irsnd_send_data(&sendIRData, 1);
					yellow_on();
					/* ?? */
					delay_ms(300);
					/* receive sent by myself too, TODO */
					if (irmp_get_data(&loopIRData)) {
						memset(buf, 0, sizeof(buf));
						/* loopIRData -> buf[0-5] */
						IRData_to_buf(&loopIRData);
						/* timestamp -> buf[6-9] */
						uint32_to_buf(timestamp, 6);
						USB_HID_SendData(0x01, buf,10);
					}
				}
			}

			/* send IR-data via USB-HID */
			memset(buf, 0, sizeof(buf));
			/* myIRData -> buf[0-5] */
			IRData_to_buf(&myIRData);
			/* timestamp -> buf[6-9] */
			uint32_to_buf(timestamp, 6);
			USB_HID_SendData(0x01, buf, 10);
		}
	}
}
