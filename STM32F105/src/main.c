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

#define BYTES_PER_QUERY	(HID_IN_BUFFER_SIZE - 4)

enum __attribute__ ((__packed__)) access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum __attribute__ ((__packed__)) command {
	CMD_EMIT,
	CMD_CAPS,
	CMD_FW,
	CMD_ALARM,
	CMD_MACRO,
	CMD_WAKE
};

enum __attribute__ ((__packed__)) status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

/* keep in sync with ir{mp,snd}config.h */
const char supported_protocols[] = {
IRMP_SIRCS_PROTOCOL,
IRMP_NEC_PROTOCOL,
IRMP_SAMSUNG_PROTOCOL,
IRMP_KASEIKYO_PROTOCOL,
IRMP_JVC_PROTOCOL,
IRMP_NEC16_PROTOCOL,
IRMP_NEC42_PROTOCOL,
IRMP_MATSUSHITA_PROTOCOL,
IRMP_DENON_PROTOCOL,
IRMP_RC5_PROTOCOL,
IRMP_RC6_PROTOCOL,
IRMP_RC6A_PROTOCOL,
IRMP_IR60_PROTOCOL,
IRMP_GRUNDIG_PROTOCOL,
IRMP_SIEMENS_PROTOCOL,
IRMP_NOKIA_PROTOCOL,
0
};

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
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
	/* Config PB7 (motherboard switch pin) */
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
void eeprom_store(uint8_t virt_addr, uint8_t *buf)
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

/* put wakeup IRData into eeprom */
void store_new_wakeup(void)
{
	uint8_t idx;
	IRMP_DATA wakeup_IRData;
	toggle_LED();
	systicks = 0;
	/* 5 seconds to press button on remote */
	delay_ms(5000);
	if (irmp_get_data(&wakeup_IRData)) {
		wakeup_IRData.flags = 0;
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS;
		/* store wakeup-code learned by remote in first wakeup slot */
		eeprom_store(idx, (uint8_t *) &wakeup_IRData);
		toggle_LED();
	}
}

void enable_ir_receiver(uint8_t enable)
{
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
GPIO_InitStructure.GPIO_Mode = enable ? GPIO_Mode_IN_FLOATING : GPIO_Mode_AIN;
GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int8_t get_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;

	switch ((enum command) buf[2]) {
	case CMD_CAPS:
		/* in first query we give informaton about slots and depth */
		if (!buf[3]) {
			buf[3] = MACRO_SLOTS;
			buf[4] = MACRO_DEPTH;
			buf[5] = WAKE_SLOTS;
			ret += 3;
			break;
		}
		/* in later queries we give information about supported protocols */
		idx = BYTES_PER_QUERY * (buf[3] - 1);
		if (idx >= sizeof(supported_protocols))
			return -1;
		strncpy((char *) &buf[3], &supported_protocols[idx], BYTES_PER_QUERY);
		/* actually this is not true for the last transmission,
		 * but it doesn't matter since it's NULL terminated
		 */
		ret = HID_IN_BUFFER_SIZE-1; //
		break;
	case CMD_ALARM:
		/* AlarmValue -> buf[3-6] */
		memcpy(&buf[3], &AlarmValue, sizeof(AlarmValue));
		ret += sizeof(AlarmValue);
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[3] + SIZEOF_IR/2 * buf[4];
		eeprom_restore(&buf[3], idx);
		ret += SIZEOF_IR;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[3];
		eeprom_restore(&buf[3], idx);
		ret += SIZEOF_IR;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t set_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	uint8_t tmp[SIZEOF_IR];
	switch ((enum command) buf[2]) {
	case CMD_EMIT:
		/* disable receiving of ir, since we don't want to rx what we txed*/
		enable_ir_receiver(0);
		irsnd_send_data((IRMP_DATA *) &buf[3], 1);
		delay_ms(300);
		/* reenable receiving of ir */
		enable_ir_receiver(1);
		break;
	case CMD_ALARM:
		AlarmValue = *((uint32_t *) &buf[3]);
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[3] + SIZEOF_IR/2 * buf[4];
		eeprom_store(idx, &buf[5]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[5], tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, &buf[4]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[4], tmp, sizeof(tmp)))
			ret = -1;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t reset_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	uint8_t zeros[SIZEOF_IR] = {0};
	switch ((enum command) buf[2]) {
	case CMD_ALARM:
		AlarmValue = 0xFFFFFFFF;
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[3] + SIZEOF_IR/2 * buf[4];
		eeprom_store(idx, zeros);
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, zeros);
		break;
	default:
		ret = -1;
	}
	return ret;
}

/* is received ir-code in one of the wakeup-slots? wakeup if true */
void check_wakeups(IRMP_DATA *ir) {
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	for (i=0; i < WAKE_SLOTS; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		if (!memcmp(buf, ir, sizeof(ir)))
			Wakeup();
	}
}

void transmit_macro(uint8_t macro)
{
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0};
	/* disable receiving of ir, since we don't want to rx what we txed*/
	enable_ir_receiver(0);
	/* we start from 1, since we don't want to tx the trigger code of the macro*/
	for (i=1; i < MACRO_DEPTH + 1; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * macro + SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		/* first encounter of zero in macro means end of macro */
		if (!memcmp(buf, &zeros, sizeof(zeros)))
			break;
		irsnd_send_data((IRMP_DATA *) buf, 1);
		delay_ms(300); // ??
	}
	/* reenable receiving of ir */
	enable_ir_receiver(1);
}

/* is received ir-code (trigger) in one of the macro-slots? transmit_macro if true */
void check_macros(IRMP_DATA *ir) {
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	for (i=0; i < MACRO_SLOTS; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		if (!memcmp(buf, ir, sizeof(ir)))
			transmit_macro(i);
	}
}

int main(void)
{
	uint8_t buf[HID_OUT_BUFFER_SIZE-1], RepeatCounter = 0;
	IRMP_DATA myIRData;
	int8_t ret;

	LED_Switch_init();
	USB_HID_Init();
	IRMP_Init();
	IRSND_Init();
	FLASH_Unlock();
	EE_Init();
	Systick_Init();

	while (1) {
		if (!AlarmValue)
			Wakeup();

		/* wakeup reset pin pulled low? */
		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) {
			/* put wakeup IRData into buf and wakeup eeprom */
			store_new_wakeup();
		}

		/* test if USB is connected to PC and command is received */
		if (USB_HID_GetStatus() == USB_HID_CONNECTED && USB_HID_ReceiveData(buf) == RX_READY && buf[0] == STAT_CMD) {

			switch ((enum access) buf[1]) {
			case ACC_GET:
				ret = get_handler(buf);
				break;
			case ACC_SET:
				ret = set_handler(buf);
				break;
			case ACC_RESET:
				ret = reset_handler(buf);
				break;
			default:
				ret = -1;
			}

			if (ret == -1) {
				buf[0] = STAT_FAILURE;
				ret = 3;
			} else {
				buf[0] = STAT_SUCCESS;
			}

			/* send data */
			USB_HID_SendData(REPORT_ID_CONFIG, buf, ret);
			toggle_LED();
		}

		/* poll IR-data */
		if (irmp_get_data(&myIRData)) {
			if (!(myIRData.flags)) { // & IRMP_FLAG_REPETITION)) { //
				RepeatCounter = 0;
			} else {
				RepeatCounter++;
			}
			if ((RepeatCounter == 0) | ( RepeatCounter >= MIN_REPEATS))
				toggle_LED();

			check_wakeups(&myIRData);

			check_macros(&myIRData);

			/* send IR-data via USB-HID */
			memcpy(buf, &myIRData, sizeof(myIRData));
			USB_HID_SendData(REPORT_ID_IR, buf, sizeof(myIRData));
		}
	}
}
